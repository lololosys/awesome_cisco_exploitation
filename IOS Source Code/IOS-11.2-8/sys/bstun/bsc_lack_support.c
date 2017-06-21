/* $Id: bsc_lack_support.c,v 3.4.6.8 1996/09/10 08:48:16 jbalestr Exp $
 * $Source: /release/112/cvs/Xsys/bstun/bsc_lack_support.c,v $
 *------------------------------------------------------------------
 * BSC local-ack module.
 *
 * February 1995, Andrew R. Rothwell.
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: bsc_lack_support.c,v $
 * Revision 3.4.6.8  1996/09/10  08:48:16  jbalestr
 * CSCdi62395:  bsc/bstun debug for specific groups and addresses
 * Branch: California_branch
 *
 * Revision 3.4.6.7  1996/08/28  12:37:43  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.4.6.6  1996/08/22  18:48:52  jbalestr
 * CSCdi51808:  bsc local-ack sends selects on inactive device
 * Branch: California_branch
 *
 * Revision 3.4.6.5  1996/07/22  18:22:19  snyder
 * CSCdi63677:  fix a pointer problem, and move more text to data
 *              moves 2692 bytes to data, 16 out of image completely
 * Branch: California_branch
 *
 * Revision 3.4.6.4  1996/06/17  08:29:53  jhernand
 * Commit Modular Cobra to California_branch
 *
 * Revision 3.4.6.3  1996/06/05  04:12:20  jbalestr
 * CSCdi59405:  bsc local-ack drops carrier during session init
 * Branch: California_branch
 *
 * Revision 3.4.6.2  1996/05/17  10:40:35  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.3.40.5  1996/05/07  21:33:30  jbalestr
 * CSCdi52473:  bsc local-ack fails link for rx frames larger than 888
 * bytes at 2400
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.3.40.4  1996/05/05  22:49:33  ppearce
 * Sync to IbuMod_Calif_baseline_960504
 *
 * Revision 3.3.40.3  1996/04/26  07:32:30  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.3.40.2  1996/04/22  08:57:46  jbalestr
 * CSCdi54541:  system gets stuck in BSC-3-BADLINESTATE
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.3.40.1  1996/03/17  17:35:51  ppearce
 * Sync IbuMod_Calif_branch to V111_1_3
 *
 * Revision 3.4.6.1  1996/04/04  04:30:04  arothwel
 * CSCdi43971:  Prevent timer events occurring on shutdown bsc interface
 * Branch: California_branch
 *
 * Revision 3.4  1996/03/01  18:27:21  jbalestr
 * CSCdi49416:  BSC lack polling does not has enough SYNs
 * Add an extra syn character
 *
 * Revision 3.3  1995/11/21  08:52:19  jbalestr
 * CSCdi42960:  bstun keepalive count
 * Added bstun keepalive interval and count
 *
 * Revision 3.2  1995/11/17  08:48:35  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:03:53  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.8  1995/10/04  06:40:17  jbalestr
 * CSCdi40972:  badminor event opcode not reported for hardware paks
 *
 * Revision 2.7  1995/09/15  02:31:05  jbalestr
 * CSCdi40129:  BSC local-ack: in cucb_delete when the head and tail of the
 * cucb list were deleted the head or tail pointers were being erroneously
 * set.
 *
 * Revision 2.6  1995/09/03  01:04:01  arothwel
 * CSCdi39397:  Fix:
 *
 *         o Add specific poll support for local-ack pollee.
 *         o Remove superfluous fields from lcb.
 *         o Generalize protocol main handler.
 *         o Finish off replacing all deprecated process mgnt calls.
 *         o Multiple poll-adds causing hard-on polling.
 *         o Add safety code to ignore 'spurious' Tx completes.
 *
 * Revision 2.5  1995/08/11  08:21:50  arothwel
 * CSCdi38625:  Fix:
 *              1. Broken timer s/ware stopping Tx/Rx.
 *              2. Incorrect bsc-mib sub-system dependencies.
 *              3. Premature starting of bstun-event-process.
 *              4. Removal of timestamp info from debugging messages.
 *              5. Replace deprecated process mngmt s/ware w/ new s/ware.
 *
 * Revision 2.4  1995/07/26  06:17:45  arothwel
 * CSCdi37680:  Fix BSC BADSHARE problem. Fix BSC FDX cfg prob. Fix BSC
 * dbg/sh cmd.
 *
 * Revision 2.3  1995/06/18  21:20:39  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.2  1995/06/16  06:24:37  arothwel
 * CSCdi35957:  BSTUN/BSC cleanup.
 *
 * Revision 2.1  1995/06/07  20:13:59  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "logger.h"
#include "msg_bsc.c"			/* Not a typo, see logger.h */
#include "packet.h"
#include "bsc_lcb.h"
#include "bsc_output.h"
#include "bsc_cmd_set.h"
#include "bsc_lack_support.h"
#include "bsc_lack_fsm.h"
#include "bsc_conv_table.h"
#include "bstun.h"
#include "bstun_public.h"

#include "sm_exec.h"
#include "protocol_events.h"
#include "bsc_debug.h"

#include "interface_private.h"  /* -> _generic when subblock work done */

/*---- Constants. */

#define ALERT_PAK_SIZE             50

#define ENQ_FRAME_SIZE             10
#define STATUS_FRAME_SIZE          9

#define LCB_DELETE_TIME            1    /* ms. */

/*---- Local storage. */

/* -- State Name Array -- */
char *const pollerfsm_StateNames[] = {
            "TCU_Down",
            "TCU_EOFile",
	    "TCU_Idle",
	    "TCU_InFile",
	    "TCU_Polled",
	    "TCU_Selected",
	    "TCU_SpecPolled",
	    "TCU_TtdDelay",
	    "TCU_TtdSent",
	    "TCU_TxEOFile",
	    "TCU_TxInFile",
	    "TCU_TxRetry",
	    };

/* -- Event Name Array -- */
static char *const pollerfsm_EventNames[] = {
            "E_LineFail",
	    "E_LineUp",
            "E_RxAbort",
	    "E_RxAck",
	    "E_RxBadAck",
	    "E_RxEnq",
	    "E_RxEot",
	    "E_RxEtb",
	    "E_RxEtx",
            "E_RxGiant",
            "E_RxInvalid",
	    "E_RxNak",
	    "E_RxRvi",
	    "E_RxTtd",
	    "E_RxWack",
	    "E_SendData",
	    "E_Timeout",
	    "E_TxComp"
	    };

/* -- State Name Array -- */
char *const polleefsm_StateNames[] = {
            "CU_DevBusy",
	    "CU_Down",
	    "CU_EOFile",
	    "CU_Idle",
	    "CU_InFile",
	    "CU_Selected",
	    "CU_TtdDelay",
	    "CU_TtdSent",
	    "CU_TxEOFile",
	    "CU_TxInFile",
	    "CU_TxRetry",
	    "CU_TxSpecPollData",
            "CU_TxStatus"
	    };

/* -- Event Name Array -- */
static char *const polleefsm_EventNames[] = {
            "E_LineFail",
	    "E_LineUp",
            "E_RxAbort",
	    "E_RxAck",
	    "E_RxBadAck",
	    "E_RxEnq",
	    "E_RxEot",
	    "E_RxEtb",
	    "E_RxEtx",
            "E_RxGiant",
            "E_RxInvalid",
	    "E_RxNak",
	    "E_RxRvi",
	    "E_RxTtd",
	    "E_RxWack",
	    "E_SendData",
	    "E_Timeout",
	    "E_TxComp",
            };


/*---- Local prototype declarations. */

static void
bsc_set_cucb_cfg_retry_counters (cucb_t* cucb, ulong retries);


/*---- External references. */

extern mgd_timer pause_timer_parent;
extern SM polleefsm;
extern SM pollerfsm;


/*----------------------------------------------------------------------------*/
/* bsc_lack_network_handler: Handle events coming from the (bstun) network. */
/*----------------------------------------------------------------------------*/

void
bsc_lack_network_handler (paktype* pak,
                          hwidbtype* idb)
{
   lcb_t* lcb = idb->lcb;
   uchar address, command;
   cucb_t* cucb;
   boolean destroy_pak = TRUE;

   if (is_bsc_debugging_group(idb->bstun_group)) {
      bsc_output_frame("NDI-rx", pak->datagramstart, pak->datagramsize, idb);
   }

   address = GET_CUADDR_FROM_PAK(pak);
   command = GET_CUCMD_FROM_PAK(pak);
   cucb = bsc_cucb_find(lcb, address);
   
   if ((!cucb) && (command != POLL_ADDR_ADD))
       lcb->unknownCUCBAddrRx++;

   switch (command) {

   case POLL_ADDR_ADD:
      /*---- Poller to pollee talking is fine. */
      if (lcb->poll_role == PR_POLLER) {

         /*
          * make sure link up event happened
          * correctly
          */
         if((lcb->state == TCU_Down) && (idb->state==IDBS_UP)) {
            bsc_cstate(idb);
         }
	 if (cucb) {
	    cucb->req_state = CURS_RESTART;
	    lcb->pending_cu_change = TRUE;
	 }
	 else {
	    cucb = bsc_cucb_create(lcb, address);
         }
	 bsc_send_peer_alert(lcb, address, POLL_ADDR_INACTIVE, 0);
      }
      else {
         /*---- Console msg. Pollee <-> pollee bad. */
         errmsg(&msgsym(BADPEERTYPE, BSC), "");
      }
      break;

   case POLL_ADDR_DELETE:
      /*---- Poller to pollee talking is fine. */
      if (lcb->poll_role == PR_POLLER) {
         if (cucb) {
            cucb->req_state = CURS_DELETE;
            lcb->pending_cu_change = TRUE;
         }
      }
      /*---- Pollee to pollee is lunatic though. */
      else {
         /*---- Send message to console. Bad configuration. */
         errmsg(&msgsym(BADPEERTYPE, BSC), "");
      }
      break;

   case POLL_ADDR_ACTIVE:
      if (cucb) {
         cucb->req_state = CURS_ACTIVE;
         lcb->pending_cu_change = TRUE;
      }
      else
         bsc_send_peer_alert(lcb, address, POLL_ADDR_DELETE, 0);
      break;

   case POLL_ADDR_INACTIVE:
      if (cucb) {
         cucb->req_state = CURS_INACTIVE;
         lcb->pending_cu_change = TRUE;
      }
      else         /* Discard frames for non-existant cu. */
         (void) bsc_send_peer_alert(lcb, address, POLL_ADDR_DELETE, 0);
      break;

   case POLL_DATA_ETB:
   case POLL_DATA_ETX:
   case SPLIT_POLL_DATA:
   case POLL_DATA_ABORT:
   case DEVICE_STATUS_DATA:
      if (cucb) {
         if (lcb->flush) {
            destroy_pak = TRUE;
            if ((command = POLL_DATA_ETX) || (command == POLL_DATA_ABORT))
               lcb->flush = FALSE;
	 }
         else {
            destroy_pak = FALSE;
            pak_lock(pak);
            enqueue(&cucb->outboundQ, pak);
            lcb->fsm(lcb, E_SendData);
         }
      }
      break;

   case SELECT_DATA_ETB:
   case SELECT_DATA_ETX:
   case SPLIT_SELECT_DATA:
   case SEL_DATA_ABORT:
      if (lcb->flush) {
         destroy_pak = TRUE;
         if ((command = POLL_DATA_ETX) || (command == POLL_DATA_ABORT))
            lcb->flush = FALSE;
      }
      else {
         destroy_pak = FALSE;
         pak_lock(pak);
         enqueue(&lcb->outboundQ, pak);
         lcb->fsm(lcb, E_SendData);
      }
      break;

   case DEVICE_DO_WACK:
     destroy_pak = TRUE;
     if(cucb){
       cucb->wack_status = WS_DO_WACK;
     }
     else {
       /* delete may have been lost so send again */
       bsc_send_peer_alert(lcb, address, POLL_ADDR_DELETE, 0);
    }
     break;

   case DEVICE_DONT_WACK :
     destroy_pak = TRUE;
     if(cucb){
       cucb->wack_status = WS_DONT_WACK;
     }
     else {
       /* delete may have been lost so send again */
       bsc_send_peer_alert(lcb, address, POLL_ADDR_DELETE, 0);
     }
     break;
   }  /* switch(command). */

   if (destroy_pak)
      datagram_done(pak);
   return;

}  /* bsc_lack_network_handler. */

/****************************************
        Action: bsc_checkCuPollStop
        Purpose:
*****************************************/
int
bsc_checkCuPollStop (lcb_t *lcb)
{   
   cucb_t* nextcucb;
   cucb_t* cucb;

   cucb = lcb->cucb_list.head;
   while (cucb) {
       nextcucb = cucb->next;
       if (cucb->no_poll) {
           /* if the lack FSM is in IDLE state, the active CU
              can be destroyed */
           if ((cucb != lcb->active_cu) 
              || ((cucb == lcb->active_cu) && (lcb->state == CU_Idle)) ) {
               if (bsc_send_peer_alert(lcb, cucb->address, POLL_ADDR_DELETE,
                                  cucb->active_dev_addr)) {
                 bsc_cucb_delete(lcb, cucb);
              }
           }
       }
       cucb = nextcucb;
   }

   /*
    * set the no poll flag to be true for all CU's.
    * if next time checked they are not polled then
    * delete them. Keep trying until all get deleted.
    */
   cucb = lcb->cucb_list.head;
   while (cucb) {
       cucb->no_poll = TRUE;
       cucb = cucb->next;
   }

   return(TRUE);

}  /* bsc_checkCuPollStop. */

/*----------------------------------------------------------------------------*/
/* bsc_lack_timeout_handler: Handle timeout events. */
/*----------------------------------------------------------------------------*/

void
bsc_lack_timer_handler (paktype* pak,
                        hwidbtype* idb)
{
   lcb_t* lcb = idb->lcb;

   if ((lcb->poll_role == PR_POLLEE) && (mgd_timer_expired(&lcb->host_activity_timer))) {
       mgd_timer_stop(&lcb->host_activity_timer);
       bsc_checkCuPollStop(lcb);
       mgd_timer_start(&lcb->host_activity_timer, lcb->host_activity_timeout);
   } else {
       if(idb->rx_in_progress && (*idb->rx_in_progress)(idb)){
           lcb_start_io_timer(lcb, DEFAULT_IO_TIMEOUT);
       }
       else
       /*---- If no io timer to restart, then handle Tx/Rx transitions/etc. */
       if (!lcb_conditional_restart_io_timer(lcb, pak)) {
          /*---- Kick lack fsm */
          lcb->fsm(lcb, E_Timeout);
       }
   }
}  /* bsc_lack_timer_handler. */


/*----------------------------------------------------------------------------*/
/* bsc_lack_hardware_handler: Handle events coming from the (bisync) serial line. */
/*----------------------------------------------------------------------------*/

void
bsc_lack_hardware_handler (paktype* pak,
                           hwidbtype* idb)
{
   int event;
   lcb_t* lcb = idb->lcb;
   uchar hware_event = get_minor_protocol_event(pak);

   if (hware_event < BSC_HW_TX_COMP) {

      /* 
       * the transmit completion has not happened yet
       * so discard any receive frames until then
       */

      if (lcb->line_state == LS_TX) {
         errmsg(&msgsym(BADLINESTATE, BSC), hware_event);
         datagram_done(pak);
         return;
      }

      if (is_bsc_debugging_group(idb->bstun_group)) {
         bsc_output_frame("SDI-rx", pak->datagramstart, pak->datagramsize, idb);
      }
   }

   switch (hware_event) {

   case BSC_HW_RX_ACK0:
      event = (lcb->rx_ack_next == lcb->bsc_cs_table[CS_ACK0]) ?
              E_RxAck : E_RxBadAck;
      break;

   case BSC_HW_RX_ACK1:
      event = (lcb->rx_ack_next == lcb->bsc_cs_table[CS_ACK1]) ?
              E_RxAck : E_RxBadAck;
      break;

   case BSC_HW_RX_ENQ:
   case BSC_HW_RX_PLSL:
      event = E_RxEnq;
      break;

   case BSC_HW_RX_EOT:
   case BSC_HW_RX_HANGUP:
      event = E_RxEot;
      break;

   case BSC_HW_RX_ETB:
      event = E_RxEtb;
      break;

   case BSC_HW_RX_ETX:
      event = E_RxEtx;
      break;

   case BSC_HW_RX_NAK:
      event = E_RxNak;
      break;

   case BSC_HW_RX_RVI:
      event = E_RxRvi;
      break;

   case BSC_HW_RX_TTD:
      event = E_RxTtd;
      break;
      
   case BSC_HW_RX_ABORT:
      event = E_RxAbort;
      break;

   case BSC_HW_RX_WACK:
      event = E_RxWack;
      break;

   case BSC_HW_TX_COMP:
      lcb->line_state = LS_IDLE;
      event = E_TxComp;
      break;

   case BSC_HW_RX_GIANT:
      event = E_RxGiant;
      break;

   case BSC_HW_RX_INVALID:
      event = E_RxInvalid;
      break;

   case BSC_HW_LINKDOWN:
      datagram_done(pak);
      lcb->fsm(lcb, E_LineFail);
      bsc_lack_poll_list_delete_all(lcb);
      bsc_hwif_goingdown(idb);
      return;

   case BSC_HW_LINKUP:
      datagram_done(pak);
      lcb->fsm(lcb, E_LineUp);
      bsc_hwif_comingup(idb);
      return;

   default:
     /*
      * bad op code. Should never get here but if it does ...     
      */
      datagram_done(pak);
      if (is_bsc_debugging_group(idb->bstun_group)) {
         bsc_output_event( idb, BSCevent_BAD_HARD_OPCODE);
      }   
      return;

   }  /* switch(command). */

   if (hware_event != BSC_HW_TX_COMP)
      lcb->rx_pak = pak;                /* Remember frame received. */

   lcb->fsm(lcb, event);                /* Send to fsm for processing. */
   return;

}  /* bsc_lack_hardware_handler. */


/*----------------------------------------------------------------------------*/
/* bsc_poller_fsm_exec: Entrypoint to poller fsm. */
/*----------------------------------------------------------------------------*/

void
bsc_poller_fsm_exec (lcb_t* lcb,        /* Ptr to line control block. */
                     int event)         /* Event to process. */
{
   int old_state, new_state;

   old_state = lcb->state;
   sm_exec(&pollerfsm, &lcb->state, event, lcb);
   new_state = lcb->state;

   /*---- Debug state changes in poller fsm. */
   if (is_bsc_debugging_group(lcb->idb->bstun_group)) {
     bsc_output_state ("POLLER",
                       pollerfsm_EventNames[event],
                       pollerfsm_StateNames[old_state],
                       pollerfsm_StateNames[new_state],
                       lcb->idb);    
   }
   return;

}  /* bsc_poller_fsm_exec. */


/*----------------------------------------------------------------------------*/
/* bsc_pollee_fsm_exec: Entrypoint to pollee fsm. */
/*----------------------------------------------------------------------------*/

void
bsc_pollee_fsm_exec (lcb_t* lcb,          /* Ptr to line control block. */
                     int event)           /* Event to process. */
{
   int old_state, new_state;

   old_state = lcb->state;
   sm_exec(&polleefsm, &lcb->state, event, lcb);
   new_state = lcb->state;

   /*---- Debug state changes in poller fsm. */
   if (is_bsc_debugging_group(lcb->idb->bstun_group)) {
     bsc_output_state ("POLLEE",
                       polleefsm_EventNames[event],
                       polleefsm_StateNames[old_state],
                       polleefsm_StateNames[new_state],
                       lcb->idb);    
   }
   return;

}  /* bsc_pollee_fsm_exec. */


/*----------------------------------------------------------------------------*/
/* bsc_cucb_list_init: Control unit control block list initialization. */
/*----------------------------------------------------------------------------*/

void
bsc_cucb_list_init (cucb_list_t* cucb_list_head)
{

   cucb_list_head->head = NULL;
   cucb_list_head->tail = NULL;
   cucb_list_head->count = 0;

}  /* bsc_cucb_list_init. */


/*----------------------------------------------------------------------------*/
/* bsc_cucb_create: Control unit control block constructor. */
/*----------------------------------------------------------------------------*/

cucb_t*
bsc_cucb_create (lcb_t* lcb,
                 uchar address)
{
   cucb_t* cucb;
   cucb_t* old_cucb_head;

   /* 
    * create cucb iff the poll address maps to a non-zero select address
    */

   uchar sel_address = bsc_map_pol_to_sel(lcb, address);
   if (sel_address == 0)
      return (NULL);

   cucb = malloc_named(sizeof(cucb_t), "poll-protocol-cucb");
   if (cucb) {
      int i;
      cucb->address = address;
      cucb->sel_address = sel_address;
      cucb->state = CU_NONRESPONDING;
      cucb->req_state = CURS_OK;
      cucb->gstatus_pending = 0 ;      
      for(i=0;i<MAX_ADDRS;i++){
        cucb->gpoll_status[i] = BSC_DEVICE_OK;
        cucb->device_status[i] = BSC_DEVICE_OK;
      }
      cucb->wack_status_sent = FALSE;
      cucb->wack_status = WS_UNKNOWN;
      cucb->no_poll = TRUE;
      
      queue_init(&cucb->outboundQ, OUTBOUND_QUEUE_DEPTH);

      bsc_poll_list_init(&cucb->spec_poll_list);
      bsc_poll_list_add(&lcb->non_responding_cu_list, address);

      /*---- Set cfg'd retry counters */
      bsc_set_cucb_cfg_retry_counters(cucb, lcb->poll_retries);
      bsc_reset_cucb_retry_counters(cucb);

      /*---- Thread new cu control block onto front of chain. */
      old_cucb_head = lcb->cucb_list.head;
      if (old_cucb_head != NULL)
         old_cucb_head->prev = cucb;
      cucb->next = old_cucb_head;
      cucb->prev = NULL;
      lcb->cucb_list.head = cucb;
      if (old_cucb_head == NULL)
         lcb->cucb_list.tail = cucb;
      lcb->cucb_list.count++;
   }

   return(cucb);

}  /* bsc_cucb_create. */


/*----------------------------------------------------------------------------*/
/* bsc_cucb_inactivate: Control unit control block set inactive. */
/*----------------------------------------------------------------------------*/

void
bsc_cucb_inactivate (cucb_t* cucb)
{
   int i;
   paktype * pak;
   
   cucb->state = CU_NONRESPONDING;
   cucb->req_state = CURS_OK;
   cucb->gstatus_pending = 0 ;      
   for(i=0;i<MAX_ADDRS;i++){
      cucb->gpoll_status[i] = BSC_DEVICE_OK;
      cucb->device_status[i] = BSC_DEVICE_OK;
   }
   /* free everything off the queue */
   while ((pak = dequeue(&cucb->outboundQ))) {
      pak_unlock(pak);
      datagram_done(pak);
   }

   /* clear the spec_poll_list for this cu */
   bsc_poll_list_init(&cucb->spec_poll_list);

   /*---- reset retry counters */
   bsc_reset_cucb_retry_counters(cucb);

}  /* bsc_cucb_inactivate. */

/*----------------------------------------------------------------------------*/
/* bsc_cucb_delete: Control unit control block destructor. */
/*----------------------------------------------------------------------------*/

void
bsc_cucb_delete (lcb_t* lcb, 
                 cucb_t* cucb)
{
   paktype* pak;

   if (cucb) {
      /*---- Handle potentially non-empty outbound queue. */
      while ((pak = dequeue(&cucb->outboundQ))) {
          pak_unlock(pak);
          datagram_done(pak);
      }

      /*---- Remove cu from poll list. */
      if (cucb->state == CU_ACTIVE)
         bsc_poll_list_delete(&lcb->active_cu_list, cucb->address);
      else
         bsc_poll_list_delete(&lcb->non_responding_cu_list, cucb->address);

      /*---- Handle deletions from middle of list. */
      if (cucb->prev) {
         cucb->prev->next = cucb->next;
         if (cucb->prev->next == NULL)
            lcb->cucb_list.tail = cucb->prev;
      }
      else
         lcb->cucb_list.head = cucb->next;

      if (cucb->next) {
         cucb->next->prev = cucb->prev;
         if (cucb->next->prev == NULL)
            lcb->cucb_list.head = cucb->next;
      }      
      else
         lcb->cucb_list.tail = cucb->prev;

      free(cucb);                       /* Return storage to heap. */

      lcb->cucb_list.count--;
   }
   return;

}  /* bsc_cucb_delete. */


/*----------------------------------------------------------------------------*/
/* bsc_cucb_delete_all: Control unit control block list destructor. */
/*----------------------------------------------------------------------------*/

void
bsc_cucb_delete_all (lcb_t* lcb)
{
   cucb_t* nextcucb;
   cucb_t* cucb;
   uchar command;

   /*---- Walk chain deleting each cu control block in succession. */
   cucb = lcb->cucb_list.head;
   while (cucb) {
       nextcucb = cucb->next;
       if (lcb->poll_role == PR_POLLEE)
          command = POLL_ADDR_DELETE;   /* Tell poller to delete cucb. */
       else
          command = POLL_ADDR_INACTIVE; /* Tell pollee make cucb inactive. */
       bsc_send_peer_alert(lcb, cucb->address, command, 0);
       bsc_cucb_delete(lcb, cucb);
       cucb = nextcucb;
   }
   lcb->active_cu = NULL;
   bsc_cucb_list_init(&lcb->cucb_list);
   return;

}  /* bsc_cucb_delete_all. */


/*----------------------------------------------------------------------------*/
/* bsc_cucb_find: Find a control unit control block in list. */
/*----------------------------------------------------------------------------*/

cucb_t*
bsc_cucb_find (lcb_t* lcb,
               uchar address)
{
   cucb_t* cucb;
   boolean found;

   found = FALSE;
   cucb = lcb->cucb_list.head;
   while (cucb) {             /* Walk chain until found or end. */
       found = (address == cucb->address);
       if (found)
          break;
       cucb = cucb->next;
   }
   return(found ? cucb : NULL);

}  /* bsc_cucb_find. */


/*----------------------------------------------------------------------------*/
/* bsc_cucb_process_poll_updates: Process poll updates; marked in cucbs. */
/*----------------------------------------------------------------------------*/

void
bsc_cucb_process_poll_updates (lcb_t* lcb)
{
   cucb_t* cucb;
   cucb_t* nextcucb;

   cucb = lcb->cucb_list.head;
   while (cucb) {                       /* Walk chain until end. */
       nextcucb = cucb->next;
       switch(cucb->req_state) {
       case CURS_RESTART:
          if (cucb->state == CU_ACTIVE) {
	     cucb->state = CU_NONRESPONDING;
             bsc_poll_list_move(&lcb->active_cu_list, &lcb->non_responding_cu_list,
                            cucb->address);
	  }
          break;
       case CURS_DELETE:
          if (cucb->state == CU_ACTIVE) {
             bsc_poll_list_delete(&lcb->active_cu_list, cucb->address);
	  }
          else {
             bsc_poll_list_delete(&lcb->non_responding_cu_list, cucb->address);

	  }
          bsc_cucb_delete(lcb, cucb);
          break;
       case CURS_INACTIVE:
	  cucb->state = CU_NONRESPONDING;
          bsc_poll_list_move(&lcb->active_cu_list, &lcb->non_responding_cu_list,
                         cucb->address);
          break;
       case CURS_ACTIVE:
	  cucb->state = CU_ACTIVE;
          bsc_poll_list_move(&lcb->non_responding_cu_list, &lcb->active_cu_list,
                         cucb->address);
          break;
       default:                         /* CURS_OK. */
          break;
       }
       cucb = nextcucb;
   }
   lcb->pending_cu_change = FALSE;
   return;

}  /* bsc_cucb_process_poll_updates. */


/*----------------------------------------------------------------------------*/
/* bsc_poll_list_init: Initialize poll-list. */
/*----------------------------------------------------------------------------*/

void
bsc_poll_list_init (list_t* this)
{
   int i;

   this->maximum = 0;
   this->current = 0;
   for (i = 0; i < MAX_ADDRS; i++)
       this->list[i] = 0;
   return;

}  /* bsc_poll_list_init. */


/****************************************
    bsc_lack_poll_list_delete_all
*****************************************/
void
bsc_lack_poll_list_delete_all (lcb_t* lcb)
{
   cucb_t* cucb;
   cucb_t* nextcucb;
   boolean all_deleted = TRUE;

   cucb = lcb->cucb_list.head;
   while (cucb) {
       nextcucb = cucb->next;
       lcb->active_cu = cucb;
       if (lcb->poll_role == PR_POLLER)
          A_cState(CU_NONRESPONDING, lcb);
       else {                           /* Pollee. */
          if (bsc_send_peer_alert(lcb, cucb->address, POLL_ADDR_DELETE,
				  cucb->active_dev_addr))
             bsc_cucb_delete(lcb, cucb);
          else
             all_deleted = FALSE;
       }
       cucb = nextcucb;
   }

   if ((lcb->poll_role == PR_POLLEE) && (all_deleted)) {
      bsc_cucb_list_init(&lcb->cucb_list);
      lcb->active_cu = NULL;
   }
   return;
}  /* bsc_lack_poll_list_delete_all */

/*----------------------------------------------------------------------------*/
/* bsc_poll_list_add: Add new device address to poll-list. */
/*----------------------------------------------------------------------------*/

void
bsc_poll_list_add (list_t* this, uchar address)
{
   int i;
   boolean found = FALSE;

   /*---- Locate address. */
   i = 0;
   while ((i < this->maximum) && (!found)) {
       found = (address == this->list[i]);
       i++;
   }

   /*---- Insert it if not already there. */
   if (!found) {
      this->list[this->maximum] = address;
      this->maximum++;
   }
   return;

}  /* bsc_poll_list_add. */


/*----------------------------------------------------------------------------*/
/* bsc_poll_list_delete: Delete device address from poll-list. */
/*----------------------------------------------------------------------------*/

void
bsc_poll_list_delete (list_t* this, uchar address)
{
   int i = 0;
   int j = 0;
   boolean found = FALSE;

   /*---- Locate address. */
   while ((i < this->maximum) && (!found)) {
       found = (address == this->list[i]);
       i++;
   }

   /*---- Got it. Shuffle all addresses above it back one. */
   if (found) {
      for (j = i - 1; j < (this->maximum - 1); j++) {
          this->list[j] = this->list[j+1];
      }
      this->list[j] = 0;

      if ((i - 1) < this->current)
	 this->current--;
      this->maximum--;
   }

   return;

}  /* bsc_poll_list_delete. */


/*----------------------------------------------------------------------------*/
/* bsc_poll_list_move: Move device address from one poll-list to another. */
/*----------------------------------------------------------------------------*/

void
bsc_poll_list_move (list_t* from, list_t* to, uchar address)
{

   bsc_poll_list_delete(from, address);
   bsc_poll_list_add(to, address);
   return;

}  /* bsc_poll_list_move. */


/*----------------------------------------------------------------------------*/
/* bsc_poll_sched: The poll scheduler. */
/*----------------------------------------------------------------------------*/

/*
    Current, is the index into the poll list. It always points to the next
    entry to poll. The only exception to this is when it is equal to maximum,
    in which case the end of the list has been reached.

    maximum is the no. of elements in the list.

    At start of world, current is 0.

    When an element moves between the lists (or is deleted), then current is
    adjusted down 1, if it was past the element deleted (see bsc_poll_list_delete).

*/

static int
bsc_poll_sched_recycle (lcb_t* lcb , boolean spec_done)
{
   cucb_t *cucb;
   cucb = lcb->active_cu
        = bsc_cucb_find(lcb,lcb->active_cu_list.list[lcb->active_cu_list.current]);

   /* if nothing in spec poll list do general poll */
   if (spec_done || cucb->spec_poll_list.maximum == 0) {
     lcb->spec_poll_done = FALSE;
     lcb->active_cu_list.current++;
     return (swNW_Poll);
   }

   if (cucb->spec_poll_list.current >= (cucb->spec_poll_list.maximum)){
      cucb->spec_poll_list.current = 0;
   }
   cucb->active_dev_addr = cucb->spec_poll_list.list[cucb->spec_poll_list.current++];
   lcb->spec_poll_done = TRUE;
   return(swNW_Spoll);
}

int
bsc_poll_sched (lcb_t* lcb)
{
   if (lcb->active_cu_list.maximum > 0) {
      if (lcb->active_cu_list.current < (lcb->active_cu_list.maximum)) {
         return(bsc_poll_sched_recycle(lcb,lcb->spec_poll_done));
      }

      if (lcb->active_poll_cycle_count < lcb->servlim) {
         if (!mgd_timer_expired(&lcb->pause_timer))
            return (swNW_Sleep);
         mgd_timer_start(&lcb->pause_timer, lcb->pause_timeout);
         lcb->active_poll_cycle_count++;
         lcb->active_cu_list.current = 0;
         return(bsc_poll_sched_recycle(lcb,FALSE));
     }
   }

   if (lcb->non_responding_cu_list.maximum > 0) {
      lcb->active_poll_cycle_count = 0;
      if (lcb->non_responding_cu_list.current >= (lcb->non_responding_cu_list.maximum))
         lcb->non_responding_cu_list.current = 0;
      lcb->active_cu =
        bsc_cucb_find(lcb,lcb->non_responding_cu_list.list[lcb->non_responding_cu_list.current++]);
      return(swNW_Poll);
   }

   if (!mgd_timer_expired(&lcb->pause_timer))
      return (swNW_Sleep);
   mgd_timer_start(&lcb->pause_timer, lcb->pause_timeout);
   lcb->active_poll_cycle_count--;
   lcb->active_cu_list.current = 0;
   return(bsc_poll_sched_recycle(lcb,FALSE));
   
}  /* bsc_poll_sched. */

/*----------------------------------------------------------------------------*/
/* bsc_find_next_poll_work */
/*----------------------------------------------------------------------------*/
int
bsc_find_next_poll_work(lcb_t * lcb)
{  
   int next_work;
   boolean not_found = TRUE;

   while (not_found == TRUE) {
      not_found = FALSE;
      /*- Call poll scheduler to determine issuing of general/specific poll. */
      if (!lcb->cucb_list.head) {
         lcb_start_io_timer(lcb, LINE_CHECK_TIMEOUT);
         next_work = swNW_Sleep;
      }
      else {
         next_work = bsc_poll_sched(lcb);
         switch(next_work) {
         case swNW_Sleep:
           lcb_start_io_timer(lcb,
                          (ulong) mgd_timer_left_sleeping(&lcb->pause_timer));
           break;
         case swNW_Poll:
           bsc_format_enq_frame(lcb, lcb->cs_pak, lcb->active_cu->address,
                              lcb->bsc_cs_table[CS_GPOLL]);
           break;
         case swNW_Spoll:
           bsc_format_enq_frame(lcb, lcb->cs_pak, lcb->active_cu->address,
                                lcb->active_cu->active_dev_addr);
           break;
         }
      }

      if((next_work != swNW_Sleep) && (lcb->active_cu != 0)){
        /*
         * if the route is down and the queue is empty delete the cucb
         */
        bstunaddrroutestate_e route_state = bstun_query_route(&lcb->active_cu->address, lcb->idb);
        if (((route_state == BSTUN_ROUTE_DOWN) || (route_state == BSTUN_ROUTE_ADDR_UNKNOWN))
           &&(lcb->active_cu->outboundQ.count == 0))
        {
           if (lcb->active_cu->state == CU_ACTIVE) {
              bsc_poll_list_delete(&lcb->active_cu_list, lcb->active_cu->address);
	   } else {
              bsc_poll_list_delete(&lcb->non_responding_cu_list, lcb->active_cu->address);
	   }
           bsc_cucb_delete(lcb, lcb->active_cu);
           lcb->active_cu = 0;
           /* search list for next work */
           not_found = TRUE;
        }
      }
   } /* while */
   return(next_work);
}


/*----------------------------------------------------------------------------*/
/* bsc_set_lcb_cfg_retry_counters: Set configured retry counters in lcb. */
/*----------------------------------------------------------------------------*/

void
bsc_set_lcb_cfg_retry_counters (lcb_t* lcb)
{

   lcb->cfg_retry_ct.ctr[lDataRetry] = 5;
   lcb->cfg_retry_ct.ctr[lEnqRetry]  = 5;
   lcb->cfg_retry_ct.ctr[lNakRetry]  = 3;
   lcb->cfg_retry_ct.ctr[lTtdRetry]  = 2;
   lcb->cfg_retry_ct.ctr[lWackRetry] = 2;

}  /* bsc_set_lcb_cfg_retry_counters. */


/*----------------------------------------------------------------------------*/
/* bsc_reset_lcb_retry_counters: Reset runtime retry counters in lcb. */
/*----------------------------------------------------------------------------*/

void
bsc_reset_lcb_retry_counters (lcb_t* lcb)
{
   int i;

   /*---- Reset retry counters in lcb. */
   for (i = 0; i < MAX_LCB_RETRY_CTRS; i++)
       lcb->run_retry_ct.ctr[i] = lcb->cfg_retry_ct.ctr[i];

}  /* bsc_reset_lcb_retry_counters. */


/*----------------------------------------------------------------------------*/
/* bsc_reset_cucb_retry_counters: Reset retry counters in cucb. */
/*----------------------------------------------------------------------------*/

void
bsc_set_cucb_cfg_retry_counters (cucb_t* cucb, ulong poll_retries)
{

   cucb->cfg_retry_ct.ctr[cDataRetry] = 5;
   cucb->cfg_retry_ct.ctr[cPollRetry] = poll_retries;
   cucb->cfg_retry_ct.ctr[cSelRetry] = 3;

}  /* bsc_set_cucb_cfg_retry_counters. */


/*----------------------------------------------------------------------------*/
/* bsc_reset_cucb_retry_counters: Reset retry counters in cucb. */
/*----------------------------------------------------------------------------*/

void
bsc_reset_cucb_retry_counters (cucb_t* cucb)
{
   int i;

   /*---- Reset counters in all cucbs. */
   for (i = 0; i < MAX_CUCB_RETRY_CTRS; i++)
       cucb->run_retry_ct.ctr[i] = cucb->cfg_retry_ct.ctr[i];

   return;

}  /* bsc_reset_cucb_retry_counters. */


/*----------------------------------------------------------------------------*/
/* bsc_timer_table_init: Initialize a timer table. */
/*----------------------------------------------------------------------------*/

void
bsc_timer_table_init (timer_table_t* timer_table)
{

   timer_table->timer[ToLineCheck] = LINE_CHECK_TIMEOUT;
   timer_table->timer[ToLineIdleStart] = LINE_IDLE_START_TIMEOUT;
   timer_table->timer[ToAck] = ACK_TIMEOUT;
   timer_table->timer[ToEnq] = ENQ_TIMEOUT;
   timer_table->timer[ToPoll] = POLL_TIMEOUT;
   timer_table->timer[ToTtdDelay] = TTD_DELAY_TIMEOUT;
   return;

}  /* timer_table_init. */


/*----------------------------------------------------------------------------*/
/* bsc_format_enq_frame: Format ENQ frame. */
/*----------------------------------------------------------------------------*/

void
bsc_format_enq_frame (lcb_t* lcb,
                      paktype* pak,
                      uchar cu_addr,
                      uchar dev_addr)
{
   uchar* data;

   pak->datagramstart = pak->data_area + ENCAPBYTES;
   pak->datagramsize = ENQ_FRAME_SIZE + BSC_HEADER_SIZE;

   data = pak->datagramstart + BSC_HEADER_SIZE;
   data[0] = lcb->bsc_cs_table[CS_EOT];
   data[1] = lcb->bsc_cs_table[CS_PAD];
   data[2] = lcb->bsc_cs_table[CS_SYN];
   data[3] = lcb->bsc_cs_table[CS_SYN];
   data[4] = lcb->bsc_cs_table[CS_SYN];
   data[5] = cu_addr;
   data[6] = cu_addr;
   data[7] = dev_addr;
   data[8] = dev_addr;
   data[9] = lcb->bsc_cs_table[CS_ENQ];

}  /* bsc_format_enq_frame. */


/*----------------------------------------------------------------------------*/
/* bsc_format_device_status  */
/*----------------------------------------------------------------------------*/

void
bsc_format_device_status (lcb_t* lcb,
                          paktype* pak,
                          uchar cu_addr,
                          uchar dev_addr,
                          uint status)
{
   uchar* data;
   ushort pos , crc = 0;

   pak->datagramstart = pak->data_area + ENCAPBYTES;
   pak->datagramsize = STATUS_FRAME_SIZE + BSC_HEADER_SIZE + lcb->bcc_length;

   data = pak->datagramstart + BSC_HEADER_SIZE;
   data[0] = lcb->bsc_cs_table[CS_SOH];
   data[1] = lcb->bsc_cs_table[CS_PERCENT];
   data[2] = lcb->bsc_cs_table[CS_UCR];
   data[3] = lcb->bsc_cs_table[CS_STX];
   data[4] = cu_addr;
   data[5] = dev_addr;
   data[6] = (uchar)(status>>8);
   data[7] = (uchar) status;
   data[8] = lcb->bsc_cs_table[CS_ETX];

   /* calculate bcc characters */
   for( pos=1; pos < STATUS_FRAME_SIZE ; pos++){
      (*lcb->bcc_fn)( data[pos], &crc );
   }
   
   data[9] = (uchar) crc;
   if (lcb->bcc_length == 2) {
      data[10] = (uchar) (crc >> 8);
   } else {
      data[10] = 0xFF;
   }
}  /* bsc_format_device_end_frame. */


/*----------------------------------------------------------------------------*/
/* bsc_send_peer_alert: Send alert to peer. */
/*----------------------------------------------------------------------------*/

boolean
bsc_send_peer_alert (lcb_t* lcb,
                     uchar cu_addr,
                     uchar cu_cmd,
                     uchar cu_devaddr)
{
   boolean alert_sent = FALSE;
   paktype* alert_pak;

   alert_pak = getbuffer(ALERT_PAK_SIZE);
   if (alert_pak) {
      set_if_input(alert_pak, lcb->idb->firstsw);
      alert_pak->datagramsize = 0;
      bsc_send_to_bstun(lcb->idb, alert_pak, cu_addr, cu_cmd, cu_devaddr);
      alert_sent = TRUE;
   }
   return(alert_sent);

}  /* bsc_send_peer_alert. */


/*----------------------------------------------------------------------------*/
/* bsc_flush_queue: Flush outbound queue until ETX or ABORT frame found. */
/*----------------------------------------------------------------------------*/

boolean
bsc_flush_queue (queuetype* obq,
                 uchar xcu_cmd)
{
   paktype* pak;
   uchar cu_cmd;
   boolean flush_more = FALSE;

   pak = dequeue(obq);
   if (pak) {
       cu_cmd = GET_CUCMD_FROM_PAK(pak);
       while (cu_cmd == xcu_cmd) {
           pak_unlock(pak);
           datagram_done(pak);
           pak = dequeue(obq);
           cu_cmd = (pak) ? GET_CUCMD_FROM_PAK(pak) : NULL_CMD;
       } 
       if (pak) {
           pak_unlock(pak);
           datagram_done(pak);
       } else {
           flush_more = TRUE;
       }   
   }   
   return(flush_more);

}  /* bsc_flush_queue. */

/*----------------------------------------------------------------------------*/
/* bsc_flush_all_queue: Flush outbound queue of everything                    */
/*----------------------------------------------------------------------------*/

void
bsc_flush_all_queue (queuetype *obq)
{
   paktype *pak;
 
   if (obq) {
       while ((pak = dequeue(obq))) {
           pak_unlock(pak);
           datagram_done(pak);
       }
   }
   return;
 
}  /* bsc_flush_all_queue */

/*----------------------------------------------------------------------------*/
/* bsc_query_printer_ack: */
/*----------------------------------------------------------------------------*/

boolean
bsc_query_printer_ack (lcb_t* lcb,
                       paktype* pak)
{
   uchar* data;
   boolean printer_ack = FALSE;

   data = pak->datagramstart;
   if ((data[3] == lcb->bsc_cs_table[CS_STX]) &&
       (data[4] == lcb->bsc_cs_table[CS_ESC])) {
      if ((data[5] == 0xf1) || (data[5] == 0xf5))
         printer_ack = (data[6] & 0x08);
   }
   return(printer_ack);

}  /* bsc_query_printer_ack. */


/*----------------------------------------------------------------------------*/
/* bsc_set_next_ack: */
/*----------------------------------------------------------------------------*/

uchar
bsc_set_next_ack (lcb_t* lcb, uchar ack_rcvd)
{
   uchar ack_next;

   ack_next = ack_rcvd;
   if (ack_rcvd == lcb->bsc_cs_table[CS_ACK0])
      ack_next = lcb->bsc_cs_table[CS_ACK1];
   else
      if (ack_rcvd == lcb->bsc_cs_table[CS_ACK1])
         ack_next = lcb->bsc_cs_table[CS_ACK0];
   return(ack_next);

}  /* bsc_set_next_ack. */

/*----------------------------------------------------------------------------*/
/* bsc_get_unit_number: */
/*----------------------------------------------------------------------------*/
uint
bsc_get_unit_number (lcb_t* lcb,
                     uint dev_addr )
{
  uint unit;
   /*
    * get the device unit number in range 0..31 from the data
    * remebering it is different depending on text mode
    */
   if(lcb->char_set == CS_EBCDIC) {
     unit = dev_addr&0x3F;  /* bottom 6 bits */
   }
   else {
     /* map using table */
     unit = ascii_dev_to_unit[dev_addr];
   }
   return(unit);
}

/*----------------------------------------------------------------------------*/
/* bsc_update_poller_device_status: */
/*----------------------------------------------------------------------------*/

void
bsc_update_poller_device_status (lcb_t* lcb,
                                 paktype* pak )
{
   uchar *data;
   uint status, unit ;
   
   queuetype reversed_queue;
   queue_init(&reversed_queue, OUTBOUND_QUEUE_DEPTH);
  
   data = pak->datagramstart;
   
   /* bytes 6 & 7 contain the device status */
   status = ((data[6]<<8) + data[7]) & BSC_DEVICE_ALL;

   /* get device unit number from 5th data byte */
   unit = bsc_get_unit_number(lcb, data[5]);

   /* if device is busy then leave it alone */
   if((status & BSC_DEVICE_BUSY_BIT)&& !(status & BSC_DEVICE_END_BIT)){
     bsc_poll_list_add(&lcb->active_cu->spec_poll_list,data[5]);
     lcb->active_cu->device_status[unit] = BSC_DEVICE_BUSY_BIT;
     return;
   }

   /* clear device end and busy bits and take off spec poll list */
   status &= ~(BSC_DEVICE_END_BIT | BSC_DEVICE_BUSY_BIT);
   lcb->active_cu->device_status[unit] = status;
   bsc_poll_list_delete(&lcb->active_cu->spec_poll_list,data[5]);
   
   /*
    * move elements of cucb queue back to lcb outbound.
    * The pak primitives do not allow queue shuffles
    * so do a double reverse to get things in the right order
    */
   while((pak=dequeue(&lcb->active_cu->outboundQ))!= 0){
     requeue(&reversed_queue, pak);
   }
   while((pak=dequeue(&reversed_queue))!= 0){
     requeue(&lcb->outboundQ, pak);
   }

}
/*----------------------------------------------------------------------------*/
/* bsc_update_pollee_device_status: */
/*----------------------------------------------------------------------------*/

void
bsc_update_pollee_device_status (lcb_t* lcb,
                                 paktype* pak )
{
   uchar *data;
   uint status, unit;

   /* skip over the bsc header bytes */
   data = pak->datagramstart+BSC_HEADER_SIZE;
   
   /* get device unit number from 5th data byte */
   unit = bsc_get_unit_number(lcb, data[5]);
   
   /* bytes 6 & 7 contain the device status */
   status = ((data[6]<<8) + data[7]);

   /* update the list for receiving spec polls */
   lcb->active_cu->device_status[unit] = status;

   /*
    * now possibly need to update the general poll status'.
    * This is done for any with DE bit set
    * or only DC or only DC&US bits on
    */
   if( (status&BSC_DEVICE_END_BIT)
       || ( (status&0x0004)&&((status&~0x0404)==0) )
     ) {
     lcb->active_cu->gstatus_pending++;
     lcb->active_cu->gpoll_status[unit] = status;
   }
}

/*----------------------------------------------------------------------------*/
/* bsc_device_clear_for_frame: */
/*----------------------------------------------------------------------------*/

sense_status_e
bsc_device_clear_for_frame (lcb_t * lcb , cucb_t* cucb)
{

  sense_status_e dev_state = SS_OTHER;

  if (cucb->state == CU_ACTIVE) {

      uint unit;
      /* get device unit number from activ device */
      unit = bsc_get_unit_number(lcb, cucb->active_dev_addr);
  
      /* test only the currently active device status */
      if(cucb->device_status[unit] == BSC_DEVICE_OK ){
          dev_state = SS_OK;
      } else {
          if(cucb->device_status[unit]&BSC_DEVICE_BUSY_BIT) {
              dev_state = SS_DB;
          }
      }
  }
  return(dev_state);
}


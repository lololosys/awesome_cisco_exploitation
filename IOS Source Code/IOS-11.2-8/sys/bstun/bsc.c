/* $Id: bsc.c,v 3.7.12.7 1996/09/10 08:47:49 jbalestr Exp $
 * $Source: /release/112/cvs/Xsys/bstun/bsc.c,v $
 *------------------------------------------------------------------
 * bsc.c - Initialization/Common routines for BSC
 *
 * August 1994, James Balestriere
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * Prepared by Metaplex.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: bsc.c,v $
 * Revision 3.7.12.7  1996/09/10  08:47:49  jbalestr
 * CSCdi62395:  bsc/bstun debug for specific groups and addresses
 * Branch: California_branch
 *
 * Revision 3.7.12.6  1996/08/05  21:43:42  jbalestr
 * CSCdi60580:  bsc contention suppresses EOTs
 * Branch: California_branch
 *
 * Revision 3.7.12.5  1996/06/17  08:29:35  jhernand
 * Commit Modular Cobra to California_branch
 *
 * Revision 3.7.12.4  1996/06/05  04:12:18  jbalestr
 * CSCdi59405:  bsc local-ack drops carrier during session init
 * Branch: California_branch
 *
 * Revision 3.7.12.3  1996/05/17  10:39:56  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.7.2.4  1996/05/07  21:33:21  jbalestr
 * CSCdi52473:  bsc local-ack fails link for rx frames larger than 888
 * bytes at 2400
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.7.2.3  1996/05/05  22:49:29  ppearce
 * Sync to IbuMod_Calif_baseline_960504
 *
 * Revision 3.7.2.2  1996/04/23  07:42:58  jbalestr
 * CSCdi48385:  BSC fails to check for getbuffer() failure
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.7.2.1  1996/04/22  08:57:29  jbalestr
 * CSCdi54541:  system gets stuck in BSC-3-BADLINESTATE
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.7.12.2  1996/04/09  05:21:53  arothwel
 * CSCdi43971:  Timer events on shutdown bsc interface
 * Branch: California_branch
 * There is a secondary part to this fix: Tell bsc protocol about
 * line transitions.
 *
 * Revision 3.7.12.1  1996/04/04  04:30:01  arothwel
 * CSCdi43971:  Prevent timer events occurring on shutdown bsc interface
 * Branch: California_branch
 *
 * Revision 3.7  1996/02/13  21:27:25  rbatz
 * CSCdi30258:  Fixed the following:
 *                  Duplicate half-duplex commands.
 *                  Could not do half-duplex stun.
 *                  Could not tune half-duplex timers for bstun.
 *
 * Revision 3.6  1995/12/14  07:11:50  jbalestr
 * CSCdi45561:  bad cleanup on shutdown of bad BSC passthru interface
 *
 * Revision 3.5  1995/12/13  03:22:40  jbalestr
 * CSCdi45429:  bsc lack poller stops transmiting after shutdown/ no shut
 *
 * Revision 3.4  1995/12/04  02:34:45  jbalestr
 * CSCdi41360:  BSC local-ack, slow response to poll.
 *
 * Revision 3.3  1995/11/21  05:43:52  jbalestr
 * CSCdi42577:  Router crashes on no bstun peer-name command
 *
 * Revision 3.2  1995/11/17  08:48:00  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:03:17  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.12  1995/10/03  07:19:29  jbalestr
 * CSCdi41421:  crash during shutdown of unconfigured bstun interface
 *
 * Revision 2.11  1995/09/03  01:03:50  arothwel
 * CSCdi39397:  Fix:
 *
 *         o Add specific poll support for local-ack pollee.
 *         o Remove superfluous fields from lcb.
 *         o Generalize protocol main handler.
 *         o Finish off replacing all deprecated process mgnt calls.
 *         o Multiple poll-adds causing hard-on polling.
 *         o Add safety code to ignore 'spurious' Tx completes.
 *
 * Revision 2.10  1995/08/11  08:21:44  arothwel
 * CSCdi38625:  Fix:
 *              1. Broken timer s/ware stopping Tx/Rx.
 *              2. Incorrect bsc-mib sub-system dependencies.
 *              3. Premature starting of bstun-event-process.
 *              4. Removal of timestamp info from debugging messages.
 *              5. Replace deprecated process mngmt s/ware w/ new s/ware.
 *
 * Revision 2.9  1995/07/31  08:49:17  arothwel
 * CSCdi37920:  Fix broken BSC/BSTUN MIB s/ware. Fix platform make for
 * same.
 *
 * Revision 2.8  1995/07/26  06:17:39  arothwel
 * CSCdi37680:  Fix BSC BADSHARE problem. Fix BSC FDX cfg prob. Fix BSC
 * dbg/sh cmd.
 *
 * Revision 2.7  1995/06/28  09:22:37  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.6  1995/06/21  02:51:10  gstovall
 * CSCdi35731:  Cleanup unused cruft in idb
 * Nuke some unused vectors and variables from the hwidb.
 *
 * Revision 2.5  1995/06/19  07:37:58  arothwel
 * CSCdi36021:  BSC uses mgd_timer_delink inappropriately
 *
 * Revision 2.4  1995/06/18  21:20:36  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.3  1995/06/17  00:50:30  dkatz
 * CSCdi36021:  BSC uses mgd_timer_delink inappropriately
 * Just use mgd_timer_stop.
 *
 * Revision 2.2  1995/06/16  06:24:18  arothwel
 * CSCdi35957:  BSTUN/BSC cleanup.
 *
 * Revision 2.1  1995/06/07  20:13:23  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*---- Include files. */

#include "master.h"
#include "interface_private.h"
#include "packet.h"
#include "sys_registry.h"
#include "subsys.h" 

#include "bsc.h"
#include "bsc_lcb.h"
#include "protocol_events.h"
#include "bsc_lack_fsm.h"
#include "bsc_lack_support.h"
#include "bsc_rbbfsm.h"
#include "bsc_conf.h"
#include "bsc_conv_table.h"

#include "bstun.h"
#include "bstun_bsc.h"
#include "bstun_public.h"
#include "bstun_externs.h"

/*---- Local constants. */

#define CS_PAK_SIZE                50   /* bytes. */
#define TIMER_PAK_SIZE             50

/*---- Global data structures. */

mgd_timer pause_timer_parent;
 
/*---- Prototype definitions: global and local. */

extern void
bsc_debug_init(void);

static void
bsc_datagram_out(paktype* pak);


/*----------------------------------------------------------------------------*/
/* bsc_hwif_goingdown: Bring BSC down on this line. */
/*----------------------------------------------------------------------------*/

void
bsc_hwif_goingdown (hwidbtype* idb)
{
   lcb_t* lcb = idb->lcb;
   leveltype level;
   paktype* pak;
   cucb_t* cucb;

   if(idb->poll_protocol_flags & PP_ACTIVE) {

      /*---- Reset receiver hardware. */
      level = raise_interrupt_level(NETS_DISABLE);
      (*idb->restart_rx)(idb);          /* Flush receiver queue. */
      (*idb->disable_rx)(idb);          /* Disable receiver. */
      reset_interrupt_level(level);

      if(lcb) {
         /*---- Let go of (partially built) Rx frames. */
         if (lcb->frame_builder_pak) {
            datagram_done(lcb->frame_builder_pak);
            lcb->frame_builder_pak = NULL;
         }
         if (lcb->rx_pak) {
            datagram_done(lcb->rx_pak);
            lcb->rx_pak = NULL;
         }

         /*---- Drain potentially non-empty outbound queue(s). */
         while ((pak = dequeue(&lcb->outboundQ))) {
            pak_unlock(pak);
            datagram_done(pak);
         }

         if(bsc_query_local_ack_capable(idb)) {
            lcb_stop_io_timer(idb->lcb);
            if (lcb->poll_role == PR_POLLER) {
                mgd_timer_stop(&idb->lcb->pause_timer);
            } else {
                mgd_timer_stop(&idb->lcb->host_activity_timer);
            }
            cucb = lcb->cucb_list.head;
            while (cucb) {
               while ((pak = dequeue(&cucb->outboundQ))) {
                  pak_unlock(pak);
                  datagram_done(pak);
               }
               cucb = cucb->next;
            }
         }
         lcb_partial_init(lcb);
      }
   }
   return;
}  /* bsc_hwif_goingdown. */


/*----------------------------------------------------------------------------*/
/* bsc_hwif_comingup: Bring BSC up on this line. */
/*----------------------------------------------------------------------------*/

void
bsc_hwif_comingup (hwidbtype* idb)
{
   lcb_t *lcb = idb->lcb;

   /*---- Tell driver to restart. */
   (*idb->restart_rx)(idb);          /* Flush receiver queue. */
   if (lcb && bsc_query_local_ack_capable(idb)){
      lcb_start_io_timer(lcb, LINE_CHECK_TIMEOUT);
      if (lcb->poll_role == PR_POLLER) {
          mgd_timer_start(&lcb->pause_timer, lcb->pause_timeout);
      } else {
          mgd_timer_start(&lcb->host_activity_timer, lcb->host_activity_timeout);
      }
   }
   return;
}  /* bsc_hwif_comingup. */


/*----------------------------------------------------------------------------*/
/* bsc_cstate: Monitor line state changes; and change BSC accordingly. */
/*----------------------------------------------------------------------------*/

void
bsc_cstate (hwidbtype* idb)
{
   if (idb->poll_protocol_flags & PP_ACTIVE) {
      paktype *pak = getbuffer(1); /* no data stored in it */

      if(pak) {
         bstun_protocol_info *gpi = 0;

         set_major_protocol_event(pak, PE_HARDWARE);

         switch (idb->state) {
         case IDBS_UP:
            set_minor_protocol_event(pak, BSC_HW_LINKUP);
            break;
         case IDBS_ADMINDOWN:
         case IDBS_DOWN:
            set_minor_protocol_event(pak, BSC_HW_LINKDOWN);
            break;
         default:
            datagram_done(pak);
            return;
         }

         gpi = bstun_pinfo[idb->bstun_group];

         if(gpi) {

            pak->if_input = idb->firstsw;
            pak->if_output = pak->if_input;

            /*---- dump frame on poll protocol queue. */
            (*gpi->hardware_forward_hook)(idb, pak);
         }
         else {
            datagram_done(pak);
         }
      } /* if pak */
   } /* if pp */
}  /* bsc_cstate. */


/*----------------------------------------------------------------------------*/
/* bsc_init: Initialize BSC at system startup. */
/*----------------------------------------------------------------------------*/

void
bsc_init (subsystype *subsys)
{
   bstun_protocol_info *current;
   char *name;

   /*---- Init debugging flags. */
   bsc_debug_init();
  
   /*---- Register bsc specific interface functions. */
   reg_add_hwif_state_change(bsc_cstate, "bsc_cstate");

   /*---- Init pause timer for local-ack. */
   mgd_timer_init_parent(&pause_timer_parent, NULL);
  
   /*---- Register BSC (pass-through and local-ack) with BSTUN. */
   current = bstun_bsc_predefined_init(&name);
   bstun_add_protocol(current, name);
   current = bstun_bsc_local_ack_predefined_init(&name);
   bstun_add_protocol(current, name);
   return;

}  /* bsc_init. */


/*----------------------------------------------------------------------------*/
/* bsc_query_local_ack_capable: Is this BSC protocol local ack capable? */
/*----------------------------------------------------------------------------*/

boolean
bsc_query_local_ack_capable (hwidbtype* idb)
{

   if (bstun_pinfo[idb->bstun_group])
      return(bstun_pinfo[idb->bstun_group]->local_ack_capable);
   else
      return(FALSE);

}  /* bsc_query_local_ack_capable. */


/*----------------------------------------------------------------------------*/
/* bsc_send_to_bstun: Prepend BSC header to frame, & pass to BSTUN. */
/*----------------------------------------------------------------------------*/

void
bsc_send_to_bstun (hwidbtype* idb,
		   paktype *pak,
                   uchar cu_addr,
                   uchar cu_cmd,
                   uchar dev_addr)
{
 
   /*---- Complete BSC framing before passing to BSTUN. */
   pak->datagramstart -= BSC_HEADER_SIZE;
   pak->datagramsize += BSC_HEADER_SIZE;
   *(pak->datagramstart+2) = dev_addr;  /* Device address. */
   *(pak->datagramstart+1) = cu_cmd;    /* Command. */
   *pak->datagramstart = cu_addr;       /* CU address. */

   /*---- Pass frame to BSTUN. */
   bstun_to_network(idb, pak, BSTUN_OP_PASS);

   return;

}  /* bsc_send_to_bstun. */


/*----------------------------------------------------------------------------*/
/* bsc_send_to_line: Strip BSC header, append SYNs & send to line. */
/*----------------------------------------------------------------------------*/

void
bsc_send_to_line (hwidbtype * idb,
		 paktype* pak)
{
   int i;

   /*---- Append a PAD to the end of the frame. */
   pak->datagramstart[pak->datagramsize] = 0xFF;
  
   /*---- Prepend BSC frame w/ SYN chars. */
   pak->datagramstart += BSC_HEADER_SIZE;
   for (i=0; i < BSC_SYN_PREFIX_SIZE; i++) {
       *--pak->datagramstart = idb->syn_char;
   }
   pak->datagramsize += (BSC_SYN_PREFIX_SIZE - BSC_HEADER_SIZE + 1);
    
   /*---- Pass frame to line driver. */
   pak->if_output->hwptr = idb;
   bsc_datagram_out(pak);
   return;
   
}  /* bsc_send_to_line. */


/*----------------------------------------------------------------------------*/
/* bsc_txComp_Notify: Handle transmit completion. */
/*----------------------------------------------------------------------------*/

void
bsc_txComp_Notify (hwidbtype* idb)
{
    bstun_protocol_info *gpi;
    paktype* pak = idb->lcb->tx_pak;

    /*---- Clear fifo and restart the receiver. */
    (*idb->restart_rx)(idb);

    if (pak) {  /* Note: We ignore 'spurious' interrupts. */
       /*---- The frame is done with, we'll re-use it for Tx completion. */
       set_major_protocol_event(pak, PE_HARDWARE);
       set_minor_protocol_event(pak, BSC_HW_TX_COMP);

       /*---- Fool the BSTUN serialiser that this frame's input interface is
              the one on which BSTUN/BSC is configured. */
       clear_if_input(pak);
       pak->if_input = pak->if_output;

       /*---- Process frame off interrupt tail. Just dump frame on poll
              protocol queue, and return immediately. */
       gpi = bstun_pinfo[idb->bstun_group];
       (*gpi->hardware_forward_hook)(idb, pak);
    }
    return;

}  /* bsc_txComp_Notify. */


/*----------------------------------------------------------------------------*/
/* bsc_datagram_out: Pass packet to line driver for transmission. */
/*----------------------------------------------------------------------------*/

static void
bsc_datagram_out (paktype* pak)
{
   hwidbtype* idb = pak->if_output->hwptr;

   /* if we are doing ASCII and the interface is the HD64570
      interface then we must correct the parity of the data stream.

      The assumption is other hardware will correct parity on transmit
      and parity has been stripped on receive.

      This means iterating thru the entire data frame :-( 
      In transparent mode parity should not be enforced */

   if ((idb->poll_protocol_flags & PP_ASCII) 
    && (idb->type == IDBTYPE_HD64570)) {  
       int transparent = 0;
       uchar *data, *end;
       
       data = pak->datagramstart;
       end  = pak->datagramstart + pak->datagramsize;

       /* the following assumes the frame has been correctly
          built in the receiver of the far end */

       while (data < end) {
           if (*data == 0x10) {
               data++;
               /* found a magic DLE character, so read next one */
               switch (*data) {
               case 0x02 /* STX */ :
                   transparent = 1;
                   break;
               case 0x17 /* ETB */ :
               case 0x03 /* ETX */ :
               case 0x1F /* ITB */ :
                   /* move to end of BCC which must be two bytes*/
                   data += 2;
                   transparent = 0;
                   break;
               default:
                   if (transparent == 0) {
                       if (data < end) {
                           *data = bsc_odd_parity_table[*data];
                       }
                   }
               }
           } else if (transparent == 0) {
               *data = bsc_odd_parity_table[*data];
           }
           data++;
       }
   }

   pak_lock(pak);                       /* Hold onto pak. */
   (*idb->oqueue)(idb, pak, HEAD);      /* Put pak on head of output queue. */
   (*idb->soutput)(idb);                /* Call line driver. */
   return;

}  /* bsc_datagram_out. */

/*----------------------------------------------------------------------------*/
/* lcb_create: Poll protocol Line Control Block constructor. */
/*----------------------------------------------------------------------------*/

lcb_t*
lcb_create (hwidbtype* idb,
            mgd_timer* io_timer_parent)
{
   lcb_t* lcb;

   lcb = malloc_named(sizeof(lcb_t), "pp-lcb");
   if (lcb) {
      lcb->idb = idb;                   /* Must be first. */
      lcb_partial_init(lcb);

      BscKeyCharSet(lcb, CS_EBCDIC);

      lcb->flush = FALSE;
      lcb->active_cu = NULL;
      lcb->next_io_timeout = 0;
      lcb->pause_timeout = POLLER_PAUSE_TIMEOUT;
      lcb->host_activity_timeout = DEFAULT_HOST_ACTIVITY_TIMEOUT;
      lcb->servlim = DEFAULT_SERVLIM;
      lcb->poll_retries = DEFAULT_POLL_RETRIES;
      lcb->active_poll_cycle_count = 0;
      lcb->spec_poll_done = TRUE;

      lcb->cs_pak = getbuffer(CS_PAK_SIZE);
      if( !lcb->cs_pak){
         free(lcb);
         return(NULL);
      }
      else {
         pak_lock(lcb->cs_pak);
         lcb->cs_pak->if_input = idb->firstsw;
         lcb->cs_pak->if_output = lcb->cs_pak->if_input;
      }

      lcb->timer_pak = getbuffer(TIMER_PAK_SIZE);
      if( !lcb->timer_pak) {
         pak_unlock(lcb->cs_pak);
         datagram_done(lcb->cs_pak);
         free(lcb);
         return(NULL);
      }
      else {
         lcb->timer_pak->if_input = idb->firstsw;
         set_major_protocol_event(lcb->timer_pak, PE_TIMER);
      }

      lcb->host_timer_pak = getbuffer(TIMER_PAK_SIZE);
      if( !lcb->host_timer_pak) {
         pak_unlock(lcb->cs_pak);
         datagram_done(lcb->cs_pak);
         datagram_done(lcb->timer_pak);
         free(lcb);
         return(NULL);
      }
      else {
         lcb->host_timer_pak->if_input = idb->firstsw;
         set_major_protocol_event(lcb->host_timer_pak, PE_TIMER);
      }

      bsc_cucb_list_init(&lcb->cucb_list);
      bsc_timer_table_init(&lcb->io_timer_table);

      bsc_set_lcb_cfg_retry_counters(lcb);
      bsc_reset_lcb_retry_counters(lcb);
      queue_init(&lcb->outboundQ, OUTBOUND_QUEUE_DEPTH);
      mgd_timer_init_leaf(&lcb->io_timer, io_timer_parent, LCB_IO_TIMER,
                        lcb->timer_pak, TRUE);

      mgd_timer_init_leaf(&lcb->host_activity_timer, io_timer_parent, 
                          LCB_HOST_ACTIVITY_TIMER, lcb->host_timer_pak, TRUE);

      mgd_timer_init_leaf(&lcb->pause_timer, &pause_timer_parent,
                          LCB_PAUSE_TIMER, lcb->timer_pak, FALSE);

      bsc_poll_list_init(&lcb->active_cu_list);
      bsc_poll_list_init(&lcb->non_responding_cu_list);
   }
   return(lcb);

}  /* lcb_create. */


/*----------------------------------------------------------------------------*/
/* lcb_delete: Poll protocol line control block destructor. */
/*----------------------------------------------------------------------------*/

void
lcb_delete (lcb_t* lcb)
{
   paktype* pak;

   if (lcb) {

      /*---- Stop timers. */
      mgd_timer_stop(&lcb->pause_timer);
      mgd_timer_stop(&lcb->host_activity_timer);
      lcb_stop_io_timer(lcb);

      /*---- Rx'ing something? Return pak to system. */
      if (lcb->frame_builder_pak)
         datagram_done(lcb->frame_builder_pak);

      /*---- Tx'ing something? Return pak to system. */
      if ((lcb->tx_pak) && (lcb->tx_pak != lcb->cs_pak))
         datagram_done(lcb->tx_pak);

      /*---- Delete all cu control blocks on this lcb. */
      bsc_cucb_delete_all(lcb);

      /*---- Handle potentially non-empty outbound queue. */
      while ((pak = dequeue(&lcb->outboundQ))) {
          pak_unlock(pak);
          datagram_done(pak);
      }

      /*---- Return cs_pak/timer_pak to buffer manager. */
      pak_unlock(lcb->cs_pak);
      datagram_done(lcb->cs_pak);
      if (lcb->timer_pak)
         datagram_done(lcb->timer_pak);
      if (lcb->host_timer_pak)
         datagram_done(lcb->host_timer_pak);

      /*---- Return storage to heap. */
      free(lcb);
   }
   return;

}  /* lcb_delete. */


/*----------------------------------------------------------------------------*/
/* lcb_reset: Reset specific fields in the lcb to default values. */
/*----------------------------------------------------------------------------*/

void
lcb_partial_init (lcb_t* lcb)
{

   lcb->rx_pak = NULL;
   lcb->tx_pak = NULL;
   lcb->frame_builder_pak = NULL;
   lcb->frame_seq_state = 0;
   lcb->rx_state = 0;
   lcb->ns = 0;
   lcb->frame_type_rcvd = 0;
   lcb->line_state = LS_IDLE;
   lcb->hdx_state = 0;
   return;

}  /* lcb_reset. */


/*----------------------------------------------------------------------------*/
/* pp_idb_init: Initialize poll protocol related fields in idb. */
/*----------------------------------------------------------------------------*/

void
pp_idb_init (hwidbtype* idb)
{
   idb->poll_protocol_flags = PP_ROLE_UNDEFINED;
   idb->syn_char = 0x0;
   idb->tx_comp_notify = bsc_txComp_Notify;
   idb->rx_char_int = bsc_rxChar_int;
   idb->rx_framer_int = bsc_rxFramer_int;
   return;

}  /* pp_idb_init. */


/*----------------------------------------------------------------------------*/
/* lcb_start_io_timer: Start the IO timer. */
/*----------------------------------------------------------------------------*/

void
lcb_start_io_timer (lcb_t* lcb,
                    ulong timeout_val)
{

   lcb_stop_io_timer(lcb);              /* Halt existing timer. */
   if (lcb->timer_pak) {                /* Timer available (not in use)? */
      lcb->timer_pak = NULL;
      lcb->next_io_timeout = 0;
      mgd_timer_start(&lcb->io_timer, timeout_val);
   }
   else                                 /* No. (In use). */
      lcb->next_io_timeout = timeout_val;
   return;

}  /* lcb_start_io_timer. */


/*----------------------------------------------------------------------------*/
/* lcb_conditional_restart_io_timer: Conditionally restart the IO timer. */
/*----------------------------------------------------------------------------*/

boolean
lcb_conditional_restart_io_timer (lcb_t* lcb,
				  paktype* pak)
{
   ulong xtimeout;
   boolean restart;

   mgd_timer_stop(&lcb->io_timer);
   lcb->timer_pak = pak;
   restart = (lcb->next_io_timeout != 0);
   if (restart) {
      xtimeout = lcb->next_io_timeout;
      lcb->next_io_timeout = 0;
      lcb_start_io_timer(lcb, xtimeout);
   }
   return(restart);

}  /* lcb_conditional_restart_io_timer. */


/*----------------------------------------------------------------------------*/
/* lcb_stop_io_timer: Stop the IO timer. */
/*----------------------------------------------------------------------------*/

void
lcb_stop_io_timer (lcb_t* lcb)
{

   if (!lcb->timer_pak) {
      if (mgd_timer_running(&lcb->io_timer)) {
         mgd_timer_stop(&lcb->io_timer);
         lcb->timer_pak = mgd_timer_context(&lcb->io_timer);
      }
   }
   return;

}  /* lcb_stop_io_timer. */


/*----------------------------------------------------------------------------*/
/* BSC subsystem header. */
/*----------------------------------------------------------------------------*/

#define BSC_MAJVERSION 1
#define BSC_MINVERSION 0
#define BSC_EDITVERSION  1

SUBSYS_HEADER(bsc, BSC_MAJVERSION, BSC_MINVERSION, BSC_EDITVERSION,
	      bsc_init, SUBSYS_CLASS_PROTOCOL,
	      "seq: bstun",
	      "req: bstun");

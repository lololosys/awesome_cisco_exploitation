/* $Id: bsc_general.c,v 3.5.30.4 1996/09/10 08:48:08 jbalestr Exp $
 * $Source: /release/112/cvs/Xsys/bstun/bsc_general.c,v $
 *------------------------------------------------------------------
 * bsc_general.c - Now a misnomer. Contains BSC pass-through only code.
 *
 * August 1994, James Balestriere
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * Prepared by Metaplex.
 * All rights reserved.
 *
 * Contains BSC pass-through only code.
 *
 *------------------------------------------------------------------
 * $Log: bsc_general.c,v $
 * Revision 3.5.30.4  1996/09/10  08:48:08  jbalestr
 * CSCdi62395:  bsc/bstun debug for specific groups and addresses
 * Branch: California_branch
 *
 * Revision 3.5.30.3  1996/08/05  21:43:52  jbalestr
 * CSCdi60580:  bsc contention suppresses EOTs
 * Branch: California_branch
 *
 * Revision 3.5.30.2  1996/06/20  01:43:32  jbalestr
 * CSCdi52599:  bsc does not update interface error counters
 * Branch: California_branch
 *
 * Revision 3.5.30.1  1996/05/17  10:40:20  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.5.18.2  1996/05/07  21:33:27  jbalestr
 * CSCdi52473:  bsc local-ack fails link for rx frames larger than 888
 * bytes at 2400
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.5.18.1  1996/04/22  08:57:36  jbalestr
 * CSCdi54541:  system gets stuck in BSC-3-BADLINESTATE
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.5  1996/01/11  18:26:49  jbalestr
 * CSCdi46666:  BSC multidrop, controller never gets deselected
 * Change primary fsm to discard before passing packet as fsm input.
 *
 * Revision 3.4  1995/12/14  07:11:52  jbalestr
 * CSCdi45561:  bad cleanup on shutdown of bad BSC passthru interface
 *
 * Revision 3.3  1995/11/21  06:47:49  jbalestr
 * CSCdi43341:  BSC passthru, all bsc traffic stops while receiving a frame
 *
 * Revision 3.2  1995/11/17  08:48:20  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:03:37  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.5  1995/09/03  01:03:55  arothwel
 * CSCdi39397:  Fix:
 *
 *         o Add specific poll support for local-ack pollee.
 *         o Remove superfluous fields from lcb.
 *         o Generalize protocol main handler.
 *         o Finish off replacing all deprecated process mgnt calls.
 *         o Multiple poll-adds causing hard-on polling.
 *         o Add safety code to ignore 'spurious' Tx completes.
 *
 * Revision 2.4  1995/08/11  08:21:48  arothwel
 * CSCdi38625:  Fix:
 *              1. Broken timer s/ware stopping Tx/Rx.
 *              2. Incorrect bsc-mib sub-system dependencies.
 *              3. Premature starting of bstun-event-process.
 *              4. Removal of timestamp info from debugging messages.
 *              5. Replace deprecated process mngmt s/ware w/ new s/ware.
 *
 * Revision 2.3  1995/07/26  06:17:42  arothwel
 * CSCdi37680:  Fix BSC BADSHARE problem. Fix BSC FDX cfg prob. Fix BSC
 * dbg/sh cmd.
 *
 * Revision 2.2  1995/06/16  06:24:27  arothwel
 * CSCdi35957:  BSTUN/BSC cleanup.
 *
 * Revision 2.1  1995/06/07  20:13:46  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "master.h"
#include "interface_private.h"
#include "packet.h"
#include "logger.h"

#include "bsc.h"
#include "bsc_lcb.h"
#include "bsc_general.h"
#include "bsc_fsfsm.h"
#include "bsc_output.h"
#include "bsc_hdx.h"
#include "bsc_conf.h"
#include "bsc_lack_fsm.h"
#include "protocol_events.h"
#include "bsc_debug.h"
#include "bsc_conv_table.h"

/*----------------------------------------------------------------------------*/
/* bsc_restart_receiver: */
/*----------------------------------------------------------------------------*/

void
bsc_restart_receiver (hwidbtype * idb)
{
  /*
   * stick the receive back in the idle state
   */
  idb->lcb->rx_state = 0;
  if (idb->poll_protocol_flags & PP_ASCII) {
      idb->lcb->char_table = ascii_to_bsc;
  }

  /*
   * clear fifo
   */
  (*idb->restart_rx)(idb);
  return;

}  /* bsc_restart_receiver. */


/*----------------------------------------------------------------------------*/
/* bsc_disable_receiver: */
/*----------------------------------------------------------------------------*/

void
bsc_disable_receiver (hwidbtype * idb)
{
  /*
   * stop the receiver 
   */
  (*idb->disable_rx)(idb);
  return;

}  /* bsc_disable_receiver. */


/*----------------------------------------------------------------------------*/
/* bsc_frame_tx: */
/*----------------------------------------------------------------------------*/

void
bsc_frame_tx (hwidbtype * idb)
{
  /* There should be a frame enqueued.   */

    /*
     * only if HDX is in the idle state can the transmit
     * meaningfully complete
     */
    if(idb->lcb->hdx_state == 0) {
     
      leveltype level;

      level = raise_interrupt_level(NETS_DISABLE);

      if(!(idb->rx_in_progress && (*idb->rx_in_progress)(idb))){

        /*
         * stop the receiver,
         * if a receive comes through now it is up to
         * the end to end protocol to recover so killing
         * future receives should speed the process
         */
        bsc_disable_receiver(idb);

        reset_interrupt_level(level);
       
        idb->lcb->tx_pak = dequeue(&idb->lcb->outboundQ);

        if (is_bsc_debugging_group(idb->bstun_group)) {
          bsc_output_frame("NDI",idb->lcb->tx_pak->datagramstart,
			   idb->lcb->tx_pak->datagramsize, idb);
        }

        switch (*(idb->lcb->tx_pak->datagramstart+1)&CB_MASK) {
        case CB_FRAME :
          idb->lcb->txCompType = bscHDX_TxCompOther;
          pass_to_frame_sequencer(REMOTE_FRAME, idb, TRUE);
          break;
        case CB_ENQ :
          idb->lcb->txCompType = bscHDX_TxCompOther;
          pass_to_frame_sequencer(REMOTE_ENQ, idb, TRUE);
          break;
        case CB_DLEEOT :
          idb->lcb->txCompType = bscHDX_TxCompEOT;
          pass_to_frame_sequencer(REMOTE_HANGUP, idb, TRUE);
          break;
        case CB_EOT :
          idb->lcb->txCompType = bscHDX_TxCompEOT;
          pass_to_frame_sequencer(REMOTE_EOT, idb, TRUE);
          break;
        }
        return;
      }
      else{
        reset_interrupt_level(level);
      }
    }
  return;

}  /* bsc_frame_tx. */


/*----------------------------------------------------------------------------*/
/* bsc_frame_rx: */
/*----------------------------------------------------------------------------*/

static void
bsc_frame_rx (
    hwidbtype *idb,
    FS_frame frame_type,
    paktype * pak
  )
{
  idb->lcb->rx_pak = pak;

  /* update MIB counters */
  idb->lcb->rx_frame_ct.ctr[CtDataFrames]++;
  idb->lcb->rx_frame_ct.ctr[CtBytes] += idb->lcb->rx_pak->datagramsize;
  
  if (is_bsc_debugging_group(idb->bstun_group)) {
    bsc_output_frame("SDI", idb->lcb->rx_pak->datagramstart, 
		       idb->lcb->rx_pak->datagramsize, idb);
  }
  
  /*
   * send signal to HDX fsm
   */
  bsc_enforce_HDX_transmit(bscHDX_RxFrame, idb);
  
  /*
   * pass to frame sequence FSM
   */
  pass_to_frame_sequencer(frame_type, idb, FALSE);
  
  /*
   * either the frame is passed up or discarded
   * SYN hunt is still enabled .
   * Check the transmit queue for frames. If any drive TX.
   */
  if(queryqueuedepth(&idb->lcb->outboundQ)) {
    bsc_frame_tx(idb);
  }
  return;

}  /* bsc_frame_rx. */


/*----------------------------------------------------------------------------*/
/* bsc_pass_timer_handler: Handler for timer events for BSC pass-thru. */
/*----------------------------------------------------------------------------*/

void
bsc_pass_network_handler (paktype* pak,
			  hwidbtype* idb)
{

   /*---- Send the packet to the interface iff interface is up. */
   if (idb->state == IDBS_UP) {
      pak->linktype = LINK_BSTUN;
      pak_lock(pak);
      enqueue(&idb->lcb->outboundQ, pak);
      bsc_frame_tx(idb);
   }
   else {
      datagram_done(pak);
   }

}  /* bsc_pass_network_handler. */


/*----------------------------------------------------------------------------*/
/* bsc_pass_timer_handler: Handler for timer events for BSC pass-thru. */
/*----------------------------------------------------------------------------*/

void
bsc_pass_timer_handler (paktype* pak,
                        hwidbtype* idb)
{
   lcb_t* lcb = idb->lcb;

   if (!lcb_conditional_restart_io_timer(lcb, pak)) {
      if (is_bsc_debugging_group(idb->bstun_group)) {
         bsc_output_event(idb, BSCevent_PENDING_TIMEOUT);
      }
      bsc_enforce_HDX_transmit(bscHDX_Timeout, idb);
   }
   return;

}  /* bsc_pass_timer_handler. */


/*----------------------------------------------------------------------------*/
/* bsc_pass_hardware_handler: Handler for hardware events for BSC pass-thru. */
/*----------------------------------------------------------------------------*/

void
bsc_pass_hardware_handler (paktype* pak,
			   hwidbtype* idb)
{
  uchar opcode = get_minor_protocol_event(pak);

  switch (opcode) {

  case BSC_HW_TX_COMP :

    /*---- Done with frame. Give it back to the system. */
    pak_unlock(pak);
    datagram_done(pak);

    /* there should only one tx frame outstanding at a time */
    idb->lcb->tx_pak = 0;
  
    /* redrive HDX fsm  */
    bsc_enforce_HDX_transmit(idb->lcb->txCompType, idb);

    break;
    
  case BSC_HW_RX_PLSL :
    bsc_frame_rx(idb, LOCAL_PS_ENQ, pak);
    break;
  case BSC_HW_RX_ENQ  :
    bsc_frame_rx(idb, LOCAL_ENQ, pak);
    break;
  case BSC_HW_RX_EOT  :
    bsc_frame_rx(idb, LOCAL_EOT, pak);
    break;
  case BSC_HW_RX_HANGUP  :
    bsc_frame_rx(idb, LOCAL_HANGUP, pak);
    break;
  case BSC_HW_LINKDOWN :
    pass_to_frame_sequencer(LINK_DOWN, idb, FALSE);
    break;
  case BSC_HW_LINKUP :
    pass_to_frame_sequencer(LINK_UP, idb, FALSE);
    break;
  case BSC_HW_RX_ACK0  :
  case BSC_HW_RX_ACK1  :
  case BSC_HW_RX_ETB   :
  case BSC_HW_RX_ETX   :
  case BSC_HW_RX_NAK   : 
  case BSC_HW_RX_RVI   :
  case BSC_HW_RX_TTD   :
  case BSC_HW_RX_WACK  :
  case BSC_HW_RX_ABORT :
  case BSC_HW_RX_GIANT :
  case BSC_HW_RX_INVALID :
    bsc_frame_rx(idb, LOCAL_FRAME, pak);
    break;
  default :
    /*
     * bad op code. Should never get here but if it does ...     
     */
    datagram_done(pak);
    if (is_bsc_debugging_group(idb->bstun_group)) {
      bsc_output_event(idb, BSCevent_BAD_HARD_OPCODE);
    }   
  }     
  return;

}  /* bsc_pass_hardware_handler. */


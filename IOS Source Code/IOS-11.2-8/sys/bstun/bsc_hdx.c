/* $Id: bsc_hdx.c,v 3.5.42.4 1996/09/10 08:48:10 jbalestr Exp $
 * $Source: /release/112/cvs/Xsys/bstun/bsc_hdx.c,v $
 *------------------------------------------------------------------
 * bsc_hdx.c - BSC Half Duplex Enforcement
 *
 * August 1994, James Balestriere
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * Prepared by Metaplex.
 * All rights reserved.
 *
 * Implementation of FSM which forces BSC to work in a half duplex 
 * transmission mode
 * 
 *------------------------------------------------------------------
 * $Log: bsc_hdx.c,v $
 * Revision 3.5.42.4  1996/09/10  08:48:10  jbalestr
 * CSCdi62395:  bsc/bstun debug for specific groups and addresses
 * Branch: California_branch
 *
 * Revision 3.5.42.3  1996/08/05  21:43:55  jbalestr
 * CSCdi60580:  bsc contention suppresses EOTs
 * Branch: California_branch
 *
 * Revision 3.5.42.2  1996/07/22  18:22:14  snyder
 * CSCdi63677:  fix a pointer problem, and move more text to data
 *              moves 2692 bytes to data, 16 out of image completely
 * Branch: California_branch
 *
 * Revision 3.5.42.1  1996/06/17  08:29:47  jhernand
 * Commit Modular Cobra to California_branch
 *
 * Revision 3.5  1995/12/19  05:42:28  jbalestr
 * CSCdi45820:  bsc unlock wrong pak
 *
 * Revision 3.4  1995/12/14  07:11:53  jbalestr
 * CSCdi45561:  bad cleanup on shutdown of bad BSC passthru interface
 *
 * Revision 3.3  1995/11/21  06:47:48  jbalestr
 * CSCdi43341:  BSC passthru, all bsc traffic stops while receiving a frame
 *
 * Revision 3.2  1995/11/17  08:48:22  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:03:41  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/08/11  08:21:49  arothwel
 * CSCdi38625:  Fix:
 *              1. Broken timer s/ware stopping Tx/Rx.
 *              2. Incorrect bsc-mib sub-system dependencies.
 *              3. Premature starting of bstun-event-process.
 *              4. Removal of timestamp info from debugging messages.
 *              5. Replace deprecated process mngmt s/ware w/ new s/ware.
 *
 * Revision 2.3  1995/07/26  06:17:43  arothwel
 * CSCdi37680:  Fix BSC BADSHARE problem. Fix BSC FDX cfg prob. Fix BSC
 * dbg/sh cmd.
 *
 * Revision 2.2  1995/06/16  06:24:29  arothwel
 * CSCdi35957:  BSTUN/BSC cleanup.
 *
 * Revision 2.1  1995/06/07  20:13:49  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "master.h"
#include "interface_private.h"
#include "packet.h"

#include "bsc_lcb.h"
#include "bsc_hdx.h"
#include "bsc_fsm.h"
#include "bsc_output.h"
#include "bsc_general.h"
#include "bsc_lack_support.h"
#include "bsc_lack_fsm.h"
#include "bsc_debug.h"

static void
bsc_hdx_purge (hwidbtype * idb)
{
  /*
   * clear the transmit queue of extra frames
   * keep either the last frame or the last (EOT or DLE EOT)
   */

  paktype *pak = 0, *keptpak = 0;
  uint keptEOT = 0, pakEOT;
  
  while ((pak = dequeue(&idb->lcb->outboundQ)) != NULL) {

    /* 
     * look for any EOT type for these frames 
     * the mask needs only check for one EOT bit to be set
     */
    pakEOT  = *(pak->datagramstart+1) & CB_DLEEOT;

    if (keptEOT && !pakEOT) {
      pak_unlock(pak);
      datagram_done(pak);
    } else {
      if (keptpak) {
         pak_unlock(keptpak);
         datagram_done(keptpak);
      }
      keptpak = pak;
      keptEOT = pakEOT;
    }
  }

  /* if any frame kept requeue it */
  if (keptpak) {
     requeue(&idb->lcb->outboundQ, keptpak);
  }
  
}

static void
bsc_hdx_act_0 (hwidbtype * idb)
{
  /* null action */
}

static void
bsc_hdx_act_1 (hwidbtype * idb)
{
  /*
   * purge the transmit queue
   */
  bsc_hdx_purge(idb);
  
  /*
   * start timer for 1 second
   */
  lcb_start_io_timer(idb->lcb, 1000);

}

static void
bsc_hdx_act_2 (hwidbtype * idb)
{
  /*
   * stop timer
   */
  lcb_stop_io_timer(idb->lcb);

}

static void
bsc_hdx_act_3 (hwidbtype * idb)
{
  /*
   * flush the transmit queue of excess frames
   */
  bsc_hdx_purge(idb);
}

static void
bsc_hdx_act_4 (hwidbtype * idb)
{
  /*
   * count the number of bytes in the frame
   */
  idb->lcb->tx_frame_ct.ctr[CtDataFrames]++;
  idb->lcb->tx_frame_ct.ctr[CtBytes] += idb->lcb->tx_pak->datagramsize;

  /*
   * pass frame to wire
   */
  bsc_send_to_line(idb, idb->lcb->tx_pak);
  
  /*
   * start timer for 10 second
   */
  lcb_start_io_timer(idb->lcb, 10000);

}

static void
bsc_hdx_act_5 (hwidbtype * idb)
{
  /*
   * clear the queue of any excess frames
   */
  bsc_hdx_purge(idb);
  
  /*
   * stop timer
   */
  lcb_stop_io_timer(idb->lcb);
  
  /*
   * if any frame queued , redrive Tx
   */
  if (queryqueuedepth(&idb->lcb->outboundQ)) {
    bsc_frame_tx(idb);
  } else {
    bsc_restart_receiver(idb);
  }
}

static void
bsc_hdx_act_6 (hwidbtype * idb)
{
  /*
   * clear the queue of any excess frames
   */
  bsc_hdx_purge(idb);
  
  /*
   * stop timer
   */
  lcb_stop_io_timer(idb->lcb);
  
  /*
   * if any frame queued , redrive Tx
   */
  if (queryqueuedepth(&idb->lcb->outboundQ)) {
    bsc_frame_tx(idb);
  }
}

static void
bsc_hdx_act_7 (hwidbtype * idb)
{
}

/*
 * Transmit complete enforcement FSM
 */

static const fsm_node hdxEnfFSM[5][3]={
/*                      IDLE-0             PendingTxComp-1      PendResponse-2 */
/* TxFrame   */ { { 1 ,bsc_hdx_act_4 },{ 1 ,bsc_hdx_act_0 },{ 2 ,bsc_hdx_act_3 } },
/* TxCompEOT */ { { 0 ,bsc_hdx_act_0 },{ 0 ,bsc_hdx_act_5 },{ 2 ,bsc_hdx_act_0 } },
/* TxCompOth */ { { 0 ,bsc_hdx_act_0 },{ 2 ,bsc_hdx_act_1 },{ 2 ,bsc_hdx_act_0 } },
/* RxFrame   */ { { 0 ,bsc_hdx_act_2 },{ 1 ,bsc_hdx_act_7 },{ 0 ,bsc_hdx_act_2 } },
/* TimeOut   */ { { 0 ,bsc_hdx_act_0 },{ 0 ,bsc_hdx_act_5 },{ 0 ,bsc_hdx_act_6 } }
};

/*
 * Driving function of the Transmit HDX Enforcement FSM.
 * Use this entry point to get inputs into FSM.
 */

char *const
bsc_map_hdx_state[] = {
    "IDLE",
    "PND_COMP",
    "PND_RCV"
};

static const char *const
map_hdx_input[] = {
    "TX",
    "CmpEOT",
    "CmpOTH",
    "RXV",
    "T/O"
};

void
bsc_enforce_HDX_transmit (
  bsc_hdx_frame_e input ,
  hwidbtype *     idb
  )
{
  fsm_node next_state;
  int old_state = idb->lcb->hdx_state;

  /*
   * move FSM to next state
   */  
  next_state = hdxEnfFSM[input][old_state];
  idb->lcb->hdx_state = next_state.state;
 
  if (is_bsc_debugging_group(idb->bstun_group)) {
    bsc_output_state("HDX",map_hdx_input[input],
                     bsc_map_hdx_state[old_state],
                     bsc_map_hdx_state[next_state.state],
                     idb);
  }

  /*
   * drive the action
   */
  (*next_state.action)(idb);
}


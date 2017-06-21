/* $Id: bsc_fsfsm.c,v 3.4.30.8 1996/09/10 08:48:05 jbalestr Exp $
 * $Source: /release/112/cvs/Xsys/bstun/bsc_fsfsm.c,v $
 *------------------------------------------------------------------
 * bsc_fsfsm.c - BSC Frame Sequencing FSM definition.
 *
 * August 1994, James Balestriere
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * Prepared by Metaplex.
 * All rights reserved.
 *
 * Implementation of FSM to ensure correct frame sequence at the 
 * BSC protocol level.
 * 
 *------------------------------------------------------------------
 * $Log: bsc_fsfsm.c,v $
 * Revision 3.4.30.8  1996/09/10  08:48:05  jbalestr
 * CSCdi62395:  bsc/bstun debug for specific groups and addresses
 * Branch: California_branch
 *
 * Revision 3.4.30.7  1996/09/03  06:45:42  jbalestr
 * CSCdi67813:  bisync passthru discards EOTS as having bad NS
 * Branch: California_branch
 *
 * Revision 3.4.30.6  1996/08/23  23:42:15  jtsao
 * CSCdi52178:  BSC passthru resets NS counts on every poll sequence
 * Branch: California_branch
 *
 * Revision 3.4.30.5  1996/08/05  21:43:49  jbalestr
 * CSCdi60580:  bsc contention suppresses EOTs
 * Branch: California_branch
 *
 * Revision 3.4.30.4  1996/07/19  22:44:47  snyder
 * CSCdi63560:  move fsm tables to text space from data
 *              gets 53,716 bytes of data back for run from flash platforms
 *              gets 68 bytes of image for all
 *
 * Branch: California_branch
 *
 * Revision 3.4.30.3  1996/06/20  01:43:28  jbalestr
 * CSCdi52599:  bsc does not update interface error counters
 * Branch: California_branch
 *
 * Revision 3.4.30.2  1996/06/17  08:29:44  jhernand
 * Commit Modular Cobra to California_branch
 *
 * Revision 3.4.30.1  1996/05/17  10:40:15  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.4.18.1  1996/04/22  08:57:33  jbalestr
 * CSCdi54541:  system gets stuck in BSC-3-BADLINESTATE
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.4  1996/01/11  18:26:46  jbalestr
 * CSCdi46666:  BSC multidrop, controller never gets deselected
 * Change primary fsm to discard before passing packet as fsm input.
 *
 * Revision 3.3  1995/12/14  07:11:51  jbalestr
 * CSCdi45561:  bad cleanup on shutdown of bad BSC passthru interface
 *
 * Revision 3.2  1995/11/17  08:48:15  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:03:33  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/09/03  01:03:54  arothwel
 * CSCdi39397:  Fix:
 *
 *         o Add specific poll support for local-ack pollee.
 *         o Remove superfluous fields from lcb.
 *         o Generalize protocol main handler.
 *         o Finish off replacing all deprecated process mgnt calls.
 *         o Multiple poll-adds causing hard-on polling.
 *         o Add safety code to ignore 'spurious' Tx completes.
 *
 * Revision 2.3  1995/07/26  06:17:40  arothwel
 * CSCdi37680:  Fix BSC BADSHARE problem. Fix BSC FDX cfg prob. Fix BSC
 * dbg/sh cmd.
 *
 * Revision 2.2  1995/06/16  06:24:26  arothwel
 * CSCdi35957:  BSTUN/BSC cleanup.
 *
 * Revision 2.1  1995/06/07  20:13:42  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "master.h"
#include "interface_private.h"
#include "packet.h"
#include "logger.h"

#include "bsc_lcb.h"
#include "bsc_fsm.h"
#include "bsc_fsfsm.h"
#include "bsc_general.h"
#include "bsc_output.h"
#include "bsc_hdx.h"
#include "bsc_conv_table.h"
#include "bsc_debug.h"

/*----------------------------------------------------------------------------*/
/* bsc_discard_tx_pak: Nuke Tx frame. */
/*----------------------------------------------------------------------------*/

static void
bsc_discard_tx_pak (hwidbtype * idb)
{

  if (is_bsc_debugging_group(idb->bstun_group)) {
    bsc_output_frame("Discard NDI", idb->lcb->tx_pak->datagramstart, 
		     idb->lcb->tx_pak->datagramsize, idb);
  }
  
  /* 
   * return tx_pak to the system buffer pools 
   */
  pak_unlock(idb->lcb->tx_pak);
  datagram_done(idb->lcb->tx_pak);
  idb->lcb->tx_pak = 0;
  
  /*
   * restart the receiver
   */
  (*idb->restart_rx)(idb);

}  /* bsc_discard_tx_pak. */


/*----------------------------------------------------------------------------*/
/* bsc_discard_rx_pak: Nuke Rx frame. */
/*----------------------------------------------------------------------------*/

static void
bsc_discard_rx_pak (hwidbtype * idb)
{
  if (idb->lcb->rx_pak) {    
    if (is_bsc_debugging_group(idb->bstun_group)) {
      bsc_output_frame("Discard SDI",idb->lcb->rx_pak->datagramstart, 
		       idb->lcb->rx_pak->datagramsize, idb);
    }
    datagram_done(idb->lcb->rx_pak);    
    idb->lcb->rx_pak = 0;
  }

}  /* bsc_discard_rx_pak. */


/*===============================================================================*
 * Primary Frame Sequencing FSM
 *===============================================================================*/

static void
primary_act_0 (hwidbtype *idb) {
  
  /* no op */
}

static void
primary_act_1 (hwidbtype * idb) {

  unsigned char addr = 0;

  addr = bsc_map_sel_to_pol(idb->lcb, idb->lcb->rx_pak->datagramstart[0]);
  
  if (addr == 0){
    /* could not map the address so dump the frame */
    if (is_bsc_debugging_group(idb->bstun_group)) {
      bsc_output_event(idb, BSCevent_ADDR_NO_MAP);
    }
    bsc_discard_rx_pak(idb);
  } else {
    
    /*
     * copy received address to secondary address
     */
    idb->lcb->secondary_address = addr;
  
    /*
     * Increment NS
     */
    idb->lcb->ns = ++idb->lcb->ns&CB_NS;
  
    /*
     * send to enforcer
     */
    bsc_send_to_bstun(idb, idb->lcb->rx_pak, idb->lcb->secondary_address, idb->lcb->ns|CB_ENQ, 0);
    idb->lcb->rx_pak = 0;
  }
}

static void
primary_act_2 (hwidbtype *idb) {

  /*
   * send to enforcer
   */
  bsc_send_to_bstun(idb, idb->lcb->rx_pak, idb->lcb->secondary_address, CB_EOT, 0);
  idb->lcb->rx_pak = 0;

}

static void
primary_act_3 (hwidbtype * idb) {

  /*
   * if frame NS !- NS then discard, otherwise send to line
   */
  uint ns = *(idb->lcb->tx_pak->datagramstart + 1)&CB_NS;

  /*
   * for backward compatibility with earlier versions,
   * if NS is zero then forward the frame to the line as well.
    */
  if ((ns != 0) && (ns != idb->lcb->ns)) {
    if (is_bsc_debugging_group(idb->bstun_group)) {
      bsc_output_event(idb, BSCevent_BAD_NS);
    }
    bsc_discard_tx_pak(idb);
  } else {
    /* forward to line  */
    bsc_enforce_HDX_transmit(bscHDX_TxFrame, idb);
    pass_to_frame_sequencer(STATE_RESET, idb, FALSE);
  }

}

static void
primary_act_4 (hwidbtype * idb) {
  /*
   * increment NS
   */
  idb->lcb->ns = ++idb->lcb->ns&CB_NS;
  
  /*
   * send to BSTUN
   */
  bsc_send_to_bstun(idb, idb->lcb->rx_pak, idb->lcb->secondary_address, idb->lcb->ns, 0);
  idb->lcb->rx_pak = 0;

}

static void
primary_act_5 (hwidbtype * idb) {
  
  /*
   * if frame NS !- NS then discard, otherwise send to line
   */
  if((*(idb->lcb->tx_pak->datagramstart + 1)&CB_NS) != idb->lcb->ns) {
    if (is_bsc_debugging_group(idb->bstun_group)) {
      bsc_output_event(idb, BSCevent_BAD_NS);
    }
    bsc_discard_tx_pak(idb);
  } else {
    bsc_enforce_HDX_transmit(bscHDX_TxFrame, idb);
  }
}

static void
primary_act_7 (hwidbtype * idb) {
  /*
   * discard transmit frame
   */
  bsc_discard_tx_pak(idb);
}


static void
primary_act_8 (hwidbtype * idb) {
  /*
   * discard receive frame
   */
  bsc_discard_rx_pak(idb);
}

static const fsm_node primaryFSM[12][2]={
/*                             IDLE- 0               Primary - 1         */
/* LOCAL_PS_ENQ  */ {  { 1, primary_act_1 },   { 1, primary_act_1 }  },
/* LOCAL_ENQ     */ {  { 0, primary_act_8 },   { 1, primary_act_4 }  },
/* REMOTE_ENQ    */ {  { 0, primary_act_7 },   { 1, primary_act_5 }  },
/* LOCAL_EOT     */ {  { 0, primary_act_8 },   { 0, primary_act_2 }  },
/* REMOTE_EOT    */ {  { 0, primary_act_7 },   { 1, primary_act_3 }  },
/* LOCAL_FRAME   */ {  { 0, primary_act_8 },   { 1, primary_act_4 }  },
/* REMOTE_FRAME  */ {  { 0, primary_act_7 },   { 1, primary_act_5 }  },
/* LOCAL HANGUP  */ {  { 0, primary_act_8 },   { 0, primary_act_2 }  },
/* REMOTE HANGUP */ {  { 0, primary_act_7 },   { 1, primary_act_3 }  },
/* LINK_DOWN     */ {  { 0, primary_act_0 },   { 0, primary_act_0 }  },
/* LINK_UP       */ {  { 0, primary_act_0 },   { 0, primary_act_0 }  },
/* STATE_RESET   */ {  { 0, primary_act_0 },   { 0, primary_act_0 }  }
};

/*===============================================================================*
 * Secondary Frame Sequencing FSM
 *===============================================================================*/

static void
secondary_act_0 (hwidbtype * idb) {
  /* no op */
}

static void
secondary_act_1 (hwidbtype * idb) {

  unsigned char * p = idb->lcb->tx_pak->datagramstart;

  /*
   * copy received address
   */
  idb->lcb->secondary_address = *p;

  /*
   * copy received NS
   */
  idb->lcb->ns = *(p+1) & CB_NS ;

  if (is_bsc_debugging_group(idb->bstun_group)) {
    bsc_output_event(idb, BSCevent_NEW_ADDR);
  }

  /*
   * send EOT to line by sticking on the front of data stream
   * use 4 SYN separators to ensure consistency
   * remember to overwrite SDLC address bytes
   */
  p += 3;
  *--p = idb->syn_char;
  *--p = idb->syn_char;
  *--p = idb->syn_char;
  *--p = idb->syn_char;
  *--p = idb->lcb->bsc_cs_table[CS_PAD];
  *--p = idb->lcb->bsc_cs_table[CS_EOT];

  /*
   * send_to_line strips 3 BSTUN bytes from the start of data
   * so make it look like two additional bytes added
   */
  idb->lcb->tx_pak->datagramstart = p-3;
  idb->lcb->tx_pak->datagramsize += 6;
  
  /*
   * send to line
   */
  bsc_enforce_HDX_transmit(bscHDX_TxFrame, idb);
}

static void
secondary_act_2 (hwidbtype * idb) {
  /*
   * forward to line
   */
  bsc_enforce_HDX_transmit(bscHDX_TxFrame, idb);
}

static void
secondary_act_3 (hwidbtype * idb) {
  /*
   * send to enforcer
   */
  bsc_send_to_bstun(idb, idb->lcb->rx_pak, idb->lcb->secondary_address,
                         (CB_EOT | idb->lcb->ns), 0);
  idb->lcb->rx_pak = 0;

}

static void
secondary_act_4 (hwidbtype * idb) {
  /*
   * send to enforcer
   */
  bsc_send_to_bstun(idb, idb->lcb->rx_pak, idb->lcb->secondary_address, idb->lcb->ns, 0);
  idb->lcb->rx_pak = 0;
  
}

static void
secondary_act_5 (hwidbtype * idb) {
  /*
   * copy received address
   */
  idb->lcb->secondary_address = *(char *)idb->lcb->tx_pak->datagramstart;

  /*
   * copy received NS
   */
  idb->lcb->ns = *(char *)(idb->lcb->tx_pak->datagramstart+1) & CB_NS ;

  /*
   * forward to line
   */
  bsc_enforce_HDX_transmit(bscHDX_TxFrame, idb);

}

static void
secondary_act_7 (hwidbtype * idb) {
  /* discard tx */
  bsc_discard_tx_pak(idb);
}

static void
secondary_act_8 (hwidbtype * idb) {
  /* discard receive */
  bsc_discard_rx_pak(idb);
}

static const fsm_node secondaryFSM[11][2] = {
/*                       IDLE - 0                  SECONDARY - 1         */
/* LOCAL_PS_ENQ  */ {  { 0, secondary_act_8 }, { 1, secondary_act_4 }  },
/* LOCAL_ENQ     */ {  { 0, secondary_act_8 }, { 1, secondary_act_4 }  },
/* REMOTE_ENQ    */ {  { 1, secondary_act_1 }, { 1, secondary_act_5 }  },
/* LOCAL_EOT     */ {  { 0, secondary_act_8 }, { 0, secondary_act_3 }  },
/* REMOTE_EOT    */ {  { 0, secondary_act_7 }, { 0, secondary_act_2 }  },
/* LOCAL_FRAME   */ {  { 0, secondary_act_8 }, { 1, secondary_act_4 }  },
/* REMOTE_FRAME  */ {  { 0, secondary_act_7 }, { 1, secondary_act_5 }  },
/* LOCAL_HANGUP  */ {  { 0, secondary_act_8 }, { 0, secondary_act_3 }  },
/* REMOTE_HANGUP */ {  { 0, secondary_act_2 }, { 0, secondary_act_2 }  },
/* LINK_DOWN     */ {  { 0, secondary_act_0 }, { 0, secondary_act_0 }  },
/* LINK_UP       */ {  { 0, secondary_act_0 }, { 0, secondary_act_0 }  }
};

/*===============================================================================*
 * Contention Frame Sequencing FSM
 *===============================================================================*/

static void
ct_act_0 (hwidbtype * idb) {
}

static void
ct_act_1 (hwidbtype * idb) {
  /*
   * set NS to 1
   */
  idb->lcb->ns = 0x01;
  
  /*
   *  send to enforcer
   */
  bsc_send_to_bstun(idb, idb->lcb->rx_pak, 0x01, CB_ENQ|idb->lcb->ns, 0);
  idb->lcb->rx_pak = 0;

}

static void
ct_act_2 (hwidbtype * idb) {
  /*
   * NS = received NS
   */
  idb->lcb->ns = *(idb->lcb->tx_pak->datagramstart + 1)&CB_NS;
  
  /*
   * forward to line 
   */
  bsc_enforce_HDX_transmit(bscHDX_TxFrame, idb);
}

static void
ct_act_3 (hwidbtype * idb) {
  /*
   * send to enforcer
   */
  bsc_send_to_bstun(idb, idb->lcb->rx_pak, 0x01, CB_EOT, 0);
  idb->lcb->rx_pak = 0;


  /*
   * reset NS
   */
  idb->lcb->ns = 0;
}

static void
ct_act_4 (hwidbtype * idb) {
  /*
   * send to line
   */
  bsc_enforce_HDX_transmit(bscHDX_TxFrame, idb);

  /*
   * reset NS
   */
  idb->lcb->ns = 0;
}

static void
ct_act_5 (hwidbtype * idb) {
  /*
   * send to enforcer
   */
  bsc_send_to_bstun(idb,  idb->lcb->rx_pak, 0x01, idb->lcb->ns, 0);
  idb->lcb->rx_pak = 0;

}

static void
ct_act_6 (hwidbtype * idb) {
  /*
   * if frame NS != NS then discard else forward to line
   */
  if((*(idb->lcb->tx_pak->datagramstart + 1)&CB_NS) != idb->lcb->ns){
    if (is_bsc_debugging_group(idb->bstun_group)) {
      bsc_output_event(idb, BSCevent_BAD_NS);
    }
    bsc_discard_tx_pak(idb);
  } else {
    bsc_enforce_HDX_transmit(bscHDX_TxFrame, idb);
  }
}

static void
ct_act_7 (hwidbtype * idb) {
  /*
   * increment NS
   */
  idb->lcb->ns = ++idb->lcb->ns&CB_NS;  
  
  /*
   * send to enforcer
   */
  bsc_send_to_bstun(idb, idb->lcb->rx_pak,  0x01, idb->lcb->ns, 0);
  idb->lcb->rx_pak = 0;

}


static void
ct_act_8 (hwidbtype * idb) {
  /*
   * reset NS
   */
  idb->lcb->ns = 0;

  /*
   * send to enforcer
   */
  bsc_send_to_bstun(idb, idb->lcb->rx_pak, 0x01, idb->lcb->ns|CB_DLEEOT, 0);
  idb->lcb->rx_pak = 0;

}
  
static void
ct_act_11 (hwidbtype * idb) {
  /* discard rx */
  bsc_discard_rx_pak(idb);
}

static void
ct_act_12 (hwidbtype * idb) {
  /* discard tx */
  bsc_discard_tx_pak(idb);
}
  
static const fsm_node contentionFSM[11][5] = {
/*                 IDLE - 0      PENDING - 1    PRIMARY - 2   SECONDARY - 3   PEND-EOT 4 */
/* LOC_PS_ENQ */{{1,ct_act_11}, {1,ct_act_11}, {2,ct_act_11}, {3,ct_act_11}, {1,ct_act_11}},
/* LOCAL_ENQ  */{{1,ct_act_1 }, {1,ct_act_1 }, {2,ct_act_7 }, {3,ct_act_5 }, {1,ct_act_1 }},
/* REMOTE_ENQ */{{1,ct_act_2 }, {1,ct_act_2 }, {2,ct_act_6 }, {3,ct_act_2 }, {1,ct_act_2 }},
/* LOCAL_EOT  */{{0,ct_act_11}, {4,ct_act_3 }, {4,ct_act_3 }, {4,ct_act_3 }, {0,ct_act_3 }},
/* REMOTE_EOT */{{0,ct_act_12}, {4,ct_act_4 }, {4,ct_act_4 }, {4,ct_act_4 }, {0,ct_act_4 }},
/* LOCL_FRAME */{{0,ct_act_11}, {3,ct_act_5 }, {2,ct_act_7 }, {3,ct_act_5 }, {0,ct_act_11}},
/* RMTE_FRAME */{{0,ct_act_12}, {2,ct_act_6 }, {2,ct_act_6 }, {3,ct_act_2 }, {0,ct_act_12}},
/* LOCL_HNGUP */{{0,ct_act_8 }, {0,ct_act_8 }, {0,ct_act_8 }, {0,ct_act_8 }, {0,ct_act_8 }},
/* RMTE_HNGUP */{{0,ct_act_4 }, {0,ct_act_4 }, {0,ct_act_4 }, {0,ct_act_4 }, {0,ct_act_4 }},
/* LINK_DOWN  */{{0,ct_act_0 }, {0,ct_act_0 }, {0,ct_act_0 }, {0,ct_act_0 }, {0,ct_act_0 }},
/* LINK_UP    */{{0,ct_act_0 }, {0,ct_act_0 }, {0,ct_act_0 }, {0,ct_act_0 }, {0,ct_act_0 }}
};

/*===============================================================================*
 *
 *===============================================================================*/

static const char * const
map_fs_input[] = {
  "SDI P/S BID",
  "SDI BID",
  "NDI BID",
  "SDI EOT",
  "NDI EOT",
  "SDI FRAME",
  "NDI FRAME",
  "SDI HANGUP",
  "NDI HANGUP",
  "LINK DOWN",
  "LINK UP",
  "STATE RESET"
};

char *
bsc_map_fs_state(int role, int state)
{
  if (state == 0) {
    return "IDLE";
  } else {
    switch (role) {
    case PR_PRIMARY :
      return "PRI ";
    case PR_SECONDARY :
      return "SEC ";
    case PR_CONTENTION:
      switch (state){
      case 1 :
        return "PEND";
      case 2 :
        return "PRI ";
      case 3 :
        return "SEC ";
      case 4 :
        return "PEND_EOT";
      }
    }
  }
  return "?";
}

void
pass_to_frame_sequencer (
  FS_frame frame_type,
  hwidbtype * idb,
  int transmitting
 )
{
  fsm_node next_state;
  uint old_state = idb->lcb->frame_seq_state;
  
  /*
   * drive the right FSM depending on config mode
   */
  
  switch (idb->lcb->poll_role) {
    case PR_PRIMARY :
      /* check the address to ensure packet is not retarded */
      if (transmitting && (idb->lcb->secondary_address != 
          bsc_map_sel_to_pol(idb->lcb, idb->lcb->tx_pak->datagramstart[0]))) {
          /* discard frame */
          if (is_bsc_debugging_group(idb->bstun_group)) {
             bsc_output_event(idb, BSCevent_BAD_ADDR);
          }
          bsc_discard_tx_pak(idb);
          return;
      } else {
          next_state = primaryFSM[frame_type][old_state];
      }
      break;
    case PR_SECONDARY :
      next_state = secondaryFSM[frame_type][old_state];
      break;
    case PR_CONTENTION :
      next_state = contentionFSM[frame_type][old_state];
      break;
    default :
      /* discard any frame to ensure buffer pool works */
      if(transmitting){
          bsc_discard_tx_pak(idb);
      } else {
          bsc_discard_rx_pak(idb);
      }
      return;
  }

  /*
   * move to the next state
   */
  idb->lcb->frame_seq_state = next_state.state;
  
  if (is_bsc_debugging_group(idb->bstun_group)) {
    bsc_output_state("FS",map_fs_input[frame_type],
                     bsc_map_fs_state(idb->lcb->poll_role, old_state),
                     bsc_map_fs_state(idb->lcb->poll_role, next_state.state),
                     idb);
  }

  /*
   * drive the action
   */
  (*next_state.action)(idb);


}

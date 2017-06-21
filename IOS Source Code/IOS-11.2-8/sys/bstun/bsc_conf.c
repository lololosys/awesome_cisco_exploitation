/* $Id: bsc_conf.c,v 3.4.12.9 1996/09/11 01:11:58 snyder Exp $
 * $Source: /release/112/cvs/Xsys/bstun/bsc_conf.c,v $
 *------------------------------------------------------------------
 * bsc_conf.c - Commands to execute once parse identifies the command
 *
 * August 1994, James Balestriere
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * Prepared by Metaplex.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: bsc_conf.c,v $
 * Revision 3.4.12.9  1996/09/11  01:11:58  snyder
 * CSCdi68742:  vigilante code review of CSCdi62395 results in more consts
 *              24 from data, 20 from image
 *              Can I rent someone's life, please?
 * Branch: California_branch
 *
 * Revision 3.4.12.8  1996/09/10  08:47:57  jbalestr
 * CSCdi62395:  bsc/bstun debug for specific groups and addresses
 * Branch: California_branch
 *
 * Revision 3.4.12.7  1996/08/07  08:56:50  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.4.12.6  1996/08/05  21:43:46  jbalestr
 * CSCdi60580:  bsc contention suppresses EOTs
 * Branch: California_branch
 *
 * Revision 3.4.12.5  1996/07/22  18:22:10  snyder
 * CSCdi63677:  fix a pointer problem, and move more text to data
 *              moves 2692 bytes to data, 16 out of image completely
 * Branch: California_branch
 *
 * Revision 3.4.12.4  1996/06/17  08:29:38  jhernand
 * Commit Modular Cobra to California_branch
 *
 * Revision 3.4.12.3  1996/05/17  10:40:06  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.4.2.4  1996/05/07  21:33:24  jbalestr
 * CSCdi52473:  bsc local-ack fails link for rx frames larger than 888
 * bytes at 2400
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.4.2.3  1996/05/05  22:49:31  ppearce
 * Sync to IbuMod_Calif_baseline_960504
 *
 * Revision 3.4.2.2  1996/04/22  08:57:32  jbalestr
 * CSCdi54541:  system gets stuck in BSC-3-BADLINESTATE
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.4.2.1  1996/04/03  13:49:52  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.4.12.2  1996/04/03  05:38:37  arothwel
 * CSCdi49282:  show bsc command crashes router
 * Branch: California_branch
 *
 * Revision 3.4.12.1  1996/03/18  19:01:07  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/07  08:38:53  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  00:19:40  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.4  1996/02/13  21:27:26  rbatz
 * CSCdi30258:  Fixed the following:
 *                  Duplicate half-duplex commands.
 *                  Could not do half-duplex stun.
 *                  Could not tune half-duplex timers for bstun.
 *
 * Revision 3.3  1996/02/01  06:00:08  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.2  1995/11/17  08:48:06  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:03:23  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.5  1995/09/03  01:03:52  arothwel
 * CSCdi39397:  Fix:
 *
 *         o Add specific poll support for local-ack pollee.
 *         o Remove superfluous fields from lcb.
 *         o Generalize protocol main handler.
 *         o Finish off replacing all deprecated process mgnt calls.
 *         o Multiple poll-adds causing hard-on polling.
 *         o Add safety code to ignore 'spurious' Tx completes.
 *
 * Revision 2.4  1995/08/11  08:21:46  arothwel
 * CSCdi38625:  Fix:
 *              1. Broken timer s/ware stopping Tx/Rx.
 *              2. Incorrect bsc-mib sub-system dependencies.
 *              3. Premature starting of bstun-event-process.
 *              4. Removal of timestamp info from debugging messages.
 *              5. Replace deprecated process mngmt s/ware w/ new s/ware.
 *
 * Revision 2.3  1995/07/26  06:17:40  arothwel
 * CSCdi37680:  Fix BSC BADSHARE problem. Fix BSC FDX cfg prob. Fix BSC
 * dbg/sh cmd.
 *
 * Revision 2.2  1995/06/16  06:24:22  arothwel
 * CSCdi35957:  BSTUN/BSC cleanup.
 *
 * Revision 2.1  1995/06/07  20:13:31  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "master.h"
#include <string.h>
#include "interface_private.h"
#include "packet.h"
#include "logger.h"

#include "bsc.h"
#include "bsc_lcb.h"
#include "bsc_conf.h"
#include "bsc_hdx.h"
#include "bsc_fsfsm.h"
#include "bsc_rbbfsm.h"
#include "bsc_general.h"
#include "bsc_lack_support.h"
#include "bsc_conv_table.h"
#include "parser_defs_bsc.h"
#include "bsc_lack_fsm.h"


/*----------------------------------------------------------------------------*/
/* bsc primary */

void
BscKeyPrimary (hwidbtype* idb,
               boolean local_ack)
{
   if (local_ack) {
      idb->lcb->poll_role = PR_POLLER;
      idb->lcb->state = TCU_Down;
      idb->lcb->fsm = bsc_poller_fsm_exec;
      lcb_start_io_timer(idb->lcb, LINE_CHECK_TIMEOUT);
      idb->lcb->pause_timeout = POLLER_PAUSE_TIMEOUT;
      mgd_timer_start(&idb->lcb->pause_timer, 0);
   } else {
     /*
      * External primary means secondary internally
      */
      idb->lcb->poll_role = PR_SECONDARY;
      idb->lcb->frame_seq_state = 0; /* Initialize state for FS FSM. */
   }
   idb->poll_protocol_flags |= PP_ACTIVE;
}  /* BscKeyPrimary. */


/*----------------------------------------------------------------------------*/
/* no bsc primary */

void
BscNKeyPrimary (hwidbtype* idb)
{

   idb->poll_protocol_flags &= ~PP_ACTIVE;
   idb->lcb->poll_role = PR_UNKNOWN ;
}  /* BscNKeyPrimary. */


/*----------------------------------------------------------------------------*/
/* bsc secondary */

void
BscKeySecondary (hwidbtype* idb,
                 boolean local_ack)
{
   if (local_ack) {
      idb->lcb->poll_role = PR_POLLEE;
      idb->lcb->state = CU_Down;
      idb->lcb->fsm = bsc_pollee_fsm_exec;
      mgd_timer_start(&idb->lcb->host_activity_timer, idb->lcb->host_activity_timeout);
   } else {
     /*
      * external secondary means primary internally
      */
      idb->lcb->poll_role = PR_PRIMARY;
      idb->lcb->frame_seq_state = 0; /* Initialize state for FS FSM. */
   }
   idb->poll_protocol_flags |= PP_ACTIVE;

}  /* BscKeySecondary. */


/*----------------------------------------------------------------------------*/
/* no bsc secondary */

void
BscNKeySecondary (hwidbtype* idb)
{

   idb->poll_protocol_flags &= ~PP_ACTIVE;
   idb->lcb->poll_role = PR_UNKNOWN ;
   idb->lcb->host_activity_timeout = DEFAULT_HOST_ACTIVITY_TIMEOUT;
}  /* BscNKeySecondary. */


/*----------------------------------------------------------------------------*/
/* bsc contention */

void
BscKeyContention (hwidbtype* idb)
{
   idb->poll_protocol_flags |= PP_ACTIVE;
   idb->lcb->poll_role = PR_CONTENTION ;
   idb->lcb->frame_seq_state = 0; /* Initialize state for FS FSM. */

}  /* BscKeyContention. */


/*----------------------------------------------------------------------------*/
/* no bsc contention */

void
BscNKeyContention (hwidbtype* idb)
{
   idb->poll_protocol_flags &= ~PP_ACTIVE;
   idb->lcb->poll_role = PR_UNKNOWN ;

}  /* BscNKeyContention. */


/*----------------------------------------------------------------------------*/
/* bsc char-set [ebcdic | ascii] */

void
BscKeyCharSet (lcb_t* lcb,
               char_set_e char_set)
{
  switch (char_set) {
  case CS_EBCDIC:
    lcb->idb->poll_protocol_flags &= ~PP_ASCII;
    lcb->idb->syn_char = bsc_to_ebcdic[CS_SYN];
    lcb->bsc_cs_table = bsc_to_ebcdic;
    lcb->char_table = ebcdic_to_bsc;
    lcb->char_set = char_set;
    lcb->ps_table = ebcdic_selpol_table;
    lcb->sp_table = ebcdic_polsel_table;
    lcb->bcc_length = 2;
    lcb->bcc_fn = bsc_crc16_rev_update;
    lcb->dev_addr_table = ebcdic_dev_address_table;
    break;
  case CS_ASCII:
    lcb->idb->poll_protocol_flags |= PP_ASCII;
    lcb->idb->syn_char = bsc_to_ascii[CS_SYN];
    lcb->bsc_cs_table = bsc_to_ascii;
    lcb->char_table = ascii_to_bsc;
    lcb->char_set = char_set;
    lcb->ps_table = ascii_selpol_table;
    lcb->sp_table = ascii_polsel_table;
    lcb->bcc_length = 1;
    lcb->bcc_fn = bsc_lrc_update;
    lcb->dev_addr_table = ascii_dev_address_table;   
    break;
  }    
}  /* BscKeyCharSet. */


/*----------------------------------------------------------------------------*/
/*
 * no bsc char-set [ebcdic|ascii]
 * no ASCII means EBCDIC and vice-versa
 */

void
BscNKeyCharSet (lcb_t* lcb,
                char_set_e char_set)
{
  switch (char_set) {
  case CS_EBCDIC:
    BscKeyCharSet(lcb, CS_ASCII);
    break;
  case CS_ASCII:
    BscKeyCharSet(lcb, CS_EBCDIC);
    break;
  }    
}  /* BscNKeyCharSet. */

/*----------------------------------------------------------------------------*/
/* bsc pause <n-tenths-secs> */

void
BscKeyPause (lcb_t* lcb,
             int pause_timeout)
{

   lcb->pause_timeout = (ulong) pause_timeout * 100;  /* Convert to ms. */

}  /* BscKeyPause. */


/*----------------------------------------------------------------------------*/
/* no bsc pause */

void
BscNKeyPause (lcb_t* lcb)
{

    lcb->pause_timeout = POLLER_PAUSE_TIMEOUT;

}  /* BscNKeyPause. */


/*----------------------------------------------------------------------------*/
/* bsc servlim */

void
BscKeyServLim (lcb_t* lcb,
               int servlim)
{

   lcb->servlim = (ulong) servlim;

}  /* BscKeyServLim. */


/*----------------------------------------------------------------------------*/
/* no bsc servlim */

void
BscNKeyServLim (lcb_t* lcb)
{

   lcb->servlim = DEFAULT_SERVLIM;

}  /* BscNKeyServLim. */


/*----------------------------------------------------------------------------*/
/* bsc poll-timeout <n-tenths-secs> */

void
BscKeyPollTimeout (lcb_t* lcb,
                   int poll_timeout)
{

   lcb->io_timer_table.timer[ToPoll] = (ulong) poll_timeout * 100;  /* Cvt to ms. */


}  /* BscKeyPollTimeout. */


/*----------------------------------------------------------------------------*/
/* no bsc poll-timeout */

void
BscNKeyPollTimeout (lcb_t* lcb)
{

   lcb->io_timer_table.timer[ToPoll] = POLL_TIMEOUT;

}  /* BscNKeyPollTimeout. */


/*----------------------------------------------------------------------------*/
/* bsc host-timeout <n-tenths-secs> */

void
BscKeyHostTimeout (lcb_t* lcb, int host_timeout)
{
   lcb->host_activity_timeout = (ulong) host_timeout * 100;

}  /* BscKeyHostTimeout. */


/*----------------------------------------------------------------------------*/
/* no bsc host-timeout */

void
BscNKeyHostTimeout (lcb_t* lcb)
{
   lcb->host_activity_timeout = DEFAULT_HOST_ACTIVITY_TIMEOUT;

}  /* BscNKeyHostTimeout. */

/*----------------------------------------------------------------------------*/
/* bsc retries <n-retries> */

void
BscKeyRetries (lcb_t* lcb,
               int retries)
{

   lcb->poll_retries = retries;

}  /* BscKeyRetries. */


/*----------------------------------------------------------------------------*/
/* no bsc retries */

void
BscNKeyRetries (lcb_t* lcb)
{

   lcb->poll_retries = DEFAULT_POLL_RETRIES;

}  /* BscNKeyRetries. */


/*----------------------------------------------------------------------------*/
/* bsc spec-poll */

void
BscKeySpecPoll (lcb_t* lcb)
{

   lcb->lack_handle_spec_poll = TRUE;

}  /* BscKeySpecPoll. */


/*----------------------------------------------------------------------------*/
/* no bsc spec-poll */

void
BscNKeySpecPoll (lcb_t* lcb)
{

   lcb->lack_handle_spec_poll = FALSE;

}  /* BscNKeySpecPoll. */


/*----------------------------------------------------------------------------*/

void
BscWriteConfig (lcb_t* lcb,
                uint bscCmd)
{
  boolean wr_pause = FALSE , wr_role = FALSE;

  if (!(lcb->idb->poll_protocol_flags & PP_ACTIVE))
     return;

  switch (bscCmd) {

  case BSC_CMD_PRIMARY:
    wr_role = (lcb->poll_role == PR_SECONDARY)||(lcb->poll_role == PR_POLLER);
    nv_write(wr_role, "bsc primary");
    break;

  case BSC_CMD_SECONDARY:
    wr_role = (lcb->poll_role == PR_PRIMARY)||(lcb->poll_role == PR_POLLEE);
    nv_write(wr_role, "bsc secondary");
    break;

  case BSC_CMD_CONTENTION:
    nv_write(lcb->poll_role == PR_CONTENTION, "bsc contention");
    break;

  case BSC_CMD_CHARSET:
       switch (lcb->char_set) {
       case CS_EBCDIC:
          nv_write(TRUE, "bsc char-set ebcdic");
          break;
       case CS_ASCII:
          nv_write(TRUE, "bsc char-set ascii");
          break;
       default:
          BscKeyCharSet(lcb, CS_EBCDIC);
          nv_write(TRUE, "bsc char-set ebcdic");
       }
    break;

  case BSC_CMD_FDX:
    break;

  case BSC_CMD_PAUSE:
     if (lcb->poll_role == PR_POLLER) {
        wr_pause = lcb->pause_timeout != POLLER_PAUSE_TIMEOUT;
     }
     nv_write(wr_pause, "bsc pause %d", lcb->pause_timeout / 100);
     break;

  case BSC_CMD_SERVLIM:
     nv_write(lcb->servlim != DEFAULT_SERVLIM,
              "bsc servlim %d", lcb->servlim);
     break;

  case BSC_CMD_POLL_TIMEOUT:
     nv_write(lcb->io_timer_table.timer[ToPoll] != POLL_TIMEOUT,
              "bsc poll-timeout %d", lcb->io_timer_table.timer[ToPoll] / 100);
     break;

  case BSC_CMD_RETRIES:
     nv_write(lcb->poll_retries != DEFAULT_POLL_RETRIES,
              "bsc retries %d", lcb->poll_retries);
     break;

  case BSC_CMD_SPECPOLL:
     nv_write(lcb->lack_handle_spec_poll, "bsc spec-poll");
     break;

   case BSC_CMD_HOST_TIMEOUT:
     nv_write(lcb->host_activity_timeout != DEFAULT_HOST_ACTIVITY_TIMEOUT,
              "bsc host-timeout %d", lcb->host_activity_timeout / 100);
     break;

 default:
     break;

  }  /* switch (bscCmd). */

}  /* BscWriteConfig. */


/*----------------------------------------------------------------------------*/

static void 
bsc_print_cucb ( 
    cucb_t *cucb, 
    boolean active_cu,
    uint address )
{
   static const char *const cu_states[] = {"Active", "Inactive", "Initializing"};

   if ((address == 0) || (address == cucb->address)) {

      uint queue_depth = queryqueuedepth(&cucb->outboundQ);

      printf("\n\n\tPoll address: %x. Select address: %x %s",
	  cucb->address, cucb->sel_address, (active_cu) ? "*CURRENT-CU*" : "");

      if (cucb->active_dev_addr != 0) {
         printf("\n\tCurrent active device address is: %x.", cucb->active_dev_addr);
      }

      printf("\n\tState is %s.", cu_states[cucb->state]);

      if (queue_depth != 0) {
          printf("\n\tOutput queue depth: %d.", queue_depth);
      }

      printf("\n\tTx Counts: %ld frames(total). %ld frames(data). %ld bytes.",
          cucb->tx_frame_ct.ctr[CtDataFrames] + cucb->tx_frame_ct.ctr[CtCtrlFrames],
          cucb->tx_frame_ct.ctr[CtDataFrames],
          cucb->tx_frame_ct.ctr[CtBytes]);
      printf("\n\tRx Counts: %ld frames(total). %ld frames(data). %ld bytes.",
          cucb->rx_frame_ct.ctr[CtDataFrames] + cucb->rx_frame_ct.ctr[CtCtrlFrames],
          cucb->rx_frame_ct.ctr[CtDataFrames],
          cucb->rx_frame_ct.ctr[CtBytes]);
   }
}


void
bsc_show_interface (hwidbtype* idb, uint address)
{
   lcb_t* lcb = idb->lcb;

   static const char* type_string[] = { "pass-through", "local-ack" };
   uint interface_type = 0 , queue_depth = queryqueuedepth(&lcb->outboundQ);

   if (bsc_query_local_ack_capable(idb))  {
       interface_type = 1;
   }

   printf("\nBSC %s on %s:", type_string[interface_type], idb->hw_namestring);

   if (queue_depth) {
      printf("\nOutput queue depth: %d.", queue_depth);
   }

   if (lcb->rx_state) {
      printf("\nFrame builder state: %s.", bsc_rbb_state_names[lcb->rx_state]);
   }

   if (idb->rx_in_progress && (*idb->rx_in_progress)(idb)) {
      printf("\nOut of SYN-hunt mode.");
   }

   if (interface_type == 1) {

      static const char *const lack_mode_string[] = {"Primary", "Secondary"};
      char* fsmstate;
      cucb_t* cucb;
      uint mode_type;

      if (lcb->poll_role == PR_POLLER) {
         mode_type = 0;
         fsmstate = pollerfsm_StateNames[lcb->state];
      } else {
         mode_type = 1;
         fsmstate = polleefsm_StateNames[lcb->state];
      }
      printf("\n%s state is %s.", lack_mode_string[mode_type], fsmstate);

      cucb = lcb->cucb_list.head;
      if (cucb) {
         printf("\nControl units on this interface:");
         while (cucb) {
            bsc_print_cucb(cucb, cucb == lcb->active_cu , address);
            cucb = cucb->next;
         }
         printf("\n");
      }
   } else {                            /* Pass thru. */
      printf("\nHDX enforcement state: %s.",
             bsc_map_hdx_state[lcb->hdx_state]);
      printf("\nFrame sequencing state: %s.",
             bsc_map_fs_state(lcb->poll_role, lcb->frame_seq_state));
   }

   printf("\nTotal Tx Counts: %ld frames(total). %ld frames(data). %ld bytes.",
          lcb->tx_frame_ct.ctr[CtDataFrames] + lcb->tx_frame_ct.ctr[CtCtrlFrames],
          lcb->tx_frame_ct.ctr[CtDataFrames], lcb->tx_frame_ct.ctr[CtBytes]);
   printf("\nTotal Rx Counts: %ld frames(total). %ld frames(data). %ld bytes.",
          lcb->rx_frame_ct.ctr[CtDataFrames] + lcb->rx_frame_ct.ctr[CtCtrlFrames],
	  lcb->rx_frame_ct.ctr[CtDataFrames], lcb->rx_frame_ct.ctr[CtBytes]);
   printf("\n");

}  /* bsc_show_interface. */

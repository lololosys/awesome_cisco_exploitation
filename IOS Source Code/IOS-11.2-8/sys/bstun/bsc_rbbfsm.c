/* $Id: bsc_rbbfsm.c,v 3.6.6.8 1996/09/10 08:48:25 jbalestr Exp $
 * $Source: /release/112/cvs/Xsys/bstun/bsc_rbbfsm.c,v $
 *------------------------------------------------------------------
 * bsc_rbbfsm.c - BSC Receive Block Building FSM definition.
 *
 * August 1994, James Balestriere
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * Prepared by Metaplex.
 * All rights reserved.
 * 
 * FSM which builds frames from the single character interrupts
 * 
 *------------------------------------------------------------------
 * $Log: bsc_rbbfsm.c,v $
 * Revision 3.6.6.8  1996/09/10  08:48:25  jbalestr
 * CSCdi62395:  bsc/bstun debug for specific groups and addresses
 * Branch: California_branch
 *
 * Revision 3.6.6.7  1996/08/13  02:15:44  gstovall
 * CSCdi39484:  Giant packets output on ethernet interface
 * Branch: California_branch
 * Make sure we drop giants, even it they are just a wee bit too big.
 *
 * Revision 3.6.6.6  1996/07/30  20:39:26  jbalestr
 * CSCdi62962:  bsc discards frames from Fujitsu ACT4000 ATM
 * Branch: California_branch
 *
 * Revision 3.6.6.5  1996/07/19  22:45:01  snyder
 * CSCdi63560:  move fsm tables to text space from data
 *              gets 53,716 bytes of data back for run from flash platforms
 *              gets 68 bytes of image for all
 *
 * Branch: California_branch
 *
 * Revision 3.6.6.4  1996/06/20  01:43:42  jbalestr
 * CSCdi52599:  bsc does not update interface error counters
 * Branch: California_branch
 *
 * Revision 3.6.6.3  1996/06/17  08:30:05  jhernand
 * Commit Modular Cobra to California_branch
 *
 * Revision 3.6.6.2  1996/05/17  10:40:45  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.5.38.5  1996/05/07  21:33:44  jbalestr
 * CSCdi52473:  bsc local-ack fails link for rx frames larger than 888
 * bytes at 2400
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.5.38.4  1996/04/22  08:57:51  jbalestr
 * CSCdi54541:  system gets stuck in BSC-3-BADLINESTATE
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.5.38.3  1996/04/10  08:26:13  jbalestr
 * CSCdi54180:  bsc framing does not see some poll sequences
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.5.38.2  1996/04/03  13:50:24  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.5.38.1  1996/03/17  17:35:54  ppearce
 * Sync IbuMod_Calif_branch to V111_1_3
 *
 * Revision 3.6.6.1  1996/03/18  19:01:17  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.5.18.4  1996/03/16  06:28:57  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.5.18.3  1996/03/13  01:12:06  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.5.18.2  1996/03/07  08:39:03  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.5.18.1  1996/02/20  13:38:34  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.6  1996/03/01  22:25:05  jbalestr
 * CSCdi49520:  BSC HD64570 interface loses polls to IBM FEP
 *
 * Revision 3.5  1995/12/04  02:34:47  jbalestr
 * CSCdi41360:  BSC local-ack, slow response to poll.
 *
 * Revision 3.4  1995/11/21  06:47:51  jbalestr
 * CSCdi43341:  BSC passthru, all bsc traffic stops while receiving a frame
 *
 * Revision 3.3  1995/11/21  05:43:53  jbalestr
 * CSCdi42577:  Router crashes on no bstun peer-name command
 *
 * Revision 3.2  1995/11/17  08:48:46  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:04:04  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/09/03  01:04:05  arothwel
 * CSCdi39397:  Fix:
 *
 *         o Add specific poll support for local-ack pollee.
 *         o Remove superfluous fields from lcb.
 *         o Generalize protocol main handler.
 *         o Finish off replacing all deprecated process mgnt calls.
 *         o Multiple poll-adds causing hard-on polling.
 *         o Add safety code to ignore 'spurious' Tx completes.
 *
 * Revision 2.3  1995/07/26  06:17:47  arothwel
 * CSCdi37680:  Fix BSC BADSHARE problem. Fix BSC FDX cfg prob. Fix BSC
 * dbg/sh cmd.
 *
 * Revision 2.2  1995/06/16  06:24:42  arothwel
 * CSCdi35957:  BSTUN/BSC cleanup.
 *
 * Revision 2.1  1995/06/07  20:14:10  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <ciscolib.h>
#include "interface_private.h"
#include "packet.h"

#include "bsc_lcb.h"
#include "bsc_rbbfsm.h"
#include "bsc_output.h"
#include "bsc_fsm.h"
#include "protocol_events.h"
#include "bsc_conv_table.h"
#include "bsc_general.h"
#include "../ip/ip.h"
#include "../tcp/tcpcompress.h"

/*=====================================================================*
 * forward definitions
 *=====================================================================*/

static void
bsc_process_receive_frame(
  int frame_type ,
  hwidbtype * idb
  );


/*
 * write data byte will write the byte to the buffer and ensure it
 * is within the maximum range.
 * RETURNS 0 if the buffer has been discarded
 */
static inline int
write_data_byte (
  hwidbtype * idb ,
  unsigned char input
  )
{
  if(idb->lcb->frame_builder_pak->datagramsize > idb->max_pak_size){
    
    /*
     * discard frame and restart receive
     */
    idb->counters.giants++;
    bsc_restart_receiver(idb);
    bsc_process_receive_frame(BSC_HW_RX_GIANT, idb);
    return(0);
  }
  else {
    /*
     * store byte in buffer
     */
    idb->lcb->frame_builder_pak->datagramstart[idb->lcb->frame_builder_pak->datagramsize++] = input;

    return(1);
  }
}

static void
bsc_RBB_Action_0 (hwidbtype * idb) { }

static void
bsc_RBB_Action_3 (hwidbtype * idb) {

  if (write_data_byte(idb, idb->lcb->input) != 0) {        
    /*
     * process receive block as a local frame
     */
    bsc_restart_receiver(idb);
    bsc_process_receive_frame(idb->lcb->frame_type_rcvd, idb);
  }
}

static void
bsc_RBB_Action_1 (hwidbtype * idb) {

  idb->lcb->frame_type_rcvd = BSC_HW_RX_PLSL;
  bsc_RBB_Action_3(idb);
}

static void
bsc_RBB_Action_2 (hwidbtype * idb) {

  write_data_byte(idb, idb->lcb->input);
  
}

static void
bsc_RBB_Action_4 (hwidbtype * idb) {
  
  /*
   * store current byte before discard
   */
  write_data_byte(idb, idb->lcb->input);
  
  /*
   * discard receive block
   */
  idb->counters.input_error++;
  idb->counters.input_frame++;
  bsc_restart_receiver(idb);
  bsc_process_receive_frame(BSC_HW_RX_INVALID, idb);
}

static void
bsc_RBB_Action_6 (hwidbtype * idb){

  idb->lcb->frame_type_rcvd = BSC_HW_RX_ENQ;
  bsc_RBB_Action_3(idb);
}

static void
bsc_RBB_Action_7 (hwidbtype * idb) {
  
  idb->lcb->frame_type_rcvd = BSC_HW_RX_HANGUP;
  bsc_RBB_Action_3(idb);
}

static void
bsc_RBB_Action_8 (hwidbtype * idb){
  
  if(write_data_byte(idb, idb->lcb->input) != 0) {        
    /*
     * put SYN SYN chars in data stream
     */
    if((idb->lcb->frame_builder_pak->datagramsize%256) == 0) {
      if(write_data_byte(idb, idb->syn_char) != 0) {
        write_data_byte(idb, idb->syn_char);
      }
    }
  }
}

static void
bsc_RBB_Action_9 (hwidbtype * idb){
  
  if(write_data_byte(idb, idb->lcb->input) != 0) {        
    /*
     * put DLE SYN chars in data stream
     */
    if((idb->lcb->frame_builder_pak->datagramsize%256) == 0) {
      if(write_data_byte(idb, idb->lcb->bsc_cs_table[CS_DLE]) != 0) {
        write_data_byte(idb, idb->syn_char);
      }
    }
  }
}

static void
bsc_RBB_Action_10 (hwidbtype * idb){
  
  /*
   * insert DLE char in data stream
   */
  if(write_data_byte(idb, idb->lcb->bsc_cs_table[CS_DLE]) != 0) {
    write_data_byte(idb, idb->lcb->input);
  }

}

static void
bsc_RBB_Action_11 (hwidbtype * idb){
  
  /*
   * store the byte
   */
  write_data_byte(idb, idb->lcb->input);

  /*
   * check the number of control bytes
   */
  if(idb->lcb->frame_builder_pak->datagramsize > 15) {
     /*
      * force through an input to discard the frame
      */
     bsc_rxChar_int(idb, idb->lcb->bsc_cs_table[CS_ITB], FALSE);
  }
}

static void
bsc_RBB_Action_12 (hwidbtype * idb)
{
   /* write the byte */
  if(write_data_byte(idb, idb->lcb->input) != 0) {
    /*
     * process local EOT frame
     */
    bsc_process_receive_frame(BSC_HW_RX_EOT, idb);
  }
}

static void
bsc_RBB_Action_13 (hwidbtype * idb)
{
  idb->lcb->frame_type_rcvd = BSC_HW_RX_NAK;
  bsc_RBB_Action_3(idb);
}

static void
bsc_RBB_Action_14 (hwidbtype * idb)
{
  /*
   * store DLE
   */
  if(write_data_byte(idb, idb->lcb->bsc_cs_table[CS_DLE]) != 0) {    
    /*
     * do rbb act 3
     */
    idb->counters.input_abort++;
    idb->counters.input_error++;
    idb->lcb->frame_type_rcvd = BSC_HW_RX_ABORT;
    bsc_RBB_Action_3(idb);
  }
}

static void
bsc_RBB_Action_15 (hwidbtype * idb)
{
  /*
   * if ascii then need to redrive the FSM 
   * don't forget to restore the parity ;)
   * else store the current byte
   */
  if(idb->lcb->char_set == CS_ASCII) {
    bsc_rxChar_int(idb, bsc_odd_parity_table[idb->lcb->input], FALSE);
  }
  else {
    write_data_byte(idb, idb->lcb->input);
  }
}

static void
bsc_RBB_Action_16 (hwidbtype * idb)
{
  idb->lcb->frame_type_rcvd = BSC_HW_RX_ACK0;
  bsc_RBB_Action_3(idb);
}

static void
bsc_RBB_Action_17 (hwidbtype * idb)
{ 
  idb->lcb->frame_type_rcvd = BSC_HW_RX_ACK1;
  bsc_RBB_Action_3(idb);
}

static void
bsc_RBB_Action_18 (hwidbtype * idb)
{ 
  idb->lcb->frame_type_rcvd = BSC_HW_RX_WACK;
  bsc_RBB_Action_3(idb);
}

static void
bsc_RBB_Action_19 (hwidbtype * idb)
{ 
  idb->lcb->frame_type_rcvd = BSC_HW_RX_RVI;
  bsc_RBB_Action_3(idb);
}

static void
bsc_RBB_Action_20 (hwidbtype * idb)
{
  /*
   * need to check previous byte
   * if STX then TTD else an ABORT
   */
  if(idb->lcb->frame_builder_pak->datagramsize == 1) {
    idb->lcb->frame_type_rcvd = BSC_HW_RX_TTD;
  }
  else {
    idb->counters.input_error++;
    idb->counters.input_abort++;
    idb->lcb->frame_type_rcvd = BSC_HW_RX_ABORT;
  }
  bsc_RBB_Action_3(idb);
}

static void
bsc_RBB_Action_21 (hwidbtype * idb)
{ 
  idb->lcb->frame_type_rcvd = BSC_HW_RX_ETX;
  bsc_RBB_Action_2(idb);
}

static void
bsc_RBB_Action_22 (hwidbtype * idb)
{ 
  idb->lcb->frame_type_rcvd = BSC_HW_RX_ETB;
  bsc_RBB_Action_2(idb);
}

static void
bsc_RBB_Action_23 (hwidbtype * idb)
{ 
  idb->lcb->frame_type_rcvd = BSC_HW_RX_ETX;
  bsc_RBB_Action_10(idb);
}

static void
bsc_RBB_Action_24 (hwidbtype * idb)
{ 
  idb->lcb->frame_type_rcvd = BSC_HW_RX_ETB;
  bsc_RBB_Action_10(idb);
}

static void
bsc_RBB_Action_25 (hwidbtype * idb)
{ 
  if(idb->lcb->input == 0xFF) {
     /*
      * force through an input to discard the frame
      */
     bsc_rxChar_int(idb, idb->lcb->bsc_cs_table[CS_ITB], FALSE);
  }
  else {
     bsc_RBB_Action_2(idb);
  }
}

static void
bsc_RBB_Action_26 (hwidbtype * idb)
{ 
  /*
   * start hunt
   */
  bsc_restart_receiver(idb);
}


/* Action 27
 * sets the mode to ascii transparent. No parity checks are done.
 */
static void
bsc_RBB_Action_27 (hwidbtype * idb)
{ 
    if (idb->poll_protocol_flags & PP_ASCII) {
        idb->lcb->char_table = ascii_transparent_to_bsc;
    }
    bsc_RBB_Action_2(idb);
}

/* Action 28
 * exits ascii transparent mode and receives the frame
 */
static void
bsc_RBB_Action_28 (hwidbtype * idb)
{
    if (idb->poll_protocol_flags & PP_ASCII) {
        idb->lcb->char_table = ascii_to_bsc;
    }
    bsc_RBB_Action_3(idb);
}

/* Action 29
 * exits ascii transparent mode and writes the byte
 */
static void
bsc_RBB_Action_29 (hwidbtype * idb)
{
    if (idb->poll_protocol_flags & PP_ASCII) {
        idb->lcb->char_table = ascii_to_bsc;
    }
    bsc_RBB_Action_2(idb);
}

/*=====================================================================*
 * RBB FSM state names.
 *=====================================================================*/

char* const bsc_rbb_state_names[] = {
      "Idle",
      "ENQ-Addr-Build",
      "SOH-Msg-Build",
      "STX-Msg-Build",
      "DLE-Start",
      "1st-BCC-Byte",
      "Last-BCC-Byte",
      "DLE-In-Msg",
      "DLE-STX-Msg-Build",
      "SOH-STX-ITB-1st-BCC",
      "ITB-Last-BCC",
      "DLE-STX-ITB-1st-BCC",
      "Add-Build-DLE",
      "PAD-Hunt",
      "Poll-hunt"
      };

/*=====================================================================*
 * RBB FSM table
 *=====================================================================*/

static const fsm_node bscRBBFsm[15/*state*/][18/*input*/] = {

  /*   */
  {             /* STATE 0  , Action   */   
  /* SYN   */    { 0    , bsc_RBB_Action_0 },
  /* SOH   */    { 2    , bsc_RBB_Action_2 },
  /* STX   */    { 3    , bsc_RBB_Action_2 },
  /* DLE   */    { 4    , bsc_RBB_Action_2 },
  /* ENQ   */    { 0    , bsc_RBB_Action_6 },
  /* PAD   */    { 1    , bsc_RBB_Action_25},
  /* SNCH  */    { 0    , bsc_RBB_Action_0 },
  /* other */    { 1    , bsc_RBB_Action_2 },
  /* ETX   */    { 0    , bsc_RBB_Action_4 },
  /* EOT   */    { 13   , bsc_RBB_Action_12},
  /* ETB   */    { 0    , bsc_RBB_Action_4 },
  /* ITB   */    { 0    , bsc_RBB_Action_4 },
  /* NAK   */    { 0    , bsc_RBB_Action_13},
  /* ACK02 */    { 1    , bsc_RBB_Action_2 },
  /* ACK12 */    { 1    , bsc_RBB_Action_2 },
  /* WACK2 */    { 1    , bsc_RBB_Action_2 },
  /* RX12  */    { 1    , bsc_RBB_Action_2 },
  /* BAD   */    { 0    , bsc_RBB_Action_4 }
  },
  { /* STATE 1   */   
  /* SYN   */    { 1    , bsc_RBB_Action_0 },
  /* SOH   */    { 0    , bsc_RBB_Action_4 },
  /* STX   */    { 0    , bsc_RBB_Action_4 },
  /* DLE   */    { 12   , bsc_RBB_Action_2 },
  /* ENQ   */    { 0    , bsc_RBB_Action_1 },
  /* PAD   */    { 1    , bsc_RBB_Action_11},
  /* SNCH  */    { 1    , bsc_RBB_Action_11},
  /* other */    { 1    , bsc_RBB_Action_11},
  /* ETX   */    { 0    , bsc_RBB_Action_4 },
  /* EOT   */    { 0    , bsc_RBB_Action_4 },
  /* ETB   */    { 0    , bsc_RBB_Action_4 },
  /* ITB   */    { 0    , bsc_RBB_Action_4 },
  /* NAK   */    { 0    , bsc_RBB_Action_4 },
  /* ACK02 */    { 1    , bsc_RBB_Action_11},
  /* ACK12 */    { 1    , bsc_RBB_Action_11},
  /* WACK2 */    { 1    , bsc_RBB_Action_11},
  /* RX12  */    { 1    , bsc_RBB_Action_11},
  /* BAD   */    { 0    , bsc_RBB_Action_4 }
  },
  { /* STATE 2   */
  /* SYN   */    { 2    , bsc_RBB_Action_0 },
  /* SOH   */    { 2    , bsc_RBB_Action_2 },
  /* STX   */    { 3    , bsc_RBB_Action_8 },
  /* DLE   */    { 4    , bsc_RBB_Action_9 },
  /* ENQ   */    { 0    , bsc_RBB_Action_6 },
  /* PAD   */    { 2    , bsc_RBB_Action_8 },
  /* SNCH  */    { 2    , bsc_RBB_Action_8 },
  /* other */    { 2    , bsc_RBB_Action_8 },
  /* ETX   */    { 5    , bsc_RBB_Action_21},
  /* EOT   */    { 2    , bsc_RBB_Action_8 },
  /* ETB   */    { 5    , bsc_RBB_Action_22},
  /* ITB   */    { 9    , bsc_RBB_Action_2 },
  /* NAK   */    { 2    , bsc_RBB_Action_8 },
  /* ACK02 */    { 2    , bsc_RBB_Action_8 },
  /* ACK12 */    { 2    , bsc_RBB_Action_8 },
  /* WACK2 */    { 2    , bsc_RBB_Action_8 },
  /* RX12  */    { 2    , bsc_RBB_Action_8 },
  /* BAD   */    { 0    , bsc_RBB_Action_4 }
  },
  { /* STATE 3   */   
  /* SYN   */    { 3    , bsc_RBB_Action_0 },
  /* SOH   */    { 3    , bsc_RBB_Action_8 },
  /* STX   */    { 3    , bsc_RBB_Action_8 },
  /* DLE   */    { 4    , bsc_RBB_Action_9 },
  /* ENQ   */    { 0    , bsc_RBB_Action_20},
  /* PAD   */    { 3    , bsc_RBB_Action_8 },
  /* SNCH  */    { 3    , bsc_RBB_Action_8 },
  /* other */    { 3    , bsc_RBB_Action_8 },
  /* ETX   */    { 5    , bsc_RBB_Action_21},
  /* EOT   */    { 3    , bsc_RBB_Action_8 },
  /* ETB   */    { 5    , bsc_RBB_Action_22},
  /* ITB   */    { 9    , bsc_RBB_Action_2 },
  /* NAK   */    { 3    , bsc_RBB_Action_8 },
  /* ACK02 */    { 3    , bsc_RBB_Action_8 },
  /* ACK12 */    { 3    , bsc_RBB_Action_8 },
  /* WACK2 */    { 3    , bsc_RBB_Action_8 },
  /* RX12  */    { 3    , bsc_RBB_Action_8 },
  /* BAD   */    { 0    , bsc_RBB_Action_4 }
  },
  { /* STATE 4   */   
  /* SYN   */    { 0    , bsc_RBB_Action_4 },
  /* SOH   */    { 0    , bsc_RBB_Action_4 },
  /* STX   */    { 8    , bsc_RBB_Action_27},
  /* DLE   */    { 0    , bsc_RBB_Action_4 },
  /* ENQ   */    { 0    , bsc_RBB_Action_4 },
  /* PAD   */    { 0    , bsc_RBB_Action_4 },
  /* SNCH  */    { 0    , bsc_RBB_Action_4 },
  /* other */    { 0    , bsc_RBB_Action_4 },
  /* ETX   */    { 0    , bsc_RBB_Action_4 },
  /* EOT   */    { 0    , bsc_RBB_Action_7 },
  /* ETB   */    { 0    , bsc_RBB_Action_4 },
  /* ITB   */    { 0    , bsc_RBB_Action_4 },
  /* NAK   */    { 0    , bsc_RBB_Action_4 },
  /* ACK02 */    { 0    , bsc_RBB_Action_16 },
  /* ACK12 */    { 0    , bsc_RBB_Action_17 },
  /* WACK2 */    { 0    , bsc_RBB_Action_18 },
  /* RX12  */    { 0    , bsc_RBB_Action_19 },
  /* BAD   */    { 0    , bsc_RBB_Action_4 }
  },
  { /* STATE 5   */   
  /* SYN   */    { 6    , bsc_RBB_Action_2 },
  /* SOH   */    { 6    , bsc_RBB_Action_2 },
  /* STX   */    { 6    , bsc_RBB_Action_2 },
  /* DLE   */    { 6    , bsc_RBB_Action_2 },
  /* ENQ   */    { 6    , bsc_RBB_Action_2 },
  /* PAD   */    { 6    , bsc_RBB_Action_2 },
  /* SNCH  */    { 6    , bsc_RBB_Action_2 },
  /* other */    { 6    , bsc_RBB_Action_2 },
  /* ETX   */    { 6    , bsc_RBB_Action_2 },
  /* EOT   */    { 6    , bsc_RBB_Action_2 },
  /* ETB   */    { 6    , bsc_RBB_Action_2 },
  /* ITB   */    { 6    , bsc_RBB_Action_2 },
  /* NAK   */    { 6    , bsc_RBB_Action_2 },
  /* ACK02 */    { 6    , bsc_RBB_Action_2 },
  /* ACK12 */    { 6    , bsc_RBB_Action_2 },
  /* WACK2 */    { 6    , bsc_RBB_Action_2 },
  /* RX12  */    { 6    , bsc_RBB_Action_2 },
  /* BAD   */    { 0    , bsc_RBB_Action_4 }
  },
  { /* STATE 6   */   
  /* SYN   */    { 0    , bsc_RBB_Action_28},
  /* SOH   */    { 0    , bsc_RBB_Action_28},
  /* STX   */    { 0    , bsc_RBB_Action_28},
  /* DLE   */    { 0    , bsc_RBB_Action_28},
  /* ENQ   */    { 0    , bsc_RBB_Action_28},
  /* PAD   */    { 0    , bsc_RBB_Action_28},
  /* SNCH  */    { 0    , bsc_RBB_Action_28},
  /* other */    { 0    , bsc_RBB_Action_28},
  /* ETX   */    { 0    , bsc_RBB_Action_28},
  /* EOT   */    { 0    , bsc_RBB_Action_28},
  /* ETB   */    { 0    , bsc_RBB_Action_28},
  /* ITB   */    { 0    , bsc_RBB_Action_28},
  /* NAK   */    { 0    , bsc_RBB_Action_28},
  /* ACK02 */    { 0    , bsc_RBB_Action_28},
  /* ACK12 */    { 0    , bsc_RBB_Action_28},
  /* WACK2 */    { 0    , bsc_RBB_Action_28},
  /* RX12  */    { 0    , bsc_RBB_Action_28},
  /* BAD   */    { 0    , bsc_RBB_Action_4 }
  },
  { /* STATE 7   */   
  /* SYN   */    { 8    , bsc_RBB_Action_0 },
  /* SOH   */    { 8    , bsc_RBB_Action_10},
  /* STX   */    { 8    , bsc_RBB_Action_10},
  /* DLE   */    { 8    , bsc_RBB_Action_10},
  /* ENQ   */    { 0    , bsc_RBB_Action_14},
  /* PAD   */    { 8    , bsc_RBB_Action_10},
  /* SNCH  */    { 8    , bsc_RBB_Action_10},
  /* other */    { 8    , bsc_RBB_Action_10},
  /* ETX   */    { 5    , bsc_RBB_Action_23},
  /* EOT   */    { 8    , bsc_RBB_Action_10},
  /* ETB   */    { 5    , bsc_RBB_Action_24},
  /* ITB   */    { 11   , bsc_RBB_Action_10},
  /* NAK   */    { 8    , bsc_RBB_Action_10},
  /* ACK02 */    { 8    , bsc_RBB_Action_10},
  /* ACK12 */    { 8    , bsc_RBB_Action_10},
  /* WACK2 */    { 8    , bsc_RBB_Action_10},
  /* RX12  */    { 8    , bsc_RBB_Action_10},
  /* BAD   */    { 0    , bsc_RBB_Action_4 }
  },
  { /* STATE 8   */   
  /* SYN   */    { 8    , bsc_RBB_Action_9 },
  /* SOH   */    { 8    , bsc_RBB_Action_9 },
  /* STX   */    { 8    , bsc_RBB_Action_9 },
  /* DLE   */    { 7    , bsc_RBB_Action_0 },
  /* ENQ   */    { 8    , bsc_RBB_Action_9 },
  /* PAD   */    { 8    , bsc_RBB_Action_9 },
  /* SNCH  */    { 8    , bsc_RBB_Action_9 },
  /* other */    { 8    , bsc_RBB_Action_9 },
  /* ETX   */    { 8    , bsc_RBB_Action_9 },
  /* EOT   */    { 8    , bsc_RBB_Action_2 },
  /* ETB   */    { 8    , bsc_RBB_Action_9 },
  /* ITB   */    { 8    , bsc_RBB_Action_9 },
  /* NAK   */    { 8    , bsc_RBB_Action_9 },
  /* ACK02 */    { 8    , bsc_RBB_Action_9 },
  /* ACK12 */    { 8    , bsc_RBB_Action_9 },
  /* WACK2 */    { 8    , bsc_RBB_Action_9 },
  /* RX12  */    { 8    , bsc_RBB_Action_9 },
  /* BAD   */    { 0    , bsc_RBB_Action_4 }
  },
  { /* STATE 9   */   
  /* SYN   */    { 10   , bsc_RBB_Action_15 },
  /* SOH   */    { 10   , bsc_RBB_Action_15},
  /* STX   */    { 10   , bsc_RBB_Action_15},
  /* DLE   */    { 10   , bsc_RBB_Action_15},
  /* ENQ   */    { 10   , bsc_RBB_Action_15},
  /* PAD   */    { 10   , bsc_RBB_Action_15},
  /* SNCH  */    { 10   , bsc_RBB_Action_15},
  /* other */    { 10   , bsc_RBB_Action_15},
  /* ETX   */    { 10   , bsc_RBB_Action_15},
  /* EOT   */    { 10   , bsc_RBB_Action_15},
  /* ETB   */    { 10   , bsc_RBB_Action_15},
  /* ITB   */    { 10   , bsc_RBB_Action_15},
  /* NAK   */    { 10   , bsc_RBB_Action_15},
  /* ACK02 */    { 10   , bsc_RBB_Action_15},
  /* ACK12 */    { 10   , bsc_RBB_Action_15},
  /* WACK2 */    { 10   , bsc_RBB_Action_15},
  /* RX12  */    { 10   , bsc_RBB_Action_15},
  /* BAD   */    { 0    , bsc_RBB_Action_4 }
  },
  { /* STATE A   */   
  /* SYN   */    { 3    , bsc_RBB_Action_29},
  /* SOH   */    { 3    , bsc_RBB_Action_29},
  /* STX   */    { 3    , bsc_RBB_Action_29},
  /* DLE   */    { 3    , bsc_RBB_Action_29},
  /* ENQ   */    { 3    , bsc_RBB_Action_29},
  /* PAD   */    { 3    , bsc_RBB_Action_29},
  /* SNCH  */    { 3    , bsc_RBB_Action_29},
  /* other */    { 3    , bsc_RBB_Action_29},
  /* ETX   */    { 3    , bsc_RBB_Action_29},
  /* EOT   */    { 3    , bsc_RBB_Action_29},
  /* ETB   */    { 3    , bsc_RBB_Action_29},
  /* ITB   */    { 3    , bsc_RBB_Action_29},
  /* NAK   */    { 3    , bsc_RBB_Action_29},
  /* ACK02 */    { 3    , bsc_RBB_Action_29},
  /* ACK12 */    { 3    , bsc_RBB_Action_29},
  /* WACK2 */    { 3    , bsc_RBB_Action_29},
  /* RX12  */    { 3    , bsc_RBB_Action_29},
  /* BAD   */    { 0    , bsc_RBB_Action_4 }
  },
  { /* STATE B   */   
  /* SYN   */    { 10   , bsc_RBB_Action_2 },
  /* SOH   */    { 10   , bsc_RBB_Action_2 },
  /* STX   */    { 10   , bsc_RBB_Action_2 },
  /* DLE   */    { 10   , bsc_RBB_Action_2 },
  /* ENQ   */    { 10   , bsc_RBB_Action_2 },
  /* PAD   */    { 10   , bsc_RBB_Action_2 },
  /* SNCH  */    { 10   , bsc_RBB_Action_2 },
  /* other */    { 10   , bsc_RBB_Action_2 },
  /* ETX   */    { 10   , bsc_RBB_Action_2 },
  /* EOT   */    { 10   , bsc_RBB_Action_2 },
  /* ETB   */    { 10   , bsc_RBB_Action_2 },
  /* ITB   */    { 10   , bsc_RBB_Action_2 },
  /* NAK   */    { 10   , bsc_RBB_Action_2 },
  /* ACK02 */    { 10   , bsc_RBB_Action_2 },
  /* ACK12 */    { 10   , bsc_RBB_Action_2 },
  /* WACK2 */    { 10   , bsc_RBB_Action_2 },
  /* RX12  */    { 10   , bsc_RBB_Action_2 },
  /* BAD   */    { 0    , bsc_RBB_Action_4 }
  },
  { /* STATE C   */   
  /* SYN   */    { 0    , bsc_RBB_Action_4 },
  /* SOH   */    { 0    , bsc_RBB_Action_4 },
  /* STX   */    { 0    , bsc_RBB_Action_4 },
  /* DLE   */    { 0    , bsc_RBB_Action_4 },
  /* ENQ   */    { 0    , bsc_RBB_Action_4 },
  /* PAD   */    { 0    , bsc_RBB_Action_4 },
  /* SNCH  */    { 0    , bsc_RBB_Action_4 },
  /* other */    { 0    , bsc_RBB_Action_4 },
  /* ETX   */    { 0    , bsc_RBB_Action_4 },
  /* EOT   */    { 0    , bsc_RBB_Action_4 },
  /* ETB   */    { 0    , bsc_RBB_Action_4 },
  /* ITB   */    { 0    , bsc_RBB_Action_4 },
  /* NAK   */    { 0    , bsc_RBB_Action_4 },
  /* ACK02 */    { 0    , bsc_RBB_Action_16},
  /* ACK12 */    { 0    , bsc_RBB_Action_4 },
  /* WACK2 */    { 0    , bsc_RBB_Action_4 },
  /* RX12  */    { 0    , bsc_RBB_Action_4 },
  /* BAD   */    { 0    , bsc_RBB_Action_4 }
  },
  { /* STATE D   */   
  /* SYN   */    { 0    , bsc_RBB_Action_26},
  /* SOH   */    { 0    , bsc_RBB_Action_26},
  /* STX   */    { 0    , bsc_RBB_Action_26},
  /* DLE   */    { 0    , bsc_RBB_Action_26},
  /* ENQ   */    { 0    , bsc_RBB_Action_26},
  /* PAD   */    { 14   , bsc_RBB_Action_0 },
  /* SNCH  */    { 14   , bsc_RBB_Action_0 },
  /* other */    { 0    , bsc_RBB_Action_26},
  /* ETX   */    { 0    , bsc_RBB_Action_26},
  /* EOT   */    { 0    , bsc_RBB_Action_26},
  /* ETB   */    { 0    , bsc_RBB_Action_26},
  /* ITB   */    { 14   , bsc_RBB_Action_0 },
  /* NAK   */    { 0    , bsc_RBB_Action_26},
  /* ACK02 */    { 0    , bsc_RBB_Action_26},
  /* ACK12 */    { 0    , bsc_RBB_Action_26},
  /* WACK2 */    { 0    , bsc_RBB_Action_26},
  /* RX12  */    { 0    , bsc_RBB_Action_26},
  /* BAD   */    { 0    , bsc_RBB_Action_4 }
  },
  { /* STATE E   */   
  /* SYN   */    { 0    , bsc_RBB_Action_0 },
  /* SOH   */    { 0    , bsc_RBB_Action_26},
  /* STX   */    { 0    , bsc_RBB_Action_26},
  /* DLE   */    { 0    , bsc_RBB_Action_26},
  /* ENQ   */    { 0    , bsc_RBB_Action_26},
  /* PAD   */    { 0    , bsc_RBB_Action_26},
  /* SNCH  */    { 14   , bsc_RBB_Action_0 },
  /* other */    { 0    , bsc_RBB_Action_26},
  /* ETX   */    { 0    , bsc_RBB_Action_26},
  /* EOT   */    { 0    , bsc_RBB_Action_26},
  /* ETB   */    { 0    , bsc_RBB_Action_26},
  /* ITB   */    { 0    , bsc_RBB_Action_26},
  /* NAK   */    { 0    , bsc_RBB_Action_26},
  /* ACK02 */    { 0    , bsc_RBB_Action_26},
  /* ACK12 */    { 0    , bsc_RBB_Action_26},
  /* WACK2 */    { 0    , bsc_RBB_Action_26},
  /* RX12  */    { 0    , bsc_RBB_Action_26},
  /* BAD   */    { 0    , bsc_RBB_Action_4 }
  }

};

void
bsc_rxChar_int (
      hwidbtype * idb , 
      uchar input , 
      boolean overrun 
  )
{
  fsm_node next_state;
  BSC_Input fsm_input;

  if (idb->lcb == NULL) {
      return;
  }

  if(overrun) {
    /*
     * discard the currently received frame
     * and restart SYN hunt
     */
    idb->counters.input_error++;
    idb->counters.input_overrun++;
    if (idb->lcb->frame_builder_pak) {
       idb->lcb->frame_builder_pak->datagramsize = 0;
    }
    bsc_restart_receiver(idb);

    return;
  }

  /*
   * allocate a receive buffer if none allocated
   */
  if(!idb->lcb->frame_builder_pak) {
    idb->lcb->frame_builder_pak = (idb->getsysbuf)(idb);
    /*
     * if there is still no receive buffer return
     */
    if(!idb->lcb->frame_builder_pak){
      bsc_restart_receiver(idb);
      return;
    }
  }
  
  /*
   * Map the chracter into something understood by RBB FSM.
   */
  if (idb->lcb->char_table == ascii_to_bsc) {
      /* strip the parity bit */
      idb->lcb->input = input&0x7F;
  } else {
      idb->lcb->input = input;
  }
  fsm_input = idb->lcb->char_table[input];

  /*
   * look up the next state in FSM
   */
  next_state = bscRBBFsm[idb->lcb->rx_state][fsm_input];

  /*
   * put FSM in next state
   */
  idb->lcb->rx_state = next_state.state;

  /*
   * drive the action for this state
   */
  (*next_state.action)(idb);
}

/*----------------------------------------------------------------------------*/
/* bsc_queue_for_process: Queue packet for process level handling. */
/*----------------------------------------------------------------------------*/

static void 
bsc_queue_for_process (
    hwidbtype *idb,
    paktype *pak)

{
    uchar *ptr, *testlong;
    ushort count;

    /*
     * Use idb->parse_packet to see if we need to change datagramstart,
     * and, to count broadcast packets:
     */
    count = pak->datagramsize;
    ptr = pak->datagramstart;
    testlong = ptr;

#ifdef ALIGN_DATAGRAM
    (*idb->parse_packet)(idb, pak, testlong);
#else
    (*idb->parse_packet)(idb, pak, NULL);
#endif

    /*
     * Did idb->parse_packet change datagramstart?
     */
    if (pak->datagramstart != ptr) {
	paktype *tmp;

	/*
	 * Yes, we need to copy the packet into a new buffer:
	 */
	tmp = pak;
	pak = input_getbuffer((uint)count, idb);
	if (pak) {
	    /* Call parse_packet again to set up the new packet. */
	    (*idb->parse_packet)(idb, pak, testlong);
	    pak->datagramsize = count;
	    bcopy(ptr, pak->datagramstart, (uint)count);
	    pak->flags = tmp->flags;
	    retbuffer(tmp);
	} else {
	    /*
	     * Failed to get new buffer to copy packet:
	     */
	    retbuffer(tmp);
	    MAYBE_DROPPED(idb);
	    return;
	}
    }

    /*
     * Toss it to the process level
     */
    idb->counters.rx_cumbytes += pak->datagramsize;
    idb->counters.inputs++;
    (*idb->iqueue)(idb, pak);
   return;

}  /* bsc_queue_for_process. */

/*=====================================================================
 * bsc_process_receive_frame
 *
 * bundles the completed frame for passing to the process level control
 *====================================================================*/

static void
bsc_process_receive_frame (
  int frame_type,
  hwidbtype * idb
  )
{
  /*
   * enqueue frame for processing OFF interrupt tail
   */
  set_minor_protocol_event(idb->lcb->frame_builder_pak,frame_type);
  set_if_input(idb->lcb->frame_builder_pak, idb->firstsw);

  bsc_queue_for_process(idb, idb->lcb->frame_builder_pak);

  /*
   * Clear the current pak. 
   * Will not be re-entered by another char interrupt so safe to do now.
   */
  idb->lcb->frame_builder_pak = 0;
}


/*=====================================================================
 * bsc_rxFramer_int
 *
 * determines minor protocol type frame from raw data
 * and possible hardware hint
 *====================================================================*/
static inline int 
decode_enq (paktype *pak)
{
   int i;
   if (pak->datagramsize == 1) {
        return (BSC_HW_RX_ENQ);
   } else if (pak->datagramsize > 16) {
        return (BSC_HW_RX_INVALID);
   }

   /* if any DLE in data stream then garbage frame otherwise
    * one of the many poll variants
    */

   for (i=0; i<pak->datagramsize; i++) {
      if (pak->datagramstart[i] == 0x10) {
         return (BSC_HW_RX_INVALID);
      }
   }
   return (BSC_HW_RX_PLSL);

} /* decode ENQ */

static int
decode_data (hwidbtype *idb, paktype **pak, boolean transparent)
{
   int rcode = BSC_HW_RX_INVALID;
   uchar lastphysicalbyte = (*pak)->datagramstart[(*pak)->datagramsize-1];

   /* need to first check the length so we don't scribble memory */
   if( (*pak)->datagramsize <= 2 ){
      /* not many frames are two bytes long. The ITB condition
         fails because to enter the first byte must be STX or SOH
         the send byte is ITB leaves no room for BCC
       */
      if(lastphysicalbyte == idb->lcb->bsc_cs_table[CS_ENQ]) {
        if((*pak)->datagramstart[0] == idb->lcb->bsc_cs_table[CS_STX])
           rcode = BSC_HW_RX_TTD;
        else
           rcode = BSC_HW_RX_ENQ;
      }
   }
   else {
      /* this has BCC chars so look further back */
      uchar lastdatabyte;
      uint char_posn, bcc_length;
      if (transparent)
         bcc_length = 2;
      else
         bcc_length = idb->lcb->bcc_length;
      char_posn = (*pak)->datagramsize - bcc_length - 1;
      lastdatabyte  = (*pak)->datagramstart[char_posn];

      if( lastdatabyte == idb->lcb->bsc_cs_table[CS_ETX] )
         rcode = BSC_HW_RX_ETX;
      else
      if( lastdatabyte == idb->lcb->bsc_cs_table[CS_ETB] )
         rcode = BSC_HW_RX_ETB;
      else      
      if( lastdatabyte == idb->lcb->bsc_cs_table[CS_ITB] ){
         /* this is only valid place to start ITB */
         idb->lcb->frame_builder_pak = *pak;
         *pak = 0; 
         return(rcode);
      }
      else      
      if( lastphysicalbyte == idb->lcb->bsc_cs_table[CS_ENQ]){
         rcode = BSC_HW_RX_ABORT;
      }
   }

   return(rcode);
}

static int
decode_dle (hwidbtype *idb, paktype **pak)
{
   int rcode = BSC_HW_RX_INVALID;
   uchar secondbyte = (*pak)->datagramstart[1];

   if(   (secondbyte == idb->lcb->bsc_cs_table[CS_SOH])
      || (secondbyte == idb->lcb->bsc_cs_table[CS_STX]))
      return decode_data(idb, pak, TRUE);

   if(secondbyte == idb->lcb->bsc_cs_table[CS_ACK0])
      rcode = BSC_HW_RX_ACK0;
   else
   if(secondbyte == idb->lcb->bsc_cs_table[CS_ACK1])
      rcode = BSC_HW_RX_ACK1;
   else
   if(secondbyte == idb->lcb->bsc_cs_table[CS_WACK])
      rcode = BSC_HW_RX_WACK;
   else
   if(secondbyte == idb->lcb->bsc_cs_table[CS_RVI])
      rcode = BSC_HW_RX_RVI;
   else
   if (secondbyte == idb->lcb->bsc_cs_table[CS_EOT]) {
      rcode = BSC_HW_RX_HANGUP;
   }
   return(rcode);
}


static uint
decode_trailer (
    hwidbtype *idb, 
    paktype **pak, 
    uchar firstbyte)
{
    uint event = BSC_HW_RX_INVALID;
    uchar lastbyte = (*pak)->datagramstart[(*pak)->datagramsize-1];

    if (lastbyte == idb->lcb->bsc_cs_table[CS_EOT]) {
        /* only single EOT is valid, else discard */
        if((*pak)->datagramsize==1){
            event = BSC_HW_RX_EOT;
        }
    } else
    if (lastbyte == idb->lcb->bsc_cs_table[CS_ENQ]) {
        event = decode_enq(*pak);
    } else
    if (firstbyte == idb->lcb->bsc_cs_table[CS_NAK]) {
        event = BSC_HW_RX_NAK;
    } else 
    if ((lastbyte == idb->lcb->bsc_cs_table[CS_ACK0]) &&
        ((*pak)->datagramstart[(*pak)->datagramsize-2] 
                               == idb->lcb->bsc_cs_table[CS_DLE])) {
        event = BSC_HW_RX_ACK0;
    } else {
        uint  char_posn = (*pak)->datagramsize - idb->lcb->bcc_length - 1;
        uchar lastdatabyte  = (*pak)->datagramstart[char_posn];

        if (lastdatabyte == idb->lcb->bsc_cs_table[CS_ETX])
           event = BSC_HW_RX_ETX;
        else
        if (lastdatabyte == idb->lcb->bsc_cs_table[CS_ETB])
           event = BSC_HW_RX_ETB;
        else
        /* 
         * there is a bug in the Cirrus driver which puts an extra
         * PAD on a frame in ASCII mode
         */
        if (((lastbyte&0x0F) == 0x0F) && (lastbyte != 0x1F)) {
            do 
                (*pak)->datagramsize--;
                /* to stop deep recursion, step over all PADs first */
            while ((*pak)->datagramstart[(*pak)->datagramsize-1] == 0x7F);

            event = decode_trailer(idb, pak, firstbyte);
        }        
    }
    return (event);
}


/* 
 * bsc_accumulate_to_itb :
 *   copy the contents of the pak to the accumulator pak
 *   if the addition is ITB stay in accumulator phase otherwise
 *   do recursive call to force frame type identification
 */
static void
bsc_accumulate_to_itb (lcb_t *lcb, paktype **pak)
{
    /*
     * accumulate the data to the stored ITB
     */
    paktype * blder = lcb->frame_builder_pak;

    if ((blder->datagramsize+(*pak)->datagramsize) <= lcb->idb->max_buffer_size) {
        bcopy((*pak)->datagramstart,
              &blder->datagramstart[blder->datagramsize],
              (*pak)->datagramsize);
        blder->datagramsize += (*pak)->datagramsize;
        
        /* free the pak that got accumulated then pass new one*/
        datagram_done(*pak);
        *pak = lcb->frame_builder_pak;
        lcb->frame_builder_pak = 0;
        
        /* do recursive call on accumulated packet */
        bsc_rxFramer_int(lcb->idb, pak);
    } else {
        /* we have a giant. the new sequence which did not terminate
           is of more interest in debugging */
        datagram_done(lcb->frame_builder_pak);
        lcb->frame_builder_pak = 0;
        set_minor_protocol_event((*pak), BSC_HW_RX_GIANT);
    }
}

void 
bsc_rxFramer_int ( hwidbtype *idb, paktype **pak )
{
   if(idb->lcb) {

      /*
       * first check for a giant frame
       */
      if ((*pak)->datagramsize > idb->max_buffer_size) {
          /* checking building ITB frame and free consistently*/
         if (idb->lcb->frame_builder_pak) {
             bsc_accumulate_to_itb(idb->lcb, pak);
         } else {
             set_minor_protocol_event((*pak), BSC_HW_RX_GIANT);
         }
         return;
      }

      /*
       * some hardware gives frame interupts on an ITB.
       * We try accumulate the entire frame before forwarding
       * to the next layer
       */
      if (idb->lcb->frame_builder_pak) {
          bsc_accumulate_to_itb(idb->lcb, pak);
      } else {

          uint event = BSC_HW_RX_INVALID;
          uchar firstbyte = (*pak)->datagramstart[0];

          if (  (firstbyte == idb->lcb->bsc_cs_table[CS_SOH])
             || (firstbyte == idb->lcb->bsc_cs_table[CS_STX]))
             event = decode_data(idb,pak, FALSE);
          else
          if (firstbyte == idb->lcb->bsc_cs_table[CS_DLE])
             event = decode_dle(idb,pak);
          else      
             event = decode_trailer(idb, pak, firstbyte);

          if (*pak) {
             set_minor_protocol_event((*pak),event);
          }
      }
   } /* if lcb */
   else {
      datagram_done(*pak);
      *pak = 0 ;
   }

}

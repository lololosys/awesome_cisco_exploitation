/* $Id: bsc_lack_fsm_actions.c,v 3.4.54.6 1996/09/10 08:48:13 jbalestr Exp $
 * $Source: /release/112/cvs/Xsys/bstun/bsc_lack_fsm_actions.c,v $
 *------------------------------------------------------------------
 * Action routines for BSC (poller and pollee) fsms.
 *
 * February 1995, Andrew R. Rothwell.
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: bsc_lack_fsm_actions.c,v $
 * Revision 3.4.54.6  1996/09/10  08:48:13  jbalestr
 * CSCdi62395:  bsc/bstun debug for specific groups and addresses
 * Branch: California_branch
 *
 * Revision 3.4.54.5  1996/07/22  18:22:17  snyder
 * CSCdi63677:  fix a pointer problem, and move more text to data
 *              moves 2692 bytes to data, 16 out of image completely
 * Branch: California_branch
 *
 * Revision 3.4.54.4  1996/06/20  01:43:35  jbalestr
 * CSCdi52599:  bsc does not update interface error counters
 * Branch: California_branch
 *
 * Revision 3.4.54.3  1996/06/17  08:29:49  jhernand
 * Commit Modular Cobra to California_branch
 *
 * Revision 3.4.54.2  1996/05/17  10:40:25  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.4.42.3  1996/04/26  07:32:28  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.4.42.2  1996/04/22  08:57:39  jbalestr
 * CSCdi54541:  system gets stuck in BSC-3-BADLINESTATE
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.4.42.1  1996/04/03  13:50:11  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.4.54.1  1996/03/18  19:01:12  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.22.2  1996/03/07  08:38:58  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.4.22.1  1996/02/20  13:38:20  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.4  1995/11/23  06:30:43  jbalestr
 * CSCdi43598:  bsc deb p shows extra bytes at start of frame
 *
 * Revision 3.3  1995/11/21  08:52:18  jbalestr
 * CSCdi42960:  bstun keepalive count
 * Added bstun keepalive interval and count
 *
 * Revision 3.2  1995/11/17  08:48:27  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:03:46  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/09/03  01:03:58  arothwel
 * CSCdi39397:  Fix:
 *
 *         o Add specific poll support for local-ack pollee.
 *         o Remove superfluous fields from lcb.
 *         o Generalize protocol main handler.
 *         o Finish off replacing all deprecated process mgnt calls.
 *         o Multiple poll-adds causing hard-on polling.
 *         o Add safety code to ignore 'spurious' Tx completes.
 *
 * Revision 2.2  1995/06/16  06:24:31  arothwel
 * CSCdi35957:  BSTUN/BSC cleanup.
 *
 * Revision 2.1  1995/06/07  20:13:53  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "packet.h"
#include "logger.h"
#include "bsc_lcb.h"
#include "bsc_lack_support.h"
#include "bsc_lack_fsm.h"
#include "bsc_cmd_set.h"
#include "bsc_conv_table.h"
#include "bsc_output.h"
#include "bstun.h"
#include "bstun_public.h"
#include "bsc_debug.h"
#include "interface_generic.h"
#include <string.h>

/****************************************
        Action: A_CheckCstate
        Purpose:
*****************************************/
int
A_CheckCstate (void* pECB)
{   
   lcb_t* lcb = (lcb_t*) pECB;
   if((lcb->state == CU_Down) && (lcb->idb->state==IDBS_UP)) {
      bsc_cstate(lcb->idb);
   }
   return(TRUE);

} /* A_CheckCstate */

/****************************************
        Action: A_CheckCuPollStop
        Purpose:
*****************************************/
int
A_CheckCuPollStop (void* pECB)
{   
  /*
   * this has been moved outside FSM as it has common 
   * action in all states hence handled outside FSM.
   * Look in bsc_checkCuPollStop
   */

   return(TRUE);

}  /* A_CheckCuPollStop. */

/****************************************
        Action: A_ClearCuNoPollFlag
        Purpose:
*****************************************/
int
A_ClearCuNoPollFlag (void* pECB)
{
   lcb_t* lcb = (lcb_t*) pECB;

   lcb->active_cu->no_poll = FALSE;
   return(TRUE);

}  /* A_ClearCuNoPollFlag. */


/****************************************
        Action: A_CuFlush
        Purpose:
*****************************************/
int
A_CuFlush (int tx_frame_ctidx,
           void* pECB)
{
   lcb_t* lcb = (lcb_t*) pECB;

   /*
    * update MIB statistics
    */
   
   if (tx_frame_ctidx != CtNone){
      lcb->tx_frame_ct.ctr[tx_frame_ctidx]++;
      lcb->active_cu->tx_frame_ct.ctr[tx_frame_ctidx]++;
   }

   lcb->flush = bsc_flush_queue(&lcb->active_cu->outboundQ, POLL_DATA_ETB);
   return(TRUE);

}  /* A_CuFlush. */

/****************************************
        Action: A_CuFlushAll
        Purpose:
*****************************************/
int
A_CuFlushAll (int tx_frame_ctidx,
              void* pECB)
{
   lcb_t* lcb = (lcb_t*) pECB;

   /*
    * update MIB statistics
    */   

   if (tx_frame_ctidx != CtNone){
      lcb->tx_frame_ct.ctr[tx_frame_ctidx]++;
   }

   if (lcb->active_cu != NULL) {
       bsc_flush_all_queue(&lcb->active_cu->outboundQ);
   }
   return(TRUE);
}

/****************************************
        Action: A_CuFreeData
        Purpose:
*****************************************/
int
A_CuFreeData (void* pECB)
{
   lcb_t* lcb = (lcb_t*) pECB;
   paktype* pak;

   pak = dequeue(&lcb->active_cu->outboundQ);
   pak_unlock(pak);
   datagram_done(pak);
   lcb->tx_pak = NULL;
   return(TRUE);

}  /* A_CuFreeData. */


/****************************************
        Action: A_RxIncr
        Purpose:
*****************************************/
int
A_RxIncr (int lcb_rx_frame_ctidx,
          int cucb_rx_frame_ctidx,
          void* pECB)
{
   lcb_t* lcb = (lcb_t*) pECB;

   lcb->rx_frame_ct.ctr[lcb_rx_frame_ctidx]++;
   lcb->active_cu->rx_frame_ct.ctr[cucb_rx_frame_ctidx]++;
   return(TRUE);

}  /* A_RxIncr. */


/****************************************
        Action: A_SetDevBusy
        Purpose:
*****************************************/
int
A_SetDevBusy (void* pECB)
{
   lcb_t* lcb = (lcb_t*) pECB;
   uint unit = bsc_get_unit_number(lcb,lcb->active_cu->active_dev_addr);

   lcb->active_cu->device_status[unit] = BSC_DEVICE_BUSY_BIT;
   return(TRUE);

}  /* A_SetDevBusy. */


/****************************************
        Action: Sw_SpecPolledDevice
        Purpose:
*****************************************/
int
Sw_SpecPollOnly (void* pECB)
{
   lcb_t* lcb = (lcb_t*) pECB;

   return(lcb->lack_handle_spec_poll ?
          sw_SpecPollDev : sw_NoSpecPollDev);

}  /* Sw_SpecPollOnly. */


/****************************************
        Action: Sw_Cu_MoreData
        Purpose:
*****************************************/
int
Sw_Cu_MoreData(void* pECB)
{
   lcb_t* lcb = (lcb_t*) pECB;
   int i;
   paktype * pak;

   /*
    * need to look at the queued data first and process
    * any queued status messages until get to the first
    * data frame on the queue
    */
   while((pak=dequeue(&lcb->active_cu->outboundQ))!=0){
     if(GET_CUCMD_FROM_PAK(pak)==DEVICE_STATUS_DATA){
       /* update the device status' */
       bsc_update_pollee_device_status(lcb, pak);
       pak_unlock(pak);
       datagram_done(pak);
     }
     else {
       /* this is a data frame so stop processing */
       requeue(&lcb->active_cu->outboundQ,pak);
       break;
     }
   }
   
   /*
    * now check the device status control blocks and send
    * any pending status' to the primary
    */
   if(lcb->active_cu->gstatus_pending) {
     for(i=0;i<MAX_ADDRS;i++){
       if(lcb->active_cu->gpoll_status[i] != BSC_DEVICE_OK){
         lcb->active_cu->active_dev_addr = lcb->dev_addr_table[i];
         return(SwStatus);
       }
     }
   }
   
   /*
    * if there is something on the queue it will be a data message
    */
   
   return(queryqueuedepth(&lcb->active_cu->outboundQ) ? SwMore : SwEmpty);

}  /* Sw_Cu_MoreData. */


/****************************************
        Action: Sw_Printer
        Purpose:
*****************************************/
int
Sw_Printer(void* pECB)
{
   lcb_t* lcb = (lcb_t*) pECB;

   return(bsc_query_printer_ack(lcb, lcb->tx_pak) ? swPr_Yes : swPr_No);

}  /* Sw_Printer. */


/****************************************
        Action: Sw_cState
        Purpose:
*****************************************/
int
Sw_cState(void* pECB)
{
   lcb_t* lcb = (lcb_t*) pECB;

   return((lcb->active_cu->state == CU_ACTIVE) ? swCS_Active : swCS_Inactive);

}  /* Sw_cState. */


/****************************************
        Action: Sw_DeviceState
        Purpose:
*****************************************/
int
Sw_DeviceState(void* pECB)
{
   lcb_t* lcb = (lcb_t*) pECB;
   uint unit = bsc_get_unit_number(lcb,lcb->active_cu->active_dev_addr);

   switch (lcb->active_cu->device_status[unit]) {
   case BSC_DEVICE_BUSY :
      return(swDS_DB);
   case BSC_DEVICE_END:
      return(swDS_DE);
   case BSC_DEVICE_OK:
      return(swDS_OK);
   default:
      return(swDS_Other);
   }

}  /* Sw_DeviceState. */


/****************************************
        Action: Sw_MoreSpecData
        Purpose:
*****************************************/

int
Sw_MoreSpecData(void* pECB)
{
   lcb_t* lcb = (lcb_t*) pECB;
   uint unit = bsc_get_unit_number(lcb,lcb->active_cu->active_dev_addr);
   return((lcb->active_cu->device_status[unit] != BSC_DEVICE_OK )
     ? swSMore : swSEmpty);

}  /* Sw_MoreSpecData. */


/****************************************
        Action: Sw_bidType
        Purpose:
*****************************************/
int
Sw_bidType(void* pECB)
{
   lcb_t* lcb = (lcb_t*) pECB;
   cucb_t* cucb;
   uchar poll_address;
   bstunaddrroutestate_e addr_route_state;
   int bid_type = swBid_Unknown;
   uchar *data = lcb->rx_pak->datagramstart;

   /*---- Validate poll: Cu & Dev addr's must be good. */
   if( (lcb->rx_pak->datagramsize != 5) 
      || (data[0] != data[1])     /* CU. */
      || (data[2] != data[3]) )   /* Dev. */
   {
     return (bid_type);
   }
   
   poll_address = bsc_map_sel_to_pol(lcb, data[0]);
   if (poll_address == 0)
   {
      return(bid_type);
   }
   cucb = bsc_cucb_find(lcb, poll_address);

   /*
    * address-route status will tell us if link is UP,DOWN or not configured
    */
   if ((!cucb) || (cucb && (cucb->state == CU_INIT)))
      addr_route_state = bstun_query_route_cfg(&poll_address, lcb->idb) ?
                         BSTUN_ROUTE_UP : BSTUN_ROUTE_ADDR_UNKNOWN;
   else
      addr_route_state = bstun_query_route(&poll_address, lcb->idb);

   /* if unknown delete cucb ( to handle dynamic delete ) */
   if (addr_route_state == BSTUN_ROUTE_ADDR_UNKNOWN) {
     if(cucb){
       bsc_cucb_delete(lcb, cucb);
     }
   }
   else {
     if (!cucb) {
       /*
        * first bid to this address
        */
       if (bsc_send_peer_alert(lcb, poll_address, POLL_ADDR_ADD, 0)){
         cucb = bsc_cucb_create(lcb, poll_address);
         if(cucb) {
           cucb->state = CU_INIT;
           TIMER_START(cucb->no_addr_add_until,POLL_ADDR_ADD_TIMEOUT);
         }
       }
     }
     else {
       /*
        * only if in init state and timer not expired
        * send the add again
        */
       if(addr_route_state != BSTUN_ROUTE_UP){
         cucb->state = CU_INIT;         
       }
       
       if(cucb->state == CU_INIT){
         if(AWAKE(cucb->no_addr_add_until)){
           if(bsc_send_peer_alert(lcb, poll_address, POLL_ADDR_ADD, 0))
             TIMER_START(cucb->no_addr_add_until,POLL_ADDR_ADD_TIMEOUT);
         }
       }
     }

     if (cucb) {
       lcb->active_cu = cucb;
       if (data[2] == lcb->bsc_cs_table[CS_GPOLL])
         bid_type = swBid_poll;
       else {
         if ( cucb->state == CU_ACTIVE ){
           bid_type = (data[0] != poll_address) ? 
             swBid_sel : swBid_Spoll;
           lcb->active_cu->active_dev_addr = data[2];
         }
       }
     }
   }
 
   return(bid_type);

}  /* Sw_bidType. */

/****************************************
        Action: A_ActivateCU
        Purpose:
*****************************************/
int
A_ActivateCU (void* pECB)
{
   lcb_t* lcb = (lcb_t*) pECB;
   /*
    * check the poll retry counter. If not equal
    * to the start value then some polls have
    * gone unanswered so count a softErr
    */
   if(lcb->active_cu->run_retry_ct.ctr[cPollRetry]
      != lcb->active_cu->cfg_retry_ct.ctr[cPollRetry]){
     A_TxIncr(CtSoftErr,CtSoftErr,pECB);
   }
   
   /*
    * reset the counter and make the CU active
    */
   A_ResetCU(cPollRetry, pECB);
   A_cState(CU_ACTIVE, pECB);
   return(TRUE);
}

/****************************************
        Action: A_BuildSpecPoll
        Purpose:
*****************************************/
int
A_BuildSpecPoll (void* pECB)
{
   lcb_t* lcb = (lcb_t*) pECB;

   bsc_format_enq_frame(lcb, lcb->cs_pak, lcb->active_cu->address,
                        lcb->active_cu->active_dev_addr);
   return(TRUE);

}  /* A_BuildSpecPoll. */


/****************************************
        Action: A_CancelIOTimer
        Purpose:
*****************************************/
int
A_CancelIOTimer (void* pECB)
{
   lcb_t* lcb = (lcb_t*) pECB;

   lcb_stop_io_timer(lcb);
   return(TRUE);

}  /* A_CancelIOTimer. */

/****************************************
        Action: A_Flush
        Purpose:
*****************************************/
int
A_Flush (int tx_frame_ctidx,
         void* pECB)
         
{
   lcb_t* lcb = (lcb_t*) pECB;


   /*
    * update MIB statistics
    */
   
   if (tx_frame_ctidx != CtNone){
      lcb->tx_frame_ct.ctr[tx_frame_ctidx]++;
      lcb->active_cu->tx_frame_ct.ctr[tx_frame_ctidx]++;
   }

   A_ResetCU(cDataRetry,pECB);

   lcb->tx_pak = NULL;
   lcb->flush = bsc_flush_queue(&lcb->outboundQ, SELECT_DATA_ETB);
   return(TRUE);

}  /* A_Flush. */


/****************************************
        Action: A_FreeData
        Purpose:
*****************************************/
int
A_FreeData (void* pECB)
{
   lcb_t* lcb = (lcb_t*) pECB;
   paktype* pak;

   pak = dequeue(&lcb->outboundQ);
   pak_unlock(pak);
   datagram_done(pak);
   lcb->tx_pak = NULL;
   return(TRUE);

}  /* A_FreeData. */


/****************************************
        Action: A_MakeSts
        Purpose:
*****************************************/
int
A_MakeSts (void* pECB)
{
   lcb_t* lcb = (lcb_t*) pECB;
   uint unit , dev_status ;
  /* get device unit number from active device */
   unit = bsc_get_unit_number(lcb,lcb->active_cu->active_dev_addr);
   dev_status = lcb->active_cu->device_status[unit];
   bsc_format_device_status(lcb, lcb->cs_pak,lcb->active_cu->address,
				 lcb->active_cu->active_dev_addr,dev_status);
   return(TRUE);

}  /* A_MakSts. */


/****************************************
        Action: A_PeerAlert
        Purpose:
*****************************************/
int
A_PeerAlert (int cu_cmd,
             void* pECB)
{
   lcb_t* lcb = (lcb_t*) pECB;

   (void) bsc_send_peer_alert(lcb, lcb->active_cu->address, (uchar) cu_cmd,
			      lcb->active_cu->active_dev_addr);
   return(TRUE);

}  /* A_PeerAlert. */


/****************************************
        Action: A_Rcv
        Purpose:
*****************************************/
int
A_Rcv (int frame_action,
       int lcb_frame_ctidx,
       int cucb_frame_ctidx,
       void* pECB)
{
   lcb_t* lcb = (lcb_t*) pECB;
   uint datasize = lcb->rx_pak->datagramsize;
   boolean drop_frame = FALSE;
   uchar cmd = NULL_CMD;

   /* if last byte is a PAD don't count it */
   if(lcb->rx_pak->datagramstart[datasize-1] == 0xFF){
     datasize--;
   }
   
   /*---- If 1st frame not an ENQ dump it. */
   if (lcb->active_cu) {
      /*----  Translate frame action into tunnel command. */
      switch(frame_action) {
      case FrPassEtb:
         cmd = POLL_DATA_ETB;
         break;
      case FrPassEtx:
         cmd = POLL_DATA_ETX;
         break;
      case FrPassSts:
         cmd = DEVICE_STATUS_DATA;
         break;
      case FrSelPassEtb:
         cmd = SELECT_DATA_ETB;
         break;
      case FrSelPassEtx:
         cmd = SELECT_DATA_ETX;
         break;
      case FrNotify:
      case FrFree:
      default:
         drop_frame = TRUE;
         break;
      }

      /*---- Decide whether to send frame on to BSTUN, or discard it. */
      if (!drop_frame) {
         bsc_output_frame("NDI-tx", lcb->rx_pak->datagramstart,
	                  lcb->rx_pak->datagramsize, lcb->idb);
         bsc_send_to_bstun(lcb->idb, lcb->rx_pak,
	   		   lcb->active_cu->address, cmd,
                           lcb->active_cu->active_dev_addr);
         datasize -= lcb->bcc_length;
      }
      else {
         datagram_done(lcb->rx_pak);
      }

      /*---- Update byte/frame received counters. */
      if (cucb_frame_ctidx != CtNone){
         lcb->active_cu->rx_frame_ct.ctr[cucb_frame_ctidx]++;
         lcb->active_cu->rx_frame_ct.ctr[CtBytes] += datasize;
      }   
   }
   else {
      datagram_done(lcb->rx_pak);
   }

   if (lcb_frame_ctidx != CtNone){
      lcb->rx_frame_ct.ctr[lcb_frame_ctidx]++;
      lcb->rx_frame_ct.ctr[CtBytes] += datasize;
   }    

   /*---- Free to use Rx pak again, one way or another. */
   lcb->rx_pak = NULL;
   return(TRUE);

}  /* A_Rcv. */


/****************************************
        Action: A_ResetCU
        Purpose:
*****************************************/
int
A_ResetCU (int retry_idx,
           void* pECB)
{
   lcb_t* lcb = (lcb_t*) pECB;

   lcb->active_cu->run_retry_ct.ctr[retry_idx] = lcb->active_cu->cfg_retry_ct.ctr[retry_idx];
   return(TRUE);

}  /* A_ResetCU. */


/****************************************
        Action: A_ResetLine
        Purpose:
*****************************************/
int
A_ResetLine (int tx_ack,
             int rx_ack,
             void* pECB)
{
   lcb_t* lcb = (lcb_t*) pECB;

   /*---- Reset lcb retry counters to original values. */
   bsc_reset_lcb_retry_counters(lcb);

   /*---- Reset expected Tx & Rx ACK characters. */
   lcb->tx_ack_next = (tx_ack == TxAck0) ?
                      lcb->bsc_cs_table[CS_ACK0] : lcb->bsc_cs_table[CS_ACK1];
   lcb->rx_ack_next = (rx_ack == RxAck0) ?
                      lcb->bsc_cs_table[CS_ACK0] : lcb->bsc_cs_table[CS_ACK1];

   /*---- Process any outstanding poll changes now. */
   if (lcb->pending_cu_change)
      bsc_cucb_process_poll_updates(lcb);
   return(TRUE);

}  /* A_ResetLine. */


/****************************************
        Action: Sw_RxFrame
        Purpose:
*****************************************/
int
Sw_RxFrame (void* pECB)
{
   lcb_t* lcb = (lcb_t*) pECB;

   uchar * data = lcb->rx_pak->datagramstart;

   /*
    * There are four types of frame that can be reeceived here
    * Status message SOH % R ...
    * Test Req       SOH % / ...
    * Read Modified  STX ...
    * Query          DLE STX ...
    */
   switch(data[0]){
   case SOH :
     /* check to see if rx frame is a status message */
     if ((lcb->rx_pak->datagramsize > 6)
         && (data[0] == lcb->bsc_cs_table[CS_SOH])
         && (data[1] == lcb->bsc_cs_table[CS_PERCENT])
         && (data[2] == lcb->bsc_cs_table[CS_UCR]))
     {
       lcb->active_cu->active_dev_addr = data[5];
       return(swRF_Sts);
     }
     else {
       /* no device address */
       return(swRF_Data);
     }
   case STX :
     lcb->active_cu->active_dev_addr = data[2];
     break;
   case DLE :
       lcb->active_cu->active_dev_addr = data[3];
   }
   
   /* stop spec poll because receive data from it */
   bsc_poll_list_delete(&lcb->active_cu->spec_poll_list,
                    lcb->active_cu->active_dev_addr);
   return(swRF_Data);
       
}  /* A_RxFrame. */


/****************************************
        Action: A_Send
        Purpose:
*****************************************/
int
A_Send (int frame_type,
        int lcb_tx_frame_ctidx,
        int cucb_tx_frame_ctidx,
        void* pECB)
{
   lcb_t* lcb = (lcb_t*) pECB;
   uchar* data;

   lcb->cs_pak->datagramstart = lcb->cs_pak->data_area + ENCAPBYTES;
   lcb->tx_pak = lcb->cs_pak;
   data = lcb->tx_pak->datagramstart;

   switch(frame_type) {
   case FrData:
      lcb->tx_pak = dequeue(&lcb->outboundQ);
      lcb->origin_queue = LCB_OBQ;
      memcpy(lcb->tx_save, lcb->tx_pak->datagramstart, BSC_HEADER_SIZE);
      lcb->tx_frame_ct.ctr[CtBytes] += (lcb->tx_pak->datagramsize - BSC_HEADER_SIZE);
      lcb->active_cu->tx_frame_ct.ctr[CtBytes] += (lcb->tx_pak->datagramsize - BSC_HEADER_SIZE);
      break;
      
   case FrCuData:
      lcb->tx_pak = dequeue(&lcb->active_cu->outboundQ);
      lcb->origin_queue = CUCB_OBQ;
      memcpy(lcb->tx_save, lcb->tx_pak->datagramstart, BSC_HEADER_SIZE);
      lcb->tx_frame_ct.ctr[CtBytes] += (lcb->tx_pak->datagramsize - BSC_HEADER_SIZE);
      lcb->active_cu->tx_frame_ct.ctr[CtBytes] += (lcb->tx_pak->datagramsize - BSC_HEADER_SIZE);
      break;
      
   case FrControl:
     /*
      * the poll frame is generated elsewhere
      * and because it has prepending EOT must be counted twice
      * and subtract three bytes for pad characters
      */
      if (lcb_tx_frame_ctidx != CtNone)
        lcb->tx_frame_ct.ctr[lcb_tx_frame_ctidx]++;
      if (cucb_tx_frame_ctidx != CtNone)
        lcb->active_cu->tx_frame_ct.ctr[cucb_tx_frame_ctidx]++;
      
      lcb->active_cu->tx_frame_ct.ctr[CtBytes] += 6;
      lcb->tx_frame_ct.ctr[CtBytes] += 6;
     break;
   case FrStatus :
     /* the status frame is generated elsewhere */
     lcb->tx_frame_ct.ctr[CtBytes] += (lcb->tx_pak->datagramsize - BSC_HEADER_SIZE);
     lcb->active_cu->tx_frame_ct.ctr[CtBytes] += (lcb->tx_pak->datagramsize - BSC_HEADER_SIZE);
     break;
   case FrAck:
      data[BSC_HEADER_SIZE] = lcb->bsc_cs_table[CS_DLE];
      data[BSC_HEADER_SIZE+1] = lcb->tx_ack_next;
      lcb->tx_pak->datagramsize = BSC_HEADER_SIZE+2;
      lcb->active_cu->tx_frame_ct.ctr[CtBytes] += 2;
      lcb->tx_frame_ct.ctr[CtBytes] += 2;
      break;
   case FrEnq:
      data[BSC_HEADER_SIZE] = lcb->bsc_cs_table[CS_ENQ];
      data[BSC_HEADER_SIZE+1] = lcb->bsc_cs_table[CS_PAD];
      lcb->tx_pak->datagramsize = BSC_HEADER_SIZE+2;

      /* don't count pads as part of data bytes */
      lcb->active_cu->tx_frame_ct.ctr[CtBytes]++;
      lcb->tx_frame_ct.ctr[CtBytes]++;
      break;
   case FrEot:
      data[BSC_HEADER_SIZE] = lcb->bsc_cs_table[CS_EOT];
      data[BSC_HEADER_SIZE+1] = lcb->bsc_cs_table[CS_PAD];
      lcb->tx_pak->datagramsize = BSC_HEADER_SIZE+2;

      /* don't count pads as part of data bytes */
      lcb->active_cu->tx_frame_ct.ctr[CtBytes]++;
      lcb->tx_frame_ct.ctr[CtBytes]++;
      break;
   case FrNak:
      data[BSC_HEADER_SIZE] = lcb->bsc_cs_table[CS_NAK];
      data[BSC_HEADER_SIZE+1] = lcb->bsc_cs_table[CS_PAD];
      lcb->tx_pak->datagramsize = BSC_HEADER_SIZE+2;

      /* don't count pads as part of data bytes */
      lcb->active_cu->tx_frame_ct.ctr[CtBytes]++;
      lcb->tx_frame_ct.ctr[CtBytes]++;
      break;
   case FrTtd:
      data[BSC_HEADER_SIZE] = lcb->bsc_cs_table[CS_STX];
      data[BSC_HEADER_SIZE+1] = lcb->bsc_cs_table[CS_ENQ];
      lcb->tx_pak->datagramsize = BSC_HEADER_SIZE+2;
      lcb->active_cu->tx_frame_ct.ctr[CtBytes] += 2;
      lcb->tx_frame_ct.ctr[CtBytes] += 2;
      break;
   case FrWack:
      data[BSC_HEADER_SIZE] = lcb->bsc_cs_table[CS_DLE];
      data[BSC_HEADER_SIZE+1] = lcb->bsc_cs_table[CS_WACK];
      lcb->tx_pak->datagramsize = BSC_HEADER_SIZE+2;
      lcb->active_cu->tx_frame_ct.ctr[CtBytes] += 2;
      lcb->tx_frame_ct.ctr[CtBytes] += 2;
      break;
   case FrRvi:   
      data[BSC_HEADER_SIZE] = lcb->bsc_cs_table[CS_DLE];
      data[BSC_HEADER_SIZE+1] = lcb->bsc_cs_table[CS_RVI];
      lcb->tx_pak->datagramsize = BSC_HEADER_SIZE+2;
      lcb->active_cu->tx_frame_ct.ctr[CtBytes] += 2;
      lcb->tx_frame_ct.ctr[CtBytes] += 2;
      break;
   }

   if (is_bsc_debugging_group(lcb->idb->bstun_group)) {
      bsc_output_frame("SDI-tx", lcb->tx_pak->datagramstart+BSC_HEADER_SIZE,
		       lcb->tx_pak->datagramsize-BSC_HEADER_SIZE, lcb->idb);
   }

   /*
    * update MIB statistics
    */
   
   if (lcb_tx_frame_ctidx != CtNone)
      lcb->tx_frame_ct.ctr[lcb_tx_frame_ctidx]++;
   if (cucb_tx_frame_ctidx != CtNone)
      lcb->active_cu->tx_frame_ct.ctr[cucb_tx_frame_ctidx]++;

   bsc_send_to_line(lcb->idb, lcb->tx_pak);           /* Put frame out on wire. */
   lcb->line_state = LS_TX;             /* We're transmitting now. */
   lcb_start_io_timer(lcb, DEFAULT_IO_TIMEOUT);  /* Start I/O timer. */
   return(TRUE);

}  /* A_Send. */


/****************************************
        Action: A_SetRxAck
        Purpose:
*****************************************/
int
A_SetRxAck (void* pECB)
{
   lcb_t* lcb = (lcb_t*) pECB;

   lcb->rx_ack_next = bsc_set_next_ack(lcb, lcb->rx_ack_next);
   return(TRUE);

}  /* A_SetRxAck. */


/****************************************
        Action: A_SetTxAck
        Purpose:
*****************************************/
int
A_SetTxAck (void* pECB)
{
   lcb_t* lcb = (lcb_t*) pECB;

   lcb->tx_ack_next = bsc_set_next_ack(lcb, lcb->tx_ack_next);
   return(TRUE);

}  /* A_SetTxAck. */


/****************************************
        Action: A_StartTimer
        Purpose:
*****************************************/
int
A_StartTimer (int timer_idx,
              void* pECB)
{
   lcb_t* lcb = (lcb_t*) pECB;

   lcb_start_io_timer(lcb, lcb->io_timer_table.timer[timer_idx]);
   return(TRUE);

}  /* A_StartTimer. */



/****************************************
        Action: A_TxDone
        Purpose:
*****************************************/
int
A_TxDone (int timer_idx,
          void* pECB)
{
   lcb_t* lcb = (lcb_t*) pECB;

   if (lcb->tx_pak != lcb->cs_pak) {
      lcb->tx_pak->datagramstart += BSC_SYN_PREFIX_SIZE - BSC_HEADER_SIZE ;
      memcpy(lcb->tx_pak->datagramstart, lcb->tx_save, BSC_HEADER_SIZE);
      lcb->tx_pak->datagramsize += BSC_HEADER_SIZE - BSC_SYN_PREFIX_SIZE - 1;
      switch (lcb->origin_queue) {
      case LCB_OBQ:
         requeue(&lcb->outboundQ, lcb->tx_pak);
         break;
      case CUCB_OBQ:
         requeue(&lcb->active_cu->outboundQ, lcb->tx_pak);
         break;
      }
   }

   if (timer_idx != ToNone)
      A_StartTimer(timer_idx, lcb);

   return(TRUE);

}  /* A_TxDone. */

 
/****************************************
        Action: A_TxAccepted
        Purpose:
*****************************************/
int  
A_TxAccepted(void* pECB)
{
   A_ResetCU(cSelRetry,pECB);
   A_ResetCU(cDataRetry,pECB);
   A_ResetCU(cPollRetry,pECB);

   if(Sw_Printer(pECB)==swPr_Yes){
      A_RcvDontWack(pECB);
   }

   A_FreeData(pECB);
   return(TRUE);
}

/****************************************
        Action: A_TxIncr
        Purpose:
*****************************************/
int
A_TxIncr (int lcb_frame_ctidx,
          int cucb_frame_ctidx,
          void* pECB)
{
   lcb_t* lcb = (lcb_t*) pECB;

   lcb->tx_frame_ct.ctr[lcb_frame_ctidx]++;
   lcb->active_cu->tx_frame_ct.ctr[cucb_frame_ctidx]++;
   return(TRUE);

}  /* A_TxIncr. */

/****************************************
        Action: A_UpdateSts
        Purpose:
*****************************************/
int
A_UpdateSts(void* pECB)
{
   lcb_t* lcb = (lcb_t*) pECB;
   
   bsc_update_poller_device_status(lcb, lcb->rx_pak);
   return(TRUE);
}

/****************************************
        Action: A_cState
        Purpose:
*****************************************/
int
A_cState (int cu_state,
          void* pECB)
{
   lcb_t* lcb = (lcb_t*) pECB;
   paktype* pak;
   int i, queue_count;

   if (cu_state != lcb->active_cu->state) {
      if (cu_state == CU_NONRESPONDING) {
         queue_count = queryqueuedepth(&lcb->outboundQ);
         for (i = 1; i < queue_count; i++) {
             pak = dequeue(&lcb->outboundQ);
             if (lcb->active_cu->address == bsc_map_sel_to_pol(lcb, pak->datagramstart[0])) {
                pak_unlock(pak);
                datagram_done(pak);
             }
             else
                enqueue(&lcb->outboundQ, pak);
         }
         if (bsc_send_peer_alert(lcb, lcb->active_cu->address, POLL_ADDR_INACTIVE, 0)) {
            bsc_cucb_inactivate(lcb->active_cu);       
            bsc_poll_list_move(&lcb->active_cu_list, &lcb->non_responding_cu_list,
                           lcb->active_cu->address);
            lcb->active_cu->state = cu_state;
         }
      }
      else {
         if (bsc_send_peer_alert(lcb, lcb->active_cu->address, POLL_ADDR_ACTIVE, 0)) {
            bsc_poll_list_move(&lcb->non_responding_cu_list, &lcb->active_cu_list,
                           lcb->active_cu->address);
            lcb->active_cu->state = cu_state;
         }
      }
   }
   return(TRUE);

}  /* A_cState. */

/****************************************
  BCC-fsm constructs
****************************************/

typedef enum { CRC_DLE , CRC_ITB , CRC_SYN , CRC_OTHER } bcc_input_e;
typedef enum { crc_a0 , crc_a1 , crc_a2 } bcc_act_e ;

const static uchar bcc_new_state [4][3] = {
              /**/   /**/   /**/ 
/* DLE   */  { 1   ,  2   ,  1  },
/* ITB   */  { 0   ,  0   ,  2  },
/* SYN   */  { 0   ,  2   ,  2  },
/* OTHER */  { 0   ,  2   ,  2  }
};

const static bcc_act_e bcc_next_action [4][3] = {
/* DLE   */  { crc_a1 , crc_a1 , crc_a0 },
/* ITB   */  { crc_a2 , crc_a2 , crc_a1 },
/* SYN   */  { crc_a0 , crc_a0 , crc_a1 },
/* OTHER */  { crc_a1 , crc_a1 , crc_a1 }
};

static inline bcc_input_e
decode( lcb_t* lcb, uchar c ) {
  /*
   * DLE, ITB are the same in ASCII and EBCDIC
   * so use direct code  rather than lookup table
   */
  if( c == lcb->bsc_cs_table[CS_SYN]){
     return(CRC_SYN);
  }
  else
  switch ( c) {
  case 0x10 :
     return(CRC_DLE);
  case 0x1F :
     return(CRC_ITB);
  default :
     return(CRC_OTHER);
  }
}

/****************************************
        Action: Sw_CrcValid
        Purpose:
*****************************************/
int
Sw_CrcValid (void* pECB)
{
   lcb_t* lcb = (lcb_t*) pECB;

  uchar *data = lcb->rx_pak->datagramstart;
  uchar state ;
  ushort accum = 0;
  int pos , end;
  uint          bcc_length;
  bcc_update_fn bcc_fn;

  /*
   * check first byte to get start of data stream
   * a DLE at the start means we wish to step over
   * an additional STX at the start of data.
   * DLE signals transparent data so in ASCII mode
   * the bcc functions should get overwritten to CRC16
   */
  if (*data == 0x10) {
    pos = 2;
    state = 2;
    bcc_length = 2;
    bcc_fn = bsc_crc16_rev_update;
  } else {
    pos = 1;
    state = 0;
    bcc_length = lcb->bcc_length;
    bcc_fn = lcb->bcc_fn;
  }

  /*
   * make the assumption that frame contains one and only
   * one ETB or ETX so no need for fsm input for it
   */
  end  = lcb->rx_pak->datagramsize - bcc_length - 1;
  data = &lcb->rx_pak->datagramstart[pos];

  /* go through every byte until get to end position */
  while ((pos < end) 
      && !((*data == lcb->bsc_cs_table[CS_ETX]) 
       || (*data == lcb->bsc_cs_table[CS_ETB]))) {
    bcc_input_e input = decode(lcb, *data);
    switch(bcc_next_action[input][state]){
    case crc_a0 :
      /* skip over byte */
      break;
    case crc_a1 :
      /* accumulate byte */
      (*bcc_fn)( *data , &accum );
      break;
    case crc_a2 :
      {
        /*
         * accumulate the next (bcc_length+1) bytes
         * only happens on ITB
         */    
        int p;
        for( p=0; p < bcc_length+1 ; p++ , data++){
          (*bcc_fn)( *data, &accum );
        }
        pos += bcc_length + 1;
        /* BCC must be zero at this stage as well */

        if (bcc_length == 1) {
            accum &= 0x7F;
        }

        if( accum != 0 ) {
           lcb->idb->counters.input_crc++;
           lcb->idb->counters.input_error++;
           return(swCrc_Bad);
        }
      }
    }
    state = bcc_new_state[input][state];
    data++;
    pos++;
  }/* while */

  /* stick last few bytes into accumulator */
  for( pos=0; pos < bcc_length+1 ; pos++ , data++){
    (*bcc_fn)( *data, &accum );
  }

  if (bcc_length == 1) {
      accum &= 0x7F;
  }

  /* if the total is zero then good crc */
  if(accum==0) {
    return(swCrc_Good);
  }
  else { 
    lcb->idb->counters.input_error++;
    lcb->idb->counters.input_crc++;
    return(swCrc_Bad); 
  }

}  /* Sw_CrcValid. */


/****************************************
        Action: Sw_CuRetry
        Purpose:
*****************************************/
int
Sw_CuRetry (int retry_idx,
            void* pECB)
{
   lcb_t* lcb = (lcb_t*) pECB;

   if (lcb->active_cu->run_retry_ct.ctr[retry_idx] > 0)
      lcb->active_cu->run_retry_ct.ctr[retry_idx]--;

   return(lcb->active_cu->run_retry_ct.ctr[retry_idx] == 0 ? SwCuRetry_Zero : SwCuRetry_NonZero);

}  /* Sw_CuRetry. */


/****************************************
        Action: Sw_Frame
        Purpose:
*****************************************/
int
Sw_Frame (void* pECB)
{
   lcb_t* lcb = (lcb_t*) pECB;
   int frame_type = 0;
   queuetype* obq;
   paktype* pak;

   obq = (lcb->poll_role == PR_POLLER) ? &lcb->outboundQ : &lcb->active_cu->outboundQ;
   pak = dequeue(obq);
   requeue(obq, pak);

   switch (GET_CUCMD_FROM_PAK(pak)) {
   case POLL_DATA_ETX:
   case SELECT_DATA_ETX:
      frame_type = swFrEtx;
      break;
   case POLL_DATA_ETB:
   case SELECT_DATA_ETB:
      frame_type = swFrEtb;
      break;
   case POLL_DATA_ABORT:
   case SEL_DATA_ABORT:
      frame_type = swFrAbort;
      break;
   }
   return(frame_type);

}  /* Sw_Frame. */


/****************************************
        Action: Sw_MoreData
        Purpose:
*****************************************/
int
Sw_MoreData (void* pECB)
{
   lcb_t* lcb = (lcb_t*) pECB;

   return(queryqueuedepth(&lcb->outboundQ) ? SwMore : SwEmpty);

}  /* Sw_MoreData. */


/****************************************
        Action: Sw_NextWork
        Purpose:
*****************************************/
int
Sw_NextWork (void* pECB)
{
   lcb_t* lcb = (lcb_t*) pECB;
   paktype* pak;

   /*---- Have chance to process poll updates. */
   if (lcb->pending_cu_change)
      bsc_cucb_process_poll_updates(lcb);

   /*---- If queue non-empty next work will try to issue select. */
   while (queryqueuedepth(&lcb->outboundQ) != 0 ) {
      pak = peekqueuehead(&lcb->outboundQ);
      lcb->active_cu =
        bsc_cucb_find(lcb, bsc_map_sel_to_pol(lcb,pak->datagramstart[0]));
      lcb->active_cu->active_dev_addr = pak->datagramstart[2];
      switch( bsc_device_clear_for_frame(lcb, lcb->active_cu) ) {
      case SS_OK :
        bsc_format_enq_frame(lcb, lcb->cs_pak,
                             lcb->active_cu->sel_address,
                             lcb->active_cu->active_dev_addr);
        return(swNW_Select);
      case SS_DB :
       /* move frame to cucb outboundQ for later processing */
        pak = dequeue(&lcb->outboundQ);
        enqueue(&lcb->active_cu->outboundQ,pak);
        break;
      default :
        /* free the pak as the device is dead */
        pak = dequeue(&lcb->outboundQ);
        pak_unlock(pak);
        datagram_done(pak);
      }
   }
 
   return(bsc_find_next_poll_work(lcb));

}  /* Sw_NextWork. */


/****************************************
        Action: Sw_Retry
        Purpose:
*****************************************/
int
Sw_Retry (int retry_idx,
          void* pECB)
{
   lcb_t* lcb = (lcb_t*) pECB;

   if (lcb->run_retry_ct.ctr[retry_idx] > 0)
      lcb->run_retry_ct.ctr[retry_idx]--;

   return(lcb->run_retry_ct.ctr[retry_idx] == 0 ? SwRetry_Zero : SwRetry_NonZero);

}  /* Sw_Retry. */


/****************************************
        Action: Sw_Send_Wack
        Purpose:
*****************************************/
int
Sw_Send_Wack (void* pECB)
{
   lcb_t* lcb = (lcb_t*) pECB;
   int send;

   switch(lcb->active_cu->wack_status) {
   case WS_UNKNOWN :
     /* only send ack on first printer frame */
     lcb->active_cu->wack_status = WS_WACK_ALL;
     send = sw_SW_no;
     break;
   case WS_DONT_WACK:
     send = sw_SW_no;
     break;
   case WS_WACK_ALL:
   case WS_DO_WACK:
     send = sw_SW_yes;
     break;
   default:
     send = sw_SW_no;
     break;
   }

   return(send);

}  /* Sw_Send_Wack. */

/****************************************
        Action: A_ClearStsData
        Purpose:
*****************************************/
int
A_ClearStsData (void* pECB)
{
   lcb_t* lcb = (lcb_t*) pECB;
   uint unit = bsc_get_unit_number(lcb,lcb->active_cu->active_dev_addr);

   /*
    * set both general and spec poll values to zero
    * unless in dev_busy state
    */
   if(lcb->active_cu->device_status[unit] != BSC_DEVICE_BUSY ){
     if(lcb->active_cu->gpoll_status[unit] != BSC_DEVICE_OK ){
       lcb->active_cu->gstatus_pending--;
       lcb->active_cu->gpoll_status[unit] = BSC_DEVICE_OK;
     }
     lcb->active_cu->device_status[unit] = BSC_DEVICE_OK;
   }

   return(TRUE);

}  /* A_ClearStsData */


/****************************************
        Action: A_RcvDoWack
        Purpose:
*****************************************/
int
A_RcvDoWack (void* pECB)
{
   lcb_t* lcb = (lcb_t*) pECB;
   if(lcb->active_cu->wack_status == WS_UNKNOWN) {
     if (bsc_send_peer_alert(lcb, lcb->active_cu->address, DEVICE_DO_WACK, 0)){
       lcb->active_cu->wack_status = WS_DO_WACK;
     }
   }
   return(TRUE);

}  /* A_RcvDoWack */

/****************************************
        Action: A_RcvDontWack
        Purpose:
*****************************************/
int
A_RcvDontWack (void* pECB)
{
   lcb_t* lcb = (lcb_t*) pECB;
   if(lcb->active_cu->wack_status == WS_UNKNOWN) {
     if (bsc_send_peer_alert(lcb, lcb->active_cu->address, DEVICE_DONT_WACK, 0)){
       lcb->active_cu->wack_status = WS_DONT_WACK;
     }
   }
  return(TRUE);

}  /* A_RcvDontWack */

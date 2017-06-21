/* $Id: bsc_output.c,v 3.2.60.3 1996/09/10 08:48:20 jbalestr Exp $
 * $Source: /release/112/cvs/Xsys/bstun/bsc_output.c,v $
 *------------------------------------------------------------------
 * bsc_output.c - BSC output encapsulation routines
 *
 * August 1994, James Balestriere
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * Prepared by Metaplex.
 * All rights reserved.
 * 
 * Set of functions to encapsulate the I/O stuff by providing
 * an extra level of indirection to ensure consistent output
 * 
 *------------------------------------------------------------------
 * $Log: bsc_output.c,v $
 * Revision 3.2.60.3  1996/09/10  08:48:20  jbalestr
 * CSCdi62395:  bsc/bstun debug for specific groups and addresses
 * Branch: California_branch
 *
 * Revision 3.2.60.2  1996/06/20  01:43:37  jbalestr
 * CSCdi52599:  bsc does not update interface error counters
 * Branch: California_branch
 *
 * Revision 3.2.60.1  1996/06/17  08:29:58  jhernand
 * Commit Modular Cobra to California_branch
 *
 * Revision 3.2  1995/11/17  08:48:40  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:03:58  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.6  1995/08/11  08:21:52  arothwel
 * CSCdi38625:  Fix:
 *              1. Broken timer s/ware stopping Tx/Rx.
 *              2. Incorrect bsc-mib sub-system dependencies.
 *              3. Premature starting of bstun-event-process.
 *              4. Removal of timestamp info from debugging messages.
 *              5. Replace deprecated process mngmt s/ware w/ new s/ware.
 *
 * Revision 2.5  1995/07/27  04:43:31  arothwel
 * CSCdi37743:  Make BSC and BSTUN debug output format similar.
 *
 * Revision 2.4  1995/07/26  07:30:08  arothwel
 * CSCdi37682:  Fix output of frames at BSC level.
 *
 * Revision 2.3  1995/07/26  06:17:46  arothwel
 * CSCdi37680:  Fix BSC BADSHARE problem. Fix BSC FDX cfg prob. Fix BSC
 * dbg/sh cmd.
 *
 * Revision 2.2  1995/06/16  06:24:39  arothwel
 * CSCdi35957:  BSTUN/BSC cleanup.
 *
 * Revision 2.1  1995/06/07  20:14:04  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "interface_private.h"
#include "packet.h"

#include "bsc_output.h"
#include "bsc_conf.h"
#include "bsc_lcb.h"
#include "bsc_general.h"
#include "bsc_debug.h"
#include "bstun.h"

void
bsc_output_event (hwidbtype* idb,
		  BSC_event event)
{

   if (bsc_debug_event) {
    
      buginf("\nBSC: %s: ", idb->hw_namestring);
     
      switch (event) {
      case BSCevent_BAD_NS :
         buginf("Bad NS(%02X) Expected(%02X)",
                *(idb->lcb->tx_pak->datagramstart+1) & CB_NS, idb->lcb->ns);
         break;
      case BSCevent_BAD_ADDR :
         buginf("Out of date frame received");
         break;
      case BSCevent_ADDR_NO_MAP :
         buginf("Could not map poll-select address");
         break;
      case BSCevent_NEW_ADDR :
         buginf("New Address(%02X) New NS(%02X)",
                 idb->lcb->secondary_address, idb->lcb->ns);
         break;
      case BSCevent_PENDING_TIMEOUT :
         switch( idb->lcb->hdx_state ) {
         case 1 :
            buginf("Transmit timed-out");
            break;
         case 2 :
            buginf("Response not received from remote");
         }
         break;
      case   BSCevent_BAD_HARD_OPCODE :
         buginf("Unknown hardware minor opcode detected");
         break;
      case   BSCevent_UNKNOWN_FRAME_TYPE :
         buginf("Unknown frame detected");
         break;     
      }
    }

}  /* bsc_output_event. */


void
bsc_output_state (const char* machine,
		  const char* event,
		  const char* oldstate,
		  const char* newstate,
		  hwidbtype* idb)
{

   if (bsc_debug_event) {
      buginf("\nBSC: %s: %s-FSM event: %s ", idb->hw_namestring,
             machine, event);
      if (oldstate != newstate)
         buginf("old_state: %s.  new_state: %s.", oldstate, newstate);
   }

}  /* bsc_output_state. */


void
bsc_output_frame (const char* msg,
		  const uchar* frame,
		  int size,
		  hwidbtype* idb)
{
   if (bsc_debug_packet) {
      buginf("\nBSC: %s: %s: Data (%d bytes):", idb->hw_namestring,
             msg, size);

      debug_output_frame_data(frame, size, bsc_debug_buffer_size);
   }
}  /* bsc_output_frame. */

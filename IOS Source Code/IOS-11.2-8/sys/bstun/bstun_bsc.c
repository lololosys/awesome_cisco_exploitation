/* $Id: bstun_bsc.c,v 3.2.60.3 1996/09/10 08:48:34 jbalestr Exp $
 * $Source: /release/112/cvs/Xsys/bstun/bstun_bsc.c,v $
 *------------------------------------------------------------------
 * bstun_bsc.c - Binding for BSTUN<->BSC configuration
 *
 * August 1994, Andrew Rothwell
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * Prepared by Metaplex.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: bstun_bsc.c,v $
 * Revision 3.2.60.3  1996/09/10  08:48:34  jbalestr
 * CSCdi62395:  bsc/bstun debug for specific groups and addresses
 * Branch: California_branch
 *
 * Revision 3.2.60.2  1996/05/17  10:40:59  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.2.48.3  1996/04/26  07:32:34  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.2.48.2  1996/04/11  08:19:46  jbalestr
 * CSCdi51807:  need a test command to transmit raw bisync frames
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.2.48.1  1996/04/03  13:50:35  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.2.60.1  1996/03/18  19:01:26  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/07  08:39:12  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  13:38:41  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  08:48:57  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:04:16  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.5  1995/09/03  01:04:10  arothwel
 * CSCdi39397:  Fix:
 *
 *         o Add specific poll support for local-ack pollee.
 *         o Remove superfluous fields from lcb.
 *         o Generalize protocol main handler.
 *         o Finish off replacing all deprecated process mgnt calls.
 *         o Multiple poll-adds causing hard-on polling.
 *         o Add safety code to ignore 'spurious' Tx completes.
 *
 * Revision 2.4  1995/08/11  08:21:57  arothwel
 * CSCdi38625:  Fix:
 *              1. Broken timer s/ware stopping Tx/Rx.
 *              2. Incorrect bsc-mib sub-system dependencies.
 *              3. Premature starting of bstun-event-process.
 *              4. Removal of timestamp info from debugging messages.
 *              5. Replace deprecated process mngmt s/ware w/ new s/ware.
 *
 * Revision 2.3  1995/07/27  04:43:31  arothwel
 * CSCdi37743:  Make BSC and BSTUN debug output format similar.
 *
 * Revision 2.2  1995/06/16  06:24:48  arothwel
 * CSCdi35957:  BSTUN/BSC cleanup.
 *
 * Revision 2.1  1995/06/07  20:14:20  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "master.h"
#include <string.h>
#include <ciscolib.h>
#include "logger.h"
#include "packet.h"
#include "../h/mgd_timers.h"

#include "bsc_lcb.h"
#include "bsc_general.h"
#include "bsc_rbbfsm.h"
#include "bsc_lack_support.h"

#include "bstun.h"
#include "bstun_externs.h"
#include "bstun_public.h"
#include "bstun_bsc.h"
#include "bstun_debug.h"

#include "interface_private.h"

bstun_protocol_info bsc_protocol_info;
bstun_protocol_info bsc_local_ack_protocol_info;


/*----------------------------------------------------------------------------*/

boolean
bsc_validate_address (hwidbtype* idb,
                      uchar* address,
                      ushort length)
{
    return((bsc_map_pol_to_sel(idb->lcb, address[0]) != 0x0));

}  /* bsc_validate_address. */


/*----------------------------------------------------------------------------*/

void
bstun_bsc_shutint (hwidbtype *idb)
{

   /*---- Stop hardware. */
   bsc_hwif_goingdown(idb);

   /*---- Destroy the BSC line control block, et al. */
   lcb_delete(idb->lcb);
   idb->lcb = NULL;
   return;

}  /* bstun_bsc_shutint. */


/*----------------------------------------------------------------------------*/

void
bstun_bsc_startint (hwidbtype* idb)
{

   /*---- Setup idb vector methods for ISR<->BSC, plus other stuff. */
   pp_idb_init(idb);

   /*---- Create and init line control block et al. */
   idb->lcb = lcb_create(idb, bstun_get_bevent_timer());

}  /* bstun_bsc_startint. */


/*----------------------------------------------------------------------------*/

void
bsc_print_frame (paktype *pak,
                 int group,
                 char *prefix,
                 hwidbtype *idb)
{  
    if (is_bstun_debugging_group(group)) {
        char *addrp;
        bstun_protocol_info *gpi;
        char buff[BSTUN_MAX_PEER_NAME];
        int size = pak->datagramsize;
      
        gpi = bstun_pinfo[group];
        addrp = (char *) pak->datagramstart;

        if (!gpi->onlyall) {
            addrp += gpi->addroffset;
            bstun_sprintf_addr((uchar *) addrp, FALSE, group, buff);
            addrp += gpi->addrlength + 2;
            size -= gpi->addroffset + gpi->addrlength + 2; 
        } else {
            buff[0] = '\0';
        }

        buginf("\nBSTUN %s: %s %s %s %s Data: ", gpi->name,
	   idb->hw_namestring, prefix, (*buff ? ("Addr: ") : ("")), buff);

        debug_output_frame_data(addrp, size, bstun_debug_buffer_size);
    }
}  /* bsc_print_frame. */


/*
 * bsc_line_state
 *
 * used to tell the protocol state of the line.
 * The interface will not be considered up until the bsc mode
 * command has been selected ie primary|secondary|contention.
 * return TRUE if line is fully configured and up.
 */

static boolean 
bsc_line_state (hwidbtype *idb)
{
   if (!idb->lcb) {
       return(FALSE);
   }

   return (idb->lcb->poll_role != PR_UNKNOWN);
}

/*----------------------------------------------------------------------------*/

bstun_protocol_info*
bstun_bsc_predefined_init (char **namep)
{

    sstrncpy(bsc_protocol_info.name, BSC_PROTOCOL_NAME, NAMEDTHINGNAMELEN+1);
    bsc_protocol_info.addrlength = 1;
    bsc_protocol_info.addroffset = 0;
    bsc_protocol_info.addrfmt = 'x';
    bsc_protocol_info.onlyall = FALSE;
    bsc_protocol_info.neverall = FALSE;
    bsc_protocol_info.permanent = TRUE;
    bsc_protocol_info.local_ack_capable = FALSE;
    bsc_protocol_info.shut_interface = bstun_bsc_shutint;
    bsc_protocol_info.start_interface = bstun_bsc_startint;
    bsc_protocol_info.validate_address = bsc_validate_address;
    bsc_protocol_info.hardware_forward_hook = bevent_hardware;
    bsc_protocol_info.network_forward_hook = bevent_network;
    bsc_protocol_info.network_receive_hook = bsc_pass_network_handler;
    bsc_protocol_info.hardware_receive_hook = bsc_pass_hardware_handler;
    bsc_protocol_info.timer_receive_hook = bsc_pass_timer_handler;
    bsc_protocol_info.print_frame = bsc_print_frame;
    bsc_protocol_info.line_state = bsc_line_state;
    *namep = &(bsc_protocol_info.name[0]);
    return(&bsc_protocol_info);

}  /* bstun_bsc_predefined_init. */


/*----------------------------------------------------------------------------*/

bstun_protocol_info*
bstun_bsc_local_ack_predefined_init (char **namep)
{

    sstrncpy(bsc_local_ack_protocol_info.name, BSC_LACK_PROTOCOL_NAME, 
	     NAMEDTHINGNAMELEN+1);
    bsc_local_ack_protocol_info.addrlength = 1;
    bsc_local_ack_protocol_info.addroffset = 0;
    bsc_local_ack_protocol_info.addrfmt = 'x';
    bsc_local_ack_protocol_info.onlyall = FALSE;
    bsc_local_ack_protocol_info.neverall = FALSE;
    bsc_local_ack_protocol_info.permanent = TRUE;
    bsc_local_ack_protocol_info.local_ack_capable = TRUE;
    bsc_local_ack_protocol_info.shut_interface = bstun_bsc_shutint;
    bsc_local_ack_protocol_info.start_interface = bstun_bsc_startint;
    bsc_local_ack_protocol_info.validate_address = bsc_validate_address;
    bsc_local_ack_protocol_info.hardware_forward_hook = bevent_hardware;
    bsc_local_ack_protocol_info.network_forward_hook = bevent_network;
    bsc_local_ack_protocol_info.network_receive_hook = bsc_lack_network_handler;
    bsc_local_ack_protocol_info.hardware_receive_hook = bsc_lack_hardware_handler;
    bsc_local_ack_protocol_info.timer_receive_hook = bsc_lack_timer_handler;
    bsc_local_ack_protocol_info.print_frame = bsc_print_frame;
    bsc_local_ack_protocol_info.line_state = bsc_line_state;
    *namep = &(bsc_local_ack_protocol_info.name[0]);
    return(&bsc_local_ack_protocol_info);

}  /* bstun_bsc_local_ack_predefined_init. */



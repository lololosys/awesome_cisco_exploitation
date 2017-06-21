/* $Id: if_les_ether.c,v 3.5.46.1 1996/03/18 20:42:13 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/les/if_les_ether.c,v $
 *------------------------------------------------------------------
 * if_les_ether.c  ethernet driver for low end software
 *
 * November 1994, Ashwin Baindur
 * Based on "if_les_lance.c", December 1989,  Chris Shaker   
 * Based on "if_lance.c", June 1988,  Bill Westfield
 *
 * Copyright (c) 1988-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: if_les_ether.c,v $
 * Revision 3.5.46.1  1996/03/18  20:42:13  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.5.14.2  1996/03/13  01:49:38  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.5.14.1  1996/01/24  22:24:10  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.5  1995/12/06  18:31:00  gstovall
 * CSCdi44769:  Interface idb queues are not getting populated correctly
 * Make sure the hwidbs are in the same order on the media hwidbQs as on
 * the hwidbQ.
 *
 * Revision 3.4  1995/11/20  22:26:00  sdurham
 * CSCdi42520:  IF MIB breaks modularity in oddball images
 *        fixed IFMIB subsystem, and moved linkUp/Down trap code to IFMIB.
 *
 * Revision 3.3  1995/11/17  17:40:18  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:59:14  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:32:33  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.5  1995/11/08  21:14:33  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.4  1995/08/10  18:58:00  gstovall
 * CSCdi38481:  Sapphire wont unthrottle
 * Convert a few IDBTYPE_LANCE to IDBTYPE_QUICC_ETHER that were missed.
 * Bad programmer, no pizza.
 *
 * Revision 2.3  1995/07/25  21:48:46  abaindur
 * CSCdi37628:  LOSTCARR msgs too frequent when c1000 ethernet cable
 * unplugged
 * Rate limit message. Also fix handling of line protocol state during
 * carrier loss, and counting of deferred transmits. Remove a bogus
 * check for idb->status in ethernet_enable.
 *
 * Revision 2.2  1995/06/21  03:01:09  gstovall
 * CSCdi35731:  Cleanup unused cruft in idb
 * Nuke some unused vectors and variables from the hwidb.
 *
 * Revision 2.1  1995/06/07  21:35:57  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "interface_private.h"
#include "logger.h"
#include "../os/pool.h"
#include "../snmp/snmp_api.h"
#include "../snmp/ifmib_registry.h"
#include "../les/if_les.h"
#include "../les/if_les_ether.h"
#include "../os/buffers.h"
#include "../if/network.h"
#include "../if/ether.h"
#include "../if/ether_private.h"



/*
 * les_ethernet_linestate()
 * Return current line protocol status
 */
static boolean les_ethernet_linestate (hwidbtype *hwidb)
{
    return(hwidb->ethernet_lineup);
}


/*
 * les_ethernet_line_up()
 * Call the link up SNMP trap
 */
void les_ethernet_line_up (hwidbtype *idb)
{
    if (!idb->ethernet_lineup) {
    	idb->ethernet_lineup = TRUE; /* Transmitted ok, must be up */
    	idb->reason = "Keepalive OK";
    	reg_invoke_ifmib_link_trap(LINK_UP_TRAP, idb);
    }
}


/*
 * les_ethernet_idb_init()
 * Initialize commons files in the idb for the low end ethernet drivers
 */
void les_ethernet_idb_init (hwidbtype *hwidb)
{
    idbtype *swidb = hwidb->firstsw;

    hwidb->status = IDB_ETHER | IDB_BCAST | IDB_BRIDGE;
    hwidb->state = IDBS_INIT;
    hwidb->ethernet_lineup = FALSE;
    hwidb->fast_switch_flag = 0;
    hwidb->span_encapsize = 0;	/* Transparent bridging */

    hwidb->pool = NULL;
    hwidb->pool_group = POOL_GROUP_INVALID;

    swidb->tbridge_media = TBR_ETHER;
    swidb->span_disabled = FALSE;

    /*
     * Setup common function calls
     */
    hwidb->linestate = les_ethernet_linestate;

    ether_idb_init(hwidb);
    ether_idb_enqueue(hwidb);
    idb_enqueue(hwidb);
}


/*
 * les_ethernet_periodic()
 * Do the common periodic checks
 */
void les_ethernet_periodic (hwidbtype *hwidb)
{
    /*
     * Force lineup if no keepalives are configured or we are shutdown.
     * Otherwise, send a keepalive. We cannot hear ourself like an MCI does,
     * but lance_TX_interrupt will clear ethernet_lineup if we get a lost
     * carrier indication back from the Lance. If we do not get a lost
     * carrier indication, lance_TX_interrupt will set ethernet_lineup. If
     * lance_RX_interrupt receives a packet, ethernet_lineup will be set.
     */
    if (hwidb->nokeepalive || hwidb->state == IDBS_ADMINDOWN) {
	if (!hwidb->ethernet_lineup) {
	    hwidb->ethernet_lineup = TRUE;
	    if (hwidb->state != IDBS_ADMINDOWN) {
		hwidb->reason = "Checking disabled";
		reg_invoke_ifmib_link_trap(LINK_UP_TRAP, hwidb);
	    }
	}
    } else {
	send_loop(hwidb, hwidb->hardware);
    }
}


/*
 * les_ethernet_show_errors()
 * Print out ethernet error statistics
 */
void les_ethernet_show_errors (hwidbtype *hwidb)
{
    ethersb *esb;
    idb_si_counters_t	*sic;

    esb = idb_get_hwsb_inline(hwidb, HWIDB_SB_ETHER);

    if (esb == NULL) {
	/*
	 * We be screwed
	 */
	crashdump(0);
    }
    sic = idb_get_counter_info(hwidb, IDB_COUNTER_SI_CNT_PTR);
    printf("%d missed datagrams, %d overruns\n",
	   hwidb->counters.input_resource - sic->input_resource,
	   hwidb->counters.input_overrun - sic->input_overrun);

    printf("%d transmitter underruns, %d excessive collisions\n",
	   hwidb->counters.output_underrun
	   - sic->output_underrun,
	   esb->output_excessive_collision -
	   esb->si_output_excessive_collision);

    printf("%d single collisions, %d multiple collisions\n",
           esb->output_one_collision - esb->si_output_one_collision,
           esb->output_more_collision - esb->si_output_more_collision);

    printf("%d dma memory errors, %d CRC errors\n",
	   hwidb->dma_memory_error - hwidb->si_dma_memory_error,
	   hwidb->counters.input_crc - sic->input_crc);

    printf("\n%d alignment errors, %d runts, %d giants\n",
           hwidb->counters.input_frame - sic->input_frame,
           hwidb->counters.runts - sic->runts,
           hwidb->counters.giants - sic->giants);
}
      

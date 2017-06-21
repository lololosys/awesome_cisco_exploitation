/* $Id: rvip_memd.c,v 3.2.62.3 1996/08/28 13:14:54 thille Exp $
 * $Source: /release/112/cvs/Xsys/src-rvip/rvip_memd.c,v $
 *------------------------------------------------------------------
 * rvip_memd.c - MEMD support for Checkers
 *
 * January 1995, David Getchell
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rvip_memd.c,v $
 * Revision 3.2.62.3  1996/08/28  13:14:54  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.2.62.2  1996/04/12  17:43:37  kao
 * CSCdi53527:  Need EOIR support on VIP2
 * Branch: California_branch
 *
 * Revision 3.2.62.1  1996/03/21  23:50:52  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.1  1996/03/18  23:35:58  mbeesley
 * Branch: ELC_branch
 * Sync to V111_1_3
 *
 * Revision 3.2  1995/11/17  18:50:40  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:29:17  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:59:08  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Include files
 */
#include "master.h"
#include "interface_private.h"
#include "types.h"
#include "sys_registry.h"
#include "packet.h"
#include "sched.h"
#include "logger.h"
#include "ttysrv.h"

#include "../pas/if_pas.h"
#include "../hes/ccb_cmd.h"
#include "../hes/if_vip.h"
#include "../hes/dgbus.h"
#include "../hes/if_fci.h"

#include "../if/network.h"
#include "../if/ether.h"

#include "../src-vip/vip_memd.h"
#include "../src-vip/vip_dma.h"
#include "../src-vip/msg_vip.c"		/* Not a typo, see logger.h */
#include "../src-vip/vip_debug.h"
#include "../src-vip/vip_dbus.h"
#include "../src-vip/dfs_vip_idb.h"

/*
 * vip_quiesce_all_interfaces:
 * Platform dependent code to quiesce all the interfaces on the card
 */
void vip_quiesce_all_interfaces (void)
{
    int waittime;

    /*
     * Recover from cbus stall if occurred
     */
    if (check_clock_loss() == TRUE)
	vsys_vec.bus_stall++;

    /*
     * First set a QUIESCE in a global control flag for the entire VIP
     * to instruct other routines not to access MEMD during the QUIESCE 
     * mode. The flag actually has been set in ccb_cmd_status()
     * This is a double insurance.
     */
    vip_dmaq_consumer();
    waittime = 0;
    while (dma_working) {
	if (++waittime > 100) {
	    ttyprintf(CONTTY, "\nDMA still active 100 ms after Quiescing");
	    break;
	}
	DELAY(1);
    }
    vip_flush_dmaq();
}

/*
 * vip_unquiesce_all_interfaces:
 * Platform dependent code to unquiesce all the interfaces on the card
 */
void vip_unquiesce_all_interfaces (void)
{
    /*
     * Recover from cbus stall if occurred
     */
    if (check_clock_loss() == TRUE)
	vsys_vec.bus_stall++;
    clr_cbus_quiesced();
}

/*
 * vip_network_chores:
 * Perform sundary chores on behalf of the Net IO interrupt handler.
 * This runs on the interrupt stack, called from the asm interrupt handler.
 */
void vip_network_chores (void)
{
    /*
     * Recover from cbus stall if occurred
     */
    if (check_clock_loss() == TRUE)
	vsys_vec.bus_stall++;

    /*
     * Check the command queue
     */
    vip_malu_attn_queue_check();

    /*
     * Check whether VIP is in QUIESCE mode
     */
    if (is_cbus_quiesced())
	return;

    /*
     * Do any DMA post-processing
     */
    vip_dmaq_consumer();

    /*
     * Poll for Tx traffic
     */
    if (vip_poll_tx_queues()) {
	vip_dmaq_consumer();
    }
}

/* end of file */


/* $Id: ipc_cbus_vip.c,v 3.7.4.6 1996/07/23 13:24:42 thille Exp $
 * $Source: /release/112/cvs/Xsys/ipc/ipc_cbus_vip.c,v $
 *------------------------------------------------------------------
 * ipc_cbus_vip.c - VIP CBUS transport-specific routines
 *
 * May 1995, Don Lobete
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ipc_cbus_vip.c,v $
 * Revision 3.7.4.6  1996/07/23  13:24:42  thille
 * CSCdi63329: Centralize logging for getbuffer() and pak_duplicate()
 * failures
 * Branch: California_branch
 * Shamelessly rip off Mr. Katz' code from CSCdi36037.  Provide
 * centralized logging for getbuffer() and pak_duplicate() failures and
 * then rip out the 9 bazillion different messages that where in the
 * various callers to report the failure.
 *
 * Revision 3.7.4.5  1996/05/23  03:58:05  kao
 * CSCdi58288:  inconsistent ip cache in VIP after mic rel with traffic
 * Branch: California_branch
 *
 * Revision 3.7.4.4  1996/04/12  17:42:51  kao
 * CSCdi53527:  Need EOIR support on VIP2
 * Branch: California_branch
 *
 * Revision 3.7.4.3  1996/04/10  03:38:53  dlobete
 * CSCdi53121:  Distributed IP Access Lists need to run at interrupt-level
 * Branch: California_branch
 *
 * Revision 3.7.4.2  1996/03/21  22:51:01  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.7.4.1  1996/03/18  20:18:15  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.2.5  1996/03/18  23:27:46  mbeesley
 * Branch: ELC_branch
 * Sync to V111_1_3
 *
 * Revision 3.2.2.4  1996/02/21  20:35:15  getchell
 * CSCdi49474:  Time to turn on IPC/DFS on viper
 * Also integrate patch from dlobete that allows dfs across mic rel
 * Branch: ELC_branch
 *
 * Revision 3.2.2.3  1996/02/10  00:06:08  mbeesley
 * CSCdi48581:  Add VIP2 support to ELC_branch
 * Branch: ELC_branch
 *
 * Revision 3.2.2.2  1996/02/08  07:49:32  gstovall
 * Branch: ELC_branch
 * Sync to ELC_baseline_960205.
 *
 * Revision 3.2.2.1  1995/12/04  22:17:22  mbeesley
 * Branch: ELC_branch
 * Perform mini-sync - particle refcount changes.
 * Revision 3.7  1996/03/09  03:18:22  kao
 * CSCdi51172:  RVIP crashes when PA bay 0 is empty
 *
 * Revision 3.6  1996/03/04  07:12:55  kao
 * CSCdi49674:  While VIP is cofigured with DFS, OIR event will crash the
 * VIP;
 *
 * Revision 3.5  1996/02/18  19:41:45  kao
 * CSCdi45136:  EOIR Problems with VIP card
 *
 * Revision 3.4  1996/01/19  03:03:16  dlobete
 * CSCdi44954:  Local-switching for DFS required
 * But that's not all! Also part of this jumbo Distributed IP
 * Switching commit:
 * - CSCdi43302: Illegal Access to Low Address crashes RVIP/4E (RSP2)
 * - DFS config which mirrors RSP route-cache setting
 *         (ie configurable distributed flow or optimal switching)
 * - Interrupt level IPC updates and invalidates for DFS cache operations
 * - Stay in closer synch with RSP cache for prefix invalidations
 *
 * Revision 3.3  1995/11/28  02:25:33  smackie
 * Move particle refcount into local memory to improve handling speed. Add
 * utility functions to allow some particle internals to be removed from
 * the drivers. (CSCdi44505)
 *
 * Revision 3.2  1995/11/17  09:37:32  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:59:38  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:05:31  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/30  00:21:35  dlobete
 * Placeholder for IPC.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "subsys.h"
#include "logger.h"
#include "config.h"
#include "parser.h"
#include "packet.h"
#include "interface_private.h"
#include "ttysrv.h"

#include "../os/pool.h"
#include "../os/particle.h"
#include "../os/buffers.h"
#include "../os/buffers_inline.h"

#include "../if/network.h"
#include "../hes/dgbus.h"
 
#include "../pas/if_pas.h"
#include "../src-vip/vip_memd.h"
#include "../src-vip/vip_dma.h"
#include "../src-vip/vip_dbus.h" 
 
#include "ipc_util.h"
#include "ipc.h"
#include "ipc_private.h"
#include "ipc_message.h"
#include "ipc_seats.h"
#include "ipc_ports.h"
#include "ipc_test.h"
#include "ipc_debug.h"
#include "ipc_cbus_vip.h"

static char slot_name_array[IPC_MAX_SLOTS][IPC_MAX_SEAT_NAME] = {
SLOT_0_NAME,
SLOT_1_NAME,
SLOT_2_NAME,
SLOT_3_NAME,
SLOT_4_NAME };

/*
 * Local storage
 */ 
static vip_memd_ifcntl_t *ipc_ifc;
static hwidbtype *ipc_idb;
static pas_cbtype *ipc_pascb;

static int ipc_tx_count;

#define IPC_TX_HIGH 100
#define IPC_TX_LOW  90

/*
 * ipc_cbus_transmit_to_rp:
 * Send a message up to the host
 */
boolean ipc_cbus_transmit_to_rp (ipc_message *message, ipc_seat_data *seat)
{
    pas_cbtype *pascb;
    hwidbtype *idb;
    ushort msg_size;
    boolean temp;

    /*
     * Find a surrogate interface. Try bay 0 first, then bay 1.
     */
    pascb = get_one_exist_pascb();
    if (!pascb)
	return(TRUE);

    /*
     * Send this message
     */
    idb = pascb->idb;
    msg_size = message->header->size + IPC_MESSAGE_ENCAPS_SIZE;
    if (ipc_debug_packets) {
	buginf("IPC: Sending %d bytes, on %s to master\n", 
	       msg_size, idb->hw_namestring);
    }
    temp = vip_send_ipc_message(pascb, (uchar *)message->header, msg_size);
    if (!temp && ipc_debug_packets) {
	buginf("IPC: failed to send message to host\n");
    }
    return(!temp);
}

/*
 * ipc_send_message_permanent_port:
 * Send IPC permanent port message
 */
boolean ipc_send_message_permanent_port (ipc_message *message, 
				      ipc_message_type type)
{
    boolean status_complement;

    message->header->dest_port = IPC_PERMANENT_PORT;
    message->header->type = type;
    status_complement = (*ipc_platform_struct.platform_tx_vector)(message, NULL);
    ipc_return_message(message);
    return(!status_complement);
}

/*
 * ipc_dispatch:
 * Dispatch a message that came down from the host. Called from the
 * DMA device driver.
 */ 
static void ipc_dispatch (hwidbtype *idb, paktype *pak)
{
    paktype *newpak;
    particletype *mp;
    uchar *dest = NULL;

    /*
     * Do some accounting
     */
    idb->counters.tx_cumbytes += pak->datagramsize;
    idb->counters.outputs += 1;

    if (ipc_debug_packets) {
	buginf("IPC: dispatch, pak=%#x, datagramstart=%#x, %d bytes\n",
	       pak, pak->datagramstart, pak->datagramsize);
    }

    /*
     * We can be handed either a scattered or contigous packet. Presently
     * the IPC server can only handle contigous packets, so check if thats
     * the case and coalesce the packet
     */
    if (!pak_particle_count(pak)) {
	/*
	 * Contigous packet. Send it to the IPC server
	 */
	ipc_process_raw_pak(pak);
    } else {
	newpak = getbuffer(pak->datagramsize);
	if (!newpak) {
	    return;
	}
	dest = newpak->datagramstart;
	while ((mp = particle_dequeue_inline(pak))) {
	    bcopy(mp->data_start, dest, mp->data_bytes);
	    dest += mp->data_bytes;
	    retparticle(mp);
	}
	ipc_process_raw_pak(newpak);
    }
}

static boolean ipc_null_vector (ipc_message *message, ipc_seat_data *seat)
{
    return(TRUE);
}

/*
 * ipc_quiesce:
 * Quiesce IPC
 */
void ipc_quiesce (void)
{
    ipc_flush_retry_queue();
    if (RPSP_SELECT)
	ipc_ifc->icb_ptr = vip_rpsp_icb_address(ipc_pascb);
    init_ifc_queues_caches(ipc_ifc);
    ipc_ifc->vip_if_status = VIP_QUIESCE;
    ipc_platform_struct.platform_tx_vector = ipc_null_vector;
}

/*
 * ipc_unquiesce:
 * UnQuiesce IPC
 */
void ipc_unquiesce (void)
{
    ipc_platform_struct.platform_tx_vector = ipc_cbus_transmit_to_rp;
}

/*
 * ipc_do_vip_init:
 * Initialize IPC on a VIP board
 */
boolean ipc_do_vip_init (void)
{
    ipc_platform_info *ipc_platform = &ipc_platform_struct;

    /*
     * Don't init more than once.
     */
    if (ipc_platform->platform_initialized) {
	return(TRUE);
    }

    /*
     * Allocate an idb, vip_memd_ifcntl and a pas_cbtype
     */
    ipc_idb = idb_create();
    ipc_pascb = malloc(sizeof(pas_cbtype));
    ipc_ifc = malloc(sizeof(vip_memd_ifcntl_t));
    if (!ipc_idb || !ipc_pascb || !ipc_ifc) {
	return(FALSE);
    }

    /*
     * Init fields in the idb, ifc and pascb
     */
    INSTANCE(ipc_idb) = ipc_pascb;
    ipc_pascb->idb = ipc_idb;
    ipc_pascb->txring_cnt_ptr = &ipc_tx_count;
    ipc_pascb->tx_polling_high = IPC_TX_HIGH;
    ipc_pascb->tx_polling_low = IPC_TX_LOW;
    ipc_pascb->vip_memd_ifcntl = ipc_ifc;
    ipc_ifc->idb = ipc_idb;
    ipc_idb->fastsend = ipc_dispatch;
    ipc_idb->hw_namestring = "IPC0/0";

    /*
     * FIXME : Check these
     */
    ipc_pascb->pa_bay = 0;
    ipc_pascb->pa_ifindex = 0;
    ipc_pascb->pa_device = 0;

    if (RPSP_SELECT)
	ipc_ifc->icb_ptr = vip_rpsp_icb_address(ipc_pascb);
    else
	vsys_vec.ipc_pascb = ipc_pascb;
    init_ifc_queues_caches(ipc_ifc);

    ipc_ifc->vip_if_status = VIP_DISABLE;
    ipc_ifc->old_vip_if_status = VIP_DISABLE;
    ipc_ifc->rx_offset = 0;

    ipc_platform->platform_initialized = TRUE;

    TIMER_START(ipc_globals->periodic, ONESEC);
    return(TRUE);
}

/*
 * ipc_do_vip_enable:
 * invoked at interrupt level as a result of an ipc_init command
 */
void ipc_do_vip_enable (ipc_port_id master_control_pid, ipc_seat my_seat_id)
{
    ipc_platform_info *ipc_platform = &ipc_platform_struct;
    volatile ushort *poll_q;

    if (!ipc_platform->platform_initialized) {
	return;
    }

    /*
     * Make sure ipc_globals has been initialized
     */
    if (ipc_globals == NULL) 
	return;

    /* 
     * Don't enable if already enabled
     */
    if (!ipc_ifc || (ipc_ifc->vip_if_status == VIP_ENABLE)) {
	return;
    }
    ipc_platform->platform_tx_vector = ipc_cbus_transmit_to_rp;
    ipc_platform->platform_init_slaves_vector = NULL;
    ipc_platform->type = IPC_SEAT_CXBUS_VIP;
 
    /*
     * Do a delayed renumber because we need to do
     * seat creation at process-level - not interrupt
     * level at which we're at here.
     * So save off the vital info into platform_struct.
     */
    ipc_platform->master_control_pid = master_control_pid;
    ipc_globals->zone_manager_pid = master_control_pid;
    ipc_platform->platform_seat = my_seat_id;
    ipc_platform->platform_seat_name = slot_name_array[vip_logical_slot()];
    ipc_platform->do_delay_initialize = TRUE;

    /* 
     * We need to set vip_status = Enable so that tx queue polling will work
     */
    ipc_ifc->vip_if_status = VIP_ENABLE;
    if (RPSP_SELECT) {
	poll_q = &(ipc_ifc->icb_ptr->icb_rpsp_ipcque);
	vip_enable_tx_polling(ipc_pascb, poll_q, 200);
    } else {
	ipc_ifc->rsp_tx_q = vsys_vec.rsp_ipc_tx_q;
	vip_enable_tx_polling(ipc_pascb, NULL, 200);
    }
}

/* end of file */

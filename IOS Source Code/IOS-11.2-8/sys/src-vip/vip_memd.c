/* $Id: vip_memd.c,v 3.15.4.15 1996/08/28 13:15:24 thille Exp $
 * $Source: /release/112/cvs/Xsys/src-vip/vip_memd.c,v $
 *------------------------------------------------------------------
 * vip_memd.c - VIP memd support
 *
 * February 1995, David Getchell
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: vip_memd.c,v $
 * Revision 3.15.4.15  1996/08/28  13:15:24  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.15.4.14  1996/08/20  00:36:16  cheilek
 * CSCdi58374:  More show diag info needed
 * Branch: California_branch
 * Add CCB_CMD_VIP_GET_CONFIG, vip_ll_sw.
 * Add sw_version_str to slot structure.
 *
 * Revision 3.15.4.13  1996/06/25  18:06:08  kao
 * CSCdi60971:  FDDI port adapter in PA0 stops working after EOIR
 * Don't send IPC or love letter during QUIESCE period
 * Branch: California_branch
 *
 * Revision 3.15.4.12  1996/06/10  15:13:41  fsunaval
 * CSCdi57738:  VIP2 SW: FDDI PA doesnt show claim, beacon counter at RSP
 * level.
 * Branch: California_branch
 *
 * Revision 3.15.4.11  1996/06/10  02:12:35  fsunaval
 * CSCdi57738:  VIP2 SW: FDDI PA doesnt show claim, beacon counter at RSP
 * level.
 * Branch: California_branch
 * 1.Fix the claim, beacon and trace counters.
 * 2.Make the MIB in the RSP aware about single/multimode interface.
 * 3.Remove excess debugging stuff.
 *
 * Revision 3.15.4.10  1996/06/03  17:29:58  hwang
 * CSCdi57723:  VIP1 tokenring interfaces wont come up
 * Branch: California_branch
 *
 * Revision 3.15.4.9  1996/06/01  16:01:44  kao
 * CSCdi57759:  VIP2 system exception & DBUS-3-DBUSINTERR error when doing
 * eoir.
 * Minor comment correction.
 * Branch: California_branch
 *
 * Revision 3.15.4.8  1996/05/26  00:17:44  hwang
 * CSCdi58206:  srb testing locks up svip
 * make vip_message2buffer available to functions outside vip_memd.c
 * Branch: California_branch
 *
 * Revision 3.15.4.7  1996/05/08  04:32:07  tboyle
 * CSCdi54363:  SW-VIP2: Dropped packets are unaccountable in FDDI-PA.
 * Branch: California_branch
 *
 * Revision 3.15.4.6  1996/04/12  17:44:43  kao
 * CSCdi53527:  Need EOIR support on VIP2
 * Branch: California_branch
 *
 * Revision 3.15.4.5  1996/04/10  03:39:04  dlobete
 * CSCdi53121:  Distributed IP Access Lists need to run at interrupt-level
 * Branch: California_branch
 *
 * Revision 3.15.4.4  1996/04/08  13:45:04  getchell
 * CSCdi52361:  DFS broken using unlike media
 * Branch: California_branch
 *
 * Revision 3.15.4.3  1996/04/04  19:45:31  tboyle
 * CSCdi51923:  Dynamic particle sizes for VIP2 based on SRAM size
 * Branch: California_branch
 *
 * Revision 3.15.4.2  1996/03/21  23:52:29  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.15.4.1  1996/03/18  22:03:48  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.2.11  1996/03/18  23:36:47  mbeesley
 * Branch: ELC_branch
 * Sync to V111_1_3
 *
 * Revision 3.3.2.10  1996/03/08  14:50:41  getchell
 * Unsafe to use pool_dequeue_cache at process level.
 * CSCdi49255:  Several error messages found during viper bandwidth perf.
 * testing.
 * Branch: ELC_branch
 *
 * Revision 3.3.2.9  1996/03/07  17:30:04  getchell
 * CSCdi50953:  viper dfs mods
 * Branch: ELC_branch
 *
 * Revision 3.3.2.8  1996/02/21  20:38:06  getchell
 * CSCdi49474:  Time to turn on IPC/DFS on viper
 * Also integrated patch from dlobete that allows dfs across mic rel
 * Branch: ELC_branch
 *
 * Revision 3.3.2.7  1996/02/21  02:54:49  mbeesley
 * CSCdi49429:  Messages to host (RP or RSP) are broken
 *         o Fix IPC and love letter processing
 *         o Fix buffer header stack processing for VIP1 on RP/SP
 * Branch: ELC_branch
 *
 * Revision 3.3.2.6  1996/02/16  22:34:30  getchell
 * CSCdi49237:  svip kernel and asm files out of whack
 * change function definition for malu queue handler (no longer static)
 * Branch: ELC_branch
 *
 * Revision 3.3.2.5  1996/02/10  00:22:34  mbeesley
 * CSCdi48581:  Add VIP2 support to ELC_branch
 * Branch: ELC_branch
 *
 * Revision 3.3.2.4  1996/02/08  09:31:17  gstovall
 * Branch: ELC_branch
 * Sync to ELC_baseline_960205.
 *
 * Revision 3.3.2.3  1996/01/24  04:17:21  gstovall
 * Branch: ELC_branch
 * Pull in some bug fixes from the 11.1 mainline.
 *
 * Revision 3.3.2.2  1996/01/16  02:36:19  dcarroll
 * Branch: ELC_branch
 * Sync ELC_branch between ELC_baseline_120995 and ELC_baseline_960110.
 *
 * Revision 3.3.2.1  1995/12/11  19:24:52  mbeesley
 * Branch: ELC_branch
 * Sync ELC_branch to ELC_baseline_120995.
 *
 * Revision 3.15  1996/03/09  03:18:36  kao
 * CSCdi51172:  RVIP crashes when PA bay 0 is empty
 *
 * Revision 3.14  1996/03/08  23:20:23  kao
 * CSCdi50763:  TRIP OIR causes mic rel under traffic thru VIP
 *
 * Revision 3.13  1996/03/06  01:52:08  jwelder
 * CSCdi50800:  Show Diagbus changed to show VIP1 Dram Size
 *
 * Revision 3.12  1996/03/04  07:14:41  kao
 * CSCdi49674:  While VIP is cofigured with DFS, OIR event will crash the
 * VIP;
 *
 * Revision 3.11  1996/02/18  19:42:45  kao
 * CSCdi45136:  EOIR Problems with VIP card
 *
 * Revision 3.10  1996/01/24  00:27:52  dlobete
 * CSCdi47213:  IP DFS statistics non-functional under traffic load
 * Don't rely on processes running on VIP - send up DFS statistics
 * when interface statistics are passed up (i.e. at interrupt level).
 *
 * Revision 3.9.2.1  1996/01/24  23:27:22  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.9  1996/01/20  05:01:09  amcrae
 * CSCdi47215:  4T exception processing doesnt signal host
 * CSCdi44907:  Fixed macro for love letter stats overlooked in previous
 * fix.
 *
 * Revision 3.8  1996/01/19  03:03:36  dlobete
 * CSCdi44954:  Local-switching for DFS required
 * But that's not all! Also part of this jumbo Distributed IP
 * Switching commit:
 * - CSCdi43302: Illegal Access to Low Address crashes RVIP/4E (RSP2)
 * - DFS config which mirrors RSP route-cache setting
 *         (ie configurable distributed flow or optimal switching)
 * - Interrupt level IPC updates and invalidates for DFS cache operations
 * - Stay in closer synch with RSP cache for prefix invalidations
 *
 * Revision 3.7  1995/12/26  23:11:40  dlobete
 * CSCdi42282:  chassis mib incomplete for VIP
 * Add support for PA MIB information to be relayed from VIP to RP/RSP
 * via love-letter.
 * Rework some of the 4T PA's data structures as a result of above.
 *
 * Revision 3.6  1995/12/23  01:18:20  rharagan
 * Delete extraneous datagram_done() call after vip_rsp_send_message()
 * added as part of CSCdi44539 fix.
 *
 * Revision 3.5  1995/12/05  02:27:28  sthormod
 * CSCdi44940:  Loveletters broken for 4T and 4R
 * Don't dereference ether subblock for 4T and 4R interfaces.
 *
 * Revision 3.4  1995/12/01  18:25:48  hwang
 * If memd transfer fails do datagram_done to free the pak.
 * CSCdi44539:  vip_send_loveletter will lose pak if pak_to host function
 * fails
 *
 * Revision 3.3  1995/11/17  18:52:08  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:33:49  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:29:59  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:29:46  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  22:59:51  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Include files
 */
#include "master.h"
#include <ciscolib.h>
#include "interface_private.h"
#include "sys_registry.h"
#include "packet.h"
#include "logger.h"
#include "ttysrv.h"

#include "../os/pool.h"
#include "../os/particle.h"
#include "../os/buffers.h"
#include "../os/buffers_inline.h"

#include "../pas/if_pas.h"
#include "../hes/ccb_cmd.h"
#include "../hes/if_vip.h"
#include "../hes/dgbus.h"
#include "../hes/if_fci.h"

#include "../dev/monitor1.h"

#include "../if/network.h"
#include "../if/ether.h"
#include "../if/ether_private.h"
#include "../if/fddi_private.h"

#include "vip_memd.h"
#include "rsp_memd.h"
#include "vip_dma.h"
#include "msg_vip.c"			/* Not a typo, see logger.h */
#include "vip_debug.h"
#include "vip_dbus.h"
#include "dfs_vip_idb.h"

#include "../ipc/ipc_util.h"
#include "../ipc/ipc.h"
#include "../ipc/ipc_private.h"
#include "../ipc/ipc_message.h"
#include "../ipc/ipc_seats.h"
#include "../ipc/ipc_ports.h"
#include "../ipc/ipc_cbus_vip.h"

/*
 * Local storage
 */
malu_q_t malu;			/* posted malu attention command queue */
vsys_vector_t vsys_vec;		/* VIP system vectors, variables */
memd_cntl_t memd_cntl;		/* MEMD control block */

/*
 * VIP control flag.
 * Currently only VIP_QUIESCE_FLAG (0x1) is defined
 */
int vip_control_flags;
sys_timestamp reload_timeout;
 

/*
 * Buffer header buffer pointer cache.
 * In order to minimize reads of the memd buffer pointers stored in memd 
 * buffer headers, we keep a cache of those pointers.
 */
uchar *(vip_bhcache[MAX_BUFFER_HEADERS]);
volatile ushort *(vip_rsp_txcached_q[MAX_BUFFER_HEADERS]);

vip_rpsp_icb_t *(vip_rpsp_icb_array[MAX_VIP_INTERFACES+1]);
vip_memd_ifcntl_t vip_ifcntl_array[MAX_VIP_INTERFACES+1];

/*
 * Love letter encapsulations
 */
static ushort vip_ether_encaps[] = {
    0xffff, 0xffff, 0xffff, 0x0000, 0x000c, 0x000c, 0xbead
};

static ushort vip_tokenring_encaps[] = {
    0x0000, 0xffff, 0xffff, 0xffff, 0x0000, 0x000c, 0x000c,
    0x0000, 0x0300, 0x0000, 0xbead 
};

static ushort vip_serial_encaps[] = {
    0xffff, 0xbead};

/*
 * vip_memd_return_periodic:
 * Periodically return buffer headers. Only runs on an RP/SP.
 */
static int return_counter;

void vip_memd_return_periodic (void)
{
    leveltype level;
    pas_cbtype *pascb;
    vip_bhdr_t *bh;
    int i;

    if (!RPSP_SELECT) {
	return;
    }
    
    /*
     * Check whether VIP is in QUIESCE mode
     */
    if (is_cbus_quiesced())
	return;

    /*
     * Rate limit ourselves, only do it every 10 seconds
     */
    if (return_counter++ < 10) {
	return;
    }
    return_counter = 0;
    
    /*
     * Go thru all the interfaces, returning buffers
     */
    for (i=0; i<(MAX_PA_BAYS*MAX_INTERFACES_PER_PA); i++) {
	pascb = memd_cntl.pascb_table[i];
	if (pascb) {
	    /*
	     * Interface exists, give back a buffer
	     */
	    level = raise_interrupt_level(ALL_DISABLE);
	    bh = vip_rpsp_local_freeq_fetch(pascb->vip_memd_ifcntl);
	    if (bh) {
		vip_rpsp_return_rx_bh(bh);
	    }
	    reset_interrupt_level(level);
	}
    }
}

/*
 * vip_init_ipc:
 * Initialize VIP IPC
 */
void vip_init_ipc (void)
{
    if (!ipc_do_vip_init()) {
	buginf("VIP: IPC init failed\n");
    }
}

/*
 * vip_init_memd_support:
 * This is vip generic memd suport initialization
 */
void vip_init_memd_support (boolean is_viper)
{
    int i;
    ushort temp;

    malu.read = 0;
    malu.write = 0;
    temp = 0;

    /* 
     * initialize array of buffer header buffer pointers
     */
    for (i = 0; i < (MAX_BUFFER_HEADERS); i++){
        vip_bhcache[i] = 0;
	vip_rsp_txcached_q[i] = NULL;
    }

    /*
     * initialize memd_cntl, logical slot and lovenote/letter counters
     */
    for (i=0; i < MAX_PA_BAYS * MAX_INTERFACES_PER_PA; i++) {
	memd_cntl.pascb_table[i] = 0;
	memd_cntl.if_fcitype[i] = 0;
    }
    vsys_vec.vip_slot = vip_logical_slot();
    vsys_vec.lovenote_count = 0;
    vsys_vec.loveletter_count = 0;
    vsys_vec.viper = is_viper;
    
    /*
     * Setup the last_sync_value
     */
    memd_cntl.last_sync_value = 0;

    /*
     * Setup the sw/hw version numbers
     */
    memd_cntl.sw_ver_done = (vip_sw_version() << 16) + CCB_DONE_OK;
    memd_cntl.vpld_ver_done = (vip_vpld_version() << 16) + CCB_DONE_OK;
    memd_cntl.rom_ver_done = (vip_rom_version() << 16) + CCB_DONE_OK;

    /*
     * Setup the maximum love letter size 
     */
    memd_cntl.max_lv_ltr_done =	(VIP_MAX_LV_LTR << 16) + CCB_DONE_OK;

    /*
     * Initialize rpsp/rsp dependent memd support. We read the SP bufferin
     * location. On an RP/SP host, this should be 0x8100. On an RSP,
     * it should not be this value.
     *
     * Direct reads of MEMD from SVIP less than 4 bytes should not be done
     * due to a cya hw bug.  Do NOT call vip_memdread16() in case of SVIP.
     * As SVIPs are only compatible with RSPs, initialize accordingly.
     */
    if (!is_viper) {
        vip_memdread16((ushort *)(ADRSPC_CBUS+VIP_BUFFERIN_OFFSET), &temp);
	if (temp == VIP_SP_BUFFERIN_SUPPORT) {
	    vsys_vec.rpsp_host = TRUE;
	    vip_rpsp_init_memd_support();
	} else {
	    vsys_vec.rpsp_host = FALSE;
	    vip_rsp_init_memd_support();
	}
    } else {
        vsys_vec.rpsp_host = FALSE;
	vip_rsp_init_memd_support();
    }
}

/*
 * vip_unsupported_cmd:
 * Provide common way to bitch about unsupported commands
 */
void vip_unsupported_cmd (malu_cmd_t *cmd)
{
    memd_cntl.errors += 1;
    errmsg(&msgsym(BADMALUCMD, VIP), cmd->cmd, cmd->arg0, cmd->pascb);
}

/*
 * install_cmd_dispatch :
 * Register a devices callback routine for handling queued
 * malu_attention commands
 */
void install_cmd_dispatch (hwidbtype *idb, cmd_dispatch_t cmd_dispatch)
{
    pas_cbtype *pascb = INSTANCE(idb);
    leveltype level;

    GLOBAL_INTERRUPT_BLOCK(level);
    vsys_vec.cmd_dispatch[pascb->pa_ifindex] = cmd_dispatch;
    GLOBAL_INTERRUPT_UNBLOCK(level);
}

void init_ifc_queues_caches (vip_memd_ifcntl_t *ifc)
{
    int i;

    if (RPSP_SELECT) {
	ifc->icb_ptr->icb_rpsp_txque = 0;
	ifc->icb_ptr_memd = RPSP_VIRTUAL2MEMD(ifc->icb_ptr);
	ifc->rpsp_local_free_q = NULL;
	ifc->rpsp_local_freeq_head = NULL;
	ifc->rpsp_scraped_txq = NULL;
	ifc->rpsp_loveq = NULL;
    } else {
	ifc->rsp_local_free_q = NULL;
	ifc->rsp_global_free_q = NULL;
	ifc->rsp_tx_q = NULL;
	ifc->rsp_tx_acc = NULL;
    }

    /*
     * Only bh_stack[0] is used for now. bh_stack_top is not used either.
     * Need to generalized it to avoid future problem
     */
    ifc->bh_stack_top = 0;
    for (i=0; i<BH_PARK_LIMIT; i++)
	ifc->bh_stack[i] = NULL;
}

/*
 * vip_null_vector:
 * Null switching vector to throw away the packet
 */
static boolean vip_null_vector (hwidbtype *idb, pas_cbtype *pascb, paktype *pak)
{
    return(FALSE);
}

/*
 * vip_quiesce_one_interface:
 * VIP-dependent routine to quiesce each interface
 */
void vip_quiesce_one_interface (pas_cbtype *pascb)
{
    pascb->pak_dfs = (pak_dfs_t)vip_null_vector;
    pascb->pak_to_host = vip_null_vector;
    pascb->pak_switch = (pak_switch_t)vip_null_vector;
}

/*
 * vip_unquiesce_one_interface:
 * VIP-dependent routine for each interface to recover from quiesced
 */
void vip_unquiesce_one_interface (pas_cbtype *pascb)
{
    vip_pak_vector_init(pascb);
}

/*
 * quiesce_all_interfaces:
 * Quiesce all the interfaces on the VIP card
 */
static void quiesce_all_interfaces (void)
{
    leveltype level;
    int ix;
    pas_cbtype *pascb;
    vip_memd_ifcntl_t *ifc;

    level = raise_interrupt_level(ALL_DISABLE);

    if (!is_cbus_quiesced()) {
	vip_quiesce_all_interfaces();
	set_cbus_quiesced();

	/*
	 * Enter here once per QUIESCE mode
	 */
	dfs_vip_quiesce();
	ipc_quiesce();

	for (ix = 0; ix < (MAX_BUFFER_HEADERS); ix++){
	    vip_bhcache[ix] = 0;
	    vip_rsp_txcached_q[ix] = NULL;
	}

	if (RPSP_SELECT) {
	    vip_rpsp_init_memd_support();
	} else {
	    vsys_vec.rsp_global_raw_q = NULL;
	    vsys_vec.rsp_event_response_q = NULL;
	    vsys_vec.rsp_event_free_q = NULL;
	    vsys_vec.rsp_loveletter_free_q = NULL;
	    vsys_vec.rsp_ipc_free_q = NULL;
	    vsys_vec.rsp_ipc_rx_q = NULL;
	    vsys_vec.rsp_ipc_tx_q = NULL;
	}

	for (ix = 0; ix < MAX_VIP_INTERFACES; ix++)
	    memd_cntl.rx_count[ix] = 0;
    }

    for (ix = 0; ix < MAX_VIP_INTERFACES; ix++) {

	/*
	 * Make sure we have this interface
	 */
	pascb = memd_cntl.pascb_table[ix];
	if (!pascb)
	    continue;

	ifc = pascb->vip_memd_ifcntl;
	if (ifc->vip_if_status == VIP_QUIESCE)
	    continue;

	/*
	 * save vip status so that we can restore it when we get the
	 * unquiesce command
	 */
	ifc->old_vip_if_status = ifc->vip_if_status;
	ifc->vip_if_status = VIP_QUIESCE;
	vip_disable_tx_polling(pascb);
	init_ifc_queues_caches(ifc);
	vip_quiesce_one_interface(pascb);
    }

    reset_interrupt_level(level);
}

/*
 * unquiesce_all_interfaces:
 * Unquiesce all the interfaces on the VIP card
 */
static void unquiesce_all_interfaces(void)
{
    leveltype level;
    int ix;
    pas_cbtype *pascb;
    vip_memd_ifcntl_t *ifc;

    level = raise_interrupt_level(ALL_DISABLE);

    if (is_cbus_quiesced()) {
	clr_cbus_quiesced();
	vip_unquiesce_all_interfaces();
	ipc_unquiesce();
    }

    for (ix = 0; ix < MAX_VIP_INTERFACES; ix++) {

	/*
	 * Make sure we have this interface
	 */
	pascb = memd_cntl.pascb_table[ix];
	if (!pascb)
	    continue;

	ifc = pascb->vip_memd_ifcntl;
	if (ifc->vip_if_status != VIP_QUIESCE)
	    continue;

	/*
	 * Restore pre-quiesce status
	 */
	ifc->vip_if_status = ifc->old_vip_if_status;
	vip_enable_tx_polling(pascb, NULL, 0);
	vip_unquiesce_one_interface(pascb);
    }

    reset_interrupt_level(level);
}

/*
 * vip_control_interface :
 * Handle CCB_CMD_STATUS. Gets called from the device driver dispatch routine.
 */
void vip_control_interface (hwidbtype *idb, malu_cmd_t *cmd)
{
    pas_cbtype *pascb = INSTANCE(idb);
    vip_memd_ifcntl_t *ifc = pascb->vip_memd_ifcntl;
    int newstate;

    switch (cmd->arg0) {

    case CCB_STATUS_IF_DISABLE:
	ifc->vip_if_status = VIP_DISABLE;

	/*
	 * Bring the interface to admin down state.
	 */
	net_cstate(idb, IDBS_ADMINDOWN);
	if (idb->shutdown) {
	    (*idb->shutdown)(idb, IDBS_ADMINDOWN);
	}

	/* 
	 * Do platform dependent memd cleanup 
	 */
	if (RPSP_SELECT) {
	    vip_disable_tx_polling(pascb);
#if 0
	    vip_rpsp_memd_disable_cleanup(pascb);
#endif
	} else {
	    vip_disable_tx_polling(pascb);
	    vip_rsp_memd_disable_cleanup(pascb);
	}
	break;

    case CCB_STATUS_IF_ENABLE:
	ifc->vip_if_status = VIP_ENABLE;
	vip_enable_tx_polling(pascb, NULL, 0);

	/*
	 * Bring the interface to up state.
	 * Serial and TR make their own decisions about being up or down.
	 */
	if (idb->status & (IDB_SERIAL|IDB_TR|IDB_FDDI)) {
	    newstate = IDBS_DOWN;
	} else {
	    newstate = IDBS_UP;
	}

	/*
	 * Cause a state transition. Then issue a hardware reset.
	 */
	net_cstate(idb, newstate);
	(*(idb->reset))(idb);
	break;

    case CCB_STATUS_IF_QUIESCE:
	quiesce_all_interfaces();
	break;

    case CCB_STATUS_IF_UNQUIESCE:
	unquiesce_all_interfaces();
	break;
    }
}

/*
 * vip_memd_return_buffer:
 * Return MEMD buffers. Not supported on RP/SP systems
 */
static boolean vip_memd_return_buffer (pas_cbtype *pascb)
{
    vip_memd_ifcntl_t *ifc;
    vip_bhdr_t *bh;

    if (RPSP_SELECT) {
	return(FALSE);
    }
    if (memd_cntl.rx_count[pascb->pa_ifindex] > 0) {
	ifc = pascb->vip_memd_ifcntl;
	bh = vip_rsp_memd_readbh(ifc->rsp_local_free_q);
	if (bh) {
	    vip_rsp_qa_enqueue(ifc->rsp_global_free_q, bh);
	    memd_cntl.rx_count[pascb->pa_ifindex] -= 1;
	}
    }
    return(TRUE);
}

/*
 * vip_set_rx_limits:
 * Setup Rx limits. Not supported on RP/SP systems
 */
static boolean vip_set_rx_limits (malu_cmd_t *cmd)
{
    vip_memd_ifcntl_t *ifc;

    if (RPSP_SELECT) {
	return(FALSE);
    }
    ifc = cmd->pascb->vip_memd_ifcntl;
    ifc->rx_limits_low = cmd->arg0;
    ifc->rx_limits_high = cmd->arg1;
    return(TRUE);
}

/*
 * vip_set_rawq_pointer:
 * Setup raw Q pointer. Not supported on RP/SP systems
 */
static boolean vip_set_rawq_pointer (malu_cmd_t *cmd)
{
    if (RPSP_SELECT) {
	return(FALSE);
    }
    vsys_vec.rsp_global_raw_q = 
	(volatile ulong *)(RSP_MEMD2VIRTUAL(cmd->arg0));
    return(TRUE);
}

/*
 * vip_setup_freeq:
 * Setup a free Q. Supported on both RP/SP and RSP systems
 */
static void vip_setup_freeq (malu_cmd_t *cmd)
{
    vip_memd_ifcntl_t *ifc;

    ifc = cmd->pascb->vip_memd_ifcntl;
    if (RPSP_SELECT) {
	/*
	 * RP/SP system
	 */
	ifc->rpsp_local_free_q = (vip_bhdr_t *)(RPSP_MEMD2VIRTUAL(cmd->arg0));
    } else {
	/*
	 * RSP system
	 */
	ifc->rsp_local_free_q = 
	    (volatile ulong *)(RSP_MEMD2VIRTUAL(cmd->arg0));
	ifc->rsp_global_free_q = 
	    (volatile ulong *)(RSP_MEMD2VIRTUAL(cmd->arg1));
    }
}

/*
 * vip_setup_loveq:
 * Setup a love Q. Supported on both RP/SP and RSP systems
 */
static void vip_setup_loveq (malu_cmd_t *cmd)
{
    vip_memd_ifcntl_t *ifc;

    if (RPSP_SELECT) {
	/*
	 * RP/SP system
	 */
	ifc = cmd->pascb->vip_memd_ifcntl;
	ifc->rpsp_loveq = (vip_bhdr_t *)(RPSP_MEMD2VIRTUAL(cmd->arg0));
    } else {
	/*
	 * RSP system
	 */
	vsys_vec.rsp_event_response_q = 
	    (volatile ulong *)(RSP_MEMD2VIRTUAL(cmd->arg0));
	vsys_vec.rsp_event_free_q = 
	    (volatile ulong *)(RSP_MEMD2VIRTUAL(cmd->arg1));
	vsys_vec.rsp_loveletter_free_q = 
	    (volatile ulong *)(RSP_MEMD2VIRTUAL(cmd->arg2));
    }
}

/*
 * vip_setup_rxoffset:
 * Setup rx offset. Supported on both RP/SP and RSP systems
 */
static void vip_setup_rxoffset (malu_cmd_t *cmd)
{
    vip_memd_ifcntl_t *ifc;

    ifc = cmd->pascb->vip_memd_ifcntl;
    if (RPSP_SELECT) {
	/*
	 * RP/SP system
	 */
	ifc->rx_offset = (cmd->arg0 & 0xffff) << 1;
    } else {
	/*
	 * RSP system.
	 */
	ifc->rx_offset = (cmd->arg0 & 0xffff);
    }
}

/*
 * get_one_exist_pascb:
 * Find a surrogate pascb for IPC
 */
pas_cbtype *get_one_exist_pascb (void)
{
    /*
     * Find a surrogate interface. Try bay 0 first, then bay 1.
     */
    if (memd_cntl.pascb_table[0]) {
	return(memd_cntl.pascb_table[0]);
    } else if (memd_cntl.pascb_table[MAX_INTERFACES_PER_PA]) {
	return(memd_cntl.pascb_table[MAX_INTERFACES_PER_PA]);
    } else {
	return(NULL);	/* no surrogate interfaces for rx into memd */
    }
}

/*
 * vip_send_hw_to_host: 
 * Tell the host of our Hardware including DRAM Size (also SRAM on VIP2)
 */
static boolean vip_send_hw_to_host (void)
{
    vip_ll_hw hw_data_ll;
    pas_cbtype *pascb;

    pascb = get_one_exist_pascb();
    if (!pascb)
	return(FALSE);
    
    hw_data_ll.ll_type = VIP_LL_HW;
    hw_data_ll.mem_size = mon_getmemsize();
    if (vsys_vec.viper) {
      hw_data_ll.io_size = mon_get_iosize();
    } else {
      hw_data_ll.io_size = 0;
    }

    return(vip_send_loveletter(pascb, (uchar *)&hw_data_ll, 
		sizeof(vip_ll_hw)));
}    

/*
 * vip_send_sw_to_host: 
 * Tell the host of our Software including IOS version string.
 * 
 * Parse the version information from the ciscoversion string, which 
 * has four lines and looks something like this:
 *
 * Cisco Internetwork Operating System Software 
 * IOS (tm) VIP Software (SVIP-DW-M), Experimental Version 11.1(13717) [cheilek 103]
 * Copyright (c) 1986-1997 by cisco Systems, Inc.
 * Compiled Mon 08-Jul-96 18:15 by cheilek
 *
 * Send up only the second line.
 */
static boolean vip_send_sw_to_host (void)
{
    vip_ll_sw  sw_data_ll;
    pas_cbtype *pascb;
    char       ciscoversion_edit[CW_MAX_DESCR_STRLEN];
    char       *version_start, *version_end;

    pascb = get_one_exist_pascb();
    if (!pascb)
	return(FALSE);
    
    sw_data_ll.ll_type = VIP_LL_SW;
    strncpy (ciscoversion_edit, ciscoversion, sizeof(ciscoversion_edit));
    ciscoversion_edit[sizeof(ciscoversion_edit)-1] = '\0';

    /*
     * Select the second line from the ciscoversion string, that 
     * contains the information we want.
     */
    version_start = strchr(ciscoversion_edit, '\n') + 1;
    if (NULL != version_start) {
	if (NULL != (version_end = strchr(version_start, '\n'))) {
	    *version_end = '\0';
	}
    }

    strncpy (sw_data_ll.ios_version, version_start, 
	     sizeof(sw_data_ll.ios_version));
    sw_data_ll.ios_version[sizeof(sw_data_ll.ios_version)-1] = '\0';

    return(vip_send_loveletter(pascb, (uchar *)&sw_data_ll, 
		sizeof(vip_ll_sw)));
}    

/*
 * vip_send_eeprom_to_host:
 * Tell the host our EEPROM data for display to user.
 */
static boolean vip_send_eeprom_to_host (void)
{
    int i;
    vip_ll_eeprom eeprom_data_ll;
    dgbus_eeprom_t eeprom_data;
    pas_cbtype *pascb;
    charlong temp;
    boolean status;
    int fail=0;

    for(i = 0 ; i < MAX_PA_BAYS; i++) {
	pas_get_eeprom_data(i, (void *)&eeprom_data, sizeof(dgbus_eeprom_t)/2);
	pascb = memd_cntl.pascb_table[i * MAX_INTERFACES_PER_PA];
	if (!pascb)
	    continue;
	
	eeprom_data_ll.ll_type = VIP_LL_EEPROM;
	eeprom_data_ll.eeprom_version = eeprom_data.eeprom_version;
	eeprom_data_ll.controller_type = eeprom_data.controller_type;

	temp.d.byte[0] = temp.d.byte[1] = 0;
	temp.d.byte[2] = eeprom_data.hardware_version_high;
	temp.d.byte[3] = eeprom_data.hardware_version_low;
	eeprom_data_ll.hardware_version = temp.d.lword;

        temp.d.byte[0] = eeprom_data.serial_number_31_24;
        temp.d.byte[1] = eeprom_data.serial_number_23_16;
        temp.d.byte[2] = eeprom_data.serial_number_15_8;
        temp.d.byte[3] = eeprom_data.serial_number_7_0;
	eeprom_data_ll.serial_number = temp.d.lword;

        temp.d.byte[0] = eeprom_data.part_number_31_24;
        temp.d.byte[1] = eeprom_data.part_number_23_16;
        temp.d.byte[2] = eeprom_data.part_number_15_8;
        temp.d.byte[3] = eeprom_data.part_number_7_0;
	eeprom_data_ll.part_number = temp.d.lword;

	eeprom_data_ll.test_history = eeprom_data.test_history;

	temp.d.byte[0] = 0;
        temp.d.byte[1] = eeprom_data.rma_number_23_16;
        temp.d.byte[2] = eeprom_data.rma_number_15_8;
        temp.d.byte[3] = eeprom_data.rma_number_7_0;
	eeprom_data_ll.rma_number = temp.d.lword;

	eeprom_data_ll.board_revision = eeprom_data.board_revision;

        temp.d.byte[0] = 0;
        temp.d.byte[1] = eeprom_data.macaddr[2];
        temp.d.byte[2] = eeprom_data.macaddr[1];
        temp.d.byte[3] = eeprom_data.macaddr[0];
	eeprom_data_ll.macaddr = temp.d.lword;

	eeprom_data_ll.flags = eeprom_data.flags;

	if (vip_debug_memd)
	    buginf("\nVIP EEPROM LOVE-LETTER for bay %d", i);

	status = vip_send_loveletter(pascb, (uchar *)&eeprom_data_ll, 
		sizeof(vip_ll_eeprom));
        if (!status)
	    fail++;
    }
    return(!fail);
}

/*
 * vip_nondriver_malu_cmd:
 * Handle a queued malu_attn command that is not destined for
 * a driver (or that needs to be seen by kernel first)
 */
static void vip_nondriver_malu_cmd (malu_cmd_t *cmd)
{
    PAS_DEBUG(ttyprintf(CONTTY, "vip_malu_cmd: cmd=%d, arg0=%d, "
			"pascb=%#x, bay=%d, intf=%d\n", 
			cmd->cmd, cmd->arg0, cmd->pascb, 
			(cmd->pascb ? cmd->pascb->pa_bay : 0),
			(cmd->pascb ? cmd->pascb->pa_device : 0)));

    /*
     * Process this command
     */
    switch (cmd->cmd) {

    case CCB_CMD_RETURN_BUFFER:        /* 24 */
	if (!vip_memd_return_buffer(cmd->pascb)) {
	    vip_unsupported_cmd(cmd);
	}
	break;

    case CCB_CMD_TX_PTRS:              /* 25 */
	if (!vip_set_tx_pointers(cmd)) {
	    vip_unsupported_cmd(cmd);
	}
	break;

    case CCB_CMD_RX_LIMITS:            /* 26 */
	if (!vip_set_rx_limits(cmd)) {
	    vip_unsupported_cmd(cmd);
	}
	break;

    case CCB_CMD_RAW_QUEUE_PTR:        /* 27 */
	if (!vip_set_rawq_pointer(cmd)) {
	    vip_unsupported_cmd(cmd);
	}
	break;

    case CCB_CMD_FREE_QUEUE_PTRS:      /* 46 */
	vip_setup_freeq(cmd);
	break;

    case CCB_CMD_RXERROR:              /* 47 */
	vip_setup_loveq(cmd);
	break;

    case CCB_CMD_RX_OFFSET:            /* 49 */
	vip_setup_rxoffset(cmd);
	break;
	
    case CCB_CMD_VIP_GET_CONFIG :      /* 120 */
        vip_send_eeprom_to_host();
        vip_send_hw_to_host();
        vip_send_sw_to_host();
	break;
	
    case CCB_CMD_IPC_QUEUE:            /* 129 */
	if (!vip_set_ipc_queues(cmd)) {
	    vip_unsupported_cmd(cmd);
	}
	break;

    case CCB_CMD_IPC_INIT :            /* 130 */
	ipc_do_vip_enable(cmd->arg2 << 16 | cmd->arg3,
			  cmd->arg1 << 16 | cmd->arg0); 
	break;

    default:
	vip_unsupported_cmd(cmd);
	break;
    }
}

/*
 * vip_interface_init:
 * Do common interface initialization for a media interface
 */
void vip_interface_init (hwidbtype *idb)
{
    if (RPSP_SELECT) {
	vip_rpsp_if_init(idb);
    } else {
	vip_rsp_if_init(idb);
    }
}

/*
 * vip_malu_attn_queue_check :
 * Called from the network IO interrupt level. If a command exists
 * in the malu Q, dequeue it and call the driver to execute. Keep
 * doing this until the Q is empty.
 */
void vip_malu_attn_queue_check (void)
{
    malu_cmd_t cmd, *ptr;
    int dintf;
    leveltype level;

    memd_cntl.queue_checked += 1;
    while (TRUE) {
	/*
	 * If read equals write, the queue is empty.
	 * Note, this is interrupt safe as the malu_attn
	 * interrupt handler will never move read (it will
	 * return an error to the RP if there is no space)
	 */
	if (malu.write == malu.read) {
	    break;
	}

	/*
	 * Lock out interrupts, increment the read index,
	 * pick up the fields in the command queue, and re-enable interrupts
	 */
	memd_cntl.processed += 1;
	GLOBAL_INTERRUPT_BLOCK(level);
	malu.read = (malu.read + 1) & (MALU_ATTN_CMD_DEPTH-1);
	ptr = &malu.cmd_q[malu.read];
	cmd.cmd = ptr->cmd;
	cmd.arg0 = ptr->arg0;
	cmd.arg1 = ptr->arg1;
	cmd.arg2 = ptr->arg2;
	cmd.arg3 = ptr->arg3;
	cmd.pascb = ptr->pascb;
	cmd.driver_cmd = ptr->driver_cmd;
	GLOBAL_INTERRUPT_UNBLOCK(level);
	
	/*
	 * Check to see if this is an interface specific
	 * command. If so, call the relevant driver
	 */
	if (cmd.driver_cmd) {
	    memd_cntl.driver_commands += 1;
	    if (!cmd.pascb) {
		memd_cntl.errors += 1;
		errmsg(&msgsym(CMDNOPASCB, VIP), cmd.cmd, cmd.arg0);
		continue;
	    }
	    dintf = cmd.pascb->pa_ifindex;

	    if (vip_debug_cmdq) {
		buginf("VIP(%d): malu cmd/arg0=0x%04x%04x, "
		       "arg1arg2=0x%04x%04x, pascb=%#x\n",
		       dintf, cmd.cmd, cmd.arg0, cmd.arg1, cmd.arg2, 
		       cmd.pascb); 
	    }

	    if (vsys_vec.cmd_dispatch[dintf]) {
		(*vsys_vec.cmd_dispatch[dintf])(&cmd);
	    } else {
		memd_cntl.errors += 1;
		errmsg(&msgsym(NODISPATCH, VIP), cmd.cmd, dintf);
	    }
	} else {
	    if (vip_debug_cmdq) {
		buginf("VIP: malu cmd/arg0=0x%04x%04x, arg1arg2=0x%04x%04x,"
		       " pascb=%#x\n",
		       cmd.cmd, cmd.arg0, cmd.arg1, cmd.arg2, cmd.pascb); 
	    }
	    memd_cntl.sys_commands += 1;
	    vip_nondriver_malu_cmd(&cmd);
	}

    }
}

/*
 * vip_show_cmdq:
 * Display the malu attention command queue
 */
void vip_show_cmdq (parseinfo *csb)
{
    int i;

    automore_enable(NULL);
    printf("\nVIP Malu Attention Command Queue:\n");
    printf("\t%d commands processed (system=%d, driver=%d)\n",
	   memd_cntl.processed, memd_cntl.sys_commands,
	   memd_cntl.driver_commands);
    printf("\t%d command errors, %d def. queue errors, queue_checked=%d\n",
	   memd_cntl.errors, memd_cntl.no_qerr_count, memd_cntl.queue_checked);
    printf("\tmalu.read=%d, malu.write=%d, sync_count=%d\n\n",
	   malu.read, malu.write, memd_cntl.sync_count);
    printf("\tInterface\tpascb_table\tcmd_dispatch\n");
    printf("\t--------------------------------------------\n");
    for (i=0; i<(MAX_PA_BAYS * MAX_INTERFACES_PER_PA); i++) {
	printf("\t%4d\t\t0x%08x\t0x%08x\n",
	       i, memd_cntl.pascb_table[i], vsys_vec.cmd_dispatch[i]);
    }
    automore_disable();
}

/*
 * vip_show_memd:
 * Show info wrt to MEMD buffers, queues etc.
 */
void vip_show_memd (parseinfo *csb)
{
    hwidbtype *idb;
    pas_cbtype *pascb;
    vip_memd_ifcntl_t *ifc;

    /*
     * Print out the vsys_vec structure
     */
    automore_enable(NULL);
    printf("VIP System Vector :\n");

    printf(" vip_slot=%d, bufferin_enabled_sp=%s, rpsp_host=%s, particle_size=%d\n",
	   vsys_vec.vip_slot,
	   (vsys_vec.bufferin_enabled_sp ? "Yes" : "No"),
	   (RPSP_SELECT ? "Yes" : "No"),
           vsys_vec.vip_particle_size);
    printf(" loveletter_count=%d, lovenote_count=%d, vip_bhcache=%#x\n",
	   vsys_vec.loveletter_count, vsys_vec.lovenote_count,
	   &vip_bhcache);

    if (RPSP_SELECT) {
	printf(" rpsp_rx_last_length=%d\n", vsys_vec.rpsp_rx_last_length);
	printf(" rpsp_rx_qtail=%#x, rpsp_rx_qhead=%#x\n",
	       vsys_vec.rpsp_rx_qtail, vsys_vec.rpsp_rx_qhead);
	printf(" rpsp_tx_qtail=%#x, rpsp_tx_qhead=%#x\n",
	       vsys_vec.rpsp_tx_qtail, vsys_vec.rpsp_tx_qhead);
	printf(" rpsp_q_rettail=%#x, rpsp_q_rethead_tail=%#x\n",
	       vsys_vec.rpsp_q_rettail, vsys_vec.rpsp_q_rethead_tail);
	printf(" rpsp_q_rawtail=%#x, rpsp_q_rawhead_tail=%#x\n",
	       vsys_vec.rpsp_q_rawtail, vsys_vec.rpsp_q_rawhead_tail);

    } else {
	printf(" rsp_global_raw_q=%#x, rsp_event_response_q=%#x\n",
	       vsys_vec.rsp_global_raw_q, vsys_vec.rsp_event_response_q);
	printf(" rsp_event_free_q=%#x, rsp_loveletter_free_q=%#x\n",
	       vsys_vec.rsp_event_free_q, vsys_vec.rsp_loveletter_free_q);
	printf(" rsp_ipc_free_q=%#x, rsp_ipc_rx_q=%#x, rsp_ipc_tx_q=%#x\n",
	       vsys_vec.rsp_ipc_free_q, vsys_vec.rsp_ipc_rx_q,
	       vsys_vec.rsp_ipc_tx_q);
    }

    /*
     * Print out all the interface information
     */
    FOR_ALL_HWIDBS(idb) {

	pascb = INSTANCE(idb);
	ifc = pascb->vip_memd_ifcntl;

	printf("\nInterface %s :\n", idb->hw_namestring);
	printf(" pascb=%#x, vip_memd_ifcntl=%#x\n", pascb, ifc);
	printf(" pa_ifindex=%d, pa_bay=%d, pa_device=%d\n",
	       pascb->pa_ifindex, pascb->pa_bay, pascb->pa_device);
	printf(" pak_to_host=%#x, pas_deallocate=%#x\n",
	       pascb->pak_to_host, pascb->pas_deallocate);
	
	if (!ifc) {
	    continue;
	}

	printf(" vip_if_status=%#x, old_vip_if_status=%#x, rx_offset=%d, "
	       "bh_stack_top=%d\n",
	       ifc->vip_if_status, ifc->old_vip_if_status, ifc->rx_offset,
	       ifc->bh_stack_top);

	if (RPSP_SELECT) {
	    printf(" rpsp_local_free_q=%#x, rpsp_local_freeq_head=%#x\n",
		   ifc->rpsp_local_free_q, ifc->rpsp_local_freeq_head);
	    printf(" rpsp_scraped_txq=%#x, rpsp_loveq=%#x, icb_ptr=%#x\n",
		   ifc->rpsp_scraped_txq, ifc->rpsp_loveq, ifc->icb_ptr);
	    printf(" icb_ptr_memd=%#x\n", ifc->icb_ptr_memd);
	} else {
	    printf(" rsp_local_free_q=%#x, rsp_global_free_q=%#x\n",
		   ifc->rsp_local_free_q, ifc->rsp_global_free_q);
	    printf(" rsp_tx_q=%#x, rsp_tx_acc=%#x\n",
		   ifc->rsp_tx_q, ifc->rsp_tx_acc); 
	    printf(" rx_count=%d, rx_high=%d, rx_low=%d\n", 
		   *ifc->rx_count_ptr, ifc->rx_limits_high, 
		   ifc->rx_limits_low);
	}
    }
    automore_disable();
}

/*
 * vip_message2buffer:
 * Copy a message into a buffer
 */
paktype *vip_message2buffer (hwidbtype *idb, uchar *buf, ushort length)
{
    paktype *pak;
    particletype *mp = NULL;
    leveltype level;

    /*
     * If we are on a VIP1, we can fire ahead and use contigous
     * buffers for messages upto our host
     */
    if (!vsys_vec.viper) {
	pak = getbuffer(length + ENCAPBYTES);
	if (pak) {
	    bcopy(buf, pak->datagramstart, length);
	    pak->datagramsize = length;
	}
	return(pak);
    }

    /*
     * We are on a VIP2. We must use particles for any and
     * all messages going up to our host
     */
    pak = pool_getbuffer(idb->headerpool);
    if (!pak) {
        return(NULL);
    }
 
    pak->datagramsize = 0;

    while (length) {

	/*
	 * Get a particle, try the buffer cache first
	 */
        level = raise_interrupt_level(ALL_DISABLE);
	mp = pool_dequeue_cache(idb->pool);
        reset_interrupt_level(level);
	
	/*
	 * If we did not get one from the cache, try a global pool
	 */
	if (!mp) {
	    mp = pool_getparticle(idb->pool->fallback);
	}

	/*
	 * If we still do not have one, clean up and return
	 */
	if (!mp) {
	    datagram_done(pak);
	    return(NULL);
	}
	
	/*
	 * Initialize start
	 */
	mp->data_start = particle_center(mp);
	mp->data_bytes = min(idb->pool->size, length);
	bcopy(buf, mp->data_start, mp->data_bytes);
	buf += mp->data_bytes;
	length -= mp->data_bytes;

	/*
	 * Add the particle to the pak
	 */
	particle_enqueue_inline(pak, mp);
    }
    return(pak);
}

/*
 * vip_encapsulate_message:
 * Add on magic encaps to a buffer so it goes thru to the RP/RSP
 */
static boolean vip_encapsulate_message (hwidbtype *idb, 
					paktype *pak, 
					ushort encap_magic)
{
    ushort *old_encap_ptr, *encap;
    ushort encap_size;

    if (idb->status & IDB_ETHER) {
	/*
	 * Ethernet encaps
	 */
	encap_size = sizeof(vip_ether_encaps);
	encap = vip_ether_encaps;
    } else if (idb->status & IDB_TR) {
	/*
	 * Token ring encaps
	 */
	encap_size = sizeof(vip_tokenring_encaps);
	encap = vip_tokenring_encaps;
    } else if (idb->status & IDB_SERIAL) {
        /*
         *  Serial encaps
         */
        encap_size = sizeof(vip_serial_encaps);
        encap = vip_serial_encaps;
    } else {

	errmsg(&msgsym(UNDEFIDBTYPE,VIP), idb->type);
	return(FALSE);
    }

    /*
     * Copy in the encaps and the encap_magic
     */
    old_encap_ptr = (ushort *)(pak->datagramstart - sizeof(ushort));
    pak->datagramstart -= encap_size;
    pak->datagramsize += encap_size;
    bcopy(encap, pak->datagramstart, encap_size);
    PUTSHORT(old_encap_ptr, encap_magic);

    if (pak_particle_count(pak)) {
	/* 
	 * Scatter gather packet - have to adjust first particle info 
	 */
	 particle_clip_inline(pak->particlequeue.qhead, encap_size);
     }

    return(TRUE);
}

int errorreadbh;
/*
 * vip_rsp_send_message :
 * Send a message up to our host. This is used to send traditional
 * love letters as well as IPC messages. RSP host.
 */
static boolean vip_rsp_send_message (hwidbtype *idb, paktype *pak,
				     volatile ulong *freeq,
				     volatile ulong *responseq)
{
    pas_cbtype *pascb = INSTANCE(idb);
    vip_memd_ifcntl_t *ifc = pascb->vip_memd_ifcntl;
    leveltype level;
    vip_bhdr_t *bh;
    particletype *mp;

    if (is_cbus_quiesced())
	return(FALSE);

    if (!freeq) {
	errorreadbh++;
	return(FALSE);
    }
    /*
     * Make sure the packet is min size
     */
    if (pak->datagramsize < 64) {
	pak->datagramsize = 64;

	if (pak_particle_count(pak)) {
	    /* 
	     * Scatter gather packet - have to adjust first particle info 
	     */
	    mp = pak->particlequeue.qhead;
	    mp->data_bytes = 64;
	}
    }

    /*
     * Attempt to get a buffer header from the freeq
     */
    level = raise_interrupt_level(ALL_DISABLE);
    bh = vip_rsp_memd_readbh(freeq);
    if (!bh) {
	reset_interrupt_level(level);
	datagram_done(pak);
	return(FALSE);
    }

    /*
     * Fill in the bh fields. Note, upper 16bits of event code are zero
     */
    bh->bf_rsp_evnt_code_and_id = RSP_VIRTUAL2MEMD(ifc->rsp_local_free_q);
    bh->bf_rsp_evnt_len_and_fq = (pak->datagramsize << 16) |
	(RSP_VIRTUAL2MEMD(freeq));
    
    /*
     * Send the message.
     */
    (*pascb->pak_switch)(ifc, pak, bh, responseq);

    /*
     * If we are running on a VIP2, then we are responsible
     * for returning the paktype header, as the message was
     * in particles in a scattered buffer.
     */
    if (vsys_vec.viper) {
        datagram_done(pak);
    }

    reset_interrupt_level(level);
    return(TRUE);
}


/*
 * vip_rpsp_send_message :
 * Send a message up to our host. This is used to send traditional
 * love letters as well as IPC messages. RPSP host.
 */
static boolean vip_rpsp_send_message (hwidbtype *idb, paktype *pak)
{
    pas_cbtype *pascb = INSTANCE(idb);
    leveltype level;
    boolean temp;

    /*
     * Make sure the packet is min size and set mci_status so the SP
     * will faithfully forward this frame to the RP.
     */
    if (pak->datagramsize < 64) {
	pak->datagramsize = 64;
    }
    pak->mci_status = (FCI_MULTICAST_MASK << 8);

    level = raise_interrupt_level(ALL_DISABLE);
    temp = (*(pascb->pak_to_host))(idb, pascb, pak);
    if (!temp) {
	/* message failed */
	datagram_done(pak);
    }
    reset_interrupt_level(level);
    return(temp);
}

/*
 * vip_send_ipc_message:
 * Send an IPC message up to the RP or RSP
 */
boolean vip_send_ipc_message (pas_cbtype *pascb, uchar *buf, ushort bytes)
{
    hwidbtype *idb = pascb->idb;
    paktype *pak;

    /*
     * Copy the message into a buffer and send it.
     */
    if (RPSP_SELECT) {
	/*
	 * On RP/SP systems, we must encapsulate the message
	 */
	pak = vip_message2buffer(idb, buf, bytes);
	if (!pak) {
	    return(FALSE);
	}
	if (!vip_encapsulate_message(idb, pak, 0xbead)) {
	    datagram_done(pak);
	    return(FALSE);
	}
	return(vip_rpsp_send_message(idb, pak));
    } else {
	pak = vip_message2buffer(idb, buf, bytes);
	if (!pak) {
	    return(FALSE);
	}
	return(vip_rsp_send_message(idb, pak, vsys_vec.rsp_ipc_free_q,
				    vsys_vec.rsp_ipc_rx_q));
    }
}

/*
 * vip_send_lovenote:
 * Send a lovenote up to the RP or RSP
 */
boolean vip_send_lovenote (pas_cbtype *pascb, ushort love_error)
{
    leveltype level;
    boolean temp;

    level = raise_interrupt_level(ALL_DISABLE);
    if (RPSP_SELECT) {
	temp = vip_rpsp_send_lovenote(pascb, love_error);
    } else {
	temp = vip_rsp_send_lovenote(pascb, love_error);
    }
    reset_interrupt_level(level);
    vsys_vec.lovenote_count += 1;
    return(temp);
}

/*
 * vip_send_loveletter:
 * Send a love letter up to the RP or RSP
 */
boolean vip_send_loveletter (pas_cbtype *pascb, uchar *buf, ushort bytes)
{
    hwidbtype *idb = pascb->idb;
    paktype *pak;

    /*
     * Increment message count
     */
    vsys_vec.loveletter_count += 1;

    /*
     * Send the message
     */
    if (RPSP_SELECT) {
	/*
	 * On RP/SP systems we must encapsulate the message
	 */
	pak = vip_message2buffer(idb, buf, bytes);
	if (!pak) {
	    return(FALSE);
	}

	if (!vip_encapsulate_message(idb, pak, 0xdead)) {
	    datagram_done(pak);
	    return(FALSE);
	}
	if (!vip_rpsp_send_message(idb, pak)) {
	    return(FALSE);
	} else {
	    return(TRUE);
	}
    } else {
	/*
	 * On RSP systems, check to make sure the message does not
	 * exceed the max length
	 */
	if (bytes > VIP_MAX_LV_LTR) {
	    if (vip_debug_memd) {
		buginf("VIP: loveletter length exceeds max, %d bytes\n",
		       bytes);
	    }
	    return(FALSE);
	}
	pak = vip_message2buffer(idb, buf, bytes);
	if (!pak) {
	    return(FALSE);
	}

	if (!vip_rsp_send_message(idb, pak, vsys_vec.rsp_loveletter_free_q,
				  vsys_vec.rsp_event_response_q)) {
	    return(FALSE);
	} else {
	    return(TRUE);
	}
    }
}

/*
 * vip_lineproto_change :
 * Send a love note to the RP/RSP indicating line protocol change.
 * Gets called by the device driver when the line protocol 
 * changes for a VIP interfaces.
 */
void vip_lineproto_change (hwidbtype *hwidb, boolean newstate)
{
    boolean temp;
    pas_cbtype *pascb;
    leveltype level;

    pascb = INSTANCE(hwidb);
    if (!pascb) {
	return;
    }
    if (hwidb->state != IDBS_UP) {
	newstate = FALSE;
    }
    /*
     * Change the carrier state and send the love note
     */
    if (vip_debug_memd) {
	buginf("VIP: Love note for %s, carrier state change to %s\n",
	       hwidb->hw_namestring,
	       newstate ? "up" : "down");
    }

    temp = vip_send_lovenote(pascb, FCI_COND_DCD);
    if (temp) {
	level = raise_interrupt_level(ALL_DISABLE);
	if (newstate) {
	    memd_cntl.if_carrier[pascb->pa_ifindex] = (1 << 16) | CCB_DONE_OK;
	} else {
	    memd_cntl.if_carrier[pascb->pa_ifindex] = CCB_DONE_OK;
	}
	reset_interrupt_level(level);
    } else {
	if(vip_debug_memd) {
	buginf("VIP: Love note for %s failed\n", hwidb->hw_namestring);
	}
    }
}

/*
 * Define data structures and macros to write the errors into the love
 * letter and zero the counters.
 */
static vip_ll vip_istats[MAX_VIP_INTERFACES];
static vip_ll vip_istats_store[MAX_VIP_INTERFACES];

/*
 * Update errors: Basically, an error counter is calculated as follows
 *
 * vip_err_pointer->foo = (vip_err_hwidb->foo - vip_err_store->foo);
 * vip_err_store->foo = (vip_err_hwidb->foo);
 */
#define VIP_ERROR_UPDATE(foo)					\
    {  vip_err_pointer->data. ## foo = 				\
	   vip_err_hwidb->counters. ## foo - vip_err_store->data. ## foo;	\
       vip_err_store->data. ## foo = vip_err_hwidb->counters. ## foo;	\
    }

#define VIP_ERROR_SB_UPDATE(foo)				\
    {  vip_err_pointer->data. ## foo = 				\
	   esb-> ## foo - vip_err_store->data. ## foo;		\
       vip_err_store->data. ## foo = esb-> ## foo;			\
    }

#define VIP_ERROR_FSB_UPDATE(foo)				\
    {  vip_err_pointer->data. ## foo = 				\
	   fsb-> ## foo - vip_err_store->data. ## foo;		\
       vip_err_store->data. ## foo = fsb-> ## foo;			\
    }
/*
 * vip_send_interface_stats :
 * Create a love letter, fill it with interface stats and send it.
 * This routine needs to run at NETS_DISABLE interrupt level.
 */
void vip_send_interface_stats (hwidbtype *hwidb, malu_cmd_t *cmd)
{
    pas_cbtype *pascb;
    hwidbtype *vip_err_hwidb;
    vip_ll *vip_err_pointer, *vip_err_store;
    ethersb	*esb = NULL;
    fddisb	*fsb = NULL;
    int intfc;

    pascb = INSTANCE(hwidb);
    if (!pascb) {
	return;
    }
    intfc = pascb->pa_ifindex;
    vip_err_pointer = &vip_istats[intfc];
    vip_err_store = &vip_istats_store[intfc];
    vip_err_hwidb = hwidb;
    
    /*
     * Suck all the errors from the idb
     */
    VIP_ERROR_UPDATE(output_error);
    VIP_ERROR_UPDATE(output_collision);
    VIP_ERROR_UPDATE(output_underrun);
    VIP_ERROR_UPDATE(input_error);
    VIP_ERROR_UPDATE(runts);
    VIP_ERROR_UPDATE(giants);
    VIP_ERROR_UPDATE(input_frame);
    VIP_ERROR_UPDATE(input_crc);
    VIP_ERROR_UPDATE(input_overrun);
    VIP_ERROR_UPDATE(input_resource);
    VIP_ERROR_UPDATE(input_abort);
    VIP_ERROR_UPDATE(input_drops);

    if (hwidb->status & IDB_ETHER) {
	esb = idb_use_hwsb_inline(hwidb, HWIDB_SB_ETHER);
    }

    switch (hwidb->type) {

    case IDBTYPE_FEIP:
	VIP_ERROR_SB_UPDATE(feip_no_carrier);
	VIP_ERROR_SB_UPDATE(feip_multicast);
	VIP_ERROR_SB_UPDATE(feip_watchdog);
	/* no break, fall through ether */
    case IDBTYPE_AMDP2:
	VIP_ERROR_SB_UPDATE(output_babbles);
	VIP_ERROR_SB_UPDATE(output_lost_carrier);
	VIP_ERROR_SB_UPDATE(output_late_collision);
	VIP_ERROR_SB_UPDATE(output_excessive_collision);
	VIP_ERROR_SB_UPDATE(output_deferred);
	idb_release_hwsb_inline(hwidb, HWIDB_SB_ETHER);
	break;
    default:
	break;
    }

    if (hwidb->status & IDB_FDDI) {
        fsb = idb_use_hwsb_inline(hwidb, HWIDB_SB_FDDI);

	    VIP_ERROR_FSB_UPDATE(fddi_claims);
	    VIP_ERROR_FSB_UPDATE(fddi_beacons);
	    VIP_ERROR_FSB_UPDATE(fddi_trace_count);
    }

    /*
     * Send the love letter
     */
    if (vip_debug_memd) {
	buginf("%s: Sending stats, %d bytes\n", hwidb->hw_namestring,
	       sizeof(vip_ll));
    }

    vip_err_pointer->ll_type = VIP_LL_STATS;
    if (!vip_send_loveletter(pascb, (uchar *)vip_err_pointer, 
			     sizeof(vip_ll))) {
	if (vip_debug_memd) {
	    buginf("%s: failed to send stats love letter\n",
		   hwidb->hw_namestring);
	}
    }

    /*
     * Trigger off interface stats to send DFS stats and do other
     * periodic things that may be starved if run at process level.
     */
    dfs_check_periodic_tasks();
}


/*
 * vip_kernel_ready:
 * Tell the host that we are ready
 */
void vip_kernel_ready (void)
{
    if (RPSP_SELECT) {
	vip_rpsp_kernel_ready();
    } else {
	/*
	 * Don't call vip_rsp_kernel_ready to set DBUS_SW_READY here.
	 * Set DBUS_SW_READY to inform RSP only after VIP finishes
	 * its initialization.
	 */
	process_create(vip_rsp_kernel_ready, "VIP Ready",
	               NORMAL_STACK, PRIO_NORMAL);
    }
}

/*
 * vip_idle_chores:
 * Return any over subscribed Rx buffers (RSP host only)
 */
int errorfreeq;
void vip_idle_chores (void)
{
    vip_memd_ifcntl_t *ifc;
    pas_cbtype *pascb;
    vip_bhdr_t *bh;
    int i;

    /*
     * Do nothing on an RP/SP system
     */
    if (RPSP_SELECT) {
	return;
    }

    /*
     * Check whether VIP is in QUIESCE mode
     */
    if (is_cbus_quiesced())
	return;

    for (i=0; i<MAX_VIP_INTERFACES; i++) {

	/*
	 * Make sure we have this interface
	 */
	pascb = memd_cntl.pascb_table[i];
	if (!pascb) {
	    continue;
	}

	ifc = pascb->vip_memd_ifcntl;
	if (ifc->vip_if_status == VIP_DISABLE) {
	    /*
	     * Return a buffer from the local free queue to the 
	     * global free queue for a disabled interface
	     */
	    if (*ifc->rx_count_ptr > 0) {
		if (!ifc->rsp_local_free_q)
		    continue;
		bh = vip_rsp_memd_readbh(ifc->rsp_local_free_q);
		if (bh) {
		    vip_rsp_qa_enqueue(ifc->rsp_global_free_q, bh);
		    *ifc->rx_count_ptr -= 1;
		}
	    }

	} else {

	    if (*ifc->rx_count_ptr > ifc->rx_limits_low) {
		/*
		 * Return a buffer from the local free queue to the
		 * global free queue for an enabled interface whose
		 * rx buffer count has exceeded the low limit
		 */
		bh = vip_rsp_memd_readbh(ifc->rsp_local_free_q);
		if (bh) {
		    vip_rsp_qa_enqueue(ifc->rsp_global_free_q, bh);
		    *ifc->rx_count_ptr -= 1;
		}
	    }
	}
    }
}

/*
 * timestamp is updated by a high level interrupt handler, r4k_4ms_interrupt,
 * which is blocked under cbus stall
 * 1) in VIP1, CPU is stalled when accessing MEMD during cbus stall
 * 2) in VIP2, cbus stall interrupt level is higher than that of r4k_4ms_interrupt
 */
boolean check_clock_loss (void)
{
    ulong cpu_compare, cpu_count;

    /*
     * read counter register first
     */
    cpu_count = getcp0_count();
    cpu_compare = getcp0_compare();
    if (cpu_compare - cpu_count > 0x7FFFFFFF) {
        leveltype level;
        ulong ms_loss;

        /*
         * If bus stall is longer than 43 seconds, then it is not detected
         */
        level = disable_interrupts();
        cpu_count = getcp0_count();
        cpu_compare = getcp0_compare();
        ms_loss = (cpu_count - cpu_compare) * 4 / r4k_4ms_pipe_ticks + 20;
        r4k_update_timestamp(ms_loss);
        setcp0_count(0);  /* initialize */
        setcp0_compare(MON_TIMER_VAL_20MS);
        enable_interrupts(level);
	return(TRUE);
    }
    return(FALSE);
}

/* end of file */

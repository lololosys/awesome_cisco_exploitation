/* $Id: rsp_memd.c,v 3.4.4.6 1996/08/28 13:15:21 thille Exp $
 * $Source: /release/112/cvs/Xsys/src-vip/rsp_memd.c,v $
 *------------------------------------------------------------------
 * rsp_memd.c : VIP RSP support routines (rvip/svip independent)
 *
 * June 1995, David Getchell
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rsp_memd.c,v $
 * Revision 3.4.4.6  1996/08/28  13:15:21  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.4.4.5  1996/06/03  17:44:35  getchell
 * CSCdi59154:  Spurious memory access with 4R
 * Branch: California_branch
 *
 * Revision 3.4.4.4  1996/05/08  06:09:46  feisal
 * CSCdi45137:  No line down event when 4E/FE removed the first time.
 * Branch: California_branch
 *
 * Revision 3.4.4.3  1996/04/12  17:44:39  kao
 * CSCdi53527:  Need EOIR support on VIP2
 * Branch: California_branch
 *
 * Revision 3.4.4.2  1996/03/21  23:52:12  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.4.4.1  1996/03/18  22:03:41  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.2.3  1996/03/18  23:36:29  mbeesley
 * Branch: ELC_branch
 * Sync to V111_1_3
 *
 * Revision 3.2.2.2  1996/03/07  17:29:57  getchell
 * CSCdi50953:  viper dfs mods
 * Branch: ELC_branch
 *
 * Revision 3.2.2.1  1996/02/10  00:22:23  mbeesley
 * CSCdi48581:  Add VIP2 support to ELC_branch
 * Branch: ELC_branch
 *
 * Revision 3.4  1996/03/08  23:20:19  kao
 * CSCdi50763:  TRIP OIR causes mic rel under traffic thru VIP
 *
 * Revision 3.3  1996/02/18  19:42:42  kao
 * CSCdi45136:  EOIR Problems with VIP card
 *
 * Revision 3.2  1995/11/17  18:51:30  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:29:41  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "interface_private.h"
#include "sys_registry.h"
#include "packet.h"
#include "ttysrv.h"
#include "logger.h"

#include "../pas/if_pas.h"
#include "../pas/msg_pas.c"		/* Not a typo, see logger.h */
#include "../hes/ccb_cmd.h"
#include "../hes/dgbus.h"
#include "../if/ether.h"

#include "vip_memd.h"
#include "vip_dma.h"
#include "vip_dbus.h"
#include "msg_vip.c"			/* Not a typo, see logger.h */
#include "dfs_vip_idb.h"

#if defined(RVIP)
# include "../src-rvip/rvip_dma.h"
#endif /* RVIP */

#if defined(SVIP)
# include "../src-svip/svip_dma.h"
#endif /* SVIP */

/*
 * vip_rsp_if_init:
 * Do rsp specific interface initialization on an RSP platform
 */
void vip_rsp_if_init (hwidbtype *hwidb)
{
    pas_cbtype *pascb;
    vip_memd_ifcntl_t *ifc;
    int intfc;

    pascb = INSTANCE(hwidb);
    intfc = (pascb->pa_bay * MAX_INTERFACES_PER_PA) + pascb->pa_device;

    /*
     * Record pascb in memd data structure
     */
    memd_cntl.pascb_table[intfc] = pascb;
    pascb->pa_ifindex = intfc;
    ifc = &vip_ifcntl_array[intfc];
    pascb->vip_memd_ifcntl = ifc;

    /*
     * Initialize idb mci_index field
     */
    hwidb->mci_index = (vip_logical_slot() * MAX_CTRLR_INTS) + intfc;
    hwidb->encsize = ETHER_ARPA_ENCAPBYTES;

    /*
     * Initialize ifc fields
     */
    ifc->idb = hwidb;

    init_ifc_queues_caches(ifc);

    ifc->vip_if_status = VIP_DISABLE;
    ifc->old_vip_if_status = VIP_DISABLE;

    ifc->rx_offset = 0;
    ifc->rx_count_ptr = &memd_cntl.rx_count[intfc];

    /*
     * Initialize pascb fields
     */
    pascb->idb = hwidb;
    vip_pak_vector_init(pascb);
    
    /* 
     * fci_type used by malu attn interrupt handler 
     */
    memd_cntl.if_fcitype[intfc] = (hwidb->fci_type << 16) | CCB_DONE_OK;
}

/*
 * vip_rsp_memd_disable_cleanup:
 * Clean up memd data structures following a disable command
 * FIXME - this might take forever - is that going to be a problem?
 */
void vip_rsp_memd_disable_cleanup (pas_cbtype *pascb)
{
    vip_memd_ifcntl_t *ifc = pascb->vip_memd_ifcntl;
    volatile ushort *ptr;
    vip_bhdr_t *bh;
    int count = 0;

    if (ifc->rsp_tx_q) {
	while (bh = vip_rsp_memd_readbh(ifc->rsp_tx_q), bh) {
	    ptr = (volatile ushort *)(&bh->bf_rsp_qptr);
	    *ptr = (ushort)(RSP_VIRTUAL2MEMD(bh));
	    count += 1;
	}
    }

    /* 
     * Update the transmit count accumulator 
     */
    if (count) {
	*(ifc->rsp_tx_acc) = (short)(-count);
    }
}

/*
 * vip_rsp_send_lovenote:
 * Send a lovenote up to the RSP.
 */
boolean vip_rsp_send_lovenote (pas_cbtype *pascb, ushort love_error)
{
    vip_bhdr_t *bh;
    vip_memd_ifcntl_t *ifc;

    if (is_cbus_quiesced())
	return(FALSE);

    if (!vsys_vec.rsp_event_free_q) {
	return(FALSE);
    }

    /*
     * Get a buffer from the love note queue
     */
    bh = vip_rsp_memd_readbh(vsys_vec.rsp_event_free_q);
    if (!bh) {
	return(FALSE);
    }
    
    /*
     * Fill in the buffer header fields
     */
    ifc = pascb->vip_memd_ifcntl;
    bh->bf_rsp_evnt_code_and_id =
	(love_error << 16) | (RSP_VIRTUAL2MEMD(ifc->rsp_local_free_q));
    bh->bf_rsp_evnt_len_and_fq = RSP_VIRTUAL2MEMD(vsys_vec.rsp_event_free_q);

    /*
     * Enqueue this buffer header
     */
    vip_rsp_qa_enqueue(vsys_vec.rsp_event_response_q, bh);

    /*
     * Return success
     */
    return(TRUE);
}

/*
 * vip_rsp_get_ccb_ptr:
 * Get the ccb pointer for an RSP system
 */
void vip_rsp_get_ccb_ptr (void)
{
    vip_bhdr_t *bh = NULL;
    volatile ulong *cmd_q;
    int i;

    /*
     * Get queue pointer
     */
    i = vip_logical_slot();
    vsys_vec.vip_slot = i;
    cmd_q = (volatile ulong *)(RSP_CMD_QUEUE(i));

    /*
     * Loop getting our ccb pointer. Note, we treat the ccbptr that we will
     * be getting back from the queue as a buffer header, which it is
     * not. This is kinda sloppy, but the system is geared towards
     * buffer headers. This should probably be cleaned up.
     */
    while (!bh) {
	usecdelay(100);		/* wait 100us */
	bh = vip_rsp_memd_readbh(cmd_q);
    }
    memd_cntl.ccbptr = (ccbtype *)bh;
}

/*
 * vip_rsp_kernel_ready
 * Tell RSP we are ready
 */
void vip_rsp_kernel_ready (void)
{
    /* Set ready bit in dbus register */
    vip_set_ready(TRUE);

    /* Get our ccb pointer from the RSP */
    vip_rsp_get_ccb_ptr();
    process_kill(THIS_PROCESS);
}

/*
 * vip_rsp_init_memd_support:
 * Do any memd initialization necessary for RSP
 */
void vip_rsp_init_memd_support (void)
{
    /*
     * Assign RSP vsys_vec fields
     */
    vsys_vec.rsp_global_raw_q = NULL;
    vsys_vec.rsp_event_response_q = NULL;
    vsys_vec.rsp_event_free_q = NULL;
    vsys_vec.rsp_loveletter_free_q = NULL;
    vsys_vec.rsp_ipc_free_q = NULL;
    vsys_vec.rsp_ipc_rx_q = NULL;
    vsys_vec.rsp_ipc_tx_q = NULL;
}

/* end of file */

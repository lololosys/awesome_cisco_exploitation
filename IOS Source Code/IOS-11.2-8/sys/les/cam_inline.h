/* $Id: cam_inline.h,v 3.2 1995/11/17 17:36:41 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/les/cam_inline.h,v $
 *------------------------------------------------------------------
 * Low-end FDDI cam populate and depopulate inlines.
 *
 * May, 1994, Steve Southam
 *
 * Copyright (c) 1993-1995 by cisco Systems, Inc.
 * All rights reserved.
 *
 * This file is being added to make the low-end CAM architecture look 
 * like the high-end. These inlines are "called" directly by low-end
 * specific code, and are referenced by the generic CAM handling code
 * in srt/bridge.c:bridge_entry_remove() via a registry call.
 *------------------------------------------------------------------
 * $Log: cam_inline.h,v $
 * Revision 3.2  1995/11/17  17:36:41  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:29:34  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:30:46  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

extern boolean fddi_debug, fddi_cmt_events_debug;

/*
 * void les_fddi_cam_populate_inline (hwidbtype *idb, uchar *addr)
 *
 * Taken from what used to be add_cam_entry in les/cam.c
 * Note that there is now no parameter passed for the contents
 * of the RAM segment - this was always NULL anyway as we were
 * not using the RAM segment. This also explains why we do not
 * have to reload the segment counter register, it can be left
 * set to the value the HW requires, ie 1 thru 3, the 48 bits
 * of MAC address only.
 */
static inline
void les_fddi_cam_populate_inline (hwidbtype *idb, uchar *addr)
{
    bsi_instance_t *ds = INSTANCE;
    xx_fddi_regs_t *rp = ds->module_regs;

    /*
     * Make sure back-to-back writes don't cause a bus error
     */
    if (rp->csr & CAM_CSR_FIFO_GO) {
        /*
         * Spin for a bit (pun intended ;-) and try again
         */
        usecdelay(2);

        if (rp->csr & CAM_CSR_FIFO_GO) {
	    if (fddi_debug | fddi_cmt_events_debug) {
		buginf("\nCAM: attempt to add CAM entry timed out");
	    }
            return;
        }
    }

    SPD_CR
    CAM_DATA_WR (*(ushort *)&addr[0])
    CAM_DATA_WR (*(ushort *)&addr[2])
    CAM_DATA_WR (*(ushort *)&addr[4])
    MOV_M_NF_CR_V
    /* Set FIFO GO bit */
    rp->csr |= CAM_CSR_FIFO_GO;
}

/*
 * les_fddi_cam_depopulate_inline()
 * Taken from what used to be fddi_remove_cam_entry in les/cam.c
 */
static inline
void les_fddi_cam_depopulate_inline (hwidbtype *hwidb, uchar *addr)
{
    hwidbtype *idb = hwidb;
    bsi_instance_t *ds = INSTANCE;
    xx_fddi_regs_t *rp = ds->module_regs;
    uchar           baddr[IEEEBYTES];

    /*
     * If we're not learning via the CAM, then we really don't want
     * to start deleting addresses, do we? The entries that'll be
     * in here are added by the system and are effectively "static"
     */
    if (!ds->cam_learning)
        return;

    rp->csr &= ~CAM_CSR_SA_MISS_INTR_EN;
    if (rp->csr & CAM_CSR_FIFO_GO) {
        /*
         * Spin for a bit (pun intended ;-) and try again
         */
        usecdelay(2);

        if (rp->csr & CAM_CSR_FIFO_GO) {
	    if (fddi_debug | fddi_cmt_events_debug) {
		buginf("\nCAM: attempt to add CAM entry timed out");
	    }
            rp->csr |= CAM_CSR_SA_MISS_INTR_EN;
            return;
        }
    }

    /*
     * Change the canonical address to wire-order
     */
    ieee_swap(addr, baddr);

    SPD_CR
    CAM_DATA_WR(*(ushort *)&baddr[0])
    CAM_DATA_WR(*(ushort *)&baddr[2])
    CAM_DATA_WR(*(ushort *)&baddr[4])

    VBC_M_ALM_E
    /* Set FIFO GO bit */
    rp->csr |= CAM_CSR_FIFO_GO;
    while (rp->csr & CAM_CSR_FIFO_GO)
	;
    rp->csr |= CAM_CSR_SA_MISS_INTR_EN;
}

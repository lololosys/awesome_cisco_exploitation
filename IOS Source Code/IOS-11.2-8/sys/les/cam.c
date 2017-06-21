/* $Id: cam.c,v 3.4.44.3 1996/06/26 19:31:29 kew Exp $
 * $Source: /release/112/cvs/Xsys/les/cam.c,v $
 *------------------------------------------------------------------
 * Routines for the Music MU9C1480 CAM for the midrange FDDI cards
 *
 * October 1991, Steve Southam
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * This module contains the driver routines for the MUSIC semiconductors
 * 
 *------------------------------------------------------------------
 * $Log: cam.c,v $
 * Revision 3.4.44.3  1996/06/26  19:31:29  kew
 * CSCdi56241:  sh cont cam hangs the router
 * Branch: California_branch
 * Reorganized a few statements in function print_cam_entry(), so we don't
 * end up in the deadlock situation.
 *
 * Revision 3.4.44.2  1996/03/21  22:52:39  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.4.44.1  1996/03/18  20:40:22  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.2.2  1996/01/15  23:10:41  dcarroll
 * Branch: ELC_branch
 * Sync ELC_branch between ELC_baseline_120995 and ELC_baseline_960110.
 *
 * Revision 3.3.2.1  1995/12/23  04:40:43  fsunaval
 * Branch: ELC_branch
 * 1.First pass at FDDI port adapter.
 *
 * Revision 3.4  1995/12/21  16:56:42  hampton
 * Remove old timer related declarations or obsolete conditionally
 * compiled code.  [CSCdi45965]
 *
 * Revision 3.3  1995/11/17  17:36:37  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:56:56  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:29:31  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/08/08  16:48:00  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi38391]
 *
 * Revision 2.1  1995/06/07 21:30:41  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "interface_private.h"
#include "access.h"
#include "ieee.h"
#include "if_les.h"  
#include "../src-c4000/c4000_bsi.h"
#include "../xdi/xdihdrsif.h"
#include "cam.h"
#include "config.h"
#include "../if/if_fddi.h"

extern ulong bsi_xilinx[];
extern ulong bsi_endxilinx[];

#define fddibuginf if (fddi_debug|fddi_cmt_events_debug) buginf

static int find_cam_entry (xx_fddi_regs_t *rp, ushort hi, ushort mid, short lo)
{
    ushort status_lo = 0;
    ushort status_hi = 0;
    
    CAM_DATA_WR_DIRECT (hi)
    CAM_DATA_WR_DIRECT (mid)
    CAM_DATA_WR_DIRECT (lo)

    status_lo = rp->dcr;
    status_hi = rp->dcr;
    
    if (status_lo & CAM_STATUS_MATCH_FLAG)
	return (FALSE);
    return (TRUE);
}

static boolean print_cam_entry (xx_fddi_regs_t *rp, ushort addr)
{
    mac_address_type cam_data;
    boolean found;
    ushort match_address;
    
    rp->csr &= (ushort)~CAM_CSR_CAM_COMPARE_ENABLE;
    while (rp->csr & CAM_CSR_CAM_COMPARE)
	;
    
    SPS_M_DIRECT(addr)

    cam_data.u.sword[0] = rp->ddr;
    cam_data.u.sword[1] = rp->ddr;
    cam_data.u.sword[2] = rp->ddr;

    if (find_cam_entry(rp, cam_data.u.sword[0], cam_data.u.sword[1],
		       cam_data.u.sword[2])) {
	
	/* Check if the match was at the correct index */
	match_address = (rp->dcr >> 1) & 0x3ff;
        
        /*
         * We're done messing around with the CAM registers so let's 
         * enable the CAM Compare again, so that we don't block the FDDI
         * interface's receive functionality for more time than necessary.
         * And especially we don't want the interface to be "deaf" while
         * we do the printf later on (since the printf could go to
         * a telnet session via the same FDDI interface and create a 
         * deadlock, as was the case with CSCdi56241) 
         */
        SPS_CR_DIRECT
        rp->csr |= CAM_CSR_CAM_COMPARE_ENABLE;

	if (match_address != addr) {
            return (FALSE);
	}

	printf("\n%-04d   %e", addr, &cam_data.u.sword[0]);
	
	ieee_swap(cam_data.u.byte, cam_data.u.byte);
	
	printf("   %e", &cam_data.u.sword[0]);
	
	found = TRUE;
    } else {
	found = FALSE;
        SPS_CR_DIRECT
        rp->csr |= CAM_CSR_CAM_COMPARE_ENABLE;
    }
    
    return (found);
}


static const char *show_cam_header =
                                    "\nIndex  MAC Address      Canonical Form";

static void show_cam (hwidbtype *idb, boolean mac_option,
		      mac_address_type address)
{
    bsi_instance_t    *ds;
    xx_fddi_regs_t    *rp;
    ushort i;
    boolean found;

    ds = INSTANCE;
    rp = ds->module_regs;

    printf("\n\nInfo for Fddi %d:", idb->unit);

    if (!ds->cam_present) {
	printf("\nCAM hardware not present");
	return;
    }

    if (!mac_option) {
	/* No parameters given, just dump the whole CAM and exit */

	automore_enable(NULL);

	if (rp->csr & CAM_CSR_CAM_FULL)
	    printf("\nThe CAM is FULL");

	printf(show_cam_header);

	for (i = 0; i < 1024; i++) {
	    found = print_cam_entry(rp, i);
	}
	automore_disable();
	return;
    }

    rp->csr &= (ushort)~CAM_CSR_CAM_COMPARE_ENABLE;
    while (rp->csr & CAM_CSR_CAM_COMPARE)
	;

    printf("\n%e", &address.u.sword[0]);

    if (find_cam_entry(rp, address.u.sword[0], address.u.sword[1],
		       address.u.sword[2])) {
	printf(" is in the CAM (wire order)");
	rp->csr |= CAM_CSR_CAM_COMPARE_ENABLE;
	return;
    }

    for (i = 0; i < 6; i++)
	address.u.byte[i] = BITSWAP (address.u.byte[i]);

    if (find_cam_entry(rp, address.u.sword[0], address.u.sword[1],
		       address.u.sword[2]))
	printf(" is in the CAM (canonical order)");
    else
	printf(" is NOT in the CAM");
    rp->csr |= CAM_CSR_CAM_COMPARE_ENABLE;
}

void show_control_cam (parseinfo *csb)
{
    hwidbtype           *hwidb = NULL;
    boolean mac_option = FALSE;
    mac_address_type address;
    boolean found = FALSE;
    list_element *l_elt;

    /*
     * Was there a specific interface ?
     */
    if (GETOBJ(idb,1)) {
	hwidb = GETOBJ(idb,1)->hwptr;
    }

    /*
     * Check for given MAC address
     */
    if (GETOBJ(hwaddr,1)->length) {
	mac_option = TRUE;
	ieee_copy(GETOBJ(hwaddr,1)->addr, address.u.byte);
    }

    if (hwidb != NULL) {
	show_cam(hwidb, mac_option, address);
    } else {
	FOR_ALL_HWIDBS_IN_LIST(HWIDBLIST_FDDI, l_elt, hwidb) {
	    if (hwidb->type == IDBTYPE_BSI) {
		show_cam(hwidb, mac_option, address);
		found = TRUE;
	    }
	}
	if (!found) {
	    printf("\nNo FDDI interface found");
	}
    }
}


void config_cam (xx_fddi_regs_t *rp)
{
    rp->csr = 0;

    /* Power-up anomalie */
    SPS_CR_DIRECT

    /* Device select */
    TCO_DS_DIRECT
    CAM_COMMAND_DIRECT(0)

    /* Reset the CAM */
    TCO_CT_DIRECT
    CAM_COMMAND_DIRECT(0)

    /* Reset the FIFO */
    rp->csr = CAM_CSR_FIFO_CLEAR;

    TCO_PA_DIRECT
    CAM_COMMAND_DIRECT(0)

    TCO_CT_DIRECT
    CAM_COMMAND_DIRECT(CAM_48_RAM_16 &
	               CAM_ENABLE_MATCH_FLAG &
	               CAM_ENABLE_FULL_FLAG &
	               CAM_INPUT_NOT_SCRAMBLED &
	               CAM_COMPARE_MASK_DISABLE &
	               CAM_ENABLE_ADDRESS_INC);

    TCO_SC_DIRECT
    CAM_COMMAND_DIRECT(CAM_DST_CNTR_START01_END11 &
		       CAM_SRC_CNTR_START01_END11 &
		       CAM_LOAD_DST_SEG_CNTR_01 &
		       CAM_LOAD_SRC_SEG_CNTR_01);
}

/*
 * cam_enable_promiscuous_multicast
 *
 * Add an OUI-only filter to the CAM comparisons.
 */

void cam_enable_promiscuous_multicast (xx_fddi_regs_t *rp)
{
    /*
     * Make sure back-to-back writes don't cause a bus error
     */
    if (rp->csr & CAM_CSR_FIFO_GO) {
	/*
	 * Spin for a bit (pun intended ;-) and try again
	 */
	usecdelay(2);

	if (rp->csr & CAM_CSR_FIFO_GO) {
	    fddibuginf("\nCAM: attempt to add CAM mask timed out");
	    return;
	}
    }

    /*
     * Write the correct mask to Mask Register 1
     *
     * The mask that's used in this mode is 00.00.00.FF.FF.FF. This
     * means that only the OUI of the MAC address is active in the
     * comparison.
     */
    TCO_SC
    CAM_COMMAND (CAM_LOAD_DST_SEG_CNTR_00 & CAM_DST_CNTR_START00_END11)
    SPD_MR1
    CAM_DATA_WR(0x0000)
    CAM_DATA_WR(0x00FF)
    CAM_DATA_WR(0xFFFF)
    CAM_DATA_WR(0xFFFF)
    SPD_CR

    TCO_SC
    CAM_COMMAND (CAM_LOAD_DST_SEG_CNTR_01 & CAM_DST_CNTR_START01_END11)

    /*
     * Reload the Control Register to let it know there's now a mask
     * involved in the comparison
     */
    TCO_CT
    CAM_COMMAND(CAM_48_RAM_16 &
		CAM_ENABLE_MATCH_FLAG &
		CAM_ENABLE_FULL_FLAG &
		CAM_INPUT_NOT_SCRAMBLED &
		CAM_COMPARE_MASK_REG1 &
		CAM_ENABLE_ADDRESS_INC);

    rp->csr |= CAM_CSR_FIFO_GO;
}    

/*
 * cam_disable_promiscuous_multicast
 *
 * Remove an OUI-only filter from the CAM comparisons.
 */

void cam_disable_promiscuous_multicast (xx_fddi_regs_t *rp)
{
    /*
     * Make sure back-to-back writes don't cause a bus error
     */
    if (rp->csr & CAM_CSR_FIFO_GO) {
	/*
	 * Spin for a bit (pun intended ;-) and try again
	 */
	usecdelay(2);

	if (rp->csr & CAM_CSR_FIFO_GO) {
	    fddibuginf("\nCAM: attempt to add CAM mask timed out");
	    return;
	}
    }

    /*
     * Clear any masks from Mask Register 1 to be on the safe side.
     */
    TCO_SC
    CAM_COMMAND (CAM_LOAD_DST_SEG_CNTR_00 & CAM_DST_CNTR_START00_END11)
    SPD_MR1
    CAM_DATA_WR(0x0000)
    CAM_DATA_WR(0x0000)
    CAM_DATA_WR(0x0000)
    CAM_DATA_WR(0x0000)
    SPD_CR

    TCO_SC
    CAM_COMMAND (CAM_LOAD_DST_SEG_CNTR_01 & CAM_DST_CNTR_START01_END11)

    /*
     * Reload the Control Register to let it know there's now *no* mask
     * involved in the comparison
     */
    TCO_CT
    CAM_COMMAND(CAM_48_RAM_16 &
		CAM_ENABLE_MATCH_FLAG &
		CAM_ENABLE_FULL_FLAG &
		CAM_INPUT_NOT_SCRAMBLED &
		CAM_COMPARE_MASK_DISABLE &
		CAM_ENABLE_ADDRESS_INC);

    rp->csr |= CAM_CSR_FIFO_GO;
}    

boolean config_xilinx (xx_fddi_regs_t *rp)
{
    char *xilinx_data_ptr;
    char *xilinx_end_ptr;

    fddibuginf ("\nconfig_xilinx()\n");
    xilinx_data_ptr = (char *)bsi_xilinx;
    xilinx_end_ptr = (char *)bsi_endxilinx;

    /* Verify LCAs are in configuration mode */
    if (rp->lcr & CAM_LCA_OPERATIONAL)
	return (CAM_FIRMWARE_ROM);

    /* Download the LCA config data from memory */
    while (xilinx_data_ptr < xilinx_end_ptr) {
	rp->lcr = (ushort)*xilinx_data_ptr++;

	/*
	 * The LCA needs 18uS between writes
	 * Play safe and give it 24
	 */
        usecdelay(24);
    }

    usecdelay(5);			/* Give the CAM time to start */

    /* Verify LCAs are in operational mode */
    return ((rp->lcr & CAM_LCA_OPERATIONAL) ?
	    CAM_FIRMWARE_SOFT : CAM_FIRMWARE_FAIL);
}

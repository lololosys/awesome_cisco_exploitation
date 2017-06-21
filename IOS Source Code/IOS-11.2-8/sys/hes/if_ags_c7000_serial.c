/* $Id: if_ags_c7000_serial.c,v 3.3.60.2 1996/08/13 02:17:24 gstovall Exp $
 * $Source: /release/112/cvs/Xsys/hes/if_ags_c7000_serial.c,v $
 *------------------------------------------------------------------
 * if_ags_c7000_serial.c -- Serial code that is used only on the 
 *                          AGS and c7000
 *
 * Copyright (c) 1988-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: if_ags_c7000_serial.c,v $
 * Revision 3.3.60.2  1996/08/13  02:17:24  gstovall
 * CSCdi39484:  Giant packets output on ethernet interface
 * Branch: California_branch
 * Make sure we drop giants, even if they are just a wee bit too big.
 *
 * Revision 3.3.60.1  1996/03/18  19:41:03  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.26.1  1996/01/24  21:46:01  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.3  1995/11/17  09:17:10  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:27:08  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:39:38  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:00:15  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  20:40:37  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "cbus_registry.h"
#include "interface_private.h"
#include "media_registry.h"
#include "if_hes_common.h"
#include "if_fci.h"
#include "if_mci.h"
#include "dbus.h"
#include "if_cmd.h"
#include "if_hes_serial.h"
#include "../wan/serial.h"
#include "../wan/serial_private.h"
#include "if_vip.h"


/*****************************************************************************
 * Name: platform_serial_reset_encsize
 *
 * Description: platform-specific serial reset encsize adjustments to the
 *   other fields of the hwidb.
 *
 * Note: hwidb->rxoffset and hwidb->span_macoffset are in bytes for the RSP
 *   and words for the c7000 and ags.
 *
 * Input:
 *   hwidb -- pointer to the hardware IDB for the interface
 *
 * Return Value: None
 *
 *****************************************************************************/
void platform_serial_reset_encsize (hwidbtype *hwidb)
{
    idbtype *swidb;

    hwidb->rxoffset = MCI_OFFSETSIZE - btow(hwidb->encsize);
    switch (hwidb->enctype) {
    case ET_PPP:
    case ET_SMDS:
        hwidb->span_encapsize = hwidb->encsize + 2;
        hwidb->span_macoffset = hwidb->rxoffset + btow(hwidb->span_encapsize); 
        break;

    case ET_HDLC: 
	if (!(system_loading | bootstrap_enable)) {
	    FOR_ALL_SWIDBS_ON_HW(hwidb, swidb) {
		reg_invoke_media_interesting_addresses(swidb);
	    }
        } /* fall through */

    default:
        hwidb->span_encapsize = hwidb->encsize;
        hwidb->span_macoffset = hwidb->rxoffset + btow(hwidb->span_encapsize); 
        break;
    }
}

/*****************************************************************************
 * Name: platform_serial_reset_action
 *
 * Description: platform-specific serial reset action
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *
 * Return Value: None
 *
 *****************************************************************************/
void platform_serial_reset_action (hwidbtype *hwidb)
{
    /*
     * Select, reset and then restart (load) the controller.
     */
#if defined(C7000)
    /*
     * We don't want to restart the whole board just
     * for one interface reset...
     */
    if (hwidb->status & IDB_HSSI) {
	dgtype *dbus;

	dbus = pslot_to_dbus(hwidb->cbus_ctrlr_index);
	dbus_ctrlr_reset(dbus);
	dbus_slot_enable(dbus); /* run */
	reg_invoke_slot_enable_led_on(hwidb->cbus_ctrlr_index);
	DELAY(FCI_SOFT_RESET_TIME);
    }
#else
    select_if(hwidb);
    send_ctrlr_reset_cmd(hwidb);
    DELAY(HSSI_DISABLE_TIME);
    send_ctrlr_load_cmd(hwidb, hwidb->cbus_ctrlr_index);
    DELAY(HSSI_DISABLE_TIME);
#endif
}

/*****************************************************************************
 * Name: platform_serial_reset_init
 *
 * Description: platform-specific serial reset initialization
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *
 * Return Value: None
 *
 *****************************************************************************/
void platform_serial_reset_init (hwidbtype *hwidb)
{
    /*
     * Assign the interface to a buffer pool, set max datagramsize,
     * set receive offset and love note pool.
     * The rxbytes size should be set dynamically according to
     * encapsulation.  Currently FDDI_BUFFERSIZE_BYTES is good
     * enough.
     */
    send_assign_poolindex_cmd(hwidb,hwidb->buffer_pool);
    send_rxbytes_cmd(hwidb,hwidb->max_buffer_size);
    send_rx_offset_cmd(hwidb,hwidb->rxoffset);
    send_rx_error_cmd(hwidb,FCI_RX_ERROR_POOL);
    send_cbus_buffer_park_cmd(hwidb,hwidb->cbus_burst_count);
}

/*****************************************************************************
 * Name: platform_serial_reset_clockrate
 *
 * Description: platform-specific serial reset clockrate
 *
 * Note: sw_version is not available on the RSP through the hwidb as accessed
 *	 here.
 *
 * Input:
 *   hwidb -- pointer to the hardware IDB for the interface
 *
 * Return Value: None
 *
 *****************************************************************************/
void platform_serial_reset_clockrate (hwidbtype *hwidb)
{
    serialsb *ssb;

    /*
     * Set up clock rate for internal or external handling.
     * New table was added for new xtal, identified by microcode
     * level greater than 5.0.
     */
    ssb = idb_get_hwsb_inline(hwidb, HWIDB_SB_SERIAL);
    if (((mcitype *)hwidb->devctl)->sw_version > 0x0500) {
	send_clockrate_cmd(hwidb,
			   mciclock_rev2[ssb->serial_clockindex].cookie);
	send_clocktype_cmd(hwidb,
			   mciclock_rev2[ssb->serial_clockindex].prescalar);
    } else {
	send_clockrate_cmd(hwidb,
			   mciclock[ssb->serial_clockindex].cookie);
	send_clocktype_cmd(hwidb,
			   mciclock[ssb->serial_clockindex].prescalar);
    }
}

/*****************************************************************************
 * Name: serial_platform_init
 *
 * Description: platform-specific serial IP init
 *
 * Input: 
 *   subsys -- pointer to the sybsystype for this subsystem
 *
 * Return Value: None
 *
 *****************************************************************************/
void serial_platform_init (subsystype *subsys)
{
#if defined(C7000)
    /*
     * Add a registry hook to suck in statistics love letters
     * from a VIP style 4T (only applies when being compiled for
     * the 7000, does not apply to the AGS)
     */
    reg_add_ether_sniff_encap(IDBTYPE_CBUSSERIAL, vip_sniff_encap,
                              "vip_sniff_encap");
#endif

}

/* $Id: rsp_feip.c,v 3.6.18.5 1996/08/28 13:13:45 thille Exp $
 * $Source: /release/112/cvs/Xsys/src-rsp/rsp_feip.c,v $
 *------------------------------------------------------------------
 * Fast Ethernet Interface Processor support code
 *
 * March 1995, Gary Kramling
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rsp_feip.c,v $
 * Revision 3.6.18.5  1996/08/28  13:13:45  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.6.18.4  1996/08/07  18:02:08  kmitchel
 * CSCdi49854:  DBUSINTERR after RSP dbus write; ineffective recvry of
 * disabled card
 * Branch: California_branch
 *
 * Revision 3.6.18.3  1996/06/07  19:02:30  kramling
 * CSCdi55236:  Problems with FEIP OIR
 * Branch: California_branch
 *
 * Revision 3.6.18.2  1996/06/03  17:51:22  getchell
 * CSCdi59311:  rsp_feip missing patch for CSCdi37087
 * Branch: California_branch
 *
 * Revision 3.6.18.1  1996/03/18  22:02:05  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.20.2  1996/02/20  18:00:39  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.6  1996/02/08  23:11:17  jjohnson
 * CSCdi48362:  Need a consistent, global mechanism for writing plural
 * strings.  Standardize on the Plural and ArgAndPlural macros
 *
 * Revision 3.5  1996/02/01  06:09:53  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.4.20.1  1996/01/24  23:03:57  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.4  1995/12/02  06:56:14  tgrennan
 * CSCdi42449:  4/8e address filter improperly set by RP
 *
 * 4/8e address filter works like FE and FEIP instead of EIP/MCI.
 *
 * Revision 3.3  1995/11/17  18:48:10  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:32:55  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:27:57  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/11/08  21:28:18  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.3  1995/08/22  15:34:03  kramling
 * CSCdi39113:  FEIP controllers are not listed in show version output
 *
 * Revision 2.2  1995/08/09  00:21:58  kramling
 * CSCdi38439:  FEIP product is now supported in 11.0.  Update support to
 * 10.3.
 *
 * Revision 2.1  1995/07/19  14:46:52  getchell
 * Placeholder files for VIP development
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "plural.h"
#include "subsys.h"
#include "logger.h"
#include "interface_private.h"
#include "rsp_bufhdr.h"
#include "rsp_if.h"
#include "rsp_vip.h"
#include "rsp_qasic.h"
#include "rsp_pcmap.h"
#include "sys_registry.h"
#include "packet.h"
#include "../hes/cbus_registry.h"
#include "media_registry.h"
#include "../hes/if_hes_common.h"
#include "rsp_common_if.h"
#include "../if/ether.h"
#include "../if/ether_private.h"
#include "../hes/if_cmd.h"
#include "../if/if_controller.h"
#include "../hes/hes_controller.h"
#include "../hes/if_fci.h"
#include "../hes/ucode.h"
#include "../dev/flash_dvr_spec.h"
#include "../hes/slot.h"
#include "lovenote.h"
#include "../hes/if_hes_ether.h"
#include "../hes/if_feip.h"
#include "../hes/if_vip.h"
#include "../ui/debug.h"
#include "../if/fast_ethernet_debug.h"
#include "../hes/msg_cbus.c"		/* Not a typo, see logger.h */


static uchar nFEIPs, nFEIPnets;


/*****************************************************************************
 * Name: rsp_fastether_idb_init
 *
 * Description: platform specific fast ethernet idb init
 *
 * Input: hwidb
 *   
 * Return Value: None
 *
 *****************************************************************************/
void rsp_fastether_idb_init (hwidbtype *hwidb, boolean vip_style)
{
    ethersb	*esb;

    /* in case anybody tries to reset us before we're ready */
    hwidb->reset = (reset_t) return_nothing;
    hwidb->periodic = NULL;

    nethernets++;
    nfethernets++;

    if (hwidb->status & IDB_NOHARDWARE) {
	hwidb->status &= ~IDB_NOHARDWARE;
	return;
    }

    hes_idb_init_before(hwidb);

    /*
     * RSP-specific fast ethernet IDB initialization.
     * XXX note, some of these are probably not RSP specific in
     * which case they can be moved to ether_idb_init()
     */
    reg_add_media_transmit_delay(IDBTYPE_FEIP,
				 cbus_set_transmit_delay,
				 "cbus_set_transmit_delay");
    hwidb->status = IDB_ETHER | FCI_IDBFLAGS;
    hwidb->firstsw->tbridge_media = TBR_ETHER;
    hwidb->listen = cbus_record_pas_ethernet_address;
    hwidb->listen_range = cbus_record_pas_ethernet_range;
    hwidb->afilter_reset = pas_ethernet_af_init;


    /*
     * If this fastethernet is on a VIP card, it is unable to hear itself.
     */
    if (vip_style) {
	hwidb->status &= ~IDB_HEARSELF;
	hwidb->status |= IDB_VIP;
    }

    /*
     * IDB initialization for all ethernet interface (high-end,
     * low-end, etc.)
     */
    ether_idb_init(hwidb);
    esb = idb_use_hwsb_inline(hwidb, HWIDB_SB_ETHER);

    /*
     * Note, that the units (bytes Vs words) used in the rxoffset
     * field is platform dependant.  On the RSP, the value represents
     * bytes (on other platforms, like the 7000, this value 
     * represents words).  The IP ucode on each platform has been
     * modified to expect the correct units for that platform.
     *
     * The following value is optimised for the encapsulation
     * and for RSP cacheline considerations. The etype field and the
     * network layer are in the same cache line. Also, the network
     * layer starts on a long word boundary.
     */
    hwidb->rxoffset = RSP_ETHER_BYTE_RECEIVE_OFFSET;
    hwidb->ip_memd_offset = hwidb->rxoffset + ADRSPC_MEMD_WRITETHRU +
	ETHER_ARPA_ENCAPBYTES;
    hwidb->encsize = ETHER_ARPA_ENCAPBYTES;

    /*
     * XXX The following need to be fixed up to reflect the
     * RSP's byte policy (instead of words).
     */
    hwidb->span_macoffset = hwidb->rxoffset;
    hwidb->span_encapsize = 0;	/* Pure transparent bridging */
    hwidb->rsrb_vringoffset = hwidb->rxoffset + ETHER_SNAP_ENCAPBYTES;
    hwidb->love_note = (void *)vip_rsp_love_letter;
    esb->media_type = ETHER_MEDIA_TYPE_FEIP_RJ45;

    /*
     * Complete set up of common fields.
     */
    init_new_hwidb(hwidb);
    hes_idb_init_after(hwidb);
    idb_release_hwsb_inline(hwidb, HWIDB_SB_ETHER);
}


/*****************************************************************************
 * Name: feip_ctrlr_analyze
 *
 * Description: analyze part of initialization for an FEIP
 *   This function is called when an board is first discovered and initialized.
 *   It is called the first time the board is plugged in, or when the router
 *   is resstarted.  It is *NOT* called for boards that are installed and
 *   running when a new board is inserted.
 *
 *   This function is responsible for discovering the interfaces (1,2)
 *   on the FEIP (via the CBUS -- select command), and creating
 *   and initializing the CDB and the IDB for those interfaces.
 *
 * Notes: 
 *   This function is *NEVER* called from interrupt level.
 *
 * Input: 
 *   slot -- pointer to a slottype structure for the IP with the
 *     following fields filled in:
 *       slot_num -- slot number for this card
 *       ccb_ptr -- pointer to the CCB for the FEIP
 *       cmd_q_ptr -- pointer to the CCB for the FEIP
 *
 * Output:
 *   The following fields in the slottype (pointed to by slot) are changed
 *     interface_count -- number of interfaces on this IP
 *     interface_list -- list of pointers to IDB's for the interfaces 
 *
 * Return Value: None
 *
 *****************************************************************************/
static void feip_ctrlr_analyze (slottype *slot)
{
    int port_num;
    ushort status;
    ushort interface_type;
    hwidbtype *hwidb;
    pa_info_slots *pa_info_ptr;

    /*
     * We loop through the interfaces on the FEIP selecting them as we go. 
     * We use the select command to tell if the interface is there.  If the
     * select command is successful, it returns the interface type (which
     * should be FCI_TYPE_ETHER).  If the select commands fails, or the
     * response says it is not an ethernet, we break out of this loop and
     * return.  If things go OK, we initialize the port.
     */
    for (port_num = 0; port_num < MAX_CTRLR_INTERFACES; ++port_num) {
	status = send_select_cmd(slot, port_num, &interface_type);
	if (status != FCI_RSP_OKAY) {
	    char name[CARDNAMELEN + 5]; /* room for name + slot number */

	    sprintf(name, "%s%u", slot->card_name, slot->slot_num);
	    errmsg(&msgsym(CMD, CBUS), "select", status, name);
	    break;
	}
	if (interface_type == FCI_TYPE_OUT_OF_RANGE)
	    break;
	if (interface_type != FCI_TYPE_ETHER) {
	    errmsg(&msgsym(PORTTYPE,CBUS), slot->card_name, slot->slot_num,
		   port_num, interface_type);
	    break;
	}

	/*
	 * Get the IDB for the FEIP interface
	 * If we find a matching one, we can just use it,
	 * otherwise we init a new one.
	 */
	hwidb = rsp_get_matching_idb(slot, port_num,
				     interface_type, IDBTYPE_FEIP);
	if (hwidb == NULL) {
	    break;		/* rsp_get_matching_idb() complains */
	}			/* about malloc() failure */

	/*
	 * Set up common fields.
         */
	slot->interface_list[port_num] = hwidb;
	slot->interface_count = 1 + port_num;

	rsp_fastether_idb_init(hwidb, FALSE);

	nFEIPnets++;
	slot->pa_info[port_num] = 
          malloc_named(sizeof(pa_info_slots), "FEIP PA info");
	pa_info_ptr = slot->pa_info[port_num];
	if (pa_info_ptr == NULL) continue;
	convert_raw_eeprom_to_pa_info(hwidb, (void *)pa_info_ptr);
	}

    ++nFEIPs;
}

/*****************************************************************************
 * Name: feip_ctrlr_init
 *
 * Description: per-card initialization for the FEIP
 *   This function is called during the initialization phase (after analyze)
 *   and after the configuration has been parsed.  The fields in the slottype
 *   have been filled in -- the information needs to be sent to the FEIP now. 
 *   It may also be necessary to pass global information to the FEIP.
 *   The algorithm is 
 *
 *       Send Set Global Queue Ptrs command
 *       Ensure the max love letter length is within allowable limits
 *
 * Notes: 
 *   This function is *NEVER* called from interrupt level.
 *
 * Input: 
 *   slot -- pointer to a slottype structure for the IP.  All fields should be
 *     valid when this function is called, but specifically
 *       slot_num -- slot number for this card
 *       ccb_ptr -- pointer to the CCB for the FEIP
 *       cmd_q_ptr -- pointer to the CCB for the FEIP
 *       event_q_ptr -- pointer to the event queue for this card to use
 *       love_note_free_q_ptr -- pointer to the queue of free love note buffer
 *         headers 
 *       love_letter_free_q_ptr -- pointer to the queue of free love letter
 *         buffers
 *
 * Output:
 *   Commands sent to the card
 *
 * Return Value: None
 *
 *****************************************************************************/
static void feip_ctrlr_init (slottype *slot)
{
    rsp_common_ctrlr_init(slot);
    return;
}

/*****************************************************************************
 * Name: feip_if_init
 *
 * Notes: 
 *   This function is *NEVER* called from interrupt level.
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface.  All fields
 *	should be valid when this function is called (as many as possible),
 *	but specifically
 *       raw_q_ptr
 *       rx_limit_lo
 *       rx_limit_hi
 *       global_free_q_size
 *       global_free_q_ptr
 *       local_free_q_ptr
 *       tx_q_ptr
 *       tx_acc_ptr
 *       rx_offset
 *
 * Output:
 *   Commands sent to the card
 *****************************************************************************/
static void feip_if_init (hwidbtype *hwidb)
{
    leveltype level;

    level = raise_interrupt_level(NETS_DISABLE);

    select_if(hwidb);

    rsp_common_if_init(hwidb);

    hwidb->reset = cbus_feip_reset;
    hwidb->periodic = cbus_mci_ether_periodic;

    reset_interrupt_level(level);
    return;
}

/*****************************************************************************
 * Name: feip_show_cbus
 *
 * Description: Get FEIP specific info. for 'show controller cbus'
 *
 * Input: 
 *   indx -- slot index number for this controller
 *
 * Return Value: None
 *
 *****************************************************************************/
void feip_show_cbus (ushort slot_index)
{
    slottype *slotp;
    hwidbtype *hwidb;
    int ix;
    
    slotp = &slots[slot_index];
    /* 
     * If the card is disabled, we have no relevant
     * information to report.  Also, we will be unable to
     * communicate with it using the cbus.
     */
    if (slotp->flags & BOARD_DISABLED)
	return;

    /*
     * Find the first interface on the FEIP
     */
    hwidb = NULL;
    for (ix = 0; ix < slotp->interface_count; ix++) {
	hwidb = slotp->interface_list[ix];
	if (hwidb)
	    break;
    }

    if (hwidb) {
	mem_lock(hwidb);
	feip_show_util(hwidb);
	free(hwidb);
    }
}

/*
 * invoked when an FEIP has been removed from the system
 */
static void
feip_ctrlr_removed (slottype *sp)
{
    int ix;
    hwidbtype *hwidb;
    ushort pa_indx;

    for (ix = 0; ix < sp->interface_count; ++ix) {
	hwidb = sp->interface_list[ix];
	if (hwidb) {
	    pa_indx = (ushort) hwidb->slotunit;
	    if (sp->pa_info[pa_indx]) {
	        free(sp->pa_info[pa_indx]);
                sp->pa_info[pa_indx] = NULL;
	    }
	    --nnets;
	    --nethernets;
	    --nfethernets;
	    --nFEIPnets;
	}
    }
    --nFEIPs;
    return;
}

/*
 * display count of FEIPs in the system
 */
static void
feip_print_controllers (void)
{
    if (nFEIPs) 
	printf("%u FEIP controller%s (%u FastEthernet).\n",
	       ArgAndPlural(nFEIPs,"","s"), nFEIPnets);
    return;
}

/*****************************************************************************
 * Name: feip_platform_init
 *
 * Description: platform specific FEIP init
 *
 * Input: 
 *   subsys -- pointer to the subsystype for this subsystem
 *
 * Return Value: None
 *
 *****************************************************************************/
void feip_platform_init (subsystype *subsys)
{
    nFEIPs = 0;
    nFEIPnets = 0;
    reg_add_ctrlr_analyze(FCI_FEIP_CONTROLLER,
			  feip_ctrlr_analyze, "feip_ctrlr_analyze");
    reg_add_ctrlr_init(FCI_FEIP_CONTROLLER, feip_ctrlr_init, "feip_ctrlr_init");
    reg_add_if_init(FCI_FEIP_CONTROLLER, feip_if_init, "feip_if_init");
    reg_add_ctrlr_removed(FCI_FEIP_CONTROLLER,
			  feip_ctrlr_removed, "feip_ctrlr_removed");
    reg_add_print_controllers(feip_print_controllers,
			      "feip_print_controllers");
    return;
}

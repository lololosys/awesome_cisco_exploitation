/* $Id: rsp_serial.c,v 3.10.12.8 1996/09/07 18:57:28 xtang Exp $
 * $Source: /release/112/cvs/Xsys/src-rsp/rsp_serial.c,v $
 *------------------------------------------------------------------
 * rsp_serial.c RSP specific serial IP (FSIP & HIP) support code
 *
 * August 1994, Marty Cieslak
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rsp_serial.c,v $
 * Revision 3.10.12.8  1996/09/07  18:57:28  xtang
 * CSCdi67401:  ALIGN-3-SPURIOUS: Spurious memory access made at
 * 0x60191478 reading
 * Branch: California_branch
 *         Turned off hwidb->serial_flags for HSSI and POSIP interfaces,
 *         therefore disallow clock rate (clockrate) command on these.
 *         Any serial interfaces that do not have clockrate command
 *         implemented should act similarly.
 *
 * Revision 3.10.12.7  1996/08/28  13:14:24  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.10.12.6  1996/07/08  22:52:56  mwu
 * CSCdi60922:  11.2 Core Features Commit
 * Branch: California_branch
 *
 * Revision 3.10.12.5  1996/06/09  09:04:48  rramacha
 * CSCdi58196:  Transmitter-delay broken on FSIP DCE interfaces
 * o FSIP panic message support, Use generic IP_PANIC for other interfaces
 *   too.
 * Branch: California_branch
 *
 * Revision 3.10.12.4  1996/05/22  23:26:27  rramacha
 * CSCdi58249:  Enable IP DFS on 4T
 * Branch: California_branch
 * Pass serial encap to serial driver in VIP
 *
 * Revision 3.10.12.3  1996/05/22  13:51:33  getchell
 * CSCdi57387:  corrupt pool pointer, VIP2 dies, box not healthy;VIP2
 * removed
 * Branch: California_branch
 *
 * Revision 3.10.12.2  1996/03/21  23:50:09  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.10.12.1  1996/03/18  22:02:36  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.2.6  1996/03/18  23:35:22  mbeesley
 * Branch: ELC_branch
 * Sync to V111_1_3
 *
 * Revision 3.3.2.5  1996/02/08  09:30:23  gstovall
 * Branch: ELC_branch
 * Sync to ELC_baseline_960205.
 *
 * Revision 3.3.2.4  1996/02/05  22:18:29  gstovall
 * Branch: ELC_branch
 * Fix cruft from my previous commit.
 *
 * Revision 3.3.2.3  1996/01/26  02:50:52  gstovall
 * Branch: ELC_branch
 * Oodles of fixes and enhancements:
 *         - Added support for the crc command to the predator 4T
 *         - Added support for the ignore-dcd command to the predator 4T
 *         - Do not keep stats on a VIP 4T
 *         - Pulled in a needed bugfix (CSCdi47390)
 *         - Revived the show controller command, it should work correctly
 *           with FDDI and token ring now.
 *
 * Revision 3.3.2.2  1996/01/16  02:35:51  dcarroll
 * Branch: ELC_branch
 * Sync ELC_branch between ELC_baseline_120995 and ELC_baseline_960110.
 *
 * Revision 3.3.2.1  1995/12/11  19:24:33  mbeesley
 * Branch: ELC_branch
 * Sync ELC_branch to ELC_baseline_120995.
 *
 * Revision 3.10  1996/02/13  20:25:38  hampton
 * Move platform specific defines from common include files to platform
 * specific include files.  [CSCdi48844]
 *
 * Revision 3.9  1996/02/10  05:51:24  rramacha
 * CSCdi48361:  crc4 and timeslots commands dont go to running config
 * after reload
 *
 * Revision 3.8  1996/02/08  23:11:25  jjohnson
 * CSCdi48362:  Need a consistent, global mechanism for writing plural
 * strings.  Standardize on the Plural and ArgAndPlural macros
 *
 * Revision 3.7  1996/02/01  06:09:58  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.6  1996/01/28  18:54:00  dstine
 * CSCdi47713:  Missing serial line control flags on RSP serial lines
 *
 * Revision 3.5.14.1  1996/01/24  23:04:22  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.5  1995/12/15  08:02:04  rramacha
 * CSCdi42101:  sh ver does not recognize CX-SMIP or CX-SSIP cards
 *
 * Revision 3.4  1995/12/02  05:50:44  rharagan
 * CSCdi41966:  Messages between 4T PA & driver should be per interface
 * Move to single interface commands, incorporate code review comments.
 *
 * Revision 3.3  1995/11/17  18:49:23  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:33:14  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:28:37  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/11/08  21:28:44  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.3  1995/07/14  00:53:27  schaefer
 * CSCdi37087: nnets not adjusted for new online-inserted IPs
 *
 * Revision 2.2  1995/07/06  01:33:12  hampton
 * Eliminate magic timestamps from SDLC and STUN code by converting them
 * to use the passive timer macros.  [CSCdi36750]
 *
 * Revision 2.1  1995/06/07 22:58:34  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "plural.h"
#include "logger.h"
#include "interface_private.h"
#include "rsp_pcmap.h"
#include "rsp_if.h"
#include "sys_registry.h"
#include "media_registry.h"
#include "../hes/cbus_registry.h"
#include "../hes/if_hes_common.h"
#include "../hes/if_fci.h"
#include "../hes/if_mci.h"
#include "rsp_common_if.h"
#include "../hes/if_cmd.h"
#include "../hes/ucode.h"
#include "../dev/flash_dvr_spec.h"
#include "../hes/slot.h"
#include "lovenote.h"
#include "../wan/serial.h"
#include "../wan/serial_private.h"
#include "../ibm/sdlc.h"
#include "../hes/if_hes_serial.h"
#include "rsp_vip.h"
#include "../hes/if_vip_4t.h"
#include "../hes/msg_cbus.c"		/* Not a typo, see logger.h */
#include "msg_rsp.c"			/* Not a typo, see logger.h */


static uchar nFSIPs, nFSIPnets, nSSIPs, nSSIPnets, nHIPs, nHIPnets;

/*****************************************************************************
 * Name: serial_love_note
 *
 * Description: love note handler for serial IP's
 *  
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the IP interface that
 *     generated the event (love note)
 *   lovenote -- pointer to the love note 
 *
 * Return Value: None
 *
 *****************************************************************************/
static void serial_love_note (hwidbtype *hwidb, lovenotetype *lovenote)
{
     if (lovenote->love_code & LOVE_CODE_PANIC) {
       errmsg(&msgsym(IP_PANIC,RSP), hwidb->hw_namestring,
              ((ulong*) lovenote)[0], ((ulong*) lovenote)[1],
              ((ulong*) lovenote)[2], ((ulong*) lovenote)[3]);
       return;
     }

    if (lovenote->love_code & LOVE_CODE_CARRIER_CHANGE) {
        cbus_mci_carrier(hwidb);
    }

    if ((lovenote->love_code & FCI_COND_LOOP) && (hwidb->status & IDB_HSSI))
        hssi_loop_request(hwidb);

    /*
     * XXX This is just a bit of policing to make sure Rob and ??? have
     * implemented the new length field policy. The FSIP and HIP never
     * send love letters, so the length should be 0. Once we've ensured
     * they have, this code should be removed.
     */
    if (lovenote->data_length) {
	if (hwidb->status & IDB_HSSI)
            buginf("\nHIP love note has non-zero length field");
        else
            buginf("\nFSIP love note has non-zero length field");
	buginf("\n  %08x %08x %08x %08x",
	       ((ulong*) lovenote)[0], ((ulong*) lovenote)[1],
	       ((ulong*) lovenote)[2], ((ulong*) lovenote)[3]);
        lovenote->data_length = 0;
    }
}
void rsp_serial_idb_init (hwidbtype *hwidb, boolean vip_style)
{
    serialsb	*ssb;

    /*
     * Protect ourselves in case anybody tries to reset us
     * before MEMD is carved:
     */
    hwidb->reset = (reset_t) return_nothing;
    hwidb->listen = (listen_t)return_nothing;
  
    switch (hwidb->type) {
    case IDBTYPE_CBUSSERIAL:
	nserialnets++;
	if (!vip_style)
	    if (hwidb->slot_ptr->ctrlr_specific == FCI_SSIP_CONTROLLER)
 		nSSIPnets++;
 	    else
 		nFSIPnets++;
	break;
    
    case IDBTYPE_HSSI:
	nhssinets++;
	nHIPnets++;
	break;
    
    default:				/* keep compiler happy */
	break;
    }
  
    if (hwidb->status & IDB_NOHARDWARE) {
	hwidb->status &= ~IDB_NOHARDWARE;
	return;
    }

    hes_idb_init_before(hwidb);

    if (hwidb->type  == IDBTYPE_CBUSSERIAL) {
	/*
	 * RSP-specific FSIP/4T initialization.
	 */
	hwidb->show_serial_state = fsip_show_serial_state;
	hwidb->status = IDB_SERIAL | IDB_SYNC | FCI_IDBFLAGS;
	hwidb->serial_flags |= (  SERIAL_FLAGS_CAN_ALT_FLAG
				| SERIAL_FLAGS_CAN_CLK_INV
				| SERIAL_FLAGS_CAN_NRZI
				| SERIAL_FLAGS_CAN_IGNORE_DCD
				| SERIAL_FLAGS_CAN_CRC_32);
	hwidb->serial_flags &= ~SERIAL_FLAGS_NO_CLOCKRATE;

	if (!hwidb->vip4t_data) {
	    hwidb->vip4t_data = malloc(sizeof(vip4t_data_t));
	}

	serial_idb_init(hwidb, MAXSERIALSIZE, MCI_MAXDGRAM_DEF,
			VISIBLE_MCI_SERIAL_BANDFAST, MCI_SERIAL_DELAY);

	reg_add_media_transmit_delay(IDBTYPE_CBUSSERIAL,
				     cbus_set_transmit_delay,
				     "cbus_set_transmit_delay");

	hwidb->firstsw->ip_mtu = MAXSERIALSIZE;

	/*
	 * hwidb->set_maxdgram must be initialized after calling
	 * serial_idb_init() so that hdlc_setup does not attempt to call
	 * cbus_mci_set_maxdgram() while doing system init.
	 */
	hwidb->set_maxdgram = cbus_mci_set_maxdgram;
	hwidb->buffersize = MCI_BUFFERSIZE_BYTES;
    
	rsp_serial_set_rxoffset(hwidb);
    
	TIMER_STOP(hwidb->sdlc_top_timer);
	hwidb->sdlc_slow_poll_interval = SDLC_DEFAULT_SLOW_POLL;
	if (vip_style) {
	    reg_add_show_pa_specific(IDBTYPE_CBUSSERIAL, 
				     cbus_vip_4t_show_controller,
				     "cbus_vip_4t_show_controller");
	} else {
	    hwidb->txqlength = fsip_get_txqlength;
	}
    } else {
	/*
	 * RSP-specific HIP initialization.
	 */
	hwidb->status = IDB_SERIAL | FCI_IDBFLAGS | IDB_HSSI | IDB_SYNC;
	hwidb->serial_flags |= SERIAL_FLAGS_CAN_CRC_32;
	hwidb->serial_flags |= SERIAL_FLAGS_NO_CLOCKRATE;
    
	serial_idb_init(hwidb, MAXDS3SIZE,
			(MAXDS3SIZE + DS3_OVERHEAD_BYTES),
			VISIBLE_FCI_SERIAL_BANDDS3, HSSI_SERIAL_DELAY);
    
	reg_add_media_transmit_delay(IDBTYPE_HSSI,
				     cbus_set_transmit_delay,
				     "cbus_set_transmit_delay");
    
	hwidb->firstsw->ip_mtu = MAXDS3SIZE;
    
	/*
	 * hwidb->set_maxdgram must be initialized after calling
	 * serial_idb_init() so that hdlc_setup does not attempt to call
	 * cbus_mci_set_maxdgram() while doing system init.
	 */
	hwidb->set_maxdgram = cbus_mci_set_maxdgram;
	hwidb->buffersize = (MAXDS3SIZE + DS3_BAGGAGE);
    
	rsp_serial_set_rxoffset(hwidb);
    
	hwidb->cbus_burst_count = hwidb->cbus_burst_count_def =
	    HSCI_BURST_COUNT_DEF;
	ssb = idb_use_hwsb_inline(hwidb, HWIDB_SB_SERIAL);
	ssb->hssi_lc_inhibit = TRUE;	/* Inhibit by default */
	idb_release_hwsb_inline(hwidb, HWIDB_SB_SERIAL);
    }
    ssb = idb_use_hwsb_inline(hwidb, HWIDB_SB_SERIAL);
    ssb->serial_clockindex = 0;
    idb_release_hwsb_inline(hwidb, HWIDB_SB_SERIAL);

    if (vip_style) {
	hwidb->status |= IDB_VIP;
	hwidb->love_note = (void *)vip_rsp_love_letter;
    } else {
	hwidb->love_note = (void *)serial_love_note;
    }

    /*
     * Complete set up of common fields.
     */
    hes_serial_idb_init(hwidb);
    init_new_hwidb(hwidb);
    hes_idb_init_after(hwidb);
}


/*****************************************************************************
 * Name: serial_ctrlr_analyze
 *
 * Description: analyze part of initialization for serial IP's
 *   This function is called when an board is first discovered and initialized.
 *   It is called the first time the board is plugged in, or when the router
 *   is resstarted.  It is *NOT* called for boards that are installed and
 *   running when a new board is inserted.
 *
 *   This function is responsible for discovering the interfaces
 *   on the IP (via the CBUS -- select command), and creating
 *   and initializing the CDB and the IDB for those interfaces.
 *
 * Notes: 
 *   This function is *NEVER* called from interrupt level.
 *
 * Input: 
 *   slot -- pointer to a slottype structure for the IP with the
 *     following fields filled in:
 *       slot_num -- slot number for this card
 *       ccb_ptr -- pointer to the CCB for the IP
 *       cmd_q_ptr -- pointer to the CCB for the IP
 *
 * Output:
 *   The following fields in the slottype (pointed to by slot) are changed
 *     interface_count -- number of interfaces on this IP
 *     interface_list -- list of pointers to IDB's for the interfaces 
 *
 * Return Value: None
 *
 *****************************************************************************/
static void serial_ctrlr_analyze (slottype *slot)
{
    int port_num;
    ushort status;
    ushort interface_type;
    hwidbtype *hwidb;
    enum IDBTYPE idb_type;
    
    /*
     * We loop through the interfaces on the IP selecting them as we go.
     * We use the select command to tell if the interface is there.  If the
     * select command is successful, it returns the interface type (which
     * should be either FCI_TYPE_SERIAL or FCI_TYPE_HSSI). If the select
     * commands fails, or the response is incorect, we break out of this
     * loop and return.  If things go OK, we initialize the port.
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
	if (interface_type == FCI_TYPE_SERIAL)
	    idb_type = IDBTYPE_CBUSSERIAL;
	else if (interface_type == FCI_TYPE_HSSI)
	    idb_type = IDBTYPE_HSSI;
	else {
	    errmsg(&msgsym(PORTTYPE,CBUS), slot->card_name, slot->slot_num,
		   port_num, interface_type);
	    break;
	}

	/*
	 * Get the IDB for the interface
	 * If we find a matching one, we can just use it,
	 * otherwise we init a new one.
	 */
	hwidb = rsp_get_matching_idb(slot, port_num, interface_type,
				     idb_type);
	if (hwidb == NULL) {
	    break;		/* rsp_get_matching_idb() complains */
	}			/* about malloc() failure */

	/*
	 * Set up common fields.
         */
	slot->interface_list[port_num] = hwidb;
	slot->interface_count = 1 + port_num;
	rsp_serial_idb_init(hwidb, FALSE);
	/*
	 * Determine the applique type so that the configuration can
	 * be parsed correctly.
	 */
	mci_applique(hwidb);
    }

    switch (slot->ctrlr_type) {
      case FCI_FSIP_CONTROLLER:	
 	if (slot->ctrlr_specific == FCI_SSIP_CONTROLLER)
 	    ++nSSIPs;
 	else
 	    ++nFSIPs; 
 	break;
      case FCI_LINK_CONTROLLER:   ++nHIPs;	break;
      default:					break;
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
    /* Nothing to do for RSP */
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
    send_serial_encap_cmd(hwidb, hwidb->enctype);
    /*
     * The rxbytes size should be set dynamically according to
     * encapsulation.
     */
    send_rx_offset_cmd(hwidb,hwidb->rxoffset);
}

/*****************************************************************************
 * Name: platform_serial_reset_clockrate
 *
 * Description: platform-specific serial reset clockrate
 *
 * Input:
 *   hwidb -- pointer to the hardware IDB for the interface
 *
 * Return Value: None
 *
 *****************************************************************************/
void platform_serial_reset_clockrate (hwidbtype *hwidb)
{
    /* Nothing to do for RSP */
}

/*
 * invoked when an FSIP has been removed from the system
 */
static void
fsip_ctrlr_removed (slottype *sp)
{
    int ix;

    for (ix = 0; ix < sp->interface_count; ++ix) {
	if (sp->interface_list[ix]) {
	    --nnets;
	    --nserialnets;
 	    if (sp->ctrlr_specific == FCI_SSIP_CONTROLLER)
 		--nSSIPnets;
 	    else
 		--nFSIPnets;
	}
    }
    if (sp->ctrlr_specific == FCI_SSIP_CONTROLLER)
 	--nSSIPs;
    else
 	--nFSIPs; 
    return;
}

/*
 * invoked when a HIP has been removed from the system
 */
static void
link_ctrlr_removed (slottype *sp)
{
    if (sp->interface_list[0]) {
	--nnets;
	--nhssinets;
	--nHIPnets;
    }
    --nHIPs;
    return;
}

/*
 * display count of FSIPs/HIPs in the system
 */
static void
serial_print_controllers (void)
{
    if (nHIPs) 
	printf("%u HIP controller%s (%u HSSI).\n",
	       ArgAndPlural(nHIPs,"","s"), nHIPnets);
    if (nFSIPs)
	printf("%u FSIP controller%s (%u Serial).\n",
	       ArgAndPlural(nFSIPs,"","s"), nFSIPnets);
    if (nSSIPs)
	printf("%u SSIP controller%s (%u Serial).\n",
	       ArgAndPlural(nSSIPs,"","s"), nSSIPnets);
    return;
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
    nFSIPs = 0;
    nFSIPnets = 0;
    nHIPs = 0;
    nHIPnets = 0;
    nSSIPs = 0;
    nSSIPnets = 0;

    reg_add_ctrlr_analyze(FCI_FSIP_CONTROLLER,
	serial_ctrlr_analyze, "serial_ctrlr_analyze");
    reg_add_ctrlr_init(FCI_FSIP_CONTROLLER,
	serial_ctrlr_init, "serial_ctrlr_init");
    reg_add_if_init(FCI_FSIP_CONTROLLER, serial_if_init, "serial_if_init");
    reg_add_ctrlr_removed(FCI_FSIP_CONTROLLER,
			  fsip_ctrlr_removed, "fsip_ctrlr_removed");

    reg_add_ctrlr_analyze(FCI_LINK_CONTROLLER,
	serial_ctrlr_analyze, "serial_ctrlr_analyze");
    reg_add_ctrlr_init(FCI_LINK_CONTROLLER,
	serial_ctrlr_init, "serial_ctrlr_init");
    reg_add_if_init(FCI_LINK_CONTROLLER, serial_if_init, "serial_if_init");
    reg_add_ctrlr_removed(FCI_LINK_CONTROLLER,
			  link_ctrlr_removed, "link_ctrlr_removed");

    reg_add_print_controllers(serial_print_controllers,
			      "serial_print_controllers");
}

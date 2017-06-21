/* $Id: rsp_trip.c,v 3.7.4.8 1996/08/28 13:14:29 thille Exp $
 * $Source: /release/112/cvs/Xsys/src-rsp/rsp_trip.c,v $
 *------------------------------------------------------------------
 * Token Ring Interface Processor support code
 *
 * April 1994, Carl Schaefer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rsp_trip.c,v $
 * Revision 3.7.4.8  1996/08/28  13:14:29  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.7.4.7  1996/08/26  15:11:54  ppearce
 * CSCdi66582:  Implement SRB vector table to cut size of SRB_CORE subsys
 * Branch: California_branch
 *
 * Revision 3.7.4.6  1996/08/13  02:24:28  gstovall
 * CSCdi39484:  Giant packets output on ethernet interface
 * Branch: California_branch
 * Make sure we drop giants, even if they are just a wee bit too big.
 *
 * Revision 3.7.4.5  1996/07/09  06:04:57  ppearce
 * CSCdi59527: inlines waste code space
 * Branch: California_branch
 *   Just say "no" to inline abuse - SRB subblock inlines removed
 *
 * Revision 3.7.4.4  1996/06/01  06:15:58  hampton
 * Remove obsolete media_tring_supports_etr registry call.  [CSCdi59254]
 * Branch: California_branch
 *
 * Revision 3.7.4.3  1996/05/29  03:45:58  skufer
 * CSCdi55758:  800E on TRIP steals MEMD from global queue, tql > tx queue
 * limit
 * Branch: California_branch
 *         TRIP on C7000 may advance tql when noisy ring causes
 *         frequent beacons.  Result is performance degradation.  When
 *         resetting the trip, new code prevents advancing the tql.  It
 *         is only a problem on c7000, not c7500 series.
 *
 * Revision 3.7.4.2  1996/05/17  11:54:39  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.5.4.4  1996/05/05  23:53:57  ppearce
 * Sync to IbuMod_Calif_baseline_960504
 *
 * Revision 3.5.4.3  1996/04/26  15:24:31  ppearce
 * IBU modularity - SRB subblock (part 1)
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.5.4.2  1996/04/03  21:43:34  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.5.4.1  1996/03/17  18:31:26  ppearce
 * Sync IbuMod_Calif_branch to V111_1_3
 *
 * Revision 3.7.4.1  1996/03/18  22:02:48  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.26.3  1996/03/16  07:36:52  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.3.26.2  1996/02/20  18:01:55  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.7  1996/03/07  11:14:26  dkerr
 * CSCdi50623:  IP flow cache needs better accounting granularity
 * TRIP rxoffset needs to be on a long word boundary.
 *
 * Revision 3.6  1996/03/04  15:29:20  dkerr
 * CSCdi50623:  IP flow cache needs better accounting granularity
 * Add minimal support for optimum/flow switching of tokenring (without
 * rif) and atm (aal5-mux and llc-snap only).
 *
 * Revision 3.5  1996/02/08  23:11:27  jjohnson
 * CSCdi48362:  Need a consistent, global mechanism for writing plural
 * strings.  Standardize on the Plural and ArgAndPlural macros
 *
 * Revision 3.4  1996/02/01  06:10:06  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.3.26.1  1996/01/24  23:04:25  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.3  1995/11/17  18:49:42  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:33:24  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:28:46  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/07/16  22:31:11  getchell
 * CSCdi35501:  src-rsp/ccb_cmd.h should be moved to hes
 * src-rsp/[ccb.h,ccb_cmd.h] combined and moved to hes/ccb_cmd.h.
 * rsp_command prototypes removed from ccb_cmd.h and moved to
 * src-rsp/rsp_commands.h.  Files including altered files modifed to
 * reflect new file structure.
 *
 * Revision 2.3  1995/07/14  00:53:29  schaefer
 * CSCdi37087: nnets not adjusted for new online-inserted IPs
 *
 * Revision 2.2  1995/06/21  03:15:35  gstovall
 * CSCdi35731:  Cleanup unused cruft in idb
 * Nuke some unused vectors and variables from the hwidb.
 *
 * Revision 2.1  1995/06/07  22:58:45  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "logger.h"
#include "../hes/msg_cbus.c"		/* Not a typo, see logger.h */
#include "plural.h"
#include "interface_private.h"
#include "sys_registry.h"
#include "../hes/cbus_registry.h"
#include "media_registry.h"
#include "../hes/if_hes_common.h"
#include "../hes/if_fci.h"
#include "../src-rsp/rsp_if.h"
#include "../hes/if_mci.h"
#include "rsp_common_if.h"
#include "rsp_pcmap.h"
#include "rsp_commands.h"
#include "../hes/if_cmd.h"
#include "../hes/ccb_cmd.h"
#include "../hes/ucode.h"
#include "../dev/flash_dvr_spec.h"
#include "../hes/slot.h"
#include "lovenote.h"
#include "../if/tring.h"
#include "../if/tring_private.h"
#include "../hes/if_trip.h"
#include "../srt/srb_core_registry.h"
#include "../if/network.h"


static uchar nTRIPs, nTRIPnets;

/*****************************************************************************
 * Name: trip_love_note
 *
 * Description: love note handler for TRIP
 *   Note the TRIP is an old-fashioned IP, that uses the bit encoding
 *   form of lovenote codes.  The TRIP generates the following events:
 *
 *     LOVE_CODE_CARRIER_CHANGE   plus
 *     a whole bunch handled by ctr_conditions()
 *  
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the TRIP interface that
 *     generated the event (love note)
 *   lovenote -- pointer to the love note 
 *
 * Return Value: None
 *
 *****************************************************************************/

static void trip_love_note (hwidbtype *hwidb, lovenotetype *lovenote)
{
    if (lovenote->love_code & LOVE_CODE_CARRIER_CHANGE) {
	cbus_mci_carrier(hwidb);
    }

    ctr_conditions(hwidb, lovenote->love_code);

    /*
     * XXX This is just a bit of policing to make sure Stan has
     * implemented the new length field policy. The TRIP never sends 
     * love letters, so the length should be 0. Once we've ensured
     * he has, this code should be removed.
     */
    if (lovenote->data_length) {
	buginf("\nTRIP love note has non-zero length field"
	       "\n  %08x %08x %08x %08x",
	       ((ulong*) lovenote)[0], ((ulong*) lovenote)[1],
	       ((ulong*) lovenote)[2], ((ulong*) lovenote)[3]);
	lovenote->data_length = 0;
    }
}

/*****************************************************************************
 * Name: trip_ctrlr_analyze
 *
 * Description: analyze part of initialization for a TRIP
 *   This function is called when an board is first discovered and initialized.
 *   It is called the first time the board is plugged in, or when the router
 *   is resstarted.  It is *NOT* called for boards that are installed and
 *   running when a new board is inserted.
 *
 *   This function is responsible for discovering the interfaces
 *   on the TRIP (via the CBUS -- select command), and creating
 *   and initializing the CDB and the IDB for those interfaces.
 *
 * Notes: 
 *   This function is *NEVER* called from interrupt level.
 *
 * Input: 
 *   slot -- pointer to a slottype structure for the IP with the
 *     following fields filled in:
 *       slot_num -- slot number for this card
 *       ccb_ptr -- pointer to the CCB for the TRIP
 *       cmd_q_ptr -- pointer to the CCB for the TRIP
 *
 * Output:
 *   The following fields in the slottype (pointed to by slot) are changed
 *     interface_count -- number of interfaces on this IP
 *     interface_list -- list of pointers to IDB's for the interfaces 
 *
 * Return Value: None
 *
 *****************************************************************************/
static void trip_ctrlr_analyze (slottype *slot)
{
    int port_num;
    ushort status;
    ushort interface_type;
    hwidbtype *hwidb;

    /*
     * We loop through the interfaces on the TRIP selecting them as we go. 
     * We use the select command to tell if the interface is there.  If the
     * select command is successful, it returns the interface type (which
     * should be FCI_TYPE_CTR).  If the select commands fails, or the
     * response says it is not a token ring, we break out of this loop and
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
	if (interface_type != FCI_TYPE_CTR) {
	    errmsg(&msgsym(PORTTYPE,CBUS), slot->card_name, slot->slot_num,
		   port_num, interface_type);
	    break;
	}

	/*
	 * Get the IDB for the TRIP interface
	 * If we find a matching one, we can just use it,
	 * otherwise we init a new one.
	 */
	hwidb = rsp_get_matching_idb(slot, port_num,
				     interface_type, IDBTYPE_CTR);
	if (hwidb == NULL) {
	    break;		/* rsp_get_matching_idb() complains */
	}			/* about malloc() failure */

	/*
	 * Set up common fields:
	 */
	slot->interface_list[port_num] = hwidb;
	slot->interface_count = 1 + port_num;

	/*
	 * Protect ourselves in case anybody tries to reset us
	 * before MEMD is carved:
	 */
	hwidb->reset = (reset_t) return_nothing;
	hwidb->periodic = NULL;

	nTRs++;
	nTRIPnets++;

	if (hwidb->status & IDB_NOHARDWARE) {
	    hwidb->status &= ~IDB_NOHARDWARE;
	    continue;
	}

	hes_idb_init_before(hwidb);
	/*
	 * RSP-specific Tokenring (TRIP) IDB initialization.
	 */
	reg_add_media_transmit_delay(IDBTYPE_CTR,
				     cbus_set_transmit_delay,
				     "cbus_set_transmit_delay");
	hwidb->status = IDB_TR | FCI_IDBFLAGS;
	hwidb->firstsw->tbridge_media = TBR_CTR;
	hwidb->listen = (listen_t)return_nothing;

	/*
	 * IDB initialization for all tring interfaces (high-end,
	 * low-end, etc.)
	 * Assume 16Mbps se we can go ahead and initialize this guy.  We
	 * will set the actual ring speed once we have read it from the
	 * configuration file, before we ever attempt to insert into the
	 * ring.
	 */
	tring_idb_init(hwidb, 16);

	hwidb->tr_ring_speed = 0;

	if (!hwidb->ctr_private) {
	    hwidb->ctr_private = malloc(sizeof(spyglass_data_t));
	}

	/*
	 * RSP-specific TRIP IDB initialization.
	 * XXX note, some of these are probably not RSP specific in
	 * which case they can be moved to tring_idb_init()
	 */
	reg_add_setup_srb(hwidb->type, ctr_setup, "ctr_setup");
	reg_add_modify_srb(hwidb->type, ctr_modify, "ctr_modify");

        hwidb->dot5_get_smt_stats = ctr_get_mib_stats;
        hwidb->dot5_add_counters  = ctr_update_prev_stats;
        hwidb->dot5_soft_reset    = cbus_token_reset;

	hwidb->system_configured = (system_configured_t) cbus_token_reset;

	if_maxdgram_adjust(hwidb, CTR_BUFFERSIZE_BYTES);
	hwidb->set_maxdgram = cbus_mci_set_maxdgram;
	
	hwidb->buffersize = (CTR_BUFFERSIZE_WORDS << 1);

	/*
	 * Note, that the units (bytes Vs words) used in the rxoffset
	 * field is platform dependant.  On the RSP, the value represents
	 * bytes (on other platforms, like the 7000, this value 
	 * represents words).  The IP ucode on each platform has been
	 * modified to expect the correct units for that platform.
	 *
	 * XXX - Chris, optimize these for RSP cachelines.
	 * XXX Also, what do we set span_ and rsrb_ stuff to on RSP?
	 *     Where are the RSP bridging queens when you need 'em?
	 */
	hwidb->rxoffset = RSP_TOKEN_BYTE_RECEIVE_OFFSET;
	hwidb->encsize = TRING_SNAP_ENCAPBYTES;
	hwidb->span_encapsize = 2;	/* 2 bytes of preamble before MAC */
	hwidb->span_macoffset = hwidb->rxoffset + hwidb->span_encapsize;
	hwidb->rsrb_vringoffset = hwidb->rxoffset + TRING_SNAP_ENCAPBYTES;
	hwidb->ip_memd_offset = hwidb->rxoffset + TRING_SNAP_ENCAPBYTES +
	  ADRSPC_MEMD_WRITETHRU;

	hwidb->set_extra_addrs = ctr_set_addresses;
	hwidb->srb_routecache = TRUE;	/* Fast SRB on by default */

	hwidb->love_note = (void *)trip_love_note;

	hwidb->shutdown = cbus_token_shutdown;
	hwidb->show_controller = ctr_show_controller;

	/*
	 * If this is the first TRIP, do some initializations:
	 */
	if (hwidb->unit == 1) {
	    /*
	     * Get the software equivalent of the deadman timer running
	     */
	    reg_add_if_onesec(ctr_deadman_poll, "ctr_deadman_poll");
	}

	/*
	 * Complete set up of common fields.
	 */
	init_new_hwidb(hwidb);
	hes_idb_init_after(hwidb);
    }
    ++nTRIPs;
}

/*****************************************************************************
 * Name: trip_ctrlr_init
 *
 * Description: per-card initizliation for the TRIP
 *   This function is called during the initialization phase (after analyze)
 *   and after the configuration has been parsed.  The fields in the slottype
 *   have been filled in -- the information needs to be sent to the TRIP now. 
 *   It may also be necessary to pass global information to the TRIP.
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
 *       ccb_ptr -- pointer to the CCB for the TRIP
 *       cmd_q_ptr -- pointer to the CCB for the TRIP
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
static void trip_ctrlr_init (slottype *slot)
{
    rsp_common_ctrlr_init(slot);
}

/*****************************************************************************
 * Name: trip_if_init
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
static void trip_if_init (hwidbtype *hwidb)
{
    leveltype level;

    level = raise_interrupt_level(NETS_DISABLE);

    select_if(hwidb);

    rsp_common_if_init(hwidb);

    send_rxbytes_cmd(hwidb,hwidb->max_buffer_size);

    hwidb->reset = (reset_t) cbus_token_reset;
    hwidb->periodic = cbus_token_periodic;

    reset_interrupt_level(level);
}

/*****************************************************************************
 * Name: trip_restart_init
 *
 * Description: Post TRIP restart reinitialization, we don't need
 * to resend anything on the RSP, because the TRIP remembers it
 * accros i/f resets.  If things have actually changed (e.g. MEMD
 * has been recarved) then we rely on that code to call the
 * trip_if_init() routine like it does at boot time.
 *
 * Input:
 *   hwidb -- pointer to the hardware IDB for the interface.
 *
 * Return Value: None
 *
 *****************************************************************************/
void trip_restart_init (hwidbtype *hwidb)
{
}

/*****************************************************************************
 * Name: platform_trip_anybody_out_there
 *
 * Description: Determine if the TRIP is still alive by sending it
 *              a simple VERSION command.
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface.
 *
 * Return Value: the status returned from the command.
 *
 *****************************************************************************/
ushort platform_trip_anybody_out_there (hwidbtype *hwidb)
{
    ushort who_cares;

    return(send_version_cmd(hwidb->slot_ptr, CCB_VERSION_SW, &who_cares));
}

/*
 * ctr_polling_failed
 * Print an errmsg and reset the bus.
 */
void ctr_polling_failed (hwidbtype *idb)
{
    errmsg(&msgsym(POLLFAIL, CBUS), idb->hw_namestring);
    errmsg(&msgsym(POLLFAILDAT, CBUS), idb->slotunit, 
	   get_ip_diag0(idb->slot_ptr),  get_ip_diag1(idb->slot_ptr), 
	   0, 0, 0, 0);
    cbus_restarts_error++;
    platform_demand_complex_restart(idb);
}

/*
 * invoked when a TRIP has been removed from the system
 */
static void
trip_ctrlr_removed (slottype *sp)
{
    int ix;

    for (ix = 0; ix < sp->interface_count; ++ix) {
	if (sp->interface_list[ix]) {
	    --nnets;
	    --nTRs;
	    --nTRIPnets;
	}
    }
    --nTRIPs;
    return;
}

/*
 * display count of TRIPs in the system
 */
static void
trip_print_controllers (void)
{
    if (nTRIPs) 
	printf("%u TRIP controller%s (%u Token Ring).\n",
	       ArgAndPlural(nTRIPs,"","s"), nTRIPnets);
    return;
}

/*****************************************************************************
 * Name: trip_platform_init
 *
 * Description: platform specific TRIP init
 *
 * Input: 
 *   subsys -- pointer to the subsystype for this subsystem
 *
 * Return Value: None
 *
 *****************************************************************************/
void trip_platform_init (subsystype *subsys)
{
    nTRIPs = 0;
    reg_add_ctrlr_analyze(FCI_CTR_CONTROLLER,
			  trip_ctrlr_analyze, "trip_ctrlr_analyze");
    reg_add_ctrlr_init(FCI_CTR_CONTROLLER, trip_ctrlr_init, "trip_ctrlr_init");
    reg_add_if_init(FCI_CTR_CONTROLLER, trip_if_init, "trip_if_init");
    reg_add_ctrlr_removed(FCI_CTR_CONTROLLER,
			  trip_ctrlr_removed, "trip_ctrlr_removed");
    reg_add_print_controllers(trip_print_controllers,
			      "trip_print_controllers");
}
/*****************************************************************************
 * Name: trip_cbus_mci_txlimit
 *
 * Description: workaround for CSCdi55758
 *   TRIP on c7000 will steal memd as tql > txl. This is not a problem on
 *   series 7500.
 *
 * Return Value: None
 *
 *****************************************************************************/
void trip_cbus_mci_txlimit(hwidbtype *trip_hwidb)
{
     cbus_mci_txlimit(trip_hwidb);
}

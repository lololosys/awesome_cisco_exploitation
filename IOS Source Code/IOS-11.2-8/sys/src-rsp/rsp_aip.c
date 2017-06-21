/* $Id: rsp_aip.c,v 3.6.6.4 1996/08/28 13:13:11 thille Exp $
 * $Source: /release/112/cvs/Xsys/src-rsp/rsp_aip.c,v $
 *------------------------------------------------------------------
 * ATM Interface Processor support code
 *
 * April 1994, Jack Jenney
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rsp_aip.c,v $
 * Revision 3.6.6.4  1996/08/28  13:13:11  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.6.6.3  1996/06/09  09:04:46  rramacha
 * CSCdi58196:  Transmitter-delay broken on FSIP DCE interfaces
 * o FSIP panic message support, Use generic IP_PANIC for other interfaces
 *   too.
 * Branch: California_branch
 *
 * Revision 3.6.6.2  1996/05/09  14:46:02  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.6.6.1.18.1  1996/04/27  07:25:47  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.6.6.1.8.1  1996/04/08  02:09:17  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.6.6.1  1996/03/18  22:01:36  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.6.10.1  1996/03/28  17:36:11  rbadri
 * Branch: LE_Syn_Cal_V111_1_3_branch
 * MTU support for sub-interfaces (used by ATM interfaces)
 *
 * Revision 3.6  1996/03/04  15:29:05  dkerr
 * CSCdi50623:  IP flow cache needs better accounting granularity
 * Add minimal support for optimum/flow switching of tokenring (without
 * rif) and atm (aal5-mux and llc-snap only).
 *
 * Revision 3.5  1996/02/08  23:11:09  jjohnson
 * CSCdi48362:  Need a consistent, global mechanism for writing plural
 * strings.  Standardize on the Plural and ArgAndPlural macros
 *
 * Revision 3.4  1996/02/01  06:09:31  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.3.26.1  1996/01/24  23:03:25  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.3  1995/11/17  18:47:05  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:32:49  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:27:24  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/11/08  21:27:54  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.3  1995/07/16  22:30:48  getchell
 * CSCdi35501:  src-rsp/ccb_cmd.h should be moved to hes
 * src-rsp/[ccb.h,ccb_cmd.h] combined and moved to hes/ccb_cmd.h.
 * rsp_command prototypes removed from ccb_cmd.h and moved to
 * src-rsp/rsp_commands.h.  Files including altered files modifed to
 * reflect new file structure.
 *
 * Revision 2.2  1995/07/14  00:53:12  schaefer
 * CSCdi37087: nnets not adjusted for new online-inserted IPs
 *
 * Revision 2.1  1995/06/07  22:57:17  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "plural.h"
#include "subsys.h"
#include "logger.h"
#include "../src-rsp/msg_rsp.c"		/* Not a typo, see logger.h */
#include "interface_private.h"
#include "rsp_bufhdr.h"
#include "sys_registry.h"
#include "media_registry.h"
#include "packet.h"
#include "../hes/cbus_registry.h"
#include "../hes/if_hes_common.h"
#include "rsp_common_if.h"
#include "rsp_pcmap.h"
#include "rsp_commands.h"
#include "../hes/if_cmd.h"
#include "../hes/ccb_cmd.h"
#include "../if/if_controller.h"
#include "../hes/hes_controller.h"
#include "../hes/if_fci.h"
#include "address.h"
#include "../if/atm.h"
#include "../if/ether.h"
#include "../ui/debug.h"
#include "../if/atm_debug_flags.h"
#include "../hes/if_aip.h"
#include "../hes/ucode.h"
#include "../dev/flash_dvr_spec.h"
#include "../hes/slot.h"
#include "lovenote.h"
#include "rsp_if.h"
#include "fastswitch_registry.h"
#include "rsp_qasic.h"
#include "qa_hw.h"
#include "../hes/msg_cbus.c"		/* Not a typo, see logger.h */
#include "../parser/parser_defs_parser.h"

static uchar nAIPs, nAIPnets;

/*****************************************************************************
 * Name: aip_love_note
 *
 * Description: love note handler for AIPs
 *   This routine handles the love notes, love letters from the AIP
 *  
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the AIP interface that
 *     generated the event (love note)
 *   lovenote -- pointer to the love note 
 *
 * Return Value: None
 *
 *****************************************************************************/
void aip_love_note (hwidbtype *hwidb, lovenotetype *lovenote)
{
    ushort    asr = lovenote->love_code;
    uchar     *loveletter;

    if (atm_event_debug) {
	buginf("\naip_love_note(%s): asr=0x%x length=%d",
	       hwidb->hw_namestring, asr, lovenote->data_length);
    }

    if (asr & LOVE_CODE_PANIC) {
	ulong *ptr;

	ptr = MEMD_OFFSET_TO_ADDR(lovenote->buffer_ptr);
	errmsg(&msgsym(IP_PANIC,RSP), hwidb->hw_namestring, ptr[0], ptr[1],
		ptr[2], ptr[3]);
	return;
    }

    if (lovenote->data_length) {
	loveletter = MEMD_OFFSET_TO_ADDR(lovenote->buffer_ptr);
	if (asr == AIP_EVNT_VC_STATS) {
	    atm_parse_vc_stats(hwidb, (*(ushort *)loveletter == 0),
		(ulong *)(loveletter + 4), lovenote->data_length - 4);
	} else {
	    if (atm_errors_debug)
		buginf("\naip_love_note(%s): UNKNOWN letter asr=0x%x",
		       hwidb->hw_namestring, asr);
	    lovenote->data_length = 0; /* Ignore it. */
	}
	return;
    }

    if (asr & LOVE_CODE_CARRIER_CHANGE) {
	cbus_mci_carrier(hwidb);
    }

    aip_love_note_common(hwidb, asr);
}

/*****************************************************************************
 * Name: aip_ctrlr_analyze
 *
 * Description: analyze part of initialization for an AIP
 *
 * Notes: 
 *   This function is *NEVER* called from interrupt level.
 *
 * Input: 
 *   slot -- pointer to a slottype structure for the IP with the
 *     following fields filled in:
 *       slot_num -- slot number for this card
 *       ccb_ptr -- pointer to the CCB for the AIP
 *       cmd_q_ptr -- pointer to the CCB for the AIP
 *
 * Output:
 *   The following fields in the slottype (pointed to by slot) are changed
 *     interface_count -- number of interfaces on this IP
 *     interface_list -- list of pointers to IDB's for the interfaces 
 *
 * Return Value: None
 *
 *****************************************************************************/

static void aip_ctrlr_analyze (slottype *slot)
{
    int port_num;
    hwidbtype *hwidb;
    ushort interface_type, status;
    
    port_num = 0;
    status = send_select_cmd(slot, port_num, &interface_type);
    if (status != FCI_RSP_OKAY) {
	char name[CARDNAMELEN + 5]; /* room for name + slot number */

	sprintf(name, "%s%u", slot->card_name, slot->slot_num);
	errmsg(&msgsym(CMD, CBUS), "select", status, name);
	return;
    }
    if (interface_type == FCI_TYPE_OUT_OF_RANGE)
	return;
    if (interface_type != FCI_TYPE_ATM) {
	errmsg(&msgsym(PORTTYPE,CBUS), slot->card_name, slot->slot_num,
	       port_num, interface_type);
	return;
    }
    ++nAIPs;
    /*
     * Get the IDB for the AIP interface
     * If we find a matching one, we can just use it, otherwise we init
     * a new one.
     */
    hwidb = rsp_get_matching_idb(slot, port_num, interface_type, IDBTYPE_AIP);
    if (hwidb != NULL) {
        /*
         * Set up common fields.
         */
	slot->interface_list[port_num] = hwidb;
	slot->interface_count = 1 + port_num;

        /*
         * Protect ourselves in case anybody tries to reset us
	 * before MEMD is carved. This is also used as a flag to
	 * indicate that "aip_periodic" should not be performed.
	 */
        hwidb->reset = (reset_t) return_nothing;

	natmnets++;
	nAIPnets++;

	if (hwidb->status & IDB_NOHARDWARE) {
	    hwidb->status &= ~IDB_NOHARDWARE;
	    return;
	}

        hes_idb_init_before(hwidb);
	/*
	 * RSP-specific AIP IDB initialization.
	 * XXX note, some of these are probably not RSP specific in
	 * which case they can be moved to reg_invoke_media_atm_setup()
	 */
	hwidb->status = IDB_ATM | FCI_IDBFLAGS | IDB_SYNC;
	hwidb->set_maxdgram = cbus_mci_set_maxdgram;
	hwidb->maxmtu = ATM_DEFAULT_MTU;
	hwidb->maxmtu_def = ATM_DEFAULT_MTU;
	hwidb->firstsw->sub_mtu = hwidb->maxmtu_def;
	hwidb->name = IFNAME_ATM;

	/*
	 * ATM-specific IDB initialization
	 */
	if (reg_invoke_media_atm_setup(hwidb)) {
	    /*
	     * AIP-specific IDB initialization
             * (Requires atm_setup to be successful!)
	     */
	    aip_setup(hwidb);
	    hwidb->reset = (reset_t) return_nothing;
	}

	/*
	 * Note, that the units (bytes Vs words) used in the rxoffset
	 * field is platform dependant.  On the RSP, the value represents
	 * bytes (on other platforms, like the 7000, this value 
	 * represents words).  The IP ucode on each platform has been
	 * modified to expect the correct units for that platform.
	 *
	 * XXX The following value needs to be optimised for the 
	 * encapsulation, and RSP cacheline considerations.
	 */
	hwidb->rxoffset = RSP_AIP_BYTE_RECEIVE_OFFSET;
	hwidb->ip_memd_offset = hwidb->rxoffset + ADRSPC_MEMD_WRITETHRU;

	/*
	 * XXX The following need to be fixed up to reflect the
	 * RSP's byte policy (instead of words).
	 */
	hwidb->span_macoffset = hwidb->rxoffset;
	hwidb->span_encapsize = hwidb->encsize;

	hwidb->love_note = (void *)aip_love_note;

        /*
         * Complete set up of common fields.
         */
	init_new_hwidb(hwidb);
        hes_idb_init_after(hwidb);

	/*
	 * Determine the plim type and rate so that the configuration can
	 * be parsed correctly. The atm_db should have been initialized by
	 * atm_setup, but we check to make sure.
	 */
	if (hwidb->atm_db)
	    aip_get_plim_info(hwidb);
    }
}

/*****************************************************************************
 * Name: aip_ctrlr_init
 *
 * Description: per-card initialization for the AIP
 *   This function is called during the initialization phase (after analyze)
 *   and after the configuration has been parsed.  The fields in the slottype
 *   have been filled in -- the information needs to be sent to the AIP now. 
 *   It may also be necessary to pass global information to the AIP.
 *   The algorithm is 
 *
 *       Send Set Global Queue Ptrs command
 *
 * Notes: 
 *   This function is *NEVER* called from interrupt level.
 *
 * Input: 
 *   slot -- pointer to a slottype structure for the IP.  All fields should be
 *     valid when this function is called, but specifically
 *       slot_num -- slot number for this card
 *       ccb_ptr -- pointer to the CCB for the AIP
 *       cmd_q_ptr -- pointer to the CCB for the AIP
 *       event_q_ptr -- pointer to the event queue for this card to use
 *       love_note_free_q_ptr -- pointer to the queue of free love note buffer
 *         headers 
 *       love_letter_free_q_ptr -- pointer to the queue of free love letter
 *         buffers
 *
 * Output:
 *   Commands sent to the card
 *****************************************************************************/
static void aip_ctrlr_init (slottype *slot)
{
    rsp_common_ctrlr_init(slot);
}

/*****************************************************************************
 * Name: aip_if_init
 *
 * Description: per-interface initialization for the AIP
 *
 * Notes: 
 *   This function is *NEVER* called from interrupt level.
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface.  All fields should
 *     be valid when this function is called (as many as possible), but
 *     specifically:
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
 *
 *****************************************************************************/
static void aip_if_init (hwidbtype *hwidb)
{
    char *str;
    char applique[48];
    atm_db_t *aip = hwidb->atm_db;
    leveltype level;

    /*
     * disable interrupts and select the interface
     */
    level = raise_interrupt_level(NETS_DISABLE);
    select_if(hwidb);

    rsp_common_if_init(hwidb);

    /*
     * Enable interface reset and "aip_periodic" functions:
     */
    hwidb->reset = aip->funcs.reset;

    /* 
     * reset the interrupt level to its original value 
     */
    reset_interrupt_level(level);

    switch (aip->plimtype) {
      default:
      case ATM_PLIM_INVALID:	str = "invalid";	break;
      case ATM_PLIM_4B5B:	str = "4B5B";		break;
      case ATM_PLIM_SONET:	str = "SONET";		break;
      case ATM_PLIM_E3:		str = "E3";		break;
      case ATM_PLIM_DS3:	str = "DS3";		break;
    }
    sprintf(applique, "%s (%uMbps)", str, aip->plimrate);
    setstring((char**) &hwidb->applique_name, applique);
    return;
}

/*****************************************************************************
 * Name: aip_restart_init
 *
 * Description: Post AIP restart reinitialization
 *
 * Input:
 *   hwidb -- pointer to the hardware IDB for the interface.
 *
 * Return Value: None
 *
 *****************************************************************************/
void aip_restart_init (hwidbtype *hwidb)
{
    slottype* slotp = hwidb->slot_ptr;

    if (!(hwidb->oir_flags & EOIR_RESET)) {
	qa_write_attn(get_queue_number(slotp->cmd_q_ptr), QA_ATTN_NONE);

	/* Re-establish the CCB. */
	send_ip_ccb_ptr(slotp);
	get_ip_ccb_status(slotp);

	set_qa_ip_attention(get_queue_number(slotp->cmd_q_ptr),
			    slotp->slot_num);

	/* Re-initialize the controller and it's one and only interface. */
	aip_ctrlr_init(slotp);
	aip_if_init(hwidb);
    }
}

/*****************************************************************************
 * Name: aip_get_nlpidtype
 *
 * Description: platform-specific retrieval of NLPID from MEMD
 *		Only valid for use when the atmtype is ATM_ENCTYPE_NLPID
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for this interface
 *   encaps -- pointer to the incoming atm encapsulation
 *
 * Return Value: nlpidtype value (ushort)
 *
 *****************************************************************************/

static ushort aip_get_nlpidtype (hwidbtype *hwidb, uchar *encaps)
{
    atm_aal5nlpid_t	*atm;

    atm = (atm_aal5nlpid_t *) encaps;
    return(atm->nlpid);
}

/*****************************************************************************
 * Name: aip_sniff
 *
 * Description: RSP specific routine "sniff" words from MEMD.
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for this interface (unused)
 *   pak -- pak pointer (unused)
 *   data -- pointer to beginning of packet in MEMD.
 *   sniff_buffer -- buffer for sniffed data (unused)
 *   offset -- offset in 16-bit words from rxoffset to data to sniff
 *   num_shorts -- number of 16-bit words to sniff (unused)
 *
 * Note:
 *   Unlike 7000, MEMD is directly accessible on RSP, so all we have to
 *   do is return pointer based on data pointer & offset.
 *
 * Return Value: None
 *
 *****************************************************************************/
ushort *
aip_sniff (hwidbtype *hwidb, paktype *pak, uchar *data,
           ushort *sniff_buffer, int offset, int num_shorts)
{
    return((ushort *)data + offset);
}

/*****************************************************************************
 * Name: send_aip_cmdrev_cmd
 *
 * Description: Send the current AIP H/W Rev. to the AIP F/W.
 *	Works around a H/W bug.
 *
 * Input: 
 *   hwidb -- a pointer to the hardware IDB for the interface
 *
 * Return Value: 
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise and error code.	
 *   *NOTE* Status MUST be ignored, on return, for backward compatibility.
 *
 *****************************************************************************/
ushort send_aip_setver_cmd (hwidbtype *hwidb)
{
    ushort status;

    send_if_cmd_1arg(hwidb, CCB_CMD_AIP_SETVER, hwidb->slot_ptr->hw_version);
    status = get_if_status(hwidb);

    return(status);
}

/*
 * invoked when an AIP has been removed from the system
 */
static void
aip_ctrlr_removed (slottype *sp)
{
    if (sp->interface_list[0]) {
	--nnets;
	--natmnets;
	--nAIPnets;
    }
    --nAIPs;
    return;
}

/*
 * display count of AIPs in the system
 */
static void
aip_print_controllers (void)
{
    if (nAIPs)
	printf("%u AIP controller%s (%u ATM).\n",
	       ArgAndPlural(nAIPs,"","s"), nAIPnets);
    return;
}

/*****************************************************************************
 * Name: aip_platform_init
 *
 * Description: platform-specific AIP init
 *
 * Input: 
 *   subsys -- pointer to the sybsystype for this subsystem
 *
 * Return Value: None
 *
 *****************************************************************************/
void aip_platform_init (subsystype *subsys)
{
    nAIPs = 0;
    nAIPnets = 0;

    reg_add_ctrlr_analyze(FCI_AIP_CONTROLLER,
			  aip_ctrlr_analyze, "aip_ctrlr_analyze");
    reg_add_ctrlr_init(FCI_AIP_CONTROLLER, aip_ctrlr_init, "aip_ctrlr_init");
    reg_add_if_init(FCI_AIP_CONTROLLER, aip_if_init, "aip_if_init");
    reg_add_atm_get_nlpidtype(aip_get_nlpidtype, "aip_get_nlpidtype");
    reg_add_ctrlr_removed(FCI_AIP_CONTROLLER,
			  aip_ctrlr_removed, "aip_ctrlr_removed");
    reg_add_print_controllers(aip_print_controllers,
			      "aip_print_controllers");
}

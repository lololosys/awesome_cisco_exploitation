/* $Id: rsp_cip.c,v 3.7.8.11 1996/08/28 13:13:25 thille Exp $
 * $Source: /release/112/cvs/Xsys/src-rsp/rsp_cip.c,v $
 *------------------------------------------------------------------
 * rsp_cip.c: RSP specific code for the Channel Interface Processor
 *
 * February 1995 Year, John Foster
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rsp_cip.c,v $
 * Revision 3.7.8.11  1996/08/28  13:13:25  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.7.8.10  1996/08/26  18:25:48  kmitchel
 * CSCdi66467:  CIP love letter stats not displayed after 49 days of uptime
 * Branch: California_branch
 *
 * Revision 3.7.8.9  1996/08/18  23:01:13  kmitchel
 * CSCdi64004:  CPU utilisation in show controller cxbus is not shown for
 *              CIP
 * Branch: California_branch
 *
 * Revision 3.7.8.8  1996/08/13  02:24:06  gstovall
 * CSCdi39484:  Giant packets output on ethernet interface
 * Branch: California_branch
 * Make sure we drop giants, even if they are just a wee bit too big.
 *
 * Revision 3.7.8.7  1996/08/07  18:01:53  kmitchel
 * CSCdi49854:  DBUSINTERR after RSP dbus write; ineffective recvry of
 * disabled card
 * Branch: California_branch
 *
 * Revision 3.7.8.6  1996/07/23  13:28:41  thille
 * CSCdi63329: Centralize logging for getbuffer() and pak_duplicate()
 * failures
 * Branch: California_branch
 * Shamelessly rip off Mr. Katz' code from CSCdi36037.  Provide
 * centralized logging for getbuffer() and pak_duplicate() failures and
 * then rip out the 9 bazillion different messages that where in the
 * various callers to report the failure.
 *
 * Revision 3.7.8.5  1996/05/22  13:51:30  getchell
 * CSCdi57387:  corrupt pool pointer, VIP2 dies, box not healthy;VIP2
 * removed
 * Branch: California_branch
 *
 * Revision 3.7.8.4  1996/05/17  11:54:19  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.5.4.3  1996/04/03  21:42:40  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.5.4.2  1996/03/21  06:10:38  ppearce
 * IBU modularity: Channel subblock
 * Branch: IbuMod_Calif_branch
 *   Move Channel fields from hwidbtype/idbtype structs to Channel subblock
 *
 * Revision 3.5.4.1  1996/03/17  18:31:00  ppearce
 * Sync IbuMod_Calif_branch to V111_1_3
 *
 * Revision 3.7.8.3  1996/05/09  14:46:08  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.7.8.2.14.1  1996/04/27  07:25:52  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.7.8.2.4.1  1996/04/08  02:09:23  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.7.8.2  1996/03/29  19:28:10  banderse
 * CSCdi51733:  CIP interfaces accumulate after OIR in RSP
 * Branch: California_branch
 * Added decrement of nchannelnets to cip_ctrlr_removed.
 *
 * Revision 3.7.8.1  1996/03/18  22:01:50  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.7.12.1  1996/03/28  17:36:16  rbadri
 * Branch: LE_Syn_Cal_V111_1_3_branch
 * MTU support for sub-interfaces (used by ATM interfaces)
 *
 * Revision 3.7  1996/02/28  19:48:23  motto
 * CSCdi45011:  Get hung output on CIP virtual interface
 *
 * Revision 3.6  1996/02/27  17:04:16  kmitchel
 * CSCdi43189:  Overzealous debug love output
 *
 * Revision 3.5  1996/02/08  23:11:14  jjohnson
 * CSCdi48362:  Need a consistent, global mechanism for writing plural
 * strings.  Standardize on the Plural and ArgAndPlural macros
 *
 * Revision 3.4  1996/02/01  06:09:38  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.3.26.1  1996/01/24  23:03:38  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.3  1995/11/17  18:47:34  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:32:51  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:27:40  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.10  1995/11/08  21:28:06  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.9  1995/10/05  18:43:13  motto
 * CSCdi40680:  OIR with CIP internal LANs, Not all internal LANs get
 * added back
 *
 * Revision 2.8  1995/09/17  20:55:20  motto
 * CSCdi40168:  IPC does not work with a CIP in a RSP (c75xx) router
 *
 * Revision 2.7  1995/09/11  10:24:16  kmitchel
 * CSCdi25909:  Erroneous CIP configuration commands are still added to
 *              config file
 *
 * Revision 2.6  1995/08/12  07:10:10  smackie
 * Repair various buffer and memory display foibles. (CSCdi37152)
 *
 *   o Unify memory dump code to one simple general purpose routine
 *   o Enhance buffer debugging by extending buffer display commands
 *   o Show all private buffer pools (so that BRI pools show up)
 *
 * Revision 2.5  1995/08/09  21:33:32  ppearce
 * CSCdi38300:  Add CSNA packet switching to 75xx (RSP)
 *
 * Revision 2.4  1995/07/18  06:48:55  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi37217]
 *
 * Revision 2.3  1995/07/14 00:53:15  schaefer
 * CSCdi37087: nnets not adjusted for new online-inserted IPs
 *
 * Revision 2.2  1995/06/29  03:02:56  foster
 * added call to init_new_hwidb
 * CSCdi36116:  CIP interfaces unnammed
 *
 * Revision 2.1  1995/06/07  22:57:36  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "plural.h"
#include "interface_private.h"
#include "../hes/cbus_registry.h"
#include "sys_registry.h"
#include "media_registry.h"
#include "rsp_common_if.h"
#include "../hes/if_mci.h"
#include "../if/fddi.h"
#include "../hes/if_fci.h"
#include "../hes/if_cip.h"
#include "../if/channel.h"
#include "../hes/channel_sb.h"
#include "../if/ether.h"
#include "rsp_bufhdr.h"
#include "rsp_if.h"
#include "lovenote.h"

#include "../hes/ucode.h"
#include "../dev/flash_dvr_spec.h"
#include "../hes/slot.h"
#include "../hes/dbus.h"
#include "../hes/if_cmd.h"
#include "logger.h"
#include "../os/free.h"
#include "../hes/if_hes_common.h"
#include "../h/packet.h"
#include "../ui/debug.h"
#include "../if/channel_debug.h"
#include "rsp_macaddr.h"
#include "../hes/msg_cbus.c"		/* Not a typo, see logger.h */
#include "msg_rsp.c"			/* Not a typo, see logger.h */
#include "../if/network.h"
#include "../parser/parser_defs_parser.h"

static uchar nCIPs, ncip_channels;
static uchar nCIP2s, ncip2_channels;

const char cip_show_util_indentation[] = "    ";


/*****************************************************************************
 * Name: cip_ilan_fastsend
 *
 * Description: fast send vector for CIP internal LANs
 *   - perform ilan vencap and redirect to the CIP virtual
 *     hwidb.
 *
 * Input:  output hwidb (CIP internal LAN)
 *         pak - fast switch frame
 *
 * Return Value: None
 *****************************************************************************/
static void cip_ilan_fastsend (hwidbtype *oidb, paktype *pak)
{
    hwidbtype *vidb = oidb->output_if_hw;
    
    if (vidb) {
        pak->linktype = LINK_SRB;
        /*
	 * Add the channel encaps for a CIP internal LAN frame
	 */
        if (oidb->vencap(pak,-1)) {
            /*
             * Send to the CIP using the CIP virtual interface hwidb.
             */
            pak->if_output = vidb->firstsw;
            (*vidb->fastsend)(vidb, pak);
            return;
        }
    } else {
        if (channel_debug_ilan) {
            buginf("\nCIP ILAN(%s) not registered with virtual interface"
                   " - cip_ilan_fastsend", oidb->hw_namestring);
        }
    }

    /*
     * If we didn't call the fastsend vector, then be sure to free the buffer
     */
    if (pak->buffer_header) {
	rsp_fs_free_memd_pak(pak);
    } else {
	datagram_done(pak);
    }
}


/*****************************************************************************
 * Name: cip_fs_setup
 *
 * Description: Setup cip ilan interface for fastswitching
 *
 * Input: hwidb
 *   
 *
 * Return Value: NONE
 *
 *****************************************************************************/
void cip_fs_setup (hwidbtype *hwidb) {

    if (is_cip_lan(hwidb)) {
        hwidb->fastsend = cip_ilan_fastsend;
    }
}


/*****************************************************************************
 * Name: receive_encaps_pak
 *
 * Description: Receive and enqueue a data buffer
 *   -these are data buffers coming from an IP that are
 *    not part of the interface traffic, for example
 *    love letters, configuration acknowledgements, and
 *    IPC messages.  These packets should not be reflected
 *    in the interface accounting values.
 *
 * Input: hwidb
 *        pointer to the data
 *        link_type (for enqueueing)
 *        length of the data
 *        debug flag
 *
 * Return Value: void
 *****************************************************************************/
static void receive_encaps_pak (
   hwidbtype  *hwidb,
   uchar      *loveletter,				     
   ushort      link_type,
   ushort      bytes)
{
    paktype *pak;
    
    pak = getbuffer(bytes);
    if (pak == NULL) {
	return;
    }
    bcopy(loveletter, pak->datagramstart, bytes);
    
    pak->linktype = link_type;
    pak->datagramsize = bytes;
    pak->if_input = hwidb->firstsw;
    reg_invoke_raw_enqueue(link_type, pak);
}
/*****************************************************************************
 * Name: channel_encaps_known
 *
 * Description: Processes packets coming from the CIP of the following types:
 *              ENCAPS_LOVE   love letters 
 *              ENCAPS_CFGACK cfg acknowledgments
 * Input:       hwidb, 
 *              pointer to the data (including the encapsulation)
 *              encapsulation type (one of the above)
 *              length of the data (including the encapsulation)
 *
 * Return Value: TRUE if the encapsulation is known
 *               FALSE otherwise
 *****************************************************************************/
static boolean channel_encaps_known (
   hwidbtype  *hwidb,
   uchar      *loveletter,				     
   ushort      encaps,
   ushort      bytes)
{
    ushort link_type;
    boolean debuggery;
    cip_idb_infotype *cip;

    /*
     * we skip over the encaps of 0x0000dead at the begining of the 
     * love letter buffer.  Note that the SP does this in the RP case
     * so we do it here for the RSP.  Skip over the vcnum as well.
     * This header is defined in channel.h
     */
    
    loveletter += CHANNEL_ENCAPBYTES;
    bytes -= CHANNEL_ENCAPBYTES;

    /* Return FALSE if we don't understand this encapsulation */
    switch (encaps) {
    case CHANNEL_ENCAPS_LOVE:
	debuggery = channel_debug_love && channel_debug_encaps;
	link_type = LINK_LOVE;
	break;
    case CHANNEL_ENCAPS_CFGACK:
        cip = channel_sb_get_cip_info(hwidb);
	if (cip && cip->cip_encaps)
	    (*cip->cip_encaps)(hwidb, loveletter, bytes);
	return TRUE;
    default:
	debuggery = channel_debug;
	link_type = LINK_MAXLINKTYPE;
	break;
    }
    if (debuggery)
	buginf("\n%s: Encaps %04X message received, bytes %d",
	       hwidb->hw_namestring,
	       encaps,
	       bytes);
    
    if (link_type == LINK_MAXLINKTYPE) {
	if (debuggery)
	    buginf("-- Unknown encapsulation -");
	return FALSE;
    }
    receive_encaps_pak(hwidb, loveletter, link_type, bytes);
    return TRUE;
}

/*****************************************************************************
 * Name: cip_love_letter
 *
 * Description: platform specific love letter routine
 *
 * Input: 
 *   
 *
 * Return Value: None
 *
 *****************************************************************************/
void cip_rsp_love_letter (hwidbtype *hwidb, lovenotetype *lovenote)
{
    ulong    length = lovenote->data_length;
    uchar   *loveletter;
    
    if (lovenote->buffer_ptr) {           /* love letter */
	loveletter = MEMD_OFFSET_TO_ADDR(lovenote->buffer_ptr);
	channel_encaps_known(hwidb, loveletter,
			     ((channel_hdr *)loveletter)->type,
			     length);
    } else {                              /* love note   */
	if (channel_debug_love)
	    buginf("\n%s: love note received, bytes %d",
		   hwidb->hw_namestring, length);
	
	if (lovenote->love_code & LOVE_CODE_CARRIER_CHANGE) {
	    cbus_mci_carrier(hwidb);
	}
	/*
	 * XXX This is just a bit of policing to make sure
	 * implemented the new length field policy.
	 * there should be no legth if lovenote->buffer_ptr is zero
	 *  after testing this code should be removed.
	 */
	if (length) {
	    buginf("\nCIP love note has non-zero length field"
		   "\n  %08x %08x %08x %08x",
		   ((ulong*) lovenote)[0], ((ulong*) lovenote)[1],
		   ((ulong*) lovenote)[2], ((ulong*) lovenote)[3]);
	    lovenote->data_length = 0;
	}
    }
}


/*****************************************************************************
 * Name: cip_restart_init
 *
 * Description: Post CIP restart reinitialization, we don't need
 * to resend anything on the RSP, because the CIP remembers it
 * accros i/f resets.  If things have actually changed (e.g. MEMD
 * has been recarved) then we rely on that code to call the
 * cip_if_init() routine like it does at boot time.
 *
 * Input:
 *   hwidb -- pointer to the hardware IDB for the interface.
 *
 * Return Value: None
 *
 *****************************************************************************/
void cip_restart_init (hwidbtype *hwidb)
{
}


/*****************************************************************************
 * Name: cip_ctrlr_analyze
 *
 * Description: analyze part of initialization for a CIP
 *   This function is called when an board is first discovered and initialized.
 *   It is called the first time the board is plugged in, or when the router
 *   is resstarted.  It is *NOT* called for boards that are installed and
 *   running when a new board is inserted.
 *
 *   This function is responsible for discovering the interfaces
 *   on the CIP (via the CBUS -- select command), and creating
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

static void cip_ctrlr_analyze (slottype *slot)
{
    hwidbtype	*hwidb;
    int		port_num;
    ushort	status;
    ushort	interface_type;
    hwidbtype   *ilans;
    int         interfaces;
    boolean     this_is_cip2_pslot;
    
    interfaces = 0;
    this_is_cip2_pslot = is_cip2_hw(slot->slot_num);
    /*
     * We loop through the interfaces on the CIP selecting them as we go. 
     * We use the select command to tell if the interface is there.  If the
     * select command is successful, it returns the interface type (which
     * should be FCI_TYPE_ATM).  Yes, sigh, ATM FCI_TYPE_ATM is used for 
     * both AIP and CIP port types.  If the select commands fails, or the 
     * response  says it is not an ATM/CIP, we break out of this loop and 
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
	if (interface_type == FCI_TYPE_UNKNOWN){
	    slot->interface_list[port_num] = NULL;
            continue;
	}

	if (interface_type == FCI_TYPE_OUT_OF_RANGE)
	    break;
	if (interface_type != FCI_TYPE_ATM) {
	    errmsg(&msgsym(PORTTYPE,CBUS), slot->card_name, slot->slot_num,
		   port_num, interface_type);
	    break;
	}

	/*
	 * Get the IDB for the CIP interface
	 * If we find a matching one, we can just use it,
	 * otherwise we init a new one.
	 */

	hwidb = rsp_get_matching_idb(slot, port_num,
				     interface_type, IDBTYPE_CIP);
	if (hwidb != NULL) {
	                        /* rsp_get_matching_idb() complains */
	                        /* about malloc() failure */

	    slot->interface_list[port_num] = hwidb;
	    slot->interface_count = 1 + port_num;

	    /*
	     * Protect ourselves in case anybody tries to reset us
	     * before MEMD is carved:
	     */
	    hwidb->reset = (reset_t) return_nothing;
	    hwidb->periodic = NULL;

	    interfaces++;
	    
	    if (hwidb->status & IDB_NOHARDWARE) {
		hwidb->status &= ~IDB_NOHARDWARE;
		continue;
	    }

            /*
             * Save and restore the internal LANS chained
             * off the virtual CIP interface. hes_idb_init_before
             * NULLs this variable out.
             */
            ilans = hwidb->next_idb_on_physint;
	    hes_idb_init_before(hwidb);
            hwidb->next_idb_on_physint = ilans;

	    /*
	     * RSP-specific CIP IDB initialization.
	     */

	    hwidb->status = IDB_CHANNEL | FCI_IDBFLAGS;

	    /* CHANNELTODO - find best value for this */
	    hwidb->max_pak_size_def = FDDI_MAXDGRAM;
	    if_maxdgram_adjust(hwidb, FDDI_MAXDGRAM);
	    hwidb->set_maxdgram =  cbus_mci_set_maxdgram;
	    hwidb->maxmtu_def = hwidb->maxmtu = CIP_DEFAULT_MTU;
	    hwidb->firstsw->sub_mtu = hwidb->maxmtu_def;
	    hwidb->name = IFNAME_CHANNEL;

	    /* 
	     * hwidb->reset is set, so this call  has to got before the 
	     * code below
	     */
	    channel_idb_init(hwidb);          /* this call has to before */
					      /*  the two lines below    */ 
	    /* as channel_idb_init sets hwidb->reset and hwidb->periodic */

	    /*
	     * Protect ourselves in case anybody tries to reset us
	     * before MEMD is carved:
	     */
	    hwidb->reset = (reset_t) return_nothing;
	    hwidb->periodic = NULL;
	    

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
	    hwidb->rxoffset = ENCAPBYTES;

	    /*
	     * XXX The following need to be fixed up to reflect the
	     * RSP's byte policy (instead of words).
	     */
	    hwidb->span_macoffset = hwidb->rxoffset;
	    hwidb->span_encapsize = hwidb->encsize;

	    hwidb->love_note = (void *)cip_rsp_love_letter;

	    /*
	     * Complete set up of common fields.
	     */
	    init_new_hwidb(hwidb);
	    hes_idb_init_after(hwidb);
	}
    }

    nchannelnets += interfaces;
    
    if (this_is_cip2_pslot) {
	++nCIP2s;
	ncip2_channels += interfaces;
    } else {
	++nCIPs;
	ncip_channels += interfaces;
    }
    return;
}


/*****************************************************************************
 * Name: cip_if_init
 *
 * Description: per-interface initialization for the CIP
 *   This function is called during the initialization phase (after analyze)
 *   and after the configuration has been parsed.  The fields in the slottype
 *   have been filled in -- the information needs to be sent to the CIP now. 
 *   It may also be necessary to pass global information to the CIP.
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
 *
 *****************************************************************************/
static void cip_if_init (hwidbtype *hwidb)
{
    leveltype level;

    level = raise_interrupt_level(NETS_DISABLE);

    select_if(hwidb);

    rsp_common_if_init(hwidb);

    send_rxbytes_cmd(hwidb,hwidb->max_buffer_size);
    send_rx_offset_cmd(hwidb,hwidb->rxoffset);

    hwidb->reset = cip_reset;
    hwidb->periodic = NULL;

    /* 
     * If Virtual CIP port is being initialized 
     *   Update CIP ILAN idb(s) under the Virtual Channel port also
     *
     */
    if (is_cip_virtual(hwidb)) {
        hwidbtype *ilan_hwidb = hwidb->next_idb_on_physint;
        while (ilan_hwidb) {
            if (is_cip_lan(ilan_hwidb)) {
                cip_copy_idb_id_info(hwidb, ilan_hwidb);
                cip_copy_idb_info(hwidb, ilan_hwidb);
            }
            ilan_hwidb = ilan_hwidb->next_idb_on_physint;
        }
    }

    reset_interrupt_level(level);
    return;
}
/*****************************************************************************
 * Name: cip_ctrlr_init
 *
 * Description: per-card initialization for the CIP
 *   This function is called during the initialization phase (after analyze)
 *   and after the configuration has been parsed.  The fields in the slottype
 *   have been filled in -- the information needs to be sent to the CIP now. 
 *   It may also be necessary to pass global information to the CIP.
 *   The algorithm is 
 *
 *       Send Set Global Queue Ptrs command
 *       Ensure the max love letter length is within allowable limits
 *
 * Notes: 
 *   This function is *NEVER* called from interrupt level.
 *   CIP requires a send_global_ptrs_cmd for all port stuffed on the card
 *   otherwise this would just be a call to rsp_common_ctrlr_init
 *
 * Input: 
 *   slot -- pointer to a slottype structure for the IP.  All fields should be
 *     valid when this function is called, but specifically
 *       slot_num -- slot number for this card
 *       ccb_ptr -- pointer to the CCB for the EIP
 *       cmd_q_ptr -- pointer to the CCB for the EIP
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
static void cip_ctrlr_init (slottype *slotp)
{
    int port_num;
    ushort interface_type;

    if (slotp->interface_count == 0)
	return;

    for (port_num=0;  port_num < slotp->interface_count;  port_num++){
	if(!slotp->interface_list[port_num])
	    continue;

	send_select_cmd(slotp, port_num, &interface_type);
	rsp_common_ctrlr_init(slotp);
    }
    return;
}

/*
 * channel_periodic
 * Called from periodic background task.
 * this is never called, included here to make the link work and still 
 * avoid evil, shameful #ifdefs
 */
void channel_periodic (hwidbtype *hwidb)
{

}

/*
 * The 7500 does not require any interfaces to not be in the ADMINDOWN 
 * state for IPC to function to that slot.
 */
boolean ipc_avail_when_interfaces_shut (void) 
{
    return (TRUE);
}


/*
 * invoked when a CIP has been removed from the system
 */
static void
cip_ctrlr_removed (slottype *sp)
{
    int ix;
    int interfaces;
    boolean this_is_cip2_pslot;
    
    interfaces = 0;
    this_is_cip2_pslot = is_cip2_hw(sp->slot_num);

    for (ix = 0; ix < sp->interface_count; ++ix) {
	if (sp->interface_list[ix]) {
	    interfaces++;
	}
    }
    
    nnets -= interfaces;
    nchannelnets -= interfaces;
    
    if (this_is_cip2_pslot) {
	--nCIP2s;
	ncip2_channels -= interfaces;
    } else {
	--nCIPs;
	ncip_channels -= interfaces;
    }
    return;
}

/*
 * display count of CIPs in the system
 */
static void
cip_print_controllers (void)
{
    if (nCIPs)
	printf("%u CIP controller%s (%u IBM Channel%s).\n",
	       ArgAndPlural(nCIPs,"","s"),
	       ArgAndPlural(ncip_channels,"","s"));
    if (nCIP2s)
	printf("%u CIP2 controller%s (%u IBM Channel%s).\n",
	       ArgAndPlural(nCIP2s,"","s"),
	       ArgAndPlural(ncip2_channels,"","s"));
    return;
}

/*****************************************************************************
 * Name: cip_platform_init
 *
 * Description: platform specific CIP init
 *
 * Input: 
 *   subsys -- pointer to the subsystype for this subsystem
 *
 * Return Value: None
 *
 *****************************************************************************/
void cip_platform_init (subsystype *subsys)
{
    nCIPs = 0;
    ncip_channels = 0;

    nCIP2s = 0;
    ncip2_channels = 0;

    reg_add_ctrlr_analyze(FCI_CIP_CONTROLLER, cip_ctrlr_analyze,
			  "cip_ctrlr_analyze");
    reg_add_ctrlr_init(FCI_CIP_CONTROLLER, cip_ctrlr_init, "cip_ctrlr_init");
    reg_add_if_init(FCI_CIP_CONTROLLER, cip_if_init, "cip_if_init");
    reg_add_ctrlr_removed(FCI_CIP_CONTROLLER, cip_ctrlr_removed, 
			  "cip_ctrlr_removed");
    reg_add_print_controllers(cip_print_controllers,
			      "cip_print_controllers");
    return;
}

/*
 *
 * fill in router serial number
 */

void cip_chassis_serial_init (cip_nodeidtype *cip_nodeid)
{
    sprintf(cip_nodeid->sequence, "%u", chassis_serial);

}


/*****************************************************************************
 * Name: cip_encaps_deed
 *
 * Description: Process data coming from the CIP
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface.
 *   src   -- start of data (RSP), NULL otherwise
 *   bytes -- length of incoming data
 *
 * Return Value: None
 *
 *****************************************************************************/
void cip_encaps_deed (hwidbtype *hwidb, void *src, ushort bytes)
{
    plevel_t         *plevel_element;
    cip_idb_infotype *cip;
    ulong             code, data;
    ulong            *p;
   
    cip = channel_sb_get_cip_info(hwidb);
    if (!cip) {
        return;
    }

    plevel_element = cip_plevel_element_get(cip, PLEVEL_PREQ);
    
    if (!plevel_element) {
	/* Gotta reset the CIP */
	p = (ulong *)src;
	code = *p++;
	data = *p;
		
	errmsg(&msgsym(CIPENCAP, CBUS), 
	       hwidb->hw_namestring, code, data);

	platform_req_interface_restart(hwidb);
	return;
    }

    /* Validate length of buffer before copying it in */
    if (bytes > PLEVENT_DSIZE) {
	errmsg(&msgsym(BADENCAPSZ, CBUS), 
	       hwidb->hw_namestring, bytes, PLEVENT_DSIZE);
	bytes = PLEVENT_DSIZE;
    }

    bcopy(src, plevel_element->pl_data, bytes);
    cip_plevel_enqueue(hwidb, cip, plevel_element, bytes);
}


/*****************************************************************************
 * Name: hwidb_lookup
 *
 * Description: return the hwidb for the interface <slot>/<port>
 *
 * Input: 
 *   slot, port
 *
 * Return Value: hwidb or NULL 
 *
 *****************************************************************************/
hwidbtype *
hwidb_lookup (ushort slot, ushort port) {

    hwidbtype *hwidb;
    slottype *slotp;

    hwidb = NULL;
    
    if (slot < MAX_SLOTS &&
	port < MAX_CTRLR_INTERFACES) {
	slotp = &slots[slot];
	hwidb = slotp->interface_list[port];
    }
    return hwidb;
}

/*****************************************************************************
 * Name: cip_version_info
 *
 * Description: return the hw and sw version numbers if a CIP is in designated
 *              slot
 * Input: 
 *   slot
 *
 * Return Value: TRUE if a CIP is in slot
 *
 *****************************************************************************/
boolean
cip_version_info (ushort slot, ushort *hw_version, ushort *sw_version) {
    
    slottype *slotp;
    
    if (slot < MAX_SLOTS) {
	slotp = &slots[slot];
	if (slotp &&
	    (slotp->ctrlr_type == FCI_CIP_CONTROLLER)) {
	    *hw_version = slotp->hw_version;
	    *sw_version = slotp->sw_version;
	    return TRUE;
	}
    }
    return FALSE;
}

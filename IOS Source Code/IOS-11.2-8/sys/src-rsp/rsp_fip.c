/* $Id: rsp_fip.c,v 3.7.4.21 1996/08/28 13:13:48 thille Exp $
 * $Source: /release/112/cvs/Xsys/src-rsp/rsp_fip.c,v $
 *------------------------------------------------------------------
 * rsp_fip.c --    The FIP specific code that was in if_fci.c
 *                 but only the routines that are further platform
 *                 specific.
 *
 * September, 1989,  Robert Fletcher
 *
 * Copyright (c) 1988-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rsp_fip.c,v $
 * Revision 3.7.4.21  1996/08/28  13:13:48  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.7.4.20  1996/08/26  15:11:39  ppearce
 * CSCdi66582:  Implement SRB vector table to cut size of SRB_CORE subsys
 * Branch: California_branch
 *
 * Revision 3.7.4.19  1996/08/20  00:35:59  cheilek
 * CSCdi58374:  More show diag info needed
 * Branch: California_branch
 * Add CCB_CMD_VIP_GET_CONFIG, vip_ll_sw.
 * Add sw_version_str to slot structure.
 *
 * Revision 3.7.4.18  1996/08/16  00:50:20  ogrady
 * CSCdi54594:  SRB on FDDI propogates broadcast storms
 *              Branch: California_branch
 *              Very small bridge packets not stripped from ring.  Make
 *              routine that inits FIP hold register common instead of
 *              platform specific.
 *
 * Revision 3.7.4.17  1996/08/13  02:24:13  gstovall
 * CSCdi39484:  Giant packets output on ethernet interface
 * Branch: California_branch
 * Make sure we drop giants, even if they are just a wee bit too big.
 *
 * Revision 3.7.4.16  1996/07/17  01:45:16  fsunaval
 * CSCdi59824:  RIF classification done on the VIP for the 1F.
 * Branch: California_branch
 * 1.Off-load RIF classification from the RSP to the VIP for the 1F.
 *   What's next, fragmentation?
 *
 * Revision 3.7.4.15  1996/07/09  06:04:47  ppearce
 * CSCdi59527: inlines waste code space
 * Branch: California_branch
 *   Just say "no" to inline abuse - SRB subblock inlines removed
 *
 * Revision 3.7.4.14  1996/06/25  18:05:43  kao
 * CSCdi60971:  FDDI port adapter in PA0 stops working after EOIR
 * Need to send UNQUIESCE command for EOIR reset
 * Branch: California_branch
 *
 * Revision 3.7.4.13  1996/06/18  20:05:56  fsunaval
 * CSCdi59153:  Provide support for cmt con and cmt dis.
 * Branch: California_branch
 * 1."cmt con" and "cmt dis" put into platform specific files.
 *
 * Revision 3.7.4.12  1996/06/10  02:01:16  fsunaval
 * CSCdi57738:  VIP2 SW: FDDI PA doesnt show claim, beacon counter at RSP
 * level.
 * Branch: California_branch
 * 1.Fix the claim, beacon and trace counters.
 * 2.Make the MIB in the RSP aware about single/multimode interface.
 * 3.Remove excess debugging stuff.
 *
 * Revision 3.7.4.11  1996/06/07  00:34:17  kao
 * CSCdi59481:  4R does not work when hot swapped
 * CSCdi59000:  FDDI PA line status is always down after an eoir event
 * Branch: California_branch
 *
 * Revision 3.7.4.10  1996/06/01  16:09:09  kao
 * CSCdi58054:  SegV exception, PC 0x60187794
 * Branch: California_branch
 *
 * Revision 3.7.4.9  1996/06/01  01:32:25  hampton
 * Remove unnecessary cross module references.  [CSCdi59221]
 * Branch: California_branch
 *
 * Revision 3.7.4.8  1996/05/17  11:54:23  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.6.4.4  1996/05/05  23:53:31  ppearce
 * Sync to IbuMod_Calif_baseline_960504
 *
 * Revision 3.6.4.3  1996/04/26  15:24:22  ppearce
 * IBU modularity - SRB subblock (part 1)
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.6.4.2  1996/04/03  21:42:54  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.6.4.1  1996/03/17  18:31:04  ppearce
 * Sync IbuMod_Calif_branch to V111_1_3
 *
 * Revision 3.7.4.7  1996/05/09  14:46:10  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.7.4.6  1996/05/08  04:32:14  tboyle
 * CSCdi54363:  SW-VIP2: Dropped packets are unaccountable in FDDI-PA.
 * Branch: California_branch
 *
 * Revision 3.7.4.5  1996/05/06  19:27:29  fsunaval
 * CSCdi56768:  Update trt_neg
 * Branch: California_branch
 * 1.Pass one more parameter throught the MIB loveletter.
 *
 * Revision 3.7.4.4  1996/05/01  09:15:48  shaker
 * CSCdi54444:  Booting causes FIP FDDI to beacon
 * Branch: California_branch
 * Wait around in the process that starts CMT for it to finish
 * so that we know that no other code disables interrupts,
 * preventing us from bringing the MAC online, causing the
 * ring to beacon.
 *
 * Revision 3.7.4.3  1996/04/30  07:01:00  fsunaval
 * CSCdi51250:  SW-VIP1: Host MIB updating needs to be completed
 * Branch: California_branch
 * 1.Code changes for formation and transfer of SMT Messages to RSP
 *   in form of love letters.
 *
 * Revision 3.7.4.2.16.1  1996/04/27  07:25:54  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.7.4.2.6.1  1996/04/08  02:09:25  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.7.4.2  1996/03/21  23:49:49  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.7.4.1  1996/03/18  22:02:07  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.2.6  1996/03/18  23:34:54  mbeesley
 * Branch: ELC_branch
 * Sync to V111_1_3
 *
 * Revision 3.3.2.5  1996/03/01  03:19:53  fsunaval
 * CSCdi50482:  Discrepancy in output error from show interface command
 * Branch: ELC_branch
 *
 * Revision 3.3.2.4  1996/01/25  12:10:32  fsunaval
 * Branch: ELC_branch
 * 1.Predator FDDI support.
 * 2.Patch CSCdi46358 (PA management interrupt) changes into ELC_branch.
 *
 * Revision 3.3.2.3  1996/01/16  02:35:25  dcarroll
 * Branch: ELC_branch
 * Sync ELC_branch between ELC_baseline_120995 and ELC_baseline_960110.
 *
 * Revision 3.3.2.2  1995/12/26  19:57:21  fsunaval
 * CSCdi46111:  Add parser support for FDDI PA and other changes
 * Branch: ELC_branch
 * 1.Add missing diffs from previous commit for init_pas.c
 *   (NMI interrupts).
 * 2.Add FDDI PA parser support (sh cont, test pas fddi).
 * 3.Do a little code cleanup of hes_fddi.c and rsp_fip.c specifically
 *   in regards to the way trt and tvx are passed down.
 *
 * Revision 3.3.2.1  1995/12/23  04:41:46  fsunaval
 * Branch: ELC_branch
 * 1.First pass at FDDI port adapter.
 *
 * Revision 3.7  1996/03/11  11:09:34  ogrady
 * CSCdi51224:  RSP emulation code is obsolete
 *              Expunge code.
 *
 * Revision 3.6  1996/02/08  23:11:19  jjohnson
 * CSCdi48362:  Need a consistent, global mechanism for writing plural
 * strings.  Standardize on the Plural and ArgAndPlural macros
 *
 * Revision 3.5.12.1  1996/01/24  23:03:59  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.5  1996/01/05  04:57:05  hampton
 * Remove extraneous includes of h/timer.h.  [CSCdi46482]
 *
 * Revision 3.4  1995/12/28  08:24:50  foster
 * CSCdi46157:  Dbus down-load needs to work on rsp2 slave down-load
 *
 * Revision 3.3  1995/11/17  18:48:13  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:32:57  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:27:59  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/11/08  21:28:20  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.3  1995/07/16  22:30:58  getchell
 * CSCdi35501:  src-rsp/ccb_cmd.h should be moved to hes
 * src-rsp/[ccb.h,ccb_cmd.h] combined and moved to hes/ccb_cmd.h.
 * rsp_command prototypes removed from ccb_cmd.h and moved to
 * src-rsp/rsp_commands.h.  Files including altered files modifed to
 * reflect new file structure.
 *
 * Revision 2.2  1995/07/14  00:53:20  schaefer
 * CSCdi37087: nnets not adjusted for new online-inserted IPs
 *
 * Revision 2.1  1995/06/07  22:57:59  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "logger.h"
#include "../hes/msg_cbus.c"		/* Not a typo, see logger.h */
#include <string.h>
#include <ciscolib.h>
#include "plural.h"
#include "sys_registry.h"
#include "../hes/cbus_registry.h"
#include "media_registry.h"
#include "interface_private.h"
#include "packet.h"
#include "../if/network.h"
#include "../if/fddi.h"
#include "../if/fddi_private.h"
#include "../if/if_fddi_public.h"
#include "../if/parser_defs_fddi.h"
#include "../hes/ucode.h"      
#include "../hes/if_fip.h"
#include "../hes/hes_fddi_public.h"
#include "../hes/if_cmd.h"
#include "../hes/ccb_cmd.h"
#include "rsp_common_if.h"
#include "rsp_pcmap.h"
#include "rsp_commands.h"
#include "../hes/if_hes_common.h"
#include "../hes/if_fci.h"
#include "../hes/if_mci.h"
#include "../src-rsp/rsp_if.h"
#include "../dev/flash_dvr_spec.h"
#include "../hes/slot.h"
#include "lovenote.h"
#include "../srt/srb_core.h"
#include "../srt/srb_core_registry.h"

#include "../xdi/xdihdrsif.h"
#include "../xdi/fddihdr.h"
#include "../xdi/csphdr.h"
#include "../xdi/cspglbl.h"
#include "../xdi/fbmhdr.h"
#include "../xdi/smtmsg.h"
#include "../xdi/mibdefs.h"
#include "../xdi/mibglbl.h"
#include "../xdi/fbmframe.h"
#include "../xdi/fbmglbl.h"
#include "../xdi/mibproto.h"
#include "../xdi/cspproto.h"
#include "../xdi/fbmproto.h"

#include "../if/if_fddi.h"
#include "../if/if_xdi_public.h"

#include "../smf/smf_public.h"
#include "../if/rif.h"
#include "../src-rsp/rsp_vip.h"
#include "../hes/if_vip.h"

/*
 * Externals
 */
extern void ProcessMAPMessageMulti(SMTMessage *);

static uchar nFIPs, nFIPnets;

queuetype fipclaimQ;

/*****************************************************************************
 * Name: fip_love_note
 *
 * Description: love note handler for FIPs
 *   Note the FIP is an old-fashioned IP, that uses the bit encoding
 *   form of lovenote codes.  The FIP generates the following events:
 *
 *	LOVE_CODE_PANIC
 *	LOVE_CODE_CARRIER_CHANGE
 *	LOVE_CODE_FIP_PHY_B
 *	LOVE_CODE_FIP_PHY_A
 *	LOVE_CODE_FIP_PHYA_JOIN
 *	LOVE_CODE_FIP_PHYB_JOIN
 *	LOVE_CODE_FIP_PHYA_SEND_BREAK
 *	LOVE_CODE_FIP_PHYB_SEND_BREAK
 *	LOVE_CODE_FIP_DUP_ADDRESS
 *     
 *  
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the FIP interface that
 *     generated the event (love note)
 *   lovenote -- pointer to the love note 
 *
 * Return Value: None
 *
 *****************************************************************************/

static void fip_love_note (hwidbtype *hwidb, lovenotetype *lovenote)
{
    if (fddi_debug) {
	buginf("\nfip_love_note(%s): love_code=0x%x",
	       hwidb->hw_namestring, lovenote->love_code);
    }

    if (lovenote->love_code & LOVE_CODE_PANIC) {
	interface_panic(hwidb, hwidb->hw_namestring);
    }

    if (lovenote->love_code & LOVE_CODE_CARRIER_CHANGE) {
	fddi_carrier(hwidb);
    }

    if (lovenote->love_code &
	(LOVE_CODE_FIP_PHYA_JOIN | LOVE_CODE_FIP_PHYB_JOIN |
	 LOVE_CODE_FIP_DUP_ADDRESS | LOVE_CODE_FIP_PHYA_SEND_BREAK |
	 LOVE_CODE_FIP_PHYB_SEND_BREAK) ) {
	fddi_smt_love_note_handler(hwidb, lovenote->love_code);
    }

    /*
     * XXX This is just a bit of policing to check for
     * the new length field policy. The FIP never sends 
     * love letters, so the length should be 0.
     */
    if (lovenote->data_length) {
	buginf("\nFIP love note has non-zero length field"
	       "\n  %08x %08x %08x %08x",
	       ((ulong*) lovenote)[0], ((ulong*) lovenote)[1],
	       ((ulong*) lovenote)[2], ((ulong*) lovenote)[3]);
	lovenote->data_length = 0;
    }
}

/*
 * cbus_vip_1f_reset:
 * Reset the MIF68840 fddi port adapter 
 */

void cbus_vip_1f_reset (hwidbtype *hwidb)
{
    leveltype level;
    ushort  *sptr;
    idbtype *swidb;
    ushort  status, ltemp; 
    boolean srbridge_on_hwidb;
    fddisb *fsb; 
    ulong srb_rng_br_rng_f;
    ulong srb_rng_br_rng_r;
    srb_vector_table_t *srbV_p;

    if (fddi_debug | fddi_cmt_events_debug) {
        buginf("\ncbus_vip_1f_reset(%s)", hwidb->hw_namestring);
    }

    /*
     * Are resets allowed yet?
     */
    if (nvsize && !system_configured) {
        return;
    }

    if (hwidb->oir_flags & EOIR_RESET) {
	level = raise_interrupt_level(NETS_DISABLE);
	status = select_if(hwidb);
	send_status_cmd(hwidb, FCI_STATUS_UNQUIESCE);
	send_rxbytes_cmd(hwidb, hwidb->max_buffer_size);
	reg_invoke_interface_cfg_modified(hwidb);
	TIMER_STOP(hwidb->xmittimer);
	reset_interrupt_level(level);
	return;
    }

    fsb = idb_use_hwsb_inline(hwidb, HWIDB_SB_FDDI);

    hwidb->counters.resets++;
    hwidb->status &= ~IDB_DISABLED;
    
    level = raise_interrupt_level(NETS_DISABLE);
    reg_invoke_bump_cache_version(hwidb, NULL, TRUE);
    reg_invoke_media_rif_reset(hwidb);
    status = select_if(hwidb);
    if (status != FCI_RSP_OKAY) {
        errmsg(&msgsym(FDDIRSET, CBUS), hwidb->hw_namestring, status,      
            "select");
        platform_req_card_restart(hwidb);
        reset_interrupt_level(level);
        idb_release_hwsb_inline(hwidb, HWIDB_SB_FDDI);
        return;
    }
    /*
     * Disable the interface and then give the firmware a chance
     * to finish the disable function.
     */
    status = send_if_disable_cmd_delay(hwidb, IF_DISABLE_TIME);
    if (status != FCI_RSP_OKAY) {
        errmsg(&msgsym(FDDIRSET, CBUS), hwidb->hw_namestring,
           status, "disable");
        platform_req_card_restart(hwidb);
        reset_interrupt_level(level);
        idb_release_hwsb_inline(hwidb, HWIDB_SB_FDDI);
        return;
    }

    /*
     * Flush hold queue -- ring down -- no Upstream or Downstream neighbors.
     */
    holdq_reset(hwidb);
    TIMER_STOP(fsb->fddi_operational);

    /*
     * Only overwrite hwidb->hardware if it is NULL:
     */
    if (GETSHORT(&hwidb->hardware[4]) == 0 &&
        GETSHORT(&hwidb->hardware[2]) == 0 &&
        GETSHORT(&hwidb->hardware[0]) == 0) {
        ieee_copy(hwidb->bia, hwidb->hardware);
    }

    /* Pass the canonical value, 6 bytes at a time */   
    sptr = (ushort *) hwidb->hardware;
    send_if_cmd_3arg(hwidb, CCB_CMD_VIP_1F_STN_ADD, *sptr,
        *(sptr+1), *(sptr+2));
    status = get_if_status(hwidb);
    if (status != FCI_RSP_OKAY)
        errmsg(&msgsym(FDDIRSET, CBUS),hwidb->hw_namestring, status,
            "stn_add cmd");

    /* Bit swap our canonical address to form the actual address. */
    ieee_swap(hwidb->hardware, hwidb->bit_swapped_hardware);
 
    /*
     * Adjust number of SMT buffers to respond to possible card insertion
     */
    adjust_smt_bufs();
 
    /*
     * Report new address to the FDDI SMT MIB
     */
    {
        SMTMessage msg;
        
        msg.destination = MIB_MSG_ID;
        msg.source = MAP_MSG_ID;
        msg.type = MIB_ACTION_CHANGE_MACADDR;
        msg.stnID = hwidb->fddi_stn_id;   
        msg.entity = 0;
        MCopyAddress (msg.data.addr, &hwidb->bit_swapped_hardware);
 
        SendMAPMessage(&msg);
    }
       
    /*
     * If interface is administratively down, don't re-enable
     */
    if (hwidb->state == IDBS_ADMINDOWN) {
        reset_interrupt_level(level);
        idb_release_hwsb_inline(hwidb, HWIDB_SB_FDDI);
        return;
    }

    hwidb->multibits = 0;
    srbridge_on_hwidb = FALSE;

    /*
     * Determine whether SRB is configured on the interface.
     */
    FOR_ALL_SWIDBS_ON_HW(hwidb, swidb)
    if (RUNNING_IF(LINK_SRB, swidb)) {
        srbridge_on_hwidb = TRUE;
        break;
    }

    /*
     * Turn on bridging on hwidb if any swidb has tbridging
     * configured. In other words go promiscuous. 
     * Also used for classification. 
     */   
    if (bridge_enable && (hwidb->tbridge_on_hwidb  || srbridge_on_hwidb)) {
        send_if_cmd_1arg(hwidb,CCB_CMD_VIP_1F_BRIDGE,0x1);
    } else {
        send_if_cmd_1arg(hwidb,CCB_CMD_VIP_1F_BRIDGE,0x0);
    }
    status = get_if_status(hwidb);
    if (status != FCI_RSP_OKAY)
        errmsg(&msgsym(FDDIRSET, CBUS), hwidb->hw_namestring, status,
            "bridge cmd");

    /* Send the TVX value */
    ltemp = (ushort) fsb->fddi_tvx;
    send_if_cmd_1arg(hwidb, CCB_CMD_VIP_1F_TVX, ltemp);
    status = get_if_status(hwidb);
    if (status != FCI_RSP_OKAY)
        errmsg(&msgsym(FDDIRSET, CBUS), hwidb->hw_namestring, status, 
            "tvx cmd");

    /* Send the TRT value */
    ltemp = (ushort) (fsb->fddi_trt / 1000);
    send_if_cmd_1arg(hwidb,CCB_CMD_VIP_1F_TRT, ltemp);
    status = get_if_status(hwidb);
    if (status != FCI_RSP_OKAY)
        errmsg(&msgsym(FDDIRSET, CBUS), hwidb->hw_namestring, status, 
            "trt cmd");


    /* Pass the broadcast value, 6 bytes at a time */
    sptr = (ushort *) baddr;
    send_if_cmd_3arg(hwidb, CCB_CMD_VIP_1F_INTERESTING_ADD, *sptr,
        *(sptr+1), *(sptr+2));
    status = get_if_status(hwidb);
    if (status != FCI_RSP_OKAY)
        errmsg(&msgsym(FDDIRSET, CBUS),hwidb->hw_namestring, status, 
            "stn_brd cmd");

    /*
     * Enable Reception for our hardware address and the broadcast address.
     */
    reg_invoke_tbridge_smf_update(hwidb->hardware,
        hwidb->firstsw,
        SMF_MACTYP_OWN_UNICAST |
        SMF_ACTION_RECEIVE);   

    /*
     * recalc srb params
     */
    srbV_p = reg_invoke_srb_get_vector_table();
    if (srbV_p) {
        srbV_p->srb_sb_init_rng_br_rng_mask(hwidb->firstsw);
        srb_rng_br_rng_f = srbV_p->srb_sb_get_rng_br_rng_f(hwidb->firstsw);
        srb_rng_br_rng_r = srbV_p->srb_sb_get_rng_br_rng_r(hwidb->firstsw);
    } else {
        srb_rng_br_rng_f = 0L;
        srb_rng_br_rng_r = 0L;
    }

    /*
     * We send the command down as follows.
     * If arg0 == 0, then ds->srbridge_on_hwidb is FALSE;
     * If arg0 == 1, then ds->srbridge_on_hwidb is TRUE;
     * If arg0 == 2, then args 1 and 2 form the  sb->rng_br_rng_f;
     * If arg0 == 3, then args 1 and 2 form the sb->rng_br_rng_r;
     */
    
    if (srbridge_on_hwidb) {
        send_if_cmd_3arg(hwidb, CCB_CMD_VIP_1F_SRB, 1, 0, 0);
    } else {
        send_if_cmd_3arg(hwidb, CCB_CMD_VIP_1F_SRB, 0, 0, 0);
    }
        
    /* Send sb->rng_br_rng_f */
    send_if_cmd_3arg(hwidb, CCB_CMD_VIP_1F_SRB, 2,   
                        (srb_rng_br_rng_f >> 16), srb_rng_br_rng_f);
    /* Send sb->rng_br_rng_r */
    send_if_cmd_3arg(hwidb, CCB_CMD_VIP_1F_SRB, 3,
                        (srb_rng_br_rng_r >> 16), srb_rng_br_rng_r);
    
    /* enable reception */
    reg_invoke_interface_cfg_modified(hwidb);
    status = send_if_enable_cmd(hwidb);
    if (status != FCI_RSP_OKAY) {
        errmsg(&msgsym(FDDIRSET, CBUS), hwidb->mci_index, status, "enable");
        platform_req_card_restart(hwidb);
        reset_interrupt_level(level);
        idb_release_hwsb_inline(hwidb, HWIDB_SB_FDDI);
        return;
    }
    reg_invoke_media_interesting_addresses(hwidb->firstsw);
    idb_release_hwsb_inline(hwidb, HWIDB_SB_FDDI);
    reset_interrupt_level(level);
}

/*
 * cbus_vip_1f_record_fddi_address
 * Firmware should check for multicast and program internal/external
 * cam according to routing/bridging requirements.
 * all addresses passed down are canonical
 */
        
void cbus_vip_1f_record_fddi_address (
    idbtype *swidb,
    uchar *address,
    uchar fsm)
{
    hwidbtype   *hwidb;
    ushort  status, *sptr;   
    leveltype   level;

    hwidb = swidb->hwptr;
    level = raise_interrupt_level(NETS_DISABLE);
    status = select_if(hwidb);
    if (status != FCI_RSP_OKAY) { 
        if (fddi_cmt_events_debug)
            buginf("\nError: (%-04x), select,
                interface %s - cbus_vip_1f_record_fddi_address()",
                status, hwidb->hw_namestring);
            reset_interrupt_level(level);  
            return;
    }

    /* Note, all addresses are in canonical form */
    /* Pass the canonical value, 6 bytes at a time */
    sptr = (ushort *) address;
    send_if_cmd_3arg(hwidb, CCB_CMD_VIP_1F_INTERESTING_ADD, *sptr,
        *(sptr+1), *(sptr+2));
    status = get_if_status(hwidb);
    if (status != FCI_RSP_OKAY)
        errmsg(&msgsym(FDDIRSET, CBUS),hwidb->hw_namestring, status, 
            "int_add cmd");
    reset_interrupt_level(level);
}


/*
 * cbus_vip_1f_record_fddi_address_range
 * time to go multicast promiscuous
 * all addresses passed down are canonical
 */
static void cbus_vip_1f_record_fddi_address_range (
    idbtype *swidb,
    uchar *address,
    uchar *mask)
{
    hwidbtype   *hwidb;
    ushort  status;   
    leveltype   level;

    hwidb = swidb->hwptr;
    level = raise_interrupt_level(NETS_DISABLE);
    status = select_if(hwidb);
    if (status != FCI_RSP_OKAY) { 
        if (fddi_cmt_events_debug)
            buginf("\nError: (%-04x), select,
                interface %s - cbus_vip_1f_record_fddi_address_range()",
                status, hwidb->hw_namestring);
        reset_interrupt_level(level);  
        return;
    }

    /* Send the promiscuous multicast command */
    send_if_cmd(hwidb, CCB_CMD_VIP_1F_ALL_MULTICAST_ADD);
    status = get_if_status(hwidb);
    if (status != FCI_RSP_OKAY)
        errmsg(&msgsym(FDDIRSET, CBUS),hwidb->hw_namestring, status,
            "all_multicast cmd");
    reset_interrupt_level(level);
}

/* cbus_vip_get_1f_ls:
 * convert Motorola number to a character string 
 */
static char *cbus_vip_get_1f_ls (uInt16 ls, uchar *linestate)
{
    linestate = "UNK";

    if (ls & 0x1)
        linestate = "NLS";  
    else if (ls & 0x2)
        linestate = "ALS";  
    else if (ls & 0x4)
        linestate = "UNK";  
    else if (ls & 0x8)
        linestate = "ILS";  
    else if (ls & 0x10)
        linestate = "QLS";  
    else if (ls & 0x20)
        linestate = "MLS";  
    else if (ls & 0x40)
        linestate = "HLS";  
    else if (ls & 0x80)
        linestate = "SILS"; 

    return(linestate);
}

/*
 * cbus_vip_1f_interface
 * A function to display some unique MIF68840 type information.
 */
static void cbus_vip_1f_interface (hwidbtype *hwidb)
{
    uchar una[6],dna[6];  /* bit swapped upstream & downstream neighbors */
    char *pcstate, *pcnbr;
    uInt16 ls;
    uchar linestate[4];
    uInt16 stnID = hwidb->fddi_stn_id;
    fddisb *fsb;
    idbtype *swidb = firstsw_or_null(hwidb);
    srb_triplet_t srb_triplet;
    srb_vector_table_t *srbV_p;
    leveltype level;

    fsb = idb_use_hwsb_inline(hwidb, HWIDB_SB_FDDI);

    /*
     * Read the status of the current FDDI error counters.
     */
    (void) get_cbus_errors(hwidb);

    level = raise_interrupt_level(NETS_DISABLE);
    send_if_cmd(hwidb, CCB_CMD_VIP_1F_GET_MIB_INFO);
    reset_interrupt_level(level);

    DELAY(10);

    srbV_p = reg_invoke_srb_get_vector_table();
    if (srbV_p) {
        srbV_p->srb_sb_get_triplet(swidb, &srb_triplet);
        if (srb_triplet.bridgeNum <= SRB_MAX_BRIDGE_NUM) {
	    printf("\n  Source bridging enabled, srn %d bn %d trn %d %s",
               srb_triplet.localRing,
               srb_triplet.bridgeNum,
               srb_triplet.remoteRing,
	       (hwidb->tr_vring_blk ? "(ring group)" : ""));
	    printf("\n    spanning explorer %sabled",
	       (srbV_p->srb_sb_get_spanning_explorer(swidb) ? "en" : "dis"));
	    automore_conditional(0);
        }
    }

    pcstate = get_pcm_state_string (MIBDATA (stnID, 
        mib->portBase[0].hwData.PCMState)); 
    pcnbr = get_phy_type_string (MIBDATA (stnID,
        mib->portBase[0].hwData.NeighborType));
    ls =  (MIBDATA (stnID, mib->portBase[0].hwData.PC_LS));
    printf("\n  Phy-A state is %s, neighbor is %s, status %4s",
        pcstate, pcnbr, cbus_vip_get_1f_ls(ls, linestate));

    pcstate = get_pcm_state_string (MIBDATA (stnID, 
        mib->portBase[1].hwData.PCMState)); 
    pcnbr = get_phy_type_string (MIBDATA (stnID,
        mib->portBase[1].hwData.NeighborType));
    ls =  (MIBDATA (stnID, mib->portBase[1].hwData.PC_LS));
    printf("\n  Phy-B state is %s, neighbor is %s, status %4s",
        pcstate, pcnbr, cbus_vip_get_1f_ls(ls, linestate));

    printf("\n  ECM is %s, CFM is %s, RMT is %s,",
        get_smt_string(&SMTMsg.ec_state,
            MIBDATA (stnID, mib->smtData.ECMState)),
        get_smt_string(&SMTMsg.cf_state,
            MIBDATA (stnID, mib->smtData.CF_State)),
        get_smt_string(&SMTMsg.rm_state,
            MIBDATA (stnID, mib->macBase[0].hwData.RMTState)));
    printf("\n  Requested token rotation %u usec, negotiated %u usec",
        fsb->fddi_trt, fsb->fddi_trt_neg);
    printf("\n  Configured tvx is %u usec", fsb->fddi_tvx);

    printf("\n  LER for PortA = %02x, LER for PortB = %02x",
        MIBDATA(stnID,mib->portBase[0].hwData.Ler_Estimate),
        MIBDATA(stnID,mib->portBase[1].hwData.Ler_Estimate));

    if (!TIMER_RUNNING(fsb->fddi_operational))
        printf(" ring not operational");
    else {
        printf(" ring operational ");
        print_dhms(fsb->fddi_operational);
    }
    ieee_swap(MIBDATA (stnID, mib->macBase[0].hwData.UpstreamNbr), una);
    ieee_swap(MIBDATA (stnID, mib->macBase[0].hwData.DownstreamNbr), dna);
    printf("\n  Upstream neighbor %e, downstream neighbor %e", una, dna);
}

static void unpack_messages(uchar *love_ptr, ulong length)
{
	SMTMessage      *smtmsg;
	uchar           *curptr;
	long            curlength;

	/* Move the pointer ahead of ll_type. The first SMT message
	 * starts there.
	 */
	curptr	= (uchar *)(love_ptr + sizeof(((vip_ll *)love_ptr)->ll_type));
	curlength	= length -  sizeof(((vip_ll *)love_ptr)->ll_type);
	for(; curlength > 0; curlength -= SIZE_SMTMessage)
	{
		smtmsg = (SMTMessage *)curptr;
		switch (smtmsg->destination) {
			case MIB_MSG_ID:
				ProcessMIBMessageMulti(smtmsg);
				break;
			case FBM_MSG_ID:
				ProcessFBMMessageMulti(smtmsg);
				break;
			case MAP_MSG_ID:
				ProcessMAPMessageMulti (smtmsg);
				break;
			case CSP_MSG_ID:
				/* We are not supposed to get here */
			default:
				if(fddi_debug)
					buginf ("DEFAULT DEST MSG. ID= %d\n", smtmsg->destination);
				break;
		}
		curptr += SIZE_SMTMessage;
		if(curptr > (love_ptr + length - SIZE_SMTMessage))
				break;
	}
}


/*****************************************************************************
 * Name: vip_1f_love_letter:
 * 
 * Description: love_letter processing from 1F Port Adapter
 *
 * Input: hwidb, lovenote 
 *
 * Return value: none
 *
 *****************************************************************************/
static void cbus_vip_1f_love_letter (hwidbtype *hwidb, lovenotetype *lovenote)
{
    ushort    asr = lovenote->love_code;
    ulong     length = lovenote->data_length;
    fddisb    *fsb  = idb_use_hwsb_inline(hwidb, HWIDB_SB_FDDI);

    if (length) {
        uchar *love_ptr = MEMD_OFFSET_TO_ADDR(lovenote->buffer_ptr);
        ulong ll_type = ((vip_ll *)love_ptr)->ll_type;  

        switch (ll_type) {
            case VIP_LL_STATS: {
                vip_ll_errors *card_errors = &((vip_ll *)love_ptr)->data;
                hwidb->counters.output_error += card_errors->output_error;
                hwidb->counters.output_underrun+=card_errors->output_underrun;
                hwidb->counters.input_error += card_errors->input_error;
                hwidb->counters.runts += card_errors->runts;
                hwidb->counters.giants += card_errors->giants;
                hwidb->counters.input_frame += card_errors->input_frame;
                hwidb->counters.input_crc += card_errors->input_crc;
                hwidb->counters.input_overrun += card_errors->input_overrun;
                hwidb->counters.input_resource += card_errors->input_resource;
                hwidb->counters.input_resource += card_errors->input_drops;
                fsb->fddi_claims  += card_errors->fddi_claims;
                fsb->fddi_beacons += card_errors->fddi_beacons;
                fsb->fddi_trace_count += card_errors->fddi_trace_count;
                break;
            }
            case VIP_LL_1F_LUVL:
                if (fddi_debug) {
                    buginf("\nvip_love_note(%s): asr=0x%x(1F_LUVL) length=%d",
                        hwidb->hw_namestring, asr, length);
                }
                unpack_messages(love_ptr, length);
                break;
            case VIP_LL_1F_MIBS: {
                uInt16    stnID = hwidb->fddi_stn_id;
                vip_1f_mib_info *ll = (vip_1f_mib_info *)love_ptr;

                if (fddi_debug) {
                    buginf("\nvip_love_note(%s):asr=0x%x(1F_MIBS) length=%d",
                        hwidb->hw_namestring, asr, length);
                }
                fsb->fddi_trt_neg = ll->fddi_trt_neg;
                STADATA(stnID, portCfg) = 
                    (ll->multimode ? &MMDASPORTCfg : &SSDASPORTCfg);
                MIBDATA(stnID,mib->smtData.ECMState) =  ll->ECMState;
                MIBDATA(stnID,mib->smtData.CF_State) =  ll->CF_State;
                MIBDATA(stnID,mib->macBase[0].hwData.RMTState) = ll->RMTState;
                MIBDATA(stnID,mib->portBase[0].hwData.NeighborType) = 
                    ll->NeighborType[0];
                MIBDATA(stnID,mib->portBase[1].hwData.NeighborType) = 
                    ll->NeighborType[1];
                MIBDATA(stnID,mib->portBase[0].hwData.PMDClass) = 
                    ll->PMDClass[0];
                MIBDATA(stnID,mib->portBase[1].hwData.PMDClass) = 
                    ll->PMDClass[1];
                MIBDATA(stnID,mib->portBase[0].hwData.ConnectState) = 
                    ll->ConnectState[0];
                MIBDATA(stnID,mib->portBase[1].hwData.ConnectState) = 
                    ll->ConnectState[1];
                MIBDATA(stnID,mib->portBase[0].hwData.PCMState) = 
                    ll->PCMState[0];
                MIBDATA(stnID,mib->portBase[1].hwData.PCMState) =
                    ll->PCMState[1];
                MIBDATA(stnID,mib->portBase[0].hwData.PC_LS) = ll->PC_LS[0];
                MIBDATA(stnID,mib->portBase[1].hwData.PC_LS) = ll->PC_LS[1];
                MIBDATA(stnID,mib->portBase[0].hwData.Ler_Estimate) = 
                    ll->Ler_Estimate[0];
                MIBDATA(stnID,mib->portBase[1].hwData.Ler_Estimate) = 
                    ll->Ler_Estimate[1];
                CSPDATA(stnID, macData[0].SM_MA_Avail) = ll->SM_MA_Avail;
                FBMDATA(stnID, fbmMACData[0].SM_MA_Avail) = ll->SM_MA_Avail;
                break;
            }
            case VIP_LL_EEPROM:
                vip_parse_eeprom_info(hwidb, love_ptr);
                break;
            case VIP_LL_HW:
                vip_parse_hw_info(hwidb, love_ptr);
                break;
            case VIP_LL_SW:
                vip_parse_sw_info(hwidb, love_ptr);
                break;
            default:
                break;
        }
        idb_release_hwsb_inline(hwidb, HWIDB_SB_FDDI);
        return;
    }

    if (asr & LOVE_CODE_CARRIER_CHANGE) {
        int newstate;
        ushort carrier_state;
    
        /* To ensure that we catch fast transitions, clear values  */
        TIMER_STOP(fsb->fddi_operational);

        /* Find out whether the interface went up or down. */
        select_if(hwidb);
        send_carrier_cmd(hwidb, &carrier_state);
        if (carrier_state == 0)
            newstate = IDBS_DOWN;
        else
            newstate = IDBS_UP;

        if ((hwidb->state != newstate) && (hwidb->state != IDBS_ADMINDOWN)){
            net_cstate(hwidb, newstate);
            hwidb->counters.transitions++;
        }

        if (newstate == IDBS_UP) {
            GET_TIMESTAMP(fsb->fddi_operational);
            hwidb->fddi_rops++;
            hwidb->mci_lineup = MCI_KEEP_COUNT;

        } else {

            /* The interface went down -- clear some hwidb variables. */
            hwidb->mci_lineup = 0;
        }
        idb_release_hwsb_inline(hwidb, HWIDB_SB_FDDI);
        send_if_cmd(hwidb, CCB_CMD_VIP_1F_GET_MIB_INFO);
    }
}

/*****************************************************************************
 * Name: cbus_vip_1f_periodic:
 * 
 * Description: Periodic function for 1F Port Adapter
 *
 * Input: hwidb 
 *
 * Return value: none
 *
 ***************************************************************************/
void cbus_vip_1f_periodic (hwidbtype *hwidb)
{
}

/*****************************************************************************
 * Name: rsp_fddi_idb_init
 *
 * Description: platform specific fddi idb init
 *
 * Input: hwidb
 *
 * Return Value: None
 *
 *****************************************************************************/
void rsp_fddi_idb_init (hwidbtype *hwidb, boolean vip_style)
{
    /*
     * In RSP init the config is parsed after this analyze but
     *   before memd is carved.
     * fip needs a memd claim beacon buffer.
     * the buffer is allocated after memd carve -
     *   then initialized in fddi_reset.
     * some parse commands reset the interface via these vectors.
     * Set them to nothing in case any parse commands try to reset us
     * before MEMD is carved and a claim buffer allocated.
     */
    hwidb->reset = (reset_t) return_nothing;
    hwidb->periodic = NULL;

    nfddinets++;

    if (hwidb->status & IDB_NOHARDWARE) {
        hwidb->status &= ~IDB_NOHARDWARE;
        return;
    }

    hes_idb_init_before(hwidb);

    /*
     * RSP-specific FDDI (FIP) IDB initialization.
     */
    hwidb->status = IDB_FDDI | (FCI_IDBFLAGS & ~IDB_HEARSELF);

    hwidb->firstsw->tbridge_media = TBR_FDDI;

    /*
     * Note, that the units (bytes Vs words) used in the rxoffset
     * field is platform dependant.  On the RSP, the value represents
     * bytes (on other platforms, like the 7000, this value 
     * represents words).  The IP ucode on each platform has been
     * modified to expect the correct units for that platform.
     */
    hwidb->rxoffset = RSP_FDDI_BYTE_RECEIVE_OFFSET;
    hwidb->ip_memd_offset = hwidb->rxoffset + ADRSPC_MEMD_WRITETHRU +
        FDDI_SNAP_ENCAPBYTES;
    hwidb->encsize = FDDI_SNAP_ENCAPBYTES;

    /*
     * Set up the span encapsulation size correctly for
     * FDDI encapsulated bridging
     */
    if (hwidb->bridge_transit)
	hwidb->span_encapsize = FDDI_SNAP_ENCAPBYTES;
    else
	hwidb->span_encapsize = FDDI_FC;
    hwidb->span_macoffset = hwidb->rxoffset + hwidb->span_encapsize;

    hwidb->rsrb_vringoffset = hwidb->rxoffset + FDDI_SNAP_ENCAPBYTES;

    hwidb->love_note = (void *)fip_love_note;

    /*
     * Do NOT set up system_configured for the RSP!
     * hwidb->reset is called for RSP FIP interfaces from idb_reset() after
     * the configuration is parsed, via reg_invoke_system_configured().
     * If we attempt to start CMT again via hwidb->system_configured,
     * we'll blow away the FIP ucode which is still trying to connect
     * us up, causing the FDDI ring to beacon!
     */
    hwidb->system_configured = NULL;

    /*
     * IDB initialization for all High end FDDI interfaces (AGS+, 7000,
     * RSP, etc):
     */
    hes_fddi_idb_init(hwidb);

    /*
     * If we are configured for an SMT process and if one is not already
     * running, start one up:
     */
    if (hwidb->fddi_options & FDDI_SMT_FRAMES) {
        if (!smt_process_id) {
            /*
             * Start the FDDI SMT input queue process:
             */
            queue_init(&smtinputQ, nfddinets);
            start_smt_input_proc();
        }
    }

    /* MIF68840 specific initialization */
    if (vip_style) {
        hwidb->status |= IDB_VIP;
        hwidb->love_note = (void *)cbus_vip_1f_love_letter;
        hwidb->system_configured = NULL;

        /*
         * Overwrite some function pointers initialized in hes_fddi_idb_init.
         */
        hwidb->show_controller = NULL;
        hwidb->fddi_options |= FDDI_BRIDGE_CAM_SUPPORTED;
        hwidb->listen = cbus_vip_1f_record_fddi_address;
        hwidb->listen_range = cbus_vip_1f_record_fddi_address_range;
        hwidb->show_support = cbus_vip_1f_interface;

        reg_add_vip_setup_srb(hwidb->type, cbus_vip_1f_reset, 
				"cbus_vip_1f_reset");

    }

    /*
     * Complete set up of common fields.
     */
    init_new_hwidb(hwidb);
    hes_idb_init_after(hwidb);
}

/*****************************************************************************
 * Name: fip_ctrlr_analyze
 *
 * Description: analyze part of initialization for an FIP
 *   This function is called when an board is first discovered and initialized.
 *   It is called the first time the board is plugged in, or when the router
 *   is resstarted.  It is *NOT* called for boards that are installed and
 *   running when a new board is inserted.
 *
 *   This function is responsible for creating and initializing the CDB
 *   and the IDB for this interface.
 *
 * Notes: 
 *   This function is *NEVER* called from interrupt level.
 *
 * Input: 
 *   slot -- pointer to a slottype structure for the IP with the
 *     following fields filled in:
 *       slot_num -- slot number for this card
 *       ccb_ptr -- pointer to the CCB for the FIP
 *       cmd_q_ptr -- pointer to the CCB for the FIP
 *
 * Output:
 *   The following fields in the slottype (pointed to by slot) are changed
 *     interface_count -- number of interfaces on this IP
 *     interface_list -- list of pointers to IDB's for the interfaces 
 *
 * Return Value: None
 *
 *****************************************************************************/
static void fip_ctrlr_analyze (slottype *slot)
{
    hwidbtype	*hwidb;
    int		port_num;
    ushort	status;
    ushort	interface_type;

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
    if (interface_type != FCI_TYPE_FDDIT) {
	errmsg(&msgsym(PORTTYPE,CBUS), slot->card_name, slot->slot_num,
	       port_num, interface_type);
	return;
    }

    ++nFIPs;

    /*
     * Get the IDB for the FIP interface
     * If we find a matching one, we can just use it,
     * otherwise we init a new one.
     */
    hwidb = rsp_get_matching_idb(slot, port_num,
				 interface_type, IDBTYPE_FDDIT);
    if (hwidb == NULL) {
	return;		/* rsp_get_matching_idb() complains */
    }			/* about malloc() failure */

    /*
     * Set up common fields:
     */
    slot->interface_list[port_num] = hwidb;
    slot->interface_count = 1 + port_num;

	rsp_fddi_idb_init(hwidb, FALSE);

    nFIPnets++;

	return;
}


/*****************************************************************************
 * Name: fip_ctrlr_init
 *
 * Description: per-card initizliation for the FIP
 *   This function is called during the initialization phase (after analyze)
 *   and after the configuration has been parsed.  The fields in the slottype
 *   have been filled in -- the information needs to be sent to the FIP now. 
 *   It may also be necessary to pass global information to the FIP.
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
 *       ccb_ptr -- pointer to the CCB for the FIP
 *       cmd_q_ptr -- pointer to the CCB for the FIP
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
static void fip_ctrlr_init (slottype *slot)
{
    rsp_common_ctrlr_init(slot);
}

/*****************************************************************************
 * Name: fip_if_init
 *
 * Description: per-card initialization for the FIP
 *   This function is called during the initialization phase (after analyze)
 *   and after the configuration has been parsed.  The fields in the slottype
 *   have been filled in -- the information needs to be sent to the FIP now. 
 *   It may also be necessary to pass global information to the FIP.
 *   The algorithm is 
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

static void fip_if_init (hwidbtype *hwidb)
{
    leveltype	level;

    level = raise_interrupt_level(NETS_DISABLE);

    select_if(hwidb);

    rsp_common_if_init(hwidb);

    /*
     * Set up the hold register bits for the FIP:
     */
    hes_fddi_set_hold_reg(hwidb, get_fip_byte_rxoffset(hwidb));

    /*
     * Enable interface reset and periodic functions:
     */
    hwidb->reset = fddi_reset;
    hwidb->periodic = fddi_periodic;
    reg_add_setup_srb(hwidb->type, fddi_reset, "fddi_reset");

    reset_interrupt_level(level);
}

/*****************************************************************************
 * Name: fip_restart_init
 *
 * Description: Post FIP restart reinitialization
 *
 * Input:
 *   hwidb -- pointer to the hardware IDB for the interface.
 *
 * Return Value: None
 *
 *****************************************************************************/
void fip_restart_init (hwidbtype *hwidb)
{
    hwidb->fddi_options |= FDDI_BRIDGE_CAM_SUPPORTED;

    /*
     * Set up the span encapsulation size correctly for
     * FDDI encapsulated bridging
     */
    if (hwidb->bridge_transit)
	hwidb->span_encapsize = FDDI_SNAP_ENCAPBYTES;
    else
	hwidb->span_encapsize = FDDI_FC;
    hwidb->span_macoffset = hwidb->rxoffset + hwidb->span_encapsize;
}

/*
 * invoked when a FIP has been removed from the system
 */
static void fip_ctrlr_removed (slottype *sp)
{
    hwidbtype *hwidb;

    hwidb = sp->interface_list[0];
    if (hwidb) {
	--nnets;
	--nfddinets;
	--nFIPnets;
	if (hwidb->claimbuf) {
	    if (data_enqueue(&fipclaimQ, hwidb->claimbuf))
		hwidb->claimbuf = NULL;
	}
	adjust_smt_bufs();		/* Free up SMT buffers */
    }
    --nFIPs;
}

/*
 * display count of FIPs in the system
 */
static void fip_print_controllers (void)
{
    if (nFIPs)
	printf("%u FIP controller%s (%u FDDI).\n",
	       ArgAndPlural(nFIPs,"","s"), nFIPnets);
}

/*****************************************************************************
 * Name: fip_platform_init
 *
 * Description: platform specific FIP init
 *
 * Input: 
 *   subsys -- pointer to the subsystype for this subsystem
 *
 * Return Value: None
 *
 *****************************************************************************/
void fip_platform_init (subsystype *subsys)
{
    int	indx, jndx;

    nFIPs = 0;
    nFIPnets = 0;
    queue_init(&fipclaimQ, 0);

    reg_add_ctrlr_analyze(FCI_FDDIT_CONTROLLER,
			  fip_ctrlr_analyze, "fip_ctrlr_analyze");
    reg_add_ctrlr_init(FCI_FDDIT_CONTROLLER, fip_ctrlr_init, "fip_ctrlr_init");
    reg_add_if_init(FCI_FDDIT_CONTROLLER, fip_if_init, "fip_if_init");
    reg_add_ctrlr_removed(FCI_FDDIT_CONTROLLER,
			  fip_ctrlr_removed, "fip_ctrlr_removed");
    reg_add_print_controllers(fip_print_controllers,
			      "fip_print_controllers");
    reg_add_fddi_ringstate_leds(fip_leds, "fip_leds");

    /*
     * Reset FDDI SMT data structure
     */
    for (indx = 0; indx < MAX_STATION_COUNT; indx++) {
	for (jndx = 0; jndx < MAX_MAC_COUNT; jndx++)
	    STADATA(indx, hwIDB[jndx]) = 0;
	for (jndx = 0; jndx < MAX_PORT_COUNT; jndx++)
	    STADATA(indx, portRegs[jndx]) = 0;
	STADATA(indx, hwPresent) = FALSE;
	STADATA(indx, prevCFState) = CF_isolated;
    }
}

/*
 * cbus_daughter_cmd
 * send a command thru the cbus controller to the daughter.
 * We can probably remove this routine altogether once fddi_reset()
 * has been sanitised (dB).
 *
 * What David is referring to above is replacing all calls to this routine
 * with calls to wrapper functions for each of the commands being executed.
 */

void cbus_daughter_cmd (hwidbtype *idb, uint cmd, uint arg)
{
    leveltype level;

    level = raise_interrupt_level(NETS_DISABLE);

    select_if(idb);

    send_if_cmd_1arg(idb, cmd, arg);

    reset_interrupt_level(level);
}

/*
 * build_claim_beacon_buffer
 * Build the claim, beacon, and directed beacon frames for the FIP.
 * on the RSP, we build them in MEMD that hwidb->claimbuf points at.
 *
 * Then, send the claim_beacon buffer to the FIP card.
 *
 * RSP SPECIFIC CODE. Needs to stay in 'rsp_fip.c'.
 *
 * trt is 2's complement of Token Rotation time
 */

ushort build_claim_beacon_buffer (hwidbtype *hwidb, int trt)
{
    uchar	*ptr;
    ushort	*sptr, *dest_ptr;
    uchar	*dbptr;
    ushort	*claimbuf_words;
    ushort	status;

    claimbuf_words = (ushort *) hwidb->claimbuf;
    
    /*
     * Zero out the entire buffer.
     */
    memset(hwidb->claimbuf, 0, FDDI_CLAIM_BEACON_BUFFER_SIZE);

    /*
     * Write the lengths of the claim and beacon portions into the buffer.
     */
    claimbuf_words[FDDI_CLAIM_LENGTH_OFFSET] = FDDI_CLAIM_LENGTH;
    claimbuf_words[FDDI_BEACON_LENGTH_OFFSET] = FDDI_BEACON_LENGTH;

    /*
     * Write the frame codes for the claim and our station address into the
     * the destination and source address of the claim frame. Lastly add
     * the 2's complement of the requested token rotation time (TRT).
     */
     if (hwidb->type == IDBTYPE_FDDIT) {
        ptr = hwidb->hardware;
        dbptr = dbptr_fcit;
     } else {
        ptr = hwidb->bit_swapped_hardware;
        dbptr = dbptr_fddi;
     }

    /*
     * This code is pretty much a direct translation of the AGS+/7000
     * code which writes the claim/beacon/directed_beacon frames:
     */
    dest_ptr = &claimbuf_words[FDDI_CLAIM_FRAME_CODE_OFFSET];
    *dest_ptr++ = (FDDI_CLAIM_FRAME_CODE << 8) | ptr[0];
    sptr = (ushort *) &ptr[1];
    *dest_ptr++ = GETSHORT(sptr++);
    *dest_ptr++ = GETSHORT(sptr--);
    *dest_ptr++ = (ptr[5] << 8) | ptr[0];
    *dest_ptr++ = GETSHORT(sptr++);
    *dest_ptr++ = GETSHORT(sptr--);
    *dest_ptr++ = (ptr[5] << 8) | ((trt >> 24) & 0xFF);
    *dest_ptr++ = trt >> 8;
    *dest_ptr++ = ((trt << 8) & 0xFF00);

    /*
     * Write the beacon frame code and our station address as the source
     * address -- the destination address is left zero.
     */
    dest_ptr = &claimbuf_words[FDDI_BEACON_FRAME_CODE_OFFSET];
    *dest_ptr++ = (FDDI_BEACON_FRAME_CODE << 8);

    dest_ptr = &claimbuf_words[FDDI_BEACON_SA_OFFSET];
    *dest_ptr++ = (ushort) ptr[0];
    *dest_ptr++ = GETSHORT(sptr++);
    *dest_ptr++ = GETSHORT(sptr);
    *dest_ptr++ = (ushort) (ptr[5] << 8);


    dest_ptr = &claimbuf_words[FDDI_DIRECT_BEACON_CODE_OFFSET];
    *dest_ptr++ = (FDDI_BEACON_FRAME_CODE << 8)| dbptr[0];
    sptr = (ushort *)&dbptr[1];
    *dest_ptr++ = GETSHORT(sptr++);
    *dest_ptr++ = GETSHORT(sptr);
    *dest_ptr++ = (dbptr[5] << 8) | ptr[0];
    sptr = (ushort *)&ptr[1];
    *dest_ptr++ = GETSHORT(sptr++);
    *dest_ptr++ = GETSHORT(sptr);
    *dest_ptr++ = (ptr[5] << 8) | 0x0001;

    /*
     * Flush the claim/beacon/directed_beacon buffer out to MEMD:
     */
    writeback_inval_data(hwidb->claimbuf, FDDI_CLAIM_BEACON_BUFFER_SIZE);

    /*
     * Send the claim/beacon/directed_beacon buffer to the FIP as the
     * large argument to the CCB_CMD_FIP_CLAIM_BEACON command.
     *
     * The argument is a 24 bit MEMD offset to the start of the beacon,
     * claim, directed beacon buffer. The high byte of the argument
     * MUST be zero.
     */
    send_if_cmd_larg(hwidb, CCB_CMD_FIP_CLAIM_BEACON,
		     ADDR_TO_MEMD_OFFSET_24((uint)hwidb->claimbuf));
    status = get_if_status(hwidb);
    return(status);
}

/*
 * fddi_print_versions
 * Print out the hardware and ucode versions for the FDDI card:
 */

void fddi_print_versions (hwidbtype *hwidb)
{
    printf("hardware version %d.%d, microcode version %d.%d",
	   (hwidb->slot_ptr->hw_version >> 8),
	   (hwidb->slot_ptr->hw_version & 0xFF),
	   (hwidb->slot_ptr->sw_version >> 8),
	   (hwidb->slot_ptr->sw_version & 0xFF));
}

/*
 * fddit_print_regs
 * Print out the registers on the FDDIT/FIP card:
 */

void fddit_print_regs (ushort regs[])
{
    /*
     * ccb:   xxxx  cmd:   xxxx  fr:   xxxx  mdptr: xxxx  mema: xxxx
     * icb:   xxxx  arg:   xxxx  app:  xxxx  mdpg:  xxxx  af:   xxxx
     * clm:   xxxx  bcn:   xxxx  clbn: xxxx  rxoff: xxxx  en:   xxxx
     * clmbc: xxxx  bcnbc: xxxx  robn: xxxx  park:  xxxx  fop:  xxxx
     *
     * txchn: xxxx  pend:  xxxx  act:  xxxx  tail:  xxxx  cnt:  xxxx
     * state: xxxx  check: xxxx  eof:  xxxx  tail:  xxxx  cnt:  xxxx
     * rxchn: xxxx  buf0:  xxxx  nxt0: xxxx  eof:   xxxx  tail: xxxx
     * eofch: xxxx  buf1:  xxxx  nxt1: xxxx  pool:  xxxx  err:  xxxx
     *
     * head:  xxxx  cur:   xxxx  t0:   xxxx  t1:    xxxx  t2:   xxxx
     * tail:  xxxx  cnt:   xxxx  t3:   xxxx  rxlft: xxxx  used: xxxx
     * txq_s: xxxx  txq_f: xxxx  Aarm: xxxx  Barm:  xxxx  fint: xxxx
     */

    printf("\nFIP registers");
    printf("\n ccbptr:        %04x  event_ptr:    %04x  cmdreg:        %04x  argreg:   %04x",
           regs[0x0], regs[0x1], regs[0x2], regs[0x3]);
    printf("\n memdptr:       %04x  memdpage:     %04x  memaptr:       %04x  afaddr:   %04x",
           regs[0x4], regs[0x5], regs[0x6], regs[0x7]);
    printf("\n frptr:         %04x  apptr:        %04x  tx_channel:    %04x  tx_claim: %04x",
           regs[0x8], regs[0x9], regs[0xa], regs[0xb]);
    printf("\n tx_claim_bc:   %04x  tx_beacon:    %04x  tx_beacon_bc:  %04x  tx_clbn:  %04x",
           regs[0xc], regs[0xd], regs[0xe], regs[0xf]);


    printf("\n tx_pend:       %04x  local_freeptr:%04x  hold_ctl:      %04x  unused:   %04x",
           regs[0x10], regs[0x11], regs[0x12], regs[0x13]);
    printf("\n tx_active_cnt: %04x  txq_ptr:      %04x  tx_accptr:     %04x  raw_ptr:  %04x",
           regs[0x14], regs[0x15], regs[0x16], regs[0x17]);
    printf("\n tx_state:      %04x  rx_channel:   %04x  rx_eof_channel:%04x  rx_error: %04x",
           regs[0x18], regs[0x19], regs[0x1a], regs[0x1b]);
    printf("\n rx_pool:       %04x  rx_buf0:      %04x  rx_buf1:       %04x  rx_next0: %04x",
           regs[0x1c], regs[0x1d], regs[0x1e], regs[0x1f]);


    printf("\n rx_next1:      %04x  rx_limit_lo:  %04x  rx_limit_hi:   %04x  rx_offset:%04x",
           regs[0x20], regs[0x21], regs[0x22], regs[0x23]);
    printf("\n enabled:       %04x  return:       %04x  phya_ls_flag:  %04x  unused:   %04x",
           regs[0x24], regs[0x25], regs[0x26], regs[0x27]);
    printf("\n phya_tmin:     %04x  phya_tmax:    %04x  unused:        %04x  txq_fill: %04x",
           regs[0x28], regs[0x29], regs[0x2a], regs[0x2b]);
    printf("\n lovenote:      %04x  not_rcv_pkt:  %04x  phyb_tmin:     %04x  phyb_tmax:%04x",
           regs[0x2c], regs[0x2d], regs[0x2e], regs[0x2f]);


    printf("\n t0:            %04x  t1:           %04x  t2:            %04x  t3:       %04x",
           regs[0x30], regs[0x31], regs[0x32], regs[0x33]);
    printf("\n txq_fill_txEof:%04x  unused:       %04x  cur:           %04x  cnt:      %04x",
           regs[0x34], regs[0x35], regs[0x36], regs[0x37]);
    printf("\n fop:           %04x  phyb_ls_flag: %04x  lst_fint:      %04x  rx_count: %04x",
           regs[0x38], regs[0x39], regs[0x3a], regs[0x3b]);
    printf("\n unused:        %04x  bogus_claim:  %04x  robin:         %04x  park:     %04x",
           regs[0x3c], regs[0x3d], regs[0x3e], regs[0x3f]);
}

/*
 * fddi_print_ip_regs
 * Print out the registers on the FDDI IP
 */

void fddi_print_ip_regs (hwidbtype *hwidb, ushort regs[])
{
    fddit_print_regs(regs);
}

/*
 * fddi_set_options
 * Set the fddi options correctly for this platform and FDDI IP
 */

void fddi_set_options (hwidbtype *hwidb)
{
    /* Only the FIP is supported on the RSP platform */
    hwidb->fddi_options |= FDDI_LEM_SUPPORTED;
    hwidb->fddi_options |= FDDI_FAST_CMT_ENB;
    hwidb->fddi_options |= FDDI_FAST_CMT;
    hwidb->fddi_options |= FDDI_CMT_ASSIST_SUPPORTED;
    hwidb->fddi_options |= FDDI_PHY_GET_LS_SUPPORTED;
}

/*
 * fddi_smt_enabled_any_intfc
 * Platform specific.
 * Return true if SMT is enabled on any FDDI interface in the box
 */

boolean fddi_smt_enabled_any_intfc (hwidbtype *hwidb)
{
    hwidbtype	*nhwidb;
    slottype	*slotp;

    /*
     * Check each IP and return true if any FDDI interface
     * has SMT enabled. Otherwise, return false:
     */
    FOR_ALL_ENABLED_IP_SLOTS(slotp) {
	if (slotp->ctrlr_type == FCI_FDDIT_CONTROLLER) {
	    nhwidb = slotp->interface_list[0];
	    if (nhwidb->fddi_options & FDDI_SMT_FRAMES)
		return(TRUE);
	}
    }
    return(FALSE);
}

void fddi_check_for_old_cam_bug (hwidbtype *hwidb)
{
    /* Nothing to do for RSP. Bug never existed on any FIP */
}

int get_fip_byte_rxoffset (hwidbtype *hwidb)
{
      /*
       * hwidb->rxoffset is in bytes on rsp
       */
      return(hwidb->rxoffset);
}

/*
 *  rsp_vip_1f_cmt_connect:
 *  cmt connect f 1/0/0
 */
static void rsp_vip_1f_cmt_connect (hwidbtype *idb, uint which)
{
    leveltype level;   
    
    level = raise_interrupt_level(NETS_DISABLE);
     
    select_if(idb);
    if (which & FDDI_PHY_A) {
        send_if_cmd_1arg(idb, CCB_CMD_FIP_KICK_PHY, FCI_FDDI_START_PHY | 0);
    }
    if (which & FDDI_PHY_B) {
        send_if_cmd_1arg(idb, CCB_CMD_FIP_KICK_PHY, FCI_FDDI_START_PHY |
            FCI_FDDI_PHY_B);
    }
    reset_interrupt_level(level);
}
  
/*
 *  rsp_vip_1f_cmt_disconnect:
 *  cmt disconnect f 1/0/0
 */  
static void rsp_vip_1f_cmt_disconnect (hwidbtype *idb, uint which)
{
    leveltype level;
       
    level = raise_interrupt_level(NETS_DISABLE);
    select_if(idb);
    if (which & FDDI_PHY_A) {
        send_if_cmd_1arg(idb, CCB_CMD_FIP_KICK_PHY, FCI_FDDI_STOP_PHY | 0);
    }
    if (which & FDDI_PHY_B) {
        send_if_cmd_1arg(idb, CCB_CMD_FIP_KICK_PHY, FCI_FDDI_STOP_PHY |
            FCI_FDDI_PHY_B);
    }
    reset_interrupt_level(level);
}

/*
 * Send Connect action message to PCM.
 */
void cmt_connect (hwidbtype *idb, int which)
{
    leveltype SR;
 
    if (idb->status & IDB_VIP) {
        rsp_vip_1f_cmt_connect(idb, which);
        return;
    } else {
        if (which & FDDI_PHY_A) {
            ((phytype *)(idb->phy[0]))->connect = TRUE;
            SendSignalMulti(idb->fddi_stn_id, SIG_PC_Enable, 0, (uInt32) 0);
            SendSignalMulti(idb->fddi_stn_id, SIG_PC_Start, 0, (uInt32) 0);
        }
        if (which & FDDI_PHY_B) {
            ((phytype *)(idb->phy[1]))->connect = TRUE;
            SendSignalMulti(idb->fddi_stn_id, SIG_PC_Enable, 1, (uInt32) 0);
            SendSignalMulti(idb->fddi_stn_id, SIG_PC_Start, 1, (uInt32) 0);
        }
    }
 
    /* Process signals */
    if (!EmptySignalQueue ()) {
    MDisableCSPInterrupts(&SR);
    ProcessCSP (&SR);
    MRestoreCSPInterrupts(&SR);
    }
}
 
/*
 * Send Disconnect action message to ECM.
 */
void cmt_disconnect (hwidbtype *idb, int which)
{
    leveltype   SR;
     
    if (idb->status & IDB_VIP) {
        rsp_vip_1f_cmt_disconnect(idb, which);
        return;
    } else {
        if (which & FDDI_PHY_A) {
            SendSignalMulti(idb->fddi_stn_id, SIG_PC_Disable, 0, (uInt32)0);
            ((phytype *)(idb->phy[0]))->connect = FALSE;
        }
        if (which & FDDI_PHY_B) {
            SendSignalMulti(idb->fddi_stn_id, SIG_PC_Disable, 1, (uInt32)0);
            ((phytype *)(idb->phy[1]))->connect = FALSE;
        }
    }
 
    /* Process signals */
    if (!EmptySignalQueue()) {
    MDisableCSPInterrupts(&SR);
    ProcessCSP (&SR);
    MRestoreCSPInterrupts(&SR);  
    }
}

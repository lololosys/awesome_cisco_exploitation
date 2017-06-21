/* $Id: ags_c7000_fddi.c,v 3.3.56.9 1996/08/28 12:43:29 thille Exp $
 * $Source: /release/112/cvs/Xsys/hes/ags_c7000_fddi.c,v $
 *------------------------------------------------------------------
 * ags_rp1_fip.c -- The FIP specific code that was in fip.c
 *             but only the parts that are specific to the
 *             AGS+/RSP1.  The FIP specific code that is generic
 *             to all platforms may be found in if_fip.c
 *
 * September, 1989,  Robert Fletcher
 *
 * Copyright (c) 1988-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *
 * no placeholder_yet
 *------------------------------------------------------------------
 * $Log: ags_c7000_fddi.c,v $
 * Revision 3.3.56.9  1996/08/28  12:43:29  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.3.56.8  1996/08/26  15:06:15  ppearce
 * CSCdi66582:  Implement SRB vector table to cut size of SRB_CORE subsys
 * Branch: California_branch
 *
 * Revision 3.3.56.7  1996/08/16  00:49:28  ogrady
 * CSCdi54594:  SRB on FDDI propogates broadcast storms
 *              Branch: California_branch
 *              Very small bridge packets not stripped from ring.  Make
 *              routine that inits FIP hold register common instead of
 *              platform specific.
 *
 * Revision 3.3.56.6  1996/07/09  05:50:35  ppearce
 * CSCdi59527:  inlines wastes code space
 * Branch: California_branch
 *   Just say "no" to inline abuse - SRB subblock inlines removed
 *
 * Revision 3.3.56.5  1996/06/18  20:05:28  fsunaval
 * CSCdi59153:  Provide support for cmt con and cmt dis.
 * Branch: California_branch
 * 1."cmt con" and "cmt dis" put into platform specific files.
 *
 * Revision 3.3.56.4  1996/06/01  01:31:09  hampton
 * Remove unnecessary cross module references.  [CSCdi59221]
 * Branch: California_branch
 *
 * Revision 3.3.56.3  1996/05/17  11:16:48  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.3.44.2  1996/04/26  15:18:12  ppearce
 * IBU modularity - SRB subblock (part 1)
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.3.44.1  1996/04/03  14:26:44  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.3.56.2  1996/03/21  22:39:37  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.3.56.1  1996/03/18  19:39:17  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.2.1  1995/12/23  04:39:48  fsunaval
 * Branch: ELC_branch
 * 1.First pass at FDDI port adapter.
 *
 * Revision 3.3  1995/11/17  09:14:18  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:25:37  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:37:07  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/21  00:35:54  ioakley
 * CSCdi36717:  Ensure that FDDI high end logic is able to detect an
 *              srb/fddi bridge and thereby initialize the control
 *              register for frame stripping.  Prevents explorer storms
 *              caused by unstripped frames running around the FDDI
 *              ring at wire speeds when a wrap of the FDDI ring occurs.
 *
 * Revision 2.1  1995/06/07  20:37:18  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "logger.h"
#include "msg_cbus.c"			/* Not a typo, see logger.h */
#include "sys_registry.h"
#include "cbus_registry.h"
#include "interface_private.h"
#include "../hes/if_hes_common.h"
#include "if_fci.h"
#include "if_mci.h"
#include "../hes/if_fip.h"
#include "../if/fddi.h"
#include "../if/fddi_private.h"
#include "../if/parser_defs_fddi.h"
#include "dbus.h"
#include "../if/if_controller.h"
#include "if_cmd.h"
#include "if_fip.h"

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

/*
 * fddi_print_versions
 * Print out the hardware and ucode versions for the FDDI card:
 */

void fddi_print_versions (hwidbtype *hwidb)
{
    mcitype	*cbus;
    dcitype	*dci;

    cbus = hwidb->devctl;
    dci = cbus->dci[hwidb->cbus_ctrlr_index];
    printf("hardware version %d.%d, microcode version %d.%d",
	   (dci->hw_version >> 8), (dci->hw_version & 0xFF),
	   (dci->sw_version >> 8), (dci->sw_version & 0xFF));
}


/*
 * fci_print_regs
 * Print out the registers on the old FCI FDDI card:
 */
static void fci_print_regs (ushort *regs)
{
    printf("\n  FDDI registers:\n    tx_current %-04x, tx_pending %-04x, tx_restore %-04x, tx_clbn %-04x",
	   regs[5], regs[6], regs[7], regs[16]);
    printf("\n    tx_pen_ofst %-04x, tx_pen_cnt %d, tx_pen_type %-04x, tx_start_cnt %-04x",
	   regs[24], regs[25], regs[26], regs[20]);
    printf("\n    rx_current %-04x, rx_pending %-04x, rx_recvd %-04x, rx_free %-04x",
	   regs[10], regs[11], regs[12], regs[13]);
    printf("\n    rx_flags %-04x, rx_error %-04x, rx_limit %d, rx_word_offset %d",
	   regs[30], regs[17], regs[27], regs[28]);
    printf("\n    hold_reg %-04x, clm_bn_flg %-04x, enable %-04x, fsvld %-04x",
	   regs[32], regs[44], regs[31], regs[47]);
    printf("\n    lstinc %u, errinc %u, mss_frm %u, frinc %u, phy-a %-04x, lem_a %u",
	   regs[49], regs[50], regs[42], regs[51], regs[29], regs[31]);
    printf("\n    fr_int_cnt %u, xtmabti %u, frmcpd %u, eof %u, phy-b %-04x, lem_b %u",
	   regs[52], regs[54], regs[59], regs[53], regs[45], regs[60]); 
    printf("\n    claim %u, beacon %u, async_reg %u, async %u, stat_mask %-04x",
	   regs[55], regs[56], regs[62], regs[57], regs[41]);
    printf("\n    sync_req %u, sync %u, tx_state %-04x, int_stat %-04x",
	   regs[61], regs[58], regs[43], regs[63]);
}

/*
 * fddit_print_regs
 * Print out the registers on the FDDIT/FIP card:
 */

void fddit_print_regs (ushort *regs)
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

#ifdef C7000
    printf("\n  FIP registers");
#else
    printf("\n  FDDI-T registers");
#endif
    printf("\n    ccb:   %04x  cmd:   %04x  fr:   %04x  mdptr: %04x  mema: %04x",
	   regs[0], regs[2], regs[8], regs[4], regs[6]);
    printf("\n    icb:   %04x  arg:   %04x  app:  %04x  mdpg:  %04x  af:   %04x",
	   regs[1], regs[3], regs[9], regs[5], regs[7]);
    printf("\n    clm:   %04x  bcn:   %04x  clbn: %04x  rxoff: %04x  en:   %04x",
	   regs[0xb], regs[0xd], regs[0xf], regs[0x23], regs[0x24]);
    printf("\n    clmbc: %04x  bcnbc: %04x  robn: %04x  park:  %04x  fop:  %04x\n",
	   regs[0xc], regs[0xe], regs[0x3e], regs[0x3f], regs[0x38]);
    printf("\n    txchn: %04x  pend:  %04x  act:  %04x  tail:  %04x  cnt:  %04x",
	   regs[0xa], regs[0x10], regs[0x12], regs[0x13], regs[0x14]);
    printf("\n    state: %04x  check: %04x  eof:  %04x  tail:  %04x  cnt:  %04x",
	   regs[0x18], regs[0x11], regs[0x15], regs[0x16], regs[0x17]);
    printf("\n    rxchn: %04x  buf0:  %04x  nxt0: %04x  eof:   %04x  tail: %04x",
	   regs[0x19], regs[0x1d], regs[0x1f], regs[0x21], regs[0x22]);
    printf("\n    eofch: %04x  buf1:  %04x  nxt1: %04x  pool:  %04x  err:  %04x\n",
	   regs[0x1a], regs[0x1e], regs[0x20], regs[0x1c], regs[0x1b]);
    printf("\n    head:  %04x  cur:   %04x  t0:   %04x  t1:    %04x  t2:   %04x",
	   regs[0x34], regs[0x36], regs[0x30], regs[0x31], regs[0x32]);
    printf("\n    tail:  %04x  cnt:   %04x  t3:   %04x  rxlft: %04x  used: %04x",
	   regs[0x35], regs[0x37], regs[0x33], regs[0x3b], regs[0x3c]);
    printf("\n    txq_s: %04x  txq_f: %04x  Aarm: %04x  Barm:  %04x  fint: %04x\n",
	   regs[0x2a], regs[0x2b], regs[0x28], regs[0x29], regs[0x3a]);
}

/*
 * fddi_print_ip_regs
 * Print out the registers on the FDDI IP
 */

void fddi_print_ip_regs (hwidbtype *hwidb, ushort regs[])
{
    if (hwidb->type == IDBTYPE_FDDIT) 
	fddit_print_regs(regs);
    else if (!(hwidb->fddi_options & FDDI_FAST_CMT_ENB)) {
	fci_print_regs(regs);
    }
}

/*
 * fddi_set_options
 * Set the fddi options correctly for this platform and FDDI IP
 */

void fddi_set_options (hwidbtype *hwidb)
{
    mcitype *cbus;

    cbus = hwidb->devctl;
    if (cbus->flags & CBUSII) {
	/* Must be FDDIT or FIP FDDI card */
	hwidb->fddi_options |= FDDI_LEM_SUPPORTED;
	hwidb->fddi_options |= FDDI_CMT_ASSIST_SUPPORTED;
	hwidb->fddi_options |= FDDI_PHY_GET_LS_SUPPORTED;
    } else {
	/* Must be original (obsolete) FCI FDDI card */
	dcitype *dci;
	dci = cbus->dci[hwidb->cbus_ctrlr_index];
	if ((dci->sw_version & 0xFF00) == 0x8000) 
	    hwidb->fddi_options |= FDDI_VERSION_128;
	else {     
	    hwidb->fddi_options |= FDDI_LEM_SUPPORTED;
	    hwidb->fddi_options |= FDDI_CMT_ASSIST_SUPPORTED;
        }
    }
}

/*
 * fddi_smt_enabled_any_intfc
 * Platform specific.
 * Return true if SMT is enabled on any FDDI interface in the box
 */

boolean fddi_smt_enabled_any_intfc (hwidbtype *hwidb)
{
    mcitype	*cbus;
    dcitype	*dci;
    hwidbtype	*nhwidb;
    int		indx;

    /*
     * Check each IP and return true if any FDDI interface
     * has SMT enabled. Otherwise, return false:
     */
    cbus = hwidb->devctl;
    for (indx = 0; indx < FCI_MAX_DCI_CTRLRS; indx++) {
	dci = cbus->dci[indx];
	if (dci) {
	    if ( (dci->ctrlr_type == FCI_FDDI_CONTROLLER) ||
		(dci->ctrlr_type == FCI_FDDIT_CONTROLLER) ) {
		nhwidb = dci->interface_list[0];
		if (nhwidb->fddi_options & FDDI_SMT_FRAMES)
		    return(TRUE);
	    }
	}
    }
    return(FALSE);
}

/*
 * Platform specific interface reinitialization code
 */

void fip_restart_init (hwidbtype *hwidb)
{
    cdbtype	*cdb;
    mcitype	*cbus;
    mciregtype	*regptr;
#ifdef C7000
    dgtype	*dgbus;
#endif /* C7000 */

    if (!(hwidb->oir_flags & EOIR_RESET)) {
	/*
	 * Select, reset and then restart (load) the controller.
	 */
	cbus = hwidb->devctl;
#ifdef C7000
	dgbus = pslot_to_dbus(hwidb->cbus_ctrlr_index);
	if ((dbus_hw_version(dgbus) >= 0x203))    /* 2.3 or greater*/
	    hwidb->fddi_options |= FDDI_BRIDGE_CAM_SUPPORTED;
	dbus_ctrlr_reset(dgbus);
	dbus_slot_enable(dgbus);
	reg_invoke_slot_enable_led_on(hwidb->cbus_ctrlr_index);
	DELAY(FCI_SOFT_RESET_TIME);
	cdb = controller_find(hwidb->slot, hwidb->slotunit);
#else
	if (((cbus->dci[hwidb->cbus_ctrlr_index]->hw_version) & 0xff) >= 3)
	    hwidb->fddi_options |= FDDI_BRIDGE_CAM_SUPPORTED;

	select_if(hwidb);
	send_ctrlr_reset_cmd(hwidb);
	DELAY(FCI_SOFT_RESET_TIME);
	send_ctrlr_load_cmd(hwidb, hwidb->cbus_ctrlr_index);
	DELAY(FCI_SOFT_RESET_TIME);
	cdb = NULL;
#endif /* C7000 */
    }

    /* 
     * Set up the fddi hack 
     */
    select_if(hwidb);
    send_fip_hack_cmd(hwidb);

    /*
     * Assign the interface to a buffer pool, set max datagramsize,
     * set receive offset and love note pool.
     */

    /* NB - not subinterface ready for bridging! */
    regptr = hwidb->mci_regptr;
    cbus_idb_assign_pool(regptr, hwidb->buffer_pool);
    cbus_idb_rx_max_byte_size(regptr, FDDI_BUFFERSIZE_BYTES);
    cbus_idb_rx_offset(regptr, FDDI_RECEIVE_WORD_OFFSET);
    hes_fddi_set_hold_reg(hwidb, get_fip_byte_rxoffset(hwidb));
    cbus_idb_rx_error(regptr, FCI_RX_ERROR_POOL);
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
    ushort	*sptr;
    uchar	*dbptr;
    mciregtype	*regptr;
    int		indx;
    ushort	status;

    regptr = hwidb->mci_regptr;

    /*
     * Get a claim/beacon buffer and stuff our station address and TRT into it.
     */
    regptr->argreg = FDDI_CLAIM_BEACON_POOL;
    regptr->cmdreg = FCI_CMD_TX0_RESERVE;
    status = regptr->cmdreg;
    if (status != FCI_RSP_OKAY) {
	errmsg(&msgsym(FDDIRSETU, CBUS), hwidb->mci_index,
	       status, "tx0 reserve");
	return(status);
    }

    /*
     * Zero out the entire buffer.
     */
    regptr->argreg = 0;
    regptr->cmdreg = FCI_CMD_TX0_SELECT;
    for (indx = 0; indx < FDDI_CLAIM_BEACON_BUFFER_SIZE / 2; indx++)
	regptr->write0short = 0;

    /*
     * Write the lengths of the claim and beacon portions into the buffer.
     */
    regptr->argreg = FDDI_CLAIM_LENGTH_OFFSET;
    regptr->cmdreg = FCI_CMD_TX0_SELECT;
    regptr->write0short = FDDI_CLAIM_LENGTH;

    regptr->argreg = FDDI_BEACON_LENGTH_OFFSET;
    regptr->cmdreg = FCI_CMD_TX0_SELECT;
    regptr->write0short = FDDI_BEACON_LENGTH;

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
    regptr->argreg = FDDI_CLAIM_FRAME_CODE_OFFSET;
    regptr->cmdreg = FCI_CMD_TX0_SELECT;
    regptr->write0short = (FDDI_CLAIM_FRAME_CODE << 8) | ptr[0];
    sptr = (ushort *) &ptr[1];
    regptr->write0short = *sptr++;
    regptr->write0short = *sptr--;
    regptr->write0short = (ptr[5] << 8) | ptr[0];
    regptr->write0short = *sptr++;
    regptr->write0short = *sptr--;
    regptr->write0short = (ptr[5] << 8) | ((trt >> 24) & 0xFF);
    regptr->write0short = trt >> 8;
    regptr->write0short = ((trt << 8) & 0xFF00);

    /*
     * Write the beacon frame code and our station address as the source
     * address -- the destination address is left zero.
     */
    regptr->argreg = FDDI_BEACON_FRAME_CODE_OFFSET;
    regptr->cmdreg = FCI_CMD_TX0_SELECT;
    regptr->write0short = (FDDI_BEACON_FRAME_CODE << 8);

    regptr->argreg = FDDI_BEACON_SA_OFFSET;
    regptr->cmdreg = FCI_CMD_TX0_SELECT;
    regptr->write0short = (ushort) ptr[0];
    regptr->write0short = *sptr++;
    regptr->write0short = *sptr;
    regptr->write0short = (ushort) (ptr[5] << 8);


    regptr->argreg = FDDI_DIRECT_BEACON_CODE_OFFSET;
    regptr->cmdreg = FCI_CMD_TX0_SELECT;
    regptr->write0short = (FDDI_BEACON_FRAME_CODE << 8)| dbptr[0];
    sptr = (ushort *)&dbptr[1];
    regptr->write0short = *sptr++;
    regptr->write0short = *sptr;
    regptr->write0short = (dbptr[5] << 8) | ptr[0];
    sptr = (ushort *)&ptr[1];
    regptr->write0short = *sptr++;
    regptr->write0short = *sptr;
    regptr->write0short = (ptr[5] << 8) | 0x0001;
    hwidb->fddi_options |= FDDI_SMT_FRAMES;


    /*
     * Reset the firmware pointer and send the buffer to the FALU.
     */
    regptr->argreg = 0;
    regptr->cmdreg = FCI_CMD_TX0_SELECT;
    regptr->argreg = 0;
    regptr->cmdreg = FDDI_CMD_CLAIM_BEACON;
    status = regptr->cmdreg;
    return(status);
}

void fddi_check_for_old_cam_bug (hwidbtype *hwidb)
{
    mciregtype	*regptr;
    mcitype	*cbus;

    cbus = hwidb->devctl;
    regptr = hwidb->mci_regptr;

    /*
     * If the FDDIT h/w is pre 7.2 h/w there is a bug that can cause
     * the CAM to get weird and cause us to stop receiving.  To
     * prevent this we check the h/w rev returned by the ucode and
     * if bad we disable the CAM entirely.
     * N.B. This problem does not affect the 7000.
     */
#ifndef C7000
    if (((cbus->dci[hwidb->cbus_ctrlr_index]->hw_version) & 0xff) < 2) {
	/*
	 * rev 7.0 and 7.1 have the CAM problem.  >= 7.2 are fine.
	 * FDDITs are always Rev 7 (type 7 FDDIT).
	 */
	regptr->argreg = FCRWC_CTL;	/* write control register */
	regptr->cmdreg = FCI_CTRLR_ARGUMENT;
	regptr->argreg = FCI_FDDI_CAM_WR_ARG;
	regptr->cmdreg = FCI_CTRLR_CMD;
	regptr->argreg = 0xa040;	/* disable MA ability */
	regptr->cmdreg = FCI_CTRLR_ARGUMENT;
	regptr->argreg = FCI_FDDI_CAM_WR_ARG;
	regptr->cmdreg = FCI_CTRLR_CMD;
	regptr->argreg = FDDI_CAM_WR_CTL;
	regptr->cmdreg = FCI_CTRLR_ARGUMENT;
	regptr->argreg = FCI_FDDI_CAM_CMD;
	regptr->cmdreg = FCI_CTRLR_CMD;
	
	/*
	 * Next Lock out the CAM.  Just to be really sure.
	 * Note the lock doesn't take until we actully do the
	 * next command that requires lock.
	 */
	regptr->argreg = FDDI_CAM_LOCK_CAM;
	regptr->cmdreg = FCI_CTRLR_ARGUMENT;
	regptr->argreg = FCI_FDDI_CAM_CMD;
	regptr->cmdreg = FCI_CTRLR_CMD;
	
	regptr->argreg = 0;
	regptr->cmdreg = FCI_CTRLR_ARGUMENT;
	regptr->argreg = FCI_FDDI_CAM_WR_ARG;
	regptr->cmdreg = FCI_CTRLR_CMD;
	regptr->argreg = FDDI_CAM_RD_CAM;
	regptr->cmdreg = FCI_CTRLR_ARGUMENT;
	regptr->argreg = FCI_FDDI_CAM_CMD;
	regptr->cmdreg = FCI_CTRLR_CMD;
    }
#endif	
}

/*
 * Send Connect action message to PCM.
 */
void cmt_connect (hwidbtype *idb, int which)
{ 
    leveltype SR;

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
    leveltype	SR;

    if (which & FDDI_PHY_A) {
        SendSignalMulti(idb->fddi_stn_id,SIG_PC_Disable,0,(uInt32)0);
        ((phytype *)(idb->phy[0]))->connect = FALSE;
    }
    if (which & FDDI_PHY_B) {
        SendSignalMulti(idb->fddi_stn_id,SIG_PC_Disable,1,(uInt32)0);
        ((phytype *)(idb->phy[1]))->connect = FALSE;
    }

    /* Process signals */
    if (!EmptySignalQueue()) {
	MDisableCSPInterrupts(&SR);
	ProcessCSP (&SR);
	MRestoreCSPInterrupts(&SR);
    }
}
int get_fip_byte_rxoffset (hwidbtype *hwidb)
{
      /*
       * hwidb->rxoffset is in short words on 7K
       * byte offset is not exactly rxoffset * 2, because its on an odd
       * boundary
       */
      return(FDDI_RECEIVE_BYTE_OFFSET);
}


/* $Id: c7000_dbus.c,v 3.3.20.5 1996/08/28 13:12:10 thille Exp $
 * $Source: /release/112/cvs/Xsys/src-68-c7000/c7000_dbus.c,v $
 *------------------------------------------------------------------
 * dbus for RP/SP
 *
 * March 1994, Carl Schaefer
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: c7000_dbus.c,v $
 * Revision 3.3.20.5  1996/08/28  13:12:10  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.3.20.4  1996/07/08  22:44:56  mwu
 * CSCdi60922:  11.2 Core Features Commit
 * Branch: California_branch
 *
 * Revision 3.3.20.3  1996/06/21  18:20:16  kmitchel
 * CSCdi47923:  DBUS-3-NORESP DBUS-3-BADCTRLRTYPE msgs when booting on RSP
 * Branch: California_branch
 *
 * Revision 3.3.20.2  1996/05/02  17:14:11  rbabu
 * CSCdi44138:  rsp becomes unresponsive when microcode is reloaded from
 * rom
 * Branch: California_branch
 *              Don't allow IPs in RSP to load ucode from ROM.
 *
 * Revision 3.3.20.1  1996/03/18  21:59:55  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.3  1996/03/07  10:43:31  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.2  1996/02/20  17:56:50  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3  1996/02/01  06:09:05  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.2.26.1  1996/01/24  23:02:04  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.2  1995/11/17  18:44:09  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:25:47  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.6  1995/11/08  21:27:15  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.5  1995/09/12  18:22:19  kramling
 * CSCdi37548:  FEIP Parity error requires OIR or Power cycle to clear it
 *
 * Revision 2.4  1995/08/10  19:11:52  schaefer
 * CSCdi38464: need branch patch in 11.0
 * don't crash if two RSPs are installed
 *
 * Revision 2.3  1995/08/09  00:21:46  kramling
 * CSCdi38439:  FEIP product is now supported in 11.0.  Update support to
 * 10.3.
 *
 * Revision 2.2  1995/07/17  08:21:45  schaefer
 * CSCdi36181: more robust microcode management (on RSP)
 *
 * Revision 2.1  1995/06/07  22:55:33  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "interface_private.h"
#include "ieee.h"
#include "logger.h"
#include "../hes/if_fci.h"
#include "../hes/if_mci.h"
#include "../hes/dbus.h"
#include "../hes/dgbus.h"
#include "../hes/ucode.h"
#include "../dev/flash_dvr_spec.h"
#include "../hes/slot.h"
#include "../hes/if_hes_common.h"
#include "../../boot/src-68-c7000/pcmap.h"
#include "../hes/msg_dbus.c"		/* Not a typo, see logger.h */
#include "../hes/msg_ucode.c"		/* Not a typo, see logger.h */

boolean system_mac_addr_avail = FALSE;

uchar system_mac_addr[IEEEBYTES];

/*
 * dbus_system_rp_slot
 * return slot number for route processor
 * assumes SP and RP in adjacent slots - true in current
 * systems but could change in future products
 */
int dbus_system_rp_slot (void) 
{
    mcitype* cbus;

    cbus = cbusQ.qhead;
    if (cbus)
	return (cbus->unit + 1);
    return -1;
}

/*
 * does the specified contain an RP?
 */
boolean
dbus_cpu_slot (uint slot)
{
    return ((int)slot == dbus_system_rp_slot());
}

/*
 * hotswap_determine_system_mac_addr
 * read our "system id" mac address from RP1 EEPROM
 * we use this address so that people can remove boards and install
 * them in other machines without horribly breaking the world due 
 * to duplicate mac addresses.
 */
void hotswap_determine_system_mac_addr (void)
{
    rp1_eeprom_t *idprom;
    dgbus_eeprom_t *common;
    system_mac_addr_avail = FALSE;

    idprom = (rp1_eeprom_t*)IDPROM_BASE;
    common = (dgbus_eeprom_t*)&idprom->dgbus_common;
    if (common->magic == IDPROM_MAGIC) {
	system_mac_addr[0] = 0;
	system_mac_addr[1] = 0;
	system_mac_addr[2] = 0xc;

	system_mac_addr[3] = common->macaddr[0];
	system_mac_addr[4] = common->macaddr[1];
	system_mac_addr[5] = common->macaddr[2];

    } else if (idprom->version == IDPROM_VERSION && idprom->magic == IDPROM_MAGIC) {
	ieee_copy(idprom->macaddr, system_mac_addr);
    } else {
	errmsg(&msgsym(NOMAGIC, DBUS));
	return;
    }
    system_mac_addr_avail = TRUE;
}

/*
 * does the given slot contain anything?
 */
boolean dbus_present (dgtype* dbus)
{
    return touch_device(dbus);
}

/*
 * memd_poke
 * poke val into addr in memd
 * NOT FOR GENERAL USE!
 */
static void memd_poke (mciregtype *regptr, ushort addr, ushort val)
{
    leveltype level;

    level = raise_interrupt_level(NETS_DISABLE);
    regptr->argreg = addr;
    regptr->cmdreg = FCI_CMD_MEMD_SELECT;
    regptr->write0short = val;
    reset_interrupt_level(level);
}

/*
 * sp_clear_icbtype
 * clear out a given IP's ICB type cells
 */
static void sp_clear_icbtype (mcitype* cbus, int slot)
{
    mciregtype* regptr;
    ushort result;
    int if_unit, slotunit;

    if (!eoir_capable)
	return;

    slotunit = -1;
    while ((if_unit = cbus_get_next_if_unit(slot, &slotunit)) >= 0) {
	regptr = (mciregtype*)cbus->regptr;
	regptr->argreg = if_unit;
	regptr->cmdreg = FCI_CMD_GET_ICB;
	if (regptr->cmdreg == FCI_RSP_OKAY)
	    result = regptr->argreg;
	else
	    continue;
	memd_poke(regptr, result, 0xffff);
    }
}


/*
 * cxbus_eoir_reset
 * start any newly inserted cards
 */

void cxbus_eoir_reset (mcitype* cbus)
{
    ushort temp;
    int slot;
    dgtype* dgbus;
    uchar slot_and_complement;
    uchar CxBus_number;

    if (oir_debug)
	buginf("\n Starting cxbus_eoir_reset(%#x)", cbus);

    /*
     * For all daughter cards (i.e. not the SP) on the assumed CxBus
     * number: reset and start them.
     */
    temp = 0;
    for (slot = 0; slot < fci_max_dci_ctrlrs; slot++) {
	if (!(slot_enabled(slot))) {
	    sp_clear_icbtype(cbus, slot);
	    continue;
	}
	if (!(cxbus_card_present(slot))) {
	    sp_clear_icbtype(cbus, slot);
	    continue;
	}
	dgbus = pslot_to_dbus(slot);
	if (!(touch_device(dgbus))) {
	    sp_clear_icbtype(cbus, slot);
	    continue;
	}
	slot_and_complement = dgbus->slot_and_comp;
	if ((slot != (slot_and_complement >> 4 & 0xf)) ||
	    ((slot_and_complement >> 4 & 0xf) !=
	     (~slot_and_complement & 0xf))) {
	    errmsg(&msgsym(SLOTCOMP, DBUS), slot, slot_and_complement>>4 & 0xf,
		   ~slot_and_complement & 0xf);
	    sp_clear_icbtype(cbus, slot);
	    continue;
	}
	CxBus_number = dgbus->CxBus_and_logical_slot >> 4 & 0x3;

	if (!dbus_slot_enabled(dgbus)) {
	    dbus_slot_enable(dgbus);
	    temp++;
	}
    }
    if (temp) {
 	DELAY(FCI_SOFT_RESET_TIME); /* Delay once for all daughters */
    }
    if (oir_debug)
	buginf("\n Ending cxbus_eoir_reset");
} 


void sip_nrzi_control (hwidbtype* idb, boolean enable)
{
    dgtype* dbus;
    mcitype* mci = idb->devctl;
    if (idb->status & IDB_MCI) {
	dbus = pslot_to_dbus(mci->unit);
	if (enable) {
	    dbus_mci_nrzi_enable(dbus, idb->slotunit);
	} else {
	    dbus_mci_nrzi_disable(dbus, idb->slotunit);
	}
    }
}

void sip_invert_txc_control (hwidbtype* idb, boolean enable)
{
    dgtype* dbus;
    mcitype* mci = idb->devctl;
    if (idb->status & IDB_MCI) {
	dbus = pslot_to_dbus(mci->unit);
	if (enable) {
	    dbus_mci_invert_txc_enable(dbus, idb->slotunit);
	} else {
	    dbus_mci_invert_txc_disable(dbus, idb->slotunit);
	}
    }
}

void sip_internal_txc_control (hwidbtype* idb, boolean enable)
{
    dgtype* dbus;
    mcitype* mci = idb->devctl;
    if (idb->status & IDB_MCI) {
	dbus = pslot_to_dbus(mci->unit);
	if (enable) {
	    dbus_mci_internal_clock_enable(dbus, idb->slotunit);
	} else {
	    dbus_mci_internal_clock_disable(dbus, idb->slotunit);
	}
    }
}

/*
 * platform-specific code to handle error in dbus status register
 */
void
dbus_slot_error (uint slot)
{
    dbus_hard_ctrlr_reset(pslot_to_dbus(slot));

    if (slots[slot].consecutive_errs++ >= DBUS_MAX_ERRS) {
	/* "Three strikes, you're out!"
	 * If we get three consecutive dbus errors, then
	 * disable the controller.
	 */
	slots[slot].flags |= BOARD_DISABLED;
    } else {
	/*
	 * Download new ucode to the offending board and set
	 * flag to reset cbus complex.
	 */
	dbus_load_ucode(slot);
	platform_demand_complex_restart(NULL);
    }
    return;
}

/*
 * handle the situation when downloading bundled microcode fails
 */
boolean
rom_ucode_fallback (uint pslot)
{
    errmsg(&msgsym(LDFAIL, UCODE), "system image", pslot, "rom");
    return download_installed_microcode(pslot_to_dbus(pslot), TRUE);
}

/*
 * dbus_no_rom_ucode
 *
 * Return TRUE if the ctrlr_type doesn't have operational code in ROM.
 * The CIP/FEIP/RVIP/SVIP do not have operational code in ROM.  This may 
 * become more common as size of ucode increases.  
 */
inline boolean dbus_no_rom_ucode (uint ctrlr_type)
{
    switch (ctrlr_type) {
    case FCI_CIP_CONTROLLER:
    case FCI_FEIP_CONTROLLER:
    case FCI_RVIP_CONTROLLER:
    case FCI_SVIP_CONTROLLER:
    case FCI_RSP2_CONTROLLER:
	return TRUE;
    default:
	return FALSE;
    }
}

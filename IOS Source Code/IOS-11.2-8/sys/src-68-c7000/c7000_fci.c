/* $Id: c7000_fci.c,v 3.10.18.6 1996/08/28 13:12:12 thille Exp $
 * $Source: /release/112/cvs/Xsys/src-68-c7000/c7000_fci.c,v $
 *------------------------------------------------------------------
 * 7000 specific FCI functions.
 *
 * June 1994, Walter R. Friedrich
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: c7000_fci.c,v $
 * Revision 3.10.18.6  1996/08/28  13:12:12  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.10.18.5  1996/08/07  18:00:47  kmitchel
 * CSCdi49854:  DBUSINTERR after RSP dbus write; ineffective recvry of
 * disabled card
 * Branch: California_branch
 *
 * Revision 3.10.18.4  1996/07/18  19:43:35  snyder
 * CSCdi61844:  given you can only have 1 SSP
 *              forgot to remove %s, 24 more bytes
 * Branch: California_branch
 *
 * Revision 3.10.18.3  1996/07/02  00:10:54  snyder
 * CSCdi61844:  given you can only have 1 SSP
 * Branch: California_branch
 *              calculated pluralization is a bit silly
 *
 * Revision 3.10.18.2  1996/04/23  14:03:20  banderse
 * CSCdi50739:  CIP and other swdbus cards do not need extra reset
 * Branch: California_branch
 * use generic dbus_ctrlr_start() routine.
 *
 * Revision 3.10.18.1  1996/03/18  21:59:57  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.8.12.3  1996/03/07  10:43:33  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.8.12.2  1996/02/20  17:56:53  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.10  1996/02/08  23:10:55  jjohnson
 * CSCdi48362:  Need a consistent, global mechanism for writing plural
 * strings.  Standardize on the Plural and ArgAndPlural macros
 *
 * Revision 3.9  1996/01/25  11:21:56  smackie
 * Registry code gets some much-needed hot-oil massaging, free liposuction
 * and a makeover.  (CSCdi47101)
 *
 *    o Compress registry structures. Saves over 120K for all platforms
 *    o Add registry subsystems and remove feature registry initializion
 *      from registry.c to fix woeful initialization scaling problems
 *    o Remove unused and seemingly unloved registry debugging code
 *    o Add registry memory statistics to shame people into action
 *
 * Revision 3.8.12.1  1996/01/24  23:02:06  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.8  1995/12/26  23:11:16  dlobete
 * CSCdi42282:  chassis mib incomplete for VIP
 * Add support for PA MIB information to be relayed from VIP to RP/RSP
 * via love-letter.
 * Rework some of the 4T PA's data structures as a result of above.
 *
 * Revision 3.7  1995/12/22  23:55:51  tli
 * CSCdi46074:  show controller cbus output inconsistent on SSP
 * Add interrupt protection around reading the Altera version number.
 *
 * Revision 3.6  1995/12/08  05:11:26  tboyle
 * CSCdi44779:  8004 Errors from reset with controller slot select
 *
 * Revision 3.5  1995/12/02  05:50:34  rharagan
 * CSCdi41966:  Messages between 4T PA & driver should be per interface
 * Move to single interface commands, incorporate code review comments.
 *
 * Revision 3.4  1995/11/21  20:20:06  tgrennan
 * CSCdi42932:  VIP on the 70x0 steals mac addresses not assigned to it
 *
 * Assign mac addresses on 2nd PA in reverse order to support any
 * combination of up to 8 interfaces between the 2 PAs.  This depends on
 * CSCdi44254 to prevent duplicate macs with an 8E plus any other adapter.
 *
 * Revision 3.3  1995/11/17  18:44:12  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:31:41  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:25:49  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  21:27:17  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/07/20  00:00:59  rmontino
 * CSCdi37098:  SH VER doesnt display SSP-2M
 *
 * Revision 2.1  1995/06/07  22:55:35  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <ciscolib.h>
#include "sys_registry.h"
#include "fastswitch_registry.h"
#include "../hes/cbus_registry.h"
#include "interface_private.h"
#include "logger.h"
#include "../hes/if_mci.h"
#include "../hes/if_fci.h"
#include "../hes/if_vip.h"
#include "../hes/if_vip_4t.h"
#include "../hes/if_cmd.h"
#include "../if/if_controller.h"
#include "../hes/hes_controller.h"
#include "../hes/dbus.h"
#include "../hes/dgbus.h"
#include "../hes/cbus_registry.regc"
#include "../hes/if_fip.h"
#include "../hes/if_hes_serial.h"
#include "../hes/ucode.h"
#include "../hes/if_hes_common.h"
#include "../hes/ctrlr_cmds.h"
#include "../hes/msg_dbus.c"		/* Not a typo, see logger.h */


/*
 * c7000_get_hw_version
 * Given a dbus pointer, return HW version from EEPROM.  indx argument
 * is not used.
 */
static ushort c7000_get_hw_version (void* bus, uint indx)
{
    return(dbus_hw_version((dgtype*)bus));
}


static void c7000_print_cbus_controller (mcitype* cbus) 
{
    ushort altera_rev = 0;
    mciregtype* regptr;
    leveltype level;

    switch (cbus->hw_version >> 8) {
    case SP_HARDWARE:
	printf("\nSwitch Processor %d, hardware version %d.%d, "
	       "microcode version %d.%d",
	       cbus->unit, (cbus->hw_version >> 8), (cbus->hw_version & 0xFF),
	       (cbus->sw_version >> 8), (cbus->sw_version & 0xFF));
	break;

    case SSP_HARDWARE:
	regptr = cbus->regptr;
	level = raise_interrupt_level(NETS_DISABLE);
	regptr->argreg = FCI_PLD_VERSION;
	regptr->cmdreg = FCI_CMD_VERSION;
	if (regptr->cmdreg == FCI_RSP_OKAY) 
	    altera_rev = regptr->argreg;
	reset_interrupt_level(level);
	printf("\nSwitch Processor %d, hardware version %d.%d, "
	       "microcode version %d.%d"
	       " Altera %d",
	       cbus->unit, (cbus->hw_version >> 8), (cbus->hw_version & 0xFF),
	       (cbus->sw_version >> 8), (cbus->sw_version & 0xFF), altera_rev);

    default:
	break;
    }
    printf("\n  Microcode loaded from %s %s", 
	   get_ucode_src(cbus->unit), 
	   get_ucode_src_file(cbus->unit));
}

/*
 * flush data structures of all references to vanished daughter card
 */ 
static void
flush_dci (mcitype* cbus, dcitype* dci)
{
    int i;
    hwidbtype *idb;

    SAVE_CALLER();

    if (dci == NULL || cbus == NULL) {
	buginf("\nflush_dci(0x%lx, 0x%lx) had NULL arg, caller 0x%lx\n",
	       cbus, dci, caller());
	return;
    }
    
    /* 
     * mark all idbs as non-existent hardware so we don't touch card! 
     */
    for (i = 0; i < MAX_CTRLR_INTERFACES; i++) {
	idb = dci->interface_list[i];
	if (!idb)
	    continue;
	cbus->interface_list[idb->mci_index] = NULL;
	for( ; idb != NULL ; idb = idb->next_idb_on_physint)
	    idb->status |= IDB_NOHARDWARE;
    }
    for (i = 0; i < fci_max_dci_ctrlrs; i++) {
	if (cbus->dci[i] == dci) {
	    cbus->dci[i] = NULL;
	    return;
	}
    }
}

/*
 * assign_mac_addr
 * Allocate a MAC address for an interface based on slot and port.
 */

static boolean
assign_mac_addr (hwidbtype* idb)
{
    ulong newaddr = 0;
    int offset = idb->mci_index;

    /* 
     * if we weren't able to get base address out of EEPROM, fail now
     * and drop back to cookies
     */
    if (!system_mac_addr_avail)
	return (FALSE);

    /* check for illegal configuration attempts */
    if ((idb->slot >= (ulong)fci_max_dci_ctrlrs) ||
	(idb->slotunit > MAX_CTRLR_INTERFACES) ||
	(offset > MAX_CBUS_INTERFACES)) {
	return (FALSE);
    }

    bcopy(system_mac_addr, idb->bia, 3);
    newaddr = (system_mac_addr[3] << 16) |
	(system_mac_addr[4] << 8)  | system_mac_addr[5];

    if (offset >= CBUS2_NON_EXT_INTERFACES) { /* 2nd PA */
	/* 
	 * On VIPs, we assign mac addresses of the second adapter in
	 * reverse order.  This allows any combination of upto 8
	 * interfaces between both adapters.  This assumes that the VIP
	 * will respond to no more than 8 interface selects; thus
	 * avoiding duplicate assignments.  It's probably easiest to
	 * envision this assignment as a mirror between adapters:
	 *
	 *           Port 0           Port 1
	 *      0 1 2 3 4 5 6 7 | 0 1 2 3 4 5 6 7 Interface
	 *	0 1 2 3 4 5 6 7 | 7 6 5 4 3 2 1 0 Mac index/offset
	 *
	 *  Note, with a FE in port 0 and a 8E in port 1, the last
	 *  interface on the 8E isn't accessible; thus rendering it
	 *  a 7E.  However, at least there aren't any duplicate
	 *  address assignments.
	 *
	 * 
	 * We use this formula to calculate the interface offset from
	 * systems base mac address:
	 *
	 * offset = offset - CBUS2_NON_EXT_INTERFACES + PA_MAX_INTERFACES - 1
	 *	- (2 * pa_relative_itf)
	 * or
	 * offset -= CBUS2_NON_EXT_INTERFACES - PA_MAX_INTERFACES + 1
	 *	+ (2 * pa_relative_itf)
	 *
	 * where:
	 * pa_relative_itf = offset % PA_MAX_INTERFACES
	 * but since PA_MAX_INTERFACE is sure to be an even number...
	 * pa_relative_itf = offset & (PA_MAX_INTERFACES - 1)
	 *
	 * thus...
	 */
	offset -= CBUS2_NON_EXT_INTERFACES - PA_MAX_INTERFACES + 1
	  + (2 * (offset & (PA_MAX_INTERFACES - 1)));
    }
    newaddr += offset;
    idb->bia[5] = newaddr & 0xff; newaddr >>= 8;
    idb->bia[4] = newaddr & 0xff; newaddr >>= 8;
    idb->bia[3] = newaddr & 0xff;
    return(TRUE);
}

/*
 * issue_ctrlr_reset
 */
static ushort issue_ctrlr_reset (mcitype *cbus, int slot)
{
    ushort sresult;
    dcitype *dci;
    
    sresult = 0;
    dci = NULL;
    if (cbus) {
	dci = cbus->dci[slot];
	if (dci) {
	    cbus->regptr->argreg = slot;
	    cbus->regptr->cmdreg = FCI_CMD_CTRLR_SELECT;
	    fci_cbus_cmd_arg_rslt(dci, cbus->regptr,
				   &sresult,
				   FCI_CTRLR_CMD,
				   FCI_CTRLR_RESET,
				   CTRLR_CMD_ATTEMPTS,
				   TRUE); 
	} else {
	    /*
	     * can not use retry wrapper at this point, so we'll try
	     * it once the old fasion way.  
	     */
	    cbus->regptr->argreg = slot;
	    cbus->regptr->cmdreg = FCI_CMD_CTRLR_SELECT;
	    cbus->regptr->argreg = FCI_CTRLR_RESET;
	    cbus->regptr->cmdreg = FCI_CTRLR_CMD;
	    sresult = cbus->regptr->cmdreg;
	} 
    } 
    return sresult;
}

/* 
 * cxbus_hard_reset
 */

static void
cxbus_hard_reset (mcitype* cbus) 
{
    int indx;
    dgtype *dgbus;
    uchar slot_and_complement;
    ushort card_type;
    sys_timestamp stime;
/*
 * For all cards on the assumed CxBus number: reset them.
 * Then release the SP to run.
 */
    for (indx = 0; indx <= FCI_MAX_CTRLR_NUMBER; indx++) {
	dgbus = pslot_to_dbus(indx);
	if (!(cxbus_card_present(indx)))
	    continue;		/* ignore SIP cards */
	if (!(touch_device(dgbus)))
	    continue;
	slot_and_complement = dgbus->slot_and_comp;
	if ((indx != (slot_and_complement >> 4 & 0xf)) ||
	    ((slot_and_complement >> 4 & 0xf) !=
	     (~slot_and_complement & 0xf))) {
	    errmsg(&msgsym(SLOTCOMP, DBUS), indx, slot_and_complement>>4 & 0xf,
		   ~slot_and_complement & 0xf);
	    continue;
	}
	if (!(cxbus_card_present(indx)))
	    continue;		/* don't molest SIP */

	card_type = dbus_controller_type(dgbus);
	dbus_xilinx_reload(dgbus, TRUE);
	GET_TIMESTAMP(stime);
	dbus_ctrlr_reset(dgbus);
#ifdef DEBUG_PARALLEL
	ttyprintf (CONTTY,"cxbus hard reset, dbus_ctrlr_reset slot %d took %d\n",indx,ELAPSED_TIME(stime));
#endif	
	/* VIP needs ucode loaded after a reset */
	if (vip_controller(card_type)) {
	    GET_TIMESTAMP(stime);
	    dbus_load_ucode(indx);
#ifdef DEBUG_PARALLEL
	    ttyprintf (CONTTY,"cxbus hard reset load vip slot %d took %d\n",indx,ELAPSED_TIME(stime));
#endif	
	} 
    }
    dgbus = pslot_to_dbus(cbus->unit); /* SP */
    /* run */
    dbus_prot_wr(cbus->unit, &dgbus->diag_control, MASTER_ENABLE|WCS_LOADED|ENABLE_LED);
    DELAY(FCI_HARD_RESET_TIME);
}

/* 
 * cxbus_soft_reset
 */

void
cxbus_soft_reset (mcitype* cbus)
{
    ushort temp;
    int jndx, appl_no;
    dgtype* dgbus;
    uchar slot_and_complement;
    ushort card_type;
    sys_timestamp stime;

/*
 * For all daughter cards (i.e. not the SP) on the assumed CxBus number: reset
 * and start them.
 */
    temp = 0;
    for (jndx = 0; jndx <= FCI_MAX_CTRLR_NUMBER; jndx++) {
	if (!(slot_enabled(jndx))) {
	    continue;
	}
	if (!(cxbus_card_present(jndx))) {
	    continue;
	}
	dgbus = pslot_to_dbus(jndx);
	if (!(touch_device(dgbus)))
	    continue;
	slot_and_complement = dgbus->slot_and_comp;
	if ((jndx != (slot_and_complement >> 4 & 0xf)) ||
	    ((slot_and_complement >> 4 & 0xf) !=
	     (~slot_and_complement & 0xf))) {
	    errmsg(&msgsym(SLOTCOMP, DBUS), jndx, slot_and_complement>>4 & 0xf,
		   ~slot_and_complement & 0xf);
	    continue;
	}
	/* reset and start the card if it is not the switch processor
	   or a multibus interface */
        if (!switchproc_present(dgbus) && !swbus_present(dgbus)) {
            card_type = dbus_controller_type(dgbus);

	    GET_TIMESTAMP(stime);
	    dbus_ctrlr_start(dgbus, card_type);
#ifdef DEBUG_PARALLEL
	    ttyprintf (CONTTY, "cxbus soft reset ctrlr_start of controller %d took %d\n",card_type,ELAPSED_TIME(stime));
#endif

	    if (vip_controller(card_type)) {
		/*
		 * Send a FCI_CTRLR_RESET command to tell the VIP
		 * to write it's icbtypes back to memd.
		 */
		issue_ctrlr_reset(cbus, jndx);
	    }

	    temp++;
	}
    }
    if (temp) {
	DELAY(FCI_SOFT_RESET_TIME); /* Delay once for all daughters */
    }

    /* some controllers need to do post-reset processing */
    /* the SP has no controllers, so count up to (fci_max_dci_ctrlrs - 1) */
    for (jndx = 0 ; jndx < fci_max_dci_ctrlrs; jndx++) {
	dgbus = pslot_to_dbus(jndx);
	if (!(touch_device(dgbus)))
	    continue;
	for (appl_no = 0 ; appl_no < CBUS2_MAX_CTRLR_INTERFACES ; appl_no++) {
	    cdbtype *cdb;
	    
	    cdb = controller_find(jndx, appl_no);
	    if (cdb != NULL && cdb->reset != NULL)
		(cdb->reset)(cdb);
	}
    }
}    

/*
 * c7000_num_dci_ctrlrs
 */
static int c7000_num_dci_ctrlrs (void)
{
    int sp_slot;

    for (sp_slot = 0; sp_slot < FCI_MAX_DCI_CTRLRS; ++sp_slot) {
	dgtype* dbus;

	dbus = pslot_to_dbus(sp_slot);
	if (touch_device(dbus) &&
	    switchproc_present(dbus))
	    break;
    }
    return sp_slot;
}

void cbus_platform_registry_init (void)
{
    reg_add_cbus_hard_reset(cxbus_hard_reset, "cxbus_hard_reset");
    reg_add_cbus_soft_reset(cxbus_soft_reset, "cxbus_soft_reset");
    reg_add_assign_mac_addr(assign_mac_addr, "assign_mac_addr");
    reg_add_fddi_ringstate_leds(fip_leds, "fip_leds");
    reg_add_slot_enable_led_on(slot_enabled_led_on, "slot_enabled_led_on");
    reg_add_disable_slot(disable_slot, "disable_slot");
    reg_add_flush_dci(flush_dci, "flush_dci");
    reg_add_slot_enabled(slot_enabled, "slot_enabled");
    reg_add_switch_proc_present(switchproc_present, "switchproc_present");
    reg_add_cxbus_card_present(cxbus_card_present, "cxbus_card_present");
    reg_add_send_ipc_slave_cmd(send_ipc_slave_cmd, "send_ipc_slave_cmd");
    reg_add_get_controller_type((service_get_controller_type_type)
				dbus_controller_type, "dbus_controller_type");
    reg_add_get_hw_version(c7000_get_hw_version, "c7000_get_hw_version");

    /*
     * Serial specific stuff common with the RSP has moved to:
     * ../hes/if_hes_serial.c
     */

    reg_add_nrzi_control(IDBTYPE_MCISERIAL, sip_nrzi_control,
			 "sip_nrzi_control");
    reg_add_invert_txclock_control(IDBTYPE_MCISERIAL, sip_invert_txc_control,
				   "sip_invert_txc_control");
    reg_add_internal_txclock_control(IDBTYPE_MCISERIAL,
				     sip_internal_txc_control,
				     "sip_internal_txc_control");

    fci_max_dci_ctrlrs = c7000_num_dci_ctrlrs();

    reg_add_print_cbus_controller(c7000_print_cbus_controller,
				  "c7000_print_cbus_controller");

    reg_add_show_pa_specific(IDBTYPE_CBUSSERIAL, cbus_vip_4t_show_controller,
			     "cbus_vip_4t_show_controller");
    
    reg_add_initialize_pa_info(cbus_initialize_pa_info, 
			       "cbus_initialize_pa_info");

    reg_add_priv_pa_info_op(cbus_priv_pa_info_op, "cbus_priv_pa_info_op");
}

/*
 * print_cxbus_controllers
 * Print the number of cxbus controllers.
 */

void print_cxbus_controllers (void)
{
    int nsp, nssp, n2mssp, vers;
    mcitype *cbus;

    n2mssp = 0;
    nsp = 0;
    nssp = 0;
    for (cbus = (mcitype *)cbusQ.qhead; cbus; cbus = cbus->next) {
	vers = cbus->hw_version >> 8;
	if (vers == SP_HARDWARE)
	    nsp++;
	else if (vers == SSP_HARDWARE) {
            if ((cbus->memd_size+1)/512 == 2048)
                n2mssp++;
            else
	        nssp++;
        }
    }
   
    if (n2mssp)
	printf("%d%s Switch Processor\n", n2mssp, " 2MB Silicon");
    if (nssp)
	printf("%d%s Switch Processor\n", nssp, " Silicon");
    if (nsp)
	printf("%d%s Switch Processor\n", nsp, ""); 
        /*        ** %s so optimizer will share string with above */
}

/*
 * verify that a given ucode rev is right for a certain card
 */

boolean ucode_version_ck (ctrlr_type_t ctrlr, uint major, uint minor)
{
    return TRUE;		/* XXX - do real version checking */
}

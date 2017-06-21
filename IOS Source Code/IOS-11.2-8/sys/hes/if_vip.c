/* $Id: if_vip.c,v 3.10.6.8 1996/08/20 00:34:59 cheilek Exp $
 * $Source: /release/112/cvs/Xsys/hes/if_vip.c,v $
 *------------------------------------------------------------------
 * if_vip.c:  Stuff for the Versatile Interface Processor (VIP)
 *
 * July 1995, David Getchell
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: if_vip.c,v $
 * Revision 3.10.6.8  1996/08/20  00:34:59  cheilek
 * CSCdi58374:  More show diag info needed
 * Branch: California_branch
 * Add CCB_CMD_VIP_GET_CONFIG, vip_ll_sw.
 * Add sw_version_str to slot structure.
 *
 * Revision 3.10.6.7  1996/08/07  08:59:13  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.10.6.6  1996/07/10  02:37:46  getchell
 * Added crash context mechanism which is shown via diag command and test
 * cbus (vc command)
 * CSCdi60415:  VIP difficult to debug in the field
 * Branch: California_branch
 *
 * Revision 3.10.6.5  1996/06/21  21:53:47  drowell
 * CSCdi60840:  Incorrect ignores reported for FEIP
 * Branch: California_branch
 * Made the vip love letter parser only look at the drops field if the
 * love letter was from a vip.
 *
 * Revision 3.10.6.4  1996/05/21  06:29:28  mordock
 * Branch: California_branch
 * Improve snmp modularity via creating services to register interfaces
 * and chassis cards.
 * Add syslog mib.
 *
 * Revision 3.10.6.3  1996/05/08  04:31:56  tboyle
 * CSCdi54363:  SW-VIP2: Dropped packets are unaccountable in FDDI-PA.
 * Branch: California_branch
 *
 * Revision 3.10.6.2  1996/04/30  07:00:45  fsunaval
 * CSCdi51250:  SW-VIP1: Host MIB updating needs to be completed
 * Branch: California_branch
 * 1.Code changes for formation and transfer of SMT Messages to RSP
 *   in form of love letters.
 *
 * Revision 3.10.6.1  1996/03/18  19:42:33  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.8.2.4  1996/03/16  06:51:14  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.8.2.3  1996/03/13  01:22:34  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.8.2.2  1996/02/20  00:52:43  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.10  1996/03/06  01:35:38  jwelder
 * vip_parse_hw_info() added to parse the vip lovenote sending the
 *     Dram Size on the vip. This function can be added to in the
 *     future to pass futher info about the Vip card. (Eeprom Size?)
 *     (Static Ram Size?)
 * CSCdi50800:  Show Diagbus changed to show VIP1 Dram Size
 *
 * Revision 3.9  1996/02/01  06:02:49  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.8.2.1  1996/01/24  21:47:24  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.8  1996/01/20  00:10:11  dlobete
 * CSCdi44907:  FEIP Interface counters are in error and mean-ing-less
 * Love-letter code path shared by both VIP and FEIP was only
 * working for the VIP.
 *
 * Revision 3.7  1996/01/11  03:50:32  kao
 * CSCdi43878:  IS_VIP() macro should be used
 *
 * Revision 3.6  1995/12/26  23:08:22  dlobete
 * CSCdi42282:  chassis mib incomplete for VIP
 * Add support for PA MIB information to be relayed from VIP to RP/RSP
 * via love-letter.
 * Rework some of the 4T PA's data structures as a result of above.
 *
 * Revision 3.5  1995/12/02  05:50:13  rharagan
 * CSCdi41966:  Messages between 4T PA & driver should be per interface
 * Move to single interface commands, incorporate code review comments.
 *
 * Revision 3.4  1995/11/29  01:42:14  sthormod
 * CSCdi44582:  rVIP: enabling a 4R or 4T interfaces crashes on RSP
 *
 * Don't use ether subblock on non-ether interfaces.
 *
 * Revision 3.3  1995/11/17  09:19:02  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:28:59  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:41:15  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:01:03  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/07/19  19:18:21  getchell
 * Placeholders for VIP development
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "string.h"
#include "interface_private.h"
#include "../if/ether.h"
#include "../if/ether_private.h"
#include "../snmp/chassismib_registry.h"

#include "../if/rvip_debug_flags.h"

#include "if_mci.h"
#include "if_fci.h"
#include "if_vip.h"
#include "if_vip_4t.h"
#include "if_vip_4r.h"
#include "ucode.h"
#include "../dev/flash_dvr_spec.h"
#include "slot.h"
#include "../src-4k/gdb_4k.h"


static char *const r4k_reg_names[] = {
    "$0", "AT", "v0", "v1",
    "a0", "a1", "a2", "a3",
    "t0", "t1", "t2", "t3",
    "t4", "t5", "t6", "t7",
    "s0", "s1", "s2", "s3",
    "s4", "s5", "s6", "s7",
    "t8", "t9", "k0", "k1",
    "gp", "sp", "s8", "ra" 
};

typedef struct context_plus_ {
    r4k_context_t context;
    ulong tracebuffer[CRASH_TRACEBACK_COUNT];
} context_plus_t;

/*
 * vip_print_context:
 * Print out vip context using specified line preface 
 */
void vip_print_context (char *preface, void *c)
{
    context_plus_t *cp = c;
    r4k_context_t *cntxt;
    int i;

    cntxt = &cp->context;

    for (i=0; i<8; i++) {
	printf("%s", preface);
	printf("%s : %08x, %s : %08x, %s : %08x, %s : %08x, \n",
	       r4k_reg_names[(i*4)],   cntxt->regs[(i*8)+1],
	       r4k_reg_names[(i*4)+1], cntxt->regs[(i*8)+3],
	       r4k_reg_names[(i*4)+2], cntxt->regs[(i*8)+5],
	       r4k_reg_names[(i*4)+3], cntxt->regs[(i*8)+7]);
    }
    printf("%s", preface);
    printf("EPC : 0x%08x, SREG : 0x%08x, Cause : 0x%08x\n",
	   cntxt->epc, cntxt->sreg, cntxt->cause);
    printf("%s", preface);
    printf("ErrorEPC : 0x%08x\n", cntxt->err_epc);

    /*
     * Print traceback
     */
    if (cp->tracebuffer[0]) {
	printf("%s", preface);
	printf("Traceback= ");
	for (i = 0; i < CRASH_TRACEBACK_COUNT; i++) {
	    if (cp->tracebuffer[i]) {
		printf("0x%8x ", cp->tracebuffer[i]);
	    } else {
		break;
	    }
	    if (((i+1) % 4) == 0 ) {
		printf("\n");
		printf("%s", preface);
		printf("           ");
	    }
	}
	printf("\n");
    }
}

/*
 * vip_controller
 *
 * Returns TRUE if card_type indicates a VIP controller
 */
boolean vip_controller (ushort card_type)
{
    if ((card_type == FCI_RVIP_CONTROLLER) ||
	(card_type == FCI_SVIP_CONTROLLER))
	return TRUE;
    else
	return FALSE;
}

/*
 * vip_parse_eeprom_info
 *
 * Process IDE eeprom info from VIP.
 */
void vip_parse_eeprom_info (hwidbtype *hwidb, uchar *love_ptr)
{
    char *cp;
    char *swver;
    char *hwver;
    char buf[48];
    vip4t_privdata_t    *vip4t_privinfo;

    vip_ll_eeprom *epr_ptr = (vip_ll_eeprom *)love_ptr;
    slottype *slot_ptr = &slots[hwidb->slot];
    uint pa = (hwidb->slotunit < PA_VIRTUAL_SLOT_BOUNDARY) ? 0 : 1;
    pa_info_slots *pa_info_ptr = slot_ptr->pa_info[pa];
    if (!pa_info_ptr)
	return;
   
    pa_info_ptr->eeprom_version = epr_ptr->eeprom_version;
    pa_info_ptr->controller_type = epr_ptr->controller_type;
    pa_info_ptr->hardware_version = epr_ptr->hardware_version;
    pa_info_ptr->serial_number = epr_ptr->serial_number;
    pa_info_ptr->part_number = epr_ptr->part_number;
    pa_info_ptr->board_revision = epr_ptr->board_revision;
    
    switch(pa_info_ptr->pa_type) {
      case FCI_TYPE_VIP_SERIAL4:
	if ((vip4t_privinfo = pa_info_ptr->private_pa_info))
	    pa_info_ptr->software_version = vip4t_privinfo->s4t_pa_sw_version;
	break;
	
      default:
	break;
    }

    cp = buf;
    cp += sprintf(hwver = cp, "%u.%u", pa_info_ptr->hardware_version >> 8, 
			pa_info_ptr->hardware_version & 0xFF);
    /* PAs don't always have a software version */
    if (pa_info_ptr->software_version) 
    	cp += sprintf(swver = ++cp, "%u.%u", 
			pa_info_ptr->software_version >> 8, 
			pa_info_ptr->software_version & 0xFF);
    else
    	cp += sprintf(swver = ++cp, "");

    reg_invoke_chassismib_update_sub_card_entry(
	reg_invoke_chassismib_find_sub_card_index(
		       reg_invoke_chassismib_find_slot_card_index(hwidb->slot),
                       pa),
	pa_info_ptr->serial_number, hwver, swver);
}

/* 
 * vip_parse_hw_info(): Parse the info about the vip hardware 
 *  including Dram Size
 */


void vip_parse_hw_info (hwidbtype *hwidb, uchar *love_ptr)
{

    vip_ll_hw *hw_ptr = (vip_ll_hw *)love_ptr;
    slottype *slot_ptr = &slots[hwidb->slot];

    slot_ptr->mem_size= hw_ptr->mem_size;
    slot_ptr->io_size= hw_ptr->io_size;
}

/* 
 * vip_parse_sw_info(): Parse the info about the vip software
 *  including IOS version string.
 */


void vip_parse_sw_info (hwidbtype *hwidb, uchar *love_ptr)
{

    vip_ll_sw *sw_ptr = (vip_ll_sw *)love_ptr;
    slottype *slot_ptr = &slots[hwidb->slot];

    strncpy (slot_ptr->sw_version_str, sw_ptr->ios_version, VERSION_STR_LEN);
}

/*
 * vip_parse_love_letter:
 * Parse a love letter from an interface on a VIP
 */
void vip_parse_love_letter (hwidbtype *hwidb, uchar *love_ptr, ulong length)
{
    vip_ll *ll = NULL;
    vip_ll_errors *card_errors = NULL;
    ulong ll_type;
    ethersb	*esb = NULL;

    /*
     * Check for non-VIP fast-ethernet (FEIP) which does not use the
     * ll_type field
     */
    if (is_vip(hwidb)) {
        ll = (vip_ll *)love_ptr;
        ll_type = ll->ll_type;
	card_errors = &ll->data;
    } else {
        ll_type = VIP_LL_STATS;
	card_errors = (vip_ll_errors *)love_ptr;
    }

    switch (ll_type) {
    case VIP_LL_STATS:
	hwidb->counters.output_error += card_errors->output_error;
	hwidb->counters.output_collision += card_errors->output_collision;
	hwidb->counters.output_underrun += card_errors->output_underrun;
	hwidb->counters.input_error += card_errors->input_error;
	hwidb->counters.runts += card_errors->runts;
	hwidb->counters.giants += card_errors->giants;
	hwidb->counters.input_frame += card_errors->input_frame;
	hwidb->counters.input_crc += card_errors->input_crc;
	hwidb->counters.input_overrun += card_errors->input_overrun;
	hwidb->counters.input_resource += card_errors->input_resource;
	/*
	 * If it's a VIP, update the drop and abort errors,
	 * otherwise, ignore them.
	 */
	if (IS_VIP(hwidb)) {
	    hwidb->counters.input_resource += card_errors->input_drops;
        hwidb->counters.input_abort += card_errors->input_abort;
	}

	if (hwidb->status & IDB_ETHER) {
	    /*
	     * esb is only valid for ethernet & fast ethernet
	     */
	    esb = idb_use_hwsb_inline(hwidb, HWIDB_SB_ETHER);	
	}
	switch (hwidb->type) {
	case IDBTYPE_FEIP:
	    esb->feip_no_carrier += card_errors->feip_no_carrier;
	    esb->feip_multicast += card_errors->feip_multicast;
	    esb->feip_watchdog += card_errors->feip_watchdog;
	    /* no break so that we also add counters below */
	case IDBTYPE_CBUSETHER:
	    esb->output_babbles += card_errors->output_babbles;
	    esb->output_lost_carrier += card_errors->output_lost_carrier;
	    esb->output_late_collision += card_errors->output_late_collision;
	    esb->output_excessive_collision += 
		card_errors->output_excessive_collision;
	    esb->output_deferred += card_errors->output_deferred;
	    idb_release_hwsb_inline(hwidb, HWIDB_SB_ETHER);
	    break;
	default:
	}
	return;
    case VIP_LL_EEPROM:
	vip_parse_eeprom_info(hwidb, love_ptr);
	break;
    case VIP_LL_HW:
	vip_parse_hw_info(hwidb, love_ptr);
	break;
    case VIP_LL_SW:
	vip_parse_sw_info(hwidb, love_ptr);
	break;
    case VIP_LL_4R_ERR_MSG:
	parse_vip_4r_err_ll(hwidb, love_ptr);
	break;
    case VIP_LL_4R_SMT_STATS:
	parse_vip_4r_smt_ll(hwidb, love_ptr);
	break;
    case VIP_LL_4R_ERROR_STATS:
	parse_vip_4r_error_ll(hwidb, love_ptr);
	break;
    case VIP_LL_4T_STATUS:
	parse_vip_4t_status_ll(hwidb, love_ptr);
	break;

    default: 
	break;
    }
}

void cbus_initialize_pa_info (hwidbtype *hwidb, mcitype *mci)
{
    int pa;
    slottype *slot_ptr = &slots[hwidb->slot];
    pa_info_slots *pa_info_ptr;
    dcitype *dci = mci->dci[hwidb->cbus_ctrlr_index];
    char *pa_name;
 
    if (dci->ctrlr_type == FCI_FEIP_CONTROLLER) {
        pa = hwidb->slotunit;
        pa_name = "FEIP PA info";
    } else {
        pa = (hwidb->slotunit < PA_VIRTUAL_SLOT_BOUNDARY) ? 0 : 1;
        pa_name = "VIP PA info";
    }
    if (!slot_ptr->pa_info[pa]) {
        slot_ptr->pa_info[pa] =
            malloc_named(sizeof(pa_info_slots), pa_name);
        if ((pa_info_ptr = slot_ptr->pa_info[pa]))
        pa_info_ptr->pa_type = dci->pa_type[pa];
    }
    pa_info_ptr = slot_ptr->pa_info[pa];
    if (pa_info_ptr) {
        pa_info_ptr->pa_ifcount++;
    }
}
/*
 * Either malloc for a particular bay or
 * free both private_pa_info structs for a slot.
 */
void cbus_priv_pa_info_op (hwidbtype *hwidb, boolean do_init, int slot)
{
    slottype *slot_ptr = &slots[slot];
    int pa;
    pa_info_slots *pa_info_ptr;

    if (do_init) {
	switch (hwidb->type) {
	  case IDBTYPE_CBUSSERIAL:
	    if (!hwidb->vip4t_data)
		hwidb->vip4t_data = 
		    malloc_named(sizeof(vip4t_data_t), "VIP 4T Data");
 
	    pa = (hwidb->slotunit < PA_VIRTUAL_SLOT_BOUNDARY) ? 0 : 1;
	    if ((pa_info_ptr = slot_ptr->pa_info[pa])) {
		if (!pa_info_ptr->private_pa_info)
		    pa_info_ptr->private_pa_info =
			malloc_named(sizeof(vip4t_privdata_t),
				     "VIP 4T Data(P)");
	    }
	    break;
	    
	  default:
	    break;
	}
    } else {
	for(pa=0; pa < SLOT_NUM_VIP_PAS; pa++) {
	    if ((pa_info_ptr = slot_ptr->pa_info[pa])) {
		if (pa_info_ptr->private_pa_info)
		    free(pa_info_ptr->private_pa_info);
		free(slot_ptr->pa_info[pa]);
	    }
	    slot_ptr->pa_info[pa] = 0;
	}
    }
}



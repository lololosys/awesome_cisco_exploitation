/* $Id: pas_controller.c,v 3.4.20.5 1996/07/04 03:02:25 mbeesley Exp $
 * $Source: /release/112/cvs/Xsys/pas/pas_controller.c,v $ 
 *------------------------------------------------------------------
 * pas_controller.c - PCI port-adaptor based controller support
 *
 * June 1995 - Michael Beesley
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: pas_controller.c,v $
 * Revision 3.4.20.5  1996/07/04  03:02:25  mbeesley
 * CSCdi62104:  Update PA/PM id enumerations
 * Branch: California_branch
 *
 * Revision 3.4.20.4  1996/05/31  23:44:11  mbeesley
 * CSCdi59046:  Implement config guidelines
 * Branch: California_branch
 *
 * Revision 3.4.20.3  1996/05/21  10:00:12  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.4.20.2  1996/04/29  17:57:32  ssangiah
 * CSCdi54472: Inadequate information in the output of "sh controllers"
 *             command
 *   o Add a "show diag" command for the c7200 platform.
 *   o Make the output from "show controllers" consistent.
 * Branch: California_branch
 *
 * Revision 3.4.20.1  1996/03/21  23:24:21  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.3  1996/02/21  23:06:01  mbeesley
 * CSCdi49494:  Predator needs to support cdbs for channelized interfaces
 * Branch: ELC_branch
 *
 * Revision 3.2.2.2  1996/02/08  08:48:28  gstovall
 * Branch: ELC_branch
 * Sync to ELC_baseline_960205.
 *
 * Revision 3.2.2.1  1996/01/16  01:44:04  dcarroll
 * Branch: ELC_branch
 * Sync ELC_branch between ELC_baseline_120995 and ELC_baseline_960110.
 *
 * Revision 3.4  1996/01/29  07:29:21  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.3  1995/12/17  18:36:04  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  18:55:32  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:58:32  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:21:56  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/19  21:54:36  mbeesley
 * Add placeholders for new development.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "interface_private.h"
#include "interface.h"
#include "logger.h"
#include "config.h"
#include "parser.h"
#include "logger.h"
#include "plural.h"

#include "../if/if_controller.h"
#include "pas_controller.h"
#include "if_pas.h"
#include "pas_id.h"

#include "../hes/if_fci.h"
#include "../dev/flash_dvr_spec.h"
#include "../hes/ucode.h"
#include "../hes/slot.h"
#include "../hes/dgbus.h"

#include "../parser/parser_public.h"
#include "../parser/parser_actions.h"

/*
 * Port Adaptor types descriptive strings
 */
static const char *const pa_hw_desc_strings[] = {
	"Fast-ethernet",		/* PA_HARDWARE_1FE */
	"Ethernet",			/* PA_HARDWARE_8E */
	"Ethernet",			/* PA_HARDWARE_4E */
	"Ethernet",			/* PA_HARDWARE_5E */
	"Serial",			/* PA_HARDWARE_4T */
	"Token-ring",			/* PA_HARDWARE_4R */
	"Channelized E1",		/* PA_HARDWARE_CE1 */
	"Channelized T1",		/* PA_HARDWARE_CT1 */
	"Encryption engine",		/* PA_HARWDARE_EEC */
	"Compression engine (768k)",	/* PA_HARDWARE_CEC */
	"FDDI DAS (Multimode)",		/* PA_HARDWARE_FDDI_DAS_MM */
	"FDDI DAS (Singlemode)",	/* PA_HARDWARE_FDDI_DAS_SS */
	"Mueslix serial",		/* PA_HARDWARE_M4T */
	"Mueslix serial (V.35)",	/* PA_HARDWARE_M8T_V35 */
	"Mueslix serial (X.21)",	/* PA_HARDWARE_M8T_X21 */
	"Mueslix serial (RS-232)",	/* PA_HARDWARE_M8T_RS232 */
	"HSSI",				/* PA_HARDWARE_DUAL_HSSI */
	"Fast-ethernet (TX-ISL)",	/* PA_HARDWARE_1FE_TX_ISL */
	"Fast-ethernet (FX-ISL)",	/* PA_HARDWARE_1FE_FX_ISL */
	"Fast-ethernet (TX-nISL)",	/* PA_HARDWARE_1FE_TX_NISL */
	"Fast-ethernet on C7200 I/O card", /* PA_HARDWARE_C7100_IO1FE */
	"C7200 CPU card",		/* PA_HARDWARE_C7100_CPU */
	"Amdahl",			/* PA_HARDWARE_AMDAHL */
	"ATM OC3 (MM)",			/* PA_HARDWARE_ATM_OC3_MM */
	"Compression engine (3M)",      /* PA_HARDWARE_CEC_3M */
	"Packet Over Sonet",		/* PA_HARDWARE_POSIP */
	"CT3",				/* PA_HARDWARE_CT3 */
        "100VG",			/* PA_HARDWARE_100VG */
	"Fast-ethernet (FX-nISL)",	/* PA_HARDWARE_1FE_FX_NISL */
	"HSSI",				/* PA_HARDWARE_SINGLE_HSSI */
	"Combo 1E, 2W",			/* PM_HARDWARE_2E2W */
	"Combo 1E, 1R, 2W",		/* PM_HARDWARE_1E1R2W */
	"BRI (S/T)",			/* PM_HARDWARE_MBRI_4ST */
	"BRI (S/T)",			/* PM_HARDWARE_MBRI_8ST */
	"BRI (U)",			/* PM_HARDWARE_MBRI_4U */
	"BRI (U)",			/* PM_HARDWARE_MBRI_8U */
	"Sync/Async",			/* PM_HARDWARE_CD2430_4AS */
	"Sync/Async",			/* PM_HARDWARE_CD2430_8AS */
	"CT1 (CSU)",			/* PA_HARDWARE_T1_1PORT_CSU */
	"CT1 (CSU)",			/* PA_HARDWARE_T1_2PORT_CSU */
	"CT1",				/* PA_HARDWARE_T1_1PORT */
	"CT1",				/* PA_HARDWARE_T1_2PORT */
	"CE1",				/* PA_HARDWARE_E1_1PORT */
	"CE1",				/* PA_HARDWARE_E1_2PORT */
	"ATM OC3 (SM)",			/* PA_HARDWARE_ATM_OC3_SM */
	"ATM OC3 (UTP5)",		/* PA_HARDWARE_ATM_OC3_UTP5 */
	"Bus and Tag",			/* PA_HARDWARE_BUS_AND_TAG */
	"Escon",			/* PA_HARDWARE_ESCON */
	"Ethernet",			/* PA_HARDWARE_16E */
	"Dual attach, FD, MM",		/* PA_HARDWARE_FMM_FD */
	"Dual attach, FD, SM",		/* PA_HARDWARE_FSM_FD */
	"UNKNOWN",			/* PA_HARDWARE_UNKNOWN_51 */
	"UNKNOWN",			/* PA_HARDWARE_UNKNOWN_52 */
	"C7200 CPU card",		/* PA_HARDWARE_C7100_JUNIOR */
	"C7200 IO card",		/* PA_HARDWARE_C7100_IO */
	"UNKNOWN"			/* PA_NUM_HARDWARE_TYPES */
};

/*
 * Port Adaptor bandwidth array
 */
static const ushort pa_bandwidth_array[] = {
	200,				/* PA_HARDWARE_1FE */
	80,				/* PA_HARDWARE_8E */
	40,				/* PA_HARDWARE_4E */
	50,				/* PA_HARDWARE_5E */
	16,				/* PA_HARDWARE_4T */
	64,				/* PA_HARDWARE_4R */
	3,				/* PA_HARDWARE_CE1 */
	3,				/* PA_HARDWARE_CT1 */
	10,				/* PA_HARWDARE_EEC */
	10,				/* PA_HARDWARE_CEC */
	100,				/* PA_HARDWARE_FDDI_DAS_MM */
	100,				/* PA_HARDWARE_FDDI_DAS_SS */
	16,				/* PA_HARDWARE_M4T */
	32,				/* PA_HARDWARE_M8T_V35 */
	32,				/* PA_HARDWARE_M8T_X21 */
	8,				/* PA_HARDWARE_M8T_RS232 */
	200,				/* PA_HARDWARE_DUAL_HSSI */
	200,				/* PA_HARDWARE_1FE_TX_ISL */
	200,				/* PA_HARDWARE_1FE_FX_ISL */
	200,				/* PA_HARDWARE_1FE_TX_NISL */
	200, 				/* PA_HARDWARE_C7100_IO1FE */
	0,				/* PA_HARDWARE_C7100_CPU */
	0,				/* PA_HARDWARE_AMDAHL */
	155,				/* PA_HARDWARE_ATM_OC3_MM */
	10,      			/* PA_HARDWARE_CEC_3M */
	155,				/* PA_HARDWARE_POSIP */
	110,				/* PA_HARDWARE_CT3 */
        100,				/* PA_HARDWARE_100VG */
	200,				/* PA_HARDWARE_1FE_FX_NISL */
	100,				/* PA_HARDWARE_SINGLE_HSSI */
	18,				/* PM_HARDWARE_2E2W */
	34,				/* PM_HARDWARE_1E1R2W */
	4,				/* PM_HARDWARE_MBRI_4ST */
	8,				/* PM_HARDWARE_MBRI_8ST */
	4,				/* PM_HARDWARE_MBRI_4U */
	8,				/* PM_HARDWARE_MBRI_8U */
	1,				/* PM_HARDWARE_CD2430_4AS */
	1,				/* PM_HARDWARE_CD2430_8AS */
	4,				/* PA_HARDWARE_T1_1PORT_CSU */
	4,				/* PA_HARDWARE_T1_2PORT_CSU */
	4,				/* PA_HARDWARE_T1_1PORT */
	8,				/* PA_HARDWARE_T1_2PORT */
	5,				/* PA_HARDWARE_E1_1PORT */
	10,				/* PA_HARDWARE_E1_2PORT */
	155,				/* PA_HARDWARE_ATM_OC3_SM */
	155,				/* PA_HARDWARE_ATM_OC3_UTP5 */
	32,				/* PA_HARDWARE_BUS_AND_TAG */
	50,				/* PA_HARDWARE_ESCON */
	160,				/* PA_HARDWARE_16E */
	200,				/* PA_HARDWARE_FMM_FD */
	200,				/* PA_HARDWARE_FSM_FD */
	0,				/* PA_HARDWARE_UNKNOWN_51 */
	0,				/* PA_HARDWARE_UNKNOWN_52 */
	0,				/* PA_HARDWARE_C7100_JUNIOR */
	0,				/* PA_HARDWARE_C7100_IO */
	0				/* PA_NUM_HARDWARE_TYPES */
};
	
/*
 * pas_controller_find_removed :
 * Walk the removed controller queue looking for an exact match
 * with the passed in parameters
 */
static cdbtype *pas_controller_find_removed (int pa_bay, 
					     int ctrlr_type,
					     enum pif_types_ pif)
{
    cdbtype *cdb;

    for (cdb = (cdbtype *)removed_cdbQ->qhead; cdb != NULL; cdb = cdb->next) {
	if (cdb->slot == pa_bay &&
	    cdb->ctrlr_type == ctrlr_type &&
	    cdb->pif_type == pif)
	    return(cdb);
    }
    return(NULL);
}

/*
 * pas_controller_reinsert :
 * A port adaptor that had been previously removed has been 
 * re-inserted into the sam port adaptor bay
 */
static void pas_controller_reinsert (cdbtype *cdb)
{
    cdb->cdb_status &= ~(CDB_NOHARDWARE | CDB_CRASHED);
    cdb->input_throttled = FALSE;
    unqueue(removed_cdbQ, cdb);
    enqueue(cdbQ, cdb);
    cdb_list[controller_index_inline(cdb->slot, cdb->appl_no,
				     max_ctrlr_interfaces)] = cdb;
}

/*
 * pas_controller_create :
 * Port Adaptor based systems controller initialization routine
 */
cdbtype *pas_controller_create (int ctrlr_type,
				enum pif_types_	pif,
				int pa_bay,
				int pa_device,
				platform_hware_handle_type *hware_handle)
{
    cdbtype *cdb;

    /*
     * First question, is there already a cdb out there that matches
     * us ? Typically means that some card was on-line installed
     */
    if ((cdb = controller_find(pa_bay, pa_device)) != NULL) {

	/*
	 * Found one. Make sure it is of the same type
	 */
	if (cdb->ctrlr_type == ctrlr_type && cdb->pif_type == pif) {
	    cdb->platform_hware_handle = hware_handle;
	    return(cdb);
	} else {
	    /*
	     * This should never happen. Infer that it was caused by very
	     * rapid install/remove of a port adaptor.
	     */
	    controller_remove(cdb);
	}
    }

    /*
     * Second question, is there a cdb out there with no hardware attached ?
     */
    if ((cdb = pas_controller_find_removed(pa_bay, ctrlr_type, pif)) != NULL) {
	pas_controller_reinsert(cdb);
	cdb->platform_hware_handle = hware_handle;
	return(cdb);
    }

    /*
     * Got to allocate one
     */
    cdb = malloc_named(sizeof(cdbtype), "Controller Info");
    if (cdb == NULL) {
	return(NULL);
    }
    
    cdb->platform_hware_handle = hware_handle;
    cdb->ctrlr_type = ctrlr_type;
    cdb->pif_type = pif;
    cdb->unit = (pa_bay * MAX_INTERFACES_PER_PA) + pa_device;
    cdb->slot = pa_bay;
    cdb->appl_no = pa_device;
    cdb->cdb_name = pif_name(pif);
    cdb->cdb_status = 0;	/* PRI line depends on the CDB_CONFIGURE bit
				   being off as the default */
    init_cdb_name(cdb);
    controller_init_common(cdb);
    cdb_list[controller_index_inline(cdb->slot, cdb->appl_no,
				     max_ctrlr_interfaces)] = cdb; 
    return(cdb);
}

/* 
 * pas_display_eeprom_contents()
 *	Dump the contents of the PA EEPROM in hexadecimal.
 */
static void pas_display_eeprom_contents (dgbus_eeprom_t *eptr)
{
    uchar *ep = (uchar *)eptr;

    printf("\n\tEEPROM format version %d", eptr->eeprom_version);
    printf("\n\tEEPROM contents (hex):");
    printf("\n\t  0x20: %02x %02x %02x %02x %02x %02x %02x %02x "
       "%02x %02x %02x %02x %02x %02x %02x %02x",
       ep[0], ep[1], ep[2], ep[3], ep[4], ep[5], ep[6], ep[7],
       ep[8], ep[9], ep[10], ep[11], ep[12], ep[13], ep[14], ep[15]);
    printf("\n\t  0x30: %02x %02x %02x %02x %02x %02x %02x %02x "
       "%02x %02x %02x %02x %02x %02x %02x %02x",
       ep[16], ep[17], ep[18], ep[19], ep[20], ep[21], ep[22], ep[23],
       ep[24], ep[25], ep[26], ep[27], ep[28], ep[29], ep[30], ep[31]);
} 

/*
 * pas_display_pa_info()
 *	Print some descriptive information about the port adapter.
 */
static void pas_display_pa_info (slottype *slotptr)
{
    int pa_type = slotptr->pa_type;
    int flags = slotptr->flags;
    int max_desc_strings;

    max_desc_strings = sizeof(pa_hw_desc_strings) / sizeof(uchar *);
    if ((pa_type >= 0) && (pa_type < max_desc_strings)) {
        printf("\n\t%s port adapter, %d port%s",
               pa_hw_desc_strings[pa_type], slotptr->interface_count,
               Plural(slotptr->interface_count, "", "s"));
    } else {
        printf("\nUNKNOWN port adapter");
    }

    if (flags) {
	printf("\n\tPort adapter is %s%s%s%s%s",
	       (flags & BOARD_DISABLED)   ? "disabled "   : "",
	       (flags & BOARD_ANALYZED)   ? "analyzed "   : "",
	       (flags & BOARD_WEDGED)     ? "wedged "     : "",
	       (flags & BOARD_UNSUITABLE) ? "unsuitable " : "",
	       (flags & BOARD_INVALID)    ? "invalid "    : "");
    }

    printf("\n\tPort adapter insertion time ");
    if (TIMER_RUNNING(slotptr->insert_time)) {
        printf("%TE ago", slotptr->insert_time);
    } else {
        printf("unknown");
    }
}

/*
 * pas_display_pa_hw_info()
 *	Display information about the port adapter hardware like
 * the version, serial number etc.
 */
static void pas_display_pa_hw_info (dgbus_eeprom_t *eptr)  
{
    uchar hwver_hi, hwver_lo, revision;
    ulong serial_number;
    uint part_number_pt1, part_number_pt2, part_number_pt3;
    uint test_history, rma_number_pt1, rma_number_pt2, rma_number_pt3;

    /*
     * Parse the data programmed into the EEPROM.
     */
    hwver_hi = eptr->hardware_version_high;
    hwver_lo = eptr->hardware_version_low;
    revision = eptr->board_revision;
    part_number_pt1 = eptr->part_number_31_24;
    part_number_pt2 = eptr->part_number_23_16 << 8 |
                      eptr->part_number_15_8;
    part_number_pt3 = eptr->part_number_7_0;
    test_history = eptr->test_history;
    rma_number_pt1 = eptr->rma_number_23_16;
    rma_number_pt2 = eptr->rma_number_15_8;
    rma_number_pt3 = eptr->rma_number_7_0;
    serial_number = eptr->serial_number_31_24 << 24 |
                    eptr->serial_number_23_16 << 16 |
                    eptr->serial_number_15_8 << 8   |
                    eptr->serial_number_7_0;
    /*
     * Print the gathered information.
     */
    printf("\n\tHardware revision %d.%10d\t", hwver_hi, hwver_lo);
    if (revision < 0x50 || revision == 0xff) {
        printf("Board revision UNKNOWN");
    } else {
        printf("Board revision %c%d", 'A' + ((revision - 0x50) / 8),
                                                      revision & 0x7);
    }
    printf("\n\tSerial number     %10lu\tPart number    %02u-%04u-%02u",
      serial_number, part_number_pt1, part_number_pt2, part_number_pt3);
    printf("\n\tTest history      0x%10x\tRMA number     %02d-%02d-%02d",
      test_history, rma_number_pt1, rma_number_pt2, rma_number_pt3);
}

/*
 * show_pas_diag()
 * 	Callback function for the "show diag" command for platforms
 * with port adapters/modules.
 */
void pas_show_diag (parseinfo *csb)
{
    int pa_bay, start_bay, bay;
    dgbus_eeprom_t *eptr;
    slottype *slotptr;
    boolean all_bays;

    /*
     * Allocate memory to hold the contents of the EEPROM on the
     * port adapter.
     */
    eptr = malloc(sizeof(dgbus_eeprom_t));
    if (!eptr) {
        printf("%%Error: No memory\n");
        return;
    }

    bay = GETOBJ(int, 1);
    /*
     * If no argument is provided to the "show diag" command, bay will
     * be -1 indicating that disgnostic information for all bays on the
     * system is to be output. If an argument is provided, information
     * for only the specified bay will be displayed. 
     */
    if (bay == -1) {
        start_bay = 0;
        all_bays = TRUE;
    }
    else {
        start_bay = bay;
        all_bays = FALSE;
    }

    automore_enable(NULL);

    /*
     * Now print out the diagnostic information.
     */
    for (pa_bay = start_bay; pa_bay < MAX_PA_BAYS; pa_bay++) {

        pas_get_eeprom_data(pa_bay, eptr, 16);

        if (pas_pa_present(pa_bay)) {

            slotptr = &slots[pa_bay];

            printf("\nSlot %d:", pa_bay);

            pas_display_pa_info(slotptr);

            pas_display_pa_hw_info(eptr);

            pas_display_eeprom_contents(eptr);

            printf("\n");
        }

        if (!all_bays)
            break;
    }
    automore_disable();

    free(eptr);
}

/*
 * pas_bandwidth_points:
 * Return the bandwidth points for a port adaptor
 */
ushort pas_bandwidth_points (int pa_type)
{
    int max_index;

    max_index = sizeof(pa_bandwidth_array) / sizeof(ushort);
    if ((pa_type >= 0) && (pa_type < max_index)) {
	return(pa_bandwidth_array[pa_type]);
    } else {
	return(0);
    }
}

/* end of file */

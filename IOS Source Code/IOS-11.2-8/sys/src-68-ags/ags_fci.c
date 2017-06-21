/* $Id: ags_fci.c,v 3.3 1996/01/25 11:21:47 smackie Exp $
 * $Source: /vws/aam/cherf.111/ios/sys/src-68-ags/ags_fci.c,v $
 *------------------------------------------------------------------
 * AGS+ specific FCI functions.
 *
 * June 1994, Walter R. Friedrich
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ags_fci.c,v $
 * Revision 3.3  1996/01/25  11:21:47  smackie
 * Registry code gets some much-needed hot-oil massaging, free liposuction
 * and a makeover.  (CSCdi47101)
 *
 *    o Compress registry structures. Saves over 120K for all platforms
 *    o Add registry subsystems and remove feature registry initializion
 *      from registry.c to fix woeful initialization scaling problems
 *    o Remove unused and seemingly unloved registry debugging code
 *    o Add registry memory statistics to shame people into action
 *
 * Revision 3.2  1995/11/17  18:42:30  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:24:31  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:54:01  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "interface_private.h"
#include "sys_registry.h"
#include "fastswitch_registry.h"
#include "../hes/if_mci.h"
#include "../hes/if_fci.h"
#include "../hes/cbus_registry.h"
#include "../hes/cbus_registry.regc"


/* 
 * cbus_hard_reset
 */

static void cbus_hard_reset (mcitype* cbus) 
{
    mciregtype* regptr;
    regptr = cbus->regptr;

    regptr->argreg = FCI_HARD_RESET;
    regptr->cmdreg = FCI_CMD_RESET;
    if (cbus->flags & CBUSII)
	DELAY(CBUSII_HARD_RESET_TIME);
    else
	DELAY(FCI_HARD_RESET_TIME);
}

/* 
 * cbus_soft_reset
 */

void cbus_soft_reset (mcitype* cbus) 
{
    mciregtype* regptr;
    regptr = cbus->regptr;

    regptr->argreg = FCI_SOFT_RESET;
    regptr->cmdreg = FCI_CMD_RESET;
    DELAY(FCI_SOFT_RESET_TIME);
}


/*
 * cbus_controller_type
 */

static ushort cbus_controller_type (mciregtype* regptr)
{
    regptr->cmdreg = FCI_CMD_CTRLR_WHO;
    return regptr->argreg;
}


/*
 * ags_cbus_card_present
 * Return TRUE if this could be the "slot" number of a card in 
 * the cbus complex.  This routine isn't particularly interesting
 * on anything but a 7000-style platform.
 */
static boolean ags_cbus_card_present (uint slot) 
{
    if (slot < FCI_MAX_DCI_CTRLRS)
	return(TRUE);
    else
	return(FALSE);
}

/*
 * ags_get_hw_version
 * Assumptions: called with interrupts disabled
 */
static ushort ags_get_hw_version (void* bus, uint indx)
{
    mciregtype* regptr = (mciregtype*)bus;

    if (mci_controller(regptr)) { /* mci controllers handled directly */
	regptr->argreg = FCI_HW_VERSION;
	regptr->cmdreg = FCI_CMD_VERSION;
	return regptr->argreg;
    } else {
	regptr->argreg = indx;	/* select controller and query via cbus */
	regptr->cmdreg = FCI_CMD_CTRLR_SELECT;
	regptr->argreg = FCI_HW_VERSION;
	regptr->cmdreg = FCI_CTRLR_ARGUMENT;
	regptr->argreg = FCI_CTRLR_VERSION;
	regptr->cmdreg = FCI_CTRLR_CMD;
	return regptr->argreg;
    }
}

/*
 * ags_print_cbus_controller
 * print out show cbus header string for cbus controller
 */ 
static void ags_print_cbus_controller (mcitype* cbus)
{
    printf("\ncBus %d, controller type %d.%d, microcode version %d.%d",
	   cbus->unit, (cbus->hw_version >> 8), (cbus->hw_version & 0xFF),
	   (cbus->sw_version >> 8), (cbus->sw_version & 0xFF));
}

void cbus_platform_registry_init (void)
{
    reg_add_cbus_hard_reset(cbus_hard_reset, "cbus_hard_reset");
    reg_add_cbus_soft_reset(cbus_soft_reset, "cbus_soft_reset");
    reg_add_assign_mac_addr((service_assign_mac_addr_type)return_false,
			    "return_false");
    reg_add_fddi_ringstate_leds((service_fddi_ringstate_leds_type)
				return_nothing,
				"return_nothing");

    reg_add_disable_slot((service_disable_slot_type)return_nothing,
			 "return_nothing");
    reg_add_flush_dci((service_flush_dci_type)return_nothing,
		      "return_nothing");
    reg_add_slot_enabled((service_slot_enabled_type)return_false,
			 "return_false");
    reg_add_switch_proc_present((service_switch_proc_present_type)return_false,
				"return_false");
    reg_add_cxbus_card_present(ags_cbus_card_present, "ags_cbus_card_present");

    reg_add_slot_enable_led_on((service_slot_enable_led_on_type)return_nothing,
			       "return_nothing");
    reg_add_switch_proc_present((service_switch_proc_present_type)return_false,
				"return_false");
    reg_add_get_controller_type((service_get_controller_type_type)
				cbus_controller_type, "cbus_controller_type");
    reg_add_get_hw_version(ags_get_hw_version, "ags_get_hw_version");

    fci_max_dci_ctrlrs = FCI_MAX_DCI_CTRLRS;

    reg_add_print_cbus_controller(ags_print_cbus_controller,
				  "ags_print_cbus_controller");
}

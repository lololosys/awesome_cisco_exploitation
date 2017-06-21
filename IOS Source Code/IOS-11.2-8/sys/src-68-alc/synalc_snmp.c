/* $Id: synalc_snmp.c,v 3.1.64.2 1996/05/21 21:39:49 mordock Exp $
 * $Source: /release/112/cvs/Xsys/src-68-alc/synalc_snmp.c,v $
 *------------------------------------------------------------------
 * synalc_snmp.c -- SNMP routines for Synergy ATM line card
 *			(based on c3000_snmp.c)
 *
 * August 31, 1995, Rick Lowe
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: synalc_snmp.c,v $
 * Revision 3.1.64.2  1996/05/21  21:39:49  mordock
 * CSCdi58327:  snmp modularity commit broke some builds
 * Branch: California_branch
 * need to use new chassismib registry
 *
 * Revision 3.1.64.1  1996/05/09  14:41:15  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.1.92.1  1996/04/27  07:25:32  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.1.82.2  1996/04/10  06:40:50  rlowe
 * Fix up ifdefs; clean-out old, obsolete code.
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.1.82.1  1996/04/08  02:09:03  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.1.70.1  1996/03/22  09:43:48  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.1.54.1  1996/03/03  21:25:00  rlowe
 * Apply SYNALC content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.1.52.1  1996/03/02  01:21:15  rlowe
 * Non-sync sync to mainline tag V111_1_0_3.
 * Branch: Synalc3_Cal_V111_1_0_3_branch
 *
 * Revision 3.1.36.1  1996/02/29  07:50:21  rlowe
 * First pass non-sync sync for Synergy ATM line card to V111_0_16.
 * Branch: Synalc3_Cal_V111_0_16_branch
 *
 * Revision 3.1  1995/11/09  13:24:57  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1.32.1  1996/02/03  07:08:38  rlowe
 * Non-sync sync of Synalc_catchup_branch to V111_0_5.
 * Branch: Synalc_catchup_pretal_branch
 *
 * Revision 2.1.2.1  1995/08/31  14:18:42  rlowe
 * Carryover useful code from old Synergy_ATMlc_branch (10.3) to new
 * Synalc2_branch (11.0).  Provide platform-dependent code for IOS
 * re-port to Synergy ATM line card.
 *
 * Branch: Synalc2_branch
 *
 * Revision 2.1  1995/08/31  14:12:51  rlowe
 * Placeholders for Synalc2_branch.
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../../boot/cpu.h"
#include "subsys.h"
#include "../les/if_les.h"
#include "../snmp/snmp_api.h"
#include "../snmp/snmp_platform_api.h"
#include "../snmp/chassismib_registry.h"
#include "../h/interface_private.h"

/*
 * generate all possible sysObjectID values for this platform
 */
#define SNMPPART_DECL
#define	N_workgroup

#include "../snmp/sr_lsmi.h"
#undef SNMPPART_DECL

static uint chassis_type;
static uint processor_type;
static uint chassis_slots;
static OID  system_id;
static boolean api_initialized = FALSE;


/*
 * Returns snmp chassisType for the platform
 */
static uint
get_snmp_chassis_type (void)
{
    return (D_chassisType_unknown);
}


/*
 * Returns snmp cardtype for onboard processor card
 */
static uint
get_snmp_processor_type (void)
{
    return (D_cardType_unknown);
}

/*
 * Returns snmp sysObjectID for the platform
 */
static void
get_snmp_system_id (OID *oid)
{
    switch (chassis_type) {
    default:
        oid->oid_ptr = (unsigned long *) IDworkgroup;
        oid->length = LNworkgroup;
        break;
    }
}

static void
snmp_platform_api_initialize (void)
{
    chassis_type = get_snmp_chassis_type();
    chassis_slots = 0;
    processor_type = get_snmp_processor_type();
    get_snmp_system_id(&system_id);
    api_initialized = TRUE;
}


OID *
snmp_platform_get_oid (snmp_platform_oid_type selector)
{
    if (!api_initialized)
	snmp_platform_api_initialize();

    switch (selector) {
    case SNMP_PLATFORM_OID_SYS_OBJECT_ID:
	return &system_id;
    default:
	return NULL;
    }
}


uint
snmp_platform_get_value (snmp_platform_value_type selector)
{
    if (!api_initialized)
	snmp_platform_api_initialize();

    switch (selector) {
    case SNMP_PLATFORM_CHASSIS_TYPE:
	return chassis_type;
    case SNMP_PLATFORM_PROCESSOR_TYPE:
	return processor_type;
    case SNMP_PLATFORM_SLOTS:
	return chassis_slots;
    default:
	return -1;
    }
}

int
snmp_platform_get_card_value(uint cardIndex,
                                snmp_platform_card_value_type selector)
{
    if (!api_initialized)
        snmp_platform_api_initialize();
 
    switch (selector) {
    case SNMP_PLATFORM_CARD_OPER_STATUS:
        return(D_cardOperStatus_not_specified);
    default:
        return -1;
    }
}
 
int
snmp_platform_get_if_value(hwidbtype *hwidb,
                                snmp_platform_if_value_type selector)
{
    if (!api_initialized)
        snmp_platform_api_initialize();
 
    if (hwidb == NULL)
        return(-1);
 
    switch (selector) {
    case SNMP_PLATFORM_IF_CARD_INDEX:
        return(reg_invoke_chassismib_find_slot_card_index(hwidb->slot));
    case SNMP_PLATFORM_IF_SLOT_NUMBER:
        return(hwidb->slot);
    case SNMP_PLATFORM_IF_PORT_NUMBER:
        return(hwidb->slotunit);
    case SNMP_PLATFORM_IF_CONNECTOR_TYPE_ENABLED:
        return(D_cardIfConnectorTypeEnabled_not_specified);
    default:
        return(-1);
    }
}


/*
 * ALC_snmp_init
 *
 * Initialize ATM line card SNMP support.
 *
 */
 
void ALC_snmp_init (subsystype *subsys)
{
    if (!api_initialized)
	snmp_platform_api_initialize();

    reg_invoke_chassismib_add_card_entry(processor_type, NO_HOTSWAP,
			platform_get_string(PLATFORM_STRING_NOM_DU_JOUR),
			platform_get_value(PLATFORM_VALUE_HARDWARE_SERIAL),
			platform_get_string(PLATFORM_STRING_HARDWARE_REVISION),
			NO_SWVERSION, NO_SLOTNUMBER);

}

/*
 * synalc_snmp subsystem header
 */
 
#define ALC_SNMP_MAJVERSION    1
#define ALC_SNMP_MINVERSION    0
#define ALC_SNMP_EDITVERSION   1
 
SUBSYS_HEADER(c5kalc_snmp,
	      ALC_SNMP_MAJVERSION, ALC_SNMP_MINVERSION, ALC_SNMP_EDITVERSION,
              ALC_snmp_init, SUBSYS_CLASS_MANAGEMENT, NULL, NULL);
      

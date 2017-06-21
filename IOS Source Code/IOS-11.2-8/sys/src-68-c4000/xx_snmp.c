/* $Id: xx_snmp.c,v 3.4.46.2 1996/05/21 06:39:52 mordock Exp $
 * $Source: /release/112/cvs/Xsys/src-68-c4000/xx_snmp.c,v $
 *------------------------------------------------------------------
 * xx_snmp.c - SNMP routines for handling 4000/4000M chassis
 *
 * June 1995, Scott Mackie (moved from init_xx.c)
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: xx_snmp.c,v $
 * Revision 3.4.46.2  1996/05/21  06:39:52  mordock
 * Branch: California_branch
 * Improve snmp modularity via creating services to register interfaces
 * and chassis cards.
 * Add syslog mib.
 *
 * Revision 3.4.46.1  1996/03/18  21:59:42  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.14.3  1996/03/14  01:25:13  gstovall
 * Branch: DeadKingOnAThrone_branch
 * The King is now ready for action.
 *
 * Revision 3.4.14.2  1996/03/09  05:12:31  hampton
 * Slim down the kernel of the router.
 * Branch: DeadKingOnAThrone_branch
 *
 * Revision 3.4.14.1  1996/01/24  23:01:41  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.4  1995/12/15  03:15:51  rpratt
 * CSCdi45651:  Chassis MIB enhancements made Port Ready
 * CSCdi42944:  Fixed chassis_slots (C4000_NUM_SLOTS)
 *
 * Revision 3.3  1995/11/24  16:41:40  jjohnson
 * CSCdi42637:  platform-specific code should be moved out of sub_snmp
 * Install an API for accessing platform-specific SNMP objects
 *
 * Revision 3.2  1995/11/17  18:43:57  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:25:38  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:27:12  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  22:55:22  hampton
 * Bump version numbers from 1.x to 2.x.
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
#include "../if/ether.h"
#include "../util/itemlist.h"

/*
 * generate all possible sysObjectID values for this platform
 */
#define SNMPPART_DECL
#define N_cisco4000
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
    switch (cpu_type) {
      case CPU_XX:
	return(D_chassisType_c4000);
	break;
      default:
	return(D_chassisType_unknown);
	break;
    }
}

/*
 * Returns snmp cardtype for onboard processor card
 */
static uint
get_snmp_processor_type (void)
{
    uchar hw_version;
    
    hw_version = platform_get_value(PLATFORM_VALUE_HARDWARE_REVISION);
    switch (cpu_type) {
      case CPU_XX:
	if (hw_version > REVNUM_4000)
	    return (D_cardType_cpu_4000m);
	else
	    return (D_cardType_cpu_4000);
	break;
      default:
	return (D_cardType_unknown);
	break;
    }
}

/*
 * Returns snmp sysObjectID for the platform
 */
static void
get_snmp_system_id (OID *oid)
{
    oid->oid_ptr = (unsigned long *) IDcisco4000;
    oid->length = LNcisco4000;
}

static void
snmp_platform_api_initialize (void)
{
    chassis_type = get_snmp_chassis_type();
    chassis_slots = C4000_NUM_SLOTS;
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
    ONE_ITEM_LIST(tmp, ETHER_HW_MEDIA_TYPE, sizeof(ulong), 0);

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
        return(hwidb->subunit);
    case SNMP_PLATFORM_IF_CONNECTOR_TYPE_ENABLED:
        switch (hwidb->type) {
        case IDBTYPE_LANCE:
	    /*
	     * Get the media type
	     */
	    (*hwidb->state_config)(hwidb, IDB_CONTROL_READ_CONFIG, &tmp_list);

	    switch (tmp_item.u.l_value) {
	      case ETHER_MEDIA_TYPE_10BASET:
		return(D_cardIfConnectorTypeEnabled_rj_45);
	      case ETHER_MEDIA_TYPE_AUI:
		return(D_cardIfConnectorTypeEnabled_db_15);
	      default:
		return(D_cardIfConnectorTypeEnabled_none);
	    }
        default:
            return(D_cardIfConnectorTypeEnabled_not_specified);
        }
    default:
        return(-1);
    }
}

/*
 * c4000_snmp_init
 *
 * Initialize NIM SNMP support.
 *
 */
 
void c4000_snmp_init (subsystype *subsys)
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
 * c4000_snmp subsystem header
 */
 
#define C4000_SNMP_MAJVERSION    1
#define C4000_SNMP_MINVERSION    0
#define C4000_SNMP_EDITVERSION   1
 
SUBSYS_HEADER(c4000_snmp, C4000_SNMP_MAJVERSION, C4000_SNMP_MINVERSION,
              C4000_SNMP_EDITVERSION,
              c4000_snmp_init, SUBSYS_CLASS_MANAGEMENT,
              NULL, NULL);

      

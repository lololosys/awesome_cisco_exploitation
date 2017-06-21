/* $Id: c1000_snmp.c,v 3.5.46.2 1996/05/21 06:39:17 mordock Exp $
 * $Source: /release/112/cvs/Xsys/src-36-c1000/c1000_snmp.c,v $
 *------------------------------------------------------------------
 * c1000_snmp.c - SNMP routines for cisco c1000 platforms
 *
 * February 1995, Scott Mackie (moved from init_c1000.c)
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: c1000_snmp.c,v $
 * Revision 3.5.46.2  1996/05/21  06:39:17  mordock
 * Branch: California_branch
 * Improve snmp modularity via creating services to register interfaces
 * and chassis cards.
 * Add syslog mib.
 *
 * Revision 3.5.46.1  1996/03/18  21:57:19  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.5.14.2  1996/03/09  05:11:28  hampton
 * Slim down the kernel of the router.
 * Branch: DeadKingOnAThrone_branch
 *
 * Revision 3.5.14.1  1996/01/24  22:50:05  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.5  1995/12/15  03:14:40  rpratt
 * CSCdi45651:  Chassis MIB enhancements made Port Ready
 *
 * Revision 3.4  1995/12/03  18:00:08  thille
 * CSCdi44905:  ciscopro targets not in 11.1
 * Put them in.
 *
 * Revision 3.3  1995/11/24  16:41:28  jjohnson
 * CSCdi42637:  platform-specific code should be moved out of sub_snmp
 * Install an API for accessing platform-specific SNMP objects
 *
 * Revision 3.2  1995/11/17  18:40:00  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:19:41  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  21:26:34  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/09/22  04:36:40  enf
 * CSCdi40869:  Add frankenstorm to 11.0
 *
 * Revision 2.1  1995/06/07  22:52:24  hampton
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
#include "interface_private.h"

/*
 * generate all possible sysObjectID values for this platform
 */
#define SNMPPART_DECL
#define N_cisco1003
#define N_cisco1004
#define N_cisco1005
#define N_ciscoPro1003
#define N_ciscoPro1004
#define N_ciscoPro1005
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
      case CPU_SAPPHIRE:
	switch (cookie->interfaces) {
	  case INTERFACE_1E1B:	/* Sapphire */
	    return D_chassisType_c1003;
	    
	  case INTERFACE_1E1N:	/* Frankenstorm */
	    return D_chassisType_c1004;
	    
	  case INTERFACE_1E1T:	/* Catwoman */
	    return D_chassisType_c1005;
	    
	  default:                                /* unknown cpu_type */
	    return D_chassisType_unknown;
	    break;
	}
	break;

      default:
	return D_chassisType_unknown;
	break;
    }
}

/*
 * Returns snmp cardtype for onboard processor card
 */
static uint
get_snmp_processor_type (void)
{
    switch (cpu_type) {
      case CPU_SAPPHIRE:
	return (D_cardType_cpu_1003);
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
    switch (chassis_type) {
    default:
    case D_chassisType_c1003:
        if (ciscopro_platform) {
            oid->oid_ptr = (unsigned long *) IDciscoPro1003;
            oid->length = LNciscoPro1003;
        } else {
            oid->oid_ptr = (unsigned long *) IDcisco1003;
            oid->length = LNcisco1003;
        }
	break;
    case D_chassisType_c1004:
        if (ciscopro_platform) {
            oid->oid_ptr = (unsigned long *) IDciscoPro1004;
            oid->length = LNciscoPro1004;
        }
        else {
            oid->oid_ptr = (unsigned long *) IDcisco1004;
            oid->length = LNcisco1004;
        }
	break;
    case D_chassisType_c1005:
        if (ciscopro_platform) {
            oid->oid_ptr = (unsigned long *) IDciscoPro1005;
            oid->length = LNciscoPro1005;
        } else {
            oid->oid_ptr = (unsigned long *) IDcisco1005;
            oid->length = LNcisco1005;
        }
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
snmp_platform_get_card_value (uint cardIndex,
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
snmp_platform_get_if_value (hwidbtype *hwidb,
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
 * c1000_snmp_init
 *
 * Initialize C1000 SNMP support.
 *
 */
 
static void c1000_snmp_init (subsystype *subsys)
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
 * c1000_snmp subsystem header
 */
 
#define C1000_SNMP_MAJVERSION    1
#define C1000_SNMP_MINVERSION    0
#define C1000_SNMP_EDITVERSION   1
 
SUBSYS_HEADER(c1000_snmp, C1000_SNMP_MAJVERSION, C1000_SNMP_MINVERSION,
              C1000_SNMP_EDITVERSION,
              c1000_snmp_init, SUBSYS_CLASS_MANAGEMENT,
              NULL, NULL);

      

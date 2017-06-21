/* $Id: ags_snmp.c,v 3.6.46.2 1996/05/21 06:39:41 mordock Exp $
 * $Source: /release/112/cvs/Xsys/src-68-ags/ags_snmp.c,v $
 *------------------------------------------------------------------
 * ags_snmp.c - Miscellaneous SNMP defintions for AGS chassis
 *
 * March 1995, Scott Mackie (moved from init_68k.c)
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ags_snmp.c,v $
 * Revision 3.6.46.2  1996/05/21  06:39:41  mordock
 * Branch: California_branch
 * Improve snmp modularity via creating services to register interfaces
 * and chassis cards.
 * Add syslog mib.
 *
 * Revision 3.6.46.1  1996/03/18  21:58:51  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.6.14.2  1996/03/09  05:12:06  hampton
 * Slim down the kernel of the router.
 * Branch: DeadKingOnAThrone_branch
 *
 * Revision 3.6.14.1  1996/02/20  17:54:34  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.6  1995/12/15  03:15:34  rpratt
 * CSCdi45651:  Chassis MIB enhancements made Port Ready
 *
 * Revision 3.5  1995/12/10  21:02:31  smackie
 * Next on a very special Geraldo. Unused global booleans and the files
 * that loved them. (CSCdi45340)
 *
 * Revision 3.4  1995/11/24  16:41:35  jjohnson
 * CSCdi42637:  platform-specific code should be moved out of sub_snmp
 * Install an API for accessing platform-specific SNMP objects
 *
 * Revision 3.3  1995/11/17  18:42:32  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:30:38  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:24:32  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:54:02  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../../boot/cpu.h"
#include "subsys.h"
#include "../snmp/snmp_api.h"
#include "../snmp/snmp_platform_api.h"
#include "../snmp/chassismib_registry.h"
#include "../h/interface_private.h"

/*
 * generate all possible sysObjectID values for this platform
 */
#define SNMPPART_DECL
#define N_ciscoGatewayServer
#define N_ciscoTerminalServer
#define N_ciscoTrouter
#define N_ciscoProtocolTranslator
#define N_ciscoAGSplus
#include "../snmp/sr_lsmi.h"
#undef SNMPPART_DECL

static uint chassis_type;
static uint processor_type;
static uint chassis_slots;
static OID  system_id;
static boolean api_initialized = FALSE;

#define CPUSLOT_AGSPLUS	2

/*
 * External declarations
 */
extern int ncBus;

/*
 * initialize snmp platform api
 */
static void
snmp_platform_api_initialize (void)
{
    switch (cpu_type) {
      case CPU_CSC1:
	processor_type = D_cardType_csc1;
	break;

      case CPU_CSC2:
	processor_type = D_cardType_csc2;
	break;

      case CPU_CSC3:
	processor_type = D_cardType_csc3;
	break;

      case CPU_CSC4:
	processor_type = D_cardType_csc4;
	break;

      default:
	processor_type = D_cardType_unknown;
	break;
    }

    if (ncBus) {
	chassis_slots = 9;
	chassis_type = D_chassisType_agsplus;
	system_id.oid_ptr = (unsigned long *) IDciscoAGSplus;
	system_id.length = LNciscoAGSplus;
    } else {
	chassis_slots = -1;
	chassis_type = D_chassisType_multibus;
        if (router_enable) {
	    system_id.oid_ptr = (unsigned long *) IDciscoGatewayServer;
	    system_id.length = LNciscoGatewayServer;
        } else if (protocolconversion) {
            system_id.oid_ptr = (unsigned long *) IDciscoProtocolTranslator;
            system_id.length = LNciscoProtocolTranslator;
        } else {
            system_id.oid_ptr = (unsigned long *) IDciscoTerminalServer;
            system_id.length = LNciscoTerminalServer;
        }
    }

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
 * ags_snmp_init
 *
 * Initialize AGS SNMP support.
 *
 */
static void
ags_snmp_init (subsystype *subsys)
{
    char swver[12];
    char* card_descr;

    if (!api_initialized)
        snmp_platform_api_initialize();

    sprintf(swver, "%u.%u", SYS_MAJORVERSION, SYS_MINORVERSION);
    switch (cpu_type) {
      case CPU_CSC1:
	card_descr = "10MHz 68000";
	break;

      case CPU_CSC2:
	card_descr = "12MHz 68020";
	break;

      case CPU_CSC3:
	card_descr = "30MHz 68020";
	break;

      case CPU_CSC4:
	card_descr = "25MHz 68040";
	break;

      default:
	card_descr = "Unknown processor";
	break;
    }
    reg_invoke_chassismib_add_card_entry(processor_type, NO_HOTSWAP,
					 card_descr, NO_SERIALNUM,
					 NO_HWVERSION, swver,
					 (ncBus) ? 
					 CPUSLOT_AGSPLUS : NO_SLOTNUMBER);

}

/*
 * ags_snmp subsystem header
 */
 
#define AGS_SNMP_MAJVERSION    1
#define AGS_SNMP_MINVERSION    0
#define AGS_SNMP_EDITVERSION   1
 
SUBSYS_HEADER(ags_snmp, AGS_SNMP_MAJVERSION, AGS_SNMP_MINVERSION,
              AGS_SNMP_EDITVERSION,
              ags_snmp_init, SUBSYS_CLASS_MANAGEMENT,
              NULL, NULL);
      

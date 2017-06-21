/* $Id: unix_snmp.c,v 3.5.46.1 1996/05/21 06:42:23 mordock Exp $
 * $Source: /release/112/cvs/Xsys/unix/unix_snmp.c,v $
 *------------------------------------------------------------------
 * unix_snmp.c - SNMP routines for handling Unix images
 *
 * March 1995, Scott Mackie (moved from unix_init.c)
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: unix_snmp.c,v $
 * Revision 3.5.46.1  1996/05/21  06:42:23  mordock
 * Branch: California_branch
 * Improve snmp modularity via creating services to register interfaces
 * and chassis cards.
 * Add syslog mib.
 *
 * Revision 3.5  1995/12/15  03:16:15  rpratt
 * CSCdi45651:  Chassis MIB enhancements made Port Ready
 *
 * Revision 3.4  1995/12/10  21:02:47  smackie
 * Next on a very special Geraldo. Unused global booleans and the files
 * that loved them. (CSCdi45340)
 *
 * Revision 3.3  1995/11/24  16:42:08  jjohnson
 * CSCdi42637:  platform-specific code should be moved out of sub_snmp
 * Install an API for accessing platform-specific SNMP objects
 *
 * Revision 3.2  1995/11/17  17:55:48  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:42:45  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:12:25  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
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
#include "../snmp/sr_lsmi.h"
#undef SNMPPART_DECL

static uint chassis_type;
static uint processor_type;
static uint chassis_slots;
static OID  system_id;
static boolean api_initialized = FALSE;


static void
snmp_platform_api_initialize (void)
{
    chassis_type = D_chassisType_unknown;
    processor_type = D_cardType_unknown;
    chassis_slots = -1;

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
 * unix_snmp_init
 *
 * Initialize unix platform SNMP support.
 *
 */
static void
unix_snmp_init (subsystype *subsys)
{
    if (!api_initialized)
        snmp_platform_api_initialize();

    reg_invoke_chassismib_add_card_entry(processor_type, NO_HOTSWAP, 
                        platform_get_string(PLATFORM_STRING_NOM_DU_JOUR),
			NO_SERIALNUM,
			NO_HWVERSION, NO_SWVERSION, NO_SLOTNUMBER);
}


/*
 * unix_snmp subsystem header
 */
 
#define UNIX_SNMP_MAJVERSION    1
#define UNIX_SNMP_MINVERSION    0
#define UNIX_SNMP_EDITVERSION   1
 
SUBSYS_HEADER(unix_snmp, UNIX_SNMP_MAJVERSION, UNIX_SNMP_MINVERSION,
              UNIX_SNMP_EDITVERSION,
              unix_snmp_init, SUBSYS_CLASS_MANAGEMENT,
              NULL, NULL);

      

/* $Id: as5200_snmp.c,v 1.1.70.1 1996/06/16 21:19:39 vnguyen Exp $
 * $Source: /release/112/cvs/Xsys/src-68-as5200/as5200_snmp.c,v $
 *------------------------------------------------------------------
 * as5200_snmp.c - SNMP routines for AS5200 platform.
 *
 * October 1995, Joe Turner (based on src-68-c3000/c3000_snmp.c)
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: as5200_snmp.c,v $
 * Revision 1.1.70.1  1996/06/16  21:19:39  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 1.1  1995/12/03  21:25:14  jturner
 * Placeholder for Brasil.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#include "master.h"
#include "config.h"
#include "packet.h"
#include "parser.h"
#include "sys_registry.h"
#include "../h/cs_registry.regh"
#include "ttysrv.h"
#include "logger.h"
#include "interface_private.h"
#include "../os/async.h"
#include "../ts/if_async.h"
#include "../ui/debug.h"
#include "../les/modem_debug.h"
#include "../dev/flash_defines.h"
#include "../les/if_as5200_tdm.h"
#include "../les/if_as5200_modem.h"
#include "../les/modem_mgmt.h"
#include "../les/parser_defs_modem.h"
#include "../les/csm_registry.h"

#include "subsys.h"
#include "as5200.h"
#include "../les/if_les.h"
#include "../snmp/snmp_api.h"
#include "../snmp/snmp_platform_api.h"
#include "../snmp/chassismib_registry.h"
#include "../h/interface_private.h"
#include "../if/ether.h"

#include "../if/if_controller.h"
#include "../les/les_controller.h"
#include "../if/dsx1.h"
#include "../les/if_as5200_mcn.h"
#include "../les/if_as5200_modem.h"
#include "../les/if_c4000_m32.h"
#include "../les/if_c4000_dsx1.h"
#include "../les/if_as5200_dsx1_framer.h"
#include "../les/if_as5200_dsx1.h"
#include "../les/if_as5200_t1.h"

/*
 * generate all possible sysObjectID values for this platform
 */
#define SNMPPART_DECL
#define N_ciscoAS5200
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
static uint get_snmp_chassis_type (void)
{
    return (D_chassisType_as5200);
}

/*
 * Returns snmp cardtype for onboard processor card (mother board)
 */
static uint get_snmp_processor_type (void)
{
    return (D_cardType_cpu_as5200);
}

/*
 * Returns snmp sysObjectID for the platform
 */
static void get_snmp_system_id (OID *oid)
{
    oid->oid_ptr = (unsigned long *) IDciscoAS5200;
    oid->length = LNciscoAS5200;
}

static void snmp_platform_api_initialize (void)
{
    chassis_type = get_snmp_chassis_type();
    chassis_slots = AS5200_MAX_SLOTS;
    processor_type = get_snmp_processor_type();
    get_snmp_system_id(&system_id);
    api_initialized = TRUE;
}

OID *snmp_platform_get_oid (snmp_platform_oid_type selector)
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

uint snmp_platform_get_value (snmp_platform_value_type selector)
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

int snmp_platform_get_card_value (uint cardIndex,
                                snmp_platform_card_value_type selector)
{
    card_entry *cep;
    card_entry *carrier_cep;
    long card_index;
    int reg_data;
    short carrier_slot;
 
    if (!api_initialized)
        snmp_platform_api_initialize();
 
    card_index = cardIndex;
    cep = reg_invoke_chassismib_get_cep(EXACT,&card_index);
    if (cep == NULL)
        return(-1);
 
    switch (selector) {
    case SNMP_PLATFORM_CARD_OPER_STATUS:
        switch (cep->card_type) {
        case D_cardType_pm_as5xxx_12m:
           /* to find out the slot number of carrier card */
           card_index = cep->card_contained_by_index;
           carrier_cep = reg_invoke_chassismib_get_cep(EXACT,&card_index);
           if (carrier_cep == NULL)
              return(-1);
           carrier_slot = carrier_cep->card_slot_number;

           if (cep->card_slot_number == 0)	
              /* it's modem module 0 */
              if ((modem_pool->modem_slot[carrier_slot]->modem_reg->mdb_control1) &  
                  MGMT_MODEM_MODULE_0_OK)
                 return(D_cardOperStatus_up); 
              else 
                 return (D_cardOperStatus_down);
           else if (cep->card_slot_number == 1)      
              /* it's modem module 1 */
              if ((modem_pool->modem_slot[carrier_slot]->modem_reg->mdb_control1) & 
                  MGMT_MODEM_MODULE_1_OK)
                 return(D_cardOperStatus_up); 
              else  
                 return(D_cardOperStatus_down); 
           else
              /* now, do not supporrt more than 2 modules*/
              return(D_cardOperStatus_not_specified);
        case D_cardType_as5200_2ct1:
        case D_cardType_as5200_2ce1:
           reg_data = t1_rd_neat_mem(cep->card_slot_number, NEAT_LEDS);
           if (reg_data & BOARD_OK)
              return(D_cardOperStatus_up);
           else
              return(D_cardOperStatus_down);
        case D_cardType_cpu_as5200:
              return(D_cardOperStatus_up);
        default:
            return(D_cardOperStatus_not_specified);
        }
    default:
        return -1;
    }
}

 
int snmp_platform_get_if_value (hwidbtype *hwidb,
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
        return(hwidb->subunit);
    case SNMP_PLATFORM_IF_CONNECTOR_TYPE_ENABLED:
        switch (hwidb->type) {
        case IDBTYPE_LANCE:
            return(D_cardIfConnectorTypeEnabled_db_15);
        default:
            return(D_cardIfConnectorTypeEnabled_not_specified);
        }
    default:
        return(-1);
    }
}

/*
 * as5200_snmp_init
 *
 * Initialize SNMP support.
 *
 */
 
void as5200_snmp_init (subsystype *subsys)
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
 * as5200_snmp subsystem header
 */
 
#define AS5200_SNMP_MAJVERSION    1
#define AS5200_SNMP_MINVERSION    0
#define AS5200_SNMP_EDITVERSION   1
 
SUBSYS_HEADER(as5200_snmp, AS5200_SNMP_MAJVERSION, AS5200_SNMP_MINVERSION,
              AS5200_SNMP_EDITVERSION,
              as5200_snmp_init, SUBSYS_CLASS_MANAGEMENT,
              NULL, NULL);

      

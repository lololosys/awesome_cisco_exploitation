/* $Id: sierra_snmp.c,v 3.7.12.2 1996/05/21 06:39:26 mordock Exp $
 * $Source: /release/112/cvs/Xsys/src-4k-c4000/sierra_snmp.c,v $
 *------------------------------------------------------------------
 * sierra_snmp.c - SNMP routines for handling 4500/4700 chassis
 *
 * June 1995, Scott Mackie (moved from init_sierra.c)
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sierra_snmp.c,v $
 * Revision 3.7.12.2  1996/05/21  06:39:26  mordock
 * Branch: California_branch
 * Improve snmp modularity via creating services to register interfaces
 * and chassis cards.
 * Add syslog mib.
 *
 * Revision 3.7.12.1  1996/03/18  21:58:28  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.5.12.4  1996/03/16  07:33:01  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.5.12.3  1996/03/14  01:24:34  gstovall
 * Branch: DeadKingOnAThrone_branch
 * The King is now ready for action.
 *
 * Revision 3.5.12.2  1996/03/09  05:11:57  hampton
 * Slim down the kernel of the router.
 * Branch: DeadKingOnAThrone_branch
 *
 * Revision 3.5.12.1  1996/01/24  22:50:44  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.7  1996/02/18  23:00:55  vdukki
 * CSCdi47769:  need to distinguish 4700m model using snmp
 * Modified the code to recognize the 4700m platform using snmp.
 *
 * Revision 3.6  1996/02/18  22:04:09  vdukki
 * CSCdi41637:  need to distinguish between 4700 models via snmp
 * Modified code to return new values for cardType and chassisType mib
 * attributes for 4700s platform.
 *
 * Revision 3.5  1995/12/15  03:15:17  rpratt
 * CSCdi45651:  Chassis MIB enhancements made Port Ready
 * CSCdi42944:  Fixed chassis_slots (C4000_NUM_SLOTS)
 *
 * Revision 3.4  1995/12/03  18:00:14  thille
 * CSCdi44905:  ciscopro targets not in 11.1
 * Put them in.
 *
 * Revision 3.3  1995/11/24  16:41:32  jjohnson
 * CSCdi42637:  platform-specific code should be moved out of sub_snmp
 * Install an API for accessing platform-specific SNMP objects
 *
 * Revision 3.2  1995/11/17  18:41:26  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:23:01  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:26:57  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  22:53:43  hampton
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
#define N_cisco4500
#define N_cisco4700
#define N_ciscoPro4500
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
  if (cpu_type == CPU_SIERRA) {
      if (platform_is_c4700()) {
	  return(D_chassisType_c4700);
      }
      if (platform_is_c4500()) {
	  return(D_chassisType_c4500);
      }
  }
  return(D_chassisType_unknown);
}

/*
 * Returns snmp cardtype for onboard processor card
 */
static uint
get_snmp_processor_type (void)
{
    uchar hw_version;
    
    hw_version = platform_get_value(PLATFORM_VALUE_HARDWARE_REVISION);
    if (cpu_type == CPU_SIERRA) {
	if (platform_is_c4700()) {
          /*
           * Check to see is it a c4700 or a c4700m
           */
          if (hw_version > REVNUM_4700)
              return(D_cardType_cpu_4700m);
          else
              return(D_cardType_cpu_4700);
	}
	if (platform_is_c4500()) {
	    /*
	     * Check to see is it a c4500 or a c4500m
	     */
	    hw_version = platform_get_value(PLATFORM_VALUE_HARDWARE_REVISION);
	    if (hw_version > REVNUM_4500) {
		return(D_cardType_cpu_4500m);
	    } else {
		return(D_cardType_cpu_4500);
	    }
	}
    }
    
    /*
     * No idea what cardType it is. Return unknown.
     */
    return(D_cardType_unknown);
}

/*
 * Returns snmp sysObjectID for the platform
 */
static void
get_snmp_system_id (OID *oid)
{
    switch (chassis_type) {
    case D_chassisType_c4500:
    default:
        if (ciscopro_platform) {
            oid->oid_ptr = (unsigned long *) IDciscoPro4500;
            oid->length = LNciscoPro4500;
        } else {
            oid->oid_ptr = (unsigned long *) IDcisco4500;
            oid->length = LNcisco4500;
        }
        break;
    case D_chassisType_c4700:
    case D_chassisType_c4700S:
        oid->oid_ptr = (unsigned long *) IDcisco4700;
        oid->length = LNcisco4700;
        break;
    }
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
      

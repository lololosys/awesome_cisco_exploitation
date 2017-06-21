/* $Id: c7000_snmp.c,v 3.6.20.5 1996/07/26 01:10:33 feisal Exp $
 * $Source: /release/112/cvs/Xsys/src-68-c7000/c7000_snmp.c,v $
 *------------------------------------------------------------------
 * c7000_snmp.c - Miscellaneous SNMP support for the RP1
 *
 * March 1995, Scott Mackie (moved from init_68k.c)
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: c7000_snmp.c,v $
 * Revision 3.6.20.5  1996/07/26  01:10:33  feisal
 * CSCdi61016:  Need to add CardType MIB definition for different flavors
 * of 1FE PA
 * Branch: California_branch
 *
 * Revision 3.6.20.4  1996/06/09  07:58:45  cheilek
 * CSCdi52427:  VIP1/VIP2 cardType enumeration labels should be vip/vip2
 * Branch: California_branch
 * Change MIBS: rvip to vip; svip to vip2.
 *
 * Revision 3.6.20.3  1996/05/21  06:39:59  mordock
 * Branch: California_branch
 * Improve snmp modularity via creating services to register interfaces
 * and chassis cards.
 * Add syslog mib.
 *
 * Revision 3.6.20.2  1996/05/14  02:57:17  feisal
 * CSCdi56797:  Card type for the 5EFL PA not added properly to the chasis
 * MIB
 * Branch: California_branch
 *
 * Revision 3.6.20.1  1996/03/18  22:00:03  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.5.14.3  1996/03/14  01:25:36  gstovall
 * Branch: DeadKingOnAThrone_branch
 * The King is now ready for action.
 *
 * Revision 3.5.14.2  1996/02/20  17:56:59  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.6  1996/02/01  06:09:07  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.5.14.1  1996/01/24  23:02:11  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.5  1995/12/15  03:15:58  rpratt
 * CSCdi45651:  Chassis MIB enhancements made Port Ready
 *
 * Revision 3.4  1995/11/24  16:41:43  jjohnson
 * CSCdi42637:  platform-specific code should be moved out of sub_snmp
 * Install an API for accessing platform-specific SNMP objects
 *
 * Revision 3.3  1995/11/17  18:44:19  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:31:44  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:25:55  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:55:41  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../hes/cbus_registry.h"
#include "subsys.h"
#include "../dev/flash_dvr_spec.h"
#include "../hes/dbus.h"
#include "../hes/if_fci.h"
#include "../hes/ucode.h"
#include "../hes/slot.h"
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
#define N_cisco7000
#define N_cisco7010
#include "../snmp/sr_lsmi.h"
#undef SNMPPART_DECL

/* this should go away soon */
extern uint which_7000_series(void);
    
static uint chassis_type;
static uint processor_type;
static uint chassis_slots;
static OID  system_id;
static boolean api_initialized = FALSE;

static void
snmp_platform_api_initialize (void)
{

    chassis_type = which_7000_series();
    processor_type = D_cardType_rp;
    switch (chassis_type) {
    default:
    case D_chassisType_c7000:
	chassis_slots = 7;
        system_id.oid_ptr = (unsigned long *) IDcisco7000;
        system_id.length = LNcisco7000;
        break;
    case D_chassisType_c7010:
	chassis_slots = 5;
        system_id.oid_ptr = (unsigned long *) IDcisco7010;
        system_id.length = LNcisco7010;
        break;
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
snmp_platform_get_value(snmp_platform_value_type selector)
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
    card_entry *cep;
    long card_index;

    if (!api_initialized)
        snmp_platform_api_initialize();

    card_index = cardIndex;
    if ((cep = reg_invoke_chassismib_get_cep(EXACT,&card_index)) == NULL)
        return(-1);
 
    switch (selector) {
    case SNMP_PLATFORM_CARD_OPER_STATUS:
	switch (cep->card_type) {
	case D_cardType_rp:
	    return(D_cardOperStatus_up);
	case D_cardType_chassisInterface7000:
            return(D_cardOperStatus_not_specified);
	case D_cardType_pa_1fe:
	case D_cardType_pa_1fe_tx_isl:
	case D_cardType_pa_1fe_fx_isl:
	case D_cardType_pa_1fe_tx_nisl:
	case D_cardType_pa_4e:
	case D_cardType_pa_5e:
	case D_cardType_pa_8e:
	case D_cardType_pa_4t:
	case D_cardType_pa_4r:
	    /* Add in support for pas when pa_info is made available */
            return(D_cardOperStatus_not_specified);
	default:
            return((slot_enabled(cep->card_slot_number)?
			D_cardOperStatus_up:D_cardOperStatus_down));
        }
    default:
        return -1;
    }
}

int
snmp_platform_get_if_value(hwidbtype *hwidb, 
                                snmp_platform_if_value_type selector)
{
    card_entry *cep;
    long card_index;
    ONE_ITEM_LIST(tmp, ETHER_HW_MEDIA_TYPE, sizeof(ulong), 0);

    if (!api_initialized)
        snmp_platform_api_initialize();

    if (hwidb == NULL)
        return(-1);

    switch (selector) {
    case SNMP_PLATFORM_IF_CARD_INDEX:
        if ((card_index = 
	     reg_invoke_chassismib_find_slot_card_index(hwidb->slot)) == -1)
	    return(-1);
        if ((cep = reg_invoke_chassismib_get_cep(EXACT,&card_index)) == NULL)
	    return(-1);
	switch (cep->card_type) {
	case D_cardType_vip:
	case D_cardType_vip2:
            return(reg_invoke_chassismib_find_sub_card_index(card_index, 
				(hwidb->slotunit >> PA_VIRTUAL_SLOT_BITS)));
	default:
	    return((uint) card_index);
	}
    case SNMP_PLATFORM_IF_SLOT_NUMBER:
        if ((card_index = 
	     reg_invoke_chassismib_find_slot_card_index(hwidb->slot)) == -1)
	    return(-1);
        if ((cep = reg_invoke_chassismib_get_cep(EXACT,&card_index)) == NULL)
	    return(-1);
	switch (cep->card_type) {
	case D_cardType_vip:
	case D_cardType_vip2:
	    /* 
	     * cardIfSlotNumber use should be phased out. PA interfaces set
             * to -1 to allow derivation of old cardIfTable.
             */
            return(-1); 
	default:
            return(hwidb->slot);
	}
    case SNMP_PLATFORM_IF_PORT_NUMBER:
        if ((card_index = 
	     reg_invoke_chassismib_find_slot_card_index(hwidb->slot)) == -1)
	    return(-1);
        if ((cep = reg_invoke_chassismib_get_cep(EXACT,&card_index)) == NULL)
	    return(-1);
	switch (cep->card_type) {
	case D_cardType_vip:
	case D_cardType_vip2: 
	    return((hwidb->slotunit) & (PA_VIRTUAL_SLOT_BOUNDARY-1));
        default:
            return(hwidb->slotunit);
        }
    case SNMP_PLATFORM_IF_CONNECTOR_TYPE_ENABLED:
        switch (hwidb->type) {
        case IDBTYPE_FEIP:
	    /*
	     * Get the media type
	     */
	    (*hwidb->state_config)(hwidb, IDB_CONTROL_READ_CONFIG, &tmp_list);

	    switch (tmp_item.u.l_value) {
	      case ETHER_MEDIA_TYPE_FEIP_RJ45:
		return(D_cardIfConnectorTypeEnabled_rj_45);
	      case ETHER_MEDIA_TYPE_FEIP_MII:
		return(D_cardIfConnectorTypeEnabled_db_40);
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
 * c7000_snmp_init
 *
 * Initialize C7000 SNMP support.
 *
 */
static void
c7000_snmp_init (subsystype *subsys)
{
    char swver[12], buf[12];
    dgtype* dbus;
    char* hwver;
    uint serialnum;

    if (!api_initialized)
        snmp_platform_api_initialize();

    sprintf(swver, "%u.%u", SYS_MAJORVERSION, SYS_MINORVERSION);

    dbus = dbus_rpslot_to_dbus();
    if (dbus) {
	ushort hw;
	
	hw = reg_invoke_get_hw_version(dbus, 0); /* slot not used for dbus */
	serialnum = dbus_serial_number(dbus);
	(void) sprintf(hwver = buf, "%u.%u", (hw >> 8) & 0xff, hw & 0xff);
    } else {
	serialnum = NO_SERIALNUM;
	hwver = NO_HWVERSION;
    }
    reg_invoke_chassismib_add_card_entry(D_cardType_rp, NO_HOTSWAP,
					 "25MHz 68040", serialnum,
					 hwver, swver, dbus_system_rp_slot());
    
}

/*
 * c7000_snmp subsystem header
 */
 
#define C7000_SNMP_MAJVERSION    1
#define C7000_SNMP_MINVERSION    0
#define C7000_SNMP_EDITVERSION   1
 
SUBSYS_HEADER(c7000_snmp, C7000_SNMP_MAJVERSION, C7000_SNMP_MINVERSION,
              C7000_SNMP_EDITVERSION,
              c7000_snmp_init, SUBSYS_CLASS_MANAGEMENT,
              NULL, NULL);
      

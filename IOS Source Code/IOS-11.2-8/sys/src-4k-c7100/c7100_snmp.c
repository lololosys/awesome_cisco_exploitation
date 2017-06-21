/* $Id: c7100_snmp.c,v 3.1.68.9 1996/07/26 01:10:20 feisal Exp $
 * $Source: /release/112/cvs/Xsys/src-4k-c7100/c7100_snmp.c,v $
 *------------------------------------------------------------------
 * c7100_snmp.c - Predator SNMP support
 *
 * October 1995, Michael Beesley
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: c7100_snmp.c,v $
 * Revision 3.1.68.9  1996/07/26  01:10:20  feisal
 * CSCdi61016:  Need to add CardType MIB definition for different flavors
 * of 1FE PA
 * Branch: California_branch
 *
 * Revision 3.1.68.8  1996/07/24  18:02:28  mbeesley
 * CSCdi63873:  Top two port adaptor slots are optional
 * Branch: California_branch
 *
 * Revision 3.1.68.7  1996/07/18  20:04:55  mbeesley
 * CSCdi63360:  Not all IO cards have fastether, make it optional
 * Branch: California_branch
 *
 * Revision 3.1.68.6  1996/05/21  06:39:38  mordock
 * Branch: California_branch
 * Improve snmp modularity via creating services to register interfaces
 * and chassis cards.
 * Add syslog mib.
 *
 * Revision 3.1.68.5  1996/05/14  02:56:58  feisal
 * CSCdi56797:  Card type for the 5EFL PA not added properly to the chasis
 * MIB
 * Branch: California_branch
 *
 * Revision 3.1.68.4  1996/04/22  03:35:37  rnardin
 * CSCdi53602:  SNMP nits on new platform
 * Branch: California_branch
 *
 * Revision 3.1.68.3  1996/04/08  17:33:10  ssangiah
 * CSCdi53822:  Need to deal with product name change.
 * Fix some left over references to the old Predator product name.
 * Branch: California_branch
 *
 * Revision 3.1.68.2  1996/04/06  02:05:43  ssangiah
 * CSCdi53822:  Need to deal with product name change.
 * Make changes to deal with the change in the product name for Predator
 * from c7100 to c7200.
 * Branch: California_branch
 *
 * Revision 3.1.68.1  1996/03/21  23:48:13  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.1.2.7  1996/03/18  23:33:13  mbeesley
 * Branch: ELC_branch
 * Sync to V111_1_3
 *
 * Revision 3.1.2.6  1996/02/15  19:45:09  dcarroll
 * CSCdi49079:  Fix snmp platform support
 * Branch: ELC_branch
 * Pull down updates to chassis mib. Get platform snmp
 * support working on c7100
 *
 * Revision 3.1.2.5  1996/02/08  09:26:54  gstovall
 * Branch: ELC_branch
 * Sync to ELC_baseline_960205.
 *
 * Revision 3.1.2.4  1996/01/16  02:16:53  dcarroll
 * Branch: ELC_branch
 * Sync ELC_branch between ELC_baseline_120995 and ELC_baseline_960110.
 *
 * Revision 3.1.2.3  1995/12/13  14:32:19  dcarroll
 * Branch: ELC_branch
 * Move pas_add_card_entry. Fix some debug messages.
 *
 * Revision 3.1.2.2  1995/12/11  19:23:07  mbeesley
 * Branch: ELC_branch
 * Sync ELC_branch to ELC_baseline_120995.
 *
 * Revision 3.1.2.1  1995/11/22  21:43:25  mbeesley
 * Initial commit of c7100 support.
 * Branch: ELC_branch
 *
 * Revision 3.1  1995/11/21  01:37:48  mbeesley
 * Add placeholder files for new development.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Include files
 */
#include "master.h"
#include "interface_private.h"
#include "sys_registry.h"
#include "subsys.h"
#include "packet.h"
#include "stacks.h"
#include "exception.h"
#include "logger.h"

#include "../pas/if_pas.h"
#include "../pas/pas_id.h"

#include "../snmp/snmp_api.h"
#include "../snmp/snmp_platform_api.h"
#include "../snmp/chassismib_registry.h"
#include "../hes/if_fci.h"
#include "../hes/parser_defs_ucode.h"
#include "../dev/flash_dvr_spec.h"
#include "../hes/ucode.h"
#include "../hes/slot.h"
#include "../if/ether.h"
#include "../if/ether_private.h"

#include "c7100_macaddr.h"

#define SNMPPART_DECL
#define N_cisco7206
#define N_cisco7204

#include "../snmp/sr_lsmi.h"

#undef SNMPPART_DECL

/*
 * Local storage
 */
static uint chassis_type;
static boolean api_initialized = FALSE;
static OID system_id;
static uint processor_type;
static uint chassis_slots;

/*
 * get_snmp_chassis_type:
 * Returns snmp chassisType for the platform.
 */
static uint get_snmp_chassis_type (void)
{
    if (c7100_chassis_model == C7100_FOUR_SLOT) {
	return(D_chassisType_c7204);
    } else if (c7100_chassis_model == C7100_SIX_SLOT) {
	return(D_chassisType_c7206);
    } else {
	return(D_chassisType_unknown);
    }
}

/*
 * get_snmp_system_id:
 * Returns snmp sysObjectID for the platform.
 */
static void get_snmp_system_id (OID *oid)
{
    if (c7100_chassis_model == C7100_FOUR_SLOT) {
	oid->oid_ptr = (unsigned long *) IDcisco7204;
	oid->length = LNcisco7204;
    } else {
	/* C7206 box by default */
	oid->oid_ptr = (unsigned long *) IDcisco7206;
	oid->length = LNcisco7206;
    }
}

/*
 * get_snmp_processor_type:
 * Returns snmp cardtype for onboard processor card
 */
static uint get_snmp_processor_type (void)
{
    return(D_cardType_cpu_7200_npe100);
}

/*
 * snmp_platform_api_initialize:
 * Initialize the SNMP API.
 */
static void snmp_platform_api_initialize (void)
{
    chassis_type = get_snmp_chassis_type();
    get_snmp_system_id(&system_id);
    processor_type = get_snmp_processor_type();
    chassis_slots = c7100_chassis_model + 1;
    api_initialized = TRUE;    
}

/*
 * snmp_platform_get_oid:
 * Get the platform OID
 */
OID *snmp_platform_get_oid (snmp_platform_oid_type selector)
{
    if (!api_initialized)
	snmp_platform_api_initialize();

    switch (selector) {

    case SNMP_PLATFORM_OID_SYS_OBJECT_ID:
	return(&system_id);
    default:
	return(NULL);
    }
}

/*
 * snmp_platform_get_value:
 * Return some snmp values
 */
uint snmp_platform_get_value (snmp_platform_value_type selector)
{
    if (!api_initialized) {
	snmp_platform_api_initialize();
    }

    switch (selector) {

    case SNMP_PLATFORM_CHASSIS_TYPE:
	return(chassis_type);

    case SNMP_PLATFORM_PROCESSOR_TYPE:
	return(processor_type);

    case SNMP_PLATFORM_SLOTS:
	return(chassis_slots);

    default:
	return(-1);
    }
}

/*
 * snmp_platform_get_card_value
 *
 * Return card index, slot number, port number, etc.
 */
int snmp_platform_get_card_value (uint cardIndex, 
				  snmp_platform_card_value_type selector)
{
    card_entry *cep;
    long card_index;

    if (!api_initialized)
        snmp_platform_api_initialize();

    card_index = cardIndex;
    if ((cep = reg_invoke_chassismib_get_cep(EXACT, &card_index)) == NULL)
	return(-1);
 
    switch (selector) {
      case SNMP_PLATFORM_CARD_OPER_STATUS:
	switch (cep->card_type) {
	  case D_cardType_cpu_7200_npe100:
	    return(D_cardOperStatus_up);
	  case D_cardType_pa_1fe:
	  case D_cardType_pa_1fe_tx_isl:
	  case D_cardType_pa_1fe_fx_isl:
	  case D_cardType_pa_1fe_tx_nisl:
	  case D_cardType_pa_4e:
	  case D_cardType_pa_5e:
	  case D_cardType_pa_8e:
	  case D_cardType_pa_4t:
	  case D_cardType_pa_4r:
	  case D_cardType_pa_fddi:
	  case D_cardType_c7200_io1fe:
          case D_cardType_c7200_io:
            return((slots[cep->card_slot_number].flags & (BOARD_DISABLED|BOARD_NONEXIST))?
		 D_cardOperStatus_down:D_cardOperStatus_up);
	  default:
	    return(D_cardOperStatus_not_specified);
	}
      default:
        return -1;
    }
}

/*
 * snmp_platform_get_if_value
 *
 * Return operational status of cards in the box.
 */
int snmp_platform_get_if_value (hwidbtype *hwidb,
				snmp_platform_if_value_type selector)
{
    ethersb* esb;

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
        case IDBTYPE_FEIP:
            esb = idb_use_hwsb_inline(hwidb, HWIDB_SB_ETHER);
            if (esb != NULL) {
                switch (esb->media_type) {
                case ETHER_MEDIA_TYPE_FEIP_RJ45:
                    return(D_cardIfConnectorTypeEnabled_rj_45);
                case ETHER_MEDIA_TYPE_FEIP_MII:
                    return(D_cardIfConnectorTypeEnabled_db_40);
                default:
                    return(D_cardIfConnectorTypeEnabled_none);
                }
            } else {
                return(D_cardIfConnectorTypeEnabled_not_specified);
            }
	  case IDBTYPE_AMDP2:
	    return(D_cardIfConnectorTypeEnabled_rj_45);
        default:
            return(D_cardIfConnectorTypeEnabled_not_specified);
        }
    default:
        return(-1);
    }
}

/*
 * c7100_snmp_init
 * Initialize SNMP support.
 */
static void c7100_snmp_init (subsystype *subsys)
{
    if (!api_initialized) {
        snmp_platform_api_initialize();
	reg_invoke_chassismib_add_card_entry(processor_type,
			    NO_HOTSWAP,
			    platform_get_string(PLATFORM_STRING_NOM_DU_JOUR),
			    platform_get_value(PLATFORM_VALUE_HARDWARE_SERIAL),
			    platform_get_string(PLATFORM_STRING_HARDWARE_REVISION),
			    NO_SWVERSION,
			    NO_SLOTNUMBER);
    }
}

/*
 * pas_add_card_entry:
 * Add a Port Adapter to the chassis MIB
 */
void pas_add_card_entry (uint pa_bay)
{
    card_type_t card_type;
    char* card_descr;
    char* swver;
    char* hwver;
    char* cp;
    char buf[48];

    slottype *sp = &slots[pa_bay];

    card_type = D_cardType_unknown;
    card_descr = "Unknown";
    swver = NO_SWVERSION;
    cp = buf;

    switch (sp->pa_type) {
      case PA_HARDWARE_1FE:
        card_type = D_cardType_pa_1fe;
        card_descr = "FastEthernet";
        break;

      case PA_HARDWARE_1FE_TX_ISL:
        card_type = D_cardType_pa_1fe_tx_isl;
        card_descr = "FastEthernet (TX-ISL)";
        break;

      case PA_HARDWARE_1FE_FX_ISL:
        card_type = D_cardType_pa_1fe_fx_isl;
        card_descr = "FastEthernet (FX-ISL)";
        break;

      case PA_HARDWARE_1FE_TX_NISL:
        card_type = D_cardType_pa_1fe_tx_nisl;
        card_descr = "FastEthernet (TX-nISL)";
        break;

      case PA_HARDWARE_8E:
        card_type = D_cardType_pa_8e;
        card_descr = "Ethernet";
        break;

      case PA_HARDWARE_4E:
        card_type = D_cardType_pa_4e;
        card_descr = "Ethernet";
        break;

      case PA_HARDWARE_5E:
        card_type = D_cardType_pa_5e;
        card_descr = "Ethernet";
        break;

      case PA_HARDWARE_4T:
        card_type = D_cardType_pa_4t;
        card_descr = "Serial";
	cp += sprintf(swver = cp, "%u.%u",
		      sp->sw_version >> 8, sp->sw_version & 0xFF);
	cp++;
        break;

      case PA_HARDWARE_4R:
        card_type = D_cardType_pa_4r;
        card_descr = "Token Ring";
        break;

      case PA_HARDWARE_C7100_IO1FE:
        card_type = D_cardType_c7200_io1fe;
        card_descr = "I/O FastEthernet";
        break;

      case PA_HARDWARE_C7100_IO:
        card_type = D_cardType_c7200_io;
        card_descr = "I/O Controller";
        break;

      case PA_HARDWARE_FDDI_DAS_MM:
      case PA_HARDWARE_FDDI_DAS_SS:
        card_type = D_cardType_pa_fddi;
        card_descr = "Fddi";
        break;

      default:
	card_type = D_cardType_unknown;
	card_descr = "Unknown";
        break;
    }

    cp += sprintf(hwver = cp, "%u.%u",
		  sp->hw_version >> 8, sp->hw_version & 0xFF);

    reg_invoke_chassismib_add_card_entry(card_type,
					 ((void*)(sp->slot_num +1)),
					 card_descr,
					 sp->serial_number,
					 hwver,
					 swver,
					 sp->slot_num);
}

/*
 * c7100 SNMP subsystem header
 */
 
#define C7100_SNMP_MAJVERSION    1
#define C7100_SNMP_MINVERSION    0
#define C7100_SNMP_EDITVERSION   1
 
SUBSYS_HEADER(c7100_snmp, C7100_SNMP_MAJVERSION, C7100_SNMP_MINVERSION,
              C7100_SNMP_EDITVERSION,
              c7100_snmp_init, SUBSYS_CLASS_MANAGEMENT,
              NULL, NULL);

/* end of file */
      

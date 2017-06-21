/* $Id: c3000_snmp.c,v 3.5.46.4 1996/06/17 08:35:35 jhernand Exp $
 * $Source: /release/112/cvs/Xsys/src-68-c3000/c3000_snmp.c,v $
 *------------------------------------------------------------------
 * c3000_snmp.c - SNMP routines for IGS/2500/3000 platforms
 *
 * February 1995, Scott Mackie (moved from init_pan.c)
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: c3000_snmp.c,v $
 * Revision 3.5.46.4  1996/06/17  08:35:35  jhernand
 * Commit Modular Cobra to California_branch
 *
 * Revision 3.5.46.3  1996/06/13  19:09:01  etrehus
 * Commit boa to California Branch
 *
 * Revision 3.5.46.2  1996/05/21  06:39:44  mordock
 * Branch: California_branch
 * Improve snmp modularity via creating services to register interfaces
 * and chassis cards.
 * Add syslog mib.
 *
 * Revision 3.5.46.1  1996/03/18  21:59:13  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.5.14.2  1996/03/09  05:12:17  hampton
 * Slim down the kernel of the router.
 * Branch: DeadKingOnAThrone_branch
 *
 * Revision 3.5.14.1  1996/02/20  17:56:00  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.5  1995/12/15  03:15:39  rpratt
 * CSCdi45651:  Chassis MIB enhancements made Port Ready
 *
 * Revision 3.4  1995/12/03  18:00:18  thille
 * CSCdi44905:  ciscopro targets not in 11.1
 * Put them in.
 *
 * Revision 3.3  1995/11/24  16:41:37  jjohnson
 * CSCdi42637:  platform-specific code should be moved out of sub_snmp
 * Install an API for accessing platform-specific SNMP objects
 *
 * Revision 3.2  1995/11/17  18:43:14  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:25:06  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:27:07  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  22:54:36  hampton
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

/*
 * generate all possible sysObjectID values for this platform
 */
#define SNMPPART_DECL
#define N_ciscoAccessProRC
#define N_ciscoAccessProEC
#define N_ciscoIGS
#define N_cisco2000
#define N_cisco2500
#define N_cisco2501
#define N_cisco2502
#define N_cisco2503
#define N_cisco2504
#define N_cisco2505
#define N_cisco2506
#define N_cisco2507
#define N_cisco2508
#define N_cisco2509
#define N_cisco2510
#define N_cisco2511
#define N_cisco2512
#define N_cisco2513
#define N_cisco2514
#define N_cisco2515
#define N_cisco2516
#define N_cisco2520
#define N_cisco2521
#define N_cisco2522
#define N_cisco2523
#define N_cisco2524
#define N_cisco2525
#define N_cisco3000
#define N_cisco3101
#define N_cisco3102
#define N_cisco3103
#define N_cisco3104
#define N_cisco3202
#define N_cisco3204
#define N_ciscoPro2501
#define N_ciscoPro2503
#define N_ciscoPro2505
#define N_ciscoPro2509
#define N_ciscoPro2511
#define N_ciscoPro2514
#define N_ciscoPro2516
#define N_ciscoPro2520
#define N_ciscoPro2522
#define N_ciscoPro2524
#define N_ciscoPro2500PCE

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
    ushort misc_reg = 0;

    switch (cpu_type) {
      case CPU_PAN:
	return D_chassisType_igs;

      case CPU_IGS_BRUT:
	return D_chassisType_c2000;

      case CPU_BASS:
      case CPU_MERLOT:
	return D_chassisType_c3000;

      case CPU_CRISTAL:
	switch (cookie->interfaces) {
	case INTERFACE_1E2T1B:              /* 3104 */
	    return D_chassisType_c3104;
	case INTERFACE_1R2T1B:              /* 3204 */
	    return D_chassisType_c3204;
	default:
	    return D_chassisType_c3000;
	}

      case CPU_CANCUN:
        if (touch_device((void *)ADRSPC_LSS_MISC)) {
            misc_reg = *(ushort *)ADRSPC_LSS_MISC;
            misc_reg &= LSS_DAUGHTER;
        }
	switch (cookie->interfaces) {
	case INTERFACE_1E2T:
	    /* Cancun and copan share same cookie values.
	     * Differentiate with number of async lines.
	     */
	    switch (nttylines) {
	    case 8:                         /* 2509 */
		return D_chassisType_c2509;
	    case 16:                        /* 2511 */
		return D_chassisType_c2511;
	    default:                        /* 2501 */
		return D_chassisType_c2501;
	    }
	case INTERFACE_1R2T:
	    /* Cancun and copan share same cookie values.
	     * Differentiate with number of async lines.
	     */
	    switch (nttylines) {
	    case 8:                         /* 2510 */
		return D_chassisType_c2510;
	    case 16:                        /* 2512 */
		return D_chassisType_c2512;
	    default:                        /* 2502 */
		return D_chassisType_c2502;
	    }
	/* cancun with bri */
	case INTERFACE_1E2T1B:              /* 2503 */
		return D_chassisType_c2503;
	case INTERFACE_1R2T1B:              /* 2504 */
		return D_chassisType_c2504;
	/* fiji */
	case INTERFACE_1E1T1I:              /* AP-EC */
		return D_chassisType_accessProEC;
	case INTERFACE_1R1T1I:              /* AP-RC */
		return D_chassisType_accessProRC;
	/* campeche */
	case INTERFACE_1R2T1E:              /* 2513 */
		return D_chassisType_c2513;
	case INTERFACE_2E2T:                /* 2514 */
		return D_chassisType_c2514;
	case INTERFACE_2R2T:                /* 2515 */
		return D_chassisType_c2515;
	/* galapagos */
	case INTERFACE_1E2Thub:
	    if (netherhubports == 8)        /* 2505 */
		return D_chassisType_c2505;
	    else                            /* 2507 */
		return D_chassisType_c2507;
        /* cabrillo */
        case INTERFACE_1E2T1Bhub:           /* 2516 */
	    return D_chassisType_c2516;
        /* cobra */
        case INTERFACE_1E2T1B2AS:
            if (!misc_reg)
                return D_chassisType_c2522; /* 2522 */
            else
                return D_chassisType_c2520; /* 2520 */
        case INTERFACE_1R2T1B2AS:
            if (!misc_reg)
                return D_chassisType_c2523; /* 2523 */
            else
                return D_chassisType_c2521; /* 2521 */
        /* boa */
        case INTERFACE_1E1B2W:             /* 2524 */
            return D_chassisType_c2524;
        case INTERFACE_1R1B2W:
            return D_chassisType_c2525;
	default:                            /* unknown cpu_cancun */
	    return D_chassisType_c2500;
	}

    default:                                /* unknown cpu_type */
	return D_chassisType_unknown;
    }
}


/*
 * Returns snmp cardtype for onboard processor card
 */
static uint
get_snmp_processor_type (void)
{
    switch (cpu_type) {
      case CPU_PAN:
	return (D_cardType_cpu_igs);
	break;
      case CPU_MERLOT:
	return (D_cardType_cpu_3000);
	break;
      case CPU_CRISTAL:
	return (D_cardType_cpu_3100);
	break;
      case CPU_CANCUN:
	if ((cookie->interfaces == INTERFACE_1E1T1I) ||
	    (cookie->interfaces == INTERFACE_1R1T1I))
	    return (D_cardType_cpu_accessPro);
	else
	    return (D_cardType_cpu_2500);
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
    case D_chassisType_accessProRC:
        oid->oid_ptr = (unsigned long *) IDciscoAccessProRC;
        oid->length = LNciscoAccessProRC;
        break;
    case D_chassisType_accessProEC:
	if (ciscopro_platform) {
            oid->oid_ptr = (unsigned long *) IDciscoPro2500PCE;
            oid->length = LNciscoPro2500PCE;
	} else {
            oid->oid_ptr = (unsigned long *) IDciscoAccessProEC;
            oid->length = LNciscoAccessProEC;
	}
        break;
    case D_chassisType_igs:
        oid->oid_ptr = (unsigned long *) IDciscoIGS;
        oid->length = LNciscoIGS;
        break;
    case D_chassisType_c2000:
        oid->oid_ptr = (unsigned long *) IDcisco2000;
        oid->length = LNcisco2000;
        break;
    case D_chassisType_c2500:
    default:
        oid->oid_ptr = (unsigned long *) IDcisco2500;
        oid->length = LNcisco2500;
        break;
    case D_chassisType_c2501:
	if (ciscopro_platform) {
            oid->oid_ptr = (unsigned long *) IDciscoPro2501;
            oid->length = LNciscoPro2501;
	} else {
            oid->oid_ptr = (unsigned long *) IDcisco2501;
            oid->length = LNcisco2501;
	}
        break;
    case D_chassisType_c2502:
        oid->oid_ptr = (unsigned long *) IDcisco2502;
        oid->length = LNcisco2502;
        break;
    case D_chassisType_c2503:
	if (ciscopro_platform) {
            oid->oid_ptr = (unsigned long *) IDciscoPro2503;
            oid->length = LNciscoPro2503;
	} else {
            oid->oid_ptr = (unsigned long *) IDcisco2503;
            oid->length = LNcisco2503;
	}
        break;
    case D_chassisType_c2504:
        oid->oid_ptr = (unsigned long *) IDcisco2504;
        oid->length = LNcisco2504;
        break;
    case D_chassisType_c2505:
	if (ciscopro_platform) {
            oid->oid_ptr = (unsigned long *) IDciscoPro2505;
            oid->length = LNciscoPro2505;
	} else {
            oid->oid_ptr = (unsigned long *) IDcisco2505;
            oid->length = LNcisco2505;
	}
        break;
    case D_chassisType_c2506:
        oid->oid_ptr = (unsigned long *) IDcisco2506;
        oid->length = LNcisco2506;
        break;
    case D_chassisType_c2507:
        oid->oid_ptr = (unsigned long *) IDcisco2507;
        oid->length = LNcisco2507;
        break;
    case D_chassisType_c2508:
        oid->oid_ptr = (unsigned long *) IDcisco2508;
        oid->length = LNcisco2508;
        break;
    case D_chassisType_c2509:
	if (ciscopro_platform) {
            oid->oid_ptr = (unsigned long *) IDciscoPro2509;
            oid->length = LNciscoPro2509;
	} else {
            oid->oid_ptr = (unsigned long *) IDcisco2509;
            oid->length = LNcisco2509;
        }
        break;
    case D_chassisType_c2510:
        oid->oid_ptr = (unsigned long *) IDcisco2510;
        oid->length = LNcisco2510;
        break;
    case D_chassisType_c2511:
	if (ciscopro_platform) {
            oid->oid_ptr = (unsigned long *) IDciscoPro2511;
            oid->length = LNciscoPro2511;
	} else {
            oid->oid_ptr = (unsigned long *) IDcisco2511;
            oid->length = LNcisco2511;
	}
        break;
    case D_chassisType_c2512:
        oid->oid_ptr = (unsigned long *) IDcisco2512;
        oid->length = LNcisco2512;
        break;
    case D_chassisType_c2513:
        oid->oid_ptr = (unsigned long *) IDcisco2513;
        oid->length = LNcisco2513;
        break;
    case D_chassisType_c2514:
	if (ciscopro_platform) {
            oid->oid_ptr = (unsigned long *) IDciscoPro2514;
            oid->length = LNciscoPro2514;
	} else {
            oid->oid_ptr = (unsigned long *) IDcisco2514;
            oid->length = LNcisco2514;
	}
        break;
    case D_chassisType_c2515:
        oid->oid_ptr = (unsigned long *) IDcisco2515;
        oid->length = LNcisco2515;
        break;
    case D_chassisType_c2516:
	if (ciscopro_platform) {
            oid->oid_ptr = (unsigned long *) IDciscoPro2516;
            oid->length = LNciscoPro2516;
	} else {
            oid->oid_ptr = (unsigned long *) IDcisco2516;
            oid->length = LNcisco2516;
	}
        break;
    case D_chassisType_c2520:
        if (ciscopro_platform) {
            oid->oid_ptr = (unsigned long *) IDciscoPro2520;
            oid->length = LNciscoPro2520;
        } else {
            oid->oid_ptr = (unsigned long *) IDcisco2520;
            oid->length = LNcisco2520;
        }
        break;
    case D_chassisType_c2521:
        oid->oid_ptr = (unsigned long *) IDcisco2521;
        oid->length = LNcisco2521;
        break;
    case D_chassisType_c2522:
        if (ciscopro_platform) {
            oid->oid_ptr = (unsigned long *) IDciscoPro2522;
            oid->length = LNciscoPro2522;
        } else {
            oid->oid_ptr = (unsigned long *) IDcisco2522;
            oid->length = LNcisco2522;
        }
        break;
    case D_chassisType_c2523:
        oid->oid_ptr = (unsigned long *) IDcisco2523;
        oid->length = LNcisco2523;
        break;
    case D_chassisType_c2524:
	oid->oid_ptr = (unsigned long *) IDcisco2524;
	oid->length = LNcisco2524;
	break;
    case D_chassisType_c2525:
	oid->oid_ptr = (unsigned long *) IDcisco2525;
	oid->length = LNcisco2525;
	break;
    case D_chassisType_c3000:
        oid->oid_ptr = (unsigned long *) IDcisco3000;
        oid->length = LNcisco3000;
        break;
    case D_chassisType_c3101:
        oid->oid_ptr = (unsigned long *) IDcisco3101;
        oid->length = LNcisco3101;
        break;
    case D_chassisType_c3102:
        oid->oid_ptr = (unsigned long *) IDcisco3102;
        oid->length = LNcisco3102;
        break;
    case D_chassisType_c3103:
        oid->oid_ptr = (unsigned long *) IDcisco3103;
        oid->length = LNcisco3103;
        break;
    case D_chassisType_c3104:
        oid->oid_ptr = (unsigned long *) IDcisco3104;
        oid->length = LNcisco3104;
        break;
    case D_chassisType_c3202:
        oid->oid_ptr = (unsigned long *) IDcisco3202;
        oid->length = LNcisco3202;
        break;
    case D_chassisType_c3204:
        oid->oid_ptr = (unsigned long *) IDcisco3204;
        oid->length = LNcisco3204;
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
 * c3000_snmp_init
 *
 * Initialize C3000 SNMP support.
 *
 */
 
void c3000_snmp_init (subsystype *subsys)
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
 * c3000_snmp subsystem header
 */
 
#define C3000_SNMP_MAJVERSION    1
#define C3000_SNMP_MINVERSION    0
#define C3000_SNMP_EDITVERSION   1
 
SUBSYS_HEADER(c3000_snmp, C3000_SNMP_MAJVERSION, C3000_SNMP_MINVERSION,
              C3000_SNMP_EDITVERSION,
              c3000_snmp_init, SUBSYS_CLASS_MANAGEMENT,
              NULL, NULL);
      

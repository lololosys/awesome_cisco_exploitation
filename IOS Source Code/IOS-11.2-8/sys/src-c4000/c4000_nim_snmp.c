/* $Id: c4000_nim_snmp.c,v 3.2.62.2 1996/05/21 06:40:06 mordock Exp $
 * $Source: /release/112/cvs/Xsys/src-c4000/c4000_nim_snmp.c,v $
 *------------------------------------------------------------------
 * c4000_nim_snmp.c - SNMP routines for handling 4000/4500 NIMs
 *
 * February 1995, Scott Mackie (moved from init_c4000.c)
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: c4000_nim_snmp.c,v $
 * Revision 3.2.62.2  1996/05/21  06:40:06  mordock
 * Branch: California_branch
 * Improve snmp modularity via creating services to register interfaces
 * and chassis cards.
 * Add syslog mib.
 *
 * Revision 3.2.62.1  1996/05/10  19:26:35  tangiral
 * CSCdi50959:  card type ct1 not supported in cardTable mib
 * Branch: California_branch
 * Since CT1 & CE1 don't have HWIDBs after intialization, scan their CDBs
 * to create cardTables for them.
 *
 * Revision 3.2  1995/11/17  18:45:43  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:26:40  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:56:34  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "interface_private.h"
#include "sys_registry.h"
#include "subsys.h"
#include "packet.h"
#include "stacks.h"
#include "exception.h"
#include "logger.h"
#include "../les/if_les.h"
#include "../snmp/chassismib_registry.h"
#include "../les/init_les.h"
#include "../les/if_c4000_mcn.h"
#include "../if/if_controller.h"



void nim_add_chassis_mib_cards (void)
{
    hwidbtype* hwidb;
    cdbtype *cdb;
    uint ix, max_slot;

    char* cp;
    char* hwver;
    char* swver;
    char buf[64];
    char* card_descr;
    card_type_t card_type;
    boolean hwidb_present;

    max_slot = 0;
    FOR_ALL_HWIDBS(hwidb)
	if (hwidb->slot > max_slot)
	    max_slot = hwidb->slot;
    FOR_ALL_CDBS(cdb)
        if (cdb->slot > max_slot)
            max_slot = cdb->slot;
 

    for (ix = 0; ix <= max_slot; ++ix) {

        hwidb_present = TRUE;
	FOR_ALL_HWIDBS(hwidb)
	    if (ix == hwidb->slot)
		break;
        if (hwidb == NULL) {                /* No HWIDB, check for CDB */
            hwidb_present = FALSE;
            FOR_ALL_CDBS(cdb)
                if (ix == cdb->slot)
                    break;
            if (cdb == NULL)
                continue;
        }


	cp = buf;
	swver = NO_SWVERSION;
	if (hwidb_present) {
	    cp += sprintf(hwver = cp, "%u", hwidb->nim_version);
	    switch (hwidb->nim_type) {
	  	case XX_FDDI_SBRIDGE:
	    	   card_type = D_cardType_npm_4000_fddi_sas;
	           card_descr = "SAS FDDI";
	           break;

	  	case XX_FDDI_DBRIDGE:
	           card_type = D_cardType_npm_4000_fddi_das;
	           card_descr = "DAS FDDI";
	           break;

	        case XX_ETHER:
	           card_type = D_cardType_npm_4000_1e;
	           card_descr = "Single port Ethernet";
	           break;

	        case XX_TOKEN_RING:
	           card_type = D_cardType_npm_4000_1r;
	           card_descr = "Single port token ring";
	           break;

	        case XX_SERIAL_DUAL:
	           card_type = D_cardType_npm_4000_2s;
	           card_descr = "Dual port serial interface";
	           break;

	        case XX_ETHER_SINGLE:
	           card_type = D_cardType_npm_4000_2e1;
	           card_descr = "Dual port Ethernet (minus one)";
	           break;

	        case XX_ETHER_DUAL:
	           card_type = D_cardType_npm_4000_2e;
	           card_descr = "Dual port Ethernet";
	           break;

	        case XX_PCI:
	        /* 
	         * right now, XX_PCI is used by the 6E NIM only.  
	         * When other PCI NIMs are implemented, need to 
	         * distinguish among adapter ids.
	         */
	           card_type = D_cardType_npm_4000_6e;
	           card_descr = "Six port Ethernet";
	           break;
	    
	        case XX_TOKEN_SINGLE:
	           card_type = D_cardType_npm_4000_2r1;
	           card_descr = "Dual port token ring (minus one)";
	           break;

	        case XX_TOKEN_DUAL:
	           card_type = D_cardType_npm_4000_2r;
	           card_descr = "Dual port token ring";
	           break;

          	case XX_SERIAL_QUAD:
	           card_type = D_cardType_npm_4000_4t;
	           card_descr = "Quad port serial interface";
	           break;

          	case XX_MCN:
		     /*
                     * The only MCN NIM which gets hwidb during initialization
                     * is MBRI. CT1 & CE1 don't get hwidbs during init. But
                     * they get cdbs.
                     */
                   switch (mcn_get_adapter_type(hwidb->slot)) {
              		case MCN_PA_BRI_SINGLE:
                           card_type = D_cardType_npm_4000_4b;
                           card_descr = "Four port BRI";
                           break;
              		case MCN_PA_BRI_DUAL:
                	   card_type = D_cardType_npm_4000_8b;
                           card_descr = "Eight port BRI";
                           break;
              		case MCN_EMPTY:
              	        default:
                	   card_type = D_cardType_unknown;
                	   card_descr = "Unknown interface card";
                	   break;
            	   }
                   break;

          	case XX_ATM_BX50:
                   card_type = D_cardType_npm_4000_1a;
                   card_descr = "ATM";
                   break;

	  	default:
	           card_type = D_cardType_unknown;
	    	   card_descr = "Unknown interface card";
	           break;
	    }
	    reg_invoke_chassismib_add_card_entry(
		      card_type, NO_HOTSWAP, card_descr, NO_SERIALNUM,
		      hwver, swver, hwidb->slot);
	 } else  {
            /*
             * Create cardtables for MCN CT1 & CE1 from their CDBs as
             * they don't get hwidbs during initialization.
             */
            cp += sprintf(hwver = cp, "%u", nim_get_version(cdb->slot));
            switch (mcn_get_adapter_type(cdb->slot)) {
                case MCN_PA_T1:
                  card_type = D_cardType_npm_4000_ct1;
                  card_descr = "Channelized T1/PRI";
                  break;
                case MCN_PA_E1:
                  card_type = D_cardType_npm_4000_ce1;
                  card_descr = "Channelized E1/PRI";
                  break;
                case MCN_EMPTY:
                default:
                  card_type = D_cardType_unknown;
                  card_descr = "Unknown interface card";
                  break;
            }
            reg_invoke_chassismib_add_card_entry(
                      card_type, NO_HOTSWAP, card_descr, NO_SERIALNUM,
                      hwver, swver, cdb->slot);
        }
    }
    return;
}

/*
 * nim_snmp_init
 *
 * Initialize NIM SNMP support.
 *
 */
 
void nim_snmp_init (subsystype *subsys)
{
    nim_add_chassis_mib_cards();
}

/*
 * nim_snmp subsystem header
 */
 
#define NIM_SNMP_MAJVERSION    1
#define NIM_SNMP_MINVERSION    0
#define NIM_SNMP_EDITVERSION   1
 
SUBSYS_HEADER(nim_snmp, NIM_SNMP_MAJVERSION, NIM_SNMP_MINVERSION,
              NIM_SNMP_EDITVERSION,
              nim_snmp_init, SUBSYS_CLASS_MANAGEMENT,
              NULL, NULL);


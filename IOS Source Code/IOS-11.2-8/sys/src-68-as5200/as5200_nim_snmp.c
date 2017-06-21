/* $Id: as5200_nim_snmp.c,v 1.1.20.2 1996/07/10 21:32:19 styang Exp $
 * $Source: /release/112/cvs/Xsys/src-68-as5200/as5200_nim_snmp.c,v $
 *------------------------------------------------------------------
 * as5200_nim_snmp.c - SNMP routines for handling as5200 NIMs
 *
 * January 22 1996, Simon Chen (ported from c4000_nim_snmp.c)
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: as5200_nim_snmp.c,v $
 * Revision 1.1.20.2  1996/07/10  21:32:19  styang
 * CSCdi55120:  nm: cardTable objects need work
 * Branch: California_branch
 * 1.To provide the t1/e1 firmware version number in the chassis mib.
 * 2.'card descripion' fileds are modified to give beeter description.
 *
 * Revision 1.1.20.1  1996/06/16  21:19:34  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 1.1  1996/03/14  01:37:57  styang
 * Initial revision
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

#include "../snmp/chassismib_registry.h"
#include "as5200.h"
#include "subsys.h"
#include "../dev/flash_defines.h"
#include "../les/if_as5200_mcn.h"
#include "../les/if_as5200_modem.h"


/*
 covernt the hw version number form 0 -- 15, to ascii
 */

static void as5200_convert_ver_num_to_ascii (int hwver_num, 
                                                char *buf) 
{
   if (hwver_num < 9) {
      *buf++ = hwver_num + '0';
      *buf='\0';
   }
   else {
      /* else 10 - 15 */
      *buf++= '1';
      *buf++ = hwver_num - 10 + '0';
      *buf='\0';
   }
}

static void  as5200_modem_add_one_card (ushort slot)
{
   modem_carrier_cookie_t *card_cookie; 
   modem_module_cookie_t *module_cookie;
   ushort module;
   char* card_descr;
   char hwver_buf[8];
   int hwver_num;

   
   if (modem_pool->modem_slot[slot] == NULL) {
      /* no card plugged in */
      return;
   }

   /* register carrier card first */    
   card_cookie = &(modem_pool->modem_slot[slot]->modem_cookie->carrier); 

   card_descr = "AS5200 Modem Carrier Card";
   hwver_num = (modem_pool->modem_slot[slot]->modem_reg->mdb_rev) & 0x000f;
   /* convert hardware version number from int to ascii */

   as5200_convert_ver_num_to_ascii(hwver_num, &hwver_buf[0]);

   reg_invoke_chassismib_add_sub_card_entry(D_cardType_as5200_carrier, 
                                        NO_HOTSWAP, card_descr, NO_SERIALNUM,
                                        &hwver_buf[0], NO_SWVERSION, slot, 0, 
                                        card_cookie->max_modules);

   card_descr = "Microcom 12-Modem Module card";
 
   /* now register modem module cards */
   for (module=0; module < card_cookie->max_modules; module++) {
      module_cookie = 
         &(modem_pool->modem_slot[slot]->modem_cookie->module[module]);
      if (module_cookie->number_of_modems != 0xff) {
         /* the modem daughter card is plugged in */
	 hwver_buf[0] = module_cookie->rev_num[0];
	 hwver_buf[1] = module_cookie->rev_num[1];
	 hwver_buf[2] = '.';
	 hwver_buf[3] = module_cookie->rev_num[2];
	 hwver_buf[4] = module_cookie->rev_num[3];
	 hwver_buf[5] = '\0';

         reg_invoke_chassismib_add_sub_card_entry(D_cardType_pm_as5xxx_12m, 
                         NO_HOTSWAP, card_descr, NO_SERIALNUM,
                         &hwver_buf[0], NO_SWVERSION, module, 
                         reg_invoke_chassismib_find_slot_card_index(slot), 0);
         
      }
   }
}

void as5200_nim_add_chassis_mib_cards (void)
{
    uint ix;
    ushort type;
    char* cp;
    char* hwver;
    char buf[64];
    char* card_descr;
    card_type_t card_type;
    char swver_buf[8];

    for (ix = 0; ix < AS5200_MAX_SLOTS; ++ix) {

       if (nim_exists(ix)) {
	  type = nim_get_type(ix);
	  if (type == BRASIL_NIM_MCN) {
	     cp = buf;
	     cp += sprintf(hwver = cp, "%u", nim_get_version(ix));
             switch (mcn_get_adapter_type(ix)) {
            	case MCN_PA_T1:
                   card_type = D_cardType_as5200_2ct1;
                   card_descr = "Dual Port Channelized T1/PRI Card";
                   as5200_convert_ver_num_to_ascii((ushort)t1_version, &swver_buf[0]);
            	   break;
            	case MCN_PA_E1:
                   card_type = D_cardType_as5200_2ce1;
                   card_descr = "Dual Port Channelized E1/PRI Card";
                   as5200_convert_ver_num_to_ascii((ushort)e1_version, &swver_buf[0]);
            	   break;
            	default:
            	   card_type = D_cardType_unknown;
            	   card_descr = "Unknown interface card";
            	   break;
             }
	     reg_invoke_chassismib_add_card_entry( card_type, NO_HOTSWAP, 
                                                   card_descr, NO_SERIALNUM,
		                                   hwver, &swver_buf[0], ix);
	  }
          else if (type == BRASIL_NIM_ASYNC_MODEM) {
             as5200_modem_add_one_card(ix);
          }
       }
    }
    return;
}

/*
 * as5200_nim_snmp_init
 *
 * Initialize NIM SNMP support.
 *
 */
 
void as5200_nim_snmp_init (subsystype *subsys)
{
    as5200_nim_add_chassis_mib_cards();
}

/*
 * as5200_nim_snmp subsystem header
 */
 
#define NIM_SNMP_MAJVERSION    1
#define NIM_SNMP_MINVERSION    0
#define NIM_SNMP_EDITVERSION   1
 
SUBSYS_HEADER(as5200_nim_snmp, NIM_SNMP_MAJVERSION, NIM_SNMP_MINVERSION,
              NIM_SNMP_EDITVERSION,
              as5200_nim_snmp_init, SUBSYS_CLASS_MANAGEMENT,
              NULL, NULL);


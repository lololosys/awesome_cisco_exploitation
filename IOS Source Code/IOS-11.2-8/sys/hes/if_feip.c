/* $Id: if_feip.c,v 3.6.12.2 1996/08/28 12:44:54 thille Exp $
 * $Source: /release/112/cvs/Xsys/hes/if_feip.c,v $
 *------------------------------------------------------------------
 * if_feip.c:  Stuff for the 100Base-X Fast Ethernet Processor (FEIP)
 *
 * November 1994, Gary Kramling
 * Adapted from if_cip.c by Stig Thormodsrud & William H. Palmer
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: if_feip.c,v $
 * Revision 3.6.12.2  1996/08/28  12:44:54  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.6.12.1  1996/03/18  19:41:43  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.26.2  1996/02/20  00:51:55  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.6  1996/02/13  21:26:42  rbatz
 * CSCdi30258:  Fixed the following:
 *                  Duplicate half-duplex commands.
 *                  Could not do half-duplex stun.
 *                  Could not tune half-duplex timers for bstun.
 *
 * Revision 3.5  1996/02/07  16:11:34  widmer
 * CSCdi48285:  Remove function name from bad_parser_subcommand
 *
 * Revision 3.4  1996/02/01  06:02:35  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.3.26.1  1996/01/24  21:46:38  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.3  1995/11/17  09:17:56  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:27:51  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:40:19  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/11/08  21:00:34  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.3  1995/08/09  00:19:48  kramling
 * CSCdi38439:  FEIP product is now supported in 11.0.  Update support to
 * 10.3.
 *
 * Revision 2.2  1995/06/09  00:03:32  hampton
 * Remove include of deleted file.  [CSCdi35520]
 *
 * Revision 2.1  1995/06/07 20:41:27  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../os/clock.h"
#include "registry.h"
#include "sys_registry.h"
#include "media_registry.h"
#include "cbus_registry.h"
#include "subsys.h"
#include "interface_private.h"
#include "if_mci.h"
#include "if_fci.h"
#include "../hes/if_hes_common.h"
#include "dgbus.h"
#include "if_feip.h"
#include "if_vip.h"
#include "logger.h"
#include "../if/network.h"
#include "packet.h"
#include "dbus.h"
#include "config.h"
#include "parser.h"
#include "../if/parser_defs_fast_ethernet.h"
#include "../parser/parser_defs_half_duplex.h"
#include "../ui/debug.h"
#include "../if/fast_ethernet_debug.h"
#include "../iprouting/route.h"
#include "../ip/ip_registry.h"
#include "../ip/ip.h"
#include "../snmp/sr_old_lchassismib.h"
#include "../os/gdb.h"
#include "../if/static_map.h"
#include "ieee.h"
#include "if_cmd.h"
#include "../if/ether.h"
#include "../if/ether_private.h"
#include "../os/msg_system.c"		/* Not a typo, see logger.h */
#include "ucode.h"
#include "ucode_hdr.h"
#include "../dev/flash_dvr_spec.h"
#include "../dev/flash_spec.h"
#include "slot.h"


/*
 * feip_init
 */
static void feip_init (subsystype *subsys)
{
    feip_platform_init(subsys);

    reg_add_show_controller_specific(FCI_FEIP_CONTROLLER, feip_show_cbus, 
				     "feip_show_cbus");

    reg_add_media_type_cmd(IDBTYPE_FEIP, feip_media_type_command,
		           "feip_media_type_command");

    /* Register for the duplex command */
    reg_add_media_half_or_full_duplex_cmd(IDBTYPE_FEIP, 
                                          feip_full_duplex_command,
                                         "feip_full_duplex_command");

    fast_ethernet_debug_init();
    fast_ethernet_parser_init();
}

/* 
 * convert_raw_eeprom_to_pa_info
 */
void convert_raw_eeprom_to_pa_info (hwidbtype *hwidb, void *pap)
{
    pa_info_slots *pa_info_ptr = (void *)pap;
    dgbus_eeprom_t *raw_eeprom, eeprom;
    charlong temp;
    ushort *sptr;
    int i;

    sptr = (ushort *)&eeprom;
    send_specific_cmd_1arg(hwidb, FCI_CMD_VERSION, FCI_PA_READ_RST);
    for (i = 0; i < 16; i++, sptr++)
        send_specific_cmd_1arg_sret(hwidb, FCI_CMD_VERSION,
                                      FCI_PA_EEPROM_READ, sptr);

    raw_eeprom = &eeprom;
    pa_info_ptr->eeprom_version = raw_eeprom->eeprom_version;
    pa_info_ptr->controller_type = raw_eeprom->controller_type;
    
    temp.d.byte[0] = temp.d.byte[1] = 0;
    temp.d.byte[2] = raw_eeprom->hardware_version_high;
    temp.d.byte[3] = raw_eeprom->hardware_version_low;
    pa_info_ptr->hardware_version = temp.d.lword;

    temp.d.byte[0] = raw_eeprom->serial_number_31_24;
    temp.d.byte[1] = raw_eeprom->serial_number_23_16;
    temp.d.byte[2] = raw_eeprom->serial_number_15_8;
    temp.d.byte[3] = raw_eeprom->serial_number_7_0;
    pa_info_ptr->serial_number = temp.d.lword;
    
    temp.d.byte[0] = raw_eeprom->part_number_31_24;
    temp.d.byte[1] = raw_eeprom->part_number_23_16;
    temp.d.byte[2] = raw_eeprom->part_number_15_8;
    temp.d.byte[3] = raw_eeprom->part_number_7_0;
    pa_info_ptr->part_number = temp.d.lword;

    pa_info_ptr->board_revision = raw_eeprom->board_revision;
}

/*
 * feip_full_duplex_command
 * Configure FastEthernet interface in full-duplex mode
 */
void feip_full_duplex_command (parseinfo *csb)
{
    hwidbtype *hwidb;
    idbtype *swidb;
    ethersb *esb;

    swidb = csb->interface;
    hwidb = swidb->hwptr;
    esb = idb_use_hwsb_inline(hwidb, HWIDB_SB_ETHER);

    if (csb->nvgen) {
        if (csb->which == CFG_HDX_SET_FULL_DUPLEX)
            nv_write(esb->feip_fullduplex, csb->nv_command);
    } 
    else {
        if (csb->sense) {          
            switch (csb->which) {
                case CFG_HDX_SET_FULL_DUPLEX:
                    esb->feip_fullduplex = TRUE;
                    break;
                case CFG_HDX_SET_HALF_DUPLEX:
                    esb->feip_fullduplex = FALSE;
                    break;
                default:
                    break;
            }
        }
        else {
            switch (csb->which) {
                case CFG_HDX_SET_FULL_DUPLEX:
                    esb->feip_fullduplex = FALSE;
                    break;
                case CFG_HDX_SET_HALF_DUPLEX:
                default:
                    break;
            }
        }
        (*hwidb->reset)(hwidb);
   }
   idb_release_hwsb_inline(hwidb, HWIDB_SB_ETHER);
}

/* 
 * feip_show_util
 */
void feip_show_util (hwidbtype *hwidb)
{
    leveltype level;
    ushort vpld_ver, rom_ver;

    /* Return if no usable interfaces */
    if (!hwidb) 
	return;

    vpld_ver = rom_ver = 0;
    
    level = raise_interrupt_level(NETS_DISABLE);
    
    select_if(hwidb);
    send_pld_version_cmd(hwidb, &vpld_ver);
    send_rom_version_cmd(hwidb, &rom_ver);
    
    reset_interrupt_level(level);
    
    printf("\n  FLASH ROM version %d.%d, VPLD version %d.%d", 
	   rom_ver  >> 8, rom_ver & 0xff,
	   vpld_ver >> 8, vpld_ver & 0xff);
}

void feip_media_type_command (parseinfo *csb)
{
    hwidbtype *hwidb = csb->interface->hwptr;
    boolean changed = TRUE;
    ethersb *esb;

    esb = idb_use_hwsb_inline(hwidb, HWIDB_SB_ETHER);
    if (csb->nvgen) {
	switch (GETOBJ(int,1)) {
	  case PARSER_MEDIATYPE_100BASET:
	    /* Default, No NV generation */
	    break;
	  case PARSER_MEDIATYPE_MII:
	    nv_write(esb->media_type == FEIP_MII, csb->nv_command);
	    break;
	  default:
	    bad_parser_subcommand(csb, GETOBJ(int,1));
	    break;
	}
	idb_release_hwsb_inline(hwidb, HWIDB_SB_ETHER);
	return;
    }

    /* If "no media-type 'param' ", reset to default */
    if (!csb->sense) 
        GETOBJ(int,1) = PARSER_MEDIATYPE_100BASET;

    switch (GETOBJ(int,1)) {
      case PARSER_MEDIATYPE_100BASET:
	if (esb->media_type == FEIP_RJ45)
	    changed = FALSE;
	else
	    esb->media_type = FEIP_RJ45;
	break;
      case PARSER_MEDIATYPE_MII:
	if (esb->media_type == FEIP_MII)
	    changed = FALSE;
	else
	    esb->media_type = FEIP_MII;
	break;
      default:
	bad_parser_subcommand(csb, GETOBJ(int,1));
	break;
    }
    if (changed)
	(*hwidb->reset)(hwidb);
    idb_release_hwsb_inline(hwidb, HWIDB_SB_ETHER);
}

/*
 * FEIP subsystem header
 */

#define FEIP_MAJVERSION   1
#define FEIP_MINVERSION   0
#define FEIP_EDITVERSION  1

SUBSYS_HEADER(feip, FEIP_MAJVERSION, FEIP_MINVERSION, FEIP_EDITVERSION,
	      feip_init, SUBSYS_CLASS_DRIVER,
	      "seq: dbus, cbus_mci",
	      "req: dbus, cbus_mci");

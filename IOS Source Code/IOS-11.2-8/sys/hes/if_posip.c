/* $Id: if_posip.c,v 3.1.2.2 1996/08/28 12:45:15 thille Exp $
 * $Source: /release/112/cvs/Xsys/hes/if_posip.c,v $
 *------------------------------------------------------------------
 * if_posip.c - POSIP interface support code
 *
 * January 1996, Robert Broberg
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_posip.c,v $
 * Revision 3.1.2.2  1996/08/28  12:45:15  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.1.2.1  1996/07/08  22:25:40  mwu
 * CSCdi60922:  11.2 Core Features Commit
 * Branch: California_branch
 *
 * Revision 3.1  1996/06/07  05:29:41  rbroberg
 * Add placeholders for new project
 *
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
#include "if_hes_common.h"
#include "logger.h"
#include "../if/network.h"
#include "packet.h"
#include "dbus.h"
#include "config.h"
#include "parser.h"
#include "../ui/debug.h"
#include "../snmp/sr_old_lchassismib.h"
#include "if_cmd.h"
#include "../os/msg_system.c"		/* Not a typo, see logger.h */
#include "if_posip.h"
#include "if_hes_serial.h"
#include "../wan/serial.h"
#include "../wan/serial_private.h"
#include "ucode.h"
#include "../dev/flash_dvr_spec.h"
#include "slot.h"

void posi_clock_cmd (parseinfo *csb)
{
    hwidbtype *hwidb;
    idbtype *swidb;
    serialsb *ssb;

    swidb = csb->interface;
    hwidb = swidb->hwptr;
    ssb = idb_use_hwsb_inline(hwidb, HWIDB_SB_SERIAL);

    if (csb->nvgen) {
	nv_write(ssb->posi_clock, csb->nv_command);
    } else {
	ssb->posi_clock = csb->sense;
	(*hwidb->reset)(hwidb);
    }
    idb_release_hwsb_inline(hwidb, HWIDB_SB_SERIAL);
}

void posi_framing_cmd (parseinfo *csb)
{
    hwidbtype *hwidb;
    idbtype *swidb;
    serialsb *ssb;

    swidb = csb->interface;
    hwidb = swidb->hwptr;
    ssb = idb_use_hwsb_inline(hwidb, HWIDB_SB_SERIAL);

    if (csb->nvgen) {
	nv_write(ssb->posi_framing, csb->nv_command);
    } else {
	ssb->posi_framing = csb->sense;
	(*hwidb->reset)(hwidb);
    }
    idb_release_hwsb_inline(hwidb, HWIDB_SB_SERIAL);

}

/*
 * posip_parse_love_letter:
 * Parse a love letter from an interface on a VIP
 */
void posip_parse_love_letter (hwidbtype *hwidb, uchar *love_ptr, ulong length)
{
    posip_ll *card_errors = NULL;
    serialsb *ssb;

    ssb = idb_use_hwsb_inline(hwidb, HWIDB_SB_SERIAL);

    card_errors = (posip_ll *)love_ptr;
    hwidb->counters.input_error += card_errors->pos_rx_errors; 
    hwidb->counters.input_crc +=  card_errors->pos_crc_errors;
    hwidb->counters.giants += card_errors->pos_giants;
    hwidb->counters.input_abort += card_errors->pos_aborts; 
    hwidb->counters.input_resource += card_errors->pos_ignores;
    hwidb->counters.runts += card_errors->pos_runts;

    idb_release_hwsb_inline(hwidb, HWIDB_SB_SERIAL);
}

/*
 * posip_init
 */
static void posip_init (subsystype *subsys)
{
    posip_platform_init(subsys);

    reg_add_show_controller_specific(FCI_POSIP_CONTROLLER, posip_show_cbus, 
				     "posip_show_cbus");
    posip_parser_init(); 
}

/* 
 * posip_show_util
 */
void posip_show_util (hwidbtype *hwidb)
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
    
    printf("\n    FLASH ROM version %d.%d, VPLD version %d.%d", 
	   rom_ver  >> 8, rom_ver & 0xff,
	   vpld_ver >> 8, vpld_ver & 0xff);
}

/*
 * POSIP subsystem header
 */

#define POSIP_MAJVERSION   1
#define POSIP_MINVERSION   0
#define POSIP_EDITVERSION  1

SUBSYS_HEADER(posip, POSIP_MAJVERSION, POSIP_MINVERSION, POSIP_EDITVERSION,
	      posip_init, SUBSYS_CLASS_DRIVER,
	      "seq: dbus, cbus_mci",
	      "req: dbus, cbus_mci");


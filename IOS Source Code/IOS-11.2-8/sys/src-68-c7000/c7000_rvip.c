/* $Id: c7000_rvip.c,v 3.2.62.2 1996/08/20 00:35:43 cheilek Exp $
 * $Source: /release/112/cvs/Xsys/src-68-c7000/c7000_rvip.c,v $
 *------------------------------------------------------------------
 * c7000_rvip.c: RP specific code for the RVIP
 *
 * August 1995, Stig Thormodsrud
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: c7000_rvip.c,v $
 * Revision 3.2.62.2  1996/08/20  00:35:43  cheilek
 * CSCdi58374:  More show diag info needed
 * Branch: California_branch
 * Add CCB_CMD_VIP_GET_CONFIG, vip_ll_sw.
 * Add sw_version_str to slot structure.
 *
 * Revision 3.2.62.1  1996/03/18  22:00:01  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.1  1996/01/24  23:02:10  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.2  1995/11/17  18:44:17  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:25:54  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:27:18  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/08/30  18:48:10  sthormod
 * placeholder for new development.
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "interface_private.h"
#include "packet.h"

#include "../ui/debug.h"
#include "../if/rvip_debug_flags.h"

#include "../hes/if_mci.h"
#include "../hes/if_fci.h"
#include "../hes/if_vip.h"
#include "../hes/if_cmd.h"
#include "../hes/ucode.h"
#include "../dev/flash_dvr_spec.h"
#include "../hes/slot.h"

/*
 * rvip_platform_init
 */
void rvip_platform_init (subsystype *subsys)
{
}

/*
 * rvip_show_cbus
 */
void rvip_show_cbus (ushort indx)
{
    mcitype *cbus;
    dcitype *dci;
    hwidbtype *hwidb;
    int jndx;    
    leveltype level;
    ushort vpld_ver, rom_ver;


    cbus = (mcitype *) cbusQ.qhead;
    dci = cbus->dci[indx];

    if (!dci)
	return;
    
    /*
     * Find the first interface on the RVIP
     */
    hwidb = NULL;
    for (jndx = 0; jndx < MAX_CTRLR_INTERFACES; jndx++) {
	hwidb = dci->interface_list[jndx];
	if (hwidb)
	    break;
    }

    if (!hwidb) 
	return;

    vpld_ver = rom_ver = 0;

    level = raise_interrupt_level(NETS_DISABLE);
    select_if(hwidb);
    send_pld_version_cmd(hwidb, &vpld_ver);
    send_rom_version_cmd(hwidb, &rom_ver);
    reset_interrupt_level(level);
    
    printf("\n  %s", slots[hwidb->slot].sw_version_str);
    printf("\n  FLASH ROM version %d.%d, VPLD version %d.%d", 
	   rom_ver  >> 8, rom_ver & 0xff,
	   vpld_ver >> 8, vpld_ver & 0xff);
}

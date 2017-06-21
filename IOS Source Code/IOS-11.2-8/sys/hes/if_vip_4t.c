/* $Id: if_vip_4t.c,v 3.8.20.1 1996/03/18 19:42:40 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/hes/if_vip_4t.c,v $
 *------------------------------------------------------------------
 * if_vip_4t.c -- cBus driver for 4T Serial port adapter
 *
 * October 1995 - Donald E. Lobete
 * 
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: if_vip_4t.c,v $
 * Revision 3.8.20.1  1996/03/18  19:42:40  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.7.2.3  1996/03/13  01:22:37  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.7.2.2  1996/02/20  00:52:48  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.8  1996/02/01  06:02:51  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.7.2.1  1996/01/24  21:47:32  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.7  1996/01/20  00:10:12  dlobete
 * CSCdi44907:  FEIP Interface counters are in error and mean-ing-less
 * Love-letter code path shared by both VIP and FEIP was only
 * working for the VIP.
 *
 * Revision 3.6  1996/01/11  03:50:34  kao
 * CSCdi43878:  IS_VIP() macro should be used
 *
 * Revision 3.5  1996/01/05  06:56:32  dlobete
 * CSCdi46484:  4T version numbers reported incorrectly
 * Correctly point to the pa_private_info structure when reporting
 * version numbers for "show cont cbus".
 *
 * Revision 3.4  1995/12/26  23:08:24  dlobete
 * CSCdi42282:  chassis mib incomplete for VIP
 * Add support for PA MIB information to be relayed from VIP to RP/RSP
 * via love-letter.
 * Rework some of the 4T PA's data structures as a result of above.
 *
 * Revision 3.3  1995/12/02  05:50:15  rharagan
 * CSCdi41966:  Messages between 4T PA & driver should be per interface
 * Move to single interface commands, incorporate code review comments.
 *
 * Revision 3.2  1995/11/17  09:19:06  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:41:19  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:01:07  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/10/17  07:17:08  dlobete
 * Placeholder for VIP_branch development.
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "master.h"
#include "interface_private.h"

#include "../dev/flash_dvr_spec.h"        /* needed for slot.h */
#include "ucode.h"			  /* needed for slot.h */
#include "../snmp/snmp_api.h"
#include "../snmp/snmp_registry.h"   

#include "if_fci.h"
#include "slot.h"

#include "if_vip.h"
#include "if_vip_4t.h"

/*
 * Show 4t-specific info when doing show controller.
 * NB: On a c7000 pass in a ctrlr_type corresponding to
 * the dci->ctrlr_type. On a c7500 pass in slotp->ctrlr_type.
 */
void cbus_vip_4t_show_controller (hwidbtype *hwidb, int ctrlr_type)
{
    pa_info_slots       *pa_info_ptr;
    vip4t_privdata_t *vip4t;
    slottype *slot_ptr = &slots[hwidb->slot];
    ushort 	pa_indx;

    if (!is_vip(hwidb))
	return;

    if (hwidb->type != IDBTYPE_CBUSSERIAL || 
	(ctrlr_type != FCI_RVIP_CONTROLLER && 
		ctrlr_type != FCI_SVIP_CONTROLLER))
	return;

    pa_indx = hwidb->slotunit < PA_VIRTUAL_SLOT_BOUNDARY ? 0 : 1;
    pa_info_ptr = slot_ptr->pa_info[pa_indx];
    if (!pa_info_ptr)
	return;
    vip4t = pa_info_ptr->private_pa_info;
    if (!vip4t)
	return;
    printf("\n   4T HW Revision %d, SW Revision %d, Unresponsive %d",
	   vip4t->s4t_pa_hw_version, vip4t->s4t_pa_sw_version,
	   vip4t->s4t_pa_is_dead_jim_total);

    return;
}

void parse_vip_4t_status_ll (hwidbtype *hwidb, uchar *love_ptr)
{
    vip_ll		*love_letter = (vip_ll *)love_ptr;
    vip_ll_errors 	*card_errors = &love_letter->data;
    vip4t_data_t 	*vip4t_info;
    vip4t_privdata_t 	*vip4t_privinfo;
    ushort      	pa_indx;
    slottype 		*slot = &slots[hwidb->slot];
    pa_info_slots 	*pa_info_ptr;

    /* interface-specific elements */
    vip4t_info = hwidb->vip4t_data;
    if (!vip4t_info)
        return;
    vip4t_info->signal_status = card_errors->s4t_sig_status;
    vip4t_info->applique_id = card_errors->s4t_applique_id;

    /* pa-specific elements */
    pa_indx = hwidb->slotunit < PA_VIRTUAL_SLOT_BOUNDARY ? 0 : 1;
    pa_info_ptr = slot->pa_info[pa_indx];
    if (!pa_info_ptr)
        return;
    vip4t_privinfo = pa_info_ptr->private_pa_info;
    if (!vip4t_privinfo)
        return;
    vip4t_privinfo->s4t_pa_sw_version = card_errors->s4t_pa_sw_version;
    vip4t_privinfo->s4t_pa_hw_version = card_errors->s4t_pa_hw_version;
    vip4t_privinfo->s4t_pa_is_dead_jim = card_errors->s4t_pa_is_dead_jim;

    if (vip4t_privinfo->s4t_pa_is_dead_jim) {
        vip4t_privinfo->s4t_pa_is_dead_jim_total++;
    }
}
      

/* $Id: if_ags_c7000_feip.c,v 3.3 1995/11/17 09:17:08 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/hes/if_ags_c7000_feip.c,v $
 *------------------------------------------------------------------
 * if_ags_c7000_feip.c: C7000 specific code for the Fast Ethernet
 *                      Interface Processor
 *
 * March 1995, Gary Kramling
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_ags_c7000_feip.c,v $
 * Revision 3.3  1995/11/17  09:17:08  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:27:06  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:39:37  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:00:13  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/08/09  00:19:37  kramling
 * CSCdi38439:  FEIP product is now supported in 11.0.  Update support to
 * 10.3.
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "interface_private.h"
#include "media_registry.h"
#include "if_mci.h"
#include "if_feip.h"
#include "if_vip.h"
#include "dbus.h"
#include "registry.h"
#include "cbus_registry.h"
#include "packet.h"
#include "../ui/debug.h"
#include "../if/fast_ethernet_debug.h"

/*****************************************************************************
 * Name: feip_platform_init
 *
 * Description: platform-specific FEIP init.
 *
 * Input: 
 *   subsys -- pointer to the sybsystype for this subsystem
 *
 * Return Value: None
 *
 *****************************************************************************/
void feip_platform_init (subsystype *subsys)
{
    reg_add_ether_sniff_encap(IDBTYPE_FEIP, vip_sniff_encap,	
			      "vip_sniff_encap");
}

/*****************************************************************************
 * Name: feip_show_cbus
 *
 * Description: Get FEIP specific info. for 'show controller cbus'
 *
 * Input: 
 *   indx -- slot index number for this controller
 *
 * Return Value: None
 *
 *****************************************************************************/
void feip_show_cbus (ushort indx)
{
    mcitype *cbus;
    dcitype *dci;
    hwidbtype *hwidb;
    int jndx;    

    cbus = (mcitype *) cbusQ.qhead;
    dci = cbus->dci[indx];

    if (!dci)
	return;
    
    /*
     * Find the first interface on the FEIP
     */
    hwidb = NULL;
    for (jndx = 0; jndx < dci->interface_count; jndx++) {
	hwidb = dci->interface_list[jndx];
	if (hwidb)
	    break;
    }
    if (hwidb) {
	feip_show_util(hwidb);
    }
}

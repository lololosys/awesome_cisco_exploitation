/* $Id: c7000_vip_4r.c,v 3.2.66.1 1996/08/13 02:23:19 gstovall Exp $
 * $Source: /release/112/cvs/Xsys/src-68-c7000/c7000_vip_4r.c,v $
 *------------------------------------------------------------------
 * c7000_vip_4r.c: RP specific code for the VIP 4R port adapter
 *
 * August 1995, Stig Thormodsrud
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: c7000_vip_4r.c,v $
 * Revision 3.2.66.1  1996/08/13  02:23:19  gstovall
 * CSCdi39484:  Giant packets output on ethernet interface
 * Branch: California_branch
 * Make sure we drop giants, even if they are just a wee bit too big.
 *
 * Revision 3.2  1995/11/17  18:44:22  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:25:57  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:27:20  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/10/17  05:35:38  sthormod
 * Placeholders for new development.
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "subsys.h"
#include "sys_registry.h"
#include "media_registry.h"
#include "interface_private.h"

#include "../hes/cbus_registry.h"
#include "../hes/if_fci.h"
#include "../hes/if_cmd.h"
#include "../hes/if_vip.h"
#include "../hes/if_vip_4r.h"


/*
 * vip_4r_platform_init
 */
void vip_4r_platform_init (subsystype *subsys)
{
    reg_add_ether_sniff_encap(IDBTYPE_CTR, vip_sniff_encap,
			      "vip_sniff_encap");
}


/*
 * vip_4r_restart_init
 */
void vip_4r_restart_init (hwidbtype *hwidb)
{
    send_assign_poolindex_cmd(hwidb, hwidb->buffer_pool);
    send_rxbytes_cmd(hwidb, hwidb->max_buffer_size);   
    send_rx_offset_cmd(hwidb, hwidb->rxoffset);
    send_rx_error_cmd(hwidb, FCI_RX_ERROR_POOL);
}


/*
 * platform_vip_4r_anybody_out_there
 */
ushort platform_vip_4r_anybody_out_there (hwidbtype *hwidb)
{
    ushort version;

    return send_sw_version_cmd(hwidb, &version);
}

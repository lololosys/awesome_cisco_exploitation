/* $Id: rsp_vip_4r.c,v 3.3.56.4 1996/08/13 02:24:32 gstovall Exp $
 * $Source: /release/112/cvs/Xsys/src-rsp/rsp_vip_4r.c,v $
 *------------------------------------------------------------------
 * rsp_vip_4r.c: RSP specific code for the VIP 4R port adapter
 *
 * August 1995, Stig Thormodsrud
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rsp_vip_4r.c,v $
 * Revision 3.3.56.4  1996/08/13  02:24:32  gstovall
 * CSCdi39484:  Giant packets output on ethernet interface
 * Branch: California_branch
 * Make sure we drop giants, even if they are just a wee bit too big.
 *
 * Revision 3.3.56.3  1996/06/03  17:47:07  getchell
 * CSCdi59155:  hot swap of 4R does not preserve config
 * Branch: California_branch
 *
 * Revision 3.3.56.2  1996/04/12  17:36:28  kao
 * CSCdi51744:  SegV execption - RSP crash wt. VIPs -FE/4E/4R/4T
 * Branch: California_branch
 *
 * Revision 3.3.56.1  1996/03/18  22:02:52  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.20.2  1996/03/14  01:26:17  gstovall
 * Branch: DeadKingOnAThrone_branch
 * The King is now ready for action.
 *
 * Revision 3.3.20.1  1996/02/20  18:01:59  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3  1995/11/29  02:48:01  sthormod
 * CSCdi42503:  4r needs modularity work
 * Clean up #ifdef C7000 kludge.
 *
 * Revision 3.2  1995/11/17  18:49:49  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:28:50  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:28:52  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/10/17  05:35:42  sthormod
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

#include "../if/tring.h"
#include "../if/tring_private.h"
#include "../hes/cbus_registry.h"
#include "../hes/if_hes_common.h"
#include "../hes/if_mci.h"
#include "../hes/if_fci.h"
#include "../hes/if_cmd.h"
#include "../hes/if_vip.h"
#include "../hes/if_vip_4r.h"
#include "../hes/ccb_cmd.h"
#include "../src-rsp/rsp_vip.h"
#include "../src-rsp/rsp_if.h"
#include "../src-rsp/rsp_pcmap.h"
#include "../if/network.h"

/*
 * vip_4r_platform_init
 */
void vip_4r_platform_init (subsystype *subsys)
{
    return;
}


/*
 * vip_4r_restart_init
 */
void vip_4r_restart_init (hwidbtype *hwidb)
{
    send_rxbytes_cmd(hwidb, hwidb->max_buffer_size);   
    send_rx_offset_cmd(hwidb, hwidb->rxoffset);
}


/*
 * rsp_vip_4r_idb_init
 */
void rsp_vip_4r_idb_init (hwidbtype *hwidb)
{
    /*
     * Protect ourselves in case anybody tries to reset us
     * before MEMD is carved:
     */
    hwidb->reset = (reset_t)return_nothing;
    hwidb->periodic = NULL;
    
    nTRs++;

    if (hwidb->status & IDB_NOHARDWARE) {
	hwidb->status &= ~IDB_NOHARDWARE;
	return;
    }

    hes_idb_init_before(hwidb);
    /*
     * RSP-specific VIP 4R Tokenring IDB initialization.
     */
    reg_add_media_transmit_delay(IDBTYPE_CTR,
				 cbus_set_transmit_delay,
				 "cbus_set_transmit_delay");
    hwidb->status = IDB_TR | FCI_IDBFLAGS;
    hwidb->firstsw->tbridge_media = TBR_CTR;
    hwidb->listen = (listen_t)return_nothing;

    /*
     * IDB initialization for all tring interfaces (high-end,
     * low-end, etc.)
     * Assume 16Mbps se we can go ahead and initialize this guy.  We
     * will set the actual ring speed once we have read it from the
     * configuration file, before we ever attempt to insert into the
     * ring.
     */
    tring_idb_init(hwidb, 16);
    
    hwidb->tr_ring_speed = 0;
    
    /*
     * RSP-specific VIP 4R tokenring IDB initialization.
     * XXX note, some of these are probably not RSP specific in
     * which case they can be moved to tring_idb_init()
     */
    hwidb->tr_srb_fastswitch = (tr_srb_fastswitch_type) return_false;

    hwidb->status |= IDB_VIP;
    cbus_vip_4r_idb_init(hwidb);

    if_maxdgram_adjust(hwidb, CTR_BUFFERSIZE_BYTES);
    hwidb->set_maxdgram = cbus_mci_set_maxdgram;
    hwidb->buffersize = (CTR_BUFFERSIZE_WORDS << 1);
    
    /*
     * Note, that the units (bytes Vs words) used in the rxoffset
     * field is platform dependant.  On the RSP, the value represents
     * bytes (on other platforms, like the 7000, this value 
     * represents words).  The IP ucode on each platform has been
     * modified to expect the correct units for that platform.
     *
     * XXX - Chris, optimize these for RSP cachelines.
     * XXX Also, what do we set span_ and rsrb_ stuff to on RSP?
     *     Where are the RSP bridging queens when you need 'em?
     */
    hwidb->rxoffset = RSP_TOKEN_BYTE_RECEIVE_OFFSET;
    hwidb->encsize = TRING_SNAP_ENCAPBYTES;
    hwidb->span_encapsize = 2;  /* 2 bytes of preamble before MAC */
    hwidb->span_macoffset = hwidb->rxoffset + hwidb->span_encapsize;
    hwidb->rsrb_vringoffset = hwidb->rxoffset + TRING_SNAP_ENCAPBYTES;
    hwidb->ip_memd_offset = hwidb->rxoffset + TRING_SNAP_ENCAPBYTES +
	ADRSPC_MEMD_WRITETHRU;

    hwidb->srb_routecache = TRUE;	/* Fast SRB on by default */
    
    hwidb->love_note = (void *)vip_rsp_love_letter;

    /*
     * Complete set up of common fields.
     */
    init_new_hwidb(hwidb);
    hes_idb_init_after(hwidb);
}


/*
 * platform_vip_4r_anybody_out_there
 */
ushort platform_vip_4r_anybody_out_there (hwidbtype *hwidb)
{
    ushort who_cares;

    return(send_version_cmd(hwidb->slot_ptr, CCB_VERSION_SW, &who_cares));
}

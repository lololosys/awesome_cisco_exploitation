/* $Id: dlsw_csm_dlx.c,v 3.6.20.2 1996/08/22 18:34:51 ravip Exp $
 *------------------------------------------------------------------
 * DLSw DLX Message Handler
 *
 * August 1994, Ravi Periasamy 
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Contains routines to handle message from peers talking cisco's
 * DLX, an extension to DLSw standard.
 *------------------------------------------------------------------
 * $Log: dlsw_csm_dlx.c,v $
 * Revision 3.6.20.2  1996/08/22  18:34:51  ravip
 * CSCdi66201:  DLSw load sharing broken
 * Branch: California_branch
 *
 * Revision 3.6.20.1  1996/05/17  10:45:15  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.6.6.2  1996/05/14  23:59:37  ioakley
 * CSCdi57686:  Streamline core_to_csm processing by replacing the
 *              core_to_csm function with inline calls to clsi/ssp/dlx
 *              handler routines from the dlsw_core.c module.
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.6.6.1  1996/04/26  07:34:00  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.6  1996/02/01  06:01:12  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.5  1996/01/12  17:31:25  ravip
 * CSCdi46357:  clean up dlsw csm debug messages
 *
 * Revision 3.4  1995/12/19  00:39:35  hampton
 * Obsolete the GET_NONZERO_TIMESTAMP routine.  It is no longer necessary
 * with 64bit timestamps.  [CSCdi45208]
 *
 * Revision 3.3  1995/11/17  09:02:46  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:21:29  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:23:20  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/11/08  20:55:14  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.3  1995/08/07  05:31:09  richl
 * CSCdi38315:  makefile rework
 *              add rif_util.c to libutil.a
 *              move common code to libibm_util.a
 *
 * Revision 2.2  1995/06/08  22:05:33  rnaderi
 * CSCdi35535:  lf support in dlsw does not use the right MTU.
 *
 * Revision 2.1  1995/06/07  20:26:34  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */



#include "master.h"
#include "interface_generic.h"
#include "sys_registry.h"
#include "media_registry.h"
#include "packet.h"
#include "logger.h"
#include "config.h"
#include "parser.h"
#include "../ui/debug.h"
#include "../util/fsm.h"
#include "../if/rif_util.h"
#include "../llc2/llc2.h"
#include "../srt/rsrb.h"
#include "../cls/clsi_msg.h"
#include "../dlsw/parser_defs_dlsw.h"
#include "../ibm/netbios.h"
#include "../ibm/libibm_externs.h"

#include "../dlsw/dlsw_dlx.h"
#include "../dlsw/dlsw_ssp.h"
#include "../dlsw/dlsw_peer_externs.h"
#include "../dlsw/dlsw_local_externs.h"
#include "../dlsw/dlsw_csm_externs.h"
#include "../dlsw/dlsw_core_externs.h"
#include "../dlsw/dlsw_csm_db.h"
#include "../dlsw/dlsw_csm.h"
#include "../dlsw/dlsw_debug.h"


/*
 * File Scope Function Prototypes
 */
static csm_rc_t dlx_fast_pass_proc (peerhandle_t, paktype *);


/*
 * SSP Message handler
 */
csm_rc_t csm_dlx_handler (peerhandle_t peer_handle, 
		          paktype *pak)
{

    csm_rc_t            rc;
    dlx_header_t        *dlx_hdr;

    dlx_hdr = (dlx_header_t *) pak->datagramstart;

    CSM_DEBUG("\nCSM: Received DLX message type = %s", 
             csm_dlx(dlx_hdr->message_type));
    switch (dlx_hdr->message_type) {
      case DLX_FAST_PASS:
        rc = dlx_fast_pass_proc(peer_handle, pak);
        break;
      default:
        CSM_DEBUG("\nDLX message type %c not known to CSM", dlx_hdr->message_type);
	rc = FRAME_REJ;
        break;
    }

    return rc;
}


/*
 * Process DLX_FAST_PASS messages
 */
static csm_rc_t dlx_fast_pass_proc (peerhandle_t ph, paktype *pak)
{
    long            mtu_size;
    char            rc_code;
    int             reach_index;
    dlx_fast_hdr_t  *fast_hdr;
    record_handle   rhandle;
    lcache_t        *cache_ptr;
    trrif_t         *trh;
    uchar           ws_mtu_size_in_rif;
    u_portid_type_t origin_prp_id;

    fast_hdr = (dlx_fast_hdr_t *) pak->datagramstart;
    trh  = (trrif_t *) (pak->datagramstart + sizeof(dlx_fast_hdr_t));
    mtu_size = rc_code_to_bytes(trh->rc_dlf & RC_LFMASK);

    /*
     * Get local cache info for destination mac address
     */
    get_mac_cache_entries(trh->daddr, &cache_ptr, &rhandle, NULL, NULL);
    if (!cache_ptr) {
        CSM_DEBUG("\nCSM: dlx_fast_pass_proc(): lcache_t not found");
        datagram_done(pak);
    } else if (cache_ptr->status != FOUND) {
        CSMV_DEBUG("\nCSM: dlx_fast_pass_proc(): status not found : %s",
                   csm_stat(cache_ptr->status));
        datagram_done(pak);
    } else {
        origin_prp_id.peer_id = ph;
        reach_index = get_reach_entry_mac(cache_ptr, mtu_size, 
                                          &origin_prp_id, TRUE);
        if (reach_index < 0) {
            CSME_DEBUG("\nCSM: Invalid port index %s", 
                        csm_error(reach_index));
            datagram_done(pak);
        } else {
            /* consumes pak */
            ws_mtu_size_in_rif = cache_ptr->r_entry[reach_index].rif[1];
            rc_code = bytes_to_rc_code(mtu_size);
            cache_ptr->r_entry[reach_index].rif[1] = 
                       (cache_ptr->r_entry[reach_index].rif[1] & ~RC_LFMASK) 
                              | rc_code;
            dlsw_fct_wan2lan_start(pak, ph,
                           cache_ptr->r_entry[reach_index].prp_id.port_id,
                           cache_ptr->r_entry[reach_index].rif);
            cache_ptr->r_entry[reach_index].rif[1] = ws_mtu_size_in_rif;
        }
    }
    return(PROC_OK);
}

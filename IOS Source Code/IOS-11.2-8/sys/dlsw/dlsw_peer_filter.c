/* $Id: dlsw_peer_filter.c,v 3.4.20.4 1996/09/09 17:40:15 fbordona Exp $
 * $Source: /release/112/cvs/Xsys/dlsw/dlsw_peer_filter.c,v $
 *------------------------------------------------------------------
 * DLSw Peer Capabilities and Output Access List Filtering Routines.
 *
 * August 1994, Frank Bordonaro 
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Routines facilitate input/output capabilities filtering, lsap
 * output access list filtering, and netbios host and bytes output
 * access list filtering for Data Link Switching.
 *------------------------------------------------------------------
 * $Log: dlsw_peer_filter.c,v $
 * Revision 3.4.20.4  1996/09/09  17:40:15  fbordona
 * CSCdi66251:  Filtering at DLSw layer will not work on border router
 * Branch: California_branch
 *
 * Revision 3.4.20.3  1996/08/28  12:41:15  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.4.20.2  1996/07/09  18:08:46  kmoberg
 * CSCdi46177:  access-expression output capabilities needs to be
 *              added to DLSw. Clean up remote peer config.
 * Branch: California_branch
 *
 * Revision 3.4.20.1  1996/05/17  10:46:24  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.4.6.1  1996/04/26  07:34:43  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.4  1996/02/01  06:01:38  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.3  1995/11/17  09:03:51  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:22:13  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:24:48  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/09/26  16:56:51  fbordona
 * CSCdi39990:  7k crashes for  DLSw+ with Direct transport,
 *              fast s/w and hdlc encap.
 *              Don't explore fast switched peers for non-TR lans.
 *              Remove dead code.
 *
 * Revision 2.1  1995/06/07  20:27:28  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "master.h"
#include "logger.h"
#include "../dlsw/msg_dlsw_peer.c"	/* Not a typo, see logger.h */
#include "interface_generic.h"
#include "sys_registry.h"
#include "media_registry.h"
#include "packet.h"
#include "config.h"
#include "access.h"
#include "parser.h"
#include "../dlsw/parser_defs_dlsw.h"
#include "../ip/ip.h"
#include "../ip/ip_registry.h"
#include "../tcp/tcpdriver.h"
#include "../tcp/tcp.h"
#include "../if/network.h"
#include "../llc2/llc2.h"
#include "../ui/debug.h"

#include "../ibm/netbios.h"
#include "../srt/srb.h"
#include "../srt/rsrb.h"
#include "../dlsw/dlsw_dlx.h"
#include "../dlsw/dlsw_ssp.h"
#include "../dlsw/dlsw_peer_externs.h"
#include "../dlsw/dlsw_peer.h"
#include "../dlsw/dlsw_debug.h"


/*
 * peer_cap_filter()
 *
 */

ulong peer_cap_filter (peerentry *peer, uchar ssap, uchar dsap)
{
    char buff[DLSW_MAX_PEER_ID];

    if (peer->cap.cap_known) {
        /*
         * Check if this peer can only reach the resources it told
         * us about (exclusive).
         */
        if (!supported_sap_check((peerhandle_t) peer, ssap, dsap)) {
            PDEBUG("\npeer_cap_filter(): Filtered by SAP to %s, s: %x d:%x",
                   peer_str(peer, buff), ssap, dsap);
            return(PEER_SAP_FAIL);
        }
    }
    return(PEER_PASS);
}

static boolean peer_mac_filter (peerentry *peer, uchar *dmac)
{
    if (dmac && !(is_ieee_zero(peer->dlsw_dmac_out))) {
	 if (!ieee_equal(peer->dlsw_dmac_out, dmac)) {
	      return(FALSE);
	 }
    }
    
    if (dmac && peer->dlsw_dmac_outlist) {
	 if (!access_check_mac(peer->dlsw_dmac_outlist, dmac)) {
	      return(FALSE);
	 }
    }
    return(TRUE);
}

/* 
 * peer_out_filter()
 *
 * Performs lsap, netbios host, and netbios bytes output access list
 * filter checking for a remote peer.
 */

boolean peer_out_filter (peerentry *peer, uchar dsap, uchar ssap, 
                         netbios_header *nbh, uchar *dmac)
{ 
    peer_lsap_u lsap_u;
    char buff[DLSW_MAX_PEER_ID];
 
    lsap_u.sap[0] = dsap;
    lsap_u.sap[1] = ssap;

    if (peer->dlsw_lsap_out) {
        if (!access_check_type(peer->dlsw_lsap_out, lsap_u.lsap)) {
            PDEBUG("\nDLSw: dsap(%x) ssap(%x) filtered to %s",
                   lsap_u.sap[0], lsap_u.sap[1], peer_str(peer, buff));
            return(FALSE);
        }
    }

    if (!peer_mac_filter(peer, dmac)) {
	PDEBUG("\nDLSw: dmac(%e) filtered to %s", dmac, peer_str(peer, buff));
	return(FALSE);
    }

    /*
     * Support for NETBIOS access filters
     * Must be LLC, UI, and NETBIOS frame for this check 
     * UI check is the responsibility of peer_out_check() caller.
     */
     if ((!nbh) || (dsap != SAP_NETBIOS))
         return(TRUE);

     if (peer->netbios_b_oacf) {
         if (!netbios_expr_bytes_check((uchar *) nbh, sizeof(netbios_header),
                                       peer->netbios_b_oacf)) {
             PDEBUG("\nDLSw: Dropping NetBIOS frame (bytes) for %s",
                    peer_str(peer, buff));
             return(FALSE);
         }
     }

     if (peer->netbios_h_oacf) {
         if (!netbios_expr_host_check((uchar *) nbh, sizeof(netbios_header),
                                       peer->netbios_h_oacf)) {
             PDEBUG("\nDLSw: NetBIOS names %s->%s filtered to %s",
                    nbh->srcname, nbh->destname, peer_str(peer, buff));
             return(FALSE);
         }
     }
     return(TRUE);
}

boolean pod_pass_mac_acl (peerhandle_t ph, char *mac_addr)
{
    peerentry	*peer;
    char buff[DLSW_MAX_PEER_ID];

    peer = (peerentry *)ph;
    if ((peer->peer_type != DEMAND_PEER) || (!mac_addr)) 
	return(TRUE);

    if (!peer_mac_filter(peer, mac_addr)) {
	PDEBUG("\nDLSw: smac(%e) filtered from pod %s",
	       mac_addr, peer_str(peer, buff));
	return(FALSE);
    }
    return(TRUE);
}

boolean pod_pass_nb_acl (peerhandle_t ph, netbios_header *orig_nbh)
{
    peerentry	*peer;
    netbios_header nbh;
    char buff[DLSW_MAX_PEER_ID];

    peer = (peerentry *)ph;
    if ((peer->peer_type != DEMAND_PEER) || (!peer->netbios_h_oacf))
	return(TRUE);

    /*
     * This one is ugly. Create a fake nb header to be checked
     * against the acl. Take srcname and destname from the original
     * pak, swap them and create a NQ. apply perr-on-demand-defaults
     * acl aginst it to check if the NQ would be forwarded via the
     * pod.
     * This routine should ONLY be used when receiving NR via a
     * border peer. Since the next netbios frame after NR would be
     * a SABME, the very first session would connect even if there
     * is an acl on the pod filtering fro the netbios name.
     */
    nbh.delimiter[0] = NETBIOS_DELIMITER_0;
    nbh.delimiter[1] = NETBIOS_DELIMITER_1;
    nbh.command = NETBIOS_NAME_QUERY;
    memcpy(nbh.srcname, orig_nbh->destname, NETBIOS_MAX_NAME_LEN);
    memcpy(nbh.destname, orig_nbh->srcname, NETBIOS_MAX_NAME_LEN);
    if (!netbios_expr_host_check((uchar *)&nbh, sizeof (nbh),
				 peer->netbios_h_oacf)) {
	PDEBUG("\nDLSw: netbios name %15s filtered from pod %s",
	       nbh.destname, peer_str(peer, buff));
	return(FALSE);
    }
    return(TRUE);
}

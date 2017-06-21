/* $Id: vpn.c,v 1.2.40.29 1996/09/12 04:02:14 tkolar Exp $
 * $Source: /release/112/cvs/Xsys/vpn/vpn.c,v $
 *------------------------------------------------------------------
 * vpn.c  --   the heart of VPN, glue between PPP and L2F
 *
 * January 1996, Shoou Yiu
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: vpn.c,v $
 * Revision 1.2.40.29  1996/09/12  04:02:14  tkolar
 * CSCdi62277:  L2F tunnel authentication over Radius fails
 * Branch: California_branch
 * Allow cleartext passwords to be passed from Radius
 *
 * Revision 1.2.40.28  1996/09/10  07:19:59  dblair
 * CSCdi62263:  Dialer profile with Multilink not working
 * Branch: California_branch
 * make multilink compatible with dialer profiles
 *
 * Revision 1.2.40.27  1996/08/23  20:38:22  syiu
 * CSCdi66741:  Multichassis or VPDN multilink broken
 * Branch: California_branch
 *
 * Revision 1.2.40.26  1996/08/22  07:20:30  perryl
 * CSCdi66647:  Can not tell what a virtual access interface is used for
 * Branch: California_branch
 * Add an addition parameter to reg_invoke_vtemplate_create_vaccess, to
 * describe the type of the new virtual access interface.
 *
 * Revision 1.2.40.25  1996/08/21  22:42:21  syiu
 * CSCdi66741:  Multichassis or VPDN multilink broken
 * Branch: California_branch
 *
 * Revision 1.2.40.24  1996/08/07  22:51:58  tkolar
 * CSCdi62172:  VPDN dual CHAP challenge from Home-Gateway
 * Branch: California_branch
 * Merge two disparate VPDN authentication schemes.
 *
 * Revision 1.2.40.23  1996/07/25  16:50:28  syiu
 * CSCdi63878:  VPDN CHAP challenge size less than default not working
 * Branch: California_branch
 *
 * Revision 1.2.40.22  1996/07/23  13:30:39  thille
 * CSCdi63329: Centralize logging for getbuffer() and pak_duplicate()
 * failures
 * Branch: California_branch
 * Shamelessly rip off Mr. Katz' code from CSCdi36037.  Provide
 * centralized logging for getbuffer() and pak_duplicate() failures and
 * then rip out the 9 bazillion different messages that where in the
 * various callers to report the failure.
 *
 * Revision 1.2.40.21  1996/07/17  00:27:39  syiu
 * CSCdi63178:  VPDN/Multichassis MLP leaving stray Vaccess interfaces
 * around
 * Branch: California_branch
 *
 * Revision 1.2.40.20  1996/07/15  16:10:10  syiu
 * CSCdi62356:  Bundle interfaces on reused Vaccess int sometimes dont
 * route
 * Branch: California_branch
 *
 * Revision 1.2.40.19  1996/07/10  00:47:03  syiu
 * CSCdi62375:  VPDN message should be more helpful when no vtemplate
 * defined
 * Branch: California_branch
 *
 * Revision 1.2.40.18  1996/07/08  16:55:26  syiu
 * CSCdi62169:  VPDN PPP authentication failure could be clearer
 * Branch: California_branch
 *
 * Revision 1.2.40.17  1996/07/08  15:58:00  syiu
 * CSCdi62107:  Multichassis config conflict with Dialer
 * Branch: California_branch
 *
 * Revision 1.2.40.16  1996/07/01  21:28:56  syiu
 * CSCdi61806:  Multichassis MLP and VPDN should coexist
 * Branch: California_branch
 *
 * Revision 1.2.40.15  1996/06/29  05:10:58  syiu
 * CSCdi60955:  BRI NAS drops VPDN connection on idle timeout
 * Branch: California_branch
 *
 * Revision 1.2.40.14  1996/06/29  00:46:24  syiu
 * CSCdi61704:  Line protocol not up on VPDN projected interfaces
 * Branch: California_branch
 *
 * Revision 1.2.40.13  1996/06/25  21:40:26  syiu
 * CSCdi59542:  Virtual-Access1: dropped, not authenticated.  Protocol =
 * 0x8021
 * Branch: California_branch
 *
 * Revision 1.2.40.12  1996/06/22  07:38:59  syiu
 * CSCdi59165:  PPP Virtual-Access Int will not negotiate NCPs on
 * projected links
 * Branch: California_branch
 *
 * Revision 1.2.40.11  1996/06/21  16:33:56  tkolar
 * CSCdi60998:  tunnel-password is obsolete attribute for VPDN
 * Branch: California_branch
 *
 * Revision 1.2.40.10  1996/06/19  20:37:24  fox
 * CSCdi59847:  Dynamic dialer maps are not cleared on async dialer ints
 * Branch: California_branch
 * Call reg_invoke_ipcp_cleanup_ip_peer() to remove the neighbor route,
 * the dynamic dialer map, and any other mechanisms tied to the current
 * peer address before clearing the peer address.
 *
 * Revision 1.2.40.9  1996/06/07  23:02:49  tkolar
 * CSCdi59923:  Unnecessary delay in bringing up L2F virtual interfaces
 * Branch: California_branch
 *
 * Revision 1.2.40.8  1996/06/05  00:02:26  tkolar
 * CSCdi55297:  Document L2F VPDN A/V pairs
 * Branch: California_branch
 * Replace magic strings with defines.
 *
 * Revision 1.2.40.7  1996/05/21  10:08:49  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 1.2.40.6  1996/05/12  19:22:21  tkolar
 * CSCdi57472:  vpn is an overloaded acronym
 * Branch: California_branch
 * Change "vpn" to "vpdn", and make a few other user interface tweaks
 * while we're at it.
 *
 * Revision 1.2.40.5  1996/05/12  18:06:17  tkolar
 * CSCdi57470:  ipwrite should still not be called at interrupt
 * Branch: California_branch
 * Correct CSCdi56324 .
 *
 * Revision 1.2.40.4  1996/05/06  21:10:39  syiu
 * CSCdi54907:  virtual-access interface counters show 0 bytes output
 * Branch: California_branch
 *
 * Revision 1.2.40.3  1996/05/06  18:53:07  perryl
 * CSCdi56515:  Virtual Access IP unnumbered is intrusive
 * Branch: California_branch
 * Virtual Template and Access interfaces are default to "no ip address"
 * when
 * created. Creating virtual access intf with "ip unnumbered" is now an
 * option.
 * For MLP, if there isn't a dialer interface associated with an incoming
 * call,
 * it will create a virtual access interface as the bundle interface with
 * the
 * "ip unnumbered" options. In most cases, the default "no ip address" is
 * used.
 * In selecting "ip unnumbered" source, it will first try to pick up
 * ethernet,
 * token ring, FDDI, ATM, serial and it will not pick dialer, virtual
 * template
 * and virtual access interfaces.
 *
 * Revision 1.2.40.2  1996/04/27  06:34:22  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 1.2.2.28  1996/04/25  23:56:45  syiu
 * Branch: Dial1_branch
 * Dial1_branch to IosSec_postmerge_960419
 *
 * Revision 1.2.2.27  1996/04/19  14:32:20  syiu
 * Branch: Dial1_branch
 * Add hwif_state_change to monitor interface downs on projected links.
 * Modified debug messages to indicate direction of VPN/L2F packets
 *
 * Revision 1.2.2.26  1996/04/16  23:58:23  syiu
 * Branch: Dial1_branch
 * Sync Dial1_branch to V111_1_3
 *
 * Revision 1.2.2.25  1996/04/10  05:15:52  tkolar
 * Branch: Dial1_branch
 * Fix CSC.wan bugs CSCdi54175, CSCdi54149, CSCdi54150, CSCdi54153,
 * and CSCdi54167
 *
 * Revision 1.2.2.24  1996/04/08  18:07:48  syiu
 * Branch: Dial1_branch
 * Synchronize Vaccess interface state changes. Otherwise route adjusts etc
 * won't work correctly.
 *
 * Revision 1.2.2.23  1996/03/29  18:20:45  syiu
 * Branch: Dial1_branch
 * VPN reset to mark interface down only for Virtual Access Interfaces
 *
 * Revision 1.2.2.22  1996/03/25  07:55:39  syiu
 * Branch: Dial1_branch
 * Set up the chapstate id to be the received id from L2F
 *
 * Revision 1.2.2.21  1996/03/25  05:32:14  tkolar
 * Branch: Dial1_branch
 * Enable Tunnel authentication.
 *
 * Revision 1.2.2.20  1996/03/24  01:44:26  tkolar
 * Branch: Dial1_branch
 * Enable AAA forwarding of VPN sessions, and complete
 * authentication of MIDs.  Also update a few parts of the
 * protocol to match changes in the RFC.
 *
 * Revision 1.2.2.19  1996/03/23  06:41:07  tkolar
 * Branch: Dial1_branch
 * Allow AAA to store VPN forwarding records.
 *
 * Revision 1.2.2.18  1996/03/23  01:45:57  tkolar
 * Branch: Dial1_branch
 * Stub out some half-working authentication.
 *
 * Revision 1.2.2.17  1996/03/22  23:12:07  tkolar
 * Branch: Dial1_branch
 * Use malloc'd memory for stack name and tunnel info.
 *
 * Revision 1.2.2.16  1996/03/22  05:34:20  tkolar
 * Branch: Dial1_branch
 * Reshape the way MLPVT interacts with VPN.
 *
 * Revision 1.2.2.15  1996/03/22  04:33:13  syiu
 * Branch: Dial1_branch
 * REenable local end-to-end local L2F
 *
 * Revision 1.2.2.14  1996/03/21  23:51:46  tkolar
 * Branch: Dial1_branch
 * Add support for multiple incoming tunnels, fix a crash in the resend
 * code, and add the beginnings of true authentication.
 *
 * Revision 1.2.2.13  1996/03/16  16:43:57  syiu
 * Branch: Dial1_branch
 * Create/Delete L2F tunnels from MLPVT. Also move code into
 * L2F create/delete tunnel routines
 *
 * Revision 1.2.2.12  1996/03/10  19:24:53  syiu
 * Branch: Dial1_branch
 * Add support for Stackability vtemplate distinct from VPN
 *
 * Revision 1.2.2.11  1996/03/07  21:52:42  tkolar
 * Branch: Dial1_branch
 * Change the expected tacacs+ syntax.
 *
 * Revision 1.2.2.10  1996/03/03  20:56:25  tkolar
 * Branch: Dial1_branch
 * Turn off local mode by default, and commit (but don't enable) the tacacs
 * code.  Also, send full username when starting l2f session.
 *
 * Revision 1.2.2.9  1996/02/25  05:25:00  tkolar
 * Branch: Dial1_branch
 * Only do buginfs when debug variable is set.
 *
 * Revision 1.2.2.8  1996/02/25  05:18:57  tkolar
 * Branch: Dial1_branch
 *
 * Revision 1.2.2.7  1996/02/17  22:59:03  tkolar
 * Branch: Dial1_branch
 * Checkpoint a pingable VPN.
 *
 * Revision 1.2.2.6  1996/02/13  23:07:48  tkolar
 * Branch: Dial1_branch
 * Checkpoint VPN.
 *
 * Revision 1.2.2.5  1996/02/09  19:11:25  tkolar
 * Branch: Dial1_branch
 * Fix compile error.  Whups...
 *
 * Revision 1.2.2.4  1996/02/08  23:20:05  tkolar
 * Branch: Dial1_branch
 * Checkpoint VPN integration
 *
 * Revision 1.2.2.3  1996/02/08  06:51:12  syiu
 * Branch: Dial1_branch
 * Tie together MLPVT/MLP/VPN registries, add vtemplate
 * support in MLPVT
 *
 * Revision 1.2.2.2  1996/02/06  20:37:09  syiu
 * Branch: Dial1_branch
 * L2F enhancements for PPP
 *
 * Revision 1.2.2.1  1996/02/04  06:30:06  syiu
 * Branch: Dial1_branch
 * Support for MLP Bundle interfaces
 *
 * Revision 1.2.40.1  1996/03/18  22:46:13  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 1.2.14.1  1996/03/07  11:06:16  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 1.2  1996/01/31  18:38:38  tkolar
 * Branch: Dial1_branch
 * Get all of the VPN code into the repository, with the right names
 * and in the right places.
 *
 * Revision 1.1  1996/01/29  18:09:11  tkolar
 * Populate the VPN directory.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "fastswitch_registry.h"
#include "../h/cs_registry.regh"
#include "interface_private.h"
#include "ieee.h"
#include "config.h"
#include "parser.h"
#include "packet.h"
#include "sys_registry.h"
#include "../h/auth_registry.regh"
#include "media_registry.h"
#include "../wan/dialer_registry.h"
#include "../wan/ppp_registry.h"
#include "../mlpvt/mlpvt_registry.regh"
#include "subsys.h"
#include "address.h"
#include "logger.h"
#include "ttysrv.h"
#include "../util/md5.h"
#include "../os/boot.h"
#include "../if/network.h"
#include "../ip/ip_registry.h"
#include "../ip/ip_addrpool.h"
#include "../ip/ip.h"
#include "../vpn/vpn.h"
#include "../wan/ppp.h"
#include "../wan/ppp_auth.h"
#include "../if/if_vtemplate.h"
#include "../ui/debug.h"
#include "../vpn/vpn_debug.h"
#include "../vpn/l2f.h"
#include "../os/os_debug_flags.h"
#include "../h/aaa.h"
#include "../parser/parser_actions.h"
#include "../wan/dialer.h"
#include "../les/isdn_bri.h"
#include <ciscolib.h>
#include "../os/pool.h"

static const int local_test = FALSE;	/* Never set, init to FALSE */

boolean vpn_force_local_chap;
ipaddrtype vpn_source_ip_address;

/*
 * Function prototypes
 */
boolean vpn_put_ipaddress(vpntype *, ipaddrtype, int, int);
static ipaddrtype vpn_get_ipaddress(vpntype *);
int vpn_get_status(vpntype *);
static void vpn_free_handle(vpntype *);
static boolean vpn_BLOCK(vpntype *);
static boolean vpn_nointBLOCK(vpntype *);
static forktype vpn_process(vpntype *);
static boolean vpn_forward_user(hwidbtype *, vpntype *);
static void vpn_bind_interface(hwidbtype *, int);
static void vpn_unbind_interface(hwidbtype *);
static void vpn_raw_enqueue(hwidbtype *, paktype *);
static boolean vpn_holdq_enqueue(hwidbtype *, paktype *, enum HEADTAIL);
static void vpn_end_to_end(hwidbtype *, hwidbtype *);     /* TEMP */
static int vpn_create_mid(hwidbtype *, vpntype *, char *, uchar, uchar, uchar, 
		uchar *, uchar, uchar *, ulong, short, uchar *, short, uchar *);
static boolean vpn_ppp_packet_iqueue(ushort, uchar, uchar, paktype *, short, 
hwidbtype *);
static boolean vpn_action(vpntype *, char *, boolean);
static void vpn_cleanup(hwidbtype *);
boolean vpn(char *, hwidbtype *, paktype *, int);
static boolean do_vpn_author(userstruct *, vpntype *);

extern vpn_parser_init(void);

/* Simulated MLPVT external routines */
static boolean mlpvlt_query_lookup(char *, vpntype *);
void mlpvlt_query_cancel(char *, vpntype *);
static forktype mlpvlt_process (vpntype *);
static boolean mlpvlt_BLOCK(vpntype *);
boolean mlpvlt_notify_process;


void chap_ssuccess(hwidbtype *, int);



static boolean vpn_parse_ip_address(char *buf, addrtype *addrp, int *pi)
{
    ushort num[4];
    boolean matched = FALSE;

    if (match_decimal(buf, 0, 255, pi, &num[0]) &&
        match_char(buf, pi, '.') &&
        match_decimal(buf, 0, 255, pi, &num[1]) &&
        match_char(buf, pi, '.') &&
        match_decimal(buf, 0, 255, pi, &num[2]) &&
        match_char(buf, pi, '.') &&
        match_decimal(buf, 0, 255, pi, &num[3])) {

        addrp->ip_addr = ((((ulong) num[0]) << 24) |
                          (((ulong) num[1]) << 16) |
                          (((ulong) num[2]) << 8)  |
                          ((ulong) num[3]));
        matched = TRUE;
    }

    if (matched) {
	return(TRUE);
    } else {
	return(FALSE);
    }

    return(FALSE);
}

static ipaddrtype vpn_parse_ip_addresses(char *buf)
{
    char *ptr;
    addrtype addrp[16];
    int addrwhich = 0;
    int count;

    ptr = buf;

    for (; *ptr != 0; ptr++) {
	if (*ptr == ' ')
	   continue;
	count = 0;
	if (vpn_parse_ip_address(ptr, &addrp[addrwhich], &count)) {
	    addrwhich++;
	    ptr += count;
	} else {
	    if (vpn_error_debug)
		buginf("\nFailed to parse IP address");
	    return(0);
	}
    }

    return(addrp[0].ip_addr);
    
}

static boolean vpn_query_lookup(char *name, vpntype *vpn_handle)
{
    l2f_tunneltype *tunnel;
    l2f_tunnel_infotype *info;
    char *ptr;
    userstruct *ustruct = NULL;
    boolean ret;

    ptr = strchr(name, '@');

    if (ptr) {
	ptr++;
        if (vpn_event_debug) {
	    buginf("\nlooking for tunnel -- %s --", ptr);
	}

	tunnel = l2f_find_active_tunnel_by_name(ptr);
	if (!tunnel) {
	    info = l2f_find_tunnel_by_name(ptr);
	    if (info) {
		tunnel = l2f_make_tunnel_structure(info);
	    }
	}
	if (tunnel) {
	    vpn_put_ipaddress (vpn_handle, tunnel->ip_address, VPN_FORWARD, 0);
	    vpn_handle->tunnel = tunnel;
	    return(TRUE);
	} 

	ret = aaa_create_user(&ustruct, ptr, NULL, NULL, NULL,
			  AAA_AUTHEN_TYPE_NONE, AAA_SVC_LOGIN,
			  0);
	if (!ret) {
	    buginf("\nFailed to create AAA user in VPN.");
	    return(FALSE);
	}

	/*
	 * Just in case we're doing radius
	 */
	ustruct->password = strdup_named("cisco", "Ppass");

	/* Set this flag so that if radius needs to make an authentication
	 * call to get this authorization information, it knows this is a
	 * special kind of authentication.
	 */
	ustruct->authen_config = TRUE;

	do_vpn_author(ustruct, vpn_handle);

	aaa_free_user(&ustruct);

	if (vpn_handle->tunnel) {
	    return(TRUE);
	}
    }
    return(FALSE);
}

/*
 * Check if VPN is enabled
 */
boolean vpn_enabled (void)
{
  return(l2f_enable);
}


/*
 * vpn
 * Will initiate the link to be projected to destination. 
 * The first component is to query for the destination. 
 * Having obtained the destination address, the forwarding component is 
 * called to initiate the actual link projection. 
 * The Query component will query sub-modules - VPN or SGMP -depending  
 * on some precedence rules. 
 * Return 
 *   TRUE - if link projection is being initiated. Note that this 
 *          does NOT mean the link has been projected.   
 *   FALSE - if link projection cannot be initiated.
 */
boolean vpn (char *caller_name, hwidbtype *link_hwidb, paktype *pak, 
int authen_type)
{


   vpntype *vpn_handle;
   char *stackname;

   /* 
    *  If L2F isn't active, there's no point in going any further.
    */
   if (!l2f_enable) 
      return(FALSE);

   /*
    * A Virtual interface cannot be iteratively forwarded
    */
   if ((link_hwidb->type == IDBTYPE_VACCESS) ||
       (link_hwidb->type == IDBTYPE_VTEMPLATE)) {
      if (vpn_error_debug)
         buginf("\n%s VPN vpn Virtual interface iterate", 
                link_hwidb->hw_namestring);
      return(FALSE);
   }

   /*
    * If the LCP state is already in the FORWARDING or FORWARDED state, don't
    * query anymore. Silently discard the pak
    */
   if (reg_invoke_ppp_lcpstate(link_hwidb, FORWARDING)
       || reg_invoke_ppp_lcpstate(link_hwidb, FORWARDED)) {
      if (vpn_event_debug)
         buginf("\n%s VPN - LCP state already FORWARDING/FORWARDED, silent discard", 
                link_hwidb->hw_namestring);
      datagram_done(pak);
      return(TRUE);
   }

   /*
    * Set the LCP state to FORWARDING
     */
   if (!reg_invoke_ppp_set_lcpstate(link_hwidb, FORWARDING)) {
     /*
      * Doesn't happen unless something really wrong
      */
     if (vpn_error_debug)
         buginf("\n%s vpn unexpected error - LCP state non-existent", 
                link_hwidb->hw_namestring);      
     return(FALSE);
   }

   /*
    * Create a handle
    */
   vpn_handle = malloc_named(sizeof(vpntype), "VPN handle");
   if (!vpn_handle)
      return(FALSE);

   /*
    * Mark the pak so that if VPN completes asynchronously, 
    * and we send it back into chap_input or pap_input, we won't
    * end up in here again
    */
   pak->flags |= PAK_CONTROL;

   /*
    * Set up the handle
    */
   vpn_handle->status = VPN_PENDING;
   vpn_handle->link_hwidb = link_hwidb;
   vpn_handle->pak = pak;
   vpn_handle->authen_type = authen_type;
   queue_init(&vpn_handle->ipaddressQ, 0);

   /*
    * If Local test, proceed to forward the link locally
    */
   if (local_test) {
       ipaddrtype source_ip;

      stackname = malloc(MAXSTR+1);
      reg_invoke_mlpvt_find_stackname(stackname, MAXSTR, &source_ip);
      if (stackname[0] == 0) {
         buginf("\nLocal Test failing on no stackname defined");
         reg_invoke_ppp_set_lcpstate(link_hwidb, AUTHENTICATING);
         vpn_handle->pak = NULL;
         pak->flags &= ~PAK_CONTROL;
         vpn_free_handle(vpn_handle);
         (link_hwidb->reset)(link_hwidb);
         return(TRUE);
      }
      mlpvlt_query_lookup(caller_name, vpn_handle);
      vpn_action(vpn_handle, caller_name, FALSE);
      return(TRUE);
   }

 
   /*
    * Call VPN ...
    */

   if (vpn_query_lookup(caller_name, vpn_handle)) {

      if (vpn_action(vpn_handle, caller_name, FALSE))
         return(TRUE);

   } else if (reg_invoke_mlpvt_query_lookup(caller_name, vpn_handle)) {
       /*
	* Called MLPVLT to query for remote stack member containing 
	* the bundle.
	*/

      /*
       * Perform action according to vpn_handle status returned.
       * If TRUE is returned (below), the link is either being forwarded or a 
       * process is being spawned to take care of asynchronous completion.
       * In the former case, the pak and vpn_handle are deleted (since it is 
       * not necessary anymore). In the latter case, neither is deleted. 
       * The spawned process takes care of that.
       * If FALSE is returned (DONT_FORWARD or some error condition), we 
       * return control to PPP. Only the handle is deleted.
       */
      if (vpn_action(vpn_handle, caller_name, FALSE))
         return(TRUE);
   }

   /*
    * Reset the LCP state to AUTHENTICATING and erase pak 
    * pointer since we want it to go thru normal PPP path on return
    */
   reg_invoke_ppp_set_lcpstate(link_hwidb, AUTHENTICATING);
   vpn_handle->pak = NULL;
   pak->flags &= ~PAK_CONTROL;
   vpn_free_handle(vpn_handle);
   return(FALSE);
}


/*
 * vpn_process
 * Handle asynchronous completion from query sub-component (MLPVLT, VPN)
 */ 
static forktype vpn_process (vpntype *vpn_handle)
{
  auth_state *chapstate;

  chapstate = vpn_handle->link_hwidb->authstate;

  /*
   * Block waiting for status to be non-VPN_PENDING
   */
  edisms((blockproc *)vpn_BLOCK, (ulong)vpn_handle);

  /*
   * If the interface not up, cancel the outstanding query
   */
   if (!interface_up_simple(vpn_handle->link_hwidb->firstsw)) {

     /*
      * If query is still outstanding, 
      * cancel the query and block again 
      */
     if (vpn_handle->status == VPN_PENDING) {
        if (vpn_event_debug)
            buginf("\n%s VPN Link down, Canceling Query...", 
                vpn_handle->link_hwidb->hw_namestring);
        reg_invoke_mlpvt_query_cancel(chapstate->remote_name, vpn_handle);
        edisms((blockproc *)vpn_nointBLOCK, (ulong)vpn_handle);
     }
     if (vpn_event_debug)
        buginf("\n%s VPN Link down, Query Cancelled", 
                vpn_handle->link_hwidb->hw_namestring);
     vpn_free_handle(vpn_handle);
     process_kill(THIS_PROCESS);
   }

   /*
    * Perform action according to vpn_handle status returned.
    * If the link to be forwarded, the pak and vpn_handle are deleted.
    * Else if DONT_FORWARD or some error condition, call PPP to continue
    * local processing of it. Delete the vpn_handle only. The pak is 
    * datagram_done inside PPP.  
    */
   vpn_action(vpn_handle, NULL, TRUE);

   vpn_free_handle(vpn_handle);
   process_kill(THIS_PROCESS);
}


/*
 * vpn_action
 * Perform appropriate action based on handle status. 
 */
static boolean vpn_action (vpntype *vpn_handle, char *caller_name, 
boolean async_completion)
{
   ipaddrtype ip_address; 
   l2f_tunnel_infotype *info;
   l2f_tunneltype *tunnel;
   int pid;
   char *stackname;

   switch (vpn_handle->status) {

     case VPN_FORWARD:
          /*
           * Do VPN forwarding chores. 
           * Get the IP address from the handle and 
           * call the VPN forwarding component
           */
          if (vpn_event_debug)
            buginf("\n%s VPN Forwarding...", 
                vpn_handle->link_hwidb->hw_namestring);

          if (local_test && vpn_event_debug)
            buginf("(LOCAL)");

   	  info = malloc(sizeof(l2f_tunnel_infotype));
	  stackname = malloc(MAXSTR+1);
	  if (!info || !stackname) {
	      free(info);
	      free(stackname);
	      return(FALSE);
	  }
	  bzero(stackname, MAXSTR+1);

          ip_address = vpn_get_ipaddress(vpn_handle);

	  if (!vpn_handle->tunnel) {
	  /*
	   * if we made it this far without a tunnel, we're doing
	   *  stackability
	   */

	     /*
	      *   If there is already an active tunnel to this address,
	      *    make use of it.
	      */
	      tunnel = l2f_find_active_tunnel_by_address(ip_address);

	      /*
	       *   There is no tunnel, we're going to have to make one.
	       */
	      if (!tunnel) {

                 /*
                  * Get the Stack name. If one is not found at this 
                  * point, it is abnormal.
                  */
	         reg_invoke_mlpvt_find_stackname(stackname, MAXSTR, 
						 &info->source_ip);
		 if (stackname[0] != 0) {
		    info->direction = L2F_OUTGOING;
		    info->type = VPDN_MULTICHASSIS;
		    strcpy(info->nas_name, stackname);
		    strcpy(info->gw_name, stackname);
		    info->ip_address = ip_address;
		    if (!info->source_ip)
			info->source_ip = -1;
		    tunnel = l2f_make_tunnel_structure(info);
		 } else if (vpn_error_debug)
		    buginf("\n%s No Stackname was defined", 
			  vpn_handle->link_hwidb->hw_namestring); 
	      }

	      if (!tunnel) {
		  if (vpn_error_debug)
		      buginf("\n%s Couldn't build VPN tunnel for %s", 
		       vpn_handle->link_hwidb->hw_namestring, info->nas_name); 
		  (*vpn_handle->link_hwidb->reset)(vpn_handle->link_hwidb);

		  if (!async_completion)
                     vpn_free_handle(vpn_handle);

                  free(info);
	          free(stackname);
                  return(FALSE);
	      }
	      vpn_handle->tunnel = tunnel;
	  }


          if (!vpn_forward_user(vpn_handle->link_hwidb, vpn_handle)) {
              if (vpn_error_debug)
                  buginf("\n%s VPN Forwarding failed", 
                        vpn_handle->link_hwidb->hw_namestring);             
              (*vpn_handle->link_hwidb->reset)(vpn_handle->link_hwidb);
	  }
          
          
          /*
           * If Synchronous completion, free the handle
           */
          if (!async_completion)
             vpn_free_handle(vpn_handle);

          free(info);
	  free(stackname);
          return(TRUE);

     case VPN_DONT_FORWARD:
          /*
           * Bundle is local - don't forward the link
           */
          if (vpn_event_debug)
            buginf("\n%s VPN Not Forwarded", 
                   vpn_handle->link_hwidb->hw_namestring);

          /*
           * If completion is synchronous, return immediately. The caller
           * path is from PPP Manager. PPP will continue its non-VPN forwarding
           * operation - i.e. PPP operation continues locally. 
           * If the completion is asynchronous, the caller is the VPN 
           * process (not the PPP Manager). We will need to reroute the 
           * logic back to PPP to continue its normal operation. 
           */
          if (!async_completion)
             break;

          /*
           * Continue PPP operation, 
           * PPP will datagram_done the pak.  
           */
          reg_invoke_ppp_set_lcpstate(vpn_handle->link_hwidb, AUTHENTICATING);
          reg_invoke_ppp_continue_vpn(vpn_handle->authen_type,vpn_handle->pak, 
                           vpn_handle->link_hwidb); 
          vpn_handle->pak = NULL;         
          break;
       
     case VPN_PENDING:
          /*
           * Applies ONLY on a synchronous completion.
           * Query is being carried out - its completion to be asynchronous. 
           * Spawn a process to handle the completion
           */
          if (async_completion) {
             buginf("\n%s VPN Illegal completion", 
                    vpn_handle->link_hwidb->hw_namestring); 
             return(FALSE);
	  }

          if (vpn_event_debug)
            buginf("\n%s VPN Pending", 
                vpn_handle->link_hwidb->hw_namestring);

          /*
           * Spawn process to handle completion
           */
          pid = cfork((forkproc *)vpn_process, (long)vpn_handle,0, 
                                      "VPN Process", 0);

          /*
           * If fork is successful.
           * NB: Do not free the handle - let the process free it 
           */
          if (pid != NO_PROCESS) {
             return(TRUE);
	  }
   
          if (vpn_error_debug)
            buginf("\n%s VPN Pending fail", 
                vpn_handle->link_hwidb->hw_namestring);

          /*
           * If the process creation somehow failed, 
           * cancel the MLPVLT query. Wait for MLPVLT to 
           * return the handle back to us (hmmm.. maybe we should 
           * timer too for insurance) 
           */
          reg_invoke_mlpvt_query_cancel(caller_name, vpn_handle);
          edisms((blockproc *)vpn_nointBLOCK, (ulong)vpn_handle);
          break;

      case VPN_HANGUP:

	  /*
	   * Well, we can't forward and SGBP tells us to hangup.
	   */
          if (vpn_error_debug || vpn_event_debug)
            buginf("\n%s VPN Can't forward, hangup", 
		   vpn_handle->link_hwidb->hw_namestring); 
	  (vpn_handle->link_hwidb->reset)(vpn_handle->link_hwidb);

	  if (!async_completion)
          vpn_free_handle(vpn_handle);
	  break;

     case VPN_ERROR:
     case VPN_TIMEOUT:
     case VPN_CANCEL:
     case VPN_ABORT:
     default:
           if (!async_completion)
              break;

           if (vpn_error_debug)
            buginf("\n%s VPN (sync) Abnormal status=%x", 
                vpn_handle->link_hwidb->hw_namestring, vpn_handle->status);
          /*
           * Continue PPP operation, 
           * PPP will datagram_done the pak.  
           */
          reg_invoke_ppp_set_lcpstate(vpn_handle->link_hwidb, AUTHENTICATING);
          reg_invoke_ppp_continue_vpn(vpn_handle->authen_type,vpn_handle->pak, 
                           vpn_handle->link_hwidb); 
          vpn_handle->pak = NULL;
           break;
  } /* end switch */

  return(FALSE);
}



/*
 * vpn_BLOCK
 * Block waiting for non VPN_PENDING status, or interface down  
 */
static boolean vpn_BLOCK (vpntype *vpn_handle)
{
  if (!interface_up_simple(vpn_handle->link_hwidb->firstsw) ||
      vpn_handle->status != VPN_PENDING)
     return(FALSE);
  return(TRUE);
}


/*
 * vpn_nointBLOCK
 * Block waiting for non VPN_PENDING status  
 */
static boolean vpn_nointBLOCK (vpntype *vpn_handle)
{
  if (!(vpn_handle->status != VPN_PENDING))
     return(FALSE);
  return(TRUE);
}


/*
 * vpn_forward_user
 * Forward the user's link to destination 
 */ 
static boolean vpn_forward_user (hwidbtype *link_hwidb, vpntype *vpn_handle)
{
  auth_state *chapstate;
  fsm *fsm;
  int status = VPN_BADPPPCFG;
  char *rem = NULL;
  userstruct *ustruct = NULL;

  chapstate = link_hwidb->authstate;
  fsm = link_hwidb->lcpfsm;

  if (!chapstate || !fsm || !chapstate->remote_name)
      return(FALSE);
  /*
   * Copy the remote user name to the gazillion other places
   * it resides (should be cleaned up someday)
   * Note that the remote name was setup without going into
   * the CHAP receive response or PAP receive reply logic
   */
  reg_invoke_dialer_remote_name(link_hwidb, chapstate->remote_name);

  /*
   * If AAA, create a ustruct to hold the username. Note that it doesn't
   * matter whether the arg supplied is CHAP or PAP - we'll juse use CHAP
   * then insert the username int PPP approprate places
   */
  
  if (!old_access_control) {
      if (is_isdn(link_hwidb) &&
	  (link_hwidb->isdn_info->remote_src == ISDN_SRC_ISDN)) {
	rem = link_hwidb->isdn_info->remote_number;
      }
      if (!aaa_create_user(&ustruct,chapstate->remote_name,NULL,
			   link_hwidb->hw_namestring,
			   rem, AAA_AUTHEN_TYPE_CHAP, AAA_SVC_PPP, PRIV_USER))
	return (FALSE);
      ustruct->acc_method = AAA_ACC_METH_LOCAL;
    }
  reg_invoke_put_authname(link_hwidb, ustruct, chapstate->remote_name);




  /*
   * Bind the Interface
   */
  vpn_bind_interface(link_hwidb, VPN_FWD_STATION);

  /*
   * Open a VPN network connection with CHAP/PAP info and interface
   * type and bandwidth. Currently, the interface type passed across
   * indicates whether the interface is ASYNC or not. Will probably change. 
   * NB: challenge_size = 0 is PAP. CHAP otherwise. 
   */
  if (chapstate->challenge_size != 0)
     status = vpn_create_mid(link_hwidb,
                      vpn_handle,
                      chapstate->remote_name,
		      L2F_AUTH_CHAP,
		      chapstate->chap_id,
                      chapstate->challenge_size, 
                      chapstate->challenge_value,
                      chapstate->value_size,
                      chapstate->md_stream,
                      link_hwidb->firstsw->visible_bandwidth,
                      fsm->confack_rcv_len,
                      fsm->confack_rcv,
                      fsm->confack_sent_len,
                      fsm->confack_sent);
  else
     status = vpn_create_mid(link_hwidb,
                      vpn_handle,
                      chapstate->remote_name,
		      L2F_AUTH_PAP,
		      chapstate->peer_pap_id,
                      0, 
                      NULL,
                      chapstate->value_size,
                      (uchar *)&chapstate->peersecret,
                      link_hwidb->firstsw->visible_bandwidth,
                      fsm->confack_rcv_len,
                      fsm->confack_rcv,
                      fsm->confack_sent_len,
                      fsm->confack_sent);

  if (status != VPN_SUCCESS) { 
     if (vpn_error_debug)
        buginf("\n%s VPN tunnel session create failed for %s",
                link_hwidb->hw_namestring, chapstate->remote_name);
     vpn_unbind_interface(link_hwidb); 
     return(FALSE);
  }
  
  if (vpn_event_debug)
     buginf("\n%s VPN vpn_forward_user %s is forwarded", 
             link_hwidb->hw_namestring, chapstate->remote_name);

  /*
   * Free up the CONFACK sent and received buffers 
   */
  if (fsm->confack_rcv)
     free(fsm->confack_rcv);
  if (fsm->confack_sent)
     free(fsm->confack_sent);
  fsm->confack_rcv = NULL;
  fsm->confack_sent = NULL;
            
  link_hwidb->ppp_lineup = TRUE;

  /*
   * If there was an installed route and assigned addresses, 
   * remove it.  As well as any dynamic dialer map.
   */
  if (link_hwidb->firstsw->ip_peer) {
     reg_invoke_clear_ip_peer_addr(link_hwidb->firstsw);
  }

  return(TRUE);
} 


/*
 * vpn_create_mid
 * Create end-to-end VPN network connection, propagating the CHAP/PAP info, 
 * LCP CONFACK info and interface bandwidth with it
 */
static int vpn_create_mid (hwidbtype *link_hwidb,
                   vpntype *vpn_handle,
                   char *user,
		   uchar auth_type,
		   uchar challenge_id,
                   uchar challenge_size,
                   uchar *challenge_value, 
                   uchar response_size,
                   uchar *response,
                   ulong interface_bandwidth,
                   short confack_rcv_size,
                   uchar *confack_rcv_ptr,
                   short confack_sent_size,
                   uchar *confack_sent_ptr)
{
    int status;
    hwidbtype *virtual_link_hwidb;
    l2f_tunneltype *l2f_tunnel;
    l2f_midtype *mid;

    if (local_test) {

        /*
         * Prepare some dummy tunnel structures
         * Get the Virtual template from MLP (Yes - MLP -this is configurable)
         */ 
        l2f_tunnel = malloc(sizeof(l2f_tunneltype));
        if (!l2f_tunnel)
           return(VPN_NORESOURCE);
        mid = malloc(sizeof(l2f_midtype));
        if (!mid) {
           free(l2f_tunnel);
           return(VPN_NORESOURCE);
	}
        mid->owner = l2f_tunnel;
        l2f_tunnel->virt_temp = reg_invoke_mlp_get_vtemplate_num();

	/*
        * This is the remote Home-Gateway or Stack member (containing the 
        * bundle) now making the call to autheticate and create a virtual 
        * interface
        */
        virtual_link_hwidb = vpn_get_mid_interface(
                      &status,
                      user,
		      auth_type,
		      challenge_id,
                      challenge_size, 
                      challenge_value,
                      response_size,
                      response,
                      interface_bandwidth,
                      confack_rcv_size,
                      confack_rcv_ptr,
                      confack_sent_size,
                      confack_sent_ptr,
		      mid);

	if (!virtual_link_hwidb) {
	    if (vpn_error_debug)
		buginf("\n%s VPN vpn_create_mid end-to-end creation fail "
			"status =%x", link_hwidb->hw_namestring, status); 
            free(l2f_tunnel);
            free(mid);
	    return(status);
	}

	/*
	 * End-to-end link creation
 	 */
	vpn_end_to_end(link_hwidb, virtual_link_hwidb);

        free(l2f_tunnel);
        free(mid);
	return(status);
    } else {
	/* Do the real VPN thang here */
	status = l2f_create_mid (link_hwidb,
                   vpn_handle->tunnel, 
                   user,
		   auth_type,
		   challenge_id,
                   challenge_size,
                   challenge_value, 
                   response_size,
                   response,
                   confack_rcv_size,
                   confack_rcv_ptr,
                   confack_sent_size,
                   confack_sent_ptr);

	return(status);
    }
}


/*
 * vpn_get_mid_interface
 * Set up the local VPN interface, setting the bandwidth, and 
 * passing the CHAP/PAP
 */
hwidbtype *vpn_get_mid_interface (int *status, char *user, 
		   uchar auth_type,
		   uchar challenge_id,
                   uchar challenge_size,
                   uchar *challenge_value, 
                   uchar response_size,
                   uchar *response,
                   ulong interface_bandwidth,
                   short confack_rcv_size,
                   uchar *confack_rcv_ptr,
                   short confack_sent_size,
                   uchar *confack_sent_ptr,
		   l2f_midtype *mid)
{
   hwidbtype *virtual_link_hwidb, *dialer_hwidb;
   hwidbtype *first_link_hwidb, *clone_hwidb;
   uchar ppp_status, *outp;
   fsm *fsm;
   int ppp_auth_type, cloned;
   auth_state *chapstate;
   boolean mlp_negotiated, aaa_present, blocking;
   paktype *pak;
   int outlen;

   aaa_present = FALSE;
   first_link_hwidb = NULL;
   clone_hwidb = NULL;
   cloned = VTEMPLATE_SUCCESS;

   if (!status) 
      return(NULL);

   if (auth_type == L2F_AUTH_CHAP) {
      ppp_auth_type = TYPE_PPP_CHAP;
   } else if (auth_type == L2F_AUTH_PAP) {
      ppp_auth_type = TYPE_PPP_UPAP;
   } else 
      return(NULL);
 
   *status = VPN_BADPPPCFG;
   ppp_status = CONFNAK;

   /*
    * Must have at least a user, a value, and CONFACK sent
    */
   if (!mid || !user || (response_size == 0) || !response || 
       !confack_sent_ptr) {
      return(NULL);
   }

   /*
    * See if this user is already on a Dialer interface (i.e. a 
    * physical link on DDR for user already connected here) 
    */
   dialer_hwidb = reg_invoke_mlp_find_bundle_netidb(user, &first_link_hwidb);

   /*
    * Create a Virtual Access Interface 
    * If a Virtual template, DDR or AAA is defined, don't block since it will 
    * be blocked later (in the clone vaccess operation) 
    * Also, assign 'no ip address' for the virtual access interface
    */
   if ((mid->owner->virt_temp != 0) || dialer_hwidb || aaa_present)
      blocking = FALSE;
   else
      blocking = TRUE;

   virtual_link_hwidb = reg_invoke_vtemplate_create_vaccess(blocking, FALSE, 
					   VTEMPLATE_TYPE_L2F);

   if (!virtual_link_hwidb) {
      *status = VPN_NORESOURCE;
      return(NULL);
   }

   if (vpn_event_debug)
      buginf("\n%s VPN Virtual interface created for %s", 
            virtual_link_hwidb->hw_namestring, user);

   /*
    * Pass on the bandwidth from the original interface to the 
    * virtual interface
    */
   virtual_link_hwidb->firstsw->visible_bandwidth = interface_bandwidth;

   /*
    * Setup the PPP state on the interface
    */
   reg_invoke_media_serial_setup(ET_PPP, NULL, virtual_link_hwidb);

   fsm = virtual_link_hwidb->lcpfsm;

   /*
    * If this Flag is enabled, it means that the user has elected the
    * Home-gateway to always send a CHAP challenge. The L2F auto-authentication
    * of PPP thus to be bypassed. The MID is set up here so that when 
    * lcp_up calls chap_init, the CHAP challenge is actually sent out the 
    * wire
    */
   if (vpn_force_local_chap) {
      /*
       * Set Up the mid
       */
      mid->idb = virtual_link_hwidb;
      mid->idb->firstsw->l2f_mid = mid;
   }

   /*
    * Check whether the remote end is an async interface by 
    * checking for async options in the last CONFACK received
    * If it is, go thru the PPP setup again, this time with the interface set
    * to async. Why do we need to do twice ? Because we need the lcpfsm 
    * from the first call. 
    */
   if (reg_invoke_lcp_ackci_forced(fsm, 
                                   confack_rcv_ptr, confack_rcv_size, 
                                   ppp_auth_type, PPP_TEST_ASYNC)) {
      if (vpn_event_debug)
         buginf("\n%s VPN Set to Async interface",  
                virtual_link_hwidb->hw_namestring); 
      virtual_link_hwidb->status |= IDB_ASYNC;
      reg_invoke_media_serial_setup(ET_PPP, NULL, virtual_link_hwidb);
   }

   /*
    * Check if Multilink PPP was negotiated
    */
   mlp_negotiated = reg_invoke_lcp_ackci_forced(fsm, 
                                   confack_rcv_ptr, confack_rcv_size, 
                                   ppp_auth_type, PPP_TEST_MLP);

   /*
    * If DDR then clone from dialer interface ONLY.
    * Block and don't filter PPP (not applicable) 
    * No cloning from Vtemplate/AAA
    */
   if (dialer_hwidb) {
	/*
	 *  dialer_hwidb could be a dialer hunt group leader of a 
	 *    dialer profile interface.
	 *  if it is a hunt group leader, then clone from dialer_hwidb
	 *  if it is a dialer profile interface, clone from the
	 *    first link in the bundle.  Why, you ask ?  Dialer profiles
	 *    does not copy ppp fields, so cloning from the profile
	 *    will not get the active ppp configs.
	 *  One could say why not clone from the first link in either case ?
	 *    This seems like it should work but would risky at this time.
	 *    This code should be consolidated in the future
	 */
	if (first_link_hwidb && is_ddr(first_link_hwidb) &&
	    DIALPOOL_MEMBER(first_link_hwidb->dialerdb) &&
	    first_link_hwidb->dialerdb->dialer_bound) {
	    /*
	     *  The bundle first link is bound to a dialer profile. Clone 
	     *    from the first link
	     */
	    clone_hwidb = first_link_hwidb;
	}
	/*
	 *  If a clone hwidb has not been selected at this point, use the
	 *    default which is the dialer_hwidb.
	 */
	if (!clone_hwidb)
	    clone_hwidb = dialer_hwidb;
       
	if (vpn_event_debug)
	    buginf("\n%s VPN Clone from %s",
		virtual_link_hwidb->hw_namestring, clone_hwidb->hw_namestring);

	cloned = reg_invoke_vtemplate_clone_vaccess(VTEMPLATE_CLONE_DIALER,
						0, 
						clone_hwidb,
						NULL,
						virtual_link_hwidb, 
						TRUE, 
						FALSE);
    } else {
	/*
	 * Clone with different configuration sources successively. 
	 * BLOCK only on the last config command. Clone with all commands
	 * unless MLP is negotiated - then copy only PPP commands from source 
	 * configuration (the full commands are copied to the Bundle interface
	 * within Multilink code)
	 * Note the order of cloning is Vtemplate followed by AAA
	 * configuration; AAA should always be cloned last.
	 */

	/*
	 * Clone from Vtemplate
	 */
	if (mid->owner->virt_temp != 0) {
	    if (vpn_event_debug)
		buginf("\n%s VPN Clone from Vtemplate %d block=%d "
		    "filterPPP=%d", virtual_link_hwidb->hw_namestring, 
		    mid->owner->virt_temp, !aaa_present, mlp_negotiated);

	    cloned = reg_invoke_vtemplate_clone_vaccess(
					VTEMPLATE_CLONE_VTEMPLATE,
					mid->owner->virt_temp,
					NULL,
					NULL,
					virtual_link_hwidb,
					(aaa_present ? FALSE : TRUE),
					(mlp_negotiated ? TRUE : FALSE));
	} else {
	    if (vpn_event_debug || vpn_error_debug)
		buginf("\n%s VPN No Virtual Template defined - check configs", 
		    virtual_link_hwidb->hw_namestring);
	}
	/*
	 * Clone from AAA last.
	 * Always BLOCK
	 */
	if (aaa_present) {
	    if (vpn_event_debug)
		buginf("\n%s VPN Clone from Vtemplate %d block=1 filterPPP=%d",
		    virtual_link_hwidb->hw_namestring, 
		    mid->owner->virt_temp, mlp_negotiated);

	    cloned = reg_invoke_vtemplate_clone_vaccess(VTEMPLATE_CLONE_AAA,
					0, 
					NULL,
					NULL,  /* should be buffer address */
					virtual_link_hwidb, 
					TRUE, 
					(mlp_negotiated ? TRUE : FALSE));
	}
    }
    /*
     * Check for successful cloning, we are outa' here otherwise
     */
    if (cloned != VTEMPLATE_SUCCESS) {
	if (vpn_event_debug || vpn_error_debug) {
	    buginf("\n%s VPN: unable to clone virtual template (%d)%s",
		virtual_link_hwidb->hw_namestring, cloned,
		cloned == VTEMPLATE_NO_VTEMPLATE ?
		", not defined - check configs" : common_str_empty_str);
	}
	*status = cloned;
	return (NULL);
    }

   /*
    * Bind the virtual interface 
    */
   vpn_bind_interface(virtual_link_hwidb, VPN_FWDED_STATION);
  
   lcp_lowerup(fsm);

    /*
    * Propagate the last CONFACK received
    */
   ppp_status = reg_invoke_lcp_ackci_forced(fsm, confack_rcv_ptr, 
                     confack_rcv_size, ppp_auth_type, PPP_TEST_NONE);

   if (!ppp_status) {
      if (vpn_error_debug)
         buginf("\n%s VPN PPP LCP not accepting rcv CONFACK",
                virtual_link_hwidb->hw_namestring);
      vpn_unbind_interface(virtual_link_hwidb);
      *status = VPN_BADPPPCFG;
      return(NULL);          
   }

   /*
    * Propagate the last CONFACK sent
    */
   if (fsm && fsm->callbacks->reqci)
      ppp_status = (*fsm->callbacks->reqci)(fsm, confack_sent_ptr, 
                   &confack_sent_size);

   if (ppp_status != CONFACK) {
      if (vpn_error_debug)
         buginf("\n%s VPN PPP LCP not accepting sent CONFACK",
                virtual_link_hwidb->hw_namestring);
      vpn_unbind_interface(virtual_link_hwidb);
      *status = VPN_BADPPPCFG;
      return(NULL);          
   }

   if (vpn_event_debug)
      buginf("\n%s VPN PPP LCP accepted sent & rcv CONFACK",
             virtual_link_hwidb->hw_namestring); 
   

   /*
    * The LCP is now in OPEN state
    */
   fsm->state = OPEN;

   /*
    * Make the LCP come up
    */
   if (fsm->callbacks->up)
       (*fsm->callbacks->up)(fsm);

   /*
    * If this Flag is not enabled (default), then we will setup the 
    * PPP engine (as a Home-gateway) as if it sent out a CHAP 
    * challenge (mirroring what the NAS sent to the client). Then we will 
    * generate a CHAP response (mirroring what the NAS received from the 
    * client) and stick it into the Vaccess's interface. 
    * For PAP, we generate the PAP request (mirroring what the NAS received
    * from the client) stick it into the Vaccess's interface.
    */
   if (!vpn_force_local_chap) {
      /*
       * Set Up the mid
       */
      mid->idb = virtual_link_hwidb;
      mid->idb->firstsw->l2f_mid = mid;

      /*
       * Get the CHAP challenge data structure
       * Initialize the CHAP id so that it will match the forcefed response
       */
      chapstate = virtual_link_hwidb->authstate;
  
      if (!chapstate) {
         vpn_unbind_interface(virtual_link_hwidb);
         *status = VPN_BADPPPCFG;
         return(NULL);
      }
      chapstate->pap_id = chapstate->chap_id = challenge_id;
  
      switch (ppp_auth_type) {
       case TYPE_PPP_CHAP:
           /*
            * The CHAP challenge value is set up to be the one
            * propagated from the remote end. This value was sent as the
            * the Challenge value to the client. CHAP challenge value size must 
            * be a ulong size - we don't support any other length.
            */ 
           if (challenge_size != chapstate->challenge_size) {
              free(chapstate->challenge_value);
              chapstate->challenge_value =
		 malloc_named(challenge_size, "VPDN CHAP challenge");
              if (!chapstate->challenge_value) {
                 chapstate->challenge_size = 0;
                 vpn_unbind_interface(virtual_link_hwidb);
                 *status = VPN_BADPPPCFG;
                 return(NULL);
              }
              chapstate->challenge_size = challenge_size;
           }

           bcopy(challenge_value, chapstate->challenge_value, challenge_size);
  
           /*
            * Set up the CHAP response value and size
            * These values were returned from the client to the remote end.
            */
           outlen = strlen(user) + response_size + sizeof(uchar);
  
           pak = getbuffer(outlen + DLLHEADERLEN + HEADERLEN);
           if (!pak) {
              vpn_unbind_interface(virtual_link_hwidb);
              *status = VPN_BADPPPCFG;
              return(NULL);
           }
  
           pak->datagramstart = pak->network_start - DLLHEADERLEN;
           outp = (uchar *) PACKET_DATA(pak->datagramstart);
           PUTCHAR(response_size, outp);
           bcopy(response, outp, response_size);
           outp += response_size;
           bcopy(user, outp, strlen(user));
           
           /*
            * Make a CHAP response packet and drive it, via the virtual 
            * interface's receive queue (iqueue), to the the PPP manager. 
            * The PPP Manager (engine) will authenticate it and send the 
            * Authorization-Ack or Nak (dropping the virtual link) as per 
            * normal PPP operation. 
            */
           if (!vpn_ppp_packet_iqueue(TYPE_PPP_CHAP, CHAP_RESPONSE, challenge_id, 
                                 pak, outlen, virtual_link_hwidb)) {
              datagram_done(pak);
              vpn_unbind_interface(virtual_link_hwidb);
              *status = VPN_BADPPPCFG;
              return(NULL);
  	 }
           break;
  
       case TYPE_PPP_UPAP:
           /*
            * outlen is the size of a request packet.  It consists of the
            * ID len, Peer ID, Pw Len and the password.
            */
           outlen = sizeof(uchar) + strlen(user) + sizeof(uchar) +
  	          response_size;
           pak = getbuffer(outlen + DLLHEADERLEN + HEADERLEN);
           if (!pak) {
              vpn_unbind_interface(virtual_link_hwidb);
              *status = VPN_BADPPPCFG;
              return(NULL);
           }
  
           pak->datagramstart = pak->network_start - PPP_ENCAPBYTES;
           outp = (uchar *) PACKET_DATA(pak->datagramstart);
  
           PUTCHAR(strlen(user), outp);
           bcopy(user, outp, strlen(user));
           outp += strlen(user);
           PUTCHAR(response_size, outp);
           bcopy(response, outp, response_size);
  
           /*
            * Make a PAP response packet and drive it, via the virtual 
            * interface's receive queue (iqueue), to the the PPP manager. 
            * The PPP Manager (engine) will authenticate it and send the 
            * Authorization-Ack or Nak (dropping the virtual link) as per 
            * normal PPP operation. 
            */
           if (!vpn_ppp_packet_iqueue(TYPE_PPP_UPAP, PAP_AUTH_REQ, 
                chapstate->pap_id, pak, outlen, virtual_link_hwidb)) {
              datagram_done(pak);
              vpn_unbind_interface(virtual_link_hwidb);
              *status = VPN_BADPPPCFG;
              return(NULL);
	 }
         break;
       }
   }
 
   /*
    * Stop the negotiation timer.  As far as we can guarantee this...
    */
   TIMER_STOP(virtual_link_hwidb->lcpstate->negtime);

   /*
    * As far as VPN is concerned, the link projection is succesful.
    */
   *status = VPN_SUCCESS; 
   return(virtual_link_hwidb);
}


/*
 * vpn_get_remote_name
 * Copy the VPDN remote name to the idb if this is a VPDN Vaccess interface
 * on the Home-Gateway
 */
void vpn_get_remote_name (hwidbtype *hwidb)
{
   /*
    * Do obvious checks. Also don't override remote name if it's
    * there already
    */
   if (!hwidb || !hwidb->authstate || hwidb->authstate->remote_name)
      return; 

   if (hwidb->firstsw->l2f_mid && 
       !hwidb->firstsw->l2f_mid->owner->instigator) {
      new_remote_name(hwidb->authstate, hwidb->firstsw->l2f_mid->name);
   }
}



/*
 * vpn_put_ipaddress
 * Will insert (if any) an ipaddress into the ipaddress list and also a
 * status field and error_code into handle fields
 */
boolean vpn_put_ipaddress (vpntype *vpn_handle, ipaddrtype ip_address, int status, int error_code)
{
   ipaddtype *ip_address_entry;

   if (!vpn_handle)
      return(FALSE);
   /*
    * If ipaddress is not specified, move on 
    */
   if (ip_address) {
      /*
       * If queue exceeds limit, don't insert this entry
       */
      if (QUEUESIZE(&vpn_handle->ipaddressQ) >= VPN_MAX_QUEUE_ENTRIES) {
         if (vpn_error_debug)
            buginf("\nvpn Queue entry limit reached");
         return(FALSE);
      }
 
      /*
       * Create an entry
       */
      ip_address_entry = malloc_named(sizeof(ipaddtype), "VPN Queue entry");
      if (!ip_address_entry)
        return(FALSE);
 
      /*
       * Stick ip address into entry and queue it to handle (tail)
       */
      ip_address_entry->address = ip_address;
      enqueue(&vpn_handle->ipaddressQ, ip_address_entry);
   }

   /*
    * Stick the error code inside handle
    */
   vpn_handle->error_code = error_code;

   /*
    * Stick the status field inside handle
    * Make sure this is the last operation in this routine 
    */
   vpn_handle->status = status;
   return(TRUE);
}


/*
 * vpn_get_ipaddress 
 * Will dequeue the first IP address (if any) in handle and return
 * Return ipaddress of 0 if none found
 */
static ipaddrtype vpn_get_ipaddress (vpntype *vpn_handle)
{
   ipaddtype *ip_address_entry;
   ipaddrtype ipaddress;

   if (!vpn_handle)
      return((ipaddrtype) 0);
   
   ip_address_entry = dequeue(&vpn_handle->ipaddressQ);
   if (!ip_address_entry) {
     if (vpn_error_debug)
        buginf("\nvpn empty queue");
     return((ipaddrtype) 0);
   }  
   
   ipaddress = ip_address_entry->address;
   free(ip_address_entry);
   return(ipaddress);
}


/*
 * vpn_get_status - Get status info from handle
 */
int vpn_get_status (vpntype *vpn_handle) 
{
  if (!vpn_handle)
     return(VPN_NOHANDLE);
  return(vpn_handle->status);
}


/*
 * vpn_free_handle - Free handle
 */
static void vpn_free_handle (vpntype *vpn_handle)
{
  ipaddtype *ip_address_entry;
  if (!vpn_handle)
     return;

  /*
   * Remove all queue entries 
   */
  while ((ip_address_entry = dequeue(&vpn_handle->ipaddressQ)))
      free(ip_address_entry);

  if (vpn_handle->pak) {
    datagram_done(vpn_handle->pak);
    vpn_handle->pak = NULL;
  }

  free(vpn_handle);
}


/*
 * vpn_ppp_packet_iqueue 
 * Prepare and send the PPP packet into the designated interface's iqueue
 */
static boolean vpn_ppp_packet_iqueue (
    ushort protocol,
    uchar code,
    uchar id,
    paktype *pak,
    short len,
    hwidbtype *link_hwidb)
{
    ppp_hdr *ppp_hdrp;
    ppp_dll_hdr *dll;

    /*
     * Shouldn't happen
     */
    if ((pak == NULL) || !link_hwidb)
       return(FALSE);
 
    if ((code != CHAP_RESPONSE) && (code != PAP_AUTH_REQ))
       return(FALSE);
 
    pak->if_input = link_hwidb->firstsw;
    pak->datagramsize = len + HEADERLEN + DLLHEADERLEN;
    pak->datagramstart = pak->network_start - DLLHEADERLEN;

    dll = (ppp_dll_hdr *)pak->datagramstart;
    dll->address = PPP_ADDRESS;
    dll->control = PPP_CONTROL;
    dll->type    = protocol;
    pak->enctype = ET_PPP;
    pak->encsize = DLLHEADERLEN;

    ppp_hdrp = (ppp_hdr *)dll->data;
    ppp_hdrp->code = (uchar)code;
    ppp_hdrp->id = id;
    ppp_hdrp->plen = len + HEADERLEN; 

    (*(link_hwidb->iqueue))(link_hwidb, pak);
    return(TRUE);
}

/*
 * TEMP
 * Creates an end-to-end tunnel session
 * Note no Challenging involved between NAS and HW
 */
static void vpn_end_to_end (hwidbtype *link_hwidb, hwidbtype *virtual_link_hwidb)
{
   if (vpn_event_debug)
    buginf("\n%s VPN end-to-end %s", link_hwidb->hw_namestring, 
           virtual_link_hwidb->hw_namestring);
   /*
    * Make the physical and virtual interface point to each other
    */
   link_hwidb->firstsw->other_link = virtual_link_hwidb;
   virtual_link_hwidb->firstsw->other_link = link_hwidb;

}


/*
 * vpn_reset
 * Interface reset for VPN virtual interfaces
 */
static void vpn_reset (hwidbtype *link_hwidb)
{
    if (is_vaccess_prevent_reset(link_hwidb->firstsw))
	return;

  if (vpn_event_debug)
    buginf("\n%s VPN reset", link_hwidb->hw_namestring);

    if (local_test) {
	/*
	 * VPN needs to tear down the tunnel session here 
	 */
        if (link_hwidb->firstsw->other_link) {
	    (*link_hwidb->firstsw->other_link->reset)
				(link_hwidb->firstsw->other_link);
	    link_hwidb->firstsw->other_link = NULL;
        }
    } else if (link_hwidb->firstsw->l2f_mid) {
        /* 
         * Do the real VPN thang here. 
         */
        if (link_hwidb->lcpstate)
           GET_TIMESTAMP(link_hwidb->lcpstate->link_transition);
	l2f_destroy_mid(link_hwidb->firstsw->l2f_mid);
	link_hwidb->firstsw->l2f_mid = NULL;
        /* we should really be waiting until we see the mid go away */

        /*
         * Mark as a down interface
         */
        if ((link_hwidb->state != IDBS_ADMINDOWN) && 
            (link_hwidb->type == IDBTYPE_VACCESS))
           net_cstate(link_hwidb, IDBS_DOWN);
    }
}

/*
 * vpn_if_state_change
 * State change on an interface
 */
static void vpn_if_state_change (hwidbtype *link_hwidb)
{
   /*
    * Some interfaces like the Serial interface don't
    * result in vpn_cleanup to be called since the reset 
    * doesn't bring down lcp directly.
    * We'll use this method to drop a projected interface
    */
   if ((link_hwidb->type != IDBTYPE_VACCESS) &&
       (link_hwidb->state == IDBS_DOWN) &&
       (reg_invoke_ppp_lcpstate(link_hwidb, FORWARDING)
        || reg_invoke_ppp_lcpstate(link_hwidb, FORWARDED))) {
      if ( local_test && link_hwidb->firstsw->other_link)
         net_cstate(link_hwidb->firstsw->other_link, IDBS_DOWN);
      else
         vpn_reset(link_hwidb);
   }
}


/*
 * vpn_bind_interface
 * Bind the interface with VPN vectors
 */
static void vpn_bind_interface (hwidbtype *link_hwidb, int direction)
{
   if (vpn_event_debug)
     buginf("\n%s VPN Bind interface direction=%d", 
            link_hwidb->hw_namestring, direction);

   switch (direction) {
     case VPN_FWD_STATION:
        if (link_hwidb->firstsw->vpn_save_iqueue) {
            if (vpn_error_debug)
                buginf("\n%s VPN Bind bad VPN_FWD_STATION ", 
                       link_hwidb->hw_namestring);
            break;
	}
        link_hwidb->firstsw->vpn_save_iqueue = link_hwidb->iqueue;
        link_hwidb->iqueue = vpn_raw_enqueue;
        break;

     case VPN_FWDED_STATION:
        if (link_hwidb->firstsw->vpn_save_oqueue) {
            if (vpn_error_debug)
                buginf("\n%s VPN Bind bad VPN_FWDED_STATION", 
                       link_hwidb->hw_namestring);
            break;
	}
        link_hwidb->firstsw->vpn_save_oqueue = link_hwidb->oqueue;
        link_hwidb->oqueue = vpn_holdq_enqueue;
        link_hwidb->reset =  vpn_reset;
        break;
   }
   link_hwidb->firstsw->vpn_flags |= direction;
}


/*
 * vpn_unbind_interface
 * Unbind the interface
 */
static void vpn_unbind_interface (hwidbtype *link_hwidb)
{
  int status; 

  if (vpn_event_debug)
    buginf("\n%s VPN Unbind interface", link_hwidb->hw_namestring);

  if (link_hwidb->firstsw->vpn_flags & VPN_FWD_STATION) {
     if (!link_hwidb->firstsw->vpn_save_iqueue) {
        if (vpn_error_debug)
           buginf("\n%s VPN Unbind bad  VPN_FWD_STATION", 
                 link_hwidb->hw_namestring);
     } else {
       link_hwidb->iqueue = link_hwidb->firstsw->vpn_save_iqueue;
       link_hwidb->firstsw->vpn_save_iqueue = NULL;
     }
  }
 
  if (link_hwidb->firstsw->vpn_flags & VPN_FWDED_STATION) {
     if (!link_hwidb->firstsw->vpn_save_oqueue) {
        if (vpn_error_debug)
           buginf("\n%s VPN Unbind bad  VPN_FWDED_STATION", 
                  link_hwidb->hw_namestring);
     } else {
       link_hwidb->oqueue = link_hwidb->firstsw->vpn_save_oqueue;
       link_hwidb->firstsw->vpn_save_oqueue = NULL;
     }
  }  

  link_hwidb->firstsw->vpn_flags &= ~(VPN_FWD_STATION + VPN_FWDED_STATION);

  /*
   * If this is a L2F Virtual interface, free it
   */
  if ((link_hwidb->type == IDBTYPE_VACCESS) &&
      (link_hwidb->firstsw->vaccess_info->type == VTEMPLATE_TYPE_L2F)) {
      status = reg_invoke_vtemplate_free_vaccess(link_hwidb); 
      if ((status != VTEMPLATE_SUCCESS) &&
  	  (status != VTEMPLATE_ALREADY_FREED)){
	  if (vpn_error_debug)
	      buginf("\n%d Fail on Vtemplate free interface", status);
      }

  }   
}



/*
 * vpn_cleanup
 * VPN cleanup triggered by LCP closing
 */
static void vpn_cleanup (hwidbtype *link_hwidb)
{
    idbtype *swidb = NULL;
    boolean cleanup_needed = FALSE;

    /*
     * Check 
     * 1) this idb has a l2f_mid
     * 2) this idb is a VPN FWD/FWDED STATION
     * 3) this idb is a L2F vaccess
     * If anyone of the above is TRUE, perform the cleanup
     */
    if (link_hwidb && link_hwidb->firstsw) {
	swidb = link_hwidb->firstsw;
	if ((swidb->l2f_mid) ||
	    (swidb->vpn_flags & (VPN_FWD_STATION | VPN_FWDED_STATION)) ||
	    ((link_hwidb->type == IDBTYPE_VACCESS) &&
	     (swidb->vaccess_info->type == VTEMPLATE_TYPE_L2F)))
	    cleanup_needed = TRUE;
    }
    if (!cleanup_needed)
	return;

    if (vpn_event_debug)
	buginf("\n%s VPN cleanup", link_hwidb->hw_namestring);

    /*
     * VPN needs to tear down the tunnel session here 
     */

    if (local_test) {
	if (link_hwidb->firstsw->other_link) {
	    (*link_hwidb->firstsw->other_link->reset)
		(link_hwidb->firstsw->other_link);
	    link_hwidb->firstsw->other_link = NULL;
	}
    } else {
	/* Do the normal VPN thang */
	vpn_reset(link_hwidb);
    }

    /*
     * Unbind the interface
     */
    vpn_unbind_interface(link_hwidb);
}


/*
 * vpn_raw_enqueue
 * VPN inbound receive packet (from hardware interface)
 * Pitch the received packet over the VPN session 
 */
static void vpn_raw_enqueue (hwidbtype *link_hwidb, paktype *pak)
{
    if (vpn_packet_debug)
        buginf("\n%s VPN (I)", link_hwidb->hw_namestring);

    if (local_test) {
	if (!link_hwidb->firstsw->other_link) {
	    if (vpn_error_debug)
	        buginf("\n%s VPN (I) vpn_raw_enqueue (test) no vpn connection", 
		   link_hwidb->hw_namestring);
	     datagram_done(pak);
	     return;
	}

        if (vpn_packet_debug)
            buginf(" to %s", link_hwidb->firstsw->other_link->hw_namestring);

	clear_if_input(pak);
	set_if_input(pak, link_hwidb->firstsw->other_link->firstsw);

	link_hwidb->firstsw->other_link->counters.rx_cumbytes += pak->datagramsize;
	link_hwidb->firstsw->other_link->counters.inputs++;   

	/*
	* Stick it to the other interface's input queue
	*/
	(*link_hwidb->firstsw->other_link->iqueue)
				(link_hwidb->firstsw->other_link, pak);
    } else {

         /*
          * Keep the Dialer happy (if any)
          */
         if (is_ddr(link_hwidb)) {
	     TIMER_START(link_hwidb->dialerdb->dialer_idle_timer,
		          link_hwidb->dialerdb->dialer_idle_ticks);
	     TIMER_STOP(link_hwidb->dialerdb->dialer_fast_idle_timer);
	 }

	 /* do the real VPN thang */
	 l2f_packet_send(link_hwidb, pak);
    }
}


/*
 * vpn_holdq_enqueue
 * VPN outbound transmit packet (to hardware interface)
 * Pitch the transmit packet over the VPN session
 */
static boolean vpn_holdq_enqueue (hwidbtype *link_hwidb, paktype *oldpak, enum HEADTAIL which)
{
    paktype *pak, *newpak;

    if (oldpak->refcount > 1) {
	/*
	 *  This is hack to handle local TCP packets.  Internally generated
	 *    TCP/IP packets seem to become corrupt when passed through 
	 *    IP twice (once being passed from TCP, then again being passed
	 *    from L2F).  This has something to do with the fact that TCP
	 *    still has a reference to the packet somewhere.  
	 *
	 *    The temporary workaround is to generate a fresh packet and 
	 *    copy the data iff the packet has more than one outstanding
	 *    reference to it.  The correct thing to do would be to 
	 *    understand exactly what problem is happening in TCP and fix
	 *    it there, but I don't have time for that right now.
	 */
	pak = getbuffer(BIGDATA);
	if (!pak) {
	    datagram_done(oldpak);
	    return(FALSE);
	}
	pak->datagramsize = oldpak->datagramsize;
	pak->enctype = oldpak->enctype;
	pak->encsize = oldpak->encsize;
	bcopy(oldpak->datagramstart, pak->datagramstart, pak->datagramsize);
	pak->if_output = link_hwidb->firstsw;
	pak->linktype = oldpak->linktype;
	pak->acct_proto = oldpak->acct_proto;
	pak->desthost = oldpak->desthost;
	pak->length = oldpak->length;
	pak->rxtype = oldpak->rxtype;
	pak->network_start = pak->datagramstart;
	datagram_done(oldpak); 

    } else {
	pak = oldpak;
    }

    /*
     * The PAK_VPN_USED flag, is to check whether the packet has
     * been thru here once. Here is the entry point for packet
     * going out on a tunnel, virtual-access interface.
     * If the packet has gone thru this routine once, it must be 
     * a tunnel in a tunnel situation. Then we need to check whether
     * the encap space, ie. pak->datagramstart - pak->data_area
     * is large enough.
     * It should have at least, ENCAPBYTES - PPP_ENCAPBYTES
     * that's max encap bytes - the ppp encapbytes.
     * If there aren't enough space, copy the content into another
     * packet and recenter the new packet.
     */
    if ( (pak->flags & PAK_VPN_USED) && 
	 (pak->datagramstart - pak->data_area < 
	  ENCAPBYTES - PPP_ENCAPBYTES ) ){

	if (vpn_packet_debug) {
	    buginf("\n%s VPN Recenter packet, data:%x, datagram:%x, diff:%d\n",
		   link_hwidb->hw_namestring,
		   pak->data_area, pak->datagramstart, 
		   pak->datagramstart - pak->data_area);
	}
	    
	/*
	 * get a packet with the same as the old one.
	 * adjust the network_start pointer, pak_copy_and
	 * _recenter() use network_start as the index to
	 * recenter.
	 */
	newpak = getbuffer(pak->pool->size);
	if (!newpak) {
	    datagram_done(pak);
	    return(FALSE);
	}
	pak->network_start = pak->datagramstart;
	pak_copy_and_recenter(pak, newpak, pak->datagramsize);

	if (!change_if_input(newpak,link_hwidb->firstsw)) {
	    datagram_done(newpak);
	    datagram_done(pak);
	    return (FALSE);
	}
	datagram_done(pak);
	pak = newpak;
    }

    if (vpn_packet_debug)
        buginf("\n%s VPN (O)", link_hwidb->hw_namestring);

    /*
     * Count the bytes here since there is no driver-level
     * code to count this at the actual transmit time
     */
    link_hwidb->counters.tx_cumbytes += pak->datagramsize;
    link_hwidb->counters.outputs++;   
    
    if (local_test) {
	if (!link_hwidb->firstsw->other_link) {
	    if (vpn_error_debug)
	        buginf("\n%s VPN (O) vpn_holdq_enqueue no vpn connection", 
		   	link_hwidb->hw_namestring);
	    datagram_done(pak);
	    return(FALSE);
	}

       if (vpn_packet_debug)
            buginf(" to %s", link_hwidb->firstsw->other_link->hw_namestring);

	pak->if_output = link_hwidb->firstsw->other_link->firstsw;

	/*
	* Stick it to the other interface's output queue
	*/   
	datagram_out(pak);

	return(TRUE);
    } else {
	/* Do the normal VPN thang */
	l2f_packet_send(link_hwidb, pak);
	return(TRUE);
    }
}


/****************External routines*************************/

/******************** MLPVLT (TEMP)*******************************/
/* 
 * Temporary MLPVLT routines for local end-to-end VPN testing.
 * To be removed when MLPVLT support is added
 */
 
/*
 * MLPVLT query lookup
 */
static boolean mlpvlt_query_lookup (char *caller_name, vpntype *vpn_handle) 
{ 
/*  ipaddrtype ip_address; */
  int pid;

  mlpvlt_notify_process = FALSE;
  TIMER_START(vpn_handle->timer, (5*ONESEC));

  /*
   * Test async completion 
   */
  pid = cfork((forkproc *)mlpvlt_process, (long) vpn_handle ,0, 
                                      "MLPVLT Process", 0);
  return(TRUE);
}

/*
 * MLPVLT query cancel 
 */
void mlpvlt_query_cancel (char *caller_name, vpntype *link_hwidb)
{
  mlpvlt_notify_process = TRUE;
}

/*
 * MLPVLT process to query and complete query
 */
static forktype mlpvlt_process (vpntype *vpn_handle)
{

  /*
   * Block waiting for query completion or timeout
   */
  edisms((blockproc *)mlpvlt_BLOCK, (ulong)vpn_handle);

  /*
   * This query is to be cancelled 
   */
  if (mlpvlt_notify_process) {
       vpn_put_ipaddress(vpn_handle, 0, VPN_CANCEL, 0);
       process_kill(THIS_PROCESS);
  }

  /*
   * Complete query with FORWARD
   */
  vpn_put_ipaddress(vpn_handle, 45, VPN_FORWARD, 0);

  process_kill(THIS_PROCESS); 

}

/*
 * Block until timeout or cancel notification
 */
static boolean mlpvlt_BLOCK (vpntype *vpn_handle) 
{

  if (mlpvlt_notify_process || AWAKE(vpn_handle->timer))
    return(FALSE);
  return(TRUE);
}
/********************* AAA interface ************************/


#define VPN_INVALID_ATTR -1
/*
 * do_vpn_author()
 */
static boolean
do_vpn_author(userstruct  *ustruct, vpntype *vpn_handle)
{
    authorstruct author;
    int i, avail = 0;
    boolean ret = TRUE;
    char *attr, *val;
    enum AV_TYPE type;
    char *tunnel_id = NULL;
    char *ip_addresses = NULL;
    char *x25_addresses = NULL;
    char *frame_relay = NULL;
    char *nas_password = NULL;
    char *gw_password = NULL;
    l2f_tunneltype *tunnel;
    l2f_tunnel_infotype *info;

   /* if (!aaa_author_needed(AAA_AUTHOR_TYPE_VPN, NULL, NULL, tty))
	return(TRUE); */

    bzero(&author, sizeof(authorstruct));
    author.auth_tty = NULL;

    /* First step.  Build outgoing A-V lists. */
    if (!aaa_add_AV(&author.out_args, &avail, AAA_ATTR_service,
		    AAA_VAL_SVC_ppp, AAA_AV_MAND) ||
	!aaa_add_AV(&author.out_args, &avail, AAA_ATTR_protocol, 
		    "vpdn", AAA_AV_MAND)) {
	ret = FALSE;
    }

    if (ret) {
	/* Second step.  Ask. */
	ret = aaa_do_author(ustruct, &author,
			    AAA_AUTHOR_TYPE_NET, 0);

	/* Third step.  Deal with response. */
	switch(author.status) {
	case AAA_AUTHOR_STATUS_ERROR:
	case AAA_AUTHOR_STATUS_FAIL:
	    ret = FALSE;
	    break;
	case AAA_AUTHOR_STATUS_PASS_ADD:
	    if (!author.in_args)
		break;
	    aaa_merge_attrs(author.out_args, author.in_args, &author.in_args);
	    /* FALLTHRU */
	case AAA_AUTHOR_STATUS_PASS_REPL:
	    if (!author.in_args)
		break;
	    for (i = 0; ret && author.in_args[i]; i++) {
		if (aaa_author_debug) {
		    buginf("\nAAA/AUTHOR/EXEC: Processing AV %s",
			   author.in_args[i]);
		}
		if (aaa_split_AV(author.in_args[i], &attr, &val, &type)) {
		    if (!strcmp(attr, AAA_ATTR_service)) {
			if (strcmp(val, AAA_VAL_SVC_ppp)) {
			    if (type == AAA_AV_MAND)
				ret = FALSE;
			    else
				if (aaa_author_debug)
				    buginf("\nAAA/AUTHOR/EXEC: "
					   "tried to change \"service\".  "
					   "Ignoring.");
			}
			continue;
		    }
		    if (!strcmp(attr, AAA_ATTR_protocol)) {
			if (strcmp(val, AAA_VAL_SVC_vpdn)) {
			    if (type == AAA_AV_MAND)
				ret = FALSE;
			    else
				if (aaa_author_debug)
				    buginf("\nAAA/AUTHOR/EXEC: "
					   "tried to change \"protocol\".  "
					   "Ignoring.");
			}
			continue;
		    }

		    if (!strcmp(attr, AAA_ATTR_tunnel_id)) {
			setstring(&tunnel_id, val);
			continue;
		    }

		    if (!strcmp(attr, AAA_ATTR_ip_addresses)) {
			setstring(&ip_addresses, val);
			continue;
		    }
		    if (!strcmp(attr, AAA_ATTR_x25_addresses)) {
			setstring(&x25_addresses, val);
			continue;
		    }
		    if (!strcmp(attr, AAA_ATTR_frame_relay)) {
			setstring(&frame_relay, val);
			continue;
		    }
		    if (!strcmp(attr, AAA_ATTR_nas_password)) {
			setstring(&nas_password, val);
			continue;
		    }
		    if (!strcmp(attr, AAA_ATTR_gw_password)) {
			setstring(&gw_password, val);
			continue;
		    }

		    /* if unknown and mandatory, we should probably
			reject the whole thing */
		    if (type == AAA_AV_MAND) {
			if (aaa_author_debug) 
			    buginf("\nAAA/AUTHOR/EXEC: received unknown "
				   "mandatory AV: %s", author.in_args[i]);
			ret = FALSE;
			continue;
		    } 
		}
	    }
	    break;
	}
    }

    if (ret) {
	if (ip_addresses && tunnel_id) {
	    info = malloc(sizeof(l2f_tunnel_infotype));
	    if (info) {
		bzero(info, sizeof(l2f_tunnel_infotype));
		info->ip_address = vpn_parse_ip_addresses(ip_addresses);
		info->direction = L2F_OUTGOING;
		info->type = VPDN_TUNNEL;
		strcpy(info->nas_name, tunnel_id);
		strcpy(info->gw_domain, ustruct->user);
		if (nas_password) {
		    if (strlen(nas_password) < MAX_VPN_SMALL_PARM) {
			strcpy(info->nas_password, nas_password);
		    } else {
			if (vpn_error_debug) 
				buginf("\nL2F NAS password too big");
		    }
		}
		if (gw_password) {
		    if (strlen(gw_password) < MAX_VPN_SMALL_PARM) {
			strcpy(info->gw_password, gw_password);
		    } else {
			if (vpn_error_debug) 
				buginf("\nL2F GW password too big");
		    }
		}
		if (vpn_event_debug)
		    buginf("\nMake tunnel with nas %s gw %s ip %i",
			tunnel_id, ustruct->user, info->ip_address);
		tunnel = l2f_make_tunnel_structure(info);
		if (tunnel) {
		    vpn_put_ipaddress (vpn_handle, tunnel->ip_address, 
					    VPN_FORWARD, 0);
		    vpn_handle->tunnel = tunnel;
		} else {
		    /* failed */
		    ret = FALSE;
		}
		free(info);
	    }
	}
    } else {
	if (vpn_error_debug)
	    buginf("%s\n", author.server_msg ?
		   author.server_msg : author_failed_msg);
    }

    aaa_cleanup_author(&author);

    free(tunnel_id);
    free(ip_addresses);
    free(x25_addresses);
    free(frame_relay);
    free(nas_password);
    free(gw_password);

    return(ret);

}

/******************** VPN *******************************/

/*
 * vpn_init
 * Initialize VPN
 */
static void vpn_init (subsystype *subsys)
{
   /*
    * Add registries
    */
   reg_add_lcp_closed_cleanup(vpn_cleanup, "vpn_cleanup");
   reg_add_vpn(vpn, "vpn");
   reg_add_vpn_enabled(vpn_enabled, "vpn_enabled");
   reg_add_vpn_get_remote_name(vpn_get_remote_name, "vpn_get_remote_name");
   reg_add_vpn_put_ipaddress(vpn_put_ipaddress, "vpn_put_ipaddress");
   reg_add_hwif_state_change(vpn_if_state_change, "vpn_if_state_change");

   /*
    * Debug initialization chores
    */
   vpn_debug_init();

   /*
    * Parser initialization
    */
   vpn_parser_init();

   /*
    * L2F initialzation
    */
   l2f_init(subsys);

   vpn_force_local_chap = FALSE;
   vpn_source_ip_address = 0L;
}



#define VPN_MAJVERSION   1
#define VPN_MINVERSION   0
#define VPN_EDITVERSION  1

SUBSYS_HEADER(vpn, VPN_MAJVERSION, VPN_MINVERSION, VPN_EDITVERSION,
	      vpn_init, SUBSYS_CLASS_PROTOCOL,
	      NULL,
	      "req: ppp");

/* $Id: vtyasync.c,v 3.10.4.8 1996/08/13 02:21:59 gstovall Exp $
 * $Source: /release/112/cvs/Xsys/pt/vtyasync.c,v $
 *------------------------------------------------------------------
 * vtyasync.c  PAD/LAT/TCP --> Virtual-Async - SLIP/PPP Mode
 *
 * April 1994, Eman Schaffer
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: vtyasync.c,v $
 * Revision 3.10.4.8  1996/08/13  02:21:59  gstovall
 * CSCdi39484:  Giant packets output on ethernet interface
 * Branch: California_branch
 * Make sure we drop giants, even if they are just a wee bit too big.
 *
 * Revision 3.10.4.7  1996/07/30  14:43:07  michellw
 * CSCdi64226:  Support larger max_frame_size (>1516) for LANE
 * Branch: California_branch
 * - The max_frame_size will be set to 1516 for ethernet and 4544 for TR
 *   if mtu is not configured on the sub-interface; otherwise, it will be
 *   set to based on the configured mtu.
 * - The lane module will not come up if the configured mtu is not valid.
 * - Add one more parameter (swidb) to the reg_invoke_media_change_mtu
 *   registry function.
 * - 1577 will come up with the default AIP mtu size (4470) if mtu is not
 *   configured on the sub-interface.
 *
 * Revision 3.10.4.6  1996/07/17  19:11:31  sjackson
 * CSCdi49571:  AAA user name is not passed in login get address requests
 * User name determined from the authentication method in use
 * Branch: California_branch
 *
 * Revision 3.10.4.5  1996/06/12  14:34:48  bjthomas
 * CSCdi38891:  need warning when line session-timeout nears
 * Branch: California_branch
 *
 * Revision 3.10.4.4  1996/05/09  14:40:39  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.10.4.3  1996/04/27  06:45:46  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 3.10.4.2.14.1  1996/04/27  07:17:40  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.10.4.2.4.1  1996/04/08  02:00:22  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.10.4.2  1996/03/27  08:35:16  billw
 * CSCdi49184:  AAA accounting records do not contain IP addr if
 * autoselected
 * Branch: California_branch
 *
 * Revision 3.10.4.1  1996/03/18  21:45:29  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.10.6.2  1996/03/28  17:35:54  rbadri
 * Branch: LE_Syn_Cal_V111_1_3_branch
 * MTU support for sub-interfaces (used by ATM interfaces)
 *
 * Revision 3.10.6.1  1996/03/22  09:41:35  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.10  1996/03/07  11:15:29  billw
 * CSCdi44863:  AAA Accounting not done for one-step protocol translation
 * Do (network) accounting for xxx->slip translation as well. (ppp is
 * already handled in lcp/etc.)
 *
 * Revision 3.9  1996/02/22  05:20:36  fox
 * CSCdi35908:  Should allow CHAP and PAP clients to same dialin pool
 * Add support for multiple acceptable inbound PPP authentication methods
 *
 * Revision 3.8.16.1  1996/03/05  05:50:35  rlowe
 * Apply LANE content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.8.12.1  1996/02/27  21:19:15  cakyol
 * non sync sync of LE_Cal_V111_0_16_branch to V111_1_0_3 yielding
 * LE_Cal_V111_1_0_3_branch
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.8  1996/01/22  21:13:05  mdb
 * CSCdi47297:  misuse of NULL
 *
 * Revision 3.7  1996/01/19  20:22:28  eschaffe
 * CSCdi46961:  Vty-async: The IP address is missing in XTA_SLIPOFF
 * message.
 *
 * Revision 3.6  1996/01/15  19:28:06  misko
 * CSCdi46049:  Problem with vty-async clean up - vty is available before
 * its clean
 * For vty async, no need to debounce cstate changes.  Initialize
 * carrierdelay appropriately.
 *
 * Revision 3.5.6.1  1996/02/15  19:18:10  rbadri
 * Branch: LE_Cal_V111_0_16_branch
 * Add an additional swidb pointer to the registry bump_cache_version
 * to invalidate caches based on swidb too.
 *
 * Revision 3.5  1995/12/07  06:00:04  eschaffe
 * CSCdi43048:  x29 profile not reliably sent to calling pad
 *
 * Revision 3.4  1995/11/29  01:14:18  lol
 * CSCdi42554:  SLIPOFF is missing information when accounting vty-async
 *
 * Revision 3.3  1995/11/18  17:42:40  fox
 * CSCdi43045:  ppp and slip exec-level commands are broken
 * When checking for a duplicate peer address, don't check our own
 * interface.
 *
 * Revision 3.2  1995/11/17  19:00:42  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:02:40  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.18  1995/11/08  21:22:33  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.17  1995/10/24  04:59:53  eschaffe
 * CSCdi42182:  session timeout not reset by ppp packets
 *
 * Revision 2.16  1995/10/13  19:58:04  eschaffe
 * CSCdi42137:  VTY lines remain in connected state but the vty-async int
 * is down
 *
 * Revision 2.15  1995/10/11  05:58:09  fox
 * CSCdi40958:  PPP address negotiation doesnt work with dialer interfaces
 * Clean up any residual peer configuration info on a re-used VTY IDB
 * Make sure a VTY IDB is default configured as an async IDB would be
 *
 * Revision 2.14  1995/10/09  23:41:43  eschaffe
 * CSCdi41378:  System crashed in pad_close()
 *
 * Revision 2.13  1995/10/02  01:54:54  lol
 * CSCdi41162:  xtacacsd: username no longer included in XTA_SLIPON
 *         Fake "if-needed" authentication for vty-async.
 *
 * Revision 2.12  1995/09/18  19:38:08  fox
 * CSCdi40523:  Add IP Address Pooling support for ISDN and other sync
 * interfaces
 * Modify IP address pooling to support all Point-to-Point interfaces
 * Remove Odd/Even Address feature
 * Add 'ppp negotiate' command
 *
 * Revision 2.11  1995/08/29  06:06:01  fox
 * CSCdi38536:  peer default ip address pool command does not match
 * documentation
 * Commit changes to mainline in order to match 11.0 Release
 * documentation.
 *
 * Revision 2.10  1995/08/23  21:33:33  eschaffe
 * CSCdi39133:  vty async translation doesnt realize users have
 * disconnected
 *
 * Revision 2.9  1995/08/23  18:49:27  eschaffe
 * CSCdi37678:  cant use per user access lists on vtyasync translate w/o
 * login
 *
 * Revision 2.8  1995/08/10  17:05:05  eschaffe
 * CSCdi37604:  Spurious memory access messages on 4500 using protocol
 * translation
 *
 * Revision 2.7  1995/07/24  23:55:36  eschaffe
 * CSCdi36312:  Memory leak in Middle and Big buffers
 *
 * Revision 2.6  1995/07/14  07:03:03  hampton
 * Convert Terminal Server to fully use the passive timers macros instead
 * of referencing the system clock directly.  [CSCdi37093]
 *
 * Revision 2.5  1995/06/10 01:17:12  eschaffe
 * CSCdi34975:  Make vty-async block more efficient
 *         Initialize wake_up_vtyasync_timer.
 *
 * Revision 2.4  1995/06/10  00:27:12  eschaffe
 * CSCdi34976:  Add a banner message to one-step vty-async connection
 *
 * Revision 2.3  1995/06/10  00:04:41  eschaffe
 * CSCdi35298:  Make sure all buginf messages are enabled by
 * "translate_debug" or "pad_debug" boolean.
 *
 * Revision 2.2  1995/06/09  13:18:33  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 22:35:55  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "../lat/mtcsys.h"
#include "interface_private.h"
#include "address.h"
#include "config.h"
#include "name.h"
#include "packet.h"
#include "../if/network.h"
#include "../ip/ip.h"
#include "subsys.h"
#include "registry.h"
#include "../pt/pt_registry.h"
#include "sys_registry.h"
#include "../h/auth_registry.regh"
#include "ttysrv.h"
#include "aaa.h"
#include "connect.h"
#include "../x25/x25.h"
#include "../x25/x25_address.h"
#include "../x25/x25_switch.h"
#include "../x25/pad.h"
#include "../tcp/telnet.h"
#include "access.h"
#include "translate.h"
#include "logger.h"
#include "login.h"
#include "../os/async.h"
#include "../ts/if_slip.h"
#include "../ts/if_async.h"
#include "../tcp/tcpcompress.h"
#include "../ts/async_ppp.h"
#include "vtyasync.h"
#include "pt_slip_ppp.h"
#include "../wan/ppp.h"
#include "../wan/dialer_registry.h"
#include "../h/cs_registry.regh"
#include "../ip/ip_registry.h"
#include "../h/media_registry.h"
#include "../arap/arap_mnp4.h"
#include "../ui/command.h"
#include "../xns/novell.h"
#include "../parser/parser_defs_autoselect.h"
#include "parser.h"
#include "../ip/ip_addrpool.h"


/*
 * Local Storage and Definitions
 */
struct vtyasync_block_args {
    tt_soc *vty;
    paktype **outpakp;
    sys_timestamp wake_up_vtyasync_timer;  /* Time to wake up vtyasync BLOCK */
};

/*
 * pt_vty_async_init
 * Initialize Virtyal Async support
 */
void pt_vty_async_init(void)
{

    reg_add_async_line_get_idb(VTY_LINE, pt_virtual_async_mode,
			       "pt_virtual_async_mode");

    reg_add_async_handle_process(VTY_LINE, pt_vtyasync_exec_connection,
				 "pt_vtyasync_exec_connection");
    
    reg_add_async_handle_options(VTY_LINE, pt_check_ppp_options, 
				 "pt_check_ppp_options");

    reg_add_slip_pt_supports_compression(pt_vtyasync_compression,
					 "pt_vtyasync_compression");
    reg_add_extra_vtylines(pt_enable_vtyasync, "pt_enable_vtyasync");
}

/*
 * pt_idb_vty_async_init
 * Get an idb , set the vty line to interactive mode, and initialize 
 * the vty to handle SLIP/PPP type of connection.
 */
static idbtype *pt_idb_vty_async_init (tt_soc *vty)
{
    idbtype *idb =  firstsw_or_null(vty->tty_idb);

    if (!idb) {
	/*
	 * Don't apply Async DTR pulse interval of 5 Secs to vtyasync lines.
	 * Vtyasync connection need to close as soon as DTR drops.
	 */
	vty->dtr_pulseinterval = (ONESEC/2);
	/* 
	 * Get an idb for this VTY async line 
	 */
	idb = get_slip_idb(vty);
	if (!idb) {
	   if (translate_debug)
	       buginf("\nvtyasync%t: Can not get idb", vty->ttynum);
	   return (NULL);
	}
	/* Set carrier state change to 0 for vty-async interfaces so the 
	 * route is deleted and line protocol is changed to down as 
	 * soon as the vty-async connection is terminated.
	 */
	vty->tty_idb->carrierdelay = vty->tty_idb->carrierdelay_def = 0;
	/* 
	 * Set the idb status so parser does not NVGEN the vty-async line
	 * configuration.
	 */
	idb->hwptr->status |= IDB_NO_NVGEN;
    } else if (idb->ip_peer) {
        /*
         * Free any previously set string as the vty may be used for a
	 * different task.
         */
	idb->ip_peer->ippeer_pool_type = PEER_ADDR_DEFAULT;
	idb->ip_peer->ippeer_addr_def = 0L;
        if (idb->ip_peer->ippeer_poolname_def) {
	    free(idb->ip_peer->ippeer_poolname_def);
	    idb->ip_peer->ippeer_poolname_def = NULL;
        }
        reg_invoke_clear_ip_peer_addr(idb);
    }
    return(idb);
}

/*
 * pt_set_keepalive
 * Enable or disable keepalive processing.
 *      "keepalive N"         enable keepalive, use N second period.
 *      "no keepalive N"      disable keepalive, reset to default period.
 */
static void pt_set_keepalive (idbtype *idb, hwidbtype *hwidb, 
			      short keepalive_period)
{
    short keepalive = keepalive_period;
    hwidb->nokeepalive = FALSE;

    if (keepalive_period == 0) {
	keepalive = KEEPALIVE_DEFAULT;
	hwidb->nokeepalive = TRUE;
    }
    hwidb->keep_period = keepalive;

    if (keepalive < hwidb->keep_count)
        hwidb->keep_count = keepalive;

    reg_invoke_encaps_set_protocol_attr(hwidb->enctype, idb);
    hwidb->inloopback = FALSE;
    /*
     * Do encapsulation specific actions to cope with keepalive changes
     */
    reg_invoke_keepalive_changed(hwidb->enctype, hwidb);
}

/*
 * pt_set_vtyasync_mtu
 * Change the maximun MTU of an interface.
 */
static boolean pt_set_vtyasync_mtu (hwidbtype *hwidb, int mtu_size)
{
    long delta;

    if (!mtu_size)
	mtu_size = hwidb->maxmtu_def;
    delta = mtu_size - hwidb->maxmtu;

    if (!delta)
	return(TRUE);                      /* no change */

    if (mtu_size < 64 ||
	!pak_pool_find_by_size(hwidb->max_buffer_size + delta )) {
	printf("\n%% Invalid MTU specified");
	return(FALSE);
    }

    hwidb->maxmtu = mtu_size;
    hwidb->firstsw->sub_mtu = hwidb->maxmtu_def;

    /*
     * Notify all protocols of a MTU change.
     */
    reg_invoke_media_change_mtu(hwidb, hwidb->firstsw, delta);
    /*
     * Invalidate the caches.
     */
    reg_invoke_bump_cache_version(hwidb, NULL, FALSE);

    (*hwidb->set_maxdgram)(hwidb, hwidb->buffersize + delta,
			   hwidb->max_buffer_size + delta);
    return(TRUE);
}

/* pt_run_vtyslip_ppp_connection (tt_soc *vty)
 * Manage one incoming PAD/LAT/TCP connection and one outgoing
 * Virtual SLIP/PPP connection.
 *
 * This is the top level of the code path that is used by both
 * one step and two step connections.  All sub-functions are used
 * for both styles...
 *  
 */
static void pt_run_vtyslip_ppp_connection (tt_soc *vty)
{
    idbtype *idb, *ipx_idb;

    /*
     * Set our connection to the current process id (forkx) so TCP/LAT/PAD
     * can block and proces the I/O on the connection.
     */
    process_get_pid(&vty->helper);

    idb = vty->tty_idb->firstsw;
    /* 
     * If we encounter a line reset, the ip address will get cleared
     * before the PT had a chance to send the SLIPOFF message.
     * Save the peer ip address so that it can be used later on in the
     * SLIPOFF message.
     */
    if (idb) {
	vty->async_peeraddr = idb->ip_peer->ippeer_addr;
    }
    pt_manage_vtyasync(vty);

    if (old_access_control) {
	/* 
	 * Send a slipoff from here instead of from ipcp_down, before
	 * tty_if_reset starts the serial_pulsetimer which then causes
	 * a different thread to clear tty->user and ippeer_addr 
	 */
	(void) reg_invoke_login_slipoff(vty, vty->async_peeraddr, 
					vty->tty_idb, FALSE);
    }
   
    /*
     * Connection has died, reset the helper process.
     */
    vty->helper = 0;
    vty->async_peeraddr = IPADDR_ZERO;

    /* 
     * Clean up anything temporary done to IDB's here.
     */
    if (idb) {
	ipx_idb = reg_invoke_pt_novell_get_owner_idb(idb);
	/*
	 * Undo ipx if it's set.
	 */
	if (ipx_idb) {
	    reg_invoke_pt_novell_pppclient_configure(ipx_idb,idb, FALSE);
	}
    }
    tty_if_reset(vty);                  /* Reset the vty async line */
}

/*
 * pt_run_vtyasync
 * Invoked by the translate daemon process for one-step connections.
 * Get an idb for a vty line and initialize it to handle async connection.
 * Check and set keepalive period, MTU size, TCP header compression, 
 * and PPP authentication.
 * Setup async mode on the VTY line.
 * Manage and run the vty-async connection.
 *
 */

void pt_run_vtyasync (translatetype *trans, tt_soc *vty)
{
    idbtype *idb;
    hwidbtype *hwidb;
    encapstype encaptype = ET_SLIP;
    char *reason = NULL;
    fsm *f;
    lcp_state *lcpstate;
    lcp_options *wo;
    ipaddrtype address = trans->out.slip_ppp.ipaddress;
    ulong slip_pppflags = trans->out.slip_ppp.slip_ppp_flags;
    boolean routing = slip_pppflags & TRANSF_SLIP_PPP_ROUTING;
    char *poolname = trans->out.slip_ppp.poolname;
    idbtype *ipx_idb = trans->out.slip_ppp.ipx_idb;
    char *method = trans->out.slip_ppp.aaa_method;
    int timeout;
    /* 
     * Terminate the login record before the vty-async session gets started
     */
    if (trans->flags & TRANSF_GLOB_LOGIN) {
	reg_invoke_login_logout(vty);
    }

    idb = pt_idb_vty_async_init(vty);
    if (!idb)
	return;

    hwidb = vty->tty_idb;

    /*
     * Mark the line for SLIP mode interactive
     */
    vty->def_capabilities |= MAYSLIP;
    vty->capabilities |= MAYSLIP;

    /*
     * Create a placeholder for the IP peer address info
     *
     * We need to create and fill this struct as an async or serial port
     * would so that the IPCP address validation routines work correctly.
     */
    if (!idb->ip_peer && !create_ippeerinfo(idb)) {
	printf(nomemory);
	return;
    }

    if (address) {
        if (!check_other_asyncs(idb, address)) {
            printf("\n%% Address %i is already in use on"
		   " an active connection\n", address);
	    return;
        }
        /*
         * Save the ip address from the translation command
         */
	idb->ip_peer->ippeer_addr_def = address;
	idb->ip_peer->ippeer_pool_type = PEER_ADDR_CONFIG;
        reg_invoke_set_ip_peer_addr(idb, address, PEER_ADDR_FROM_VTY);
    }
    else {
	int get_pool_result;

	if (!poolname || !*poolname) {
	    idb->ip_peer->ippeer_pool_type = PEER_ADDR_DEFAULT;
	    poolname = DEFAULT_LOCAL_POOL_NAME;
	} else {
	    idb->ip_peer->ippeer_pool_type = PEER_ADDR_POOL;
	    setstring(&idb->ip_peer->ippeer_poolname_def, poolname);
	    if (!idb->ip_peer->ippeer_poolname_def) {
	        printf(nomemory);
	        return;
	    }
	}
	idb->ip_peer->ippeer_addr_def = 0L;
	get_pool_result = reg_invoke_ip_get_pool_addr(&address,
						      idb, get_user_name(vty),
						      poolname, 4*ONESEC);
	if (!get_pool_result) {
	    printf("\n%% Unable to get an address from the %s pool\n", poolname);
	    return;
	}
    }

    /* change keepalive and interface mtu size if configured */
    pt_set_keepalive(idb, hwidb, trans->out.slip_ppp.keepalive_period);
    if (!pt_set_vtyasync_mtu(hwidb, trans->out.slip_ppp.mtu)) {
	reg_invoke_clear_ip_peer_addr(idb);
	return;
    }
    /*
     * Default encapsulation type is set to ET_SLIP
     * Change it for PPP
     */
    if (trans->out.slip_ppp.proto != SLIP_PROTO) {
        encaptype = ET_PPP;
    }
    
    /* 
     * Set up the VTY Async interface encapsulation 
     */
    if (hwidb->enctype != encaptype)
        async_new_encap(hwidb, encaptype);

    if (hwidb->enctype != encaptype) {
	printf("\n%% Unable to set encapsulation");
	reg_invoke_clear_ip_peer_addr(idb);
	return;
    }

    /* 
     * Allow routing update on this connection only if requested.
     * Routing is enabled by default.
     */
    idb->static_routing = !routing;

    /*
     * This needs to be set so check_slip_on_access() won't return for 
     * one-step vty-async connections when called from ipcp_up().
     */
    vty->autoselectmask |= PT_ONESTEP_CONNECTION;

    if (encaptype == ET_PPP) {
	/* Initialize PPP FSM and LCP authentication options */
	f = hwidb->lcpfsm;
	wo = (lcp_options *) f->wantoptions;
	wo->neg_auth = 0;
	bzero(wo->authtypes, PPP_AUTH_NUM);
	f->localauthen = 0;
	f->restarttime = trans->out.slip_ppp.confreq_resttime;
	if (translate_debug) {
	    buginf("\nvty-async%t: PPP-CONFREQ restart timeout = %d seconds",
		   vty->ttynum, f->restarttime/ONESEC);
	}

	lcpstate = (lcp_state *)hwidb->lcpstate;
	/* 
	 * Set the authentication options for PPP translation 
	 */
	if (slip_pppflags & TRANSF_PPP_AUTH) {
	    wo->neg_auth = NUM_AUTH_INSISTS;
	    bcopy(trans->out.slip_ppp.authtypes, wo->authtypes, PPP_AUTH_NUM);
	}
	if (old_access_control) {
	    if (slip_pppflags & TRANSF_SLIP_PPP_USE_TACACS) {
		f->localauthen |= PPP_LAUTHEN_TACACS; 
	    }
	
	    /* 
	     * If logins are *required* by the translate command, and
	     * chap and ppp are *not* required, set
	     * PPP_PT_LAUTHEN_IFNEEDED here to tell ppp to employ the
	     * username that login obtained for us.  This is a
	     * backward compatibility hack to make login_slipon use the
	     * correct username in ipcp_up.
	     */
	    if ((trans->flags & TRANSF_GLOB_LOGIN) && !wo->neg_auth) {
		f->localauthen |= PPP_PT_LAUTHEN_IFNEED;
	    }
	} else {
	    sstrncpy(lcpstate->acc_list_name, method,
		     sizeof(lcpstate->acc_list_name));
	}
    }
    else {
	/*
	 * Check if this user is allowed to use this address
 	 * Do this for SLIP only.  Login_slipon for PPP is done at IPCP 
	 */
	if (!check_slip_on_access(idb, get_user_name(vty), NULL, &reason, vty,
				  address, routing, TRUE)) {
	    if (reason)
		printf(" - %s", reason);
	    reg_invoke_clear_ip_peer_addr(idb);
	    return;
	}
    }

    /* 
     * Turn on header-compression [passive] if selected
     */
    ip_header_compression_set(idb, slip_pppflags & TRANSF_SLIP_PPP_PASSIVE,
			      slip_pppflags & 
			      TRANSF_SLIP_PPP_HEADERCOMPRESSED);
    /*
     *  Handle IPX.
     */
    if (ipx_idb)
    	reg_invoke_pt_novell_pppclient_configure(ipx_idb,idb, TRUE);

    /* Print out the banner for vty-async upon the start-up of the
     * SLIP or PPP protocol just like async dial-up.
     */
    if (!(trans->flags & TRANSF_GLOB_QUIET)) {
        printf("Entering %s%s mode.", encstring(encaptype),
               routing ? " routing" : NULL);
        printf("\n%s interface address is ",
               reg_invoke_dialer_huntgroup_member(hwidb) ? 
						"Dialer" : "Vty-Async");
        printf("unnumbered (%s)\n", idb->ip_unnumbered->namestring);
        if (address)
            printf("Your IP address is %i. ", address);
        printf("MTU is %d bytes", hwidb->maxmtu);
        if (hwidb->tcp_compress) {
            printf("\nHeader compression ");
            if (hwidb->tcp_compress->flags & THC_PERMACTIVE)
                printf("is on.");
            else
                printf("will match your system.");
	}
        printf("\n\n");
        startoutput(vty);
	timeout = 10;
	/*
	 * Pause enough for output to finish.
	 * Take the worst example with 1200 buad rate, it will take
	 * one second to output 120 char.  Max delay can be 5 seconds.
	 */
	do {
	    /*
	     * If we have unread data, startoutput will fail pending 
	     * X.29 input messages and preventing the banner messages 
	     * from printing.
	     */
	    process_sleep_for(ONESEC/2);
	    if (conn_unread(vty->conntop)) {
		/* 
		 * Flush any pending x.29 messages which will reset the
		 * unreadbytes and startoutput again.
		 */
		getc(vty);  
		startoutput(vty);
	    }
	} while (outputpending(stdio) && timeout--);
    } 
    /* 
     * Turn on SLIP/PPP protocol for this line 
     */
    if (!async_proto_mode(vty)) {
	if (translate_debug) {
	    buginf("\nvtyasync%t: unable to turn async mode on this line", 
		   vty->ttynum);
	}
	reg_invoke_clear_ip_peer_addr(idb);
	return;
    }
    /* 
     * Manage and run a vty-async connection
     */
    if (hwidb->enctype != ET_PPP) {
	/*
	 * Note that the accounting start message for a PPP connection
	 * will be sent when LCP comes up, so don't send it here.
	 */
	reg_invoke_aaa_acct_netinfo(vty->ustruct, AAA_ATTR_ADDR, &address,
				    AAA_TYPE_IP, sizeof address, FALSE);
	reg_invoke_aaa_acct_net(vty->ustruct, NULL, idb);
    }
    pt_run_vtyslip_ppp_connection(vty);
}

/*
 * pt_set_vtyasync_ip_mtu
 * Change the maximun MTU of an interface.

static void pt_set_vtyasync_ip_mtu (idbtype *idb, int size)
{
    int old_mtu;

    if (pt_ip_mtu == 0) {
	old_mtu = idb->ip_mtu;
        idb->ip_mtu = DEF_PROTO_MTU(idb, LINK_IP);
        reg_invoke_ip_change_mtu(idb->hwptr, idb->ip_mtu - old_mtu);
    }
    else {
	old_mtu = idb->ip_mtu;
	idb->ip_mtu = pt_ip_mtu;
	reg_invoke_ip_change_mtu(idb->hwptr, old_mtu - pt_ip_mtu);
    }
    reg_invoke_ipcache_invalidate(idb->hwptr, FALSE);
}
*/

/*
 * pt_vtyasync_compression
 * PT is configured for Virtual Async lines, returns TRUE if 
 * header compression [passive]
 * The stub returns FALSE for none PT images or configurations.
 */
boolean pt_vtyasync_compression (tt_soc *vty)
{
    if (vty->type == VTY_LINE)
	return(pt_compress_on || pt_compress_passive);
    else
	return(FALSE);
}
/*
 * pt_virtual_async_mode
 * Initialize and set the idb and vty.  Set ip header compression
 * for two-step connection from the global vty-async commands.
 */
boolean pt_virtual_async_mode (tt_soc *vty, 
			       idbtype **idb, hwidbtype **hwidb)
{

    *idb = pt_idb_vty_async_init(vty);
    if (!*idb)
	return(FALSE);

    *hwidb = vty->tty_idb;
    /* 
     * These are set from global vty-async config commands
     * 
     * Set up header compression
     */
    ip_header_compression_set(*idb, pt_compress_passive, pt_compress_on);
    /*
     * Set Keepalive period
     */
    pt_set_keepalive(*idb, *hwidb, pt_keepalive_period);
    /*
     *  Handle IPX.
     */
    if (pt_ipx_pppclient)
    	reg_invoke_pt_novell_pppclient_configure(pt_ipx_pppclient_owner, 
		*idb, TRUE);
    /*
     * Set ip mtu size
     */
    if (!pt_set_vtyasync_mtu(*hwidb, pt_vtyasync_mtu))
	return(FALSE);

    return(TRUE);
}

/*
 * pt_check_ppp_options
 * Invoked by the exec ppp command
 * Our index is the Encapsulation TYPE.  
 * Check and set ppp authentication options for a vty_async line
 */
void pt_check_ppp_options (tt_soc *vty)
{
    hwidbtype *hwidb = vty->tty_idb;
    fsm *f = hwidb->lcpfsm;
    lcp_options *wo = (lcp_options *) f->wantoptions;
    wo->neg_auth = 0;
    bzero(wo->authtypes, PPP_AUTH_NUM);
    f->localauthen = 0;
    hwidb = vty->tty_idb;
    
    if (pt_ppp_authentication) {
	wo->neg_auth = NUM_AUTH_INSISTS;
	bcopy(pt_auth_types, wo->authtypes, PPP_AUTH_NUM);
    }
    if (old_access_control) {
	if (pt_ppp_use_tacacs)
	    f->localauthen |= PPP_LAUTHEN_TACACS;
    } else { /* !old_access_control */
	sstrncpy(hwidb->lcpstate->acc_list_name, pt_ppp_aaa_method,
		 sizeof(hwidb->lcpstate->acc_list_name));
    }
}

/*
 * pt_vtyasync_exec_connection
 * Handle two-steps connection initiated from a virtual exec process
 */
void pt_vtyasync_exec_connection (tt_soc *vty)
{
    pt_run_vtyslip_ppp_connection(vty);
    /* 
     * We are done.  Close the connection and kill the virtual exec process
     */
    vty->helper = 0;
    death(vty);
}

/*
 * vty_asyncBLOCK - 
 * Scheduler test for VTY-ARAP VTY-IPX or VTY-ASYNC (SLIP/PPP) mode.
 * Determine if the process has anything to do.
 */
static boolean vty_asyncBLOCK (struct vtyasync_block_args *args)
{
    tt_soc *vty = args->vty;

    /*
     * NOTE: We need to make sure and check on the closing events before we 
     * check if there is data to send or receive.  If the connection was
     * dropped immediately, inputpending() or outputblock() can return
     * FALSE making it impossible for the closing events to be checked 
     * and for the process to get terminated.
     * 
     *
     * Don't block if we need to close the session.
     * vty-async driver events are checked once a second in 
     * pt_wakeup_vtyasync().
     */
    if (!TIMER_RUNNING_AND_SLEEPING(args->wake_up_vtyasync_timer)) {
	TIMER_START(args->wake_up_vtyasync_timer, ONESEC);
	if (pt_wakeup_vtyasync(vty)) {
	    /* Start a timer to wake up vtyasync_BLOCK once a second. */
	    return(FALSE);
	}
    }

    /* 
     * Don't block if input pending 
     */
    if (inputpending(vty))
	return(FALSE);            

    /* 
     * Don't block if output packet to send and stream output wouldn't block
     */
    if (!outputblock(vty)) {
	if (vty->arapptr) {
	    if (mnp4_packets_pending(vty->mnp4ptr))
		return(FALSE);
	}
	else {
	    if (*args->outpakp || vty->tty_idb->output_qcount)
		return(FALSE);
	}
    }
    return(TRUE);               /* Nothing to do right now */
}

/*
 * pt_manage_vtyasync
 * A function to manage one incoming PAD/LAT/TCP connection and one outgoing
 * Virtual ARAP or IP/IPX Async SLIP/PPP connection.
 */
void pt_manage_vtyasync (tt_soc *vty)
{
    conntype *conn = vty->conntop;
    hwidbtype *hwidb = vty->tty_idb;
    struct vtyasync_block_args block_args;

    block_args.vty = vty;
    if (vty->arapptr) {
	block_args.outpakp = &vty->mnp4ptr->mnp4_outpak;
    }
    else {
	if (hwidb->enctype == ET_PPP)
	    block_args.outpakp = &vty->appp_context->outpak;
	else
	    block_args.outpakp = &vty->slip_outpak;
    }

    GET_TIMESTAMP(conn->idletime);      /* no connection idle timer yet */
    if (vty->tty_sessiontimeout) {
	start_sessiontimeout(conn, vty);
    }
    else
	TIMER_STOP(conn->sessiontimer); /*  no session time out yet */
    vty->statbits &= ~IDLETIMEOUT;      /* reset line inactivity timeout */
    TIMER_START(block_args.wake_up_vtyasync_timer, ONESEC);

    while (TRUE) {
        /* wait for something to do */
        edisms((blockproc *)vty_asyncBLOCK,(long) &block_args);
	if (conn) {
	    if (conn->status & CONN_CHECKALL) {
		conn->status &= ~CONN_CHECKALL;
		/*
		 * Check vty driver events:
		 *  - Session time out
		 *  - Line is deallocated
		 *  - Connection is gone
		 *  - Inactivity/idletime out
		 *  - CARDROP - Done in modemcheck()
		 *  - Vty-async line is down
		 */
		if (vty->tty_sessiontimeout) {
		    if (translate_debug)
			buginf("\nvty-async%t: Session timed out", 
			       vty->ttynum);
		    break;
		}
		if (!(vty->statbits & HASPROC)) {
		    if (translate_debug)
			buginf("\nvty-async%t: Line is no longer assigned",
			       vty->ttynum);
                    break;
		}
		if (!conn_alive(conn)) {
		    if (translate_debug)
			buginf("\nvty-async%t: Connection not alive", 
			       vty->ttynum);
		    break;
		}
		if (vty->statbits & IDLETIMEOUT) {
		    if (translate_debug)
			buginf("\nvty-async%t: Inactivity timeout",
			       vty->ttynum);
		    break;
		}
		if (modemcheck(vty)) {
		    if (translate_debug) 
			buginf("\nvty-async%t: Carrier drop", vty->ttynum);
		    break;
		}
		if (vty->statbits & VTYASYNC_LINE_STATE) {
		    if (translate_debug) { 
			buginf("\nvty-async%t: Session closed - "
			       "Remote async line went down", vty->ttynum);
		    }
		    break;
                }
	    }
	    /* 
	     * Shovel bytes both ways 
	     */
	    rx_tx_handler(vty);
	} else
	    break;
    }
    vty->statbits &= ~VTYASYNC_LINE_STATE;
    TIMER_STOP(block_args.wake_up_vtyasync_timer);
    /*
     * Connection has died. Reset tx and rx services
     */
    tty_noservicehook(vty);
}

/*
 * pt_close_vtyasync
 */
void pt_close_vtyasync (translatetype *trans, tt_soc *vty)
{
    conntype *vtyconn = vty->conntop;
    fsm *f;
    hwidbtype *hwidb;

    if (vty->tty_idb && vty->tty_idb->firstsw)
	reg_invoke_clear_ip_peer_addr(vty->tty_idb->firstsw);

    /* We are done.  Clean up and kill the translation process */
    if (vtyconn) {
	conn_close(vtyconn);
	vty->conntop = NULL;
    }
    vty->statbits &= ~CONNECTED;
    vty->statbits |= CARDROP;
    trans->active--;
    vty->autoselectmask &= ~PT_ONESTEP_CONNECTION;

    /* make sure the the special PT if-needed flag is switched off */
    hwidb = vty->tty_idb;
    if (hwidb->enctype == ET_PPP) {
	f = hwidb->lcpfsm;
	f->localauthen &= ~PPP_PT_LAUTHEN_IFNEED;
    }

    /*
     * Don't allow death() to kill the current helper process
     */
    vty->helper = 0;
    death(vty);				/* close down */
    process_kill(THIS_PROCESS);		/* just in case */
}

/*
 * pt_enable_vtyasync
 * Called/invoked by "vty-async" or "line vty xxx" parser commands to enable 
 * Async mode (SLIP/PPP) on the vty lines.  
 */
void pt_enable_vtyasync (int maxlines)
{
    int line;
    tt_soc *vty;
    if ((maxlines) && (pt_vty_async_enabled)) {
	for (line = VTYBase; line < maxlines; line++) {
	    vty = MODEMS[line];
	    /*
	     * Mark the line for SLIP mode interactive
	     * for both privileged and none privileged users
	     */
	    vty->def_capabilities |= MAYSLIP;
	    vty->capabilities |= MAYSLIP;
	    /*
	     * User must specify an address at the start-up of the 
	     * connection.
	     */
	    vty->slip_flags |= SLIP_DYNAMIC;
	    if (pt_dynamic_routing)
		vty->slip_flags |= SLIP_DYN_ROUTING;
	}
    }
}

/* pt_wakeup_vtyasync
 * Called once every second to check on any vty-async events and wake up 
 * vtyasync_BLOCK if:
 * - Line protocol went down.
 * - Carrier loss.
 * - connection is dead.
 * - Session timed out.
 */
boolean pt_wakeup_vtyasync (tt_soc *vty)
{
    boolean line_up = FALSE;
    conntype *vtyconn;

    /* Since async line protocol state does not come up immediately
     * after the line state does, we have to wait till the 
     * protocol is up first, set a flag that its up, and check 
     * again if it comes down after being up so we can close the
     * vtyasync session.
     */
    if ((!vty->arapptr) && (vty->tty_idb != NULL)) {
	line_up = hw_interface_up(vty->tty_idb);
    }
    vtyconn = vty->conntop;
    if (vtyconn == NULL)
	return(TRUE);

    if (!(vty->statbits & VTYASYNC_LINE_STATE) && line_up) {
	vty->statbits |= VTYASYNC_LINE_STATE;
    }
    if (((vty->statbits & VTYASYNC_LINE_STATE) && !line_up) ||
	(vty->tty_idb->state == IDBS_RESET) ||
	(!(vty->statbits & HASPROC)) ||
	(vty->statbits & CARDROP) ||
	(!conn_alive(vtyconn)) ||
	(vty->tty_sessiontimeout &&
	 conn_session(vtyconn, TRUE)== FALSE)) {
	vtyconn->status |= CONN_CHECKALL;
	return(TRUE);
    }
    return(FALSE);
}

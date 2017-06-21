/* $Id: l2f.c,v 1.3.10.36 1996/09/12 04:02:10 tkolar Exp $
 * $Source: /release/112/cvs/Xsys/vpn/l2f.c,v $
 *------------------------------------------------------------------
 * l2f.c   --   main functions for the l2f protocol
 *
 * January 1996, Tim Kolar
 *
 * Copyright (c) 1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: l2f.c,v $
 * Revision 1.3.10.36  1996/09/12  04:02:10  tkolar
 * CSCdi62277:  L2F tunnel authentication over Radius fails
 * Branch: California_branch
 * Allow cleartext passwords to be passed from Radius
 *
 * Revision 1.3.10.35  1996/09/11  23:35:52  tkolar
 * CSCdi63923:  L2F sequence numbers per tunnel, not MID
 * Branch: California_branch
 * Do L2F sequence numbers as per the RFC.
 *
 * Revision 1.3.10.34  1996/09/09  21:47:42  tkolar
 * CSCdi54167:  show vpn truncates domain name in display
 * Branch: California_branch
 * Give more room to the username field in the "show vpdn" display.
 *
 * Revision 1.3.10.33  1996/08/28  13:20:02  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 1.3.10.32  1996/08/16  22:05:12  tkolar
 * CSCdi66351:  L2F tunnel authentication should allow arbitrary length
 * challenges
 * Branch: California_branch
 * Allow them, and change the default challenge to 16 bytes.
 *
 * Revision 1.3.10.31  1996/08/14  15:42:34  tkolar
 * CSCdi64221:  L2F checksum does not conform to PPP
 * Branch: California_branch
 * Complement the checksum before using ("You look good in hex!")
 *
 * Revision 1.3.10.30  1996/08/13  21:37:15  tkolar
 * CSCdi64206:  L2F Bad checksum causes tracebacks
 * Branch: California_branch
 * Make sure we exit the function after detecting a bad checksum
 *
 * Revision 1.3.10.29  1996/08/13  18:20:43  tkolar
 * CSCdi65882:  PPP/L2F keepalives lose their priority status
 * Branch: California_branch
 * Make use of the L2F_PRIORITY bit.
 *
 * Revision 1.3.10.28  1996/08/07  22:51:56  tkolar
 * CSCdi62172:  VPDN dual CHAP challenge from Home-Gateway
 * Branch: California_branch
 * Merge two disparate VPDN authentication schemes.
 *
 * Revision 1.3.10.27  1996/07/24  16:22:50  syiu
 * CSCdi63910:  Bridging not working with VPDN
 * Branch: California_branch
 *
 * Revision 1.3.10.26  1996/07/16  08:01:36  raj
 * CSCdi61197:  Part of  CSCdi49395 patches missing from 11.2
 * Branch: California_branch
 * Inserted patches which were missing and update dirresp code as well.
 *
 * Revision 1.3.10.25  1996/07/12  17:29:39  tkolar
 * CSCdi62195:  L2F tunnel clearing command needed
 * Branch: California_branch
 * Make is possible to shut down an entire tunnel, rather than having
 * to do it MID by MID.
 *
 * Revision 1.3.10.24  1996/07/10  00:30:03  tkolar
 * CSCdi62247:  L2F checksums should be generated in a portable fashion
 * Branch: California_branch
 *
 * Revision 1.3.10.23  1996/07/08  17:02:42  syiu
 * CSCdi62170:  VPDN L2F debug message displays IP address in hex
 * Branch: California_branch
 *
 * Revision 1.3.10.22  1996/06/29  04:32:47  syiu
 * CSCdi60957:  nas needs to reset sync serial when tunnel  goes down
 * Branch: California_branch
 *
 * Revision 1.3.10.21  1996/06/28  23:31:47  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 1.3.10.20  1996/06/24  23:10:41  tkolar
 * CSCdi61123:  L2F Tunnel authentication message more helpful
 * Branch: California_branch
 * Make an authentication failure show up under "debug vpdn error", and
 * change the key generation method to match the RFC.
 *
 * Revision 1.3.10.19  1996/06/21  16:39:13  tkolar
 * CSCdi59243:  L2F enable warning message should be VPDN
 * Branch: California_branch
 *
 * Revision 1.3.10.18  1996/06/21  16:37:22  tkolar
 * CSCdi54166:  %ALIGN-3-CORRECT: Alignment correction while using VPN
 * Branch: California_branch
 * Use the offset bit of L2F to provide correct alignment, and while we're
 * here hide a buginf behind a debug flag where it belongs.
 *
 * Revision 1.3.10.17  1996/06/18  21:03:17  tkolar
 * CSCdi60718:  L2F_KEY is ignored
 * Branch: California_branch
 * Treat keys as per the RFC.
 *
 * Revision 1.3.10.16  1996/06/04  23:16:28  tkolar
 * CSCdi59422:  show vpdn should not display if vpdn is not enabled.
 * Branch: California_branch
 * See if the VPDN daemon is running before printing anything.
 *
 * Revision 1.3.10.15  1996/05/28  19:55:21  tkolar
 * CSCdi58835:  L2F should default to not having checksums
 * Branch: California_branch
 *
 * Revision 1.3.10.14  1996/05/28  16:00:23  vandys
 * CSCdi58491:  Require accounting event for L2F tunnel creation
 * Branch: California_branch
 *
 * Revision 1.3.10.13  1996/05/24  23:44:55  perryl
 * CSCdi56912:  MLPVT tcp pipe down detection should warn L2F
 * Branch: California_branch
 * Listen to SGBP event. If SGBP brings down its pipe to a peer
 * We will bring down the l2f tunnel to that peer.
 *
 * Revision 1.3.10.12  1996/05/22  20:58:02  syiu
 * CSCdi58451:  prefix more L2F debug messages
 * Branch: California_branch
 *
 * Revision 1.3.10.11  1996/05/21  10:08:48  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 1.3.10.10  1996/05/12  21:24:17  tkolar
 * CSCdi57473:  L2F implementation does not match RFC
 * Branch: California_branch
 * Assuage those poor fools who believe that just because we
 * wrote the RFC, our implementation should somehow interoperate
 * with it.
 *
 * Revision 1.3.10.9  1996/05/12  19:22:19  tkolar
 * CSCdi57472:  vpn is an overloaded acronym
 * Branch: California_branch
 * Change "vpn" to "vpdn", and make a few other user interface tweaks
 * while we're at it.
 *
 * Revision 1.3.10.8  1996/05/12  18:06:18  tkolar
 * CSCdi57470:  ipwrite should still not be called at interrupt
 * Branch: California_branch
 * Correct CSCdi56324 .
 *
 * Revision 1.3.10.7  1996/05/08  04:43:59  syiu
 * CSCdi57037:  L2F debug messages prefix
 * Branch: California_branch
 *
 * Revision 1.3.10.6  1996/05/01  14:13:04  tkolar
 * CSCdi56324:  VPDN should not call ipwrite() at interrupt
 * Branch: California_branch
 * Send VPDN packets up to process level for forwarding.
 *
 * Revision 1.3.10.5  1996/05/01  00:26:46  syiu
 * CSCdi56248:  Multichassis MLP not starting L2F daemon automatically
 * Branch: California_branch
 *
 * Revision 1.3.10.4  1996/04/30  20:54:30  syiu
 * CSCdi56228:  Multichassis MLP/VPN not working on PRIs
 * Branch: California_branch
 *
 * Revision 1.3.10.3  1996/04/29  09:43:59  fox
 * CSCdi55358:  PPP subsystem needs to be more modular
 * Branch: California_branch
 * Move Network Control Protocol (NCP) support from PPP subsystem to
 * individual subsystems.  Attach NCP subsystems to respective protocol
 * subsystems.  Add registry calls to network protocol init code to
 * register default NCP support for NCPs not requiring option negotiation.
 *
 * Revision 1.3.10.2  1996/04/27  06:34:13  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 1.2.2.27  1996/04/25  23:56:40  syiu
 * Branch: Dial1_branch
 * Dial1_branch to IosSec_postmerge_960419
 *
 * Revision 1.2.2.26  1996/04/20  20:59:38  tkolar
 * Branch: Dial1_branch
 * Bring the L2F flags into agreement with the RFC.
 *
 * Revision 1.2.2.25  1996/04/19  14:32:21  syiu
 * Branch: Dial1_branch
 * Add hwif_state_change to monitor interface downs on projected links.
 * Modified debug messages to indicate direction of VPN/L2F packets
 *
 * Revision 1.2.2.24  1996/04/16  23:58:18  syiu
 * Branch: Dial1_branch
 * Sync Dial1_branch to V111_1_3
 *
 * Revision 1.2.2.23  1996/04/10  05:15:51  tkolar
 * Branch: Dial1_branch
 * Fix CSC.wan bugs CSCdi54175, CSCdi54149, CSCdi54150, CSCdi54153,
 * and CSCdi54167
 *
 * Revision 1.2.2.22  1996/03/28  15:40:43  syiu
 * Branch: Dial1_branch
 * Move ppp_parse_packet to input thread only.
 *
 * Revision 1.2.2.21  1996/03/26  07:05:33  syiu
 * Branch: Dial1_branch
 * Correct I/O byte counts for L2F interfaces
 *
 * Revision 1.2.2.20  1996/03/25  23:37:37  syiu
 * Branch: Dial1_branch
 * Fix crash in deletion of non-existent L2F local tunnel
 *
 * Revision 1.2.2.19  1996/03/25  05:32:10  tkolar
 * Branch: Dial1_branch
 * Enable Tunnel authentication.
 *
 * Revision 1.2.2.18  1996/03/24  01:44:23  tkolar
 * Branch: Dial1_branch
 * Enable AAA forwarding of VPN sessions, and complete
 * authentication of MIDs.  Also update a few parts of the
 * protocol to match changes in the RFC.
 *
 * Revision 1.2.2.17  1996/03/22  05:34:18  tkolar
 * Branch: Dial1_branch
 * Reshape the way MLPVT interacts with VPN.
 *
 * Revision 1.2.2.16  1996/03/21  23:51:41  tkolar
 * Branch: Dial1_branch
 * Add support for multiple incoming tunnels, fix a crash in the resend
 * code, and add the beginnings of true authentication.
 *
 * Revision 1.2.2.15  1996/03/19  20:52:52  syiu
 * Branch: Dial1_branch
 * Delete tunnel to distinguish L2F direction
 *
 * Revision 1.2.2.14  1996/03/19  03:36:52  tkolar
 * Branch: Dial1_branch
 * Clean up after tunnels when they close connections, and after
 * developers when they fat-finger commits.
 *
 * Revision 1.2.2.13  1996/03/19  02:59:38  tkolar
 * Branch: Dial1_branch
 * Clean up crash in l2f_fwd_data_packet.
 *
 * Revision 1.2.2.12  1996/03/16  16:43:55  syiu
 * Branch: Dial1_branch
 * Create/Delete L2F tunnels from MLPVT. Also move code into
 * L2F create/delete tunnel routines
 *
 * Revision 1.2.2.11  1996/03/13  23:20:08  syiu
 * Branch: Dial1_branch
 * Generalise startoutput in L2F
 *
 * Revision 1.2.2.10  1996/03/12  04:55:28  tkolar
 * Branch: Dial1_branch
 * Vastly expand L2F comments, add two new debugging variables, and make
 * wraparound sequence numbers fully understood.
 *
 * Revision 1.2.2.9  1996/03/11  22:58:30  tkolar
 * Branch: Dial1_branch
 * Clean up a small collection of nits.
 *
 * Revision 1.2.2.8  1996/03/11  05:57:05  tkolar
 * Branch: Dial1_branch
 * Provide L2F timeouts.
 *
 * Revision 1.2.2.7  1996/03/11  02:07:35  tkolar
 * Branch: Dial1_branch
 * Enable L2F management packet reliable delivery.
 *
 * Revision 1.2.2.6  1996/03/10  19:24:52  syiu
 * Branch: Dial1_branch
 * Add support for Stackability vtemplate distinct from VPN
 *
 * Revision 1.2.2.5  1996/03/03  20:56:24  tkolar
 * Branch: Dial1_branch
 * Turn off local mode by default, and commit (but don't enable) the tacacs
 * code.  Also, send full username when starting l2f session.
 *
 * Revision 1.2.2.4  1996/02/25  05:18:54  tkolar
 * Branch: Dial1_branch
 *
 * Revision 1.2.2.3  1996/02/17  22:59:01  tkolar
 * Branch: Dial1_branch
 * Checkpoint a pingable VPN.
 *
 * Revision 1.2.2.2  1996/02/13  23:07:46  tkolar
 * Branch: Dial1_branch
 * Checkpoint VPN.
 *
 * Revision 1.2.2.1  1996/02/08  23:20:00  tkolar
 * Branch: Dial1_branch
 * Checkpoint VPN integration
 * Revision 1.3.10.1  1996/03/18  22:46:08  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 1.2.14.2  1996/03/16  07:54:09  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 1.2.14.1  1996/03/07  11:06:09  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 1.3  1996/02/22  14:39:47  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 1.2  1996/01/31  18:38:32  tkolar
 * Branch: Dial1_branch
 * Get all of the VPN code into the repository, with the right names
 * and in the right places.
 *
 * Revision 1.1  1996/01/29  18:09:08  tkolar
 * Populate the VPN directory.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "types.h"
#include "../util/md5.h"
#include <ctype.h>
#include "ttysrv.h"
#include "../tcp/tcp.h"
#include "../tcp/tcpinternal.h"
#include "../h/interface_private.h"
#include "../ip/ip_registry.h"
#include "../wan/ppp_registry.h"
#include "aaa.h"
#include "subsys.h"
#include "config.h"
#include "parser.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../parser/parser_defs_parser.h"
#include "../parser/parser_defs_config.h"
#include "../parser/parser_defs_exec.h"
#include "logger.h"
#include "../os/msg_system.c"		/* Not a typo, see logger.h */
#include "../os/msg_sched.c"		/* Not a typo, see logger.h */
#include "../wan/ppp.h"
#include "../if/network.h"
#include "l2f.h"
#include "vpn.h"
#include "vpn_debug.h"
#include "../if/if_vidb.h"
#include "../ts/async_ppp.h"
#include "../h/auth_registry.regh"
#include <ciscolib.h>
#include "../util/random.h"
#include "../mlpvt/mlpvt.h"

/*
 *   Debugging variables
 */
int l2f_packet_debug;
int l2f_event_debug;
int l2f_error_debug;

/* 
 * Externs
 */
queuetype l2f_tunnelQ;
 
/*
 *  Test variables
 */
hwidbtype *l2f_testidb;    /* idb to hang test MID off of */

/*
 *  Miscellaneous globals
 */
uchar l2f_enable;               /* is the L2F daemon running? */
static uchar l2f_configured;    /* was the L2F daemon configured? */
static int next_tunnel_clid;    /* CLID to give to next tunnel we create */
static watched_queue *l2f_manage_pakQ;  /* all management packets come 
					      in through here */
static watched_queue *l2f_data_pakQ;    /* Outgoing data packets go */
					/*    out through here */
static queuetype l2f_resendQ;	 	/* after a management packet is */
                                        /*    sent, it is stored here   */
					/*    for  a while */
static pid_t l2f_management_daemon_pid;
static pid_t l2f_data_daemon_pid;

static l2f_tunnel_infotype *local_tunnels;

static boolean l2f_ignore_mid_sequence;

/*
 * some forwards
 */
static boolean l2f_ip_send(l2f_tunneltype *, uchar *, int);
static void l2f_close_tunnel_structure(l2f_tunneltype *);
static void l2f_fwd_data_packet(paktype *pak);
static boolean l2f_undefine_tunnel(int, char *);
static l2f_tunnel_infotype *l2f_define_tunnel(char *, char *, ipaddrtype,
					      int, int);

/*
 * l2f_get_packet_type
 *
 *  Reach into a management packet and retrieve its type
 *   (L2F_CONF, L2F_OPEN, etc.)
 */
static uchar l2f_get_packet_type (uchar *data)
{
    l2f_headertype *header;
    int offset;

    header = (l2f_headertype *) data;
    offset = sizeof(l2f_headertype) + header->offset;

    if (!(header->version_and_flags & L2F_KEY)) {
	/* if there's no KEY, the offset it 4 bytes less */
	offset -= sizeof(long);
    }

    return(data[offset]);
}

/*
 * l2f_tunnel_state_string
 *
 *  About what you'd expect.
 */
char *l2f_tunnel_state_string (int state) 
{
    switch(state) {
      case L2FTUNNEL_CLOSED:
	  return("closed");
      case L2FTUNNEL_OPENING:
	  return("opening");
      case L2FTUNNEL_OPEN:
	  return("open");
      case L2FTUNNEL_CLOSING:
	  return("closing");
      default:
	  return("Unknown");
    }
}

/*
 * l2f_mid_state_string
 *
 *   Same as l2f_tunnel_state_string.
 */
static char *l2f_mid_state_string (int state) 
{
    switch(state) {
      case L2FMID_CLOSED:
	  return("closed");
      case L2FMID_WAITING_FOR_TUNNEL:
	  return("waiting_for_tunnel");
      case L2FMID_OPENING:
	  return("opening");
      case L2FMID_OPEN:
	  return("open");
      case L2FMID_CLOSING:
	  return("closing");
      default:
	  return("Unknown");
    }
}

/*
 * l2f_tunnel_move_to_state
 *
 *  A formalized way of changing a tunnel's state.  
 */
void l2f_tunnel_move_to_state (l2f_tunneltype *tunnel, int state)
{
    tunnel->state = state;
    GET_TIMESTAMP(tunnel->state_time);
}

/*
 * l2f_tunnel_move_to_state
 *
 *  A formalized way of changing a MID's state.  
 */
void l2f_mid_move_to_state (l2f_midtype *mid, int state)
{
    /*
     * Offer an accounting record
     */
    if ((mid->state != state) && (state == L2FMID_OPEN)
	    && !old_access_control) {
	hwidbtype *hw = mid->idb;
	lcp_state *lcp = hw->lcpstate;

	if (lcp->ustruct == NULL) {
	    int ret;
	    char buf[32];

	    sprintf(buf, "%i", mid->owner->ip_address);
	    ret = aaa_create_user(&lcp->ustruct,
		mid->name, NULL,
		hw->hw_namestring, buf,
		AAA_AUTHEN_TYPE_CHAP, AAA_SVC_PPP, PRIV_USER);
	    if (!ret) {
		if (l2f_event_debug) {
		    buginf("\n%s L2F aaa_create_user failed for %s",
			hw->hw_namestring, mid->name);
		}
	    }
	}
	if (mid->owner->type == VPDN_MULTICHASSIS) {
	    /*
	     * If we get here, then we're doing MMLP and trying to forward
	     * this user to another box in our stack group.  The accounting
	     * record for this link will be generated by the box containing
	     * the MLP bundle so we don't want to do it here.  We set the
	     * no_acct flag because we want to make sure we don't generate
	     * both the START record(done below) and the STOP record(done
	     * in the PPP code.
	     */

	    lcp->ustruct->no_acct = TRUE;
	}
	reg_invoke_aaa_acct_net(lcp->ustruct, NULL, hw->firstsw);
    }

    mid->state = state;
    GET_TIMESTAMP(mid->state_time);
}

void clear_vpdn_command (parseinfo *csb)
{
    l2f_tunneltype *target = NULL;
    char *nas, *gw;

    if (!l2f_configured) {
	printf("%% VPDN is not enabled\n");
	return;
    }

    if (csb->which == L2F_CLEAR_ACCOUNT) {
	return;
    }

    nas = GETOBJ(string,1);
    gw = GETOBJ(string,2);

    target = l2f_find_active_tunnel_by_names(nas, gw);
 
    if (target == NULL) {
	printf("Couldn't find specified tunnel.\n");
    } else {
	printf("Clearing all MIDs and dropping the tunnel.\n");
	l2f_clear_tunnel(target);
    }

    return;
}


/*
 * show_l2f_command
 *
 *  The function called when a "show vpdn" command is issued.
 */
void show_l2f_command (parseinfo *csb)
{
    char *nas, *gw;
    l2f_midtype *mid;
    int i, j, tunnel_size;
    l2f_tunneltype *tunnel;
    struct l2f_midlock_info *mid_lock;
    struct {
	l2f_tunneltype *tunnel;
	struct l2f_midlock_info *mid_locks;
	int mid_size;
    } *tunnels;

    if (!l2f_enable) {
	return;
    }

    nas = GETOBJ(string,1);
    gw = GETOBJ(string,2);

    tunnel_size = QUEUESIZE(&l2f_tunnelQ);
    if (!tunnel_size) {
	printf("\n%% No active L2F tunnels");
	return;
    }
    tunnels = malloc(tunnel_size * sizeof(*tunnels));
    if (!tunnels)
	return;

    /* lock all tunnels */
    for (i = 0; i < tunnel_size; i++) {
	tunnel = l2f_getnext_tunnel();
	if (!nas[0] || (!strcmp(nas, tunnel->nas_name) &&
	    !strcmp(gw, tunnel->gw_name))) {
	    tunnels[i].tunnel = tunnel;
	    tunnels[i].mid_locks =
		l2f_lock_mids_in_tunnel(tunnel, &tunnels[i].mid_size);
	}
    }

    automore_enable(NULL);

    /*
     *  If we're given a specification for a particular tunnel, just print
     *   out that info.
     */
    if (nas[0]) {
	for (i = 0; i < tunnel_size; i++) {
	    tunnel = tunnels[i].tunnel;
	    if (tunnel) {
		printf("NAS name: %s\n", nas);
		printf("NAS CLID: %d\n", tunnel->nas_clid);
		if (tunnel->sock) 
		    if (tunnel->instigator) 
			printf("NAS IP address %i\n", 
			       tunnel->sock->laddress.ip_addr);
		    else 
			printf("NAS IP address %i\n", 
			       tunnel->sock->faddress.ip_addr);
  		
		printf("Gateway name: %s\n", gw);
		printf("Gateway CLID: %d\n", tunnel->gw_clid);
		if (tunnel->sock) 
		    if (tunnel->instigator) 
			printf("Gateway IP address %i\n", 
			       tunnel->sock->faddress.ip_addr);
		    else 
			printf("Gateway IP address %i\n", 
			       tunnel->sock->laddress.ip_addr);
  	
		printf("State: %s\n", l2f_tunnel_state_string(tunnel->state));
		printf("Packets out: %d\n", tunnel->packets_out);
		printf("Bytes out: %d\n", tunnel->bytes_out);
		printf("Packets in: %d\n", tunnel->packets_in);
		printf("Bytes in: %d\n", tunnel->bytes_in);

		mid_lock = tunnels[i].mid_locks;
		for (j = 0; j < tunnels[i].mid_size; j++, mid_lock++) {
		    mid = mid_lock->mid;
		    if (mid->owner == tunnel) {
			printf("-----------------");
			printf("	MID: %d\n", mid->mid);
			printf("	User:  %s\n",
			       mid_lock->name ? mid_lock->name : "");
			printf("	Interface:  %s\n", 
			       mid->idb ? mid->idb->hw_namestring : "");
			printf("	State:  %s\n", 
			       l2f_mid_state_string(mid->state));
			printf("	Packets out: %d\n", mid->packets_out);
			printf("	Bytes out: %d\n", mid->bytes_out);
			printf("	Packets in: %d\n", mid->packets_in);
			printf("	Bytes in: %d\n", mid->bytes_in);
		    }
		}
	    }
	    /* free one tunnel at a time */
	    l2f_unlock_mids_in_tunnel(tunnel, tunnels[i].mid_locks,
				      tunnels[i].mid_size);
	}
	free(tunnels);
	automore_disable();
	return;
    }

    printf("\n%% Active L2F tunnels = %d\n", tunnel_size);
    for (i = 0; i < tunnel_size; i++) {
	tunnel = tunnels[i].tunnel;

        printf("\nNAS Name        Gateway Name    NAS CLID   Gateway CLID   "
	       "State");
	printf("\n%15s %17s %12d %10d %10s",
		tunnel->nas_name, tunnel->gw_name, 
		tunnel->nas_clid, tunnel->gw_clid, 
		l2f_tunnel_state_string(tunnel->state));

	if (tunnel->sock) 
	    if (tunnel->instigator) 
		printf("\n%15i %17i\n", tunnel->sock->laddress.ip_addr,
		       tunnel->sock->faddress.ip_addr);
	    else 
		printf("\n%15i %17i\n", tunnel->sock->faddress.ip_addr,
		       tunnel->sock->laddress.ip_addr);
  
        printf("\nL2F MIDs = %d\n", tunnels[i].mid_size);
        printf("Name                       NAS Name      Interface    MID      "
	       "State");
	mid_lock = tunnels[i].mid_locks;
	for (j = 0; j < tunnels[i].mid_size; j++, mid_lock++) {
	    mid = mid_lock->mid;
	    printf("\n%26s %14s %12s %7d %10s",
		   mid_lock->name ? mid_lock->name : "",
		   mid_lock->owner ? mid_lock->owner->nas_name : "",
		   mid->idb ? mid->idb->hw_short_namestring : "",
		   mid->mid, l2f_mid_state_string(mid->state));
	}
        printf("\n\n");

	/* unlock one tunnel at a time */
	l2f_unlock_mids_in_tunnel(tunnel, tunnels[i].mid_locks,
				  tunnels[i].mid_size);
    }

    free(tunnels);
    automore_disable();
}

/* 
 *  get_new_tunnel_clid
 *
 *  Assign a tunnel clid
 *
 *  Note that assigned CLIDs are unique for every tunnel on the box, 
 *    whether it's incoming or outgoing.
 */
static int get_new_tunnel_clid (void)
{
    return(next_tunnel_clid++);
}

/*
 * l2f_find_tunnel_by_name
 *
 *   This is for looking up tunnels locally, so if we're the gateway
 *     we use the NAS name, gateway domain otherwise. 
 */ 
l2f_tunnel_infotype *l2f_find_tunnel_by_name (char *name)
{
    l2f_tunnel_infotype *info;

    for (info = local_tunnels; info != NULL; info = info->next) {
	if (info->direction == L2F_INCOMING) {
	    if (!strcmp(name, info->nas_name)) {
		return(info);
		break;
	    }
	} else {
	    if (!strcmp(name, info->gw_domain)) {
		return(info);
		break;
	    }
	}
    }

    if (l2f_event_debug)
	buginf("\nL2F Never heard of %s", name);
    return(NULL);
}

/*
 *   l2f_instigate_tunnel
 *
 *   Do the opening volley for a tunnel.
 */
static boolean l2f_instigate_tunnel (l2f_tunneltype *l2f_tunnel)
{
    int length;
    uchar *buf;
    addrtype daddr, saddr;

    buf = malloc_named(MAX_L2FMANAGE_SIZE, "l2f tnlopen");
    if (!buf) {
	return(FALSE);
    }

    if (l2f_tunnel->sock) {
	free(buf);
	return(FALSE);
    }

    /*
     * Open UDP socket for the tunnel
     * If this is a Stackability tunnel and an ip source address is defined
     * than use it. This is used for SGBP stack group with backup links.
     * Otherwise, vpn source ip address may be defined. This is used
     * for passing the firewall on the Home Gateway side.
     * If no static source ip address is specified, we will
     * dynamically choose the best local ip address for each packet
     * going out of this socket. Note, the best local ip address, may
     * not be the ip address of the interface it goes out of.
     */

    daddr.ip_addr = l2f_tunnel->ip_address;
    saddr.ip_addr = 0;
    if (l2f_tunnel->source_ip) {
	if (l2f_tunnel->source_ip != -1)
	    saddr.ip_addr = l2f_tunnel->source_ip;
    }
    else
	saddr.ip_addr = vpn_source_ip_address;
		
    if (saddr.ip_addr) {
	l2f_tunnel->sock = (ipsocktype *) open_ipsocket(UDP_PROT, &saddr,
		&daddr, L2F_PORT, L2F_PORT, 
		SOCK_ANYSRC | SOCK_SRC_SPECIFIED);
    } else {
	l2f_tunnel->sock = (ipsocktype *) open_ipsocket(UDP_PROT, 0, 
		&daddr, L2F_PORT, L2F_PORT, SOCK_ANYSRC | SOCK_SRC_DYN);
    }

    /* send opening message */
    if (!l2f_tunnel->sock) {
	free(buf);
	return(FALSE);
    }
    if (l2f_event_debug) {
	if (saddr.ip_addr)
	    buginf("\nL2F Open UDP socket from %i to %i",
		   l2f_tunnel->sock->laddress.ip_addr, 
		   l2f_tunnel->sock->faddress.ip_addr);
	else
	    buginf("\nL2F Open UDP socket to %i",
		   l2f_tunnel->sock->faddress.ip_addr);
    }

    length = l2f_nas2gw_tunnel0(l2f_tunnel, buf, MAX_L2FMANAGE_SIZE);
    if (length > 0) {
	l2f_send(l2f_tunnel, buf, length, INITIAL_SEND);
	l2f_tunnel_move_to_state(l2f_tunnel, L2FTUNNEL_OPENING);
    }

    free(buf);
    return(TRUE);
}

/*
 *  l2f_initiate_tunnel_shutdown
 *
 *  Send a "close tunnel" packet to the other side.
 */
void l2f_initiate_tunnel_shutdown (l2f_tunneltype *tunnel) 
{
    int length;
    uchar *buf;

    if (l2f_event_debug)
	buginf("\nL2F Initiating tunnel shutdown.");

    buf = malloc_named(MAX_L2FMANAGE_SIZE, "l2f tnlshut");

    if (!buf) {
	return;
    }

    length = l2f_pkt_close_tunnel(tunnel, buf, MAX_L2FMANAGE_SIZE);
    if (length > 0) {
	l2f_send(tunnel, buf, length, INITIAL_SEND);
	l2f_tunnel_move_to_state(tunnel, L2FTUNNEL_CLOSING);
    }

    free(buf);
}

/*
 * l2f_destroy_mid_structure
 */
void l2f_destroy_mid_structure (l2f_midtype *mid, boolean reset_int)
{

    l2f_clean_resendQ(mid->owner, L2F_CLOSE, mid->mid);
    l2f_clean_resendQ(mid->owner, L2F_OPEN, mid->mid);

    if (mid->owner && QUEUESIZE(&mid->owner->tunnel_mid_elemQ))
       l2f_remove_mid_elem_from_tunnel(mid->owner, mid);
  
    /*
     *  If we've never heard of this MID, bail.
     */
    if (!mid)
       return;

    if (mid->owner->instigator) {
	/* if we're the NAS, this got stored with the gateway clid */
	l2f_remove_mid(mid->owner->gw_clid, mid->mid);
    } else {
	/* if we're the gateway, this got stored with the NAS clid */
	l2f_remove_mid(mid->owner->nas_clid, mid->mid);
    }

    if ((mid->idb) && (reset_int)) {

        /*
         * If this is the NAS, drive the idb state down for 
         * serial devices since a reset was not good enough
         * (a serial device special)
         * Otherwise just reset the interface.
         * Both cases, clear the mid entry
         */
        if ((mid->idb->type != IDBTYPE_VACCESS) && 
            (mid->idb->status & IDB_SERIAL) && 
            (mid->owner->instigator) && !is_isdn(mid->idb)) {
           mid->idb->firstsw->l2f_mid = NULL;
           net_cstate(mid->idb, IDBS_DOWN);
        } else {
	   (*mid->idb->reset)(mid->idb);
	   mid->idb->firstsw->l2f_mid = NULL;
        }

    }

    mid->owner->midcount--;

    if (mid->owner->midcount == 0) {
       if (l2f_event_debug)
	   buginf("\nL2F Tunnel is going down!");
       l2f_initiate_tunnel_shutdown(mid->owner);
    }

    if (mid->challenge)
        free(mid->challenge);

    if (mid->response)
        free(mid->response);

    if (mid->rcv_ack) {
	free(mid->rcv_ack);
    }

    if (mid->snt_ack) {
	free(mid->snt_ack);
    }

    if (mid->name) {
	free(mid->name);
    }
    free(mid);
}

/*
 * l2f_make_mid_structure
 *
 *  Allocate and initialize a mid for either nas or gw
 */ 
l2f_midtype *l2f_make_mid_structure (l2f_tunneltype *l2f_tunnel)
{
    l2f_midtype *mid;

    mid = malloc_named(sizeof(l2f_midtype), "l2f mid");
    if (mid == NULL) {
	return(NULL);
    }

    mid->mid = 0;
    mid->idb = NULL;
    l2f_mid_move_to_state(mid, L2FMID_CLOSED);
    mid->owner = l2f_tunnel;
    mid->owner->midcount++;
    mid->outgoing_sequence = 0;
    mid->incoming_sequence = 255;
    mid->packets_in = 0;
    mid->bytes_in = 0;
    mid->packets_out = 0;
    mid->bytes_out = 0;

    /*
     * Insert in tunnel list
     */
    if (!l2f_store_tunnel_mid_elem(l2f_tunnel, mid)) {
       free(mid);
       return(NULL);
    }

    return(mid);
}

/*
 * l2f_activate_waiting_mids
 *
 *  When a tunnel comes up there are mostly likely one or more MIDs waiting
 *  to take advantage of it.  This function checks for these and gets the
 *  MIDs going when their tunnel is up
 */
void l2f_activate_waiting_mids (l2f_tunneltype *tunnel)
{
    l2f_midtype *mid;
    int length, mid_size;
    uchar *buf;

    buf = malloc_named(MAX_L2FMANAGE_SIZE, "l2f tnlstrt");
    if (!buf) {
	return;
    }

    /*
     *  If the tunnel isn't open, nevermind.
     */
    if (tunnel->state != L2FTUNNEL_OPEN) {
	free(buf);
	return;
    }

    mid_size = QUEUESIZE(&tunnel->tunnel_mid_elemQ);
    mid = l2f_getnext_mid_from_tunnel(tunnel);
    while ((mid_size > 0) && mid) {
	if ((mid->state == L2FMID_WAITING_FOR_TUNNEL) && 
		(mid->owner == tunnel)) {
	    /* send opening message */
	    length = l2f_nas2gw_mid0(mid, buf, MAX_L2FMANAGE_SIZE);
	    if (length > 0) {
		l2f_send(tunnel, buf, length, INITIAL_SEND);
		l2f_mid_move_to_state(mid, L2FMID_OPENING);
	    }
	}
        mid = l2f_getnext_mid_from_tunnel(tunnel);
        mid_size--;
    }
    free(buf);
}

/*
 * l2f_destroy_mid
 */
void l2f_destroy_mid (l2f_midtype *mid)
{
    int length;
    uchar *buf;
    l2f_tunneltype *tunnel;

    buf = malloc_named(MAX_L2FMANAGE_SIZE, "l2f mid destry");
    if (!buf) {
	return;
    }

    tunnel = mid->owner;

    length = l2f_pkt_close_mid(mid, buf, MAX_L2FMANAGE_SIZE, VPN_ADMIN);
    if (length > 0) {
	l2f_send(tunnel, buf, length, INITIAL_SEND);
	l2f_mid_move_to_state(mid, L2FMID_CLOSING);
    }

    free(buf);
}

/*
 *  l2f_create_mid
 *
 *   This is the call made by PPP to instigate a MID.  
 *   If the tunnel that the mid would be created over does not yet exist,
 *   the MID is scheduled for creation and the tunnel creation is done first.
 *   Otherwise the MID is created immediately.
 *
 *   Called by the NAS
 */
int l2f_create_mid (
		    hwidbtype *idb,
		    l2f_tunneltype *tunnel,
		    char *chap_username_rcv,
		    uchar auth_type,
		    uchar chap_id,
		    uchar chap_challenge_len,
		    uchar *chap_challenge,
		    uchar chap_response_len,
		    uchar *chap_response,
		    int len_lcp_ack1,
		    uchar *rcv_ack,
		    int len_lcp_ack2,
		    uchar *snt_ack)
{
    int length;
    uchar *buf;
    l2f_midtype *mid;


    if ((tunnel == NULL) && l2f_error_debug){
      buginf("\nL2F Attempt to create mid with no tunnel");
    }

    /* 
     *  If a mid already exists for this idb, then most likely we
     *  already closed it but the other side never got back to us.
     *  If the Mid is Open, it means that the interface went down and came 
     *  back up before the net background had a chance to bring the previous
     *  mid down. We will initiate an L2F close for the mid.  
     *  Otherwise, we destroy the mid locally, 
     */
     mid = l2f_get_mid_by_idb(idb);
     if (mid) {
	if (mid->state ==  L2FMID_OPEN) {
           if (l2f_error_debug)
              buginf("\nL2F NAS/HG Clid=%d/%d Mid=%d Open on %s, closing",
                    mid->owner->nas_clid, mid->owner->gw_clid, mid->mid,
                    mid->idb->hw_namestring);
           l2f_destroy_mid(mid);
	   idb->firstsw->l2f_mid = NULL; 
        } else {
           if (l2f_error_debug)
              buginf("\nL2F NAS/HG Clid=%d/%d Mid=%d %s on %s, override",
                    mid->owner->nas_clid, mid->owner->gw_clid, 
                    mid->mid,
                    l2f_mid_state_string(mid->state), 
                    mid->idb->hw_namestring);
	      l2f_destroy_mid_structure(mid, FALSE);
        }
     }


    buf = malloc_named(MAX_L2FMANAGE_SIZE, "l2f mid create");
    if (!buf) {
	return(VPN_NORESOURCE);
    }

    mid = l2f_make_mid_structure(tunnel);
    mid->mid = tunnel->next_mid++;
    l2f_store_mid(tunnel->gw_clid, mid->mid, mid);

    setstring(&mid->name, chap_username_rcv);

    mid->authtype = auth_type;
    mid->chap_id = chap_id;

    mid->challenge_len = chap_challenge_len;
    if (mid->challenge_len) {
	 mid->challenge = malloc_named(mid->challenge_len, "l2f chlng");
	 if (mid->challenge == NULL) {
             l2f_destroy_mid_structure(mid, FALSE);
	     free(buf);
	     return(VPN_NORESOURCE);
	 }
	 bcopy(chap_challenge, mid->challenge, mid->challenge_len);
    }

    mid->response_len = chap_response_len;
    if (mid->response_len) {
	 mid->response = malloc_named(mid->response_len, "l2f chlng resp");
	 if (mid->response == NULL) {
             l2f_destroy_mid_structure(mid, FALSE);
             free(buf);
	     return(VPN_NORESOURCE);
	 }
	 bcopy(chap_response, mid->response, mid->response_len);
    }

    mid->rcv_ack_len = len_lcp_ack1;
    if (mid->rcv_ack_len) {
	 mid->rcv_ack = malloc_named(mid->rcv_ack_len, "l2f mid ack1");
	 if (mid->rcv_ack == NULL) {
             l2f_destroy_mid_structure(mid, FALSE);
             free(buf);
	     return(VPN_NORESOURCE);
	 }
	 bcopy(rcv_ack, mid->rcv_ack, mid->rcv_ack_len);
    }

    mid->snt_ack_len = len_lcp_ack2;
    if (mid->snt_ack_len) {
	 mid->snt_ack = malloc_named(mid->snt_ack_len, "l2f mid ack2");
	 if (mid->snt_ack == NULL) {
             l2f_destroy_mid_structure(mid, FALSE);
	     free(buf);
	     return(VPN_NORESOURCE);
	 }
	 bcopy(snt_ack, mid->snt_ack, mid->snt_ack_len);
    }

    mid->idb = idb;
    if (idb)
	idb->firstsw->l2f_mid = mid;

    switch (tunnel->state) {
      case L2FTUNNEL_CLOSED:
        if (!l2f_instigate_tunnel(tunnel)) {
             l2f_destroy_mid_structure(mid, FALSE);
	     free(buf);
	     return(VPN_NORESOURCE);
	}
	l2f_mid_move_to_state(mid, L2FMID_WAITING_FOR_TUNNEL);
	break;
      case L2FTUNNEL_OPENING:
	l2f_mid_move_to_state(mid, L2FMID_WAITING_FOR_TUNNEL);
	break;
      case L2FTUNNEL_OPEN:
        length = l2f_nas2gw_mid0(mid, buf, MAX_L2FMANAGE_SIZE);
	if (length > 0) {
	    l2f_send(tunnel, buf, length, INITIAL_SEND);
	    l2f_mid_move_to_state(mid, L2FMID_OPENING);
	}
	break;
      case L2FTUNNEL_CLOSING:
	break;
      default:
	if (l2f_error_debug)
	    buginf("\nL2F Unknown tunnel state in l2f_create_mid");
	break;
    }
    free(buf);
    return(VPN_SUCCESS);
}


/*
 * l2f_clear_tunnel
 *
 * This cleans out all of the MIDs in a tunnel.  The last MID to go down
 *   will automatically pull the tunnel down with it -- what's left for us
 *   to do is to destroy the tunnel structure.
 */
void l2f_clear_tunnel (l2f_tunneltype *tunnel)
{
    l2f_remove_mids_from_tunnel(tunnel);
    l2f_close_tunnel_structure(tunnel);
}

/*
 *  l2f_stack_peer_event_listener
 *
 *  Listen to SGBP event. If SGBP brings down its pipe to a peer
 *  We will take the warning under advisement by sending L2F echoes
 *
 */
static void l2f_stack_peer_event_listener (int event, ipaddrtype addr)
{
    l2f_tunneltype *tunnel;

    if (event == MLPVT_PEER_VANISH) {

	tunnel = l2f_find_active_tunnel_by_address(addr);

	if (tunnel) {
	    if (l2f_event_debug || l2f_error_debug) 
		buginf("\nL2F SGBP pipe to %i is down, under advisement",addr);
	    TIMER_START(tunnel->last_traffic, 0);
	}
    }
}

/*
 *  l2f_clean_resendQ
 *
 *  This goes through the resendQ and removes packets that are waiting 
 *  there.  
 * 
 *  If "which" == L2F_ANY_TYPE, the mid is ignored and all packets are
 *  stripped from the tunnel.  Useful when you're closing a tunnel down.
 */
boolean l2f_clean_resendQ (l2f_tunneltype *tunnel, int which, int mid)
{
    l2f_resend_elem *resend, *next;
    uchar packet_type;
    l2f_headertype *header;
    boolean found = FALSE;


    if (which == L2F_ANY_TYPE) {
	for (resend = l2f_resendQ.qhead; resend != NULL; resend = next) {
	    next = resend->next;
	    if (resend->tunnel == tunnel) {
		if (l2f_event_debug)
		    buginf("\nL2F Removing resend packet");
		unqueue(&l2f_resendQ, resend);
		free(resend->data);
		free(resend);
	    }
	}
        return(TRUE);
    } else {
	for (resend = l2f_resendQ.qhead; resend != NULL; resend = next) {
	    next = resend->next;

	    packet_type = l2f_get_packet_type(resend->data);

	    header = (l2f_headertype *) resend->data;

	    if ((resend->tunnel == tunnel) && (packet_type == which)
		&& (mid == header->mid)) {
		if (l2f_event_debug)
		    buginf("\nL2F Removing resend packet (type %d)", 
                           packet_type);
                found = TRUE;
		unqueue(&l2f_resendQ, resend);
		free(resend->data);
		free(resend);
	    }
	}
    }
   return(found);
}

/*
 * l2f_run_resendQ
 *
 *  This runs once every five seconds of so and causes any packets in the
 *   resendQ to be resent.  This is the only place that a retry can be
 *   generated from.
 *
 *  If any packet is resent more than 3 times, then the entire tunnel is
 *   pulled down.  Therefore it is imperative that packets be removed
 *   from the resendQ in a timely fashion using l2f_clean_resendQ.
 */
static void l2f_run_resendQ (void)
{
    l2f_resend_elem *resend, *next;
    uchar packet_type;
    l2f_headertype *header;
    l2f_midtype *mid;

    for (resend = l2f_resendQ.qhead; resend != NULL; resend = next) {
        next = resend->next;

	/* if this packet hasn't timed out, ignore it */
	if (SLEEPING(resend->last_sent)) {
	    continue;
	}

        /*
         * Get the Mid using the Remote CLID and local mid
         */
        header = (l2f_headertype *) resend->data;
	mid = l2f_get_mid( 
          (resend->tunnel->instigator? resend->tunnel->gw_clid : 
                   resend->tunnel->nas_clid), header->mid);

        /*
         * Resend if less than max
         */
	if (resend->times_resent < L2F_RETRY_RESEND ) {
	    if (l2f_event_debug) 
		buginf("\nL2F resending packet, time #%d", 
                        resend->times_resent);
	    l2f_send(resend->tunnel, resend->data, resend->datalen, 
				resend->times_resent);
	    unqueue(&l2f_resendQ, resend);
	    free(resend->data);
	    free(resend);
	    continue;
	}

	/* 
	 * time to take drastic measures... 
	 */
	
	packet_type = l2f_get_packet_type(resend->data);

	if (packet_type == L2F_CLOSE) {
	   if (header->mid != L2F_MANAGEMENT_MID) {
	       /* 
                * The MID has sent the L2F_CLOSE but has not received an
                * L2F_CLOSE response.
                * The tunnel probably is dead. Leave it for the tunnel
                * to timeout on its keepalive and clean up. Here, we 
                * just cleanup the mid (and its resendQ)
		*/
                if (mid)
                  l2f_destroy_mid_structure(mid, TRUE);
                else
		  l2f_clean_resendQ(resend->tunnel, L2F_CLOSE, header->mid);

                /*
		 *  We may have destroyed the packets on the resendQ
		 *  with the above, so bail and deal with the rest of
		 *  the packets on the next pass.
		 */
		return;
	   }
	}

	switch(packet_type) {
	    case L2F_CONF:
	    case L2F_OPEN:
		/* must be for tunnel, MID is covered above */
	    case L2F_CLOSE:
		/* must be for tunnel, MID is covered above */
	    case L2F_ECHO:
		/* 
		 *  If you're not going to talk to us, we'll take our
		 *   marbles and go home.
		 */
		if (l2f_event_debug)
		    buginf("\nL2F resend packet (type %d) around too long, "
		    	"time to kill off the tunnel", packet_type);
		l2f_clear_tunnel(resend->tunnel);
		/*
		 * we may have destroyed have the packets on the resendQ
		 *  with the above, so bail and deal with the rest of
		 *  the packets on the next pass.
		 */
		return;
		break;
	    default:
		break;
	}

	

    }
}

/*
 * l2f_run_tunnel_periodic
 *  
 *  Do all those little housekeeping chores on the tunnels.  Send echos
 *  where necessary, time the tunnel info out if it's not local, etc.
 */
static void l2f_run_tunnel_periodic (void) 
{
    l2f_tunneltype *tunnel, *next;
    int length, tunnel_size;
    uchar *buf;

    tunnel_size = QUEUESIZE(&l2f_tunnelQ);
    tunnel = l2f_getnext_tunnel();
    while ((tunnel_size > 0) && tunnel) { 
	next = tunnel->next;
	switch (tunnel->state) {
	  case L2FTUNNEL_CLOSED:
	    l2f_clear_tunnel(tunnel);
	    l2f_destroy_tunnel_structure(tunnel);
	    if (l2f_event_debug)
		buginf("\nL2F Deleted inactive tunnel");
	    break;
	  case L2FTUNNEL_OPENING:
	    /*
	     *  If the tunnel has been opening for more than 10 seconds,
	     *    Kill it off.
	     */
	    if (CLOCK_OUTSIDE_INTERVAL(tunnel->state_time, 10*ONESEC)) {
		l2f_clear_tunnel(tunnel);
		l2f_destroy_tunnel_structure(tunnel);
	    }
	    break;
	  case L2FTUNNEL_OPEN:
	    if (AWAKE(tunnel->last_traffic) && !tunnel->echo_outstanding) {
                buf = malloc_named(MAX_L2FMANAGE_SIZE, "l2f tnlper");
                if (!buf)
	           return;
		length = l2f_echo(tunnel, buf, MAX_L2FMANAGE_SIZE);
		if (length > 0) {
		    l2f_send(tunnel, buf, length, INITIAL_SEND);
		    tunnel->echo_outstanding = TRUE;
		}
                free(buf);
	    }
	    /*
	     *  clean up any echo responses that are laying about
	     */
	    l2f_clean_resendQ(tunnel, L2F_ECHO_RESP, L2F_MANAGEMENT_MID);
	    break;
	  case L2FTUNNEL_CLOSING:
	    l2f_clear_tunnel(tunnel);
	    l2f_destroy_tunnel_structure(tunnel);
            if (l2f_event_debug)
	       buginf("\nL2F Deleted inactive tunnel");
	    break;
	  default:
	    break;
	}
        tunnel = l2f_getnext_tunnel();
        tunnel_size--;   
    }
}

#ifdef NO_ONE_CALLS_ME
/*
 *  l2f_run_mid_periodic
 *
 *  Take care of anything that needs to be done with a MID periodically.
 */
void l2f_run_mid_periodic (void)
{
    l2f_midtype *mid;
    l2f_tunneltype *tunnel;
    int tunnel_size, mid_size;

    tunnel_size = QUEUESIZE(&l2f_tunnelQ);
    tunnel = l2f_getnext_tunnel();

    while ((tunnel_size > 0) && tunnel) {
    
        mid_size = QUEUESIZE(&tunnel->tunnel_mid_elemQ);
        mid = l2f_getnext_mid_from_tunnel(tunnel);
        while ((mid_size > 0) && mid) {
	  switch (mid->state) {
	    case L2FMID_CLOSED:
	    case L2FMID_WAITING_FOR_TUNNEL:
	    case L2FMID_OPENING:
	    case L2FMID_OPEN:
	      break;
	    case L2FMID_CLOSING:
	      if (CLOCK_OUTSIDE_INTERVAL(mid->state_time, 10*ONESEC)) {
	          l2f_destroy_mid_structure(mid, TRUE);
	      }
	      break;
	    default:
	    break;
          }
	  mid = l2f_getnext_mid_from_tunnel(tunnel);
          mid_size--;
        }
        tunnel = l2f_getnext_tunnel();
        tunnel_size--;        
    }
}
#endif NO_ONE_CALLS_ME

/*
 * l2f_check_checksum
 *
 *  Pretty much what it sounds like.  Given a packet, tell if the checksum
 *  (the two bytes following the data buffer passed) is correct.
 */
static boolean l2f_check_checksum (uchar *data)
{
    l2f_headertype *header;
    ushort given_checksum, calculated_checksum;
    uchar *ptr;

    header = (l2f_headertype *) data;

    ptr = data + header->length;

    /* checksum is sent in reversed byte order */
    given_checksum = ptr[0];
    given_checksum |= ptr[1] << 8;

    calculated_checksum = h2n(pppfcs(PPPINITFCS, data, header->length));

    calculated_checksum ^= 0xffff;                 /* complement */

    return(given_checksum == calculated_checksum);

}

static void l2f_data_packets (void)
{
    l2f_tunneltype *tunnel;
    int count = PAK_SWITCHCOUNT;
    paktype *pak;

    while (count-- > 0) {
        pak = process_dequeue(l2f_data_pakQ);
	if (!pak)       /* should never happen */
	    return;

        if (!pak->if_output) {
	    datagram_done(pak);
            continue;
	}

        /*
         * If the Tunnel or mid has disappeared, get rid of the pak
         */
	if (!pak->if_output->l2f_mid || !pak->if_output->l2f_mid->owner ||
            (pak->if_output->l2f_mid->owner->state != L2FTUNNEL_OPEN) ||
             !pak->if_output->l2f_mid->owner->sock) {
	    datagram_done(pak);
	    continue;
	}

	tunnel = pak->if_output->l2f_mid->owner;

	pak->if_output = NULL;

	/* IP specific below here */

	/* leave room for the encapsulation */
	pak->network_start = pak->datagramstart -
		    (IPHEADERBYTES(NOPT) + UDPHEADERBYTES);

        /*
         * Snap the frame to alignment if required
         */
        ALIGN_DATAGRAM_CMD(pak_realign(pak, NULL));

        /*
         * Mark this as a L2F packet so that there is no UDP checksum. 
         * Note that this flag is also used to signify a tunnel in a tunnel
         * problem. These are two different issues.
         */
        pak->flags |= PAK_VPN_USED;

	ip_udp_send (pak, tunnel->sock->laddress.addr.ip_address, 
		     tunnel->sock->faddress.addr.ip_address,
		     tunnel->sock->lport, tunnel->sock->fport);

    }
}

/*
 * l2f_switch_ip_socket
 *
 * Today, HGW dynamic discover its tunnel destination ip address, by
 * using NAS's UDP source ip address.
 * In case of NAS's orginal source ip address fail over to a secondary
 * address, the HGW need to adjust its tunnel destination ip address.
 * This is the function where the HGW will do that.
 *
 * Note, the NAS's UDP source ip address may not be the ip address of
 * the interface the packet goes out of. Hence, the dynamic discovery
 * method can only ensure when the primary ip address fails, we can
 * always fail over to the secondary. However, it does not ensure
 * when the primary comes up, the data packet will goes back to the
 * primary right away.
 * Thru testing, it shows under heavy traffic, the source ip address
 * has a higher choice to be different form the interface ip address.
 * The recommended method is to use sgbp source-ip or vpdn source-ip
 * than the whole failover/recover scheme is complelety independent on
 * the UPD source address generation scheme (ip_best_local_address()).
 */
 static void l2f_switch_ip_socket(l2f_tunneltype *tunnel, ipaddrtype ip_address) 
{
    ipsocktype *soc = NULL;
    addrtype daddr, saddr;

    if (!tunnel)
	return;

    if (l2f_event_debug) {
	buginf("\nL2F NAS %s change its ip address from %i to %i\n", 
	       tunnel->nas_name, tunnel->ip_address, ip_address);
    }

    tunnel->ip_address = ip_address;
    daddr.ip_addr = tunnel->ip_address;
    saddr.ip_addr = 0;
    if (tunnel->source_ip) {
	if (tunnel->source_ip != -1)
	    saddr.ip_addr = tunnel->source_ip;
    }
    else
	saddr.ip_addr = vpn_source_ip_address;
	
    if (saddr.ip_addr) {
	soc = (ipsocktype *) open_ipsocket(UDP_PROT, &saddr,
		&daddr, L2F_PORT, L2F_PORT, SOCK_ANYSRC | SOCK_SRC_SPECIFIED);
    } else {
	soc = (ipsocktype *) open_ipsocket(UDP_PROT, 0, 
		&daddr, L2F_PORT, L2F_PORT, SOCK_ANYSRC | SOCK_SRC_DYN);
    }

    if (soc) {
	close_ipsocket(tunnel->sock);
	tunnel->sock = soc;
	if (l2f_event_debug) {
	    if (saddr.ip_addr)
		buginf("\nL2F Open UDP socket from %i to %i",
		       tunnel->sock->laddress.ip_addr, 
		       tunnel->sock->faddress.ip_addr);
	    else
		buginf("\nL2F Open UDP socket to %i",
		       tunnel->sock->faddress.ip_addr);
	}
    }
}

/*
 *  l2f_receive_management_packet
 *
 *  This is run at process level, and drives the tunnel and mid state machines.
 *   It does this by taking management packets off of the queue and 
 *   interpreting them.
 */
static void l2f_receive_management_packet (void)
{
    paktype *pak;
    uchar *payload;
    l2f_headertype *header;
    l2f_shortheadertype *shortheader;
    int count, payload_size;
    l2f_tunneltype *tunnel;
    l2f_midtype *mid;
    uchar osequence;
    boolean offset;
    long key;

    while((pak = process_dequeue(l2f_manage_pakQ))) {
	if (!pak)       /* should never happen */
	    return;

	header = (l2f_headertype *) pak->datagramstart;
	shortheader = (l2f_shortheadertype *) pak->datagramstart;

	offset = (header->version_and_flags & L2F_OFFSET);

	if (offset) {
	    payload = pak->datagramstart + sizeof(l2f_headertype) + 
				header->offset;
	} else {
	    payload = pak->datagramstart + sizeof(l2f_shortheadertype);
	}

	if (!(header->version_and_flags & L2F_KEY)) {
	    /* if no key is present, payload actually starts four bytes
		earlier*/
	    payload -= sizeof(long);
	}

	tunnel = l2f_find_tunnel_by_clid(GETSHORT(&header->clid));

	if (tunnel) {
	    tunnel->packets_in++;
	    tunnel->bytes_in += header->length;
	    /* 
	     * if we know about this tunnel, check sequence number 
	     */
	    if (header->mid && !l2f_ignore_mid_sequence) {
		/* 
		 *  This is a MID management packet 
		 */
		mid = l2f_get_mid(GETSHORT(&header->clid), 
				GETSHORT(&header->mid));
		/*
		 *  If the MID is null, this is probably an L2F_OPEN for
		 *   a new MID.  Otherwise, handle the sequencing.
		 */
		if (mid != NULL) {
		    mid->packets_in++;
		    mid->bytes_in += header->length;
		    osequence = header->sequence;
		    osequence -= mid->incoming_sequence + 1 ;
		    if (osequence > 127) {
			/* out of sequence packet received */
			if (l2f_error_debug)
			    buginf("\nL2F Out of sequence" 
				" MID(%d) packet %d (wanted %d)", header->mid, 
				header->sequence, mid->incoming_sequence);
			datagram_done(pak);
			return;
		    }
		    mid->incoming_sequence = header->sequence;
		}
	    } else {
		/* 
		 *  This is a tunnel management packet 
		 */
		osequence = header->sequence;
		osequence -= tunnel->incoming_sequence + 1 ;
		if (osequence > 127) {
		    /* out of sequence packet received */
		    if (l2f_error_debug)
			buginf("\nL2F Out of sequence" 
			    " tunnel packet %d (wanted %d)",
			    header->sequence, tunnel->incoming_sequence);
		    datagram_done(pak);
		    return;
		}
		tunnel->incoming_sequence = header->sequence;
	    }


	    TIMER_START(tunnel->last_traffic, L2F_IDLE_TIME);
	    if (tunnel->key_exists) {
	       if (offset) {
		   key = header->key;
	       } else {
		   key = shortheader->key;
	       }
	       if (tunnel->instigator) {
		    /* we're the NAS, check the GW key */
		    if (key != tunnel->gw_key) {
			if (l2f_error_debug)
			   buginf("\nL2F packet has bogus1 key %x %x",
				key, tunnel->gw_key);
			if (vpn_error_debug)
			   buginf("\nVPDN tunnel management packet failed"
				  " to authenticate");
			datagram_done(pak);
			return;
		    }
	       } else {
		    /* we're the GW, check the NAS key */
		    if (key != tunnel->nas_key) {
	 		if (l2f_error_debug)
			   buginf("\nL2F packet has bogus2 key %x %x",
				key, tunnel->nas_key);
			if (vpn_error_debug)
			   buginf("\nVPDN tunnel management packet failed"
				  " to authenticate");
			datagram_done(pak);
			return;
		    }
	       }
	    }
	}

	if (header->version_and_flags & L2F_CHECKSUM) {
	    if (!l2f_check_checksum(pak->datagramstart)) {
		/* bad packet received */
		if (l2f_error_debug)
		    buginf("\nL2F Bad Checksum!");
		datagram_done(pak);
		return;
	    }
	}

	if (l2f_packet_debug) {
	    if (tunnel) 
		buginf("\nL2F RECEIVED from %i to %i (%d):", 
		       tunnel->sock->faddress.ip_addr, 
		       tunnel->sock->laddress.ip_addr,
		       pak->datagramsize);
	    else
		buginf("\nL2F RECEIVED (%d):", pak->datagramsize);
	    for (count =0; count < pak->datagramsize; count++) {
		buginf(" %x", pak->datagramstart[count]);
	    }
	}

	/*
	 * For HGW if the NAS's source address (pak->desthost) changes, 
	 * we will have to change the HGW tunnel's destination address.
	 * NAS orginal interface may have gone down, and fail over to
	 * the  backup interface.
	 * For NAS, we don't have this issue, since NAS tunnel
	 * destination address is handed from SGBP or VPDN, while HGW 
	 * dynamicly discover its destination address from the NAS 
	 * source address
	 */
	if (tunnel && !tunnel->instigator && 
	    tunnel->ip_address != pak->desthost)
	    l2f_switch_ip_socket(tunnel, pak->desthost);

	payload_size = header->length - (payload - pak->datagramstart);

	switch (payload[0]) {
	    case L2F_CONF:
		if (l2f_event_debug)
		    buginf("\nL2F_CONF received");
		l2f_receive_l2f_conf(header, payload, payload_size, 
				pak->desthost);
		break;
	    case L2F_OPEN:
		if (l2f_event_debug)
		    buginf("\nL2F_OPEN received");
		l2f_receive_l2f_open(header, payload, payload_size);
		break;
	    case L2F_CLOSE:
		if (l2f_event_debug)
		    buginf("\nL2F_CLOSE received");
		l2f_receive_l2f_close(header, payload, payload_size);
		break;
	    case L2F_ECHO:
		if (l2f_event_debug)
		    buginf("\nL2F_ECHO received");
		l2f_receive_l2f_echo(header, payload, payload_size);
		break;
	    case L2F_ECHO_RESP:
		if (l2f_event_debug)
		    buginf("\nL2F_ECHO_RESP received");
		if (tunnel) {
		    l2f_clean_resendQ(tunnel, L2F_ECHO, L2F_MANAGEMENT_MID);
		    tunnel->echo_outstanding = FALSE;
		}
		break;
	    default:
		if (l2f_error_debug)
		    buginf("\nL2F Bogus payload in L2F management packet");
		break;
	}

	datagram_done(pak);
    }
}

/* 
 *  l2f_management daemon
 *
 *  The core L2F management process.  
 */
static process l2f_management_daemon (void) 
{
    ulong major, minor;
    sys_timestamp l2f_periodic;

    l2f_manage_pakQ = create_watched_queue("L2F management packets", 0, 
						L2FMANAGE_QUEUE);
    TIMER_START(l2f_periodic, 5*ONESEC);
    process_watch_timer(&l2f_periodic, ENABLE);
    process_watch_queue(l2f_manage_pakQ, ENABLE, RECURRING);

    while (TRUE) {
	process_wait_for_event();
	while (process_get_wakeup(&major, &minor)) {
	    switch (major) {
	      case TIMER_EVENT:
		TIMER_START(l2f_periodic, 5*ONESEC);
		l2f_run_tunnel_periodic();
		l2f_run_resendQ();
		break;
	      case QUEUE_EVENT:
		switch (minor) {
		  case L2FMANAGE_QUEUE:
		    l2f_receive_management_packet();
		    break;
		  default:
		    errmsg(&msgsym(UNEXPECTEDEVENT,SCHED), major, minor);
		    break;
		}
		break;
	      default:
		errmsg(&msgsym(UNEXPECTEDEVENT,SCHED), major, minor);
		break;
	    }
	}
    }
}

/* 
 *  l2f_data_daemon
 *
 *  The core L2F data packet switching process.  
 */
static process l2f_data_daemon (void) 
{
    ulong major, minor;

    l2f_data_pakQ = create_watched_queue("L2F data packets", 0, 
						L2FDATA_QUEUE);

    process_watch_queue(l2f_data_pakQ, ENABLE, RECURRING);

    while (TRUE) {
	process_wait_for_event();
	while (process_get_wakeup(&major, &minor)) {
	    switch (major) {
	      case QUEUE_EVENT:
		switch (minor) {
		  case L2FDATA_QUEUE:
		    l2f_data_packets();
		    break;
		  default:
		    errmsg(&msgsym(UNEXPECTEDEVENT,SCHED), major, minor);
		    break;
		}
		break;
	      default:
		errmsg(&msgsym(UNEXPECTEDEVENT,SCHED), major, minor);
		break;
	    }
	}
    }
}

/*
 *  l2f_close_tunnel_structure
 */

static void l2f_close_tunnel_structure (l2f_tunneltype *tunnel)
{
    close_ipsocket(tunnel->sock);    
    l2f_clean_resendQ(tunnel, L2F_ANY_TYPE, L2F_MANAGEMENT_MID);

    /*
     * reset tunnel back to its native state 
     */
    tunnel->sock = 0;
    tunnel->version_and_flags = L2F_VERSION;
    tunnel->do_checksum = FALSE;
    tunnel->offset_exists = TRUE;
    tunnel->protocol = L2F_PPP;
    tunnel->echo_outstanding = FALSE;
    tunnel->midcount = 0;
    tunnel->next_mid = 1;
    tunnel->key_exists = FALSE;
    tunnel->outgoing_sequence = 0;
    tunnel->incoming_sequence = 255;
    if (tunnel->instigator) {
	tunnel->nas_clid = 0;
    } else {
	tunnel->gw_clid = 0;
    }
    if (l2f_event_debug)
	buginf("\nL2F Closed tunnel structure");
    l2f_tunnel_move_to_state(tunnel, L2FTUNNEL_CLOSED);
}

/*
 * l2f_make_tunnel_structure
 *
 *  Allocate and initialize a tunnel
 */
l2f_tunneltype *l2f_make_tunnel_structure (l2f_tunnel_infotype *info)
{
    l2f_tunneltype *l2f_tunnel;

    l2f_tunnel = malloc_named(sizeof(l2f_tunneltype), "l2f tnlinit");

    if (l2f_tunnel == NULL) {
	return(NULL);
    }

    bzero(l2f_tunnel, sizeof(l2f_tunneltype));

    l2f_tunnel->version_and_flags = L2F_VERSION;
    l2f_tunnel->do_checksum = FALSE;  /* Never do checksum */
    l2f_tunnel->offset_exists = TRUE;
    l2f_tunnel_move_to_state(l2f_tunnel, L2FTUNNEL_CLOSED);
    l2f_tunnel->protocol = L2F_PPP;
    l2f_tunnel->next_mid = 1;
    l2f_tunnel->incoming_sequence = 255;
    l2f_tunnel->packets_in = 0;
    l2f_tunnel->bytes_in = 0;
    l2f_tunnel->packets_out = 0;
    l2f_tunnel->bytes_out = 0;
    random_fill(l2f_tunnel->nas_challenge, VPN_CHALLENGE_SIZE, TRUE);
    l2f_tunnel->nas_challenge_length = VPN_CHALLENGE_SIZE;
    random_fill(l2f_tunnel->gw_challenge, VPN_CHALLENGE_SIZE, TRUE);
    l2f_tunnel->gw_challenge_length = VPN_CHALLENGE_SIZE;

    if (!l2f_store_tunnel(l2f_tunnel)) {
        free(l2f_tunnel);
        return(NULL);
    }

    strcpy(l2f_tunnel->nas_name, info->nas_name);
    strcpy(l2f_tunnel->gw_name, info->gw_name);
    strcpy(l2f_tunnel->gw_domain, info->gw_domain);

    if (info->nas_password[0]) {
	strcpy(l2f_tunnel->nas_password, info->nas_password);
    }
    if (info->gw_password[0]) {
	strcpy(l2f_tunnel->gw_password, info->gw_password);
    }


    l2f_tunnel->type = info->type;
    l2f_tunnel->source_ip = info->source_ip;

    if (info->direction == L2F_INCOMING) {
             /*  We're the Home-Gateway */
        l2f_tunnel->nas_clid = get_new_tunnel_clid();
        l2f_tunnel->virt_temp = info->virt_temp;
	l2f_tunnel->instigator = FALSE;
    } else { 
              /* We're the NAS */
	l2f_tunnel->instigator = TRUE;
        l2f_tunnel->gw_clid = get_new_tunnel_clid();
        l2f_tunnel->ip_address = info->ip_address;
    }
    
    return(l2f_tunnel);
}

/*
 * Create the L2F daemon, if not already created
 */
static void l2f_create_daemons (void)
{
    if (!l2f_enable) {
	l2f_management_daemon_pid = process_create(l2f_management_daemon,
						   "L2F management daemon", 
						   LARGE_STACK,
						   PRIO_NORMAL);
	l2f_data_daemon_pid = process_create(l2f_data_daemon,
					     "L2F data daemon",
					     LARGE_STACK,
					     PRIO_NORMAL);
	process_set_crashblock(l2f_management_daemon_pid, TRUE);
	process_set_crashblock(l2f_data_daemon_pid, TRUE);

	l2f_enable = TRUE;
    }
}

/*
 * l2f_command
 *
 *  Parser configuration command for L2F
 */

void l2f_command (parseinfo *csb)
{
    l2f_tunnel_infotype *info;

    if (csb->nvgen) {
	if (l2f_configured) {
	    nv_write(TRUE, "%s enable", csb->nv_command);
	    for (info=local_tunnels; info != NULL; info=info->next) {
		if (info->direction == L2F_OUTGOING) {
		    nv_write(TRUE, "%s outgoing %s %s ip %i", 
			    csb->nv_command, 
			    info->gw_domain,
			    info->nas_name,
			    info->ip_address);
		} else {
		    nv_write(TRUE, "%s incoming %s %s virtual-template %d", 
			    csb->nv_command, 
			    info->nas_name,
			    info->gw_name,
			    info->virt_temp);
		}
	    }
	    nv_write(vpn_force_local_chap, "%s force-local-chap", 
					csb->nv_command);
	    nv_write(l2f_ignore_mid_sequence, "%s ignore-mid-sequence", 
					csb->nv_command);
	    nv_write(vpn_source_ip_address, "%s source-ip %i",
		                        csb->nv_command,
					vpn_source_ip_address);
	    return; 
	}
	return;
    }

    if (csb->which == L2F_ENABLE) {
	if (csb->sense) {
	    l2f_configured = TRUE;
	    l2f_create_daemons();
	} else {
	    l2f_configured = FALSE;
	}
	return;
    }

    if (!l2f_configured) {
	printf("%% VPDN is not enabled\n");
	return;
    }

    if (csb->which == L2F_FORCE_LOCAL_CHAP) {
	vpn_force_local_chap = csb->sense;
	return;
    }

    if (csb->which == L2F_IGNORE_MID_SEQUENCE) {
	l2f_ignore_mid_sequence = csb->sense;
	return;
    }
    if (csb->which == L2F_SOURCE_IP_ADDRESS) {
	if (csb->sense) {
	    if (GETOBJ(paddr,1)) 
		vpn_source_ip_address = GETOBJ(paddr,1)->ip_addr;
	}
	else
	    vpn_source_ip_address = 0L;
	return;
    }

    if (!csb->sense) {
        if (!l2f_undefine_tunnel(csb->which, GETOBJ(string,1))) {
	    printf("%% Couldn't find tunnel %s to delete.\n",
		   GETOBJ(string,1));
	}
	return;
    }

    if (l2f_find_tunnel_by_name(GETOBJ(string,1))) {
	printf("There is already a VPN tunnel description for %s.\n",
		GETOBJ(string,1));
	return;
    }

    l2f_define_tunnel(GETOBJ(string,1), GETOBJ(string,2),
                               GETOBJ(paddr,1)->ip_addr,
                               GETOBJ(int,1),
                               csb->which);

}

static boolean l2f_undefine_tunnel (int direction, char *name)
{
    l2f_tunnel_infotype *info, *prev;
    char *search_name;

    info = local_tunnels;

    if (!info)
       return(FALSE);

    if (direction == L2F_OUTGOING) {
        search_name = info->gw_domain;
    } else {
        search_name = info->nas_name;
    }

    if (!strcmp(search_name, name)) {
	local_tunnels = info->next;
	free(info);
	return(TRUE);
    }

    prev = local_tunnels;
    for (info = local_tunnels->next; info != NULL; info = info->next) {
	if (direction == L2F_OUTGOING) {
	    search_name = info->gw_domain;
	} else {
	    search_name = info->nas_name;
	}

	if (!strcmp(search_name, name)) {
	    prev->next = info->next;
	    free(info);
	    return(TRUE);
	}
	prev = info;
	
    }

    return(FALSE);
}

/*
 * Create a Tunnel. Note that this tunnel is created locally. 
 * The end-to-end creation is dynamic.
 */
static l2f_tunnel_infotype *l2f_define_tunnel (char *first,
					       char *second,
					       ipaddrtype ip_address,
					       int vtemplate_num,
					       int direction)
{
    l2f_tunnel_infotype *info, *ptr;
 
    info = malloc_named(sizeof(l2f_tunnel_infotype), "l2f tnlinfo");
    if (!info)
       return(NULL);

    if (direction == L2F_INCOMING) {
	strcpy(info->nas_name, first);
	strcpy(info->gw_name, second);
    } else {
	strcpy(info->gw_domain, first);
	strcpy(info->nas_name, second);
    }
    info->direction = direction;
    info->type = VPDN_TUNNEL;
    info->virt_temp = vtemplate_num;
    info->ip_address = ip_address;
    info->next = NULL;
	
    if (!local_tunnels) {
	local_tunnels = info;
	return(info);
    } else {
	for (ptr = local_tunnels; ptr != NULL; ptr = ptr->next) {
	    if (ptr->next == NULL) {
		ptr->next = info;
		return(info);
	    }
	}
    }
    return(info);
}



/*
 * print_l2f_header
 *
 *  Called from debugging code, gives a quick summary of a packet header 
 */
static void
print_l2f_header (l2f_headertype *header)
{
    l2f_shortheadertype *shortheader;
    if (header->version_and_flags & L2F_OFFSET) {
	buginf("\nL2F header flags: %d version %d protocol %d sequence %d "
		"mid %d cid %d length %d offset %d key %d",
		header->version_and_flags,
		header->version_and_flags,
		header->protocol,
		header->sequence,
		header->mid,
		GETSHORT(&header->clid),
		header->length,
		header->offset,
		header->key);
    } else {
	shortheader = (l2f_shortheadertype *) header;
	buginf("\nL2F header flags: %d version %d protocol %d sequence %d "
		"mid %d cid %d length %d key %d",
		header->version_and_flags,
		header->version_and_flags,
		header->protocol,
		header->sequence,
		header->mid,
		GETSHORT(&header->clid),
		header->length,
		header->key);
    }
}

/*
 *  l2f_ip_udp_input 
 *
 *  The point where UDP packets enter the L2F subsystem.
 */
static boolean
l2f_ip_udp_input (paktype *pak, udptype *udp)
{
    l2f_headertype *l2f_header;
    iphdrtype *ip;

    if (udp->dport != L2F_PORT)
        return(FALSE);

    if (!l2f_enable)
	return(FALSE);

    ip = (iphdrtype *)ipheadstart(pak);

    /*  
     * The return packet needs to go to the source address.  Store
     *	that info away now. 
     */
    pak->desthost = ip->srcadr;

    /* strip off IP header and move pkt to datagramstart */

    pak->datagramstart = (uchar *) udp + UDPHEADERBYTES;
    pak->datagramsize  = udp->length - UDPHEADERBYTES;
    pak->network_start = pak->datagramstart;

    l2f_header = (l2f_headertype *) pak->datagramstart; 

    if (l2f_packet_debug)
	print_l2f_header(l2f_header);

    if (l2f_header->version_and_flags & L2F_CHECKSUM) {
	if (!l2f_check_checksum(pak->datagramstart)) {
	    /* bad packet received */
	    if (l2f_error_debug)
		buginf("\nL2F Bad Checksum!");
	    datagram_done(pak);
	    return(TRUE);
	}
    }

    if (l2f_header->protocol != L2F_PROTO) {
        /* This is a data packet, send it on its merry way */
        l2f_fwd_data_packet(pak);
    } else {
	/* otherwise, it's a managment packet... */
	process_enqueue(l2f_manage_pakQ, pak);
    }

    return(TRUE);
}

/*
 * l2f_fwd_data_packet
 *
 *  Take a packet from the net, and do all the tweaking to the pak structure
 *  to get the interface (hardware or virtual) to accept it without blinking.
 *
 *  Here there be tigers.
 */
static void l2f_fwd_data_packet (paktype *pak)
{
    l2f_headertype *header;
    l2f_shortheadertype *shortheader;
    hwidbtype *hwidb;
    l2f_midtype *mid;
    int len;
    int count;
    void (*iqueue)(hwidbtype *, paktype *);
    ulong key;
    boolean offset;

    header = (l2f_headertype *) pak->datagramstart;
    shortheader = (l2f_shortheadertype *) pak->datagramstart;

    offset = (header->version_and_flags & L2F_OFFSET);

    if (header->version_and_flags & L2F_PRIORITY) {
	pak->flags |= PAK_PRIORITY;
    }

    mid = l2f_get_mid(GETSHORT(&header->clid), header->mid);
    if (mid == NULL) {
	if (l2f_packet_debug)
	    buginf("\nL2F packet with no MID");
	datagram_done(pak);
 	return;
    }

    mid->packets_in++;
    mid->bytes_in += header->length;
    mid->owner->packets_in++;
    mid->owner->bytes_in += header->length;

    if (mid->owner->key_exists) {
       if (offset) {
	   key = header->key;
       } else {
	   key = shortheader->key;
       }
       if (mid->owner->instigator) {
	    /* we're the NAS, check the GW key */
	    if (key != mid->owner->gw_key) {
		if (l2f_error_debug)
		   buginf("\nL2F data packet has bogus key %d %d",
			    header->key, mid->owner->gw_key);
		datagram_done(pak);
		return;
	    }
       } else {
	    /* we're the GW, check the NAS key */
	    if (key != mid->owner->nas_key) {
		if (l2f_error_debug)
		   buginf("\nL2F data packet has bogus key %d %d",
			    header->key, mid->owner->nas_key);
		datagram_done(pak);
		return;
	    }
       }
    }

    TIMER_START(mid->owner->last_traffic, L2F_IDLE_TIME);

    hwidb = mid->idb;
    if (hwidb == NULL) {
        if (l2f_error_debug)
	   buginf("\nL2F Mid with no hwidb");
	datagram_done(pak);
	return;
    }

    pak->enctype       = ET_PPP;
    pak->flags &= ~PAK_MCICHECKED;

    if (offset) {
	len = sizeof(l2f_headertype);
	len += header->offset;
    } else {
	len = sizeof(l2f_shortheadertype);
    }

    if (!(header->version_and_flags & L2F_KEY)) {
	len -= sizeof(long);
    }

    pak->datagramstart += len;
    pak->datagramsize -= len;
    pak->dataptr = pak->datagramstart;

    if (header->version_and_flags & L2F_CHECKSUM) {
	 /* if there was also a checksum, slice off the last two bytes */
	 pak->datagramsize -= 2;
    }

    if (l2f_packet_debug) {
	buginf("\nL2F-IN ");
        if (mid->owner->instigator) 
           buginf("Output to %s", hwidb->hw_namestring);
        else
           buginf("Input to %s", hwidb->hw_namestring); 

        buginf(" (%d):", pak->datagramsize);
	for (count = 0; count < pak->datagramsize; count++) {
	   buginf(" %x", pak->datagramstart[count]);
	}
    }

    /*
     * Stick it to the other interface -- iqueue if this is the gateway,
     *   oqueue if this is the NAS.
     */
    if (mid->owner->instigator) {
	pak->if_output =  hwidb->firstsw;

        /*
         * If there's a board encap, call that first. PRIs. e.g.
         * needs that to distinguish which actual channel to go out
         * on
         */
        if (hwidb->board_encap) {
	   if (!(*hwidb->board_encap)(pak, hwidb)) {
              if (l2f_error_debug)
                 buginf("\nL2F Bad board encap on %s",
			     hwidb->hw_namestring);
              datagram_done(pak);
              return;
	   }
        }
        datagram_out(pak);
    } else {
        hwidb->counters.rx_cumbytes += pak->datagramsize;
        hwidb->counters.inputs++;
        change_if_input(pak, hwidb->firstsw);
        iqueue = ppp_parse_packet(hwidb, pak, NULL);
	(*iqueue)(hwidb, pak);
    }
}

/*
 * l2f_packet_send
 *
 *  Take a packet from an interface (hardware or virtual), slap the L2F
 *   header on it, and send it off to the other end of our tunnel.
 */
void l2f_packet_send (hwidbtype *hwidb, paktype *pak)
{
    l2f_tunneltype *tunnel;
    l2f_midtype *mid;
    l2f_headertype *header;
    int count;
    int offset = 0;

    /*
     *  Assume that we're sending this to a 4500.   We can save ourselves
     *   a lot of cycles realigning the packet on the other end if we
     *   just offset the data on this end to bring the necessary bits
     *   into alignment.
     */
    offset = (int) pak->datagramstart % 4;

    if (l2f_packet_debug) {
	buginf("\nL2F-OUT (%d):", pak->datagramsize);

	for (count = 0; count < pak->datagramsize; count++) {
	   buginf(" %x", pak->datagramstart[count]);
	}
    }

    mid = hwidb->firstsw->l2f_mid;
    if (mid == NULL) {
      if (l2f_error_debug)
         buginf("\nL2F Discarding packet because of no mid on %s",
                hwidb->hw_namestring);
       datagram_done(pak);
       return;
    }

    tunnel = mid->owner;

    if (tunnel->state != L2FTUNNEL_OPEN) {
       if (l2f_error_debug)
	   buginf("\nL2F Discarding data packet because tunnel isn't"
           " open on %s", hwidb->hw_namestring);
       datagram_done(pak);
       return;
    }

    pak->datagramstart -= (sizeof(l2f_headertype) + offset);
    pak->datagramsize += (sizeof(l2f_headertype) + offset);

    pak->if_output = hwidb->firstsw;


    header = (l2f_headertype *) pak->datagramstart;
    PUTSHORT(&header->version_and_flags, 1);
    header->protocol = tunnel->protocol;
    header->sequence = 0;
    PUTSHORT(&header->mid, mid->mid);
    PUTSHORT(&header->length, pak->datagramsize);

    if (tunnel->instigator) {
	PUTSHORT(&header->clid, tunnel->nas_clid);
    } else {
	PUTSHORT(&header->clid, tunnel->gw_clid);
    }

    if (tunnel->offset_exists) {
	PUTSHORT(&header->version_and_flags, 
		GETSHORT(&header->version_and_flags) | L2F_OFFSET);
	PUTSHORT(&header->offset, offset);
    }

    if (pak->flags & PAK_PRIORITY) {
	PUTSHORT(&header->version_and_flags, 
		GETSHORT(&header->version_and_flags) | L2F_PRIORITY);
    }

    if (tunnel->key_exists) {
	header->version_and_flags |= L2F_KEY;
	if (tunnel->instigator) {
	    header->key = tunnel->nas_key;
	} else {
	    header->key = tunnel->gw_key;
	}
    }

    if (l2f_packet_debug)
	print_l2f_header(header);

    tunnel->packets_out++;
    tunnel->bytes_out += header->length;
    mid->packets_out++;
    mid->bytes_out += header->length;
  
    process_enqueue(l2f_data_pakQ, pak);

}

/*
 * l2f_send
 *
 * This is used for sending internal management packets.  
 */
boolean
l2f_send (l2f_tunneltype *l2f_tunnel, uchar *data, int datalen, 
	  int times_resent)
{
    ushort checksum, reversed_checksum;
    l2f_headertype *header;
    l2f_midtype *mid;
    l2f_resend_elem *resend;
    uchar packet_type;
    boolean put_resend_queue = TRUE;
 
    header = (l2f_headertype *) data;
    packet_type = l2f_get_packet_type(data);
  
    /*
     * If this is a L2F_OPEN sent from the HG, don't put it in the resend
     * Q. The NAS will resend if it does not receive the expected L2F_OPEN 
     */
    if ((packet_type == L2F_OPEN) && !l2f_tunnel->instigator)
       put_resend_queue = FALSE;
  
    if (put_resend_queue) {

      /*
       *   Handle the resend operation first.
       */
      resend = malloc_named(sizeof(l2f_resend_elem), "l2f tnlresnd");
      if (!resend)
         return(FALSE);

      resend->next = NULL;
      resend->tunnel = l2f_tunnel;
      resend->times_resent = times_resent + 1;
      TIMER_START(resend->last_sent, L2F_RESEND_TIME);
      resend->datalen = datalen;
      /*
       *  Get the data size plus room for the checksum later 
       */
      resend->data = malloc_named(datalen + sizeof(short), "l2f tnlsend");
      if (!resend->data) {
         free(resend);
         return(FALSE);
      }
      bcopy(data, resend->data, datalen);
      enqueue(&l2f_resendQ, resend);
    }

    l2f_tunnel->packets_out++;
    l2f_tunnel->bytes_out += header->length;

    if (header->mid && !l2f_ignore_mid_sequence) {
        /*
         * Get the mid using the Remote CLID and local mid
         */ 
	mid = l2f_get_mid( 
          (l2f_tunnel->instigator? l2f_tunnel->gw_clid : l2f_tunnel->nas_clid),
          header->mid);
	if (!mid) {
	    if (l2f_error_debug)
		buginf("\nSending packet with no apparent MID!");
	} else {
	    header->sequence = mid->outgoing_sequence++;
	    mid->packets_out++;
	    mid->bytes_out += header->length;
	}
    } else {
	header->sequence = l2f_tunnel->outgoing_sequence++;
    }

    header->version_and_flags |= L2F_SEQUENCED;

    if (l2f_tunnel->do_checksum) {
	header->version_and_flags |= L2F_CHECKSUM;
	checksum = h2n(pppfcs(PPPINITFCS, data, header->length));

	checksum ^= 0xffff;                 /* complement */

	/* checksum is sent in reversed byte order */
	reversed_checksum = checksum >> 8;
	reversed_checksum |= checksum << 8;

	PUTSHORT(data+header->length, reversed_checksum);
	datalen += sizeof(ushort);
    }

    return(l2f_ip_send(l2f_tunnel, data, datalen));
}

/*
 * l2f_ip_send
 *
 * Take a L2F management packet headed for an IP pipe and send it on its way.
 * L2F data packets do not use this routine.
 */
static boolean
l2f_ip_send (l2f_tunneltype *l2f_tunnel, uchar *data, int datalen)
{

    paktype *pak;
    int start, count;

    if (!l2f_tunnel->sock)
	return(FALSE);

    pak = setup_ipsocket(l2f_tunnel->sock, datalen, &start);

    bcopy(data, pak->data_area + start, datalen);

    if (l2f_packet_debug) {
	buginf("\nL2F SENDING from %i to %i (%d):", 
	       l2f_tunnel->sock->laddress.ip_addr, 
	       l2f_tunnel->sock->faddress.ip_addr, datalen);
	for (count =0; count < datalen; count++) {
	    buginf(" %x", data[count]);
	}
    }

    /* ip_write */
    write_ipsocket(l2f_tunnel->sock, pak, datalen);
    return (TRUE);
}

/*
 * l2f_init
 *
 * Subsystem initialization for l2f (as part of VPN subsystem)
 */
void l2f_init (subsystype *subsys) 
{
    l2f_enable = FALSE;
    reg_add_l2f_create_daemon(l2f_create_daemons, "l2f_create_daemon");
    reg_add_ip_udp_input(l2f_ip_udp_input, "l2f_ip_udp_input");
    reg_add_vpn_peer_event_occured(l2f_stack_peer_event_listener,
				     "l2f_peer_event_listener");
    l2f_init_mid_storage();
    queue_init(&l2f_resendQ, 0);
    next_tunnel_clid = 1;

}


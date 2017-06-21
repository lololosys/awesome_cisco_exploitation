/* $Id: ppp_auth.c,v 3.13.8.35 1996/09/12 04:02:26 tkolar Exp $
 * $Source: /release/112/cvs/Xsys/wan/ppp_auth.c,v $
 *------------------------------------------------------------------
 * ppp_auth.c - PPP authentication protocol support
 *
 * 23-March-92 	W. Miskovetz
 *
 * Copyright (c) 1992-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ppp_auth.c,v $
 * Revision 3.13.8.35  1996/09/12  04:02:26  tkolar
 * CSCdi62277:  L2F tunnel authentication over Radius fails
 * Branch: California_branch
 * Allow cleartext passwords to be passed from Radius
 *
 * Revision 3.13.8.34  1996/09/10  06:16:42  fox
 * CSCdi68456:  Parallel connection information lost if one link
 * disconnects
 * Branch: California_branch
 * Expand check for parallel connections to protect IP dynamic dialer
 * maps.
 *
 * Revision 3.13.8.33  1996/09/06  03:49:08  fox
 * CSCdi67375:  ppp pap sent-username must be configured twice
 * Branch: California_branch
 * Copy chap and pap strings in copy_ppp_fields() like all other LCP
 * and authentication parameters.
 *
 * Revision 3.13.8.32  1996/09/06  03:28:34  fox
 * CSCdi68282:  PAP hostnames truncated to 3 characters
 * Branch: California_branch
 * When char user[50]; changed to char *user;, sizeof(user) went from
 * 50 to 4.
 *
 * Revision 3.13.8.31  1996/08/30  07:28:18  fox
 * CSCdi52745:  username limited to 49 characters
 * Branch: California_branch
 * Compute 'userlen' as length of username not of string to hold the
 * username.
 *
 * Revision 3.13.8.30  1996/08/29  20:17:06  sbelair
 * CSCdi52745:  username limited to 49 characters
 * Branch: California_branch
 * Fix problems (found by Craig Fox) with last check-in: can't use strlen
 * on a non-null-terminated string.
 *
 * Revision 3.13.8.29  1996/08/28  13:22:29  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.13.8.28  1996/08/27  21:43:55  sbelair
 * CSCdi52745:  username limited to 49 characters
 * Branch: California_branch
 * Instead of copying the username into an array of 50 bytes on the
 * stack, malloc the necessary space for the username once we know
 * how long it is, and the free the buffer later.
 *
 * Revision 3.13.8.27  1996/08/18  09:09:03  fox
 * CSCdi58100:  PPP option types displayed inconsistently
 * Branch: California_branch
 * Consistently display PPP codes and types in decimal.  Convert several
 * occurrences of '0x%x' to '%#x'.
 *
 * Revision 3.13.8.26  1996/08/16  05:22:52  fox
 * CSCdi62495:  Outbound PAP fails with tacacs+ authentication
 * Branch: California_branch
 * password field in astruct is not zero-terminated, use passed length
 *
 * Revision 3.13.8.25  1996/08/07  22:50:46  tkolar
 * CSCdi62172:  VPDN dual CHAP challenge from Home-Gateway
 * Branch: California_branch
 * Merge two disparate VPDN authentication schemes.
 *
 * Revision 3.13.8.24  1996/08/07  09:04:37  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.13.8.23  1996/07/30  20:18:37  dblair
 * CSCdi59050:  multilink broken when dialing Ascend with 2 phone numbers
 * Branch: California_branch
 * Adding support for PPP LCP endpoint discriminator
 *
 * Revision 3.13.8.22  1996/07/25  16:50:23  syiu
 * CSCdi63878:  VPDN CHAP challenge size less than default not working
 * Branch: California_branch
 *
 * Revision 3.13.8.21  1996/07/23  13:32:02  thille
 * CSCdi63329: Centralize logging for getbuffer() and pak_duplicate()
 * failures
 * Branch: California_branch
 * Shamelessly rip off Mr. Katz' code from CSCdi36037.  Provide
 * centralized logging for getbuffer() and pak_duplicate() failures and
 * then rip out the 9 bazillion different messages that where in the
 * various callers to report the failure.
 *
 * Revision 3.13.8.20  1996/06/28  23:32:21  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.13.8.19  1996/06/27  16:36:55  hampton
 * Keep your mitts off the scheduler private data structures.  [CSCdi61518]
 * Branch: California_branch
 *
 * Revision 3.13.8.18  1996/06/25  21:05:25  fox
 * CSCdi60877:  duplicate CHAP response when should not
 * Branch: California_branch
 * Don't zero PAP and CHAP packet IDs for each Authentication Phase.
 * Ignore obsolete authentication requests when looking for duplicate
 * responses.
 *
 * Revision 3.13.8.17  1996/06/05  19:49:43  fox
 * CSCdi59586:  PPP authentication hangs when timer is stopped
 * Branch: California_branch
 * Keep timer running until bidirectional authentication is complete.
 *
 * Revision 3.13.8.16  1996/06/05  15:27:15  fox
 * CSCdi59558:  Received PAP request but did not negotiate PAP
 * Branch: California_branch
 * go->authtypes[] now hold internal parser types for PAP & CHAP
 *
 * Revision 3.13.8.15  1996/05/30  18:03:55  fox
 * CSCdi57978:  calls stay up after line protocol of b channel go
 * down(under stress)
 * Branch: California_branch
 * Call lcp_close() instead of lcp_down() when max CHAP timeouts reached.
 * Save copy of CHAP Response packet for next Authentication timeout.
 * Resend if no CHAP Success or Failure packet received.
 *
 * Revision 3.13.8.14  1996/05/28  13:35:51  fox
 * CSCdi57337:  Remote PAP authentication broken for AAA/TACACS+ configs
 * Branch: California_branch
 * Re-enable support for AAA/TACACS+ to provide the remote PAP password.
 * If 'pap sent-username' is configured use that, else try AAA/TACACS+.
 * AAA has been blocked from getting the password from the local database.
 *
 * Fix two bugs in LCP negotiation of Shiva PAP (SPAP).  One, prevent
 * mal-formed responses by ensuring that the pointer is at the end of the
 * option.  Two, NAK with CHAP/MD5 if SPAP option is received.(CSCdi58734)
 *
 * Revision 3.13.8.13  1996/05/21  10:09:50  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.13.8.12  1996/05/10  18:31:31  fox
 * CSCdi57029:  PPP fails CHAP on PRI
 * Branch: California_branch
 * Localize use of lcpstate to avoid confusion.
 *
 * Revision 3.13.8.11  1996/05/10  01:45:27  wmay
 * CPP and PPP half-bridge commit
 * Branch: California_branch
 *
 * Revision 3.13.8.10  1996/05/08  15:20:50  fox
 * CSCdi57029:  PPP fails CHAP on PRI
 * Branch: California_branch
 * Store AAA-computed CHAP hash in outgoing hash struct.  Duh!
 *
 * Revision 3.13.8.9  1996/05/07  20:12:16  billw
 * CSCdi51333:  multiple tacacs/radius requests sent if server is slow
 * Branch: California_branch
 *
 * Revision 3.13.8.8  1996/05/06  17:03:44  fox
 * CSCdi49278:  Update PAP handling of outbound calls
 * Branch: California_branch
 * Three bug fixes in one!
 * Disable remote PAP authentication support by default.  Add 'ppp pap
 * sent-username xxx password yyy' to re-enable support if needed.  Warn
 * if sent-username/password matches entry in local database. (CSCdi49278)
 * Add 'ppp chap hostname xxx' command to allow multiple boxes to use the
 * same hostname in their CHAP Challenges.  (CSCdi44884)
 * Add 'ppp chap password yyy' command to allow a box to respond to many
 * unknown peers with the same password.  (CSCdi54088)
 *
 * Revision 3.13.8.7  1996/04/29  10:09:28  mdb
 * The 96q1 gcc is much more particular about flagging potentially
 * uninitialized variables [CSCdi56015].
 * Branch: California_branch
 *
 * Revision 3.13.8.6  1996/04/27  06:37:38  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 3.13.8.5  1996/04/20  17:59:38  lol
 * CSCdi46005:  ISDN CLID not propagated to rem_addr field of T+ packet
 * Branch: California_branch
 *
 * Revision 3.6.2.9  1996/04/26  00:00:12  syiu
 * Branch: Dial1_branch
 * Dial1_branch to IosSec_postmerge_960419
 *
 * Revision 3.6.2.8  1996/04/18  00:27:21  syiu
 * Branch: Dial1_branch
 * Make ppp registry depends work correctly
 *
 * Revision 3.6.2.7  1996/04/17  00:02:05  syiu
 * Branch: Dial1_branch
 * Sync Dial1_branch to V111_1_3
 *
 * Revision 3.6.2.6  1996/03/16  16:43:23  syiu
 * Branch: Dial1_branch
 * Create/Delete L2F tunnels from MLPVT. Also move code into
 * L2F create/delete tunnel routines
 *
 * Revision 3.6.2.5  1996/02/08  23:46:59  syiu
 * Branch: Dial1_branch
 * Support for CHAP challenge by stackname if locally defined
 *
 * Revision 3.6.2.4  1996/01/31  18:37:43  tkolar
 * Branch: Dial1_branch
 * Get all of the VPN code into the repository, with the right names
 * and in the right places.
 *
 * Revision 3.6.2.3  1996/01/21  06:08:48  syiu
 * Branch: Dial1_branch
 * More PPP L2F support
 *
 * Revision 3.6.2.2  1996/01/19  02:15:55  syiu
 * Branch: Dial1_branch
 * PPP L2F support
 *
 * Revision 3.6.2.1  1996/01/16  15:45:27  dblair
 * CSCdi42375:  Move Multilink PPP into PPP subsystem
 * Branch: Dial1_branch
 *
 * Revision 3.13.8.4  1996/04/19  15:47:39  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commit
 *
 * Revision 2.1.6.9  1996/04/10  23:10:06  lol
 * Branch: IosSec_branch
 *         Incorporate code review comments.
 *
 * Revision 2.1.6.8  1996/04/09  22:15:31  che
 * Branch: IosSec_branch
 * Sync to California_branch (960405)
 *
 * Revision 2.1.6.7  1996/03/19  09:06:07  che
 * Branch: IosSec_branch
 * Sync to V111_1_3
 *
 * Revision 2.1.6.6  1996/02/28  06:15:36  lol
 * Branch: IosSec_branch
 * TACACS+ processing.
 *
 * Revision 2.1.6.5  1996/02/21  04:23:52  che
 * Branch: IosSec_branch
 * Sync to 11.1 Mainline (02/20/96)
 *
 * Revision 2.1.6.4  1996/01/23  07:09:27  che
 * Branch: IosSec_branch
 * Sync with 11.1 mainline. (960122)
 *
 * Revision 2.1.6.3  1995/12/19  04:27:25  che
 * Branch: IosSec_branch
 * Re-sync to Arkansas to fix things that broke.
 *
 * Revision 2.1.6.2  1995/12/17  02:44:48  che
 * Branch: IosSec_branch
 * Sync to Arkansas
 *
 * Revision 2.1.6.1  1995/11/17  18:50:40  carrel
 * Branch: IosSec_branch
 * Sync to Arkansas
 *
 * Revision 3.13.8.3  1996/03/28  20:33:14  fox
 * CSCdi31465:  PPP authentication timer is not stopped correctly
 * Branch: California_branch
 * Stop the authentication timer if we are not authenticating
 * Separate inbound and outbound authentication timers
 *
 * Revision 3.13.8.2  1996/03/27  10:36:45  irfan
 * CSCdi36933:  Customer desires configurable PAP/CHAP retries
 * Branch: California_branch
 * Add PPP UI to make us accept more than one PAP/CHAP Authentication
 * attempt. Instead of hanging up the physical layer right away.
 *
 * Revision 3.13.8.1  1996/03/18  22:48:28  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.7.2.3  1996/03/16  07:56:14  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.7.2.2  1996/03/07  11:08:10  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.7.2.1  1996/02/20  21:52:51  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.13  1996/02/28  09:34:19  hampton
 * CSCdi50135:  Consolidate process creation error messages
 *
 * Revision 3.12  1996/02/22  05:21:15  fox
 * CSCdi35908:  Should allow CHAP and PAP clients to same dialin pool
 * Add support for multiple acceptable inbound PPP authentication methods
 *
 * Revision 3.11  1996/02/09  09:03:31  billw
 * CSCdi43680:  RADIUS doesnt support magic number ip address values
 * also add authstruct->auth_idb, and change auth->tty to auth->auth_tty
 *
 * Revision 3.10  1996/01/29  00:30:38  billw
 * CSCdi13190:  Increase password characters encryted
 * Make sure that the maximum length password that can be encrypted is
 * the maximum length that will be accepted.
 *
 * Revision 3.9  1996/01/26  20:47:54  fox
 * CSCdi47633:  buginf does not start on new line
 *
 * Revision 3.8  1996/01/23  22:39:53  billw
 * CSCdi43679:  RADIUS support does not include CHAP
 * Do it in a way that will be obsolete in California
 *
 * Revision 3.7  1996/01/22  07:46:01  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.6  1996/01/11  10:52:45  billw
 * CSCdi43115:  PPP CHAP should use larger challenge for radius
 * compatibility
 * Also implement random_fill() which will fill an arbitrary string of
 * bytes with random and unpredictable data bytes
 *
 * Revision 3.5  1995/12/17  18:41:23  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.4  1995/11/29  00:16:45  lol
 * CSCdi42385:  TACACS+ crashes router in aaa_split_AV
 *
 * Revision 3.3  1995/11/19  04:50:43  fox
 * CSCdi43514:  PAP authentication is broken
 * Clean up logic used to prevent premature LCP Opens and extend the
 * logic to all possible cases.
 *
 * Revision 3.2  1995/11/17  18:04:07  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:50:37  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.19  1995/11/08  21:36:48  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.18  1995/10/19  23:56:00  lol
 * CSCdi42005:  multiple client chap responses cause repeated tacacs
 * processing
 *
 * Revision 2.17  1995/10/12  20:52:17  dblair
 * CSCdi41254:  PAP authentication sequencing error allows lcp_authcomplete
 * to execute twice.
 *
 * Revision 2.16  1995/10/02  20:43:23  lol
 * CSCdi36244:  Console line failing authorization should be more forgiving
 *              Authenticated console users now bypass authorization.
 *
 * Revision 2.15  1995/09/20  22:39:17  dblair
 * CSCdi40745:  PPP Multilink feature commit
 *
 * Revision 2.14  1995/09/10  23:06:03  carrel
 * CSCdi39960:  ppp chap should ignore non-chap pkts
 * When receiving PAP authentication requests, ensure that PAP is the
 * negotiated authentication method.
 *
 * Revision 2.13  1995/09/08  23:50:39  irfan
 * CSCdi39879:  memory leaking as AAA AV pairs from PAP/CHAP threads
 * more ping pong b/w me and the reviewer.
 *
 * Revision 2.12  1995/09/08  19:53:09  irfan
 * CSCdi39879:  memory leaking as AAA AV pairs from PAP/CHAP threads
 * Move the authorstruct cleanup to lcp_do_author.
 *
 * Revision 2.11  1995/09/07  09:36:52  irfan
 * CSCdi39879:  memory leaking as AAA AV pairs from PAP/CHAP threads
 * Plug the holes by remembering to aaa_cleanup_author() for both return
 * cases of lcp_do_author().
 *
 * Revision 2.10  1995/08/08  20:50:54  lol
 * CSCdi37361:  AAA should not require auth for originating PPP connections
 *
 * Revision 2.9  1995/08/08  16:51:01  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi38391]
 *
 * Revision 2.8  1995/08/04 23:14:13  irfan
 * CSCdi38157:  memory leaks in PPP PAP, CHAP, Callback
 * fix memory leaks in pap_rrequest and chap_rresponse path towards
 * callback. change as many malloc's to malloc_named, as possible.
 *
 * Revision 2.7  1995/07/21  19:40:29  fox
 * CSCdi37393:  PAP password should not appear in debugging messages
 * Remove password from authentication debug output.
 *
 * Revision 2.6  1995/07/14  08:05:59  misko
 * CSCdi31925:  PPP:Incorrect handling of CHAP RESPONSE messages
 * Missed a corner case in the PPP spec where if we have already responded
 * to a CHAP RESPONSE, we should respond with the same result.  This
 * allows us to short circuit the chap code and thus when using TACACS
 * not send multiple TACACS queries.
 *
 * Revision 2.5  1995/06/20  20:38:24  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.4  1995/06/15  22:58:49  syiu
 * CSCdi35686:  Async callback AAA authorization and accounting phase not
 * working
 *
 * Revision 2.3  1995/06/15  03:43:03  lol
 * CSCdi35713:  AAA subsystems needs more debugging output
 *              Add
 *                 aaa debug authentication
 *                 aaa debug authorization
 *                 aaa debug accounting
 *
 * Revision 2.2  1995/06/14  06:31:16  billw
 * CSCdi35380:  AAA accounting causes crashes
 * Clean up aaa_dup_user and callers, correct freeing of non-malloced
 * memory, and a couple other things.
 *
 * Revision 2.1  1995/06/07  23:19:08  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#include "master.h"
#include "../ui/common_strings.h"
#include <string.h>
#include <stdlib.h>		/* for atoi() */
#include <ciscolib.h>
#include "sys_registry.h"
#include "../h/auth_registry.regh"
#include "dialer_registry.h"
#include "ppp_registry.h"
#include "../mlpvt/mlpvt_registry.regh"
#include "interface_private.h"
#include "ttysrv.h"
#include "packet.h"
#include "config.h"
#include "login.h"
#include "connect.h"
#include "../util/md5.h"
#include "dialer.h"
#include "ppp.h"
#include "ppp_auth.h"
#include "../util/md5.h"
#include "aaa.h"
#include "../servers/tacacs_plus.h"
#include "../servers/tacacs.h"
#include "logger.h"
#include "../wan/msg_ppp.c"		/* Not a typo, see logger.h */
#include "ppp_public.h"
#include "../if/network.h"
#include "../parser/parser_actions.h"
#include "../ui/debug.h"
#include "../os/os_debug_flags.h"
#include "../wan/parser_defs_ppp.h"
#include "stacks.h"
#include "../util/random.h"
#include "../les/isdn_bri.h"
#include "dialer.h"


/*
 * This structure describes an AAA transaction which may block due
 * to communication with an external daemon.  It is built under the
 * PPP input process and processed by the PPP auth process.
 */
enum aaa_op {DO_CHAP_RCHALLENGE,
	     DO_CHAP_RRESPONSE,
	     DO_PAP_RREQUEST,
	     DO_PAP_SENDREQUEST};
typedef struct aaa_req_ {
    struct aaa_req_ *next;	/* For queueing */
    enum aaa_op op;		/* Requested operation */
    paktype *pak;		/* Packet (NULL for DO_PAP_SENDREQUEST) */
    hwidbtype *idb;		/* IDB */
    u_char *inp;		/* Input, ID, and length */
    u_char id;
    short len;
    sys_timestamp changed;	/* Timestamp to detect intfc changes */
} aaa_req;

static void queue_aaa_request(enum aaa_op, paktype *, hwidbtype *,
			      u_char *, u_char, short);
static void pap_rrequest(hwidbtype *, uchar *, uchar, ushort);
static boolean ppp_vpn(enum aaa_op, paktype *, hwidbtype *, u_char *,
		       u_char, short len);
static boolean ppp_authen_prepare(enum aaa_op, hwidbtype *, auth_state *,
				  u_char *, u_char, short);
static void chap_sendchallenge(hwidbtype *);
static void pap_sendrequest (hwidbtype *);

static watched_queue *authqueue;	/* Queue of work for PPP auth process */
static aaa_req *current_req = NULL;	/* Request in progress */
static int current_id;			/* so we can respond with the latest */

extern hwidbtype *authenticatingidb;

/*
 * chap_hold_response
 *  Return TRUE if CHAP Response packet should be held until we say OK.
 */

static boolean chap_hold_response (hwidbtype *idb)
{
    return ((idb->lcpstate->call_direction == PPP_CALL_IN) &&
	    !idb->lcpstate->chap_nowait);
}

/*
 * We need to save the remote's name.  Since we allow the peer to have a
 * different challenge name, we may need two strings.  But since the usual
 * case is either unidirectional authentication or the same name string, why
 * waste storage space.  We will point both pointers at the same string in
 * memory if they are the same string.  This can be very dangerous so we
 * will run all operations thru these functions.
 */

uchar *chap_new_peer_name (uchar **target, uchar **othername, uchar *name)
{
    if (!strcmp(*othername, name)) {
	name = *othername;	/* We already have that string in memory... */
    } else if (name) {
	name = strdup_named(name, "CHAP name");
    }
    if (*target && (*othername != *target)) {
	free(*target);
    }
    *target = name;
    return(name);
}

/*
 * auth_init
 * initialize structures needed for CHAP authentication.
 * Set _ok flags in the assumption that all is done.  They will be cleared
 * to FALSE if necessary.
 * We no longer zero the ID values on each call to auth_init since they are
 * zeroed by the malloc_named() and should not restart each time the link
 * comes up.
 */

void auth_init (hwidbtype *idb)
{
    auth_state *chapstate;

    if (!idb) {
	if (ppp_error_debug) {
	    buginf("\nPPP auth: no idb!");
	}
	return;
    }
    
    if (idb->authstate == NULL) {
        chapstate = malloc_named(sizeof(auth_state), "PPP CHAPstate");
	if (!chapstate)
	    return;
        idb->authstate = chapstate;
	chapstate->idb = idb;
    } else {
	chapstate = idb->authstate;
	chapstate->num_failures = 0;
	chapstate->bad_auth_attempts = 0;
	new_remote_name(chapstate, NULL);
	new_challenge_name(chapstate, NULL);
	/*
	 * Flush the CHAP response packet if it still exists.  We do this
	 * here because there is no chap_stop (or auth_stop) function and
	 * renegotiation could occur so fast that the timer does not fire
	 * and free the packet.
	 */
	auth_cleanup(idb);
    }

    TIMER_STOP(chapstate->retry_timer);
    chapstate->we_said_ok = TRUE;
    chapstate->they_said_ok = TRUE;
}

void auth_cleanup (hwidbtype *idb)
{
    auth_state *chapstate;
 
    if (!idb || !idb->authstate)
	return;
 
    chapstate = idb->authstate;
    if (chapstate->challenge_value) {
	free(chapstate->challenge_value);
	chapstate->challenge_value = NULL;
	chapstate->challenge_size = 0;
    }
 
    flush_chap_resp_pkt(idb);   /* Free the saved CHAP response packet if any */
}

/*
 * auth_start
 * We have negotiated authentication.  Set it up properly and start the
 * authentication timer.  We do it here and in auth_timeout to avoid the
 * potential for skew (and a potential denial of service attack).
 */

void auth_start (hwidbtype *idb)
{
    lcp_options *go;
    lcp_options *ho;

    go = idb->lcpfsm->gotoptions;
    ho = idb->lcpfsm->heroptions;

    /*
     * What did they agree to do for us?
     */
    if (go->neg_auth) {
	if (go->authtypes[0] == PPP_AUTH_CHAP) {
	    chap_sendchallenge(idb);
	} else if (go->authtypes[0] == PPP_AUTH_PAP) {
	    idb->authstate->we_said_ok = FALSE;
	}
    }

    /*
     * What did we agree to do for them?
     */
    if (ho->neg_auth) {
	if (ho->authtypes[0] == PPP_AUTH_CHAP) {
	    idb->authstate->they_said_ok = FALSE;
	} else if (ho->authtypes[0] == PPP_AUTH_PAP) {
	    /*
	     * Queue PAP send request to AAA auth task because
	     * pap_sendrequest() calls AAA code that can dismiss.
	     */
	    queue_aaa_request(DO_PAP_SENDREQUEST, NULL, idb, NULL, 0, 0);
	}
    }

    TIMER_START(idb->authstate->retry_timer, DEFCHAP_TIMEOUT);
}

/*
 * chap_sfailure
 * Send a CHAP failure packet to the remote.  Fill in the failure packet
 * with the given message.
 * If another challenge is to be sent, it will go out the next time the
 * timer fires.
 */

static void chap_sfailure (
    hwidbtype *idb,
    uchar id,
    char *message)
{
    paktype *pak;
    uchar *outp;
    int outlen;

    outlen = strlen(message);
    pak = getbuffer(outlen + DLLHEADERLEN + HEADERLEN);
    if (!pak)
        return;               /* bail out if no buffers */
    pak->datagramstart = pak->network_start - DLLHEADERLEN;
    outp = PACKET_DATA(pak->datagramstart);
    bcopy(message, outp, strlen(message));
    ppp_send_packet(TYPE_PPP_CHAP, CHAP_FAILURE, id, pak, outlen, idb);

    if (ppp_chap_debug)
        buginf("\nPPP %s: Send CHAP Failure id=%d, %s", 
	       idb->hw_namestring, id, message);
}

/*
 * chap_hash
 * Ok, now we have the secret, the packet id, and the challenge
 * value.  So put them all together, and run them through the
 * MD5 digest algorithm.
 */
void
chap_hash (char *secret, uchar id, uchar *value, int value_size, uchar *hash)
{
    MD5_CTX mdcontext;

    MD5Init(&mdcontext);
    MD5Update(&mdcontext, &id, 1);
    MD5Update(&mdcontext, secret, strlen(secret));
    MD5Update(&mdcontext, value, value_size);
    MD5Final(hash, &mdcontext);
}

/*
 * ppp_hostname
 * Return pointer to hostname to use for CHAP authentication
 */

static char stackname[MAXSTR+1];	/* make permanent space for stackname*/

char *
ppp_hostname (hwidbtype *idb)
{
    hwidbtype *lead;
    ipaddrtype source_ip;

    if (idb) {
	/*
	 * If the interface has overriden the CHAP name, return that
	 */
	if (idb->lcpstate && idb->lcpstate->chap_name) {
	    if (ppp_chap_debug) {
		buginf("\nPPP %s: Using alternate CHAP hostname %s.",
		       idb->hw_namestring, idb->lcpstate->chap_name);
	    }
	    return(idb->lcpstate->chap_name);
	}

	/*
	 * If this is a dialer IDB, and the lead IDB has an overriden
	 * CHAP name, honor that, too.
	 */
	lead = get_netidb(idb);
	if (lead && lead->lcpstate && lead->lcpstate->chap_name) {
	    if (ppp_chap_debug) {
		buginf("\nPPP %s: Using CHAP hostname %s from interface %s.",
		   idb->hw_namestring,
		   lead->lcpstate->chap_name,
		   lead->hw_namestring);
	    }
	    return(lead->lcpstate->chap_name);
	}
    }

    /*
     * Let SGBP stackname serve if configured
     */
    bzero(stackname, (MAXSTR+1));
    if (reg_invoke_mlpvt_find_stackname(stackname, MAXSTR, &source_ip)) {
        return(stackname);
    }

    /*
     * Last resort--configured hostname of this box
     */
    if (hostname) {
        return(hostname);
    }

    /*
     * Don't know!
     */
    return("Unknown");
}

/*
 * chap_sign_challenge
 * Add two signatures to the random fill in the Challenge value so that we
 * and other compliant boxes can detect a spoof attempt.  The theory is that
 * we will try to detect playback attempts by a) recognizing a signature and
 * b) recognizing that the challenge has been altered or replayed.
 */

static ulong chap_direction_keys[PPP_CALL_OUT + 1] =
{     0,              /* dummy entry */
      0x4f6f7073,     /* Oops */
      0x62454356,     /* rECV */
      0x4d614465      /* MaDe */
};

static ulong chap_xor_challenge (uchar *challenge_value, int xor_size)
{
    ulong hash;
    int i;

    for (i = 0, hash = 0; i < xor_size; i += sizeof(ulong)) {
	hash ^= GETLONG(&challenge_value[i]);
    }
    return (hash);
}


static void chap_sign_challenge (uchar *challenge_value,
				 int challenge_size,
				 uchar call_direction,
				 uchar *name,
				 int namelen)
{
    MD5_CTX mdcontext;
    uchar outhash[MD5_LEN];
    int offset;
    ulong hash;

    offset = challenge_size - (2 * sizeof(ulong));

    /*
     * Compute the hostname field and stick it in the Challenge value
     */
    MD5Init(&mdcontext);
    MD5Update(&mdcontext, name, namelen);
    MD5Final(outhash, &mdcontext);
    PUTLONG(&challenge_value[offset], GETLONG(outhash));

    offset += sizeof(ulong);

    /*
     * Finally we compute the checksum at the end of the message.
     */
    hash = chap_xor_challenge(challenge_value, offset) ^
	   chap_direction_keys[call_direction];
    PUTLONG(&challenge_value[offset], hash);
}

/*
 * chap_check_challenge
 * Check the Challenge value to see if we are being spoofed.
 * Returns TRUE if the Challenge should be responded to.
 */
static boolean chap_check_challenge (uchar *challenge_value,
				     int challenge_size,
				     uchar call_direction,
				     uchar *name,
				     int namelen)
{
    MD5_CTX mdcontext;
    uchar outhash[MD5_LEN];
    int offset, direction;
    ulong hash;

    /*
     * It is not a compliant Challenge unless the size is at least 16 bytes
     * and a multiple of four bytes, so Respond to any Challenge value that
     * does not meet these requirements.
     */
    if ((challenge_size < CHAP_CHALLENGE_SIZE) || (challenge_size & 0x03)) {
	return TRUE;		/* Not a compliant signature, respond */
    }

    offset = challenge_size - (2 * sizeof(ulong));

    /*
     * First, we check the checksum at the end of the message.
     */
    hash = chap_xor_challenge(challenge_value, challenge_size);
    /*
     * We have removed the XOR part of the checksum.  What should be left
     * is one of the Magic constants.  If not, this is not a compliant
     * Challenge value and should be responded to.
     */
    for (direction = PPP_NO_CALL; direction <= PPP_CALL_OUT; direction++) {
	if (hash == chap_direction_keys[direction])
	    break;
    }
    if (direction > PPP_CALL_OUT) {
	return (TRUE);		/* Not a compliant signature, respond */
    }

    MD5Init(&mdcontext);
    MD5Update(&mdcontext, name, namelen);
    MD5Final(outhash, &mdcontext);
    if (GETLONG(outhash) != GETLONG(&challenge_value[offset])) {
	return (FALSE);	/* compressed name and challenge name do not match */
    }

    /*
     * Finally, we need to check the Call direction to ensure that we are
     * not being spoofed by having both parties think the call is an incoming
     * call.
     */
    if ((direction == PPP_CALL_IN) && (call_direction == PPP_CALL_IN)) {
	return (FALSE);	/* We both think that we received this call! */
    }

    return (TRUE);	/* Everything checks out, respond to the Challenge */
}

/*
 * chap_rchallenge
 * Process a received challenge packet.  Respond to that packet with
 * a response packet.
 */

static void chap_rchallenge (
    hwidbtype *idb,
    uchar *inp,
    uchar id,
    ushort len)
{
    ushort outlen;
    int namelen;
    uchar value_size;
    char secret[PASSWORDLEN+1];
    char *user;
    int  userlen;
    paktype *pak;
    uchar *outp;
    uchar outhash[MD5_LEN];
    int use_tacacs;
    char *name;
    boolean ok;
    hwidbtype *lead;
    sys_timestamp state_time;
    lcp_options *ho;
    lcp_state *lcpstate;
    auth_state *chapstate;

    state_time = idb->lcpstate->link_transition;
    lcpstate = idb->lcpstate;
    chapstate = idb->authstate;

    use_tacacs = idb->lcpfsm->localauthen & PPP_LAUTHEN_TACACS;

    name = ppp_hostname(idb);
    namelen = strlen(name);

    /* Don't acknowledge CHAP Challenges if we didn't negotiate CHAP. */
    ho = idb->lcpfsm->heroptions;
    if (!ho->neg_auth || (ho->authtypes[0] != PPP_AUTH_CHAP)) {
	if (ppp_error_debug || ppp_chap_debug)
	    buginf("\nPPP %s: Received CHAP Challenge but did not "
		   "negotiate CHAP.", idb->hw_namestring);
	return;
    }

    /*
     * get the value size.  Later get the value.  The name
     * after this is the name of the host challenging us.  Look for the
     * secret to use for that host.
     */
    GETCHAR(value_size, inp);
    outp = inp;
    /*
     * position the pointer after the digest value so we can extract the
     * hostname.
     */
    inp += value_size;
    userlen = len - value_size - sizeof(uchar);
    user = malloc_named(userlen + 1, "chap_rchallenge user");
    if (!user) {
	if (ppp_chap_debug)
            buginf("\nPPP %s: Unable to authenticate.  "
		   "Malloc for user name failed", idb->hw_namestring);
	lcp_close(idb->lcpfsm);
        return;
    }
    sstrncpy(user, (char *)inp, userlen + 1);
    userlen = strlen(user);

    inp = outp;		/* reset inp to point to "value" */

    if (ppp_chap_debug) {
	buginf("\nPPP %s: CHAP Challenge id=%d received from %s",
	       idb->hw_namestring, id, user);
    }

    /*
     * Quick sanity check.  Don't bother talking to ourselves.  While
     * this might be found further down AND we should not have a password
     * for ourselves, it is probably a good idea to check this while we
     * can.
     */
    if (!lcpstate->chap_noignoreus && !strcmp(user, name)) {
	if (ppp_chap_debug)
	    buginf("\nPPP %s: ignoring challenge with local name",
		   idb->hw_namestring);
	free(user);
	return;			/* just ignore the Challenge */
    }

    /*
     * Preserve the name so that it cannot challenge us with another name.
     */
    if (chapstate->challenge_name) {
	/*
	 * We have been challenged at least once already.  If this is a
	 * different name, then ignore the Challenge.  Otherwise, the
	 * peer can probe us for a third party.
	 */
	if (strcmp(chapstate->challenge_name, user)) {
	    if (ppp_chap_debug)
		buginf("\nPPP %s: ignoring peer's new name",
		       idb->hw_namestring);
	    free(user);
	    return;			/* just ignore the Challenge */
	}
    } else {
	/*
	 * First time that we have been challenged.  So save the name
	 * as efficiently as possible (it might be the same as the
	 * authenticated name).
	 */
	new_challenge_name(chapstate, user);
    }

    /*
     * Make sure that the peer is not using two different names on us (unless
     * we allow that).
     */
    if (!lcpstate->chap_split_names &&
	chapstate->remote_name &&
	(chapstate->remote_name != chapstate->challenge_name)) {
	if (ppp_chap_debug)
	    buginf("\nPPP %s: Challenge name (%s) does not match Response"
		   " name (%s), ignoring",
		   idb->hw_namestring, user, chapstate->remote_name);
	free(user);
	return;				/* just ignore the Challenge */
    }

    /*
     * Got this far.  I guess we are ready to authenticate for the peer.
     * Wait!  The peer might be spoofing us looking for a valid response
     * to our or someone else's CHAP Challenge.  Lets verify the peer's
     * Challenge value.
     */
    if (!chap_check_challenge(inp, value_size, lcpstate->call_direction,
			      user, userlen)) {
	if (ppp_chap_debug)
	    buginf("\nPPP %s: ignoring spoofed CHAP Challenge",
		   idb->hw_namestring);
	free(user);
	return;				/* just ignore the Challenge */
    }

    /*
     * The object of the next four 'if...elseif...else' sections is to
     * finish with ok == TRUE and outhash containing the CHAP hash value to
     * send.  If we fail to find a password, we will exit with ok == FALSE
     * and check for a default password.
     *
     * The first check verifies that we received a peer name in the challenge.
     * If we did not, our only choice is to use the default password.
     */
    if (!userlen) {
        if (ppp_chap_debug)
            buginf("\nPPP %s: No name received from peer", idb->hw_namestring);
	ok = FALSE;
    } else if (old_access_control && use_tacacs) {
        uchar *tacacs_data = NULL;
	char *err = NULL;

	if (!tacacs_extended) {
	    errmsg(&msgsym(NOEXTTACACS, PPP));
	    free(user);
	    return;
	}

	outlen = sizeof(uchar) + value_size;
	tacacs_data = malloc_named(MAX(outlen, MD5_LEN), "PPP Tacacs Data");
	if (!tacacs_data) {
	    free(user);
	    return;
	}
	tacacs_data[0] = id;
	bcopy(inp, &tacacs_data[1], value_size);
	authenticatingidb = idb;
	ok = reg_invoke_login_authen(user, tacacs_data, outlen, MD5_LEN,
				     &err, XTA_CHAP_AUTH, idb->idb_tty);
	if (!ok) {
	    if (ppp_chap_debug) {
		buginf("\nPPP %s: USERNAME %s not found.",
		       idb->hw_namestring, user);
	    }
	} else {
	    bcopy(tacacs_data, outhash, MD5_LEN);
	}
	free(tacacs_data);
    } else if (old_access_control) {
        usernametype *info;

	/*
	 * get the secret for the given name.
	 */
	info = find_namedthing(&userQ, user);
	if (!info) {
	    if (ppp_chap_debug) {
		buginf("\nPPP %s: USERNAME %s not found.",
		       idb->hw_namestring, user);
	    }
	    ok = FALSE;
	} else if (info->flags & USER_NOPASSWORD) {
	    if (ppp_chap_debug) {
		buginf("\nPPP %s: No password defined for USERNAME %s",
		       idb->hw_namestring, user);
	    }
	    ok = FALSE;
	} else {
	    if (info->flags & USER_ENCRYPTION) {
		decrypt(info->password, secret, PASSWORDLEN);
	    } else {
		bcopy(info->password, secret, strlen(info->password) + 1);
	    }
	    chap_hash(secret, id, inp, value_size, outhash);
	    ok = TRUE;
	}
    } else {	/* ! old_access_control */
	userstruct *ustruct;
	authenstruct astruct;
	char *rem = NULL;
	char *data = NULL;

	bzero(&astruct, sizeof(astruct));
	if (idb->idb_tty) {
	    astruct.tty = idb->idb_tty;
	    rem = tty_get_remote_addr(idb->idb_tty);
	}
	if (is_isdn(idb) && (idb->isdn_info->remote_src == ISDN_SRC_ISDN)) {
	    rem = idb->isdn_info->remote_number;
	}
	ok = aaa_create_user(&ustruct, user, NULL, idb->hw_namestring, rem,
			      AAA_AUTHEN_TYPE_CHAP, AAA_SVC_PPP, PRIV_USER);
	if (ok) {
	    data = malloc_named(value_size + 1, "Chap rchal, new auth");
	    if (!data) {
		ok = FALSE;
	    } else {
	        data[0] = id;
	        bcopy(inp, &data[1], value_size);
	        astruct.user_data = data;
	        astruct.user_data_len = value_size + 1;
	        ok = aaa_start_login(ustruct, &astruct,
				     lcpstate->acc_list_name,
				      AAA_AUTHEN_ACT_SENDAUTH, AAA_SVC_PPP);
	        astruct.user_data = NULL;
	        astruct.user_data_len = 0;
	        free(data);
		if (ok && (astruct.status == AAA_AUTHEN_STATUS_PASS)) {
		    bcopy(astruct.server_data, outhash, MD5_LEN);
		} else {
		    ok = FALSE;
		}
		aaa_cleanup_login(ustruct, &astruct);
	    }
	}
	if (!ok) {
	    if (ppp_chap_debug) {
		buginf("\nPPP %s: USERNAME %s: lookup failure.",
		       idb->hw_namestring, user);
	    }
	}
	aaa_free_user(&ustruct);
    }

    /*
     * Check to see if the interface changed state since we started.  If it
     * did, then we should not do anything more with request.
     */
    if (TIMERS_NOT_EQUAL(state_time, idb->lcpstate->link_transition)) {
	free(user);
	return;
    }

    if (!ok) {
	/*
	 * Last chance is the default password
	 */
	if (lcpstate->chap_pw) {
	    decrypt(lcpstate->chap_pw, secret, PASSWORDLEN);
	    chap_hash(secret, id, inp, value_size, outhash);
	    if (ppp_chap_debug) {
	        buginf("\nPPP %s: Using default CHAP password.",
		       idb->hw_namestring);
	    }
	} else if ((lead = get_netidb(idb)) && lead->lcpstate &&
		lead->lcpstate->chap_pw) {
	    decrypt(lead->lcpstate->chap_pw, secret, PASSWORDLEN);
	    chap_hash(secret, id, inp, value_size, outhash);
	    if (ppp_chap_debug) {
		buginf("\nPPP %s: Using default CHAP password from %s.",
		       idb->hw_namestring, lead->hw_namestring);
	    }
	} else {
	    if (ppp_chap_debug) {
	        buginf("\nPPP %s: Unable to authenticate for peer.",
		       idb->hw_namestring);
	    }
	    lcp_close(idb->lcpfsm);
	    free(user);
	    return;
	}
    }
    free(user);

    /*
     * get a buffer big enough for the value_size, and the digest value and
     * our hostname
     */
    outlen = namelen + MD5_LEN + sizeof(uchar);
    pak = getbuffer(outlen + DLLHEADERLEN + HEADERLEN);
    if (!pak) {
	return;			/* No packet, can't send anything */
    }
    pak->datagramstart = pak->network_start - DLLHEADERLEN;
    outp = PACKET_DATA(pak->datagramstart);
    PUTCHAR(MD5_LEN, outp);
    bcopy(outhash, outp, MD5_LEN);
    outp += MD5_LEN;
    bcopy(name, outp, namelen);
    flush_chap_resp_pkt(idb);	/* Free the saved CHAP response packet if any */
    idb->authstate->chap_resp_pkt = pak;	/* save packet for next time */
    if (!chap_hold_response(idb) || idb->authstate->we_said_ok) {
	if (ppp_chap_debug)
	    buginf("\nPPP %s: Send CHAP Response id=%d",
		   idb->hw_namestring, id);
	pak_lock(pak);
	ppp_send_packet(TYPE_PPP_CHAP, CHAP_RESPONSE, id, pak, outlen, idb);
    } else {
	ppp_hdr *ppp_hdrp;

	if (ppp_chap_debug)
	    buginf("\nPPP %s: Waiting for peer to authenticate first",
		   idb->hw_namestring);
	/*
	 * ppp_send_packet() won't set these fields, so we have to because
	 * resend_chap_response() is expecting them.
	 */
	ppp_hdrp = (ppp_hdr *)pak->network_start;
	ppp_hdrp->id = id;
	ppp_hdrp->plen = outlen + HEADERLEN;
    }
}

/*
 * resend_chap_response
 * Timed out, so we need to send the response if it still exists
 */
static void resend_chap_response (hwidbtype *idb)
{
    paktype *pak;
    ppp_hdr *ppp_hdrp;
    uchar id;
    ushort outlen;

    pak = idb->authstate->chap_resp_pkt;

    ppp_hdrp = (ppp_hdr *)pak->network_start;
    id = ppp_hdrp->id;
    outlen = ppp_hdrp->plen - HEADERLEN;

    if (!chap_hold_response(idb) || idb->authstate->we_said_ok) {
	if (ppp_chap_debug)
	    buginf("\nPPP %s: Send CHAP Response id=%d",
		   idb->hw_namestring, id);
	pak_lock(pak);		/* lock it once more */
	ppp_send_packet(TYPE_PPP_CHAP, CHAP_RESPONSE, id, pak, outlen, idb);
    }
}

/*
 * flush_chap_resp_pkt
 * Free up the saved CHAP response packet (if any)
 */
void flush_chap_resp_pkt (hwidbtype *idb)
{
    if (idb->authstate->chap_resp_pkt) {
	datagram_done(idb->authstate->chap_resp_pkt);
	idb->authstate->chap_resp_pkt = NULL;
    }
}

/*
 * chap_rfailure
 * Process a received CHAP failure packet.  
 */

static void chap_rfailure (
    hwidbtype *idb,
    uchar *inp,
    uchar id,
    ushort len)
{
    char *message;

    flush_chap_resp_pkt(idb);	/* Free the saved CHAP response packet if any */

    if (ppp_chap_debug) {
        buginf("\nPPP %s: Failed CHAP authentication with remote.", 
	       idb->hw_namestring);
	if (len) {
	    message = malloc_named(len + 1, "PPP CHAP message");
	    if (!message)
	        return;
	    sstrncpy(message, (char *)inp, len+1);
            buginf("\nRemote message is: %s", message);
	    free(message);
        }
    }
}

/*
 * chap_ssuccess
 * Send a CHAP success packet to the remote.  
 */

void chap_ssuccess (
    hwidbtype *idb,
    uchar id)
{
    paktype *pak;
    auth_state *chapstate;


    pak = getbuffer(DLLHEADERLEN + HEADERLEN);
    if (!pak)
        return;               /* bail out if no buffers */
    pak->datagramstart = pak->network_start - DLLHEADERLEN;
    ppp_send_packet(TYPE_PPP_CHAP, CHAP_SUCCESS, id, pak, 0, idb);

    if (ppp_chap_debug)
        buginf("\nPPP %s: Send CHAP Success id=%d", 
	       idb->hw_namestring, id);

    chapstate = idb->authstate;
    chapstate->num_failures = 0;
    if (ppp_chap_debug)
        buginf("\nPPP %s: remote passed CHAP authentication.", 
	       idb->hw_namestring);
}

/*
 * chap_rsuccess
 * Process a received CHAP success packet.  If we aren't requiring them
 * to authenticate, or if they have already authenticated, then tell lcp
 * that authentication is completed.  Otherwise, wait until they have
 * authenticated.  Make sure we tell lcp only once.
 */

static void chap_rsuccess (
    hwidbtype *idb,
    uchar *inp,
    uchar id,
    ushort len)
{
    auth_state *chapstate;

    chapstate = idb->authstate;
    if (ppp_chap_debug)
        buginf("\nPPP %s: Passed CHAP authentication with remote.", 
	       idb->hw_namestring);

    flush_chap_resp_pkt(idb);	/* Free the saved CHAP response packet if any */

    /*
     * We only want to complete authentication once.
     */
    if (chapstate->they_said_ok)
	return;

    chapstate->they_said_ok = TRUE;
    if (chapstate->we_said_ok)
	lcp_authcomplete(idb, NULL);
}

/*
 * ppp_bad_auth_attempt
 * Remote client could not authenticate with us. Increment bad
 * auth attempts counter and maybe close lcp.
 * If the peer needs to be challenged again, the timer will fire and
 * kick off a challenge.
 */
static inline void ppp_bad_auth_attempt (hwidbtype *idb, fsm *f,
					 auth_state *chapstate)
{
    chapstate->bad_auth_attempts++;
    if (chapstate->bad_auth_attempts >= f->max_bad_auths) {
	lcp_close(idb->lcpfsm);
    } else {
	if (ppp_chap_debug) {
	    buginf("\nPPP %s: Will allow %d more authentication attempts",
	           idb->hw_namestring, f->max_bad_auths -
	           chapstate->bad_auth_attempts);
	}
    }
}

/*
 * chap_rresponse
 * Process a received response packet.  Respond to that packet with
 * either a success or failure packet after validating the information.
 */

static void chap_rresponse (
    hwidbtype *idb,
    uchar *inp,
    uchar id,
    ushort len)
{
    uchar *md_stream;
    uchar value_size;
    char secret[PASSWORDLEN+1];
    usernametype callback_info;
    char *user;
    int  userlen;
    usernametype *info = NULL;
    char digest[MD5_LEN];
    auth_state *chapstate;
    fsm *f;
    lcp_state *lcpstate;
    int use_tacacs;
    userstruct *ustruct = NULL;
    sys_timestamp state_time;

    state_time = idb->lcpstate->link_transition;

    chapstate = idb->authstate;
    f = idb->lcpfsm;
    lcpstate = idb->lcpstate;
    memset(&callback_info, 0, sizeof(usernametype));

    /*
     * if the received id isn't equal to the id we transmitted, then
     * just ignore this response
     */
    if (chapstate->chap_id != id) {
	if (ppp_error_debug || ppp_chap_debug) {
	    buginf("\nPPP %s: CHAP Response ignored."
		   "  Expected id %d, got id %d.",
		   idb->hw_namestring, chapstate->chap_id, id);
	}
        return;
    }

    /*
     * We may get a response in after we've already authenticated them.
     * Just return success.
     */
    if (chapstate->we_said_ok) {
        chap_ssuccess(idb, id);
	return;
    }

    GETCHAR(value_size, inp);

    if (value_size != MD5_LEN) {
	if (ppp_error_debug || ppp_chap_debug)
            buginf("\nPPP %s: invalid MD5 response received.", 
	           idb->hw_namestring);
        chap_sfailure(idb, id, "Short MD5 response received");
	ppp_bad_auth_attempt(idb, f, chapstate);
        return;
    }

    md_stream = inp;
    /*
     * position the pointer after the digest value so we can extract the
     * hostname.
     */
    inp += value_size;
    userlen = len - value_size - sizeof(uchar);
    user = malloc_named(userlen + 1, "chap_rresponse user");
    if (!user) {
	if (ppp_error_debug || ppp_chap_debug)
            buginf("\nPPP %s: Unable to authenticate.  "
		   "Malloc for user name failed", idb->hw_namestring);
	chap_sfailure(idb, id, "Not enough resources for authentication");
        return;
    }
    sstrncpy(user, (char *)inp, userlen + 1);

    if (!strlen(user)) {
        if (ppp_error_debug || ppp_chap_debug)
            buginf("\nPPP %s: Unable to authenticate.  "
                    "No name received from peer", idb->hw_namestring);
        chap_sfailure(idb, id, "No name received to authenticate");
        ppp_bad_auth_attempt(idb, f, chapstate);
        return;
    }

    if (ppp_chap_debug)
        buginf("\nPPP %s: CHAP Response id=%d received from %s", 
	       idb->hw_namestring, id, user);

    /*
     * Make sure that the peer is not using two different names on us (unless
     * we allow that).
     */
    if (!idb->lcpstate->chap_split_names &&
	chapstate->challenge_name &&
	strcmp(chapstate->challenge_name, user)) {
	if (ppp_chap_debug)
	    buginf("\nPPP %s: Response name (%s) does not match Challenge"
		   " name (%s), ignoring",
		   idb->hw_namestring, user, chapstate->challenge_name);
	free(user);
	return;				/* just ignore the Challenge */
    }

    if (old_access_control) {
	use_tacacs = (f->localauthen & PPP_LAUTHEN_TACACS);

	if (use_tacacs) {		/**** Get secret via TACACS ****/
	    int ok;
	    char *reason, *spass;
	    uchar *tacacs_data = &digest[0];

	    /*
	     * Make sure we initialize the 'reason' string pointer since
	     * subsequent calls may not set it before returning.  strlen()
	     * does not do well if it points to non-zero memory.  It may
	     * bus-error before stopping.   CSCdj02493
	     */
	    reason = NULL;

	    if (!tacacs_extended) {
		errmsg(&msgsym(NOEXTTACACS, PPP));
		free(user);
		return;
	    }

	    /*
	     * Single-Line support takes the users password from the Username
	     * field.  It looks for a separator character and does a standard
	     * TACACS xlogin query.  The CHAP response value is ignored.
	     */
	    if ((f->localauthen & PPP_LAUTHEN_SINGLELINE) &&
		((spass = strchr(user, TAC_SINGLELINE_CHAR)) != NULL)) {
		*spass = '\0';
		spass++;
		authenticatingidb = idb;
		ok = reg_invoke_login_login(idb->idb_tty, user, spass,
					    &reason);
		if (!ok) {
		    buginf("\nPPP %s: Unable to validate single-line CHAP via "
			   "TACACS. USERNAME %s not found.",
			   idb->hw_namestring, user);
		    free(user);
		    chap_sfailure(idb, id, reason);
		    ppp_bad_auth_attempt(idb, f, chapstate);
		    return;
		}
		/* TACACS keeps track of when folks log in and log out.
		 * The above call to reg_invoke_login_login() is really
		 * only an authentication call, _not_ a login.  The right
		 * fix is to create a new TACACS type code for
		 * authentication and use that, but instead we are just
		 * going to do an immediate logout to keep the accounting
		 * record straight.
		 */
		authenticatingidb = idb;
		reg_invoke_login_logout(idb->idb_tty);
		/* Now fake the MD5 hash for later code. */
		bcopy(md_stream, digest, MD5_LEN) ;
	    } else {
		tacacs_data[0] = id;
		bcopy(chapstate->challenge_value, &tacacs_data[1],
		      chapstate->challenge_size);
		authenticatingidb = idb;
		ok = reg_invoke_login_authen(user, tacacs_data,
				    (1 + chapstate->challenge_size),
				     MD5_LEN, &reason, XTA_CHAP_AUTH,
				     idb->idb_tty);
		if (!ok) {
		    if (ppp_chap_debug)
			buginf("\nPPP %s: Unable to validate CHAP via TACACS. "
			       "USERNAME %s not found.",
			       idb->hw_namestring, user);
		    free(user);
		    chap_sfailure(idb, id, reason);
		    ppp_bad_auth_attempt(idb, f, chapstate);
		    return;
		}
	    }
	} else {			/**** Get secret locally ****/
	    /*
	     * get the secret for the given name. 
	     */
	    info = (usernametype *) find_namedthing(&userQ, user);
	    if (!info) {
		if (ppp_chap_debug)
		    buginf("\nPPP %s: Unable to validate CHAP response.  "
			   "USERNAME %s not found.", idb->hw_namestring, user);
		free(user);
		chap_sfailure(idb, id, "Authentication failure.");
		ppp_bad_auth_attempt(idb, f, chapstate);
		return;
	    }
	    if (info->flags & USER_NOPASSWORD) {
		if (ppp_chap_debug)
		    buginf("\nPPP %s: Unable to validate CHAP response.  No "
			   "password defined for USERNAME %s",
			   idb->hw_namestring, user);
		free(user);
		chap_sfailure(idb, id, "Authentication failure.");
		ppp_bad_auth_attempt(idb, f, chapstate);
		return;
	    }
	    if (info->flags & USER_ENCRYPTION)
		decrypt(info->password, secret, PASSWORDLEN);
	    else
		bcopy(info->password, secret, strlen(info->password) + 1);

	    /*
	     * Ok, now we have the secret, the packet id, and the challenge
	     * value.  So put them all together, and run them through the
	     * MD5 digest algorithm.
	     */
	    chap_hash(secret, id, chapstate->challenge_value,
		      chapstate->challenge_size, digest);
	}

	/*
	 * Now compare the received digest value with the just calculated
	 * digest value.  If they are equal, then send a success packet.
	 * Otherwise send a failure packet.
	 */
	if (bcmp(digest, md_stream, MD5_LEN)) {
	    if (!old_access_control)
		aaa_free_user(&ustruct);
	    free(user);
 	    chap_sfailure(idb, id, "MD compare failed");
	    lcp_close(f);
	    return;
	}
    } else {	/* ! old_access_control */
	authenstruct astruct;
	boolean ret;
	char *rem = NULL;
	char *data = NULL;
	char *spass;

	/* 
	 * If single-line is configured, and the username is of the form
	 * user*OTP, then do a simple login instead of CHAP 
	 */
	if ((f->localauthen & PPP_LAUTHEN_SINGLELINE) &&
	    ((spass = strchr(user, TAC_SINGLELINE_CHAR)) != NULL)) {
	    *spass = '\0';
	    spass++;
	    ret = aaa_otp_login(idb, idb->idb_tty, 
					user, spass, lcpstate->acc_list_name);
	    if (!ret) {
		free(user);
		chap_sfailure(idb, id, "OTP Authentication failure.");
		lcp_close(f);
		return;
	    }
	} else {
	    bzero(&astruct, sizeof(astruct));
	    if (idb->idb_tty) {
		astruct.tty = idb->idb_tty;
		rem = tty_get_remote_addr(idb->idb_tty);
	    }
	    if (is_isdn(idb) && (idb->isdn_info->remote_src == ISDN_SRC_ISDN)) {
		rem = idb->isdn_info->remote_number;
	    }
	    ret = aaa_create_user(&ustruct, user, NULL, idb->hw_namestring, rem,
				  AAA_AUTHEN_TYPE_CHAP, AAA_SVC_PPP, PRIV_USER);
	    if (ret) {
		data = malloc_named(1 + chapstate->challenge_size + MD5_LEN,
				    "Chap rresp, new auth");
		if (!data)
		    ret = FALSE;
	    }
	    if (ret) {
		data[0] = id;
		bcopy(chapstate->challenge_value, &data[1], 
		      chapstate->challenge_size);
		bcopy(md_stream, &data[1 + chapstate->challenge_size], MD5_LEN);
		astruct.user_data = data;
		astruct.user_data_len = 1 + chapstate->challenge_size + MD5_LEN;
		ret = aaa_start_login(ustruct, &astruct, lcpstate->acc_list_name,
				      AAA_AUTHEN_ACT_LOGIN, AAA_SVC_PPP);
		astruct.user_data = NULL;
		astruct.user_data_len = 0;
		free(data);
	    }
	    if (!ret || (astruct.status != AAA_AUTHEN_STATUS_PASS)) {
		if (ppp_chap_debug) {
		    buginf("\nPPP %s: Unable to validate CHAP response.  "
			   "USERNAME %s: Authentication failure.",
			   idb->hw_namestring, user);
		}
		free(user);
		chap_sfailure(idb, id, astruct.server_msg ?
			      astruct.server_msg : "Authentication failure.");
		lcp_close(f);
		aaa_cleanup_login(ustruct, &astruct);
		aaa_free_user(&ustruct);
		return;
	    }
	    aaa_cleanup_login(ustruct, &astruct);
	} /* single-line */
    } /* ! old_access_control */
    
    if (!old_access_control) {
	authorstruct author;

	if (!lcp_do_author(ustruct, &author, idb, f, &callback_info)) {
	    clear_usernametype(&callback_info);
	    aaa_free_user(&ustruct);
	    free(user);
	    chap_sfailure(idb, id, author.server_msg ? author.server_msg :
			  "Authorization failed");
	    ppp_bad_auth_attempt(idb, f, chapstate);
	    aaa_cleanup_author(&author);
	    return;
	}
	aaa_cleanup_author(&author);

        /*
         * This time we need to send a pointer to our automatic
	 * storage.
         */
	info = &callback_info;
    }

    /*
     * Check to see if the interface changed state since we started.  If it
     * did, then we should not do anything more with request.
     */
    if (TIMERS_NOT_EQUAL(state_time, idb->lcpstate->link_transition)) {
	return;
    }

    /*
     * along with sending success notification, we need to store the
     * user info someplace
     */

    reg_invoke_put_authname(idb,ustruct,user);

    chap_ssuccess(idb, id);
    chapstate->we_said_ok = TRUE;

    /*
     * Just in case we were waiting for the peer to go first, now is the
     * time to respond to its Challenge.
     */
    if (chapstate->chap_resp_pkt &&
	chap_hold_response(idb)) {
	resend_chap_response(idb);
    }

    /*
     * In case we are a dial on demand interface, inform the dialing
     * software who we are connected to so that when it replys to
     * packets it knows which interface to send them to.
     */
    reg_invoke_dialer_remote_name(idb, user);

    /*
     * update remote_name whenever we authenticate another system
     */
    new_remote_name(chapstate, user);

    /*
     *  The following code depends on remote_name to be updated
     *    Do not move it above reg_invoke_dialer_remote_name;
     */

    free(user);
    if (chapstate->they_said_ok)
	lcp_authcomplete(idb, info);
    clear_usernametype(&callback_info);

}


/*
 * ppp_auth_background()
 *	Process AAA operations in a seperate process
 */
static process
ppp_auth_background (void)
{
    aaa_req *req;
    int reqcount;

    process_watch_queue(authqueue, ENABLE, RECURRING);

    /*
     * Endless processing loop
     */
    while (TRUE) {
	/*
	 * Wait for more requests to arrive and/or for others to run
	 */
	process_wait_for_event();

	reqcount = PAK_SWITCHCOUNT;

	/*
	 * Process PAK_SWITCHCOUNT requests or until the request queue is empty
	 * We know that either the local processing will be fast or that we
	 * will suspend in TCP processing for others to run.  But this logic
	 * is necessary to satisfy the THRASHING check.
	 */
	while (reqcount--) {
	    /* 
	     * Get the next request from the queue and save it for others
	     * to see while we process it.  We don't want a duplicate entry
	     * put on the queue while we process this one.  If the queue is
	     * empty break out of the loop and wait for more work.
	     */
	    req = process_dequeue(authqueue);
	    if (req == NULL) {
		break;
	    }
	    current_req = req;

	    /*
	     * If the interface has changed since our request was
	     * queued, dump it without answering.  We should really
	     * scan the queue and remove items when:
	     *  a) an interface goes down (match on req->idb)
	     *  b) we find an item in the queue (check req->changed)
	     * But we don't do that because there is little to be
	     * gained.  If we do ever cleanse the authentication queue
	     * then we can simplify some of the logic in queue_aaa_request().
	     */
 	    if (req->idb->lcpstate && 
                TIMERS_EQUAL(req->changed,
                             req->idb->lcpstate->link_transition)) {
		/*
		 * Vector out to each operation
		 */
		switch (req->op) {
		case DO_CHAP_RCHALLENGE:
		    chap_rchallenge(req->idb, req->inp, req->id, req->len);
		    break;
		case DO_CHAP_RRESPONSE:
		    chap_rresponse(req->idb, req->inp, req->id, req->len);
		    break;
		case DO_PAP_RREQUEST:
		    pap_rrequest(req->idb, req->inp, req->id, req->len);
		    break;
		case DO_PAP_SENDREQUEST:
		    pap_sendrequest(req->idb);
		    break;
		}
	    }

	    /*
	     * Now that we are done with it, clear out the shared pointer.
	     */
	    current_req = NULL;

	    /*
	     * Release packet (if any), free request element
	     */
	    if (req->pak) {
		datagram_done(req->pak);
	    }
	    free(req);
	}
    }
}

/*
 * find_aaa_request()
 * Search the current request and authentication queues for a match
 * Examine queue_aaa_request() for a better understanding of this process.
 */
static boolean
find_aaa_request(enum aaa_op op, hwidbtype *idb, u_char id)
{
    aaa_req *req;

    /*
     * First, we check the current entry in progress
     */
    req = current_req;
    if (req &&				/* a request in progress? */
	(req->idb == idb) &&		/* for this interface? */
	(req->op == op) &&		/* the same request type? */
	(req->id == id) &&		/* the same request ID? */
	TIMERS_EQUAL(req->changed, idb->lcpstate->link_transition)) {  /* same connection? */

	return TRUE;

    }

    /*
     * Then we scan the queue for the desired entry
     */
    for (req = process_queue_peek(authqueue); req; req = req->next) {
	if ((req->idb == idb) && (req->op == op) && (req->id == id) &&
	    TIMERS_EQUAL(req->changed, idb->lcpstate->link_transition)) {

	    return TRUE;

	}
    }

    /*
     * Failed to find the desired entry.
     */
    return FALSE;
}


/*
 * queue_aaa_request()
 *	Queue up request to do a AAA transaction
 *
 * Each of these requests may block while getting information from
 * AAA to build their transaction.  Therefore, we do not do these
 * operations on the PPP process, but instead queue them to a
 * process devoted to AAA transactions.
 */

static const char *ppp_duplicate_msg =
    "\nPPP: %s duplicate authentication request id=%d already in progress";

static void
queue_aaa_request (enum aaa_op op, paktype *pak, hwidbtype *idb,
		   u_char *inp, u_char id, short len)
{
    aaa_req *req;
    static pid_t authpid = 0;

    /*
     * First time only, create PPP auth process and queue.
     * WARNING!!!:  Kerberos is a major stack-hog.  Make sure
     * ppp_auth has enough stack to authenticate via Kerberos.
     */
    if (authpid == 0) {
 	authqueue = create_watched_queue("PPP auth Q", 0, 0);
 	authpid = process_create(ppp_auth_background, "PPP auth",
 				 HUGE_STACK, PRIO_NORMAL);
 	if (authpid == NO_PROCESS) {
	    if (ppp_error_debug || ppp_chap_debug) {
		buginf("\nPPP %s: can't start CHAP/PAP process",
		       idb->hw_namestring);
	    }
	    datagram_done(pak);
	    return;
	}
    }

    /*
     * The following process is risky and thus must be approached with care.
     * None of the Authentication operations should be duplicated on the
     * authentication queue.  Duplication can occur when AAA operations take
     * long enough that the peer times out and sends another CHAP challenge
     * or CHAP response or PAP request (or we timeout and want to send
     * another PAP request).  Duplication is unnecessary in all cases, can
     * cause one-time-password logic to break and further increases the
     * delay which can result in authentication failures.  The following
     * code searches for a previous operation of the same type for the same
     * interface.  What it does, if it finds a duplicate, is request type
     * dependant.  Note that the 'id' and interface 'timestamp' are not part
     * of the search criteria but are considered as part of the logic.
     *
     * WARNING:  The following code depends on certain process characteristics
     *           including non-preemptive threading and single processor
     *           access/modification of the authentication queue.
     *
     * NOTE:  Obviously this code can be cleaned up if we ever decide to
     *        'cleanse' the authentication queue when an interface goes
     *        down.  We will no longer need to check the timestamp.  Now
     *        we check the timestamp to make sure that this operation was
     *        queued by the current connection.
     *
     * COMMENT:  Before you criticize the length and complexity of this code,
     *           please note that this code is mostly a NOP if the requests
     *           are processed faster than they arrive (as the queue will be
     *           empty) and incredibly important if the requests begin to
     *           backup.
     *
     * The first step is to examine the current request to see if it is a
     * duplicate entry.  The handling of the current request must be different
     * than the entries in the queue because we cannot modify the current
     * request (its values have already been passed to the code that does
     * the actual work requested).
     */
    req = current_req;
    if (req &&				/* a request in progress? */
	(req->idb == idb) &&		/* for this interface? */
	(req->op == op) &&		/* the same request type? */
	TIMERS_EQUAL(req->changed, idb->lcpstate->link_transition)) {  /* same connection? */

	if (ppp_chap_debug) {
	    buginf(ppp_duplicate_msg, idb->hw_namestring, id);
	}
	switch (op) {
	case DO_CHAP_RCHALLENGE:
	    /*
	     * If this is an identical challenge (same ID and challenge
	     * value) then boogie out because we will be sending a response
	     * shortly.  Otherwise, queue it below.  Note that we could
	     * report the case of a challenge with the same ID but a
	     * different challenge value but it is both unlikely and not
	     * that significant.
	     */
	    if ((req->id == id) &&
		(req->len == len) &&
		!memcmp(req->inp, inp, len)) {
		datagram_done(pak);
		return;
	    }
	    break;			/* continue processing the request */
	case DO_CHAP_RRESPONSE:
	    /*
	     * If this is a duplicate CHAP response (same ID) we want to
	     * ignore it as we will correctly respond when we get done with
	     * this request and we don't want duplicate responses to break
	     * the OTP logic as TACACS+ re-authenticates.
	     * If this is not a duplicate CHAP response, we will let it get
	     * queued for later processing because we cannot modify a
	     * request in progress.
	     * Note that we ignore the actual response in making this
	     * decision as the peer shouldn't have a second chance at the
	     * same challenge.  In fact, the RFC requires that the retransmit
	     * include the same response string.
	     */
	    if (req->id == id) {
		datagram_done(pak);
		return;
	    }
	    break;			/* continue processing the request */
	case DO_PAP_RREQUEST:
	    /*
	     * The peer should only have one chance at getting in so we can
	     * safely drop this request as RFC 1334 requires us to return the
	     * same response to all subsequent PAP requests (whether the
	     * values change or not).  But since we want to return with the
	     * latest ID value, overwrite the current ID.  We know that this
	     * is a safe time to do it, because the first thing that
	     * pap_rrequest does, is set the current PAP ID.  It is this
	     * value that is used to generate the response ID.
	     */
	    current_id = id;
	    datagram_done(pak);
	    return;
	case DO_PAP_SENDREQUEST:
	    /*
	     * We are trying to send a PAP request so ignore this request.
	     *
	     * Note that we do not do a datagram_done() since PAP send
	     * requests are queued with a NULL pak pointer.
	     */
	    return;
	}
    }
    /*
     * Then we scan the queue for a duplicate entry that we can possibly
     * manipulate.
     */
    for (req = process_queue_peek(authqueue); req; req = req->next) {
	if ((req->idb == idb) && (req->op == op)) {
	    if (ppp_chap_debug) {
		buginf(ppp_duplicate_msg, idb->hw_namestring, id);
	    }
	    switch (op) {
	    case DO_CHAP_RCHALLENGE:
		/*
		 * We always respond to the most recent challenge so we want
		 * to replace the old challenge with the new one.  We could
		 * just check to see if it is a duplicate challenge, but
		 * then to be absolutely safe, we would have to compare the
		 * challenge strings.  It is just easier to replace the old
		 * with the new.
		 */
		break;
	    case DO_CHAP_RRESPONSE:
		/*
		 * If this is a duplicate CHAP response (same ID & timestamp)
		 * we want to ignore it as we will correctly respond when we
		 * get to this request and we don't want duplicate responses
		 * to break the OTP logic as TACACS+ re-authenticates.
		 * If this is not a duplicate response, then we will replace
		 * the current request with this new information and free up
		 * the original datagram.
		 */
		if ((req->id == id) &&
		    TIMERS_EQUAL(req->changed, idb->lcpstate->link_transition)) {
		    datagram_done(pak);
		    return;
		}
		break;
	    case DO_PAP_RREQUEST:
		/*
		 * The peer should only have one chance at getting in so we
		 * can safely drop this request as RFC 1334 requires us to
		 * return the same response to all subsequent PAP requests
		 * (whether the values change or not).  But we want to
		 * respond with the latest ID as the original ID will likely
		 * be ignored on the peer.
		 */
		if (TIMERS_EQUAL(req->changed, idb->lcpstate->link_transition)) {
		    req->id = id;
		    datagram_done(pak);
		    return;
		}
		break;
	    case DO_PAP_SENDREQUEST:
		/*
		 * We always just ignore duplicate requests since we will get
		 * the password request out as soon as possible.
		 *
		 * Note that we do not do a datagram_done() since PAP send
		 * requests are queued with a NULL pak pointer.
		 */
		return;
	    }
	    /*
	     * Fill in the fields that might change
	     */
	    if (req->pak) {
		datagram_done(req->pak);
	    }
	    req->pak = pak;
	    req->inp = inp;
	    req->id = id;
	    req->len = len;
	    req->changed = idb->lcpstate->link_transition;

	    if (ppp_chap_debug) {
		buginf(", replacing");
	    }
	    /*
	     * We can safely return now, since this logic will never let a
	     * another duplicate occur on the queue.
	     */
	    return;
	}
    }

    /*
     * Get a queue element for queueing the request
     */
    req = malloc_named(sizeof(aaa_req), "PPP AAA request");
    if (req == NULL) {
	datagram_done(pak);
	return;
    }

    /*
     * Fill in fields
     */
    req->op = op;
    req->pak = pak;
    req->idb = idb;
    req->inp = inp;
    req->id = id;
    req->len = len;
    req->changed = idb->lcpstate->link_transition;

    /*
     * Queue to process, and done for this thread
     * The enqueue cannot fail as there is no longer a queue limit.
     */
    process_enqueue(authqueue, req);
}

/*
 * chap_input
 * Process a chap packet received from the remote side
 */

void chap_input (paktype *pak, hwidbtype *idb)
{
    ppp_hdr *ppp_hdrp;
    u_char code, id, *inp;
    short len, l;

    if (ppp_packet_debug)
	auth_print(pak);

    l = pak->datagramsize;

    /*
     * parse header (code, id and length).
     * if packet too short, drop it.
     */
    ppp_hdrp = (ppp_hdr *)pak->network_start;
    inp = ((uchar *) ppp_hdrp) + HEADERLEN;
    if (l < HEADERLEN) {
	if (ppp_error_debug || ppp_chap_debug)
	    buginf("\nPPP %s: ppp rcvd short header for CHAP.", 
	           idb->hw_namestring);
	datagram_done(pak);
	return;
    }
    code = ppp_hdrp->code;
    id = ppp_hdrp->id;
    len = ppp_hdrp->plen;
    if (len < HEADERLEN) {
	if (ppp_error_debug || ppp_chap_debug)
	    buginf("\nPPP %s: ppp rcvd illegal length for CHAP.", 
	           idb->hw_namestring);
	datagram_done(pak);
	return;
    }
    if (len > l) {
	if (ppp_error_debug || ppp_chap_debug)
	    buginf("\nPPP %s: ppp rcvd short packet. len %d > %d "
		   "for PPP AUTH CHAP", idb->hw_namestring, len, l);
	datagram_done(pak);
	return;
    }
    len -= HEADERLEN;
    /*
     * Action depends on code.
     */
    switch (code) {
    case CHAP_CHALLENGE:
        /*
         * When this system has VPN.... 
         * If we sent out a challenge and is waiting for a response,
         * discard this received challenge. This forces this sequence 
         *  Client <------- CHAP Challenge -------- Nas
         *         ---------CHAP Response --------> possible projection 
         *                                          to Home-Gateway
         *         ---------CHAP Challenge -------> (Nth retry)
         *         <--------CHAP Response---------- 
         * 
         * By forcing this chronology, we ensure that if vpdn is in effect, 
         * the Nth retry CHAP challenge from the client is actually sent 
         * across to the Home-Gateway
         */
        if (reg_invoke_vpn_enabled() && !idb->authstate->we_said_ok) {
           if (ppp_chap_debug)
              buginf("\n%s PPP silent discard received Challenge", 
                    idb->hw_namestring);
           break;
        }

	queue_aaa_request(DO_CHAP_RCHALLENGE, pak, idb, inp, id, len);
	pak = NULL;
	break;
    case CHAP_RESPONSE:
        /*
         * Now that we have the remote client's name inside the CHAP
         * response, check if this user is to be VPN forwarded 
         * to some destination. 
         * If TRUE is returned, the link *being* forwarded (completion 
         * of forwarding is not implied). 
         * If False is returned, the link is not forwarded and 
         * we will continue as normal
         */
        if (ppp_vpn(DO_CHAP_RRESPONSE, pak, idb, inp, id, len)) {
            /*
             * We enter the FORWARDED state. Subsequent CHAP packets
             * are discarded. When the link is forwarded (asynchronous 
             * to this thread), the link becomes a PPP frame forwarded
             */
            pak = NULL;
            break;
        } 
	queue_aaa_request(DO_CHAP_RRESPONSE, pak, idb, inp, id, len);
	pak = NULL;
	break;
    case CHAP_SUCCESS:
        chap_rsuccess(idb, inp, id, len);
	break;
    case CHAP_FAILURE:
        chap_rfailure(idb, inp, id, len);
	break;
    default:
	if (ppp_error_debug || ppp_chap_debug) {
            buginf("\nPPP %s: PPP AUTH CHAP received unknown code",
	           idb->hw_namestring);
	}
	break;
    }

    if (pak) {
	datagram_done(pak);
    }
}

/*
 * pap_snak
 *	Nak the Authenication Request.
 */
static void pap_snak (
    hwidbtype *idb,
    uchar id,
    char *message)
{
    paktype *pak;
    int outlen;
    uchar *outp;
    fsm *f;

    f = idb->lcpfsm;

    /*
     * The PAP part of this packet looks as follows:
     *
     *	0x03, ID, 2 bytes of Length, message length, strlen(message) bytes
     *
     * 	NOTE: This differs from a CHAP packet because of the message
     *		  length. Message length is the length of the reason for the
     *		  NAK. This extra byte is the reason for adding a 1 to the
     *		  getbuffer call and the ppp_send_packet call.
     */ 
    outlen = strlen(message);
    pak = getbuffer(outlen + DLLHEADERLEN + HEADERLEN + 1);
    if (pak) {
	pak->datagramstart = pak->network_start - PPP_ENCAPBYTES;
	outp = PACKET_DATA(pak->datagramstart);
	*outp++ = outlen;
	bcopy(message, outp, outlen);
	ppp_send_packet(TYPE_PPP_UPAP, PAP_AUTH_NAK, id, pak, outlen+1, idb);
    }

    /*
     * buginf the same message sent to the peer
     */
    if (ppp_error_debug || ppp_chap_debug) {
	buginf("\nPPP %s: %s", idb->hw_namestring, message);
    }

    /*
     * If we sent a NAK then we have authentication failure.  The one possible
     * exception, would be a malloc failure which is not a failure on their
     * part, but we might as well hang up anyway.
     */
    ppp_bad_auth_attempt(idb, f, idb->authstate);
}

static char *pap_display_password_error (enum password_check_type check_it)
{
    switch (check_it) {
    case PASSWORD_NO_USER: return("USERNAME %s not found.");
    case PASSWORD_NO_PASSWORD: return("No password defined for USERNAME %s");
    case PASSWORD_NOT_CORRECT: return("Bad password defined for USERNAME %s");
    case PASSWORD_CORRECT: return("Password correct for USERNAME %s");
    }
    return(NULL);
}


/*
 * pap_sendok
 * Send an Authenticate-ACK to the peer.
 */
static void pap_sendok (hwidbtype *idb, uchar id)
{
    paktype *pak;
    int outlen;
    uchar *outp;

    outlen =  0;
    pak = getbuffer(outlen + DLLHEADERLEN + HEADERLEN + 1);
    if (!pak)
	return;
    pak->datagramstart = pak->network_start - PPP_ENCAPBYTES;
    outp = PACKET_DATA(pak->datagramstart);

    /*
     * The PAP part of this packet looks as follows:
     *
     *  0x03, ID, 2 bytes of Length, message length (0 in this case)
     *
     *  NOTE: This differs from a CHAP packet because of the message
     *        length. Message length is the length is set to zero for
     *        ACK. This extra byte is the reason for adding a 1 to the
     *        ppp_send_packet call.
     */
    *outp = outlen;
    ppp_send_packet(TYPE_PPP_UPAP, PAP_AUTH_ACK, id, pak, outlen+1, idb);
}

/*
 * pap_rrequest
 * Validate an Authenticate-Request packet from the remote.  Packet consists
 * of an id, a Peer-ID len, a Peer-ID, password len and a password.
 */
static void pap_rrequest (
    hwidbtype *idb,
    uchar *inp,
    uchar id,
    ushort len)
{
    auth_state *chapstate;
    usernametype callback_info;
    usernametype *info = NULL;
    char peersecret[PASSWORDLEN+1];	/* the one in the message recvd */
    uchar value_size;
    char *user;	
    fsm *f;
    lcp_options *go;
    lcp_state *lcpstate;
    char *reason;
    sys_timestamp state_time;

    state_time = idb->lcpstate->link_transition;

    /*
     * Save immediately in case a subsequent queue_aaa_request() needs to
     * overwrite.  That way we can respond to the last PAP Auth Request
     * received.  Responding to earlier requests is a waste of time.
     */
    current_id = id;

    /*
     * Get some local variables pointing to often used values
     */
    f = idb->lcpfsm;
    go = f->gotoptions;
    chapstate = idb->authstate;
    memset(&callback_info, 0, sizeof(usernametype));
    lcpstate = idb->lcpstate;

    /* Don't acknowledge PAP requests if we didn't negotiate PAP. */
    if (!go->neg_auth || (go->authtypes[0] != PPP_AUTH_PAP)) {
	if (ppp_error_debug || ppp_chap_debug)
	    buginf("\nPPP %s: Received PAP request but did not negotiate PAP.",
		   idb->hw_namestring);
	return;
    }

    /*
     * Check if we've already answered.  If we had already answered with an
     * ACK, we would have set the we_said_ok flag.  But that point forward,
     * we are always supposed to respond with an ACK to keep the peer from
     * trying alternate passwords in the background (RFC 1334).  This also
     * covers us when the authentication server takes so long that the peer
     * timeouts and resends.  In that case, we still need to send reply, but
     * we don't need to do additional state changes.
     */
    if (chapstate->we_said_ok) {
	if (ppp_chap_debug) {
	    buginf("\nPPP %s: Resending PAP Auth-Ack.", idb->hw_namestring);
	}
	pap_sendok(idb, current_id);
	return;
    }

    /*
     * get the Peer-id size.  Later get the value.  The name
     * after this is the name of the host challenging us.  Look for the
     * secret to use for that host.
     */
    GETCHAR(value_size, inp);

    /*
     * Apply a cap on the size
     *
     * Note that the Peer-id size is stored in a byte so the maximum
     * name size is 255.  Thus we do not need truncate as long as
     * MAXNAMELEN is >= 256.  Also note that a correctly formatted packet
     * will leave at least one byte at the end of the packet to store
     * the password length (which could be zero).
     */
    if (value_size > (len-1)) {
        pap_snak(idb, current_id, "PAP hostname extends past end of packet");
	return;
    }

    user = malloc_named(value_size+1, "pap_rrequest user");
    if (!user) {
        pap_snak(idb, current_id, "Not enough resources for authentication");
        return;
    }

    /*
     * position the pointer after the Peer-id size value so we can extract
     * their hostname.
     */
    sstrncpy(user, (char *)inp, value_size+1);

    /*
     * get the PwLen size.  Later get the value.  The password
     * after this is the password of the host challenging us.  Look for the
     * secret to use for that host.
     */
    inp += value_size;
    len -= value_size;
    GETCHAR(value_size, inp);

    /*
     * Apply sanity length on packet
     */
    if ((value_size >= sizeof(peersecret)) || (value_size > len)) {
	free(user);
        pap_snak(idb, current_id, "PAP password is too long");
	return;
    }

    /*
     * position the pointer after the PwLen size value so we can extract
     * their password.
     */
    sstrncpy(peersecret, (char *)inp, value_size+1);
    if (!strlen(user)) {
	free(user);
        pap_snak(idb, current_id, "No hostname received to authenticate");
        return;
    }

    if (ppp_chap_debug)
        buginf("\nPPP %s: PAP authenticating peer %s", idb->hw_namestring, user);

    if (old_access_control) {
	/*
	 * localauthen specifies local authentication mechanisms (ie TACACS)
	 * if the PPP_LAUTHEN_TACACS field is zero we are doing PAP so process
	 * the hostname and password ourselves otherwise let TACACS take
	 * care of it.
	 */
	if (!(f->localauthen & PPP_LAUTHEN_TACACS)) {
	    enum password_check_type check_it;
	    /*
	     * get the secret for their hostname.
	     */
	    check_it = check_local_password(user, peersecret, &info);
	    if (check_it != PASSWORD_CORRECT) {
		pap_snak(idb, current_id, "Authentication failure.");
		if (ppp_chap_debug) {
		    buginf(pap_display_password_error(check_it), user);
		}
		free(user);
		return;
	    }
	} else {	/* (f->localauthen & PPP_LAUTHEN_TACACS) */
	    /*
	     * We are running an authsubytpe of PAP-TACACS. Allow the TACACS
	     * code to verify the hostname and password. The TACACS code
	     * will return TRUE if the hostname and password is accepted.
	     */

	    if (ppp_chap_debug)
		buginf("\nPPP %s: performing lookup using TACACS.",
		       idb->hw_namestring);
	    authenticatingidb = idb;
	    if (!reg_invoke_login_login(idb->idb_tty, user, peersecret,
					&reason)) {
		free(user);
		pap_snak(idb, current_id, "Authentication failure.");
		return;
	    }
	    /* The login succeeded, so do a TACACS logout to keep the
	     * accounting straight and set lcpstate->user to point to
	     * copy of username.
	     */
	    authenticatingidb = idb;
	    reg_invoke_login_logout(idb->idb_tty);
	    setstring_named(&lcpstate->user, user, "PPP LCP user");
	    if (idb->idb_tty && !idb->idb_tty->user)
		setstring_named(&idb->idb_tty->user, user, "PPP TTY user");
	}
    } else { /* ! old_access_control */
	userstruct *ustruct;
	authenstruct astruct;
	authorstruct author;
	boolean ret;
	char *rem = NULL;

	memset(&astruct, 0, sizeof(astruct));
	if (idb->idb_tty) {
	    astruct.tty = idb->idb_tty;
	    rem = tty_get_remote_addr(idb->idb_tty);
	}
	if (is_isdn(idb) && (idb->isdn_info->remote_src == ISDN_SRC_ISDN)) {
	    rem = idb->isdn_info->remote_number;
	}
	ret = aaa_create_user(&ustruct, user, NULL, idb->hw_namestring, rem,
			      AAA_AUTHEN_TYPE_PAP, AAA_SVC_PPP, PRIV_USER);
	if (ret) {
	    astruct.user_data = peersecret;
	    astruct.user_data_len = value_size;
	    ret = aaa_start_login(ustruct, &astruct, lcpstate->acc_list_name,
				  AAA_AUTHEN_ACT_LOGIN, AAA_SVC_PPP);
	    astruct.user_data = NULL;
	    astruct.user_data_len = 0;
	}
	if (!ret || (astruct.status != AAA_AUTHEN_STATUS_PASS)) {
	    free(user);
	    pap_snak(idb, current_id, (ret && astruct.server_msg) ?
		     astruct.server_msg : "Password validation failure.");
	    aaa_cleanup_login(ustruct, &astruct);
	    aaa_free_user(&ustruct);
	    return;
	}
	aaa_cleanup_login(ustruct, &astruct);

	/* Do authorization */
	if (!lcp_do_author(ustruct, &author, idb, f, &callback_info)) {
	    clear_usernametype(&callback_info);
	    aaa_free_user(&ustruct);
	    free(user);
	    pap_snak(idb, current_id, author.server_msg ? author.server_msg :
		     "Authorization failed");
	    aaa_cleanup_author(&author);
	    return;
	}
	aaa_cleanup_author(&author);
	aaa_free_user(&lcpstate->ustruct);
	lcpstate->ustruct = ustruct;

	if (idb->idb_tty) {
	    if (aaa_acct_nested) {
		/*
		 * If we started PPP from the EXEC prompt, we want the network
		 * accounting records to be generated before the EXEC-STOP
		 * record.  This will have the side-effect that the "show user"
		 * output for this connection will show the user authenticated
		 * at EXEC-authentication and not PPP-authentication.
		 */
		if (!idb->idb_tty->ustruct) {
		    aaa_lock_user(ustruct);
		    idb->idb_tty->ustruct = ustruct;
		}
	    } else {
		/*
		 * replace whatever authentication information
		 * existed with the re-authentication we just did
		 * Doing this will also generate an EXEC-STOP record
		 * before the NETWORK-START record if PPP was started
		 * at the EXEC prompt.
		 */
		if (idb->idb_tty->ustruct) 
		    aaa_free_user(&idb->idb_tty->ustruct);
		aaa_lock_user(ustruct);
		idb->idb_tty->ustruct = ustruct;
	    }
	}

        /*
         * Send a ptr to our automatic storage to lcp_authcomplete
         */
        info = &callback_info;
    }

    /*
     * Check to see if the interface changed state since we started.  If it
     * did, then we should not do anything more with request.
     */
    if (TIMERS_NOT_EQUAL(state_time, idb->lcpstate->link_transition)) {
	return;
    }

    /*
     * We have a valid password and a match. We need to send an Ack.
     */
    if (ppp_chap_debug) {
	buginf("\nPPP %s: Remote passed PAP authentication sending Auth-Ack.",
	       idb->hw_namestring);
    }
    pap_sendok(idb, current_id);
    chapstate->we_said_ok = TRUE;

    /*
     * Just in case we were waiting for the peer to go first, now is the
     * time to respond to its Challenge.
     */
    if (chapstate->chap_resp_pkt &&
	chap_hold_response(idb)) {
	resend_chap_response(idb);
    }

    /*
     * In case we are a dial on demand interface, inform the dialing
     * software who we are connected to so that when it replys to
     * packets it knows which interface to send them to.
     * update idb->remote_name whenever we authenticate another system
     */
    reg_invoke_dialer_remote_name(idb, user);

    /*
     * update idb->remote_name whenever we authenticate another system
     */
    new_remote_name(chapstate, user);

    /*
     *  The following code depends on remote_name to be updated
     *    Do not move it above reg_invoke_dialer_remote_name;
     */

    if (chapstate->they_said_ok)
 	lcp_authcomplete(idb, info);
    clear_usernametype(&callback_info);
    free(user);
}

/*
 * pap_rack
 * Process a received PAP ACK packet.  If we aren't requiring them
 * to authenticate, or if they have already authenticated, then tell lcp
 * that authentication is completed.  Otherwise, wait until they have
 * authenticated.
 */

static void pap_rack (
    hwidbtype *idb,
    uchar *inp,
    uchar id,
    ushort len)
{
    auth_state *chapstate;
    fsm *f;
    lcp_options *wo;
    char *message;


    f = idb->lcpfsm;
    wo = f->wantoptions;
    chapstate = idb->authstate;

    /*
     * if the received id isn't equal to the id we transmitted, then
     * just ignore this response
     */
    if (chapstate->pap_id != id) {
	if (ppp_error_debug || ppp_chap_debug) {
	    buginf("\nPPP %s: PAP ACK ignored.  Expected id %d, got id %d.",
		   idb->hw_namestring, chapstate->pap_id, id);
	}
        return;
    }

    if (ppp_chap_debug) {
        buginf("\nPPP %s: PAP ACK", idb->hw_namestring);
	if (len) {
	    message = malloc_named(len + 1, "PPP PAP message");
	    if (message) {
	        sstrncpy(message, (char *)inp, len+1);
                buginf("\nRemote message is: %s", message);
	        free(message);
	    }
        }
    }

    chapstate->num_failures = 0;
    if (ppp_chap_debug)
        buginf("\nPPP %s: Passed PAP authentication with remote", 
	       idb->hw_namestring);
    chapstate->they_said_ok = TRUE;
    if (chapstate->we_said_ok)
        lcp_authcomplete(idb, NULL);
}

/*
 * pap_rnak
 * Process a received PAP NAK packet.
 */

static void pap_rnak (
    hwidbtype *idb,
    uchar *inp,
    uchar id,
    ushort len)
{
    char *message;
    fsm *f;
    lcp_options *wo;

    f = idb->lcpfsm;
    wo = f->wantoptions;

    if (ppp_chap_debug)
        buginf("\nPPP %s: Failed PAP authentication with remote.",
	       idb->hw_namestring);
    if (len) {
        message = malloc_named(len + 1, "PPP PAP fail");
        if (!message)
            return;
        sstrncpy(message, (char *)inp, len+1);
	buginf("\nRemote message is: %s", message);
        free(message);
    }
}

/*
 * pap_input
 * Process a pap packet received from the remote side
 */

void pap_input (paktype *pak, hwidbtype *idb)
{
    ppp_hdr *ppp_hdrp;
    u_char code, id, *inp;
    short len, l;
    fsm *f;
    lcp_options *wo;

    f = idb->lcpfsm;
    wo = f->wantoptions;
    l = pak->datagramsize;

    if (ppp_packet_debug)
	auth_print(pak);

    /*
     * parse header (code, id and length).
     * if packet too short, drop it.
     */
    ppp_hdrp = (ppp_hdr *)pak->network_start;
    inp = ((uchar *) ppp_hdrp) + HEADERLEN;
    if (l < HEADERLEN) {
	if (ppp_error_debug || ppp_chap_debug)
	    buginf("\nPPP %s: ppp rcvd short header for PAP.",
		   idb->hw_namestring);
	datagram_done(pak);
	return;
    }

    code = ppp_hdrp->code;
    id = ppp_hdrp->id;
    len = ppp_hdrp->plen;
    if (len < HEADERLEN) {
	if (ppp_error_debug || ppp_chap_debug)
	    buginf("\nPPP %s: ppp rcvd illegal length for PAP.",
		   idb->hw_namestring);
	datagram_done(pak);
	return;
    }

    if (len > l) {
	if (ppp_error_debug || ppp_chap_debug)
	    buginf("\nPPP %s: ppp rcvd short packet. len %d > %d for PAP",
		   idb->hw_namestring, len, l);
	datagram_done(pak);
	return;
    }

    len -= HEADERLEN;

    /*
     * Action depends on code.
     */
    switch (code) {
    case PAP_AUTH_REQ:
        /*
         * Check if this user is to be VPN forwarded 
         * to some destination. 
         * If TRUE is returned, the link *being* forwarded (completion 
         * of forwarding is not implied). 
         * If False is returned, the link is not forwarded and 
         * we will continue as normal
         */
        if (ppp_vpn(DO_PAP_RREQUEST, pak, idb, inp, id, len)) {
            /*
             * We enter the FORWARDED state. Subsequent CHAP packets
             * are discarded. When the link is forwarded (asynchronous 
             * to this thread), the link becomes a PPP frame forwarded
             */
            pak = NULL;
            break;
        }
	queue_aaa_request(DO_PAP_RREQUEST, pak, idb, inp, id, len);
	pak = NULL;
	break;
    case PAP_AUTH_ACK:
        pap_rack(idb, inp, id, len);
	break;
    case PAP_AUTH_NAK:
	len = *inp++;		/* get the length of the error string */
        pap_rnak(idb, inp, id, len);
	break;
    default:
	if (ppp_error_debug || ppp_chap_debug) {
	    buginf("\nPPP %s: PAP%s received unknown code %d", 
	           idb->hw_namestring, code);
	}
	break;
    }

    if (pak) {
	datagram_done(pak);
    }
}

/*
 * chap_protrej
 * Called when a remote rejects a chap packet.  In theory this will never
 * happen as if they've negotiated CHAP, they should accept CHAP packets.
 * But it is does occur, consider everything down.
 */

void auth_protrej (hwidbtype *idb, ushort protocol)
{
    if (ppp_error_debug || ppp_chap_debug) {
        buginf("\nPPP %s: Protocol reject received for CHAP.",
	       idb->hw_namestring);
    }
    fsm_lowerdown(idb->lcpfsm);
}

/*
 * chap_sendchallenge
 * Send a challenge packet to the remote.  Packet consists of an id, and a
 * random number (we use msclock).
 *
 * Note: We always send CHAP challenge of size CHAP_CHALLENGE_SIZE.
 */

static void chap_sendchallenge (hwidbtype *idb)
{
    auth_state *chapstate;
    paktype *pak;
    int outlen, namelen;
    uchar *outp;
    char *name;

    chapstate = idb->authstate;

    /*
     * First, we should check the authentication queue to make sure that we
     * are not holding his response for AAA to verify.  Why challenge him
     * when we are about to find out if he knows the secret.
     */
    if (find_aaa_request(DO_CHAP_RRESPONSE, idb, chapstate->chap_id)) {
	return;
    }

    name = ppp_hostname(idb);
    namelen = strlen(name);

    chapstate->chap_id++;			/* send a unique ID */
    chapstate->we_said_ok = FALSE;

    if (chapstate->challenge_size != CHAP_CHALLENGE_SIZE) {
        if (chapstate->challenge_value)
            free(chapstate->challenge_value);
        chapstate->challenge_value =
            malloc_named(CHAP_CHALLENGE_SIZE, "PPP CHAP challenge");
        if (!chapstate->challenge_value) {
            chapstate->challenge_size = 0;
            return;
        }
    }

    /*
     * Fill in our challange, which should be unique (not repeated)
     * and random.  Note that random_gen() only generates 0..10000,
     * so we use something else (plus extra bits to ensure uniqueness)
     */
    random_fill(chapstate->challenge_value, CHAP_RANDOM_FILL_SIZE, FALSE);
    /*
     * After filling some random values, we add two signs to allow us to
     * detect spoofing.
     */
    chap_sign_challenge(chapstate->challenge_value,
			CHAP_CHALLENGE_SIZE,
			idb->lcpstate->call_direction,
			name,
			namelen);

    /*
     * outlen is the size of a challenge packet.  It consists of the 
     * value size and the name size.
     */
    outlen = sizeof(uchar) + CHAP_CHALLENGE_SIZE + namelen;
    pak = getbuffer(outlen + DLLHEADERLEN + HEADERLEN);
    if (!pak)
	return;
    pak->datagramstart = pak->network_start - DLLHEADERLEN;
    outp = PACKET_DATA(pak->datagramstart);

    PUTCHAR(CHAP_CHALLENGE_SIZE, outp);
    chapstate->challenge_size = CHAP_CHALLENGE_SIZE;
    bcopy(chapstate->challenge_value, outp, CHAP_CHALLENGE_SIZE);
    outp += CHAP_CHALLENGE_SIZE;
    bcopy(name, outp, namelen);
    ppp_send_packet(TYPE_PPP_CHAP, CHAP_CHALLENGE, chapstate->chap_id, pak,
		    outlen, idb);

    if (ppp_chap_debug)
        buginf("\nPPP %s: Send CHAP Challenge id=%d", 
	       idb->hw_namestring, chapstate->chap_id);
}

/*
 * pap_sendrequest
 * Send an Authenticate-Request  packet to the remote.  Packet consists 
 * of an id, a Peer-ID len, a Peer-ID, password len and a password.
 * Note that we use the configured sent-username/password if configured,
 * else expect that AAA can provide the information.
 */

static void pap_sendrequest (hwidbtype *idb)
{
    auth_state *chapstate;
    paktype *pak;
    int outlen, namelen, pwlen;
    uchar *outp;
    char password[PASSWORDLEN+1], *name, *pw;
    userstruct *ustruct;
    authenstruct astruct;
    lcp_state *lcpstate;
    hwidbtype *lead;
    sys_timestamp state_time;

    state_time = idb->lcpstate->link_transition;

    ustruct = NULL;

    chapstate = idb->authstate;
    chapstate->pap_id++;                /* send a unique ID */
    chapstate->they_said_ok = FALSE;

    lcpstate = idb->lcpstate;

    if (lcpstate->pap_sent_name) {
	name = lcpstate->pap_sent_name;
        decrypt(lcpstate->pap_sent_pw, password, PASSWORDLEN);
	pw = password;
	pwlen = strlen(pw);
    } else if ((lead = get_netidb(idb)) && lead->lcpstate &&
	    lead->lcpstate->pap_sent_name) {
	name = lead->lcpstate->pap_sent_name;
	decrypt(lead->lcpstate->pap_sent_pw, password, PASSWORDLEN);
	pw = password;
	pwlen = strlen(pw);
    } else {
	if (old_access_control) {
	    if (ppp_error_debug || ppp_chap_debug) {
	        buginf("\nPPP %s: ppp pap sent-username not configured",
	               idb->hw_namestring);
	    }
	    return;
	} else {	/* ! old_access_control */
	    char *rem = NULL;
	    boolean ret;

	    bzero(&astruct, sizeof(astruct));
	    if (idb->idb_tty) {
	        astruct.tty = idb->idb_tty;
	        rem = tty_get_remote_addr(idb->idb_tty);
	    }
	    if (is_isdn(idb) && (idb->isdn_info->remote_src == ISDN_SRC_ISDN)) {
	        rem = idb->isdn_info->remote_number;
	    }
	    ret = aaa_create_user(&ustruct, hostname, NULL, idb->hw_namestring,
				  rem, AAA_AUTHEN_TYPE_PAP, AAA_SVC_PPP,
				  PRIV_USER);
	    if (ret)
	        ret = aaa_start_login(ustruct, &astruct,
				      lcpstate->acc_list_name,
				      AAA_AUTHEN_ACT_SENDAUTH, AAA_SVC_PPP);
	    if (!ret || (astruct.status != AAA_AUTHEN_STATUS_PASS)) {
	        if (ppp_error_debug || ppp_chap_debug) {
		    buginf("\nPPP %s: Failed request for PAP credentials.  "
		           "USERNAME %s.", idb->hw_namestring, ustruct->user);
	        }
	        aaa_cleanup_login(ustruct, &astruct);
	        aaa_free_user(&ustruct);
	        return;
	    }
        }
	pw = astruct.server_data;
	pwlen = astruct.server_data_len;
	name = ustruct->user;
    }

    /*
     * Check to see if the interface changed state since we started.  If it
     * did, then we should not do anything more with request.
     */
    if (TIMERS_NOT_EQUAL(state_time, idb->lcpstate->link_transition)) {
	return;
    }

    /*
     * outlen is the size of a request packet.  It consists of the
     * ID len, Peer ID, Pw Len and the password.
     */
    namelen = strlen(name);
    outlen = sizeof(uchar) + namelen + sizeof(uchar) + pwlen;
    pak = getbuffer(outlen + DLLHEADERLEN + HEADERLEN);
    if (pak) {
        pak->datagramstart = pak->network_start - PPP_ENCAPBYTES;
        outp = PACKET_DATA(pak->datagramstart);

        PUTCHAR(namelen, outp);
        bcopy(name, outp, namelen);
        outp += namelen;
        PUTCHAR(pwlen, outp);
        bcopy(pw, outp, pwlen);
        ppp_send_packet(TYPE_PPP_UPAP, PAP_AUTH_REQ, chapstate->pap_id, pak,
		        outlen, idb);
    }
    if (ustruct) {
	aaa_cleanup_login(ustruct, &astruct);
	aaa_free_user(&ustruct);	/* needed until name/pw is copied */
    }
}

/*
 * auth_timeout
 * Handle CHAP timeouts.  Retransmit another challenge unless the max has
 * been reached.  In that case, shut everything down.
 */

void auth_timeout (hwidbtype *idb)
{
    fsm *f;
    auth_state *chapstate;
    lcp_options *go;
    lcp_options *ho;

    chapstate = idb->authstate;

    /*
     * Sanity check to ensure that we are still authenticating.  This is
     * likely to be the mechanism by which the CHAP response packet is
     * finally flushed and the timer is stopped.  Both of these steps
     * should be done in a 'auth_stop()' function called from lcp_closed()
     * lcp_authcomplete(), but for now this is a safety check.
     */
    if (idb->lcpstate->phase != AUTHENTICATING) {
	flush_chap_resp_pkt(idb);
	TIMER_STOP(chapstate->retry_timer);
	return;
    }

    /*
     * We keep the timer running because we don't want to wait for the
     * peer forever.
     */

    TIMER_START(chapstate->retry_timer, DEFCHAP_TIMEOUT);

    chapstate->num_failures++;

    if (chapstate->num_failures > MAXCHAP_FAILURES) {
        lcp_close(idb->lcpfsm);
    } else {
	f = idb->lcpfsm;
	go = f->gotoptions;
	ho = f->heroptions;

	if (!chapstate->we_said_ok) {
	    if (go->neg_auth && go->authtypes[0] == PPP_AUTH_CHAP) {
       	        chap_sendchallenge(idb);
	    }
	}
	if (!chapstate->they_said_ok) {
	    if (ho->authtypes[0] == PPP_AUTH_PAP) {
	        /*
	         * Queue PAP send request to AAA auth task because
	         * pap_sendrequest() calls AAA code that can dismiss.
	         */
	        queue_aaa_request(DO_PAP_SENDREQUEST, NULL, idb, NULL, 0, 0);
	    } else if (ho->authtypes[0] == PPP_AUTH_CHAP) {
		/*
		 * Resend last CHAP response packet since it may have gotten
		 * lost.  Do immediately since the packet is already formed.
		 */
		if (chapstate->chap_resp_pkt) {
		    resend_chap_response(idb);
		}
	    }
	}
    }
}

/*
 * auth_print
 * buginf out an entire authentication packet.
 */
static char *const pap_names[] = {
    "??",			/* 0 oops */
    "AUTH-REQ",			/* 1 Authentication request */
    "AUTH-ACK",			/* 2 Authentication Ack */
    "AUTH-NAK"			/* 3 Authentication Nak */
    };
#define MAX_PAPNAMES 3

static char *const chap_names[] = {
    "??",			/* 0 oops */
    "CHALLENGE",		
    "RESPONSE",			
    "SUCCESS",			
    "FAILURE"
    };
#define MAX_CHAPNAMES 4

void auth_print (paktype *pak)
{
    char protocol[100];
    uchar *p = (uchar *)(pak->network_start);
    idbtype *idb = pak->if_input;

    if (!idb)
	idb = pak->if_output;

    if (ppp_get_protocol_type(pak) == TYPE_PPP_CHAP) {
	sprintf(protocol, "PPP %s: %c CHAP %s(%d) id %d len %d",
		idb ? idb->namestring : "?", pak->if_input ? 'I' : 'O',
		*p > MAX_CHAPNAMES ? "UNKNOWN" : chap_names[*p],
		*p,
		*(p+1),
		*(short *) (p+2));
	buginf("\n%s", protocol);
    } else {
	sprintf(protocol, "PPP %s: %c PAP %s(%d) id %d len %d",
		idb ? idb->namestring : "?", pak->if_input ? 'I' : 'O',
		*p > MAX_PAPNAMES ? "UNKNOWN" : pap_names[*p],
		*p,
		*(p+1),
		*(short *) (p+2));
	buginf("\n%s", protocol);
    }
}

boolean lcp_do_author (userstruct *u, authorstruct *a, hwidbtype *idb, 
		       fsm *f, usernametype *callback_info)
{
    boolean ret = TRUE;
    int i, avail = 0;
    char *attr, *val;
    enum AV_TYPE type;

    memset(a, 0, sizeof(authorstruct));
    if (!aaa_author_needed(AAA_AUTHOR_TYPE_NET, 0, idb, idb->idb_tty))
	return(TRUE);	/* Authorization not required. */

    a->auth_idb = idb;
    a->auth_tty = idb->idb_tty;

    /* First step.  Build outgoing A-V lists. */
    if (!aaa_add_AV(&a->out_args, &avail, AAA_ATTR_service, AAA_VAL_SVC_ppp,
		    AAA_AV_MAND) ||
	!aaa_add_AV(&a->out_args, &avail, AAA_ATTR_protocol, AAA_VAL_PROT_lcp,
		    AAA_AV_MAND))
	ret = FALSE;

    if (ret) {
	/* Second step.  Ask. */
	if (aaa_author_debug) {
	    buginf("\nAAA/AUTHOR/LCP: authorize LCP");
	}

	ret = aaa_do_author(u, a, AAA_AUTHOR_TYPE_NET, 0);

	/* Third step.  Deal with response. */
	switch(a->status) {
	case AAA_AUTHOR_STATUS_ERROR:
	case AAA_AUTHOR_STATUS_FAIL:
	    ret = FALSE;
	    break;
	case AAA_AUTHOR_STATUS_PASS_ADD:
	    if (!a->in_args) {
		aaa_clear_attrs(&f->cached_author);
		f->cached_author = aaa_copy_attrs(a->out_args);
		break;
	    }
	    aaa_merge_attrs(a->out_args, a->in_args, &a->in_args);
	    /* FALLTHRU */
	case AAA_AUTHOR_STATUS_PASS_REPL:
	    if (!a->in_args) {
		aaa_clear_attrs(&f->cached_author);
		break;
	    }
	    for (i = 0; ret && a->in_args[i]; i++) {
		if (aaa_author_debug) {
		    buginf("\nAAA/AUTHOR/LCP: %s: Processing AV %s", 
			   idb->hw_namestring, a->in_args[i]);
		}
		if (aaa_split_AV(a->in_args[i], &attr, &val, &type)) {
		    if (!strcmp(attr, AAA_ATTR_service)) {
			if (strcmp(val, AAA_VAL_SVC_ppp)) {
			    if (type == AAA_AV_MAND) {
				if (aaa_author_debug)
				    buginf("\nAAA/AUTHOR/LCP: "
					   "attempt to change service to %s"
					   " Denied.", val);
				ret = FALSE;
			    }
			    else
				if (aaa_author_debug)
				    buginf("\nAAA/AUTHOR/LCP: "
					   "tried to change \"service\" to %s "
					   "Ignoring.", val);
			}
			continue;
		    }
		    if (!strcmp(attr, AAA_ATTR_protocol)) {
			if (strcmp(val, AAA_VAL_PROT_lcp)) {
			    if (type == AAA_AV_MAND) {
				if (aaa_author_debug)
				    buginf("\nAAA/AUTHOR/LCP: "
					   "Attempt to change protocol to %s "
					   "denied", val);
				ret = FALSE;
			    }
			    else
				if (aaa_author_debug)
				    buginf("\nAAA/AUTHOR/LCP: "
					   "tried to change \"protocol\".  "
					   "Ignoring.");
			}
			continue;
		    }

                    if (!strcmp(attr, AAA_ATTR_callback) && 
                        callback_info) {
			setstring_named(&callback_info->dial_string, val,
					"AAA callback");
                        callback_info->flags |= USER_CALLBACK_DIALSTRING;
			continue;
		    }

                    if (!strcmp(attr, AAA_ATTR_callback_noverify)) { 
                        if (atoi(val) == 1) 
			   callback_info->flags |= USER_NOCALLBACK_VERIFY;
			continue;
		    }
 
                    if (!strcmp(attr, AAA_ATTR_callback_rotary) && 
                        callback_info) {
                        callback_info->callback_rotary_or_line = atoi(val);
			callback_info->flags |= USER_CALLBACK_ROTARY;
			continue;
		    }

                    if (!strcmp(attr, AAA_ATTR_callback_line) &&
                        callback_info) {
                        callback_info->callback_rotary_or_line = atoi(val);
			callback_info->flags |= USER_CALLBACK_LINE;
			continue;
		    }

		    /* DAVE -- Futures - Add in processing for
		     *		timeout
		     *		idletime
		     */
		    /* if unknown and mandatory, then bail */
		    if (type == AAA_AV_MAND) {
			if (aaa_author_debug)
			    buginf("\nAAA/AUTHOR/LCP: Unknown mandatory "
				   "attribute %s.  Denied", attr);

			ret = FALSE;
		    }
		}
	    }
	    if (ret) {
		aaa_clear_attrs(&f->cached_author);
		f->cached_author = aaa_copy_attrs(a->in_args);
	    }
	    break;
	}
    }
    if (!ret && aaa_author_debug) {
	buginf("\nAAA/AUTHOR/LCP: denied");
    }
    return(ret);
}


/*
 * ppp_vpn
 * Gather the CHAP response or PAP request information and call 
 * VPN to attempt to forward this particular user. 
 */ 
static boolean
ppp_vpn (enum aaa_op op, paktype *pak, hwidbtype *idb, u_char *inp,
	 u_char id, short len)
{
    auth_state *chapstate;

    /*
     * A packet that has already gone thru this iteration must 
     * not go thru again
     */
    if (pak->flags & PAK_CONTROL) {
        pak->flags &= ~PAK_CONTROL;
        return(FALSE);
    }

    chapstate = idb->authstate;

    if (!ppp_authen_prepare(op, idb, chapstate, inp, id, len))
        return(FALSE);

    switch (op) {
    case DO_CHAP_RRESPONSE:
        return(reg_invoke_vpn(chapstate->remote_name, idb, pak, TYPE_PPP_CHAP));

    case DO_PAP_RREQUEST:
        return(reg_invoke_vpn(chapstate->remote_name, idb, pak, TYPE_PPP_UPAP));
     
    default:
        return(FALSE);
    }
}

/*
 * Prepare authentication content in authstate
 */
static boolean
ppp_authen_prepare (enum aaa_op op, hwidbtype *idb, auth_state *chapstate,
		    u_char *inp, u_char id, short len)
{
    char user[MAXSTR+1];
    uchar *md_stream;
    uchar value_size;
    int user_len;
    fsm *f;
    lcp_options *go;

    if (!idb || !chapstate || !inp)
       return(FALSE);

    bzero(user, MAXSTR+1);

    switch (op) {
     case DO_CHAP_RRESPONSE:

       /*
        * if the received id isn't equal to the id we transmitted, then
        * just ignore this response
        */
       if (chapstate->chap_id != id)
          return(FALSE);
    
       /*
        * We may get a response in after we've already authenticated them.
        * Just return.
        */
       if (chapstate->we_said_ok)
	  return(FALSE);

       /*
        * Get the name of the host that has responded to our challenge
        * Get the value size.  Later get the value.  The name
        * after this.
        */
       GETCHAR(value_size, inp);

       if (value_size != MD5_LEN) {
	  if (ppp_error_debug || ppp_chap_debug)
              buginf("\nPPP %s: invalid MD5 response received.", 
	             idb->hw_namestring);
          return(FALSE);
       }

       md_stream = inp;
     
       /*
        * position the pointer after the digest value so we can extract the
        * hostname.
        */
       inp += value_size;

       user_len =  (len - value_size - sizeof(uchar) + 1);
       if (user_len > MAXSTR) {
            return(FALSE);
       }

       sstrncpy(user, (char *)inp, user_len);
  
       if (!strlen(user)) {
	   if (ppp_chap_debug)
               buginf("\nPPP %s: Unable to authenticate.  No name received"
                      " from peer", idb->hw_namestring);
           return(FALSE);
       }

       if (ppp_chap_debug)
           buginf("\nPPP %s: CHAP response received from %s", 
                  idb->hw_namestring, user);

       /*
        * Store the response values in the authstate
        */
       chapstate->value_size = value_size;
       bcopy(md_stream, chapstate->md_stream, MD5_LEN);
       if (!new_remote_name(chapstate, user)) {
           return(FALSE);	/* can't do VPN without a remote name */
       }
       break;

     case DO_PAP_RREQUEST:

       f = idb->lcpfsm;
       go = f->gotoptions;

       /* 
        * Don't acknowledge PAP requests if we didn't negotiate PAP. 
        */
       if (!go->neg_auth || (go->authtypes[0] != PPP_AUTH_PAP)) {
	if (ppp_error_debug || ppp_chap_debug)
	    buginf("\nPPP %s: Received PAP request but did not negotiate PAP.",
		   idb->hw_namestring);
	return(FALSE);
       }

       /*
        * Get the name of the host in the PAP request 
        * Get the Peer-id size.  Later get the value.  The name
        * after this is the name of the host.  
        */
       GETCHAR(value_size, inp);

       if (value_size == 0) {
        return(FALSE);
       }

       /*
        * Apply a cap on the size
        */
       if (value_size > MAXSTR) {
	 value_size = MAXSTR;
       }
       if (value_size > len) {
	 value_size = len;
       }

       /*
        * Extract username
        */
       sstrncpy(user, (char *)inp, (value_size + 1));

       /*
        * get the PwLen size.  The password after this is the 
        * password of the host challenging us.  
        * Look for the secret to use for that host.
        */
       inp += value_size;
       len -= value_size;
       GETCHAR(value_size, inp);

       /*
        * Apply sanity length on packet
        */
       if (value_size > PASSWORDLEN) {
	   value_size = PASSWORDLEN;
       }
       if (value_size > len) {
	value_size = len;
       }

       if (ppp_chap_debug)
        buginf("\nPPP %s: PAP receive authenticate request %s",
	       idb->hw_namestring, user);
       chapstate->value_size = value_size;
       sstrncpy(chapstate->peersecret, (char *)inp, (value_size + 1));
       chapstate->peer_pap_id = id;
       if (!new_remote_name(chapstate, user)) {
           return(FALSE);	/* can't do VPN without a remote name */
       }
       break;

    default:
        return(FALSE);

    }
    return(TRUE);
}

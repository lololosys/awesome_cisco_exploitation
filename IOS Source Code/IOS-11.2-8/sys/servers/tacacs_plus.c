/* $Id: tacacs_plus.c,v 3.15.4.16 1996/09/13 19:12:06 lol Exp $
 * $Source: /release/112/cvs/Xsys/servers/tacacs_plus.c,v $
 *------------------------------------------------------------------
 * tacacs_plus.c -- The TACACS+ subsystem.
 *
 * September 1994, Dave Carrel
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: tacacs_plus.c,v $
 * Revision 3.15.4.16  1996/09/13  19:12:06  lol
 * CSCdi69012:  tacacs+: no address for get_server
 * Branch: California_branch
 * Un-initialised stack variable. Bad programmer, no diet-coke...
 *
 * Revision 3.15.4.15  1996/08/28  13:08:58  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.15.4.14  1996/08/12  15:41:12  che
 * CSCdi65727:  Authentication loops if no tacacs-server directed-request
 * Branch: California_branch
 * Go back to tacacs_plus_start_login() to parse directed-request server
 * only if directed-request is set.
 *
 * Revision 3.15.4.13  1996/08/01  00:57:28  che
 * CSCdi64804:  tacacs direted-request broken
 * Branch: California_branch
 * Check for directed-server request in tacacs_plus_cont_login().
 *
 * Revision 3.15.4.12  1996/07/23  00:20:58  perryl
 * CSCdi60963:  %ALIGN-1-FATAL ,addr=0x1, pc=0x607945B0, ra=0x601A8010,
 * sp=0x60DA0AD
 * Branch: California_branch
 * Remove the count check in tacacs_attr_convert(), so we are only
 * limited by the size of acct_data rather than AAA_ACCT_NAVS, 30
 * records.
 * Add 1 to the avs[] in create_acct_req(), so there is always an
 * empty slot at the end of avs[]. Also allow dynamic allocated buffer
 * if avs[] is not big enough
 *
 * Revision 3.15.4.11  1996/07/06  05:56:27  thille
 * CSCdi62136:  seq: iphost and req: iphost duplicated and waste memory
 * Branch: California_branch
 * Put copies of the strings in common_strings.c/h and reference them
 * instead of having multiple copies of the literals.  Save 660 bytes.
 *
 * Revision 3.15.4.10  1996/06/25  23:51:47  che
 * CSCdi60064:  tacacs directed request does not use ip host properly.
 * Branch: California_branch
 * Walk through all IP addresses of a specified directed-host
 * before trying list of tacacs-servers.
 *
 * Revision 3.15.4.9  1996/06/04  23:52:50  lol
 * CSCdi59432:  T+ chap always succeeds
 * Branch: California_branch
 *
 * Revision 3.15.4.8  1996/05/21  10:01:38  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.15.4.7  1996/05/11  06:23:21  lol
 * CSCdi55924:  show tacacs displays handle(0x0) ??? output with
 * Ciscosecure daemon
 * Branch: California_branch
 *
 * Revision 3.15.4.6  1996/04/24  02:59:56  lol
 * CSCdi55359:  TACACS events debug output displays handle (0X0) ????
 * Branch: California_branch
 * Cosmetic changes to T+ output formats.
 *
 * Revision 3.15.4.5  1996/04/20  20:55:00  lol
 * CSCdi52836:  T+ tcp open timeouts are not accurate enough
 * Branch: California_branch
 *
 * Revision 3.15.4.4  1996/04/19  15:29:37  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commit
 *
 * Revision 2.1.6.10  1996/04/10  23:08:27  lol
 * Branch: IosSec_branch
 *         Incorporate code review comments.
 *
 * Revision 2.1.6.9  1996/04/09  21:45:22  che
 * Branch: IosSec_branch
 * Sync to California_branch (960405)
 *
 * Revision 2.1.6.8  1996/03/22  17:25:13  lol
 * Branch: IosSec_branch
 *         Modularise tacacs_plus tcp handle cleanup.
 *
 * Revision 2.1.6.7  1996/03/19  08:33:08  che
 * Branch: IosSec_branch
 * Sync to V111_1_3
 *
 * Revision 2.1.6.6  1996/02/28  06:15:10  lol
 * Branch: IosSec_branch
 * TACACS+ processing.
 *
 * Revision 2.1.6.5  1996/02/21  03:59:14  che
 * Branch: IosSec_branch
 * Sync to 11.1 Mainline (02/20/96)
 *
 * Revision 2.1.6.4  1996/01/23  06:32:43  che
 * Branch: IosSec_branch
 * Sync with 11.1 mainline. (960122)
 *
 * Revision 2.1.6.3  1995/12/19  04:13:09  che
 * Branch: IosSec_branch
 * Re-sync to Arkansas to fix things that broke.
 *
 * Revision 2.1.6.2  1995/12/17  02:16:24  che
 * Branch: IosSec_branch
 * Sync to Arkansas
 *
 * Revision 2.1.6.1  1995/11/17  17:39:36  carrel
 * Branch: IosSec_branch
 * Sync to Arkansas
 *
 * Revision 3.15.4.3  1996/04/15  14:59:53  widmer
 * CSCdi47180:  No Single Command to Collect General Router Data
 * Branch: California_branch
 * Add "show tech-support" command
 * Change references to encryption types to uint
 * Change tt_soc to use password_struct
 *
 * Revision 3.15.4.2  1996/03/27  07:51:07  billw
 * CSCdi51197:  memory leak in tacacs+ accounting
 * Branch: California_branch
 *
 * Revision 3.15.4.1  1996/03/18  21:47:09  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.10.2.3  1996/03/16  07:28:50  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.10.2.2  1996/03/07  10:42:14  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.10.2.1  1996/02/20  17:09:59  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.15  1996/03/08  00:42:53  lol
 * CSCdi50738:  no timeouts in T+
 *         Implement T+ timeouts for a customer who just won't wait. This
 *         code becomes obsolete in California.
 *
 * Revision 3.14  1996/03/07  02:15:46  irfan
 * CSCdi50921:  garbled IP address in debug output
 * avoid printing IP address out of an unintialized structure. While at
 * it, name all the mallocs.
 *
 * Revision 3.13  1996/02/09  09:03:13  billw
 * CSCdi43680:  RADIUS doesnt support magic number ip address values
 * also add authstruct->auth_idb, and change auth->tty to auth->auth_tty
 *
 * Revision 3.12  1996/02/07  16:15:07  widmer
 * CSCdi48285:  Remove function name from bad_parser_subcommand
 *
 * Revision 3.11  1996/02/01  06:08:53  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.10  1996/01/16  03:37:39  ahh
 * CSCdi46921:  TCP: remove unused second parameter to tcp_close
 *
 * Revision 3.9  1996/01/06  01:14:37  lol
 * CSCdi46518:  NAS accepts cleartext packets even when a key is set
 *
 * Revision 3.8  1995/12/28  18:28:35  jenny
 * CSCdi46011:  TCP/IP need to do code clean up.
 * This is the first patch of code changes for TCP/IP code cleanup.
 * . removed all references to TUBA_IP and TUBA_CLNS.
 * . renamed open_tuba_socket() to ip_open_socket().
 * . renamed tcp_tuba_open() to tcp_open().
 * . pass addrtype * instead of ipaddrtype for local and foreight addresses
 *   to tcp_open() for feature portability.
 *
 * Revision 3.7  1995/12/17  18:36:48  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.6  1995/12/13  02:59:38  billw
 * CSCdi42536:  aaa_acct not proper subsystem
 * Move some code around, change some calls to reg_invokes, etc.
 *
 * Revision 3.5  1995/12/08  00:54:49  irfan
 * CSCdi45224:  tacacs could use a specified source address
 * allow tacacs (TA, XTA, TA+) to use a specific (up) interface's IP
 * address.
 *
 * Revision 3.4  1995/11/29  00:16:17  lol
 * CSCdi42385:  TACACS+ crashes router in aaa_split_AV
 *
 * Revision 3.3  1995/11/22  06:24:31  irfan
 * CSCdi43911:  clear line slow to work when authenticating via AAA,tacacs+
 * Do set tty in tcb for all cases except accounting. Allows misc tcp
 * blocks to abort after noticing CARDROP on the tty.
 *
 * Revision 3.2  1995/11/17  18:51:53  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:12:07  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.10  1995/11/08  21:24:42  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.9  1995/10/19  21:17:23  che
 * CSCdi36726:  tacacs login causes crash
 * Lock the memory location of the Tacacs server we're trying to contact
 * so it doesn't get freed up while we're waiting for a response.
 *
 * Revision 2.8  1995/09/10  23:37:01  carrel
 * CSCdi40013:  User information needs to be present even when no login
 * occurs
 * aaa_create_user() now initializes the acc_method to be
 * AAA_ACC_METH_NONE.
 *
 * Revision 2.7  1995/08/11  06:18:23  billw
 * CSCdi37886:  AAA TACACS+ TCP connections should not set tcb->tty
 *
 * Revision 2.6  1995/07/06  21:46:07  ahh
 * CSCdi36734:  RCP: cannot stop file transfer on a VTY
 * Consume VTY input during file transfer so the TTY code will
 * notice when someone hits the escpae char.
 *
 * Revision 2.5  1995/06/28  09:30:24  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.4  1995/06/20  07:17:50  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.3  1995/06/15  03:42:35  lol
 * CSCdi35713:  AAA subsystems needs more debugging output
 *              Add
 *                 aaa debug authentication
 *                 aaa debug authorization
 *                 aaa debug accounting
 *
 * Revision 2.2  1995/06/14  06:30:58  billw
 * CSCdi35380:  AAA accounting causes crashes
 * Clean up aaa_dup_user and callers, correct freeing of non-malloced
 * memory, and a couple other things.
 *
 * Revision 2.1  1995/06/07  22:44:24  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#include "master.h"
#include "../ui/common_strings.h"
#include <string.h>
#include "subsys.h"
#include "sys_registry.h"
#include "../h/auth_registry.regh"
#include "../h/cs_registry.regh"
#include "interface_private.h"
#include "address.h"
#include "name.h"
#include "config.h"
#include "parser.h"
#include "ttysrv.h"
#include "aaa.h"
#include "../servers/tacacs_plus.h"
#include "../servers/tacacs.h"
#include "../util/md5.h"
#include "../tcp/tcp.h"
#include "../ui/debug.h"
#include "../os/os_debug_flags.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "parser_defs_tacacs.h"
#include "../if/network.h"
#include "logger.h"
#include "../os/msg_aaa.c"		/* Not a typo, see logger.h */
#include "../util/random.h"
#include "../wan/ppp_auth.h"
#include <ciscolib.h>
#include "../tcp/tcpinternal.h"
#include "../os/techsupport.h"
#include "../ip/ip_registry.h"
#include "../os/free.h"

/*
 * Global variables.
 */
char *tacacs_plus_key;
boolean tacacs_plus_alias_lookup;

/*
 * static variables
 */
static char md5_buffer[PARSEBUF+6]; /* used by crypt routine */

/* error message for key mismatch */
static uchar check_key_str[] = "\nTAC+: Invalid AUTHEN/START packet (check keys)"; 

/*
 * Prototypes for local functions.
 */
static void
tacacs_plus_start_login(boolean *result, userstruct *u, authenstruct *a,
			int action);

static void
tacacs_plus_cont_login(boolean *result, userstruct *u, authenstruct *a);

static void
tacacs_plus_abort_login(userstruct *u, authenstruct *a, char *reason);

static void
tacacs_plus_do_author(boolean *result, userstruct *u, authorstruct *a);

static void
tacacs_plus_do_acct(boolean *result, userstruct *u, acctstruct *a);

static boolean
tacacs_plus_send_receive(tacacs_plus_pkt_hdr *out, tacacs_plus_pkt_hdr **in,
			 addrtype *addr, void *handle, int id);

static void
tacacs_plus_closeconn(addrtype *addr, void **handle);

static servtype *
find_tacacs_server_index(ipaddrtype ipaddr, int *index);

static void
tacacs_plus_get_conn(addrtype *server, void **handle,
		     enum TACACS_PLUS_PAK_TYPES type, int *index);

static boolean
tacacs_plus_openconn(addrtype *addr, void **handle);

static tacacs_plus_pkt_hdr *
create_authen_start(userstruct *u, authenstruct *a, int action, int version);

static tacacs_plus_pkt_hdr *
create_authen_cont(userstruct *u, authenstruct *a, int version);

static tacacs_plus_pkt_hdr *
create_author_req(userstruct *u, authorstruct *a, int version);

static tacacs_plus_pkt_hdr *
create_acct_req(userstruct *u, acctstruct *a, int version);

static void
fill_tacacs_plus_hdr(tacacs_plus_pkt_hdr *tac_pak, uchar version, uchar type,
		     uchar seq_num, uchar encrypt, int session_id, int len);

static boolean
tacacs_plus_hdr_sanity_check(tacacs_plus_pkt_hdr *tac_pak, int version,
			     int session_id, int seq_num, char *type);

static boolean
authen_resp_sanity_check(tacacs_plus_pkt_hdr *tac_pak, int ver,
			 int session_id, int seq_num);

static boolean
author_resp_sanity_check(tacacs_plus_pkt_hdr *tac_pak, int version,
			 int session_id, int seq_num);

static boolean
acct_resp_sanity_check(tacacs_plus_pkt_hdr *tac_pak, int version,
		       int session_id, int seq_num);

static void
compatibility_layer (enum AUTH_ACTIONS action, authenstruct *a, userstruct *u, 
		     addrtype *server);

/*
 * Parse chains for TACACS+ commands
 */
#define	ALTERNATE	NONE
#include "cfg_tacacs_plus.h"
LINK_POINT(cfg_tacacs_plus_cmds, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for tacacs show commands
 */
#define	ALTERNATE	NONE
#include "exec_show_tacacs.h"
LINK_POINT(tacacs_show_commands, ALTERNATE);
#undef	ALTERNATE



/*
 * TACACS+ subsystem initialization.
 */
static void
tacacs_plus_init (subsystype *subsys)
{
    tacacsplus_is_installed = TRUE;			/* In aaa.c */
    tacacs_plus_key = NULL;
    tacacs_plus_alias_lookup = FALSE;
    
    reg_add_start_login(AAA_ACC_METH_TACACSPLUS, tacacs_plus_start_login,
			       "tacacs_plus_start_login");
    reg_add_cont_login(AAA_ACC_METH_TACACSPLUS, tacacs_plus_cont_login,
			       "tacacs_plus_cont_login");
    reg_add_abort_login(AAA_ACC_METH_TACACSPLUS, tacacs_plus_abort_login,
			       "tacacs_plus_abort_login");
    reg_add_do_author(AAA_AUTHOR_METH_TAC_PLUS, tacacs_plus_do_author,
		      "tacacs_plus_do_author");
    reg_add_do_acct(AAA_AUTHOR_METH_TAC_PLUS, tacacs_plus_do_acct,
		    "tacacs_plus_do_acct");
    reg_add_tacacs_closeconn(tacacs_plus_closeconn, "tacacs_plus_closeconn");
    reg_add_tacacs_plus_close_handle(close_handle, "close_handle");
    parser_add_commands(PARSE_ADD_CFG_TACACS, &pname(cfg_tacacs_plus_cmds),
			"tacacs+");
    parser_add_commands(PARSE_ADD_SHOW_CMD, &pname(tacacs_show_commands),
			"tacacs+ show");
}

void
tacacs_plus_command (parseinfo *csb)
{
    if (csb->nvgen) {
	switch (csb->which) {
	case TACACS_KEY:
	    nv_write(tacacs_plus_key != NULL, "%s %s", csb->nv_command,
		     (csb->flags & HIDE_PASSWORDS) ?
		     techsupport_blank_string : tacacs_plus_key);
	    break;
	case TACACS_ALIAS_LOOKUP:
	    nv_write(tacacs_plus_alias_lookup, "%s",
		     csb->nv_command);
	    break;
	default:
	    bad_parser_subcommand(csb, csb->which);
	    break;
	}
	return;
    }

    switch (csb->which) {
    case TACACS_KEY:
	if (tacacs_plus_key) {
	    free(tacacs_plus_key);
	    tacacs_plus_key = NULL;
	}
	if (csb->sense) {
	    tacacs_plus_key = strdup_named(GETOBJ(string,1), "TAC+ 1");
	}
	break;
    case TACACS_ALIAS_LOOKUP:
	tacacs_plus_alias_lookup = csb->sense;
	break;
    default:
	bad_parser_subcommand(csb, csb->which);
	break;
    }
}

/*
 * Encrypt/decrypt routine
 *
 * data ^ MD5{session_id, key, version, seq #, [previous hash]}
 *
 * Since this routine can't block, md5_buffer is kept in static
 * storage instead of on the stack. 
 */
static boolean
crypt (char *key, tacacs_plus_pkt_hdr *tac_pak)
{
    int len, key_len, md5_buffer_len, i;
    char output[MD5_LEN], *ptr;
    MD5_CTX mdcontext;

    len = n2hl(tac_pak->length);
    key_len = strlen(key);
    *((int *)(&md5_buffer[0])) = h2nl(tac_pak->session_id);
    md5_buffer_len = sizeof(int);
    bcopy(key, &md5_buffer[4], key_len);
    md5_buffer_len += key_len;
    md5_buffer[4 + key_len] = tac_pak->version;
    md5_buffer_len++;
    md5_buffer[5 + key_len] = tac_pak->seq_no;
    md5_buffer_len++;
    MD5Init(&mdcontext);
    MD5Update(&mdcontext, md5_buffer, md5_buffer_len);
    MD5Final(&output[0], &mdcontext);
    
    ptr = ((char *)tac_pak) + TACACS_PLUS_HDR_SIZ;

    for (; len > MD5_LEN; len -= MD5_LEN) {
	for (i = 0; i < MD5_LEN; i++) {
	    *ptr++ ^= output[i];
	}
	bcopy(output, &md5_buffer[md5_buffer_len], MD5_LEN);
	MD5Init(&mdcontext);
	MD5Update(&mdcontext, md5_buffer, md5_buffer_len + MD5_LEN);
	MD5Final(&output[0], &mdcontext);
    }
    for (i = 0; i < len; i++) {
	*ptr++ ^= output[i];
    }
	
    tac_pak->encrypted = (char)(tac_pak->encrypted ?
				TAC_PLUS_ENCRYPTED : TAC_PLUS_CLEAR);
    return(TRUE);
}

boolean
tacacs_plus_encrypt (addrtype *server, tacacs_plus_pkt_hdr *tac_pak)
{
    char *key = get_server_key(server);

    if (!key && (tac_pak->encrypted == TAC_PLUS_CLEAR)) {
	return(TRUE); /* pak is clear and we have no key */
    }

    if (key && (tac_pak->encrypted != TAC_PLUS_CLEAR)) {
	if (tacacs_debug) {
	    buginf("\nTAC+: encrypt: pak is already encrypted");
	}
	return(FALSE);
    }
    return(crypt(key, tac_pak));
}
static boolean
tacacs_plus_decrypt (addrtype *server, tacacs_plus_pkt_hdr *tac_pak)
{
    char *key = get_server_key(server);

    if (!key) {
	if (tac_pak->encrypted == TAC_PLUS_CLEAR) {
	    return(TRUE); /* pak is clear and we have no key */
	}

	if (tacacs_debug) {
	    buginf("\nTAC+: decrypt: no key for encrypted pak");
	}
	return(FALSE);
    }

    /* There is a key */
    if (tac_pak->encrypted != TAC_PLUS_ENCRYPTED) {
	if (tacacs_debug) {
	    buginf("\nTAC+: decrypt: pak is unencrypted but we have a key");
	}
	return(FALSE);
    }
    return(crypt(key, tac_pak));
}


/*
 * TACACS+ networking routines
 */

/* find_tacacs_server_index
 *
 * Given an IP address (ipaddr), find the index into the
 * tacacs-host-list (tacacshostQ) and pass back the pointer to this
 * list-element (or NULL if none is found). Also sets the index
 * variable to the postion of the host in the list.
 */
static servtype *
find_tacacs_server_index (ipaddrtype ipaddr, int *index)
{
    servtype *lp;
    
    lp = (servtype *)tacacshostQ.qhead;

    if (tacacs_debug && !lp) {
	buginf("\nTAC+: find_index: no tacacs servers defined");
    }

    for (*index = 1; lp; (*index)++, lp = lp->next) {
      if (ipaddr == lp->addr) {
	break;
      }
    }
    
    return( lp );
}	
    
    
static void
tacacs_plus_get_conn (addrtype *server, void **handle,
		      enum TACACS_PLUS_PAK_TYPES type, int *index)
{
    servtype *lp;
    nametype *host_entry;
    boolean ret,skip;
    int i = 0;
    int server_index = 1;

    if (!server || !index)
	return;		/* should never happen */

    skip = ret = FALSE;
    lp = NULL;

    /* If given a server, and we have permission to do so (via
     * tacacs_plus_alias_lookup), try to use it */
    if (server->type) {	

	if (tacacs_plus_alias_lookup) {
	    if (tacacs_debug) {
		buginf("\nTAC+: lookup %i in DNS %s", 
		       server->ip_addr, 
		       domain_enable ? "servers" : "local cache");
	    }
	    
	    host_entry = reg_invoke_ip_address_lookup(server->addr.ip_address);
	    
	    if (host_entry) {

		/* run through all the aliases for this host */
		for (i=0; host_entry->v.ip.adr[i].type == ADDR_IP; i++) {
		    if (tacacs_debug) {
			buginf("\nTAC+: DNS entry #%d = %i", i,
			       host_entry->v.ip.adr[i].addr.ip_address);
		    }

		    lp = find_tacacs_server_index(
			host_entry->v.ip.adr[i].addr.ip_address,
			&server_index);
		    
		    if (lp) {
			/* copy the DNS alias into server, and try openconn */
			server->addr = lp->addr;
			ret = tacacs_plus_openconn(server, handle);
			if (ret) {
			    /* Successful server open. Set its index
			     * and get out.  This value is the index
			     * of the server we want to use NEXT TIME
			     * we come through the code.  */
			    *index = server_index;
			    if (tacacs_debug) {
				buginf("\nTAC+: Opened %i index=%d", 
				       server->ip_addr, *index);
			    }
			    return;
			}
		    } else {
			if (tacacs_debug) {
			    buginf("\nTAC+: %i is not a tacacs-server host",
				   host_entry->v.ip.adr[i].addr.ip_address);
			}
		    }
		} /* for (i) */
	    }
	} else { /* tacacs_plus_alias_lookup */
	    
	    ret = tacacs_plus_openconn(server, handle);
	    if (ret) {
		/* Successful server open. Set its index
		 * and get out.  This value is the index
		 * of the server we want to use NEXT TIME
		 * we come through the code.  */
		*index = server_index;
		if (tacacs_debug) {
		    buginf("\nTAC+: Opened %i index=%d", 
			   server->ip_addr, *index);
		}
		return;
	    }
	} /* if (tacacs_plus_alias_lookup) */
    } /* if (server->type) */

    /*
     * If we had a server passed in, and we want the search confined
     * to this server, then do not search the list
     */
    if (server->type && tacacs_directed_restricted) {
	ret = FALSE;
	skip = TRUE;
    }
    
    if (!ret && !skip) {
	/* If no server yet, look in tacacs list, using value of index
	 * to choose an appropriate server. 
	 */
	lp = (servtype *)tacacshostQ.qhead;
	if (tacacs_debug) {
	    buginf("\nTAC+: %s",
		   lp ? "Using default tacacs server list." :
		   "no tacacs servers defined");
	}
	
	for (i = 0; ((i < *index) && lp); i++) {
	    lp = lp->next;
	}
	server->type = ADDR_IP;
	server->length = ADDRLEN_IP;
    }

    while (!skip && !ret && lp) {	/* keep trying till we connect */
	(*index)++;
	server->addr = lp->addr;

	mem_lock(lp);
	ret = tacacs_plus_openconn(server, handle);
	if (lp->addr != (ipaddrtype) 0) {
	    /* All free() is doing is decrementing the ref count. */
	    free(lp);
	    lp = lp->next;
	} else {
	    free(lp);
	    lp = NULL;
	}
    }
    if (!ret)
	memset(server, 0, sizeof(addrtype));

    return;
}

/*
 * Same as tcp_open but a non-zero timeout sets the giveuptime value
 * for this tcb only. 
 */
static tcbtype *
tcp_open_timed (
    tt_soc *tty,		/* pointer to associated terminal block */
    ushort fport,		/* foreign port */
    addrtype *fhost,		/* foreign address */
    ushort lport,		/* local port (zero if don't care) */
    addrtype *lhost,		/* local address (NULL if don't care) */
    int *status,		/* error code on failure */
    ipoptiontype *ipopts,	/* ip options to use (source route) */
    int timeout)                /* set per tcb giveuptime value (msecs). */
{
    tcbtype *tcb;

    /* Create the TCB and mark it as an old-style TCB. */
    tcb = tcp_create(tty, status, ipopts);
    if (tcb == NULL)
	return(NULL);

    tcb->giveuptime = timeout;

    tcb->internal_flags &= ~TCB_INT_PRIMITIVES;

    /* Bind the local address if requested. */
    if (lport || lhost) {
	*status = tcp_bind(tcb, lhost, lport);
	if (*status != OK) {
	    tcp_close(tcb);
	    return(NULL);
	}
    }

    /* Connect to the remote host. */
    *status = tcp_connect(tcb, fhost, fport);
    if (*status != OK) {
	tcp_close(tcb);
	return(NULL);
    }
    return(tcb);
}


boolean
open_handle (addrtype *addr, void **handle)
{
    tcbtype *tcb;
    int status;
    addrtype source_addr, *source_addr_ptr;
    int port, timeout;

    switch (addr->type) {
    case ADDR_IP:
	if (tacacs_source_idb && tacacs_source_idb->ip_address &&
	    interface_up(tacacs_source_idb)) {
	    source_addr.ip_addr = tacacs_source_idb->ip_address;
	    source_addr.type = ADDR_IP;
	    source_addr.length = ADDRLEN_IP;
	    source_addr_ptr = &source_addr;
	} else {
	    source_addr_ptr = (addrtype *) NULL;
	    source_addr.ip_addr = 0;
	}

	port = get_server_port(addr);
	timeout = get_server_timeout(addr);

	if (tacacs_debug || tacacs_events_debug) {
	    buginf("\nTAC+: Opening TCP/IP to %i/%d timeout=%d",
		   addr->ip_addr, port, timeout/ONESEC);
	}

	tcb = tcp_open_timed(NULL, port, addr, 0, source_addr_ptr, &status, 
			     NULL, timeout);
	if (!tcb) {
	    if (tacacs_debug || tacacs_events_debug) {
		buginf("\nTAC+: TCP/IP open to %i/%d failed -- %s",
		       addr->ip_addr, port, tcperror2string(status));
	    }
	    return(FALSE);
	}

	if (tacacs_debug || tacacs_events_debug) {
	    if (source_addr.ip_addr) {
		buginf("\nTAC+: Opened TCP/IP handle 0x%x to %i/%d using "
		       "source %i",
		       tcb, addr->ip_addr, port, source_addr.ip_addr);
	    } else {
		buginf("\nTAC+: Opened TCP/IP handle 0x%x to %i/%d", 
		       tcb, addr->ip_addr, port);
	    }
	}

	*handle = tcb;
	return(TRUE);
	break;
    default:
	return(FALSE);
	break;
    }
}


static boolean
tacacs_plus_openconn (addrtype *addr, void **handle)
{
    boolean status;
    servtype *server;

    if (stay_open_server(addr, NULL)) {
	*handle = NULL;
	return(TRUE); /* no-op */
    }
    status = open_handle(addr, handle);
    server = get_server(addr);
    if (server && status) {
	server->opens++;
    }
    return(status);
}


void
close_handle (void **handle)
{
    tcbtype *tcb;
    tcb = *handle;

    if (tacacs_debug || tacacs_events_debug) {
	buginf("\nTAC+: Closing TCP/IP 0x%x connection to %i/%d",
	       tcb, tcb->foreignhost.ip_addr, tcb->foreignport);
    }
    tcp_close(tcb);
    *handle = NULL;
}

static void
tacacs_plus_closeconn (addrtype *addr, void **handle)
{
    servtype *server;

    if (!handle || !*handle || !addr)
	return;

    if (stay_open_server(addr, *handle)) {
	/* no-op */
	*handle = NULL;
	return;
    }
    server = get_server(addr);
    if (server) {
	server->closes++;
    }
    close_handle(handle);
}



static boolean
tacacs_plus_send_receive (tacacs_plus_pkt_hdr *out,
			  tacacs_plus_pkt_hdr **in,
			  addrtype *addr, void *handle, int session_id)
{
    boolean status;

    status = tacacs_plus_proc_send_receive(handle, out, in, addr, 
					   TAC_PLUS_EXPECT_REPLY);
    return(status);
}



/*
 * Routines for manipulating all TACACS+ packets
 * If version is NULL, omit the version check.
 */
static boolean
tacacs_plus_hdr_sanity_check (tacacs_plus_pkt_hdr *tac_pak, int version,
			     int session_id, int seq_num, char *type)
{
    if (version && (tac_pak->version != version)) {
 	if (tacacs_debug) {
 	    buginf("\nTAC+: received bad %s packet: version = %d, "
 		   "expected %d", type, tac_pak->version, version);
	}
 	return(FALSE);
    }

    if (n2hl(tac_pak->session_id) != session_id) {
	if (tacacs_debug) {
	    buginf("\nTAC+: received bad %s packet: session id = %u, "
		   "expected %u", type, n2hl(tac_pak->session_id), session_id);
	}
	return(FALSE);
    }
    if (tac_pak->seq_no != seq_num) {
	if (tacacs_debug) {
	    buginf("\nTAC+: received bad %s packet: sequence = %d, "
		   "expected %d", type, tac_pak->seq_no, seq_num);
	}
	return(FALSE);
    }
    return(TRUE);
}

static void
fill_tacacs_plus_hdr (tacacs_plus_pkt_hdr *tac_pak, uchar version, uchar type,
		     uchar seq_num, uchar encrypt, int session_id, int len)
{
    tac_pak->version = version;
    tac_pak->type = type;
    tac_pak->seq_no = seq_num;
    tac_pak->encrypted = encrypt;
    tac_pak->session_id = h2nl(session_id);
    tac_pak->length = h2nl(len);
}

/*
 * Routines for manipulating authentication packets
 */
static boolean
authen_resp_sanity_check (tacacs_plus_pkt_hdr *tac_pak, int version,
			 int session_id, int seq_num)
{
    authen_response_body *resp;
    int len, server_len;

    if (!tacacs_plus_hdr_sanity_check(tac_pak, version, session_id, 
				      seq_num, "AUTHEN"))
	return(FALSE);

    server_len = n2hl(tac_pak->length);

    if (server_len <= 0) {
	if (tacacs_debug) {
	    buginf("\nTAC+: received bad AUTHEN packet: length = %d",
		   server_len);
	}
	return(FALSE);
    }

    resp = (authen_response_body *)(((char *)tac_pak) + TACACS_PLUS_HDR_SIZ);
    len = AUTHEN_RESPONSE_BODY_SIZ + n2h(resp->msg_len) + n2h(resp->data_len);
    if (server_len != len) {
	if (tacacs_debug) {
	    buginf("\nTAC+: received bad AUTHEN packet: length = %d, "
		   "expected %d", server_len, len);
	}
	return(FALSE);
    }

    return(TRUE);
}

/* Copy authentication response details from reply packet into a */
static void
unload_authen_resp (authenstruct *a, authen_response_body *response)
{
    uint mlen, dlen;

    a->status = response->status;
    a->flags = response->flags;
    a->seq_num++;
    mlen = n2h(response->msg_len);
    dlen = n2h(response->data_len);
    if (mlen) {
	if (a->server_msg)
	    free(a->server_msg);
	a->server_msg = malloc_named(mlen + 1, "TA+SMSG");
	if (!a->server_msg) {
	    a->status = AAA_AUTHEN_STATUS_ERROR;
	} else {
	    sstrncpy(a->server_msg,
		     ((char *)response) + AUTHEN_RESPONSE_BODY_SIZ, mlen + 1);
	}
    }
    if (dlen) {
	a->server_data = malloc_named(dlen,"TA+SDATA");
	if (!a->server_data) {
	    a->status = AAA_AUTHEN_STATUS_ERROR;
	} else {
	    bcopy(((char *)response) + AUTHEN_RESPONSE_BODY_SIZ + mlen,
		  a->server_data, dlen);
	    a->server_data_len = dlen;
	}
    }
}

static tacacs_plus_pkt_hdr *
create_authen_start (userstruct *u, authenstruct *a, int action, int version)
{
    int len, user_len, port_len, addr_len, udata_len;
    tacacs_plus_pkt_hdr *tac_pak;
    authen_start_body *start;
    char *data;

    user_len = strlen(u->user);
    port_len = strlen(u->port);
    addr_len = strlen(u->rem_addr);
    udata_len = a->user_data_len;
    len = AUTHEN_START_BODY_SIZ + user_len + port_len + addr_len + udata_len;
    tac_pak = malloc_named(TACACS_PLUS_HDR_SIZ + len, "TA+TACPAK1");
    if (!tac_pak) {
	return(NULL);
    }
    start = (authen_start_body *)(((char *)tac_pak) + TACACS_PLUS_HDR_SIZ);
    data = ((char *)start) + AUTHEN_START_BODY_SIZ;

    fill_tacacs_plus_hdr(tac_pak, version, TAC_PLUS_AUTHEN, a->seq_num,
			 TAC_PLUS_CLEAR,
			 a->session_id, len);

    start->action = (char)action;
    start->priv_lvl = (char)u->priv_lvl;
    start->authen_type = (char)u->authen_type;
    start->service = (char)u->service;
    start->user_len = (char)user_len;
    start->port_len = (char)port_len;
    start->rem_addr_len = (char)addr_len;
    start->data_len = (char)udata_len;

    if (user_len) {
	bcopy(u->user, data, user_len);
	data += user_len;
    }
    if (port_len) {
	bcopy(u->port, data, port_len);
	data += port_len;
    }
    if (addr_len) {
	bcopy(u->rem_addr, data, addr_len);
	data += addr_len;
    }
    if (udata_len) {
	bcopy(a->user_data, data, udata_len);
	data += udata_len;
    }

    return(tac_pak);
}


static tacacs_plus_pkt_hdr *
create_authen_cont (userstruct *u, authenstruct *a, int version)
{
    uint len, msg_len;
    tacacs_plus_pkt_hdr *tac_pak;
    authen_cont_body *cont;
    char *data;

    msg_len = strlen(a->user_reply);
    len = AUTHEN_CONT_BODY_SIZ + msg_len;
    tac_pak = malloc_named(TACACS_PLUS_HDR_SIZ + len, "TA+TACPAK2");
    if (!tac_pak) {
	return(NULL);
    }
    cont = (authen_cont_body *)(((char *)tac_pak) + TACACS_PLUS_HDR_SIZ);
    data = ((char *)cont) + AUTHEN_CONT_BODY_SIZ;

    fill_tacacs_plus_hdr(tac_pak, version, TAC_PLUS_AUTHEN, a->seq_num,
			 TAC_PLUS_CLEAR,
			 a->session_id, len);

    cont->msg_len = h2n((ushort)msg_len);
    cont->data_len = 0;
    cont->flags = 0;

    if (msg_len) {
	bcopy(a->user_reply, data, msg_len);
	data += msg_len;
    }

    return(tac_pak);
}

/*
 * Routines for manipulating authorization packets
 */
static tacacs_plus_pkt_hdr *
create_author_req (userstruct *u, authorstruct *a, int version)
{
    int len, user_len, port_len, addr_len, arg_len, arg_cnt, i;
    tacacs_plus_pkt_hdr *tac_pak;
    author_request_body *request;
    char *data;

    if (u) {
	user_len = strlen(u->user);
	port_len = strlen(u->port);
	addr_len = strlen(u->rem_addr);
    } else {
	user_len = 0;
	port_len = 0;
	addr_len = 0;
    }
    for (arg_cnt = 0; a->out_args[arg_cnt]; arg_cnt++);
    len = AUTHOR_REQUEST_BODY_SIZ + user_len + port_len + addr_len + arg_cnt;
    for (i = 0; i < arg_cnt; i++)
	len += strlen(a->out_args[i]);
    tac_pak = malloc_named(TACACS_PLUS_HDR_SIZ + len, "TA+TACPAK3");
    if (!tac_pak) {
	return(NULL);
    }
    request = (author_request_body *)(((char *)tac_pak) + TACACS_PLUS_HDR_SIZ);
    data = ((char *)request) + AUTHOR_REQUEST_BODY_SIZ;
    fill_tacacs_plus_hdr(tac_pak, version, TAC_PLUS_AUTHOR, a->seq_num,
			 TAC_PLUS_CLEAR,
			 a->session_id, len);

    if (u) {
	request->authen_method = (uchar)u->acc_method;
	request->priv_lvl = (uchar)u->priv_lvl;
	request->authen_type = (uchar)u->authen_type;
	request->service = (uchar)u->service;
    } else {
	request->authen_method = (uchar)AAA_ACC_METH_NOT_SET;
	request->priv_lvl = (uchar)(a->auth_tty ?
				    CUR_PRIV(a->auth_tty->privilege) : 0);
	request->authen_type = (uchar)AAA_AUTHEN_TYPE_NONE;
	request->service = (uchar)AAA_SVC_NONE;
    }
    request->user_len = (uchar)user_len;
    request->port_len = (uchar)port_len;
    request->rem_addr_len = (uchar)addr_len;
    request->arg_cnt = (uchar)arg_cnt;

    for(i = 0; i < arg_cnt; i++) {
	data[i] = (uchar)strlen(a->out_args[i]);
    }
    data += arg_cnt;
    if (user_len) {
	if (aaa_author_debug) {
	    buginf("\nAAA/AUTHOR/TAC+: (%u): user=%s", 
		   a->session_id, u->user);
	}
	bcopy(u->user, data, user_len);
	data += user_len;
    } else {
	if (aaa_author_debug) {
	    buginf("\n%%AAA/AUTHOR/TAC+: (%u): no username in request",
		   a->session_id);
	}
    }
    if (port_len) {
	bcopy(u->port, data, port_len);
	data += port_len;
    }
    if (addr_len) {
	bcopy(u->rem_addr, data, addr_len);
	data += addr_len;
    }
    for(i = 0; i < arg_cnt; i++) {
	arg_len = strlen(a->out_args[i]);
	bcopy(a->out_args[i], data, arg_len);
	data += arg_len;
	if (aaa_author_debug) {
	    buginf("\nAAA/AUTHOR/TAC+: (%u): send AV %s", 
		   a->session_id, a->out_args[i]);
	}
    }

    return(tac_pak);
}


static boolean
author_resp_sanity_check (tacacs_plus_pkt_hdr *tac_pak, int version,
			 int session_id, int seq_num)
{
    author_response_body *resp;
    char *data;
    int i, len, server_len;
    uint mlen, dlen, arg_cnt;

    if (!tacacs_plus_hdr_sanity_check(tac_pak, version, session_id, 
				      seq_num, "AUTHOR"))
	return(FALSE);

    server_len = n2hl(tac_pak->length);
    resp = (author_response_body *)(((char *)tac_pak) + TACACS_PLUS_HDR_SIZ);
    data = (char *)resp + AUTHOR_RESPONSE_BODY_SIZ;
    arg_cnt = (uint)resp->arg_cnt;
    mlen = (uint)n2h(resp->msg_len);
    dlen = (uint)n2h(resp->data_len);
    len = AUTHOR_RESPONSE_BODY_SIZ + arg_cnt + mlen + dlen;
    /*
     * Check that arg_cnt is sane before reading off the deep end.
     * This is not an accurate count yet, but it will keep us from reading
     * beyond the packet.
     * At this point len is everything but the args themselves.
     */
    if (len > server_len) {
	if (tacacs_debug) {
	    buginf("\nTAC+: received bad AUTHOR packet: arg_cnt = %d",
		   arg_cnt);
	}
	return(FALSE);
    }
    /* Now add in actual lengths of the args. */
    for (i = 0; i < arg_cnt; i++) {
	len += data[i];
    }
    if (server_len != len) {
	if (tacacs_debug) {
	    buginf("\nTAC+: received bad AUTHOR packet: length = %d, "
		   "expected %d", server_len, len);
	}
	return(FALSE);
    }

    return(TRUE);
}

static void
unload_author_resp (authorstruct *a, author_response_body *resp)
{
    char *data, *data2;
    int i, in_cnt;
    uint msg_len, data_len;

    a->status = resp->status;
    in_cnt = resp->arg_cnt;
    msg_len = (uint)n2h(resp->msg_len);
    data_len = (uint)n2h(resp->data_len);
    data = ((char *)resp) + AUTHOR_RESPONSE_BODY_SIZ;
    data2 = data + in_cnt;	/* move past arg lengths. */
    if (msg_len) {
	if (a->server_msg)
	    free(a->server_msg);
	a->server_msg = malloc_named(msg_len + 1, "TA+SMSG2");
	if (a->server_msg)
	    sstrncpy(a->server_msg, data2, msg_len + 1);
	data2 += msg_len;
    }

    switch (a->status) {
    case TAC_PLUS_AUTHOR_STATUS_PASS_REPL:
	/* 
	 * A replace which returns no AV pairs is most probably a
	 * mistake.  At best, its meaning is undefined. 
	 */
	if (in_cnt <= 0) {
	    if (tacacs_debug) {
		buginf("\nTAC+: got AUTHOR replace containing %d AV pairs"
		       " -- declaring an error", in_cnt);
	    }
	    a->status = TAC_PLUS_AUTHOR_STATUS_ERROR;
	    break;
	}
	/* FALLTHRU */
    case TAC_PLUS_AUTHOR_STATUS_PASS_ADD:
	if (in_cnt) {
	    a->in_args = malloc_named((in_cnt + 1) * sizeof(char *), "TA+INARGS1");
	    if (a->in_args) {
		data2 += data_len;	/* point at first arg, ignore data */
		for (i = 0; i < in_cnt; i++) {
		    a->in_args[i] = malloc_named(data[i] + 1, "TA+INARGS2");
		    if (a->in_args[i])
			sstrncpy(a->in_args[i], data2, data[i] + 1);
		    data2 += data[i];
		}
		a->in_args[in_cnt] = NULL;
	    }
	}
	break;
    case TAC_PLUS_AUTHOR_STATUS_ERROR:
	if (data_len) {
	    a->server_admin_msg = malloc_named(data_len + 1, "TA+SADMINMSG");
	    if (a->server_admin_msg) {
		sstrncpy(a->server_admin_msg, data2, data_len + 1);
	    }
	}
	break;
    case TAC_PLUS_AUTHOR_STATUS_FAIL:
	break;
    }
    a->seq_num++;
    return;
}
/*
 * tacplus_attr_convert
 * Convert a list of protocol independent AVs to tacacs+ style text AVs.
 */
static void
tacplus_attr_convert (uchar *start, uchar *end, char ***output, int *count)
{
    char c_attr[20];
    char c_val[30];
    char *w;
    struct acct_attr_ *attr;

    while (start < end) {
	attr = (struct acct_attr_ *) start;
	sprintf(c_attr, "%s", aaa_attr_names[attr->attr_ident]);
	w = c_val;
	switch (attr->value_fmt) {
	  case AAA_TYPE_BYTE:
	    sprintf(w, "%d", GETLONG(attr->attr_value));
	    break;
	  case AAA_TYPE_INT:
	    sprintf(w, "%u", GETLONG(attr->attr_value));
	    break;
	  case AAA_TYPE_STR:
	    w = attr->attr_value;
	    break;
	  case AAA_TYPE_IP:
	    sprintf(w, "%i", GETLONG(attr->attr_value));
	    break;
	  case AAA_TYPE_IPX:
	    sprintf(w, "%x", attr->attr_value);
	    break;
	  case AAA_TYPE_ETHER:
	    sprintf(w, "%e", attr->attr_value);
	    break;
	  default:
	    w = "BADFMT";
	    break;
	}
	if (!aaa_add_AV(output, count, c_attr, w, AAA_AV_MAND))
	    errmsg(&msgsym(BADSTR, AAAA), "add_AV failure");
	start = (start + sizeof(struct acct_attr_) + attr->value_length)-1;
    }
}

static tacacs_plus_pkt_hdr *
create_acct_req (userstruct *u, acctstruct *a, int version)
{
    int len, user_len, port_len, addr_len, arg_len, arg_cnt, i;
    tacacs_plus_pkt_hdr *tac_pak;
    acct_request_body *request;
    char *data;
    char *avs[AAA_ACCT_NAVS+1];
    char **avptr = avs;
    int avcount = AAA_ACCT_NAVS;
    uchar *start, *end;

    memset(avs, 0, sizeof avs);

    start = a->info->acct_data;
    /*
     * For start packets, make sure we only send as much as was
     * indicated by the sending process
     */
    if (a->flags & ACCT_FLAG_START) {
	end = a->info->acct_end_write ? a->info->acct_end_write :
	    a->info->acct_dptr;
    } else {
	end = a->info->acct_dptr;
    }

    /*
     * if avs[] is not big enough for all the records, 
     * tacplus_attr_convert() will dynamically allocate a large 
     * enough buffer to store the records
     */
    tacplus_attr_convert(start, end, &avptr, &avcount);
    if (u) {
	user_len = strlen(u->user);
	port_len = strlen(u->port);
	addr_len = strlen(u->rem_addr);
    } else {
	user_len = 0;
	port_len = 0;
	addr_len = 0;
    }
    for (arg_cnt = 0; avptr[arg_cnt]; arg_cnt++);
    len = ACCT_REQUEST_BODY_SIZ + user_len + port_len + addr_len + arg_cnt;
    for (i = 0; i < arg_cnt; i++)
	len += strlen(avptr[i]);
    tac_pak = malloc_named(TACACS_PLUS_HDR_SIZ + len, "TA+TACPAK4");
    if (!tac_pak) {
	if (mem_lockable(avptr)) {
	    free(avptr);
	}
	return(NULL);
    }
    request = (acct_request_body *)(((char *)tac_pak) + TACACS_PLUS_HDR_SIZ);
    data = ((char *)request) + ACCT_REQUEST_BODY_SIZ;
    fill_tacacs_plus_hdr(tac_pak, version, TAC_PLUS_ACCT, a->seq_num,
			 TAC_PLUS_CLEAR,
			 a->session_id, len);

    request->flags = a->flags;
    if (u) {
	request->authen_method = (uchar)u->acc_method;
	request->priv_lvl = (uchar)u->priv_lvl;
	request->authen_type = (uchar)u->authen_type;
	request->service = (uchar)u->service;
    } else {
	request->authen_method = (uchar)AAA_ACC_METH_NOT_SET;
	request->priv_lvl = (uchar)(a->tty ? CUR_PRIV(a->tty->privilege) : 0);
	request->authen_type = (uchar)AAA_AUTHEN_TYPE_NONE;
	request->service = (uchar)AAA_SVC_NONE;
    }
    request->user_len = (uchar)user_len;
    request->port_len = (uchar)port_len;
    request->rem_addr_len = (uchar)addr_len;
    request->arg_cnt = (uchar)arg_cnt;

    for(i = 0; i < arg_cnt; i++) {
	data[i] = (uchar)strlen(avptr[i]);
    }
    data += arg_cnt;
    if (user_len) {
	bcopy(u->user, data, user_len);
	data += user_len;
    }
    if (port_len) {
	bcopy(u->port, data, port_len);
	data += port_len;
    }
    if (addr_len) {
	bcopy(u->rem_addr, data, addr_len);
	data += addr_len;
    }
    for(i = 0; i < arg_cnt; i++) {
	arg_len = strlen(avptr[i]);
	bcopy(avptr[i], data, arg_len);
	free(avptr[i]);
	data += arg_len;
    }
    if (mem_lockable(avptr)) {
	free(avptr);
    }
    return(tac_pak);
}


static boolean
acct_resp_sanity_check (tacacs_plus_pkt_hdr *tac_pak, int version,
		       int session_id, int seq_num)
{
    acct_response_body *resp;
    int len, server_len;
    uint mlen, dlen;

    if (!tacacs_plus_hdr_sanity_check(tac_pak, version, session_id, 
				      seq_num, "ACCT"))
	return(FALSE);

    server_len = n2hl(tac_pak->length);
    resp = (acct_response_body *)(((char *)tac_pak) + TACACS_PLUS_HDR_SIZ);
    mlen = (uint)n2h(resp->msg_len);
    dlen = (uint)n2h(resp->data_len);
    len = ACCT_RESPONSE_BODY_SIZ + mlen + dlen;

    if (server_len != len) {
	if (tacacs_debug) {
	    buginf("\nTAC+: received bad ACCT packet: length = %d, "
		   "expected %d", server_len, len);
	}
	return(FALSE);
    }

    return(TRUE);
}

static void
author_debug_response (tacacs_plus_pkt_hdr *in_tac_pak)
{
    author_response_body *response = 
	(author_response_body *)(((char *)in_tac_pak) +
				 TACACS_PLUS_HDR_SIZ);
    char *status = "UNKNOWN";

    if (!tacacs_debug && !aaa_authen_debug) {
	return;
    }

    switch(response->status) {
    case TAC_PLUS_AUTHOR_STATUS_PASS_ADD:
	status = "PASS_ADD";
	break;
    case TAC_PLUS_AUTHOR_STATUS_PASS_REPL:
	status = "PASS_REPL";
	break;
    case TAC_PLUS_AUTHOR_STATUS_FAIL:
	status = "FAIL";
	break;
    case TAC_PLUS_AUTHOR_STATUS_ERROR:
	status = "ERROR";
	break;
    }

    buginf("\nTAC+: (%u): received author response status = %s",
	   in_tac_pak->session_id, status);
}


static void
authen_debug_response (tacacs_plus_pkt_hdr *in_tac_pak)
{
    authen_response_body *response = 
	(authen_response_body *)(((char *)in_tac_pak) +
				 TACACS_PLUS_HDR_SIZ);
    char *status = "UNKNOWN";

    if (!tacacs_debug && !aaa_authen_debug) {
	return;
    }

    switch(response->status) {
    case TAC_PLUS_STATUS_PASS:
	status = "PASS";
	break;
    case TAC_PLUS_STATUS_FAIL:
	status = "FAIL";
	break;
    case TAC_PLUS_STATUS_GETDATA:
	status = "GETDATA";
	break;
    case TAC_PLUS_STATUS_GETUSER:
	status = "GETUSER";
	break;
    case TAC_PLUS_STATUS_GETPASS:
	status = "GETPASS";
	break;
    case TAC_PLUS_STATUS_RESTART:
	status = "RESTART";
	break;
    case TAC_PLUS_STATUS_ERROR:
	status = "ERROR";
	break;
    case TAC_PLUS_STATUS_FOLLOW:
	status = "FOLLOW";
	break;
    }

    buginf("\nTAC+: ver=%d id=%u received AUTHEN status = %s",
	   in_tac_pak->version, in_tac_pak->session_id, status);
}


/*
 * TACACS+ core routines
 */

/* 
 * Decide which version (minor version 0 or minor version 1) this
 * request should be.
 */
static uchar
choose_version (enum AUTH_ACTIONS action, userstruct *u)
{
    switch(action) {
    case AAA_AUTHEN_ACT_LOGIN:
	switch(u->authen_type) {
	case AAA_AUTHEN_TYPE_PAP:
	case AAA_AUTHEN_TYPE_CHAP:
	case AAA_AUTHEN_TYPE_ARAP:
	    return(TAC_PLUS_VER);

	case AAA_AUTHEN_TYPE_ASCII:
	    /* includes service=enable */
	    return(TAC_PLUS_VER_0);

	default:
	    buginf("\n%%TAC+: illegal type=%d for login", u->authen_type);
	    return(TAC_PLUS_VER_0);
	}


    case AAA_AUTHEN_ACT_SENDAUTH:
	switch(u->authen_type) {
	case AAA_AUTHEN_TYPE_PAP:
	case AAA_AUTHEN_TYPE_CHAP:
	    return(TAC_PLUS_VER);

	default:
	    buginf("\n%%TAC+: illegal type=%d for sendauth", u->authen_type);
	    return(TAC_PLUS_VER);
	}

    case AAA_AUTHEN_ACT_CHPASS:
	/* not supported. *FALLTHRU* */
    default:
	buginf("\n%%TAC+: illegal action=%d/type=%d", action, u->authen_type);	
	return(TAC_PLUS_VER);
    }

    return(TAC_PLUS_VER_0);
}

/*
 * tacacs_plus_start_login
 * START a AAA dialog with the tacacs+ server.  This starts up the
 * conversation and sees what the server wants us to supply, essentially
 */
static void
tacacs_plus_start_login (boolean *result, userstruct *u,
			 authenstruct *a, int action)
{
    tacacs_plus_pkt_hdr *out_tac_pak, *in_tac_pak = NULL;
    authen_response_body *response = NULL;
    addrtype server;
    int server_index = 0;
    uchar version;

    *result = FALSE;		/* Default is failure */

    setstring(&a->server_msg, NULL);
    setstring(&a->server_data, NULL);
    a->server_data_len = 0;

    memset(&server, 0, sizeof(addrtype));
    /* If username contains a valid tacacs server name, then truncate
     * username and try that server first.  Otherwise use one in
     * userstruct if it exists.
     */
    server.ip_addr = tacacs_parse_server(u->user);
    if (server.ip_addr) {
	server.type = ADDR_IP;
	server.length = ADDRLEN_IP;
    } else if (u->server.type) {
	bcopy(&u->server, &server, sizeof(addrtype));
    }

    version = choose_version(action, u);

    out_tac_pak = create_authen_start(u, a, action, version);
    if (!out_tac_pak) {
	if (tacacs_debug) {
	    buginf("\nTAC+: Error creating authentication start pkt.");
	}
	return;
    }

    if (tacacs_debug || aaa_authen_debug) {
	buginf("\nTAC+: send AUTHEN/START packet ver=%d id=%u",
	       out_tac_pak->version, out_tac_pak->session_id);
    }

    /*
     * Send packet and get back response.
     */
    tacacs_plus_get_conn(&server, &a->connection_handle,
			 TAC_PLUS_AUTHEN, &server_index);
    while (server.type) {
	if (!tacacs_plus_send_receive(out_tac_pak, &in_tac_pak, &server,
				      a->connection_handle, a->session_id)) {
	    if (in_tac_pak) {
		free(in_tac_pak);
		in_tac_pak = NULL;		/* mark as if invalid */
	    }
	}

	if (in_tac_pak) {
	    if (!tacacs_plus_decrypt(&server, in_tac_pak)) {
		if (tacacs_debug) {
		    buginf("\nTAC+: Unable to decrypt data from server.");
		}
		if (in_tac_pak) {
		    free(in_tac_pak);
		    in_tac_pak = NULL;		/* mark as if invalid */
		}
	    } else {
		/* 
		 * The 0 for the 2nd. arg means don't check the
		 * version, since it may be down-rev, and we want to
		 * detect and deal with this later. 
		 */
		if (!authen_resp_sanity_check(in_tac_pak, 0,
					      a->session_id, a->seq_num+1)) {
		    if (tacacs_debug) {
			buginf(check_key_str);
		    }
		    if (in_tac_pak) {
			free(in_tac_pak);
			in_tac_pak = NULL;	/* mark as if invalid */
		    }
		}
	    }
	}

	if (!in_tac_pak ||
	    (TAC_PLUS_GET_MINOR_PAK_VER(out_tac_pak) == TAC_PLUS_MINOR_VER_1 &&
	     TAC_PLUS_GET_MINOR_PAK_VER(in_tac_pak) == TAC_PLUS_MINOR_VER_0 &&
	     in_tac_pak->seq_no == 2)) {

	    if (in_tac_pak) {
		/*
		 * We sent version 1 and received version 0, so try
		 * down-rev compatibility mode to set a->status.  If we
		 * get a pass or a fail, we're done. Anything else will
		 * cause the original reply to generate a version mismatch
		 * error. 
		 */
		
		response = (authen_response_body *)(((char *)in_tac_pak) +
						    TACACS_PLUS_HDR_SIZ);
		authen_debug_response(in_tac_pak);
	    }
	    
	    compatibility_layer(action, a, u, &server);

	    if (a->status == AAA_AUTHEN_STATUS_PASS ||
		a->status == AAA_AUTHEN_STATUS_FAIL) {

		/* we have an answer */
		if (out_tac_pak) {
		    free(out_tac_pak);
		    out_tac_pak = NULL;
		}
		if (in_tac_pak) {
		    free(in_tac_pak);
		    in_tac_pak = NULL;
		}
		/* 
		 * Compatibility calls never involve CONTINUEs, so
		 * we're done with this connection here. 
		 */
		tacacs_plus_closeconn(&server, &a->connection_handle);

		bcopy(&server, &u->server, sizeof(addrtype));
		*result = TRUE;
		return;
	    }
	}

	if (in_tac_pak) {

	    if (in_tac_pak->version != out_tac_pak->version) {
		if (tacacs_debug) {
		    buginf("\nTAC+: version mismatch out=%d in=%d",
			   out_tac_pak->version, in_tac_pak->version);
		}
	    }

	    response = (authen_response_body *)(((char *)in_tac_pak) +
						TACACS_PLUS_HDR_SIZ);
	    if (response) {
		authen_debug_response(in_tac_pak);
	    }

	    if (response && 
		(in_tac_pak->version == out_tac_pak->version) &&
		(response->status != TAC_PLUS_STATUS_FOLLOW)  &&
		(response->status != TAC_PLUS_STATUS_ERROR)) {
		break;	/* We got an answer, get out of loop. */
	    }
	}
	tacacs_plus_closeconn(&server, &a->connection_handle);
	memset(&server, 0, sizeof(addrtype));
	tacacs_plus_get_conn(&server, &a->connection_handle,
			     TAC_PLUS_AUTHEN, &server_index);
    } /* while */

    free(out_tac_pak);
    out_tac_pak = NULL;
    if (!server.type || !in_tac_pak || !response) {
	/* Couldn't find a server. */
	a->status = AAA_AUTHEN_STATUS_ERROR;
	tacacs_plus_closeconn(&server, &a->connection_handle);
	if (in_tac_pak)
	    free(in_tac_pak);
	return;
    }
    bcopy(&server, &u->server, sizeof(addrtype));
    unload_authen_resp(a, response);	/* This increments seq_no */
    free(in_tac_pak);
    *result = TRUE;
    return;
}

/* Enough padding to contain an ip address as text */
#define ADDR_PAD_SIZE 17
/*
 * tacacs_plus_cont_login
 * Continue a login, after we've already received and processed some data.
 * This covers arbitrary dialogs in conjunction with the code in AAA.
 */
static void
tacacs_plus_cont_login (boolean *result, userstruct *u, authenstruct *a)
{
    tacacs_plus_pkt_hdr *out_tac_pak, *in_tac_pak;
    authen_response_body *response;
    servtype *lp;
    uchar version;
    addrtype server;
    char *atloc;
    char *switch_msg = "Switching to directed server @ ";
    char *buf;

    *result = FALSE;

    setstring(&a->server_msg, NULL);
    setstring(&a->server_data, NULL);
    a->server_data_len = 0;

    if (tacacs_directed_request) {
	atloc = strchr(u->user,'@');
	if (atloc) {
	    /* Note: if tacacs_parse_server successfully parses a
	     * server name and manages to translate it via DNS, then
	     * it will replace the '@' sign with a NULL
	     */
	    server.ip_addr = tacacs_parse_server(u->user);
	    if (server.ip_addr) {
		*atloc = '@'; /* replace the NULL with the original @ char */
		buf = malloc_named(strlen(switch_msg) + ADDR_PAD_SIZE,
				   "TAC cont");
		if (!buf) {
		    return;
		}
		sprintf(buf, "%s%i", switch_msg, server.ip_addr);
		tacacs_plus_abort_login(u, a, buf);
		free(buf);
		tacacs_plus_closeconn(&u->server, &a->connection_handle);
		
		a->seq_num = 1;		/* start all over */
		aaa_randomize_id(&a->session_id);
		
		tacacs_plus_start_login(result, u, a, a->action);
		return;
	    }
	}
    }
    
    version = choose_version(a->action, u);

    out_tac_pak = create_authen_cont(u, a, version);
    if (!out_tac_pak) {
	if (tacacs_debug) {
	    buginf("\nTAC+: Error creating continue packet.");
	}
	a->status = AAA_AUTHEN_STATUS_ERROR;
	return;
    }

    if (tacacs_debug || aaa_authen_debug) {
	buginf("\nTAC+: send AUTHEN/CONT packet id=%u",
	       out_tac_pak->session_id);
    }

    if (!tacacs_plus_send_receive(out_tac_pak, &in_tac_pak, &u->server,
				  a->connection_handle, a->session_id)) {
	if (tacacs_debug) {
	    buginf("\nTAC+: Error sending continue packet.");
	}
	free(out_tac_pak);
	a->status = AAA_AUTHEN_STATUS_ERROR;
	tacacs_plus_closeconn(&u->server, &a->connection_handle);
	return;
    }
    free(out_tac_pak);
    out_tac_pak = NULL;
    if (!tacacs_plus_decrypt(&u->server, in_tac_pak)) {
	if (tacacs_debug) {
	    buginf("\nTAC+: Unable to decrypt data from server.");
	}
	a->status = AAA_AUTHEN_STATUS_ERROR;
	free(in_tac_pak);
	tacacs_plus_closeconn(&u->server, &a->connection_handle);
	return;
    }
    response = (authen_response_body *)(((char *)in_tac_pak) +
					TACACS_PLUS_HDR_SIZ);
    if (!authen_resp_sanity_check(in_tac_pak, TAC_PLUS_VER_0, a->session_id,
				  a->seq_num+1)) {
	if (tacacs_debug) {
	    buginf("\nTAC+: received corrupt data from server.");
	}
	a->status = AAA_AUTHEN_STATUS_ERROR;
	free(in_tac_pak);
	tacacs_plus_closeconn(&u->server, &a->connection_handle);
	return;
    }

    authen_debug_response(in_tac_pak);

    if (response->status == TAC_PLUS_STATUS_FOLLOW) {
	free(in_tac_pak);
	tacacs_plus_closeconn(&u->server, &a->connection_handle);
	a->seq_num = 1;		/* start all over */
	for (lp = (servtype *)tacacshostQ.qhead; lp; lp = lp->next) {
	    if (u->server.ip_addr == lp->addr)
		break;
	}
	if (lp) {
	    if (lp->next) {
		u->server.ip_addr = lp->next->addr;
	    } else {
		/* have tried last one in list */
		if (tacacs_debug) {
		    buginf("\nTAC+: no more servers to try");
		}
		a->status = AAA_AUTHEN_STATUS_ERROR;
		return;
	    }
	} else {
	    /* if old addres isn't in list anymore, then list is changing */
	    u->server.ip_addr = ((servtype *)tacacshostQ.qhead)->addr;
	}
	tacacs_plus_start_login(result, u, a, a->action);
	return;
    }
    unload_authen_resp(a, response);	/* This increments seq_no */
    free(in_tac_pak);
    *result = TRUE;
}

/*
 * tacacs_plus_abort_login
 * Abort a login in the middle.  Clean up any intermediate data structures
 * that we may have created.
 */
static void
tacacs_plus_abort_login (userstruct *u, authenstruct *a, char *reason)
{
    int len, client_data_len;
    tacacs_plus_pkt_hdr *tac_pak, *in = NULL;
    authen_cont_body *cont;
    char *data;
    servtype *server;

    client_data_len = strlen(reason);	/* Do not add in NULL */
    len = AUTHEN_CONT_BODY_SIZ + client_data_len;
    tac_pak = malloc_named(TACACS_PLUS_HDR_SIZ + len, "TA+TACPAK5");
    if (!tac_pak) {
	return;
    }
    cont = (authen_cont_body *)(((char *)tac_pak) + TACACS_PLUS_HDR_SIZ);
    data = ((char *)cont) + AUTHEN_CONT_BODY_SIZ;

    tac_pak->version = TAC_PLUS_VER;
    tac_pak->type = TAC_PLUS_AUTHEN;
    tac_pak->seq_no = a->seq_num;
    tac_pak->encrypted = TAC_PLUS_CLEAR;
    tac_pak->session_id = h2nl(a->session_id);
    tac_pak->length = h2nl(len);

    cont->msg_len = 0;
    cont->data_len = (char)client_data_len;
    cont->flags = TAC_PLUS_CONT_FLAG_ABORT;

    bcopy(reason, data, client_data_len);

    if (tacacs_debug || aaa_authen_debug) {
	buginf("\nTAC+: send abort reason=%s", reason);
    }

    server = get_server(&u->server);
    if (server) {
	server->aborts++;
    }

    /* Don't expect a reply to this */
    (void) tacacs_plus_proc_send_receive(a->connection_handle,
					 tac_pak, &in, 
					 &u->server,
					 TAC_PLUS_EXPECT_NO_REPLY);
    free(tac_pak);
}

/*
 * tacacs_plus_do_author
 * Do authorization.  Well, at least the network parts of it.  Load info
 * into the right place for AAA to interpret it.
 */

static void tacacs_plus_do_author (boolean *result, userstruct *u,
				   authorstruct *a)
{
    int server_index = 0;
    addrtype server;
    tacacs_plus_pkt_hdr *in_tac_pak = NULL, *out_tac_pak;
    author_response_body *response = NULL;
    void *connection_handle;

    *result = FALSE;
    setstring(&a->server_msg, NULL);
    setstring(&a->server_admin_msg, NULL);

    out_tac_pak = create_author_req(u, a, TAC_PLUS_VER_0);
    if (!out_tac_pak) {
	if (tacacs_debug) {
	    buginf("\nTAC+: Error creating Authorization request packet.");
	}
	return;
    }

    if (u && u->server.type)
	bcopy(&u->server, &server, sizeof(addrtype));
    else
	memset(&server, 0, sizeof(addrtype));
    tacacs_plus_get_conn(&server, &connection_handle,
			 TAC_PLUS_AUTHOR, &server_index);
    while(server.type) {
	if (!tacacs_plus_send_receive(out_tac_pak, &in_tac_pak, &server,
				      connection_handle, a->session_id)) {
	    if (in_tac_pak) {
		free(in_tac_pak);
		in_tac_pak = NULL;
	    }
	}
	if (in_tac_pak) {
	    if (!tacacs_plus_decrypt(&server, in_tac_pak)) {
		if (tacacs_debug) {
		    buginf("\nTAC+: Unable to decrypt data from server.");
		}
		free(in_tac_pak);
		in_tac_pak = NULL;		/* mark as if invalid */
	    }
	}
	if (in_tac_pak &&
	    !author_resp_sanity_check(in_tac_pak, TAC_PLUS_VER_0,
				      a->session_id, a->seq_num+1)) {
	    if (tacacs_debug) {
		buginf(check_key_str);
	    }
	    free(in_tac_pak);
	    in_tac_pak = NULL;	/* mark as if invalid */
	}
	if (in_tac_pak) {
	    response = (author_response_body *)(((char *)in_tac_pak) +
						TACACS_PLUS_HDR_SIZ);
	    if (response) {
		author_debug_response(in_tac_pak);
	    }

	    if (response &&
		response->status != TAC_PLUS_AUTHOR_STATUS_ERROR) {
		break;	/* We got an answer, get out of loop. */
	    }
	}
	tacacs_plus_closeconn(&server, &connection_handle);
	memset(&server, 0, sizeof(addrtype));
	tacacs_plus_get_conn(&server, &connection_handle,
			     TAC_PLUS_AUTHOR, &server_index);
    }

    free(out_tac_pak);
    out_tac_pak = NULL;
    if (server.type)
	tacacs_plus_closeconn(&server, &connection_handle);
    if (!server.type || !in_tac_pak || !response) {
	/* Couldn't find a server. */
	a->status = AAA_AUTHOR_STATUS_ERROR;
	if (in_tac_pak)
	    free(in_tac_pak);
	return;
    }

    if (u)
	bcopy(&server, &u->server, sizeof(addrtype));
    unload_author_resp(a, response);	 /* This increments seq_no */
    free(in_tac_pak);
    *result = TRUE;
}

static void
tacacs_plus_do_acct (boolean *result, userstruct *u, acctstruct *a)
{
    int server_index = 0;
    addrtype server;
    tacacs_plus_pkt_hdr *in_tac_pak = NULL, *out_tac_pak;
    acct_response_body *response = NULL;
    void *connection_handle;

    if (aaa_acct_suppress_null_username && null(u->user)) {
	if (tacacs_debug || aaa_acct_debug) {
	    buginf("\nTAC+:  Ignoring NULL-USER accounting record");
	}
	*result = TRUE;
	a->status = AAA_ACCT_STATUS_SUCCESS;
	return;
    }

    *result = FALSE;
    setstring(&a->server_msg, NULL);
    setstring(&a->server_admin_msg, NULL);

/* DAVE -- what about multiple packets */

    out_tac_pak = create_acct_req(u, a, TAC_PLUS_VER_0);
    if (!out_tac_pak) {
	if (tacacs_debug) {
	    buginf("\nTAC+: Error creating Accounting request packet.");
	}
	return;
    }

    if (u && u->server.type)
	bcopy(&u->server, &server, sizeof(addrtype));
    else
	memset(&server, 0, sizeof(addrtype));
    tacacs_plus_get_conn(&server, &connection_handle,
			 TAC_PLUS_ACCT, &server_index);
    while(server.type) {
	if (!tacacs_plus_send_receive(out_tac_pak, &in_tac_pak, &server,
				      connection_handle, a->session_id)) {
	    if (in_tac_pak) {
		free(in_tac_pak);
		in_tac_pak = NULL;
	    }
	}
	if (in_tac_pak) {
	    if (!tacacs_plus_decrypt(&server, in_tac_pak)) {
		if (tacacs_debug) {
		    buginf("\nTAC+: Unable to decrypt data from server.");
		}
		if (in_tac_pak) {
		    free(in_tac_pak);
		    in_tac_pak = NULL;		/* mark as if invalid */
		}

		if (!acct_resp_sanity_check(in_tac_pak, TAC_PLUS_VER_0,
					    a->session_id, a->seq_num+1)) {
		    if (tacacs_debug) {
			buginf(check_key_str);
		    }
		    if (in_tac_pak) {
			free(in_tac_pak);
			in_tac_pak = NULL;	/* mark as if invalid */
		    }
		}
	    }
	}
	if (in_tac_pak) {
	    response = (acct_response_body *)(((char *)in_tac_pak) +
						TACACS_PLUS_HDR_SIZ);
	    if (response &&
		response->status != TAC_PLUS_ACCT_STATUS_ERROR) {
		break;	/* We got an answer, get out of loop. */
	    }
	} else {
	    response = NULL;
	}
	tacacs_plus_closeconn(&server, &connection_handle);
	memset(&server, 0, sizeof(addrtype));
	tacacs_plus_get_conn(&server, &connection_handle,
			     TAC_PLUS_ACCT, &server_index);
    }

    free(out_tac_pak);
    out_tac_pak = NULL;
    if (server.type)
	tacacs_plus_closeconn(&server, &connection_handle);
    if (!server.type || !in_tac_pak || !response) {
	/* Couldn't find a server. */
	a->status = AAA_ACCT_STATUS_ERROR;
	if (in_tac_pak)
	    free(in_tac_pak);
	return;
    }

    if (u)
	bcopy(&server, &u->server, sizeof(addrtype));
    a->status = response->status;
    a->seq_num++;
    free(in_tac_pak);
    *result = TRUE;
}

/*
 * Cleanup data structures used after making spoof requests to
 * down-rev servers. 
 */
static void
cleanup_spoof_data (authenstruct *astruct, 
	 userstruct *ustruct,
	 tacacs_plus_pkt_hdr *in,
	 tacacs_plus_pkt_hdr *out)
{
    if (in) {
	free(in);
    }
    if (out) {
	free(out);
    }
    aaa_cleanup_login(ustruct, astruct);
    aaa_free_user(&ustruct);
}

/* 
 * Fake a down-rev outbound pap request.
 *
 * In Version 1, send SENDAUTH/PAP(username) and expect a password in
 * the reply data. A v0 server will return an ERROR.
 *
 * In Version 0, send GETPASS/PAP(username) and expect a password in
 * the reply. 
 */
static void
rev0_outbound_pap (authenstruct *a, userstruct *u, addrtype *server)
{
    userstruct *local_u;
    authenstruct local_a;
    tacacs_plus_pkt_hdr *out_tac_pak = NULL, *in_tac_pak = NULL;
    int server_index = 0;
    authen_response_body *response;
    void **handlep = &a->connection_handle;

    /* 
     * Connection may have closed. Re-open it.
     */
    tacacs_plus_closeconn(server, handlep);

    tacacs_plus_get_conn(server, handlep,
			 TAC_PLUS_AUTHEN, &server_index);

    bzero(&local_a, sizeof(authenstruct));

    if (!aaa_create_user(&local_u, u->user, u->ruser, u->port, u->rem_addr, 
			 AAA_AUTHEN_TYPE_PAP, AAA_SVC_PPP, 
			 PRIV_USER)) {
	return;
    }

    local_a.seq_num = 1;
    aaa_randomize_id(&local_a.session_id);

    if (tacacs_debug) {
	buginf("\nTAC+: rev0 outbound pap for id=%u using id=%u",
	       a->session_id, local_a.session_id);
    }

    out_tac_pak = create_authen_start(local_u, &local_a, 
				      AAA_AUTHEN_ACT_SENDPASS, 
				      TAC_PLUS_VER_0);
    if (!out_tac_pak) {
	if (tacacs_debug) {
	    buginf("\nTAC+: Error creating authentication start pkt.");
	}
	cleanup_spoof_data(&local_a, local_u, in_tac_pak, out_tac_pak);
	return;
    }

    if (!tacacs_plus_send_receive(out_tac_pak, &in_tac_pak, 
				  server, *handlep, local_a.session_id)) {
	cleanup_spoof_data(&local_a, local_u, in_tac_pak, out_tac_pak);
	return;
    }

    if (!in_tac_pak) {
	if (tacacs_debug) {
	    buginf("\nTAC+: No reply from server.");
	}
	cleanup_spoof_data(&local_a, local_u, in_tac_pak, out_tac_pak);
	return;
    }

    if (!tacacs_plus_decrypt(server, in_tac_pak)) {
	if (tacacs_debug) {
	    buginf("\nTAC+: Unable to decrypt data from server.");
	}
	cleanup_spoof_data(&local_a, local_u, in_tac_pak, out_tac_pak);
	return;
    }

    if (!authen_resp_sanity_check(in_tac_pak, TAC_PLUS_VER_0,
				  local_a.session_id, local_a.seq_num+1)) {
	if (tacacs_debug) {
		buginf(check_key_str);
	}
	cleanup_spoof_data(&local_a, local_u, in_tac_pak, out_tac_pak);
	return;
    }

    response = (authen_response_body *)(((char *)in_tac_pak) +
					TACACS_PLUS_HDR_SIZ);

    authen_debug_response(in_tac_pak);

    /* 
     * Propagate a fail upwards. Everything else except a PASS is an
     * error. 
     */

    if (response->status == TAC_PLUS_STATUS_FAIL) {
	if (tacacs_debug) {
	    buginf("\nTAC+: rev0 outbound pap status=FAIL for id=%u", 
		   a->session_id);
	}
	unload_authen_resp(a, response);
	cleanup_spoof_data(&local_a, local_u, in_tac_pak, out_tac_pak);
	return;

    }

    if (response->status != TAC_PLUS_STATUS_PASS) {
	if (tacacs_debug) {
	    buginf("\nTAC+: rev0 outbound pap status=ERROR for id=%u", 
		   a->session_id);
	}
	cleanup_spoof_data(&local_a, local_u, in_tac_pak, out_tac_pak);
	return;
    }

    if (tacacs_debug) {
	buginf("\nTAC+: rev0 outbound pap completed status=PASS for id=%u", 
	       a->session_id);
    }

    unload_authen_resp(a, response);
    cleanup_spoof_data(&local_a, local_u, in_tac_pak, out_tac_pak);
    return;

}

/* 
 * Fake a down-rev inbound chap request. Return TRUE and adjust
 * authenstruct if we get an answer. Return FALSE if the next server
 * should be tried.
 *
 * Version 1 sends LOGIN/CHAP/PPP(username, data=(id,chall,response))
 * and expects a PASS or FAIL reply. A down-rev server will either
 * reply with a down-rev ERROR or a down-rev GETPASS packet.
 *
 * Version 0 sends SENDPASS/CHAP/PPP(username) and expects the chap
 * secret in the reply data. 
 */

static void
rev0_inbound_chap (authenstruct *a, userstruct *u, addrtype *server)
{
    userstruct *local_u = NULL;
    authenstruct local_a;
    tacacs_plus_pkt_hdr *out_tac_pak = NULL, *in_tac_pak = NULL;
    int server_index = 0;
    authen_response_body *response;
    uchar *chall, *chap_resp;
    char digest[MD5_LEN];
    uchar id, *mdp;
    MD5_CTX mdcontext;
    char *secret, *data;
    int len, secret_len;
    int chap_chall_size;
    void **handlep = &a->connection_handle;

    /* 
     * If we're here because we got a GETPASS, we are in the middle of
     * a session which we need to terminate. 
     */
    tacacs_plus_closeconn(server, handlep);
    tacacs_plus_get_conn(server, handlep, TAC_PLUS_AUTHEN, 
			 &server_index);

    /* Create a down-rev CHAP/SENDPASS request */
    bzero(&local_a, sizeof(local_a));
    if (!aaa_create_user(&local_u, u->user, u->ruser, u->port, u->rem_addr, 
			 AAA_AUTHEN_TYPE_CHAP, AAA_SVC_PPP, PRIV_USER)) {
	return;
    }

    local_a.seq_num = 1;
    aaa_randomize_id(&local_a.session_id);
	
    if (tacacs_debug) {
	buginf("\nTAC+: rev0 inbound chap for id=%u using id=%u",
	       a->session_id, local_a.session_id);
    }

    out_tac_pak = create_authen_start(local_u, &local_a, 
				      AAA_AUTHEN_ACT_SENDPASS, 
				      TAC_PLUS_VER_0);

    if (!out_tac_pak) {
	if (tacacs_debug) {
	    buginf("\nTAC+: Error creating authentication start pkt.");
	}
	cleanup_spoof_data(&local_a, local_u, in_tac_pak, out_tac_pak);
	return;
    }

    if (!tacacs_plus_send_receive(out_tac_pak, &in_tac_pak, 
				  server, *handlep, 
				  local_a.session_id)) {
	cleanup_spoof_data(&local_a, local_u, in_tac_pak, out_tac_pak);
	return;
    }

    if (!in_tac_pak) {
	if (tacacs_debug) {
	    buginf("\nTAC+: No reply from server.");
	}
	cleanup_spoof_data(&local_a, local_u, in_tac_pak, out_tac_pak);
	return;
    }

    if (!tacacs_plus_decrypt(server, in_tac_pak)) {
	if (tacacs_debug) {
	    buginf("\nTAC+: Unable to decrypt data from server.");
	}
	cleanup_spoof_data(&local_a, local_u, in_tac_pak, out_tac_pak);
	return;
    }

    if (!authen_resp_sanity_check(in_tac_pak, TAC_PLUS_VER_0,
					 local_a.session_id, 
					 local_a.seq_num+1)) {
	if (tacacs_debug) {
		buginf(check_key_str);
	}
	cleanup_spoof_data(&local_a, local_u, in_tac_pak, out_tac_pak);
	return;
    }

    response = (authen_response_body *)(((char *)in_tac_pak) +
					    TACACS_PLUS_HDR_SIZ);
    authen_debug_response(in_tac_pak);

    /* 
     * Everything except a PASS or FAIL is an ERROR as far as
     * we're concerned. 
     */

    if (response->status == TAC_PLUS_STATUS_FAIL) {
	if (tacacs_debug) {
	    buginf("\nTAC+: rev0 inbound chap FAIL for id=%u", a->session_id);
	}

	unload_authen_resp(a, response);
	cleanup_spoof_data(&local_a, local_u, in_tac_pak, out_tac_pak);
	return;
    }

    if (response->status != TAC_PLUS_STATUS_PASS) {
	if (tacacs_debug) {
	    buginf("\nTAC+: rev0 inbound chap ERROR for id=%u", a->session_id);
	}
	cleanup_spoof_data(&local_a, local_u, in_tac_pak, out_tac_pak);
	return;
    }

    /* 
     * We have received a PASS. This will contain this user's CHAP
     * secret. 
     */
    unload_authen_resp(&local_a, response);
    if (tacacs_debug) {
	buginf("\nTAC+: rev0 inbound chap SENDPASS status=%s for id=%u", 
	       local_a.status == AAA_AUTHEN_STATUS_PASS ? "PASS" : "FAIL",
	       a->session_id);
    }

    /* Just in case we still terminate prematurely */
    local_a.status = TAC_PLUS_STATUS_ERROR; 

    secret_len = local_a.server_data_len;
    if (secret_len <= 0) {
	/* Oops. Should've been given a secret here */
	cleanup_spoof_data(&local_a, local_u, in_tac_pak, out_tac_pak);
	return;
    }
    secret = local_a.server_data;

    /* 
     * Get the *original* id, challenge & response
     */
    data = a->user_data;
    chap_chall_size = a->user_data_len - sizeof(uchar) - MD5_LEN;
    id = data[0];
    chall = &data[1]; /* length == chap_call_size */
    chap_resp = &data[1 + chap_chall_size]; /* length == MD5_LEN */

    /*
     * Take the secret, the id, and the challenge, put them all
     * together, and run them through MD5.
     */
    len = sizeof(uchar) + secret_len + chap_chall_size;
    mdp = malloc_named(len, "compat. hash");
    if (!mdp) {
	free(secret);
	cleanup_spoof_data(&local_a, local_u, in_tac_pak, out_tac_pak);
	return;
    }

    mdp[0] = id;
    bcopy(secret, &mdp[1], secret_len);
    bcopy(chall, &mdp[secret_len + 1], chap_chall_size);
    MD5Init(&mdcontext);
    MD5Update(&mdcontext, mdp, len);
    MD5Final(digest, &mdcontext);
    free(mdp);
	
    /*
     * Now compare the digest value received from the peer with the
     * just calculated one. 
     */
    if (bcmp(digest, chap_resp, MD5_LEN)) {
	a->status = AAA_AUTHEN_STATUS_FAIL;
    } else {
	a->status = AAA_AUTHEN_STATUS_PASS;
    }

    if (tacacs_debug) {
	buginf("\nTAC+: rev0 inbound chap MD5 compare %s",
	       a->status == AAA_AUTHEN_STATUS_PASS ? "OK" : "FAILED");
    }
    
    cleanup_spoof_data(&local_a, local_u, in_tac_pak, out_tac_pak);
    return;
}

/* 
 * Fake a down-rev outbound chap request adjusting
 * authenstruct appropriately. 
 *
 * Version 1 sends SENDAUTH/CHAP/PPP(username, data=(id,chall))
 * and expects a FAIL or a PASS with an MD5 hash.
 *
 * A down-rev server will reply with a down-rev ERROR
 *
 * Version 0 sends SENDPASS/CHAP/PPP(username) and expects the chap
 * secret in the reply data. 
 */

static void
rev0_outbound_chap (authenstruct *a, userstruct *u, addrtype *server)
{
    userstruct *local_u = NULL;
    authenstruct local_a;
    tacacs_plus_pkt_hdr *out_tac_pak = NULL, *in_tac_pak = NULL;
    int server_index = 0;
    authen_response_body *response;
    uchar *chall;
    char digest[MD5_LEN];
    uchar id, *mdp;
    MD5_CTX mdcontext;
    char *secret, *data;
    int len, secret_len;
    void **handlep = &a->connection_handle;
    int chap_chall_size;

    tacacs_plus_closeconn(server, handlep);
    tacacs_plus_get_conn(server, handlep, TAC_PLUS_AUTHEN, 
			 &server_index);

    /* Create a down-rev CHAP/SENDPASS request */
    bzero(&local_a, sizeof(local_a));
    if (!aaa_create_user(&local_u, u->user, u->ruser, u->port, u->rem_addr, 
			 AAA_AUTHEN_TYPE_CHAP, AAA_SVC_PPP, PRIV_USER)) {
	return;
    }

    local_a.seq_num = 1;
    aaa_randomize_id(&local_a.session_id);
	
    if (tacacs_debug) {
	buginf("\nTAC+: rev0 outbound chap for id=%u using id=%u",
	       a->session_id, local_a.session_id);
    }

    out_tac_pak = create_authen_start(local_u, &local_a, 
				      AAA_AUTHEN_ACT_SENDPASS, 
				      TAC_PLUS_VER_0);

    if (!out_tac_pak) {
	if (tacacs_debug) {
	    buginf("\nTAC+: Error creating authentication start pkt.");
	}
	cleanup_spoof_data(&local_a, local_u, in_tac_pak, out_tac_pak);
	return;
    }

    if (!tacacs_plus_send_receive(out_tac_pak, &in_tac_pak, 
				  server, *handlep, 
				  local_a.session_id)) {
	cleanup_spoof_data(&local_a, local_u, in_tac_pak, out_tac_pak);
	return;
    }

    if (!in_tac_pak) {
	if (tacacs_debug) {
	    buginf("\nTAC+: id=%u No reply from server.", local_a.session_id);
	}
	cleanup_spoof_data(&local_a, local_u, in_tac_pak, out_tac_pak);
	return;
    }

    if (!tacacs_plus_decrypt(server, in_tac_pak)) {
	if (tacacs_debug) {
	    buginf("\nTAC+: Unable to decrypt data from server.");
	}
	cleanup_spoof_data(&local_a, local_u, in_tac_pak, out_tac_pak);
	return;
    }

    if (!authen_resp_sanity_check(in_tac_pak, TAC_PLUS_VER_0,
					 local_a.session_id, 
					 local_a.seq_num+1)) {
	if (tacacs_debug) {
		buginf(check_key_str);
	}
	cleanup_spoof_data(&local_a, local_u, in_tac_pak, out_tac_pak);
	return;
    }

    response = (authen_response_body *)(((char *)in_tac_pak) +
					    TACACS_PLUS_HDR_SIZ);
    authen_debug_response(in_tac_pak);

    if (tacacs_debug) {
	buginf("\nTAC+: rev0 outbound chap completed for id=%u using=id=%u", 
	       a->session_id, local_a.session_id);
    }

    /* 
     * Everything except a PASS or FAIL is an ERROR as far as
     * we're concerned. 
     */

    if (response->status == TAC_PLUS_STATUS_FAIL) {
	if (tacacs_debug) {
	    buginf("\nTAC+: rev0 outbound chap status=FAIL for id=%u", 
		   a->session_id);
	}
	unload_authen_resp(a, response);
	cleanup_spoof_data(&local_a, local_u, in_tac_pak, out_tac_pak);
	return;
    }

    if (response->status != TAC_PLUS_STATUS_PASS) {
	if (tacacs_debug) {
	    buginf("\nTAC+: rev0 outbound chap status=ERROR for id=%u", 
		   a->session_id);
	}
	cleanup_spoof_data(&local_a, local_u, in_tac_pak, out_tac_pak);
	return;
    }

    /* 
     * We have received a PASS. This will contain this user's CHAP
     * secret. 
     */

    unload_authen_resp(&local_a, response);
    if (tacacs_debug) {
	buginf("\nTAC+: rev0 outbound chap SENDPASS status=PASS for id=%u", 
	       a->session_id);
    }

    /* In case we terminate prematurely */
    a->status = TAC_PLUS_STATUS_ERROR; 

    secret_len = local_a.server_data_len;
    if (secret_len <= 0) {
	cleanup_spoof_data(&local_a, local_u, in_tac_pak, out_tac_pak);
	return;
    }
    secret = local_a.server_data;

    /* 
     * Get the *original* id and challenge
     */
    data = a->user_data;
    chap_chall_size = a->user_data_len - sizeof(uchar);
    id = data[0];
    chall = &data[1]; /* length = chap_chall_size */

    /*
     * Take the secret, the id, and the challenge, put them all
     * together, and run them through MD5.
     */
    len = sizeof(uchar) + secret_len + chap_chall_size;
    mdp = malloc_named(len, "compat #1");
    if (!mdp) {
	cleanup_spoof_data(&local_a, local_u, in_tac_pak, out_tac_pak);
	return;
    }

    mdp[0] = id;
    bcopy(secret, &mdp[1], secret_len);
    bcopy(chall, &mdp[secret_len + 1], chap_chall_size);
    MD5Init(&mdcontext);
    MD5Update(&mdcontext, mdp, len);
    MD5Final(digest, &mdcontext);
    free(mdp);

    a->server_data = malloc_named(MD5_LEN, "compat #2");
    if (!a->server_data) {
	cleanup_spoof_data(&local_a, local_u, in_tac_pak, out_tac_pak);
	return;
    }
    bcopy(digest, a->server_data, MD5_LEN);
    a->server_data_len = MD5_LEN;
    a->status = AAA_AUTHEN_STATUS_PASS;

    if (tacacs_debug) {
	buginf("\nTAC+: rev0 outbound chap status=PASS for id=%u", 
	       a->session_id);
    }
    cleanup_spoof_data(&local_a, local_u, in_tac_pak, out_tac_pak);
    return;
}


/* 
 * Spoof an inbound pap login to a down-rev server. Set status to
 * PASS, FAIL or ERROR as appropriate.
 *
 * Version 1 inbound pap sends a START(username, password) and expects
 * a PASS/FAIL reply.
 *
 * Version 0 sends a START(username), expects a GETPASS, sends a
 * CONT(password) and expects a PASS/FAIL reply. 
 * 
 */
static void
rev0_inbound_pap (authenstruct *a, userstruct *u, addrtype *server)
{
    userstruct *local_u;
    authenstruct local_a;
    tacacs_plus_pkt_hdr *out_tac_pak = NULL, *in_tac_pak = NULL;
    int server_index = 0;
    authen_response_body *response;
    void **handlep = &a->connection_handle;
    
    /* 
     * We may have received a GETPASS, so close and re-open the
     * connection.
     */
    tacacs_plus_closeconn(server, handlep);

    tacacs_plus_get_conn(server, handlep,
			 TAC_PLUS_AUTHEN, &server_index);

    /* start new request */
    bzero(&local_a, sizeof(local_a));

    if (!aaa_create_user(&local_u, u->user, u->ruser, u->port, u->rem_addr, 
			 AAA_AUTHEN_TYPE_PAP, AAA_SVC_PPP, 
			 PRIV_USER)) {
	return;
    }

    local_a.seq_num = 1;
    aaa_randomize_id(&local_a.session_id);

    if (tacacs_debug) {
	buginf("\nTAC+: rev0 inbound pap login for id=%u using id=%u",
	       a->session_id, local_a.session_id);
    }

    out_tac_pak = create_authen_start(local_u, &local_a, AAA_AUTHEN_ACT_LOGIN, 
				      TAC_PLUS_VER_0);
    if (!out_tac_pak) {
	if (tacacs_debug) {
	    buginf("\nTAC+: Error creating authentication start pkt.");
	}
	cleanup_spoof_data(&local_a, local_u, in_tac_pak, out_tac_pak);
	return;
    }

    if (!tacacs_plus_send_receive(out_tac_pak, &in_tac_pak, 
				  server, *handlep, local_a.session_id)) {
	cleanup_spoof_data(&local_a, local_u, in_tac_pak, out_tac_pak);
	return;
    }

    if (!in_tac_pak) {
	if (tacacs_debug) {
	    buginf("\nTAC+: No reply from server.");
	}
	cleanup_spoof_data(&local_a, local_u, in_tac_pak, out_tac_pak);
	return;
    }

    if (!tacacs_plus_decrypt(server, in_tac_pak)) {
	if (tacacs_debug) {
	    buginf("\nTAC+: Unable to decrypt data from server.");
	}
	cleanup_spoof_data(&local_a, local_u, in_tac_pak, out_tac_pak);
	return;
    }

    if (!authen_resp_sanity_check(in_tac_pak, TAC_PLUS_VER_0,
				  local_a.session_id, local_a.seq_num+1)) {
	if (tacacs_debug) {
		buginf(check_key_str);
	}
	cleanup_spoof_data(&local_a, local_u, in_tac_pak, out_tac_pak);
	return;
    }

    if (!in_tac_pak) {
	cleanup_spoof_data(&local_a, local_u, in_tac_pak, out_tac_pak);
	return;
    }

    response = (authen_response_body *)(((char *)in_tac_pak) +
					TACACS_PLUS_HDR_SIZ);

    authen_debug_response(in_tac_pak);

    /* 
     * If we got a legitimate packet which ends this session, pass it
     * up 
     */

    switch (response->status) {
    case TAC_PLUS_STATUS_PASS:
    case TAC_PLUS_STATUS_FAIL:
	unload_authen_resp(a, response);
	cleanup_spoof_data(&local_a, local_u, in_tac_pak, out_tac_pak);
	return;

    case TAC_PLUS_STATUS_GETPASS:
	/* what we were hoping for. Keep processing */
	break;

    default:
	cleanup_spoof_data(&local_a, local_u, in_tac_pak, out_tac_pak);
	return;
    }

    /* 
     * Send a CONT(password) reply
     */
    local_a.user_reply = strdup_named(a->user_data,
				      "TAC+ 2"); /* Peer's pap password */
    local_a.flags = 0;
    /* 
     * We have sent a start packet and received a reply. The next
     * packet we send must have sequence number 3 (unload_authen_resp
     * normally tracks this count but we're spoofing so it may not
     * have been called). 
     */
    local_a.seq_num = 3;
    free(out_tac_pak);

    out_tac_pak = create_authen_cont(local_u, &local_a, TAC_PLUS_VER_0);

    if (!out_tac_pak) {
	if (tacacs_debug) {
	    buginf("\nTAC+: Error creating continue packet.");
	}
	cleanup_spoof_data(&local_a, local_u, in_tac_pak, out_tac_pak);
	return;
    }

    if (tacacs_debug || aaa_authen_debug) {
	buginf("\nTAC+: send AUTHEN/CONT packet");
    }

    if (!tacacs_plus_send_receive(out_tac_pak, &in_tac_pak, 
				  server,
				  *handlep, local_a.session_id)) {
	if (tacacs_debug) {
	    buginf("\nTAC+: Error sending continue packet.");
	}
	cleanup_spoof_data(&local_a, local_u, in_tac_pak, out_tac_pak);
	return;
    }

    if (!in_tac_pak) {
	if (tacacs_debug) {
	    buginf("\nTAC+: No reply from server.");
	}
	cleanup_spoof_data(&local_a, local_u, in_tac_pak, out_tac_pak);
	return;
    }

    if (!tacacs_plus_decrypt(server, in_tac_pak)) {
	if (tacacs_debug) {
	    buginf("\nTAC+: Unable to decrypt data from server.");
	}
	cleanup_spoof_data(&local_a, local_u, in_tac_pak, out_tac_pak);
	return;
    }

    if (!authen_resp_sanity_check(in_tac_pak, TAC_PLUS_VER_0,
				  local_a.session_id,
				  local_a.seq_num+1)) {
	if (tacacs_debug) {
	    buginf("\nTAC+: received corrupt data from server.");
	}
	cleanup_spoof_data(&local_a, local_u, in_tac_pak, out_tac_pak);
	return;
    }

    authen_debug_response(in_tac_pak);

    response = (authen_response_body *)(((char *)in_tac_pak) +
					TACACS_PLUS_HDR_SIZ);

    /* Propagate pass, fail, restart, error, follow, forward
     * etc. upwards. Everything else is a failure.
     */

    switch (response->status) {
    case TAC_PLUS_STATUS_PASS:
    case TAC_PLUS_STATUS_FAIL:
	if (tacacs_debug) {
	    buginf("\nTAC+: rev0 inbound pap completed for %u status=%s", 
		   a->session_id,
		   response->status == TAC_PLUS_STATUS_PASS ? "PASS" : "FAIL");
	}
	unload_authen_resp(a, response);
	break;

    default:
	break;
    }

    cleanup_spoof_data(&local_a, local_u, in_tac_pak, out_tac_pak);
    return;
}

/* 
 * Fake a down-rev arap login adjusting status in
 * authenstruct accordingly.
 *
 * Version 1 sends LOGIN/ARAP(username, data=(nas_challenge,
 * remote_challenge, remote_response)) and expects PASS or FAIL with
 * encrypted peer challenge in the data.
 *
 * A down-rev server will either reply with a down-rev ERROR or a
 * down-rev GETPASS packet.
 *
 * Version 0 sends SENDPASS/ARAP(username) and expects the arap secret
 * in the reply data. 
 */
static void
rev0_arap_login (authenstruct *a, userstruct *u, addrtype *server)
{
    userstruct *local_u = NULL;
    authenstruct local_a;
    tacacs_plus_pkt_hdr *out_tac_pak = NULL, *in_tac_pak = NULL;
    int server_index = 0;
    authen_response_body *response;
    void **handlep = &a->connection_handle;
    des_state_type *des_state = NULL;
    char *secret, *result;
    aaa_arap_auth_data arap_data;
    uchar *data;

    /* 
     * If we're here because we got a GETPASS, we are in the middle of
     * a session which we need to terminate. 
     */
    tacacs_plus_closeconn(server, handlep);
    tacacs_plus_get_conn(server, handlep, TAC_PLUS_AUTHEN, 
			 &server_index);

    /* Create a down-rev ARAP/SENDPASS request */
    bzero(&local_a, sizeof(local_a));
    if (!aaa_create_user(&local_u, u->user, u->ruser, u->port, u->rem_addr, 
			 AAA_AUTHEN_TYPE_ARAP, AAA_SVC_ARAP, PRIV_USER)) {
	return;
    }

    local_a.seq_num = 1;
    aaa_randomize_id(&local_a.session_id);

    if (tacacs_debug) {
	buginf("\nTAC+: rev0 arap login for id=%u using id=%u",
	       a->session_id, local_a.session_id);
    }

    out_tac_pak = create_authen_start(local_u, &local_a, 
				      AAA_AUTHEN_ACT_SENDPASS, 
				      TAC_PLUS_VER_0);

    if (!out_tac_pak) {
	if (tacacs_debug) {
	    buginf("\nTAC+: Error creating authentication start pkt.");
	}
	cleanup_spoof_data(&local_a, local_u, in_tac_pak, out_tac_pak);
	return;
    }

    if (!tacacs_plus_send_receive(out_tac_pak, &in_tac_pak, 
				  server, *handlep, 
				  local_a.session_id)) {
	cleanup_spoof_data(&local_a, local_u, in_tac_pak, out_tac_pak);
	return;
    }

    if (!in_tac_pak) {
	if (tacacs_debug) {
	    buginf("\nTAC+: No reply from server.");
	}
	cleanup_spoof_data(&local_a, local_u, in_tac_pak, out_tac_pak);
	return;
    }

    if (!tacacs_plus_decrypt(server, in_tac_pak)) {
	if (tacacs_debug) {
	    buginf("\nTAC+: Unable to decrypt data from server.");
	}
	cleanup_spoof_data(&local_a, local_u, in_tac_pak, out_tac_pak);
	return;
    }

    if (!authen_resp_sanity_check(in_tac_pak, TAC_PLUS_VER_0,
					 local_a.session_id, 
					 local_a.seq_num+1)) {
	if (tacacs_debug) {
		buginf(check_key_str);
	}
	cleanup_spoof_data(&local_a, local_u, in_tac_pak, out_tac_pak);
	return;
    }

    response = (authen_response_body *)(((char *)in_tac_pak) +
					    TACACS_PLUS_HDR_SIZ);
    authen_debug_response(in_tac_pak);

    /* 
     * Everything except a PASS or FAIL is an ERROR as far as
     * we're concerned. 
     */

    if (response->status == TAC_PLUS_STATUS_FAIL) {
	unload_authen_resp(a, response);
	cleanup_spoof_data(&local_a, local_u, in_tac_pak, out_tac_pak);
	return;
    }

    if (response->status != TAC_PLUS_STATUS_PASS) {
	cleanup_spoof_data(&local_a, local_u, in_tac_pak, out_tac_pak);
	return;
    }

    /* 
     * We have received a PASS. This will contain this user's ARAP
     * secret. 
     */

    unload_authen_resp(&local_a, response);

    secret = malloc_named(local_a.server_data_len+1, "arap compat 2");
    if (!secret) {
	cleanup_spoof_data(&local_a, local_u, in_tac_pak, out_tac_pak);
	return;
    }
    bcopy(local_a.server_data, secret, local_a.server_data_len);
    if (secret[0] == '\0') {
	cleanup_spoof_data(&local_a, local_u, in_tac_pak, out_tac_pak);
	free(secret);
	return;
    }

    /* 
     * Get the *original* challenges & response
     */

    data = a->user_data;
    bcopy(data, arap_data.myrandom, 8);
    bcopy(data+8, arap_data.theirrandom, 8);
    bcopy(data+16, arap_data.theirresult, 8);

    reg_invoke_des_pw_bitshift(secret);

    /* First check that they encrypted my random correctly */
    reg_invoke_des_init(0, &des_state);
    reg_invoke_des_setkey(des_state, secret);
    reg_invoke_des_endes(des_state, arap_data.myrandom);
    reg_invoke_des_done(des_state);
    des_state = NULL;

    if (bcmp(arap_data.myrandom, arap_data.theirresult,8)) {
	/* They are different -- fail */
	a->status = AAA_AUTHEN_STATUS_FAIL;
	cleanup_spoof_data(&local_a, local_u, in_tac_pak, out_tac_pak);
	free(secret);
	return;
    }
	
    /* Now encrypt their challenge */
    reg_invoke_des_init(0, &des_state);
    reg_invoke_des_setkey(des_state, secret);
    reg_invoke_des_endes(des_state, arap_data.theirrandom);
    reg_invoke_des_done(des_state);
    des_state = NULL;

    result = malloc_named(8, "arap compat. data");
    if (!result) {
	cleanup_spoof_data(&local_a, local_u, in_tac_pak, out_tac_pak);
	free(secret);
	return;
    }

    bcopy(arap_data.theirrandom, result, 8);

    a->status = AAA_AUTHEN_STATUS_PASS;
    a->server_data = result;
    a->server_data_len = 8;

    if (tacacs_debug) {
	buginf("\nTAC+: rev0 arap login completed for id=%u", a->session_id);
    }
    
    cleanup_spoof_data(&local_a, local_u, in_tac_pak, out_tac_pak);
    free(secret);
    return;
}


static void
compatibility_layer (enum AUTH_ACTIONS action, 
		     authenstruct *a, userstruct *u, 
		     addrtype *server)
{
    /* Assume the worst */
    a->status = AAA_AUTHEN_STATUS_ERROR;

    switch(action) {
    case AAA_AUTHEN_ACT_LOGIN:
	switch(u->authen_type) {
	case AAA_AUTHEN_TYPE_PAP:
	    rev0_inbound_pap(a, u, server);
	    break;
	case AAA_AUTHEN_TYPE_CHAP:
	    rev0_inbound_chap(a, u, server);
	    break;
	case AAA_AUTHEN_TYPE_ARAP:
	    rev0_arap_login(a, u, server);
	    break;
	default:
	    break;
	}
	break;

    case AAA_AUTHEN_ACT_SENDAUTH:
	switch(u->authen_type) {
	case AAA_AUTHEN_TYPE_PAP:
	    rev0_outbound_pap(a, u, server);
	    break;
	case AAA_AUTHEN_TYPE_CHAP:
	    rev0_outbound_chap(a, u, server);
	    break;
	default:
	    break;
	}
	break;

    default:
	break;
    }
}

/*
 * TACACS+ subsystem header
 */
#define TACACS_PLUS_MAJVERSION 1
#define TACACS_PLUS_MINVERSION 0
#define TACACS_PLUS_EDITVERSION  1

SUBSYS_HEADER(tacacs_plus, TACACS_PLUS_MAJVERSION, TACACS_PLUS_MINVERSION,
	      TACACS_PLUS_EDITVERSION,
	      tacacs_plus_init, SUBSYS_CLASS_LIBRARY,
	      NULL,
	      subsys_req_iphost);

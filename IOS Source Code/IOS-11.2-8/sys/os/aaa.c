/* $Id: aaa.c,v 3.16.6.20 1996/09/11 01:46:23 che Exp $
 * $Source: /release/112/cvs/Xsys/os/aaa.c,v $
 *------------------------------------------------------------------
 * aaa.c -- system authentication, authorization and accounting functions
 *
 * September 1994, David Carrel
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: aaa.c,v $
 * Revision 3.16.6.20  1996/09/11  01:46:23  che
 * CSCdi66795:  Double authentication with NASI and TACACS+
 * Branch: California_branch
 * Bypass exec authentication if we have already done NASI authentication.
 *
 * Revision 3.16.6.19  1996/08/15  21:42:53  billw
 * CSCdi65576:  regress - problem found in code review for CSCdi63788
 * Branch: California_branch
 * "Copy" savedauthor by mem_lock() and pointer copy.
 * Make it "const", add comments and casts as appropriate so no one
 * breaks this without concerted effort!
 *
 * Revision 3.16.6.18  1996/08/13  20:41:21  lol
 * CSCdi65470:  aaa authorization methods aren't reset properly
 * Branch: California_branch
 *
 * Revision 3.16.6.17  1996/07/31  23:36:54  billw
 * CSCdi63788:  apparent RADIUS  memory leak in PPP Authentication
 * Branch: California_branch
 *
 * Revision 3.16.6.16  1996/07/30  01:41:49  billw
 * CSCdi59105:  AAA authenthenication errors during continue login cause
 * failure
 * Branch: California_branch
 *
 * Revision 3.16.6.15  1996/07/10  22:21:18  dstine
 * CSCdi62471:  Superfluous #include files in atalk, atm, os files
 * Branch: California_branch
 *
 * Revision 3.16.6.14  1996/07/05  19:01:58  snyder
 * CSCdi62128:  change some char*[] to const and save on data space
 * Branch: California_branch
 *         Gets 244 bytes out of data segment and into text.
 *
 * Revision 3.16.6.13  1996/06/28  23:24:30  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.16.6.12  1996/06/26  22:44:16  billw
 * CSCdi59376:  AAA local prompts not configurable
 * Branch: California_branch
 *
 * Revision 3.16.6.11  1996/06/22  00:35:52  billw
 * CSCdi55669:  null usernames are not handled in a backward compatible way
 * Branch: California_branch
 *
 * Revision 3.16.6.10  1996/06/01  12:28:26  che
 * CSCdi54857:  krb5-instance problem with none auth list
 * Branch: California_branch
 * Add code to set ustruct->user to Kerberos principal and only
 * map instances if authenticated via Kerberos.
 *
 * Revision 3.16.6.9  1996/05/28  13:34:32  fox
 * CSCdi57337:  Remote PAP authentication broken for AAA/TACACS+ configs
 * Branch: California_branch
 * Don't allow remote PAP lookup of password from the local database.
 * If remote PAP support is needed, they can use TACACS+ or 'pap
 * sent-username'.
 *
 * Revision 3.16.6.8  1996/05/21  09:57:56  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.16.6.7  1996/05/06  18:34:44  lol
 * CSCdi56616:  crash in aaa_author_needed
 * Branch: California_branch
 *
 * Revision 3.16.6.6  1996/05/01  02:05:51  perryl
 * CSCdi56162:  MultiChassis MLP and VPN AAA calls mismatched
 * Branch: California_branch
 * Add wrapper functions aaa_chap_rresponse() and aaa_chap_rchallenge(),
 * for
 * L2F and MLPVT to do CHAP style authenication with the new AAA API.
 *
 * Revision 3.16.6.5  1996/04/27  06:36:07  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 3.16.6.4  1996/04/24  02:59:49  lol
 * CSCdi55359:  TACACS events debug output displays handle (0X0) ????
 * Branch: California_branch
 * Cosmetic changes to T+ output formats.
 *
 * Revision 3.10.2.3  1996/04/26  01:12:41  syiu
 * Branch: Dial1_branch
 * Dial1_branch to IosSec_postmerge_960419
 *
 * Revision 3.10.2.2  1996/04/17  03:52:00  syiu
 * Branch: Dial1_branch
 * Sync Dial1_branch to V111_1_3
 *
 * Revision 3.10.2.1  1996/02/23  20:52:39  lbustini
 * Branch: Dial1_branch
 * Move dialer fields out of the idb.
 *
 * Revision 3.16.6.3  1996/04/19  15:24:14  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commit
 *
 * Revision 2.1.6.11  1996/04/16  03:55:37  che
 * Branch: IosSec_branch
 * Code changes mandated by code review comments.
 *
 * Revision 2.1.6.10  1996/04/10  23:07:34  lol
 * Branch: IosSec_branch
 *         Incorporate code review comments.
 *
 * Revision 2.1.6.9  1996/04/09  21:25:40  che
 * Branch: IosSec_branch
 * Sync to California_branch (960405)
 *
 * Revision 2.1.6.8  1996/03/19  08:24:42  che
 * Branch: IosSec_branch
 *
 * Revision 2.1.6.7  1996/03/05  04:43:50  che
 * Branch: IosSec_branch
 * Kerberos Phase 2 commit.
 *
 * Revision 2.1.6.6  1996/02/28  06:14:39  lol
 * Branch: IosSec_branch
 * TACACS+ processing.
 *
 * Revision 2.1.6.5  1996/02/21  03:47:47  che
 * Branch: IosSec_branch
 * Sync to 11.1 Mainline (02/20/96)
 *
 * Revision 2.1.6.4  1996/01/23  06:13:17  che
 * Branch: IosSec_branch
 * Sync with 11.1 mainline. (960122)
 *
 * Revision 2.1.6.3  1995/12/19  04:08:40  che
 * Branch: IosSec_branch
 * Re-sync to Arkansas to fix things that broke.
 *
 * Revision 2.1.6.2  1995/12/17  02:11:20  che
 * Branch: IosSec_branch
 * Sync to Arkansas
 *
 * Revision 2.1.6.1  1995/11/17  17:21:03  carrel
 * Branch: IosSec_branch
 * Sync to Arkansas
 *
 * Revision 3.16.6.2  1996/04/15  14:58:46  widmer
 * CSCdi47180:  No Single Command to Collect General Router Data
 * Branch: California_branch
 * Add "show tech-support" command
 * Change references to encryption types to uint
 * Change tt_soc to use password_struct
 *
 * Revision 3.16.6.1  1996/03/18  21:28:35  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.10.6.3  1996/03/16  07:20:04  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.10.6.2  1996/03/07  10:17:33  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.10.6.1  1996/02/20  16:36:31  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.16  1996/03/01  18:25:30  fox
 * CSCdi50216:  crash in localuser_do_author (no ustruct)
 * Sanity check 'ustruct' pointer passed in call
 *
 * Revision 3.15  1996/02/09  10:04:06  billw
 * CSCdi48421:  autoselect during-login causes multiple execs on single tty
 *
 * Revision 3.14  1996/02/03  00:33:02  billw
 * CSCdi48056:  AAA/AUTHEN/CHAP not supported by method is displayed
 * make the buginf conditional.  Sigh.
 *
 * Revision 3.13  1996/02/01  06:07:22  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.12  1996/01/29  00:30:31  billw
 * CSCdi13190:  Increase password characters encryted
 * Make sure that the maximum length password that can be encrypted is
 * the maximum length that will be accepted.
 *
 * Revision 3.11  1996/01/23  22:39:43  billw
 * CSCdi43679:  RADIUS support does not include CHAP
 * Do it in a way that will be obsolete in California
 *
 * Revision 3.10  1996/01/12  02:40:48  billw
 * CSCdi44858:  use #define for AAA attribute values
 * Bring the AV pairs used up to spec level...
 *
 * Revision 3.9  1995/12/24  09:36:02  che
 * CSCdi46019:  Store Kerberos credentials in Kerberos library cred cache
 * Use Kerberos library credentials cache structures and functions to
 * manipulate user credentials cache.
 *
 * Revision 3.8  1995/12/20  17:36:35  lol
 * CSCdi45882:  tacacs+ crash in aaa_dup_ifneeded
 *
 * Revision 3.7  1995/12/17  18:34:01  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.6  1995/12/13  02:59:32  billw
 * CSCdi42536:  aaa_acct not proper subsystem
 * Move some code around, change some calls to reg_invokes, etc.
 *
 * Revision 3.5  1995/12/12  06:00:59  billw
 * CSCdi41773:  Login (username) promtps different with local override
 * confiured
 *
 * Revision 3.4  1995/11/29  18:53:05  syiu
 * CSCdi40008:  memory leaking in AAA ustruct when using PPP with CHAP/PAP
 *
 * Revision 3.3  1995/11/17  18:44:05  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:16:32  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:43:28  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.19  1995/11/08  21:18:10  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.18  1995/10/20  20:24:49  lol
 * CSCdi42253:  callback causes IPCP authorization failures
 *              Only return callback info when doing local authorization of
 *              ppp/lcp, slip, arap or exec
 *
 * Revision 2.17  1995/10/02  20:42:51  lol
 * CSCdi36244:  Console line failing authorization should be more forgiving
 *              Authenticated console users now bypass authorization.
 *
 * Revision 2.16  1995/09/16  21:42:52  carrel
 * CSCdi40334:  Bus error crash in AAA accounting code
 * Removed unnecessary NULL ptr checks.
 * Added some needed NULL ptr checks!!
 * Made a call to acct_free() be safe!!
 * Removed unneeded call to aaa_dup_user().
 * Generally made the code prettier.  ;-)
 * Removed unused arg from aaa_dup_ifneeded().
 *
 * Revision 2.15  1995/09/10  23:36:01  carrel
 * CSCdi40013:  User information needs to be present even when no login
 * occurs
 * aaa_create_user() now initializes the acc_method to be
 * AAA_ACC_METH_NONE.
 *
 * Revision 2.14  1995/09/07  09:46:45  irfan
 * CSCdi39880:  Memory leaking in AAA/tacacs_plus routines called from exec
 * Remember to free up server_msg and server_admin_msg strings in
 * aaa_author_cleanup.
 *
 * Revision 2.13  1995/08/08  20:50:23  lol
 * CSCdi37361:  AAA should not require auth for originating PPP connections
 *
 * Revision 2.12  1995/08/08  16:58:03  widmer
 * CSCdi37574:  long  commands truncated via tftp download
 * cut & paste programming errors changes wrong variables
 *
 * Revision 2.11  1995/08/07  21:52:21  lol
 * CSCdi36743:  async PPP fails to connect with /routing and tacacs+
 * authorization
 *
 * Revision 2.10  1995/08/04  23:14:00  irfan
 * CSCdi38157:  memory leaks in PPP PAP, CHAP, Callback
 * fix memory leaks in pap_rrequest and chap_rresponse path towards
 * callback. change as many malloc's to malloc_named, as possible.
 *
 * Revision 2.9  1995/07/26  14:14:52  billw
 * CSCdi37036:  No AAA Accounting records generated for system events
 * generate "system" records for startup, shutdown, and change in the
 * state of "aaa accounting system" configuration.
 *
 * Revision 2.8  1995/07/26  00:00:54  billw
 * CSCdi37596:  tacacs+ crash in _login_exec_command/aaa_dup_user
 * dupuser will now fail on null input userstruct
 *
 * Revision 2.7  1995/07/13  06:45:45  billw
 * CSCdi36954:  AAA accounting done improperly when exec login command is
 * used
 *
 * aaa_dup_user no longer duplicates acctQ.  New registry point for
 * changing from one username to another.
 *
 * Revision 2.6  1995/06/30  06:42:20  dstine
 * CSCdi36619:  IDB nits
 *         - remove #include of interface_private.h
 *
 * Revision 2.5  1995/06/30  05:21:29  hampton
 * Move externs for the random number generator to their own file.  Remove
 * redundant initialization of the random number generator.  [CSCdi36588]
 *
 * Revision 2.4  1995/06/15 22:57:43  syiu
 * CSCdi35686:  Async callback AAA authorization and accounting phase not
 * working
 *
 * Revision 2.3  1995/06/15  03:42:16  lol
 * CSCdi35713:  AAA subsystems needs more debugging output
 *              Add
 *                 aaa debug authentication
 *                 aaa debug authorization
 *                 aaa debug accounting
 *
 * Revision 2.2  1995/06/14  06:30:40  billw
 * CSCdi35380:  AAA accounting causes crashes
 * Clean up aaa_dup_user and callers, correct freeing of non-malloced
 * memory, and a couple other things.
 *
 * Revision 2.1  1995/06/07  21:52:59  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <string.h>
#include <ciscolib.h>
#include "ttysrv.h"
#include "login.h"
#include "packet.h"
#include "registry.h"
#include "../h/auth_registry.regh"
#include "config.h"
#include "parser.h"
#include "../util/md5.h"
#include "../util/md5_crypt.h"
#include "parser_defs_aaa.h"
#include "../parser/parser_actions.h"
#include "aaa.h"
#include "../ui/debug.h"
#include "../os/os_debug_flags.h"
#include "logger.h"
#include "free.h"
#include "../util/random.h"
#include "interface_private.h"
#include "../wan/dialer.h"
#include "../wan/dialer_registry.h"
#include "connect.h"
#include "../les/isdn_bri.h"
#include <stdlib.h>

boolean old_access_control = TRUE;	/* Is old access control in effect? */
boolean local_override = FALSE;	/* Are local accounts always first */
boolean kerberos5_is_installed = FALSE;
boolean tacacsplus_is_installed = FALSE;
boolean radius_is_installed = FALSE;
boolean aaa_author_conf_commands = TRUE;
boolean aaa_acct_suppress_null_username = FALSE;
boolean aaa_acct_nested = FALSE;

ulong aaa_user_create, aaa_user_frees, aaa_user_acctleft;

static struct access_method_list login_access_methods[MAX_ACC_LISTS_LOGIN];
static struct access_method_list enable_access_methods[MAX_ACC_LISTS_ENABLE];
static struct access_method_list ppp_access_methods[MAX_ACC_LISTS_PPP];
static struct access_method_list arap_access_methods[MAX_ACC_LISTS_ARAP];
static struct access_method_list nasi_access_methods[MAX_ACC_LISTS_NASI];

/*
 * Default authentication lists to use if no authentication is configured,
 * so that our "backup" behavior is as expected, and doesn't prevent admins
 * from continuing to configure the box.  These are more or less the same as
 * the old enable and console behaviors before "aaa new-model".
 *
 * If no authentication list is specified, and no default list 
 * exists, then we pick a default action
 *   if on console and this is a LOGIN 
 *      then no authentication is required
 *   else if on console and this is a ENABLE
 *      then use enable pass if set else no authentication is required
 *   else if this is an ENABLE
 *      then use enable password only
 *   else
 *      use LOCAL db only
 */

static int ena_noauthset_method[] = {AAA_ACC_METH_ENABLE,
					 AAA_ACC_METH_NOT_SET};
static int loc_noauthset_method[] = {AAA_ACC_METH_LOCAL, AAA_ACC_METH_NOT_SET};
static int con_noauthset_method[] = {AAA_ACC_METH_NONE, AAA_ACC_METH_NOT_SET};
static int con_e_noauthset_meth[] = {AAA_ACC_METH_ENABLE, AAA_ACC_METH_NONE,
					 AAA_ACC_METH_NOT_SET};
static int localauth_method[] =     {AAA_ACC_METH_LOCAL, AAA_ACC_METH_NOT_SET};


static struct author_method_list author_lists[AUTHOR_TYPE_LAST + PRIV_MAX + 1];

struct acct_method_list acct_lists[AUTHOR_TYPE_LAST + PRIV_MAX + 1];

static const char default_username_prompt[] =		"Username: ";
static const char default_password_prompt[] =		"Password: ";
char * username_prompt = NULL;
char * password_prompt = NULL;

static const char old_password_prompt[] =	"Old Password: ";
static const char new_password_prompt[] =	"New Password: ";
const char auth_failed[] =			"% Access denied";
const char author_failed_msg[] =		"% Authorization failed.";
static const char backup_authen_msg[] =		"% Backup authentication";

/*
 * Local static functions
 */
static boolean localuser_start_login(userstruct *, authenstruct *, int,
				     enum AAA_SERVICES);
static boolean localuser_cont_login(userstruct *, authenstruct *);
static boolean fixedpass_start_login(userstruct *, authenstruct *, int, int,
				     enum AAA_SERVICES);
static boolean fixedpass_cont_login(userstruct *, authenstruct *);
static boolean getpass(char *pass, char *outpass, uint enc_type);
static boolean localuser_do_author(userstruct *ustruct, authorstruct *astruct);
static int aaa_scan_callback(authorstruct *, usernametype *);
static struct access_method_list *aaa_pick_accmethod_list (enum AAA_SERVICES, int*);

static char *name_kerberos = "KRB5";
static char *name_radius = "RADIUS";
static char *name_tacplus = "TACACS+";
static boolean aaa_pap_check(authenstruct *astruct, char *passwd);
static boolean aaa_chap_check(authenstruct *astruct, char *passwd);
static boolean aaa_arap_check(authenstruct *astruct, char *passwd);
static boolean aaa_chap_sendauth(authenstruct *astruct, char *passwd);
  

static boolean
aaa_pap_check(authenstruct *astruct, char *passwd)
{
    int len = strlen(passwd);
    if ((len != astruct->user_data_len) ||
	strncmp(astruct->user_data, passwd, len))
	return(FALSE);
    return(TRUE);
}

static boolean aaa_chap_check(authenstruct *astruct, char *passwd)
{
    int plen, clen, olen;
    MD5_CTX mdcontext;
    char digest[MD5_LEN];
    char *mdstream, *chal, *resp;

    if (astruct->user_data_len <= (MD5_LEN + 2)) {
	/* Invalid packet or NULL challenge */
	return(FALSE);
    }

    plen = strlen(passwd);
    clen = astruct->user_data_len - 1 - MD5_LEN;
    olen = 1 + plen + clen;
    chal = &astruct->user_data[1];
    resp = &astruct->user_data[1 + clen];

    mdstream = malloc_named(olen, "aaa_chap_check");
    if (!mdstream)
	return(FALSE);
    mdstream[0] = astruct->user_data[0];
    bcopy(passwd, &mdstream[1], plen);
    bcopy(chal, &mdstream[1 + plen], clen);
    MD5Init(&mdcontext);
    MD5Update(&mdcontext, mdstream, olen);
    MD5Final(digest, &mdcontext);
    free(mdstream);

    if (bcmp(digest, resp, MD5_LEN))
	return(FALSE);
    return(TRUE);
}	

static boolean aaa_arap_check(authenstruct *astruct, char *passwd)
{
    des_state_type *des_state = NULL;
    aaa_arap_auth_data *data = (aaa_arap_auth_data *)astruct->user_data;
    char lpass[DES_BLOCK_SIZE];
    int i;

    if (astruct->user_data_len != sizeof(aaa_arap_auth_data)) {
	if (aaa_authen_debug)
	    buginf("\nAAA/AUTHEN/ARAP: bad data size.");
	return(FALSE);
    }

    bzero(lpass, sizeof(lpass));
    for (i = 0; i < DES_BLOCK_SIZE && passwd[i]; i++)
        lpass[i] = passwd[i] << 1;

    /* First check that they encrypted our random # correctly */
    reg_invoke_des_init(0, &des_state);
    reg_invoke_des_setkey(des_state, lpass);
    reg_invoke_des_endes(des_state, data->myrandom);
    reg_invoke_des_done(des_state);
    des_state = NULL;
    if (bcmp(data->myrandom, data->theirresult, DES_BLOCK_SIZE))
	return(FALSE);		/* Authentication failed. */

    free(astruct->server_data);
    astruct->server_data = malloc_named(DES_BLOCK_SIZE, "aaa_arap_check");
    if (!astruct->server_data) {
	astruct->server_data_len = 0;
	return(FALSE);
    }
    astruct->server_data_len = DES_BLOCK_SIZE;
    bcopy(data->theirrandom, astruct->server_data, DES_BLOCK_SIZE);
    /* Now encrypt their challenge */
    reg_invoke_des_init(0, &des_state);
    reg_invoke_des_setkey(des_state, lpass);
    reg_invoke_des_endes(des_state, astruct->server_data);
    reg_invoke_des_done(des_state);
    des_state = NULL;

    return(TRUE);
}

static boolean aaa_chap_sendauth(authenstruct *astruct, char *passwd)
{
    int plen, clen, olen;
    MD5_CTX mdcontext;
    char *mdstream, *chal;

    free(astruct->server_data);
    astruct->server_data_len = 0;
    if (astruct->user_data_len <= 2) {
	/* Invalid packet or NULL challenge */
	return(FALSE);
    }

    astruct->server_data = malloc_named(MD5_LEN, "aaa_chap_sendauth 2");
    if (!astruct->server_data)
	return(FALSE);

    plen = strlen(passwd);
    clen = astruct->user_data_len - 1;
    olen = 1 + plen + clen;
    chal = &astruct->user_data[1];

    mdstream = malloc_named(olen, "aaa_chap_sendauth 1");
    if (!mdstream) {
	setstring(&astruct->server_data, NULL);
	return(FALSE);
    }
    mdstream[0] = astruct->user_data[0];
    bcopy(passwd, &mdstream[1], plen);
    bcopy(chal, &mdstream[1 + plen], clen);
    MD5Init(&mdcontext);
    MD5Update(&mdcontext, mdstream, olen);
    MD5Final(astruct->server_data, &mdcontext);
    free(mdstream);

    astruct->server_data_len = MD5_LEN;
    return(TRUE);
}	

void
aaa_init (void)
{
    aaa_parser_init();
    setstring_named(&password_prompt, default_password_prompt,
		    "AAA PROMPT P1");
    setstring_named(&username_prompt, default_username_prompt,
		    "AAA PROMPT U1");
}

void
aaa_command (parseinfo *csb)
{
    int i, list_num, max_num;
    struct access_method_list *list;
    char *list_name, *svc_name;
    
    if (csb->nvgen) {
	switch (csb->which) {
	case PARSER_AUTHEN_PROMPTS:
	    if (GETOBJ(int,1) == AAA_PROMPT_USER)
		nv_write(strcmp(default_username_prompt, username_prompt),
			 "%s %S", csb->nv_command, username_prompt);
	    if (GETOBJ(int,1) == AAA_PROMPT_PASSWORD)
		nv_write(strcmp(default_password_prompt, password_prompt),
			 "%s %S", csb->nv_command, password_prompt);
	    break;
	case PARSER_AUTHEN_NEW_MODEL:
	    nv_write(!old_access_control, csb->nv_command);
	    break;
	case PARSER_ACCT_SUPPRESS_NULL_USER:
	    nv_write(aaa_acct_suppress_null_username, csb->nv_command);
	    break;
	case PARSER_ACCT_NESTED:
	    nv_write(aaa_acct_nested, csb->nv_command);
	    break;
	case PARSER_AUTHEN_LOCAL_O:
	    nv_write(local_override, csb->nv_command);
	    break;
	case PARSER_AUTHEN_LISTS:
	    list = aaa_pick_accmethod_list(GETOBJ(int,1), &max_num);
	    if (!list) {
		buginf("\n%% AAA: bad authentication list name.");
		return;
	    }
	    for (list_num = 0; list_num < max_num; list_num++) {
		if (list[list_num].valid) {
		    if (!list[list_num].name[0] ||
			list[list_num].methods[0] == AAA_ACC_METH_NOT_SET) {
			buginf("\n%% AAA: Bad authentication list in memory.");
			break;
		    }
		    nv_write(TRUE, "%s %s", csb->nv_command,
			     list[list_num].name);
		    for (i = 0; i < MAX_ACC_METHS; i++) {
			if (list[list_num].methods[i] == AAA_ACC_METH_NOT_SET)
			    break;
			switch (list[list_num].methods[i]) {
			case AAA_ACC_METH_NONE:
			    nv_add(TRUE, " none");
			    break;
			case AAA_ACC_METH_KRB5:
			    nv_add(TRUE, " krb5");
			    break;
			case AAA_ACC_METH_KRB5_TELNET:
			    nv_add(TRUE, " krb5-telnet");
			    break;
			case AAA_ACC_METH_LINE:
			    nv_add(TRUE, " line");
			    break;
			case AAA_ACC_METH_ENABLE:
			    nv_add(TRUE, " enable");
			    break;
			case AAA_ACC_METH_LOCAL:
			    nv_add(TRUE, " local");
			    break;
			case AAA_ACC_METH_TACACSPLUS:
			    nv_add(TRUE, " tacacs+");
			    break;
			case AAA_ACC_METH_IFNEEDED:
			    nv_add(TRUE, " if-needed");
			    break;
			case AAA_ACC_METH_GUEST:
			    nv_add(TRUE, " guest");
			    break;
			case AAA_ACC_METH_AUTHGUEST:
			    nv_add(TRUE, " auth-guest");
			    break;
		        case AAA_ACC_METH_RADIUS:
			    nv_add(TRUE, " radius");
			    break;
			}
		    }
		}
	    }
	    break;
	case PARSER_AUTHOR_LIST:
	case PARSER_ACCT_LIST:
	    max_num = list_num = GETOBJ(int,2);
	    if (list_num == AAA_AUTHOR_TYPE_CMD) {
		max_num += PRIV_MAX;
	    }
	    switch (GETOBJ(int,1)) {
	    case PARSER_AUTHOR_LIST:
		/* check if we have 'aaa authorization config-commands', which
		 * is handled differently
		 */
		if (list_num == PARSER_AUTHOR_CONFIG_CMD) {
		    nv_write(!aaa_author_conf_commands, "no %s",
			     csb->nv_command);
		    break;
		} else {
		    for (; list_num <= max_num; list_num++) {
			if (author_lists[list_num].valid) {
			    nv_write(TRUE, "%s", csb->nv_command);
			    if (GETOBJ(int,2) == AAA_AUTHOR_TYPE_CMD)
				nv_add(TRUE, " %d",
				       list_num - AAA_AUTHOR_TYPE_CMD);
			    for (i = 0; i < NUM_AUTHOR_METHS; i++) {
				switch (author_lists[list_num].methods[i]) {
				case AAA_AUTHOR_METH_TAC_PLUS:
				    nv_add(TRUE, " tacacs+");
				    break;
				case AAA_AUTHOR_METH_RADIUS:
				    nv_add(TRUE, " radius");
				    break;
				case AAA_AUTHOR_METH_IF_AUTHEN:
				    nv_add(TRUE, " if-authenticated");
				    break;
				case AAA_AUTHOR_METH_LOCAL:
				    nv_add(TRUE, " local");
				    break;
				case AAA_AUTHOR_METH_NONE:
				    nv_add(TRUE, " none");
				    break;
				case AAA_AUTHOR_METH_KRB5_INSTANCE:
				    nv_add(TRUE, " krb5-instance");
				    break;
				case AAA_AUTHOR_METH_NOT_SET:
				    i = NUM_AUTHOR_METHS; /* get out of loop */
				    break;
				}
			    }
			}
		    }
		}
		break;
	    case PARSER_ACCT_LIST:
		for (; list_num <= max_num; list_num++) {
		    if (acct_lists[list_num].valid) {
			nv_write(TRUE, "%s", csb->nv_command);
			if (GETOBJ(int,2) == AAA_AUTHOR_TYPE_CMD)
			    nv_add(TRUE, " %d",list_num - AAA_AUTHOR_TYPE_CMD);
			switch (acct_lists[list_num].act) {
			case AAA_ACCT_ACT_START_STOP:
			    nv_add(TRUE, " start-stop");
			    break;
			case AAA_ACCT_ACT_WAIT_START:
			    nv_add(TRUE, " wait-start");
			    break;
			case AAA_ACCT_ACT_STOP_ONLY:
			    nv_add(TRUE, " stop-only");
			    break;
			}
			for (i = 0; i < NUM_ACCT_METHS; i++) {
			    switch (acct_lists[list_num].methods[i]) {
			    case AAA_AUTHOR_METH_TAC_PLUS:
				nv_add(TRUE, " tacacs+");
				break;
			    case AAA_AUTHOR_METH_RADIUS:
				nv_add(TRUE, " radius");
				break;
			    case AAA_AUTHOR_METH_NOT_SET:
			    default:	/* because acct is subset of author */
				i = NUM_ACCT_METHS;	/* get out of loop */
				break;
			    }
			}
		    }
		}
		break;
	    default:
		buginf("\n%% AAA: not authorization or accounting.");
		break;
	    }
	    break;
	default:
	    buginf("\n%% AAA: bad aaa argument.");
	    break;
	}
	return;
    }
    switch (csb->which) {
    case PARSER_AUTHEN_NEW_MODEL:
	old_access_control = csb->sense ? FALSE : TRUE;
	break;
    case PARSER_ACCT_SUPPRESS_NULL_USER:
	aaa_acct_suppress_null_username = csb->sense;
	break;
    case PARSER_ACCT_NESTED:
	aaa_acct_nested = csb->sense;
	break;
    case PARSER_AUTHEN_LOCAL_O:
	local_override = csb->sense;
	break;
    case PARSER_AUTHEN_PROMPTS:
	if (GETOBJ(int,1) == AAA_PROMPT_USER) {
	    if (csb->sense && !null(GETOBJ(string,1)))
		setstring_named(&username_prompt, GETOBJ(string,1),
				"AAA PROMPT U2");
	    else
		setstring_named(&username_prompt, default_username_prompt,
				"AAA PROMPT U3");
	} else
	if (GETOBJ(int,1) == AAA_PROMPT_PASSWORD) {
	    if (csb->sense && !null(GETOBJ(string,1)))
		setstring_named(&password_prompt, GETOBJ(string,1),
				"AAA PROMPT P2");
	    else
		setstring_named(&password_prompt, default_password_prompt,
				"AAA PROMPT P3");
	}
	break;
    case PARSER_AUTHEN_LISTS:
	switch (GETOBJ(int,1)) {
	case AAA_SVC_ARAP:
	    list = &arap_access_methods[0];
	    max_num = MAX_ACC_LISTS_ARAP;
	    svc_name = "arap";
	    break;
	case AAA_SVC_PPP:
	    list = &ppp_access_methods[0];
	    max_num = MAX_ACC_LISTS_PPP;
	    svc_name = "ppp";
	    break;
	case AAA_SVC_ENABLE:
	    list = &enable_access_methods[0];
	    max_num = MAX_ACC_LISTS_ENABLE;
	    svc_name = "enable";
	    break;
	case AAA_SVC_LOGIN:
	    list = &login_access_methods[0];
	    max_num = MAX_ACC_LISTS_LOGIN;
	    svc_name = "login";
	    break;
	case AAA_SVC_NASI:
	    list = &nasi_access_methods[0];
	    max_num = MAX_ACC_LISTS_NASI;
	    svc_name = "nasi";
	    break;
	default:
	    buginf("\n%% AAA: bad authentication list name.");
	    return;
	    break;
	}

	switch (GETOBJ(int,2)) {
	case 1:
	    list_name = ACC_LIST_DEFAULT;
	    list_num = 0;
	    break;
	case 2:
	    list_name = GETOBJ(string,1);
	    for (list_num = 1; list_num < max_num; list_num++) {
		if (list[list_num].valid && !strcmp(list_name,
						    list[list_num].name))
		    break;
	    }
	    break;
	default:
	    buginf("\n%% AAA: Bad state for authentication list name.");
	    return;
	    break;
	}
	if (!csb->sense) {
	    if (list_num < max_num) {	/* found a list */
		list[list_num].valid = FALSE;
		for (i = 0; i < MAX_ACC_METHS; i++) {
		    list[list_num].methods[i] = AAA_ACC_METH_NOT_SET;
		}
	    }
	} else {
	    if (list_num == max_num) {	/* didn't find an existing list */
		for (list_num = 1; list_num < max_num; list_num++) {
		    if (!list[list_num].valid)
			break;
		}
		if (list_num == max_num) {
		    printf("%% AAA: No free authentication lists for "
			   "\"%s\".\n", svc_name);
		    return;
		}
	    }
	    list[list_num].valid = TRUE;
	    sstrncpy(list[list_num].name, list_name,
		     sizeof(list[list_num].name));
	    for (i = 1; i <= MAX_ACC_METHS; i++) {
		if (PARSED_METH_KRB5 == i) {
		    list[list_num].methods[i-1] = AAA_ACC_METH_KRB5;
		    continue;
		}
		if (PARSED_METH_TACPLUS == i) {
		    list[list_num].methods[i-1] = AAA_ACC_METH_TACACSPLUS;
		    continue;
		}
		if (PARSED_METH_RADIUS == i) {
		    list[list_num].methods[i-1] = AAA_ACC_METH_RADIUS;
		    continue;
		}
		if (PARSED_METH_LOCAL == i) {
		    list[list_num].methods[i-1] = AAA_ACC_METH_LOCAL;
		    continue;
		}
		if (PARSED_METH_LINE == i) {
		    list[list_num].methods[i-1] = AAA_ACC_METH_LINE;
		    continue;
		}
		if (PARSED_METH_ENABLE == i) {
		    list[list_num].methods[i-1] = AAA_ACC_METH_ENABLE;
		    continue;
		}
		if (PARSED_METH_NONE == i) {
		    list[list_num].methods[i-1] = AAA_ACC_METH_NONE;
		    continue;
		}
		if (PARSED_METH_IFNEEDED == i) {
		    list[list_num].methods[i-1] = AAA_ACC_METH_IFNEEDED;
		    continue;
		}
		if (PARSED_METH_GUEST == i) {
		    list[list_num].methods[i-1] = AAA_ACC_METH_GUEST;
		    continue;
		}
		if (PARSED_METH_AUTHGUEST == i) {
		    list[list_num].methods[i-1] = AAA_ACC_METH_AUTHGUEST;
		    continue;
		}
		if (PARSED_METH_KRB5_TELNET == i) {
		    list[list_num].methods[i-1] = AAA_ACC_METH_KRB5_TELNET;
		    continue;
		}
		break;
	    }
	    if (i <= MAX_ACC_METHS)	 /* Terminate list */
		list[list_num].methods[i-1] = AAA_ACC_METH_NOT_SET;
	}
	break;
    case PARSER_AUTHOR_LIST:
    case PARSER_ACCT_LIST:
	/* Both authorization and accounting come in through here. */
	list_num = GETOBJ(int,2);
	if (list_num == AAA_AUTHOR_TYPE_CMD)
	    list_num += GETOBJ(int,3);
	switch(GETOBJ(int,1)) {
	case PARSER_AUTHOR_LIST:
	    /* check for 'aaa authorization config-commands, since that is
	     * handled differently
	     */
	    if (list_num == PARSER_AUTHOR_CONFIG_CMD) {
		aaa_author_conf_commands = csb->sense;
		break;
	    } else {
		if (!csb->sense) {
		    author_lists[list_num].valid = FALSE;
		    author_lists[list_num].methods[0] =
			AAA_AUTHOR_METH_NOT_SET;
		} else {
		    author_lists[list_num].valid = TRUE;
		    for (i = 0; i < (NUM_AUTHOR_METHS - 1); i++) {
			if (PARSED_METH_TACPLUS == (i + 1)) {
			    author_lists[list_num].methods[i] =
				AAA_AUTHOR_METH_TAC_PLUS;
			    continue;
			}
			if (PARSED_METH_RADIUS == (i + 1)) {
			    author_lists[list_num].methods[i] =
				AAA_AUTHOR_METH_RADIUS;
			    continue;
			}
			if (PARSED_METH_IFNEEDED == (i + 1)) {
			    author_lists[list_num].methods[i] =
				AAA_AUTHOR_METH_IF_AUTHEN;
			    continue;
			}
			if (PARSED_METH_LOCAL == (i + 1)) {
			    author_lists[list_num].methods[i] =
				AAA_AUTHOR_METH_LOCAL;
			    continue;
			}
			if (PARSED_METH_NONE == (i + 1)) {
			    author_lists[list_num].methods[i] =
				AAA_AUTHOR_METH_NONE;
			    continue;
			}
			if (PARSED_METH_KRB5 == (i + 1)) {
			    author_lists[list_num].methods[i] =
				AAA_AUTHOR_METH_KRB5_INSTANCE;
			    continue;
			}
			break;
		    }
		    /* terminate list */
		    author_lists[list_num].methods[i] =
			AAA_AUTHOR_METH_NOT_SET;
		}
	    }
	    break;
	case PARSER_ACCT_LIST:
	if (GETOBJ(int,1) == PARSER_ACCT_LIST)
	    if (!csb->sense) {
		if (acct_lists[list_num].valid)	/* Notify before turning off */
		    reg_invoke_aaa_acct_cmdparse(csb);
		acct_lists[list_num].valid = FALSE;
		acct_lists[list_num].methods[0] = AAA_AUTHOR_METH_NOT_SET;
	    } else {
		boolean needacct = !acct_lists[list_num].valid;

		acct_lists[list_num].valid = TRUE;
		acct_lists[list_num].act = GETOBJ(int,4);
		for (i = 0; i < (NUM_ACCT_METHS - 1); i++) {
		    if (PARSED_METH_TACPLUS == (i + 1)) {
			acct_lists[list_num].methods[i] =
			    AAA_AUTHOR_METH_TAC_PLUS;
			continue;
		    }
		    if (PARSED_METH_RADIUS == (i + 1)) {
			acct_lists[list_num].methods[i] =
			    AAA_AUTHOR_METH_RADIUS;
			continue;
		    }
		    break;
		}
		/* terminate list */
		acct_lists[list_num].methods[i] = AAA_AUTHOR_METH_NOT_SET;
		if (needacct) /* Now send info after we are turned on */
		    reg_invoke_aaa_acct_cmdparse(csb);
	    }
	    break;
	default:
	    buginf("\n%% AAA: not authorization or accounting.");
	    break;
	}
	break;
    default:
	buginf("\n%% AAA: bad aaa argument.");
	break;
    }
    return;
}

boolean
aaa_is_list_defined (char *list_name, enum AAA_SERVICES service)
{
    int max_num, i;
    struct access_method_list *lists;

    lists = aaa_pick_accmethod_list(service, &max_num);

    for (i = 0; i < max_num; i++) {
	if (lists[i].valid && !strcmp(lists[i].name, list_name))
	    return(TRUE);
    }
    return(FALSE);
}

void
aaa_close_connection (userstruct *u, void **handle)
{
    switch (u->acc_method) {
    case AAA_ACC_METH_TACACSPLUS:
	reg_invoke_tacacs_closeconn(&u->server, handle);
	break;
    default:
	break;
    }
}

static boolean
getpass (char *pass, char *outpass, uint enc_type)
{
    switch(enc_type) {
    case ENCRYPT_NONE:
	sstrncpy(outpass, pass, PASSWORDLEN);
        return(TRUE);
    case ENCRYPT_TEMP:
        decrypt(pass, outpass, PASSWORDLEN);
	return(TRUE);
    case ENCRYPT_UNIX:
    case ENCRYPT_MD5:
        return(FALSE);
    default:
        buginf("\n%% Password check with invalid encryption type");
        return(FALSE);
    }
}

static boolean
fixedpass_start_login(userstruct *ustruct, authenstruct *astruct, int action,
		      int access_method, enum AAA_SERVICES service)
{
    char outpass[PASSWORDLEN+1], *pass = NULL;
    int level;
    boolean error = FALSE, secret = FALSE;
    password_struct *password = NULL;

    setstring(&astruct->server_msg, NULL);

    switch (access_method) {
    case AAA_ACC_METH_LINE:
	if (!astruct->tty) {
	    if (aaa_authen_debug) {
		buginf("\nAAA/AUTHEN (%u): no tty for line password",
		       astruct->session_id);
	    }
	} else {
	    password = astruct->tty->password;
	}
	break;
    case AAA_ACC_METH_ENABLE:
	if ((service == AAA_SVC_ENABLE) && ustruct)
	    level = ustruct->priv_lvl;
	else
	    level = PRIV_MAX;
	if (enable_secret[level].pw) {
	    secret = TRUE;
	} else if (enable_password[level].pw) {
	    password = &enable_password[level];
	}
	break;
    default:
	break;
    }

    if (password && password->pw &&
	getpass(password->pw, outpass, password->encrypt_type)) {
	pass = outpass;
    }

    if (!secret && !pass) {
	if (aaa_authen_debug) {
	    buginf("\nAAA/AUTHEN (%u): can't find any passwords",
		   astruct->session_id);
	}
	error = TRUE;
    }

    if (!error) {
	switch (action) {
	case AAA_AUTHEN_ACT_CHPASS:
	    astruct->status = AAA_AUTHEN_STATUS_ERROR;
	    if (aaa_authen_debug) {
		buginf("\nAAA/AUTHEN (%u): CHPASS can't change fixed "
		       "passwords", astruct->session_id);
	    }
	    error = TRUE;
	    break;
	case AAA_AUTHEN_ACT_SENDAUTH:
	    if (aaa_authen_debug) {
		buginf("\nAAA/AUTHEN (%u): Invalid SENDAUTH (fixed_pass).",
		       astruct->session_id);
	    }
	    error = TRUE;
	    break;
	case AAA_AUTHEN_ACT_LOGIN:
	    switch (ustruct->authen_type) {
	    case AAA_AUTHEN_TYPE_PAP:
	    case AAA_AUTHEN_TYPE_CHAP:
	    case AAA_AUTHEN_TYPE_NONE:
		if (aaa_authen_debug) {
		    buginf("\nAAA/AUTHEN (%u): Invalid LOGIN (fixed_pass).",
			   astruct->session_id);
		}
		error = TRUE;
		break;
	    case AAA_AUTHEN_TYPE_ARAP:
		if (secret) {
		    error = TRUE;	/* Should never happen. */
		} else if (pass && aaa_arap_check(astruct, pass))
		    astruct->status = AAA_AUTHEN_STATUS_PASS;
		else
		    astruct->status = AAA_AUTHEN_STATUS_FAIL;
		break;
	    case AAA_AUTHEN_TYPE_ASCII:
		setstring(&ustruct->user, NULL);
		astruct->status = AAA_AUTHEN_STATUS_GETPASS;
		astruct->flags = AAA_RESP_FLAG_NOECHO;
		setstring_named(&astruct->server_msg, password_prompt,
				"AAA PROMPT P4");
		if (!astruct->server_msg)
		    error = TRUE;
		break;
	    }
	    break;
	}
    }

    if (error) {
	astruct->status = AAA_AUTHEN_STATUS_ERROR;
	return(FALSE);
    }
    astruct->seq_num++;
    return(TRUE);
}


static boolean
fixedpass_cont_login (userstruct *ustruct, authenstruct *astruct)
{
    char *secret = NULL;
    int  level;
    boolean ret;
    password_struct *password = NULL;

    setstring(&astruct->server_msg, NULL);

    if ((astruct->action == AAA_AUTHEN_ACT_CHPASS) ||
	(astruct->action == AAA_AUTHEN_ACT_SENDAUTH)) {
	if (aaa_authen_debug) {
	    buginf("\nAAA/AUTHEN (%u): illegal action %d "
		   "for fixedpass_cont_login",
		   astruct->session_id, astruct->action);
	}
	astruct->status = AAA_AUTHEN_STATUS_ERROR;
	return(FALSE);
    }
    
    if (astruct->status != AAA_AUTHEN_STATUS_GETPASS) {
	buginf("\n%%AAA (%u): Internal state error \"fixedpass_cont\".",
	       astruct->session_id);
	astruct->status = AAA_AUTHEN_STATUS_ERROR;
	return(FALSE);
    }

    switch (ustruct->acc_method) {
    case AAA_ACC_METH_LINE:
	if (!astruct->tty) {
	    if (aaa_authen_debug) {
		buginf("\nAAA/AUTHEN (%u): no tty for line password",
		       astruct->session_id);
	    }
	    astruct->status = AAA_AUTHEN_STATUS_ERROR;
	    return(FALSE);
	}
	password = astruct->tty->password;
	break;
    case AAA_ACC_METH_ENABLE:
	if (ustruct && (ustruct->service == AAA_SVC_ENABLE)) {
	    level = ustruct->priv_lvl;
	} else {
	    level = PRIV_MAX;
	}
	secret = enable_secret[level].pw;
	if (!secret) {
	    password = &enable_password[level];
	}
	break;
    default:
	if (aaa_authen_debug) {
	    buginf("\nAAA/AUTHEN (%u): illegal method %d", 
		   astruct->session_id, ustruct->acc_method);
	}
	astruct->status = AAA_AUTHEN_STATUS_ERROR;
	return(FALSE);
    }

    if (secret) {
	ret = md5_crypt_compare(secret, astruct->user_reply, ENCRYPT_NONE);
    } else {
	if (password) {
	    ret = passwdcheck(astruct->user_reply, password->pw,
			      password->encrypt_type);
	} else {
	    ret = passwdcheck(astruct->user_reply, NULL, ENCRYPT_NONE);
	}
    }
    if (ret) {
	astruct->status = AAA_AUTHEN_STATUS_PASS;
	setstring(&astruct->server_msg, NULL);
    } else {
	if (aaa_authen_debug) {
	    buginf("\nAAA/AUTHEN (%u): password incorrect",
		   astruct->session_id);
	}
	astruct->status = AAA_AUTHEN_STATUS_FAIL;
	setstring_named(&astruct->server_msg, auth_failed, "AAA FAIL 1");
	if (!astruct->server_msg) {
	    astruct->status = AAA_AUTHEN_STATUS_ERROR;
	    return(FALSE);
	}
    }
    
    astruct->seq_num++;		/* once on the way out */
    return(TRUE);
}



static boolean
localuser_start_login (userstruct *ustruct, authenstruct *astruct, int action,
		       enum AAA_SERVICES service)
{
    usernametype *info = NULL;
    char passwd[PASSWORDLEN+1], *passp;
    boolean ret = FALSE;

    switch (action) {
    default:
	buginf("\n%%TAC+: illegal authen action %d for localuser_start_login",
	       action);
	break;

    case AAA_AUTHEN_ACT_CHPASS:
	return(FALSE);		/* Can't change local passwords yet */
	break;
    case AAA_AUTHEN_ACT_SENDAUTH:
	/*
	 * Close the outbound PAP security hole by refusing to provide
	 * an address from the local database.
	 */
	if (ustruct->authen_type == AAA_AUTHEN_TYPE_PAP)
	    return(FALSE);	/* Allow alternate method */
	if (!find_passwd(ustruct->user, &passwd[0])) {
	    if (aaa_authen_debug) {
		buginf("\nAAA/AUTHEN (%u): SENDAUTH no password for %s",
		       astruct->session_id, ustruct->user);
	    }
	    astruct->status = AAA_AUTHEN_STATUS_FAIL;
	    setstring_named(&astruct->server_msg, auth_failed,
			    "AAA FAIL 2");
	} else {
	    switch (ustruct->authen_type) {
	    case AAA_AUTHEN_TYPE_PAP:
		astruct->server_data = strdup_named(&passwd[0], "AAA 3");
		if (astruct->server_data) {
		    ret = TRUE;
		    astruct->server_data_len = strlen(passwd); /* No NULL */
		}
		break;
	    case AAA_AUTHEN_TYPE_CHAP:
		ret = aaa_chap_sendauth(astruct, passwd);
		break;
	    case AAA_AUTHEN_TYPE_ARAP:
	    case AAA_AUTHEN_TYPE_ASCII:
	    case AAA_AUTHEN_TYPE_NONE:
		ret = FALSE;
		break;
	    }

	    setstring(&astruct->server_msg, NULL);
	    if (!ret) {
		astruct->status = AAA_AUTHEN_STATUS_ERROR;
		return(FALSE);
	    }
	    astruct->status = AAA_AUTHEN_STATUS_PASS;
	}
	break;
    case AAA_AUTHEN_ACT_LOGIN:
	if (!ustruct->user) {
	    /* If we don't have a username, then request it. */
	    astruct->status = AAA_AUTHEN_STATUS_GETUSER;
	    setstring_named(&astruct->server_msg, username_prompt,
			    "AAA PROMPT U4");
	    if (!astruct->server_msg) {
		astruct->status = AAA_AUTHEN_STATUS_ERROR;
		return(FALSE);
	    }
	    break;	/* Done with LOGIN processing. */
	}

	/* Already know user so get password if needed */
	info = (usernametype *) find_namedthing(&userQ, ustruct->user);
	if (!info) {
	    /* Couldn't find the user */
	    astruct->status = AAA_AUTHEN_STATUS_FAIL;
	    setstring(&astruct->server_msg, NULL);
	    break;
	}

	if (info->flags & USER_ENCRYPTION) {
	    decrypt(info->password, passwd, PASSWORDLEN);
	    passp = passwd;
	} else {
	    passp = info->password;
	}

	switch (ustruct->authen_type) {
	case AAA_AUTHEN_TYPE_PAP:
	    if (info &&	((info->flags & USER_NOPASSWORD) ||
			 aaa_pap_check(astruct, passp))) {
                astruct->status = AAA_AUTHEN_STATUS_PASS;
		setstring(&astruct->server_msg, NULL);
	    } else {
		astruct->status = AAA_AUTHEN_STATUS_FAIL;
		setstring(&astruct->server_msg, NULL);
	    }
	    break;
	case AAA_AUTHEN_TYPE_CHAP:
	    if (info && ((info->flags & USER_NOPASSWORD) ||
			 aaa_chap_check(astruct, passp))) {
                astruct->status = AAA_AUTHEN_STATUS_PASS;
		setstring(&astruct->server_msg, NULL);
	    } else {
		astruct->status = AAA_AUTHEN_STATUS_FAIL;
		setstring_named(&astruct->server_msg, auth_failed,
				"AAA FAIL 3");
	    }
	    break;
	case AAA_AUTHEN_TYPE_ARAP:
	    if (info && aaa_arap_check(astruct, passp)) {
                astruct->status = AAA_AUTHEN_STATUS_PASS;
		setstring(&astruct->server_msg, NULL);
	    } else {
		astruct->status = AAA_AUTHEN_STATUS_FAIL;
		setstring_named(&astruct->server_msg, auth_failed,
				"AAA FAIL 4");
	    }
	    break;
	case AAA_AUTHEN_TYPE_ASCII:
	    if (info && (info->flags & USER_NOPASSWORD)) {
		astruct->status = AAA_AUTHEN_STATUS_PASS;
		setstring(&astruct->server_msg, NULL);
	    } else {
		astruct->status = AAA_AUTHEN_STATUS_GETPASS;
		setstring_named(&astruct->server_msg, password_prompt,
				"AAA PROMPT P5");
		if (!astruct->server_msg) {
		    astruct->status = AAA_AUTHEN_STATUS_ERROR;
		    return(FALSE);
		}
		astruct->flags = AAA_RESP_FLAG_NOECHO;
	    }
	    break;
	case AAA_AUTHEN_TYPE_NONE:
	    astruct->status = AAA_AUTHEN_STATUS_ERROR;
	    return(FALSE);
	    break;
	default:
	    buginf("\n%%TAC+: illegal authen type %d "
		   "for localuser_start_login", ustruct->authen_type);
	    break;
	}
	break;
    }
    astruct->seq_num++;
    return(TRUE);
}


static boolean
localuser_cont_login (userstruct *ustruct, authenstruct *astruct)
{
    usernametype *info = NULL;

    setstring(&astruct->server_msg, NULL);

    switch (astruct->action) {
    case AAA_AUTHEN_ACT_SENDPASS:
	/* Deprecated */
	/* FALLTHRU */
    case AAA_AUTHEN_ACT_CHPASS:
	/* Not implemented yet. */
	astruct->status = AAA_AUTHEN_STATUS_ERROR;
	return(FALSE);
	break;
    case AAA_AUTHEN_ACT_SENDAUTH:
	/* We should never get here for a SENDAUTH */
	astruct->status = AAA_AUTHEN_STATUS_ERROR;
	return(FALSE);
	break;
    case AAA_AUTHEN_ACT_LOGIN:
	switch (astruct->status) {
	case AAA_AUTHEN_STATUS_GETUSER:
	    /* check that we actually got a username. NULL username restarts
	     * login sequence
	     */
	    if (null(ustruct->user)) {
		if (aaa_authen_debug) {
		    buginf("\nAAA/AUTHEN/LOCAL (%d): no username: "
			   "GETUSER", astruct->session_id);
		}
		setstring_named(&astruct->server_msg, username_prompt,
				"AAA PROMPT U5");
		if (!astruct->server_msg) {
		    astruct->status = AAA_AUTHEN_STATUS_ERROR;
		    return(FALSE);
		}
		astruct->seq_num = 0;
		return(TRUE);
	    }

	    info = (usernametype *) find_namedthing(&userQ, ustruct->user);
	    if (info && (info->flags & USER_NOPASSWORD)) {
		astruct->status = AAA_AUTHEN_STATUS_PASS;
		setstring(&astruct->server_msg, NULL);
	    } else {
		astruct->status = AAA_AUTHEN_STATUS_GETPASS;
		setstring_named(&astruct->server_msg, password_prompt,
				"AAA PROMPT P6");
		if (!astruct->server_msg) {
		    astruct->status = AAA_AUTHEN_STATUS_ERROR;
		    return(FALSE);
		}
		astruct->flags = AAA_RESP_FLAG_NOECHO;
	    }
	    break;
	case AAA_AUTHEN_STATUS_GETPASS:
	    info = (usernametype *) find_namedthing(&userQ, ustruct->user);
	    if (info && passwdcheck(ustruct->password, info->password, 
				    info->flags & USER_ENCRYPTION)) {
		astruct->status = AAA_AUTHEN_STATUS_PASS;
		setstring(&astruct->server_msg, NULL);
	    } else {
		if (aaa_authen_debug) {
		    buginf("\nAAA/AUTHEN (%u): password incorrect",
			   astruct->session_id);
		}
		astruct->status = AAA_AUTHEN_STATUS_FAIL;
		setstring_named(&astruct->server_msg, auth_failed,
				"AAA FAIL 5");
	    }
	    break;
	default:
	    astruct->status = AAA_AUTHEN_STATUS_ERROR;
	    setstring_named(&astruct->server_msg, auth_failed,
			    "AAA FAIL 6");
	    return(FALSE);
	    break;
	}
	break;
    }

    astruct->seq_num++;		/* once on way out */
    return(TRUE);
}

static char *
debug_authen_type (enum AUTHEN_TYPE type)
{
    switch(type) {
    default: 
	return("UNKNOWN");
    case AAA_AUTHEN_TYPE_NONE:
	return("NONE");
    case AAA_AUTHEN_TYPE_ASCII: 
	return("ASCII");
    case AAA_AUTHEN_TYPE_PAP: 
	return("PAP");
    case AAA_AUTHEN_TYPE_CHAP:
	return("CHAP");
    case AAA_AUTHEN_TYPE_ARAP:
	return("ARAP");
    }
}

static char *
debug_authen_svc (enum AAA_SERVICES type)
{
    switch(type) {
    default:             return("UNKNOWN");
    case AAA_SVC_NONE:   return("NONE");
    case AAA_SVC_LOGIN:  return("LOGIN");
    case AAA_SVC_ENABLE: return("ENABLE");
    case AAA_SVC_PPP:    return("PPP");
    case AAA_SVC_ARAP:   return("ARAP");
    case AAA_SVC_RCMD:   return("RCMD");
    case AAA_SVC_X25:    return("X25");
    case AAA_SVC_NASI:   return("NASI");
    }
}

boolean
aaa_create_user (userstruct **ustruct, char *user, char *ruser, char *port,
		 char *rem_addr, 
		 enum AUTHEN_TYPE authen_type, 
		 enum AAA_SERVICES service,
		 int priv_lvl)
{
    userstruct *ustr;

    if (priv_lvl < PRIV_MIN || priv_lvl > PRIV_MAX) {
	if (aaa_authen_debug) {
	    buginf("\nAAA/AUTHEN: create_user: bad privilege level %d",
		   priv_lvl);
	}
	return(FALSE);
    }
    
    ustr = malloc_named(sizeof(userstruct), "AAA new ustruct");
    if (!ustr) {
	*ustruct = NULL;
	return(FALSE);
    }
    aaa_user_create++;

    /* strdup() checks for NULL */
    ustr->user = strdup_named(user, "AAA 11");
    ustr->ruser = strdup_named(ruser, "AAA 12");
    ustr->port = strdup_named(port, "AAA 13");
    ustr->rem_addr = strdup_named(rem_addr, "AAA 14");

    ustr->authen_type = authen_type;
    ustr->service = service;
    ustr->priv_lvl = priv_lvl;
    ustr->acc_method = AAA_ACC_METH_NONE;
    ustr->cred = NULL;

    *ustruct = ustr;

    if (aaa_authen_debug) {
	buginf("\nAAA/AUTHEN: create_user user='%s' ruser='%s' port='%s' "
	       "rem_addr='%s' authen_type=%s service=%s priv=%d",
	       ustr->user, ustr->ruser, ustr->port, 
	       ustr->rem_addr, debug_authen_type(ustr->authen_type), 
	       debug_authen_svc(ustr->service),
	       ustr->priv_lvl);
    }

    return(TRUE);
}

/*
 * aaa_dup_user
 * This is normally used when we wish to possibly modify a user struct without
 * destroying the old structure (in case login fails, for example).  Thus we
 * actually make a copy of the original that can be modified if necessary.
 */
boolean
aaa_dup_user (userstruct **ustruct1, userstruct **ustruct2,
	      enum AAA_SERVICES new_service, int priv_lvl)
{
    userstruct *ustr;

    if (*ustruct1 == NULL) {	/* Can happen when we switch from old to */
	*ustruct2 = NULL;	/*  new authentication methods */
	return(FALSE);
    }

    ustr = malloc_named(sizeof(userstruct), "AAA dup ustruct");
    if (!ustr) {
	*ustruct2 = NULL;
	return(FALSE);
    }
    aaa_user_create++;

    bcopy(*ustruct1, ustr, sizeof(userstruct));

    /* strdup() checks for NULL */
    ustr->user = strdup_named((*ustruct1)->user, "AAA 15");
    ustr->ruser = strdup_named((*ustruct1)->ruser, "AAA 16");
    ustr->port = strdup_named((*ustruct1)->port, "AAA 17");
    ustr->rem_addr = strdup_named((*ustruct1)->rem_addr, "AAA 18");
    ustr->password = strdup_named((*ustruct1)->password, "AAA 19");
    if (ustr->savedauthor)
	mem_lock((void *)ustr->savedauthor);

    ustr->service = new_service;
    ustr->priv_lvl = priv_lvl;
    if (ustr->cred) {
	ustr->cred->ref_cnt++;
    }
    ustr->no_acct = FALSE;

    queue_init(&ustr->acctQ, 0); /* New user has no accounting yet */

    if (aaa_authen_debug) {
	buginf("\nAAA/AUTHEN: dup user='%s' ruser='%s' port='%s' "
	       "rem_addr='%s' authen_type=%s service=%s priv=%d",
	       ustr->user, ustr->ruser, ustr->port, 
	       ustr->rem_addr, debug_authen_type(ustr->authen_type), 
	       debug_authen_svc(ustr->service),
	       ustr->priv_lvl);
    }

    *ustruct2 = ustr;
    return(TRUE);
}

/*
 * aaa_lock_user
 *
 * Our equivalent of memlock()
 *
 */
void
aaa_lock_user (userstruct *ustruct)
{
    if (!ustruct) {
	return;
    }

    ustruct->sharecount++;
}

/*
 * aaa_unlock_user
 *
 * Used to unlock a locked user-structure. Free's if necessary
 *
 * Use this function, if no other action, other than the unlock,
 * is desired. A aaa_free_user() tries to send all pending accounting
 * records from the user->acctQ before decrementing the sharecount,
 * and possibly freeing the ustruct.
 *
 * See comment before aaa_free_user().
 */
void
aaa_unlock_user(userstruct **ustruct)
{
    if (!ustruct || !*ustruct) {
    	return;
    }

    (*ustruct)->sharecount--;

    if ((*ustruct)->sharecount < 0) {
	aaa_free_user(ustruct);
    }
}

/*
 * aaa_free_user
 *
 * check the sharecount, and free the memory only if the sharecount
 * drops below 0
 *
 * Do NOT use this function, if the desired effect is only to unlock
 * the ustruct, as this function processes accounting records before
 * checking the sharecount. If unlock is desired, please use the
 * function by that name.
 *
 * See comment before aaa_unlock_user()
 */
void
aaa_free_user (userstruct **ustruct)
{
    userstruct *ustr;

    if (!ustruct || !*ustruct) {
	return;
    }
    ustr = *ustruct;
    *ustruct = NULL;

    reg_invoke_aaa_acct_cleanup(ustr);

    ustr->sharecount--;
    if (ustr->sharecount < 0) { /* Don't actually free if still in use !*/
	if (ustr->acctQ.qhead)
	    aaa_user_acctleft++;
#if 0
	    buginf("\nFree_user %s (%lx) with accounting left!", ustr->user,
		   ustr);
	if (aaa_authen_debug)
	    buginf("\nAAA: Freeing user %s structure at %0#x", ustr->user, ustr);
#endif
	if (aaa_authen_debug) {
	    buginf("\nAAA/AUTHEN: free user %s %s %s %s "
		   "authen_type=%s service=%s priv=%d",
		   ustr->user, ustr->ruser, ustr->port, ustr->rem_addr,
		   debug_authen_type(ustr->authen_type), 
		   debug_authen_svc(ustr->service), 
		   ustr->priv_lvl);
	}
	
	aaa_user_frees++;
	/* free() checks for NULL */
	free(ustr->user);
	free(ustr->ruser);
	free(ustr->port);
	free(ustr->rem_addr);
	free((void *)ustr->savedauthor);
	if (ustr->cred) {
	    if (ustr->cred->ref_cnt > 1)
		ustr->cred->ref_cnt--;
	    else {
		(*ustr->cred->free_funct)(&ustr->cred->cred);
		free(ustr->cred);
		ustr->cred = NULL;
	    }
	}
	free(ustr->password);
	free(ustr);
    }
}


boolean
aaa_get_user_p (userstruct *ustruct, char **user)
{
    if (!ustruct)
	return(FALSE);

    if ((ustruct->acc_method == AAA_ACC_METH_NOT_SET) ||
	(ustruct->acc_method == AAA_ACC_METH_NONE))
	return(FALSE);

    if (!ustruct->user)
	return(FALSE);

    if (user)
	*user = ustruct->user;
    return(TRUE);
}

boolean
aaa_get_cred (userstruct *ustruct, char **user, enum AUTHEN_TYPE *authen_type,
	      enum ACC_METHOD *acc_method, void **cred, char **password)
{
    if (!ustruct)
	return(FALSE);

    if ((ustruct->acc_method == AAA_ACC_METH_NOT_SET) ||
	(ustruct->acc_method == AAA_ACC_METH_NONE))
	return(FALSE);

    if (user)
	*user = strdup_named(ustruct->user, "AAA 20");
    if (authen_type)
	*authen_type = ustruct->authen_type;
    if (acc_method)
	*acc_method = ustruct->acc_method;
    if (cred)
	*cred = ustruct->cred->cred;
    if (password)
	*password = strdup_named(ustruct->password, "AAA 21");

    return(TRUE);
}


boolean
aaa_authen_first_method (enum AAA_SERVICES service, char *list_name,
			 enum ACC_METHOD meth)
{
    int max_num, i, *method_list = NULL;
    struct access_method_list *list;
    char *name;


    list = aaa_pick_accmethod_list(service, &max_num);

    name = (list_name && *list_name) ? list_name : ACC_LIST_DEFAULT;
    for (i = 0; i < max_num; i++) {     /* First try to find named list */
	if (list[i].valid && !strcmp(list[i].name, name)) {
	    method_list = &(list[i].methods[0]);
	    break;
	}
    }

    if (!method_list || (method_list[0] != meth))
	return(FALSE);

    return(TRUE);
}

/*
 * aaa_dup_ifneeded
 * Here we are propagating our user info to other locations (eg to an idb
 * from a tty that is already logged in.)  Here we just increment the share
 * count of the userstruct and have two pointers.
 */
boolean
aaa_dup_ifneeded (tt_soc *tty, char *acc_list_name, userstruct **ustruct,
		  enum AAA_SERVICES service)
{
    if (!aaa_authen_first_method(service, acc_list_name,
				 AAA_ACC_METH_IFNEEDED))
	return(FALSE);

    if (!tty) {
	return(FALSE);
    }

    if (!aaa_get_user_p(tty->ustruct, NULL))
	return(FALSE);

    if (aaa_acct_nested) {
	aaa_lock_user(tty->ustruct);
	*ustruct = tty->ustruct;
    } else {
	/*
	 * The logical thing to do now would be to lock the tty->ustruct and 
	 * copy the pointer to lcpstate->ustruct.  However, CSCdi77152
	 * frees the tty->ustruct and copies the lcpstate->ustruct to 
	 * tty->ustruct after PPP authentication.  This causes accounting 
	 * records to be generated in the following sequence:  
	 * EXEC-START/EXEC-STOP/NET-START/NET-STOP.  So we need to be consistent
	 * here.  We will dup the tty->ustruct and free it.  This generates the
	 * EXEC-STOP before the NET-START.  Then we copy our dup'd ustruct into
	 * tty->ustruct and lcpstate->ustruct.(Yes I know this is ugly)
	 */

	if (!aaa_dup_user(&tty->ustruct, ustruct, tty->ustruct->service,
	    tty->ustruct->priv_lvl)) {

	    return(FALSE);
	}
	aaa_free_user(&tty->ustruct);
	aaa_lock_user(*ustruct);
	tty->ustruct = *ustruct;
    }

    return(TRUE);
}



static void
debug_authen_status (authenstruct *astruct)
{
    char *s;

    if (!aaa_authen_debug) {	
	return;
    }

    switch(astruct->status) {
    case AAA_AUTHEN_STATUS_PASS:
	s = "PASS";
	break;
    case AAA_AUTHEN_STATUS_FAIL:
	s = "FAIL";
	break;
    case AAA_AUTHEN_STATUS_GETDATA:
	s = "GETDATA";
	break;
    case AAA_AUTHEN_STATUS_GETUSER:
	s = "GETUSER";
	break;
    case AAA_AUTHEN_STATUS_GETPASS:
	s = "GETPASS";
	break;
    case AAA_AUTHEN_STATUS_RESTART:
	s = "RESTART";
	break;
    case AAA_AUTHEN_STATUS_ERROR:
	s = "ERROR";
	break;
    default:
	s = "UNKNOWN";
	break;
    }

    buginf("\nAAA/AUTHEN (%u): status = %s", 
	   astruct->session_id, s);
}

static void
debug_start_login (userstruct *ustruct, authenstruct *astruct,
		   char *acc_list_name,
		   enum AUTH_ACTIONS action, enum AAA_SERVICES service)
{
    char *u = "UNKNOWN";
    char *a, *s;

    if (!aaa_authen_debug)
	return;

    switch(action) {
    case AAA_AUTHEN_ACT_LOGIN:
	a = "LOGIN";
	break;
    case AAA_AUTHEN_ACT_CHPASS:
	a = "CHPASS";
	break;
    case AAA_AUTHEN_ACT_SENDAUTH:
	a = "SENDAUTH";
	break;
    default:
	a = u;
	break;
    }

    switch(service) {
    case AAA_SVC_NONE:
 	s = "NONE";
	break;
    case AAA_SVC_LOGIN:
	s = "LOGIN";
	break;
    case AAA_SVC_ENABLE:
	s = "ENABLE";
	break;
    case AAA_SVC_PPP:
	s = "PPP";
	break;
    case AAA_SVC_ARAP:
	s = "ARAP";
	break;
    case AAA_SVC_PT:
	s = "PT";
	break;
    case AAA_SVC_RCMD:
	s = "RCMD";
	break;
    case AAA_SVC_X25:
	s = "X25";
	break;
    case AAA_SVC_NASI:
	s = "NASI";
	break;
    default:
	s = u;
	break;
    }

    buginf("\nAAA/AUTHEN/START (%u): port='%s' list='%s' action=%s service=%s",
	   astruct->session_id,
	   ustruct ? ustruct->port : "unknown",
	   acc_list_name, a, s);
}

/*
 * Fill in the session id with a random value, courtesy of random-fill
 */
void 
aaa_randomize_id (int *session_idp)
{
    random_fill((uchar *)session_idp, sizeof(int), TRUE);
}


/*
 * aaa_pick_accmethod_list
 * Pick a list of method lists and set the max number of method lists in
 * the list, based on the service.
 */

static struct access_method_list *
aaa_pick_accmethod_list (enum AAA_SERVICES service, int *max_num)
{
    switch (service) {
    case AAA_SVC_X25:
    case AAA_SVC_PT:
    case AAA_SVC_RCMD:
    case AAA_SVC_LOGIN:
	*max_num = MAX_ACC_LISTS_LOGIN;
	return(&login_access_methods[0]);
    case AAA_SVC_PPP:
	*max_num = MAX_ACC_LISTS_PPP;
	return(&ppp_access_methods[0]);
    case AAA_SVC_ARAP:
	*max_num = MAX_ACC_LISTS_ARAP;
	return(&arap_access_methods[0]);
    case AAA_SVC_ENABLE:
	*max_num = MAX_ACC_LISTS_ENABLE;
	return(&enable_access_methods[0]);
    case AAA_SVC_NASI:
	*max_num = MAX_ACC_LISTS_NASI;
	return(&nasi_access_methods[0]);
    default:
	buginf("\n%% AAA/AUTHEN: Bad service %d.", service);
	*max_num = 0;
	return(NULL);
    }
}

/*
 * aaa_pick_authen_method
 * Pick an authentication method to try for this user/action/service.
 * Sets astruct->methods to an int point to the methods themselves
 * (incidentally losing (or hiding) the reference to the other information
 *  about the list being used.)
 * Include a mechanism for picking the "next" method in the case of an
 * error in the current method by passing in acc_list_name == NULL.  Note
 * that it is an error on the callers part if both acc_list_name AND
 * astruct->methods are null, but we don't report this unlikely occurance.
 */
static void
aaa_pick_authen_method (char * acc_list_name, enum AAA_SERVICES service,
	authenstruct *astruct, userstruct *ustruct)
{
    struct access_method_list *accmethod_list;
    int i, max_num;

    /*
     * Mechanism for stepping to the next authentication method in the
     * case of a (late) error in the current method.  Notice this by
     * looking for a NULL list name, and handle it by incrementing
     * the methods pointer (which BETTER exist in this case!)
     */
    if (acc_list_name == NULL && astruct->methods != NULL) {
	astruct->methods += 1;	/* Step to next method */
	if (aaa_authen_debug)
	    buginf("\nAAA/AUTHEN/START (%d): Restart",
		   astruct->session_id);
	return;
    }

    astruct->methods = NULL;

    /*
     * First, choose our list of access methods
     */
    accmethod_list = aaa_pick_accmethod_list(service, &max_num);
    if (!accmethod_list ||  (acc_list_name == NULL)) {
	astruct->status = AAA_AUTHEN_STATUS_ERROR;
	return;
    }

    for (i = 0; i < max_num; i++) {	/* First try to find named list */
	if (accmethod_list[i].valid && !strcmp(accmethod_list[i].name,
					     acc_list_name)) {
	    astruct->methods = &(accmethod_list[i].methods[0]);
	    if (aaa_authen_debug) {
		buginf("\nAAA/AUTHEN/START (%d): found list %s", 
		       acc_list_name, astruct->session_id);
	    }
	    return;
	}
    }

    /*
     * If we can't find named list, try to find default list.
     */
    for (i = 0; i < max_num; i++) {
	if (accmethod_list[i].valid && !strcmp(accmethod_list[i].name,
					       ACC_LIST_DEFAULT)) {
	    astruct->methods = &(accmethod_list[i].methods[0]);
	    if (aaa_authen_debug) {
		buginf("\nAAA/AUTHEN/START (%d): using \"default\" list", 
		       astruct->session_id);
	    }
	    return;
	}
    }
    /*
     * If no authentication list is specified, and no default list 
     * exists, then we pick a default action
     *   if on console and this is a LOGIN 
     *      then no authentication is required
     *   else if on console and this is a ENABLE
     *      then use enable pass if set else no authentication is required
     *   else if this is an ENABLE
     *      then use enable password only
     *   else
     *      use LOCAL db only
     */
    if (astruct->tty && (astruct->tty->ttynum == startup_ttynum) &&
	((service == AAA_SVC_LOGIN) || (service == AAA_SVC_ENABLE))) {
	if (service == AAA_SVC_ENABLE) {
	    astruct->methods = con_e_noauthset_meth;
	    if (aaa_authen_debug) {
		buginf("\nAAA/AUTHEN/START (%d): console enable"
		       " - default to enable password (if any)",
		       astruct->session_id);
	    }
	} else { /* AAA_SVC_LOGIN */
	    astruct->methods = con_noauthset_method;
	    if (aaa_authen_debug) {
		buginf("\nAAA/AUTHEN/START (%d): console login"
		       " - default to \"no auth required\"",
		       astruct->session_id);
	    }
	}
    } else {
	if (service == AAA_SVC_ENABLE) {
	    astruct->methods = ena_noauthset_method;
	    if (aaa_authen_debug) {
		buginf("\nAAA/AUTHEN/START (%d): non-console "
		       "enable - default to enable password",
		       astruct->session_id);
	    }
	} else {
	    astruct->methods= loc_noauthset_method;
	    if (aaa_authen_debug) {
		buginf("\nAAA/AUTHEN/START (%d): non console login "
		       "- defaults to local database", 
		       astruct->session_id);
	    }
	}
    }
}

/* aaa_start_login: Try and service a login. All new-model logins come
   through here.

   Inputs: ustruct, astruct, service, 
       acc_list_name  -- the name of a configured login list or NULL,
       data -- Data for the start packet.
   Outputs: sets status in astruct.  Sets methods in astruct.

   Returns false if no authentication mechanism can be divined, or 
   if we failed to authenticate.
 */

boolean
aaa_start_login (userstruct *ustruct, 
		 authenstruct *astruct,
		 char *acc_list_name, 
		 enum AUTH_ACTIONS action, 
		 enum AAA_SERVICES service)
{
    int i;
    boolean done;

    debug_start_login(ustruct, astruct, acc_list_name, action, service);

    if (!ustruct) {
	astruct->status = AAA_AUTHEN_STATUS_ERROR;
	if (aaa_authen_debug) {
	    buginf("\nAAA/AUTHEN/START (%u): ERROR (no ustruct)",
		   astruct->session_id);
	}
	return(FALSE);
    }

    /*
     * OK first some basic sanity checking for bogus combos.
     */
    if (((action == AAA_AUTHEN_ACT_CHPASS) &&
	 ((ustruct->authen_type == AAA_AUTHEN_TYPE_CHAP) ||
	  (ustruct->authen_type == AAA_AUTHEN_TYPE_PAP)))  ||
	((action == AAA_AUTHEN_ACT_SENDAUTH) &&
	 ((ustruct->authen_type == AAA_AUTHEN_TYPE_ASCII) ||
	  (ustruct->authen_type == AAA_AUTHEN_TYPE_ARAP)))) {
	if (aaa_authen_debug) {
	    buginf("\nAAA/AUTHEN/START (%u): bad action/authen_type combo.",
		   astruct->session_id);
	}
	astruct->status = AAA_AUTHEN_STATUS_ERROR;
	return(FALSE);
    }

    aaa_pick_authen_method(acc_list_name, service, astruct, ustruct);
    if (astruct->methods == NULL) {
	astruct->status = AAA_AUTHEN_STATUS_ERROR;
	return(FALSE);
    }

    /*
     * If Local Override is set and local account exists, then use it.
     * But not if action is SENDPASS and there is no local password.
     * (don't check for local username if no authentication is in use!)
     * Also, don't use Local Override if we have already authenticated
     * via Kerberos credentials.
     */
    if (local_override && (service != AAA_SVC_ENABLE) &&
	(*astruct->methods != AAA_ACC_METH_NONE) && 
	(ustruct->acc_method != AAA_ACC_METH_KRB5_TELNET)) {
	usernametype *linfo;
	/*
	 * first ensure that we have a local username.
	 */
	if (null(ustruct->user)) {
	    astruct->status = AAA_AUTHEN_STATUS_GETUSER;
	    setstring_named(&astruct->server_msg, username_prompt,
			    "AAA PROMPT U6");
	    if (!astruct->server_msg) {
		astruct->status = AAA_AUTHEN_STATUS_ERROR;
		return(FALSE);
	    }
	    ustruct->acc_method = AAA_ACC_METH_NOT_SET;
	    astruct->seq_num = 0;
	    /* Save  values so cont_login() can call start_login() again. */
	    astruct->opaque = strdup_named(acc_list_name, "AAA 23");
	    astruct->action = action;
	    ustruct->service = service;
	    if (aaa_authen_debug) {
		buginf("\nAAA/AUTHEN/START (%d): local override set: "
		       "GETUSER", astruct->session_id);
	    }
	    return(TRUE);
	}
	linfo = (usernametype *)find_namedthing(&userQ, ustruct->user);
	if (linfo && !((action == AAA_AUTHEN_ACT_SENDPASS) &&
		       (linfo->flags & USER_NOPASSWORD))) {
	    astruct->methods = localauth_method;
	    if (aaa_authen_debug) {
		buginf("\nAAA/AUTHEN/START (%d): local override set: "
		       "local account=%s", 
		       astruct->session_id, ustruct->user);
	    }
	}
    }

    astruct->seq_num = 1;	/* Set starting sequence number */
    aaa_randomize_id(&astruct->session_id);

    done = FALSE;
    /*
     * MAX_ACC_METHS in this case provides a sanity check, even though the
     * actual list of methods may be shorter than this either because it
     * started out that way, or because the methods pointer has been
     * incremented in the case of a late error (to step to next method!)
     */
    for (i = 0; i < MAX_ACC_METHS; i++) {
	switch (astruct->methods[i]) {
	case AAA_ACC_METH_NOT_SET:
	    if (aaa_authen_debug) {
		buginf("\nAAA/AUTHEN/START (%u): no methods left to try",
		       astruct->session_id);
	    }
	    done = TRUE;
	    astruct->status = AAA_AUTHEN_STATUS_ERROR;
	    break;
	case AAA_ACC_METH_NONE:
	    if (aaa_authen_debug) {
		buginf("\nAAA/AUTHEN/START (%u): Method=NONE", 
		       astruct->session_id);
	    }
	    if (action == AAA_AUTHEN_ACT_LOGIN) {
		astruct->status = AAA_AUTHEN_STATUS_PASS;
		done = TRUE;
	    }
	    break;
	case AAA_ACC_METH_LINE:
	case AAA_ACC_METH_ENABLE:
	    if (aaa_authen_debug) {
		buginf("\nAAA/AUTHEN/START (%u): Method=%s", 
		       astruct->session_id,
		       (astruct->methods[i] == AAA_ACC_METH_LINE) ? 
		       "LINE" : "ENABLE");
	    }
	    done = fixedpass_start_login(ustruct, astruct, action,
					 astruct->methods[i], service);
	    break;
	case AAA_ACC_METH_LOCAL:
	    if (aaa_authen_debug) {
		buginf("\nAAA/AUTHEN/START (%u): Method=LOCAL", 
		       astruct->session_id);
	    }
	    done = localuser_start_login(ustruct, astruct, action, service);
	    break;
	case AAA_ACC_METH_KRB5:
        case AAA_ACC_METH_RADIUS:
	case AAA_ACC_METH_TACACSPLUS:
	    if (aaa_authen_debug) {
		char *p = name_kerberos;
		if (astruct->methods[i] == AAA_ACC_METH_RADIUS)
		    p = name_radius;
		else if (astruct->methods[i] == AAA_ACC_METH_TACACSPLUS)
		    p = name_tacplus;
		buginf("\nAAA/AUTHEN/START (%u): Method=%s",
		       astruct->session_id, p);
	    }
	    reg_invoke_start_login(astruct->methods[i], &done, ustruct,
				   astruct, action);
	    break;
	case AAA_ACC_METH_KRB5_TELNET:
	    if (aaa_authen_debug) {
		buginf("\nAAA/AUTHEN/START (%u): Method=KRB5-TELNET", 
		       astruct->session_id);
	    }
	    if ((ustruct) && (ustruct->user) 
		&& (ustruct->acc_method == AAA_ACC_METH_KRB5_TELNET)) {
		astruct->status = AAA_AUTHEN_STATUS_PASS;
		done = TRUE;
	    } else {
		astruct->status = AAA_AUTHEN_STATUS_ERROR;
	    }
	    break;
	case AAA_ACC_METH_IFNEEDED:
	case AAA_ACC_METH_GUEST:
	case AAA_ACC_METH_AUTHGUEST:
	    /*
	     * These aren't handled here, so try next method.
	     * if-needed is checked by PPP by calling 
	     * aaa_dup_ifneeded() which pulls in the tty userstruct
	     * if it's valid and if if-needed is configured.
	     */
	    /* FALLTHRU */
	default:
	    done = FALSE;
	    break;
	}

	debug_authen_status(astruct);

	if (done)
	    break;	/* get out before i is incremented. */
    }

    if (done && (astruct->status != AAA_AUTHEN_STATUS_ERROR)) {
	ustruct->acc_method = astruct->methods[i];
	/*
	 * set our list of methods to be pointing to the method we
	 * chose to use.  If it fails, we can easilly step to the next.
	 */
	astruct->methods = &astruct->methods[i];
	ustruct->service = service;
	astruct->action = action;
	return(TRUE);
    }    

    if (aaa_authen_debug) {
	buginf("\nAAA/AUTHEN/START (%u): failed to authenticate",
	       astruct->session_id);
    }

    return(FALSE);	/* No authentication mechanism was found or worked. */
}

/*
 * aaa_cont_login
 * continue a login based on information set up by start_login.  This
 * continuation may include prompting the user for data, or additional
 * protocol level exchanges with the server.  If an authentication error 
 * occurs in here, we go back to start_login to get it to try the next
 * method.  (note that start_login mau be responsible for making an inital
 * connection to the server.
 */
boolean
aaa_cont_login (userstruct *ustruct, authenstruct *astruct)
{
    boolean done = FALSE;

    if (aaa_authen_debug) {
	buginf("\nAAA/AUTHEN/CONT (%u): continue_login",
	       astruct->session_id);
	debug_authen_status(astruct);
    }

    switch (astruct->status) {
    case AAA_AUTHEN_STATUS_GETUSER:
	free(ustruct->user); /* checks for NULL */
	ustruct->user = strdup_named(astruct->user_reply, "AAA 24");
	break;
    case AAA_AUTHEN_STATUS_GETPASS:
	free(ustruct->password); /* checks for NULL */
	ustruct->password = strdup_named(astruct->user_reply, "AAA 25");
	break;
    default:
	break;
    }

    /*
     * Clear any flags in authenstruct.
     */
    astruct->flags = 0;

    /*
     * If local user override is set, and we just got a username, then
     * we really want to call aaa_start_login
     */
    if (local_override && (astruct->status == AAA_AUTHEN_STATUS_GETUSER) &&
	(astruct->seq_num == 0)) {
	char *acc_list;
	acc_list = astruct->opaque;
	astruct->opaque = NULL;

	if (aaa_authen_debug) {
	    buginf("\nAAA/AUTHEN/CONT (%u): local_override. "
		   "Call start_login again", 
		   astruct->session_id);
	}

	done = aaa_start_login(ustruct, astruct, acc_list,
				astruct->action, ustruct->service);
	free(acc_list);
	return(done);
    }

    astruct->seq_num++;		/* Increment sequence number */

    switch (ustruct->acc_method) {
    case AAA_ACC_METH_LINE:
    case AAA_ACC_METH_ENABLE:
	if (aaa_authen_debug) {
	    buginf("\nAAA/AUTHEN/CONT (%u): Method=%s", 
		   astruct->session_id,
		   ustruct->acc_method == AAA_ACC_METH_LINE ? 
		   "LINE" : "ENABLE");
	}
	done = fixedpass_cont_login(ustruct, astruct);
	break;
    case AAA_ACC_METH_LOCAL:
	if (aaa_authen_debug) {
	    buginf("\nAAA/AUTHEN/CONT (%u): Method=LOCAL",
		   astruct->session_id);
	}
	done = localuser_cont_login(ustruct, astruct);
	break;
    case AAA_ACC_METH_KRB5:
    case AAA_ACC_METH_RADIUS:
    case AAA_ACC_METH_TACACSPLUS:
	if (aaa_authen_debug) {
	    char *p = name_kerberos;
	    if (ustruct->acc_method ==  AAA_ACC_METH_RADIUS)
		p = name_radius;
	    else if (ustruct->acc_method == AAA_ACC_METH_TACACSPLUS)
		p = name_tacplus;
	    buginf("\nAAA/AUTHEN (%u): Method=%s",
		   astruct->session_id, p);
	}
	reg_invoke_cont_login(ustruct->acc_method, &done,
					 ustruct, astruct);
	break;
    case AAA_ACC_METH_NOT_SET:	/* Should never call "continue" for NOT_SET */
    case AAA_ACC_METH_IFNEEDED:	/* Should never call "continue" for IFNEEDED */
    case AAA_ACC_METH_NONE:	/* Should never call "continue" for NONE */
    case AAA_ACC_METH_KRB5_TELNET: /* Should never call "continue" for KRB5_TELNET */

    default:
	buginf("\n%% AAA/AUTHEN/CONT: Bad state in aaa_cont_login().");
	done = FALSE;
	break;
    }

    debug_authen_status(astruct);

    /*
     * If we have an error condition in continue_login, we have to go back
     * to start_login to try the next method in our list.
     */
    if (astruct->status == AAA_AUTHEN_STATUS_ERROR) {
	setstring_named(&astruct->server_msg, backup_authen_msg,
			"AAA BACKUP 1");
	return(aaa_start_login(ustruct, astruct, NULL,
				astruct->action, ustruct->service));
    } else if (astruct->status == AAA_AUTHEN_STATUS_GETPASS) {
	/*
	 * The tacacs+ server can potentially ask for multiple passwords
	 * using getpass.  Ensure that if this happens, we zero out the
	 * previous password so that new_ascii_login doesn't think that
	 * it already has the password.
	 */
	free(ustruct->password);
	ustruct->password = NULL;
    }
    return(done);
}


void
aaa_abort_login (userstruct *ustruct, authenstruct *authen, char *reason)
{
    if (aaa_authen_debug)
	buginf("\nAAA/AUTHEN/ABORT: (%u) because %s.", 
	       authen->session_id, reason);

    switch (ustruct->acc_method) {
    case AAA_ACC_METH_RADIUS:
    case AAA_ACC_METH_TACACSPLUS:
	authen->seq_num++;
	reg_invoke_abort_login(ustruct->acc_method,
				   ustruct, authen, reason);
	break;
    default:
	break;
    }
}

void
aaa_cleanup_login (userstruct *ustruct, authenstruct *authen)
{
    /* free checks for NULL */
    free(authen->server_msg);
    free(authen->server_data);
    free(authen->user_reply);
    free(authen->user_data);
    free(authen->opaque);
    if (authen->connection_handle)
	aaa_close_connection(ustruct, &authen->connection_handle);
    memset(authen, 0, sizeof(authenstruct));
}

boolean
aaa_simple_login_check (tt_soc *tty, char *user, char *pw, char *rem_user,
			boolean save_ustruct, enum AAA_SERVICES svc)
{
    userstruct *ustruct;
    authenstruct astruct;
    boolean ret;
    char location[8], *list_name = NULL, *rem_addr;

    memset(&astruct, 0, sizeof(astruct));
    if (tty) {
	astruct.tty = tty;
	sprintf(location, "tty%d", tty->ttynum);
	rem_addr = tty_get_remote_addr(tty);
    } else {
	location[0] = '\0';
	rem_addr = NULL;
    }
    ret = aaa_create_user(&ustruct, user, rem_user, location,
			  rem_addr, AAA_AUTHEN_TYPE_ASCII, svc, PRIV_USER);
    if (!ret) {
	return(FALSE);
    }
    if (tty) {
	switch (svc) {
	case AAA_SVC_LOGIN:
	case AAA_SVC_X25:
	    list_name = &tty->login_acc_list_name[0];
	    break;
	case AAA_SVC_NASI:
	    list_name = &tty->nasi_acc_list_name[0];
	    break;
	default:
	    list_name = NULL;
	    break;
	}
    }
    ret = aaa_start_login(ustruct, &astruct, list_name,
			  AAA_AUTHEN_ACT_LOGIN, svc);
    if (ret && (astruct.status == AAA_AUTHEN_STATUS_GETPASS)) {
	astruct.user_reply = strdup_named(pw, "AA #26");
	if (!astruct.user_reply) {
	    ret = FALSE;
	} else {
	    ret = aaa_cont_login(ustruct, &astruct);
	}
    }
    if (!ret || (astruct.status != AAA_AUTHEN_STATUS_PASS)) {
	aaa_cleanup_login(ustruct, &astruct);
	aaa_free_user(&tty->ustruct);
	return(FALSE);
    }
    aaa_cleanup_login(ustruct, &astruct);
    if (save_ustruct) {
	if (tty->ustruct)
	    aaa_free_user(&ustruct);
	tty->ustruct = ustruct;
    } else {
	aaa_free_user(&ustruct);
    }
    return(TRUE);
}

/*
 * AAA Authorization routines
 */

static void
debug_author_status (authorstruct *astruct)
{
    char *p;

    if (!aaa_author_debug) {	
	return;
    }

    switch(astruct->status) {
    case AAA_AUTHOR_STATUS_PASS_ADD:
	p = "PASS_ADD";
	break;
    case AAA_AUTHOR_STATUS_PASS_REPL:
	p = "PASS_REPL";
	break;
    case AAA_AUTHOR_STATUS_FAIL:
	p = "FAIL";
	break;
    case AAA_AUTHOR_STATUS_ERROR:
	p = "ERROR";
	break;
    default:
	p = "UKNOWN";
	break;
    }
    buginf("\nAAA/AUTHOR (%u): Post authorization status = %s",
	   astruct->session_id, p);
}


/*
 * This routine know the following attributes:
 * if service=shell && cmd=NULL
 *	autocommand
 *	acl
 *	noescape ???
 *	nohangup ???
 *	priv_lvl
 * For everything else it returns PASS_ADD with no args
 */
static boolean
localuser_do_author (userstruct *ustruct, authorstruct *astruct)
{
    usernametype *info = NULL;
    char buf[256];
    int avail = 0;
    boolean ret;
    char *service = NULL;
    char *cmd = NULL;
    char *protocol = NULL;

    if (ustruct)
	info = (usernametype *) find_namedthing(&userQ, ustruct->user);
    if (!ustruct || !info) {
	if (aaa_author_debug) {
	    if (ustruct) {
	        buginf("\nAAA/AUTHOR/LOCAL: no entry for %s", ustruct->user);
	    } else {
	        buginf("\nAAA/AUTHOR/LOCAL: lost user info");
	    }
	}
	astruct->status = AAA_AUTHOR_STATUS_ERROR;
	setstring_named(&astruct->server_msg, author_failed_msg,
			"AAA FAIL 7");
	return(FALSE);
    }

    /* This is the default return status unless we re-set it */
    astruct->status = AAA_AUTHOR_STATUS_PASS_ADD;

    service = aaa_get_value(astruct->out_args, AAA_ATTR_service);
    cmd = aaa_get_value(astruct->out_args, AAA_ATTR_cmd);
    protocol = aaa_get_value(astruct->out_args, AAA_ATTR_protocol);

    /*
     * If service=shell and cmd=NULL, then this is authorization for starting
     * a shell.  Set the following attributes.
     */
    if(STREQ(service, AAA_VAL_SVC_shell) && STREQ(cmd, AAA_VAL_null)) {
	if (info->flags & USER_AUTOCOMMAND) {
	    ret = aaa_add_AV(&astruct->in_args, &avail, AAA_ATTR_autocmd,
			     info->autocommand, AAA_AV_MAND);
	    if (!ret) {
		astruct->status = AAA_AUTHOR_STATUS_ERROR;
		return(FALSE);
	    }
	    astruct->status = AAA_AUTHOR_STATUS_PASS_ADD;
	}
	if (info->flags & USER_ACCESSLIST) {
	    sprintf(buf, "%d", info->accesslist);
	    ret = aaa_add_AV(&astruct->in_args, &avail, AAA_ATTR_acl, buf,
			     AAA_AV_MAND);
	    if (!ret) {
		astruct->status = AAA_AUTHOR_STATUS_ERROR;
		return(FALSE);
	    }
	    astruct->status = AAA_AUTHOR_STATUS_PASS_ADD;
	}
	if (info->flags & USER_NOESCAPE) {
	    ret = aaa_add_AV(&astruct->in_args, &avail, AAA_ATTR_noescape,
			     AAA_VAL_true, AAA_AV_MAND);
	    if (!ret) {
		astruct->status = AAA_AUTHOR_STATUS_ERROR;
		return(FALSE);
	    }
	    astruct->status = AAA_AUTHOR_STATUS_PASS_ADD;
	}
	if (info->flags & USER_NOHANGUP) {
	    ret = aaa_add_AV(&astruct->in_args, &avail, AAA_ATTR_nohangup,
			     AAA_VAL_true, AAA_AV_MAND);
	    if (!ret) {
		astruct->status = AAA_AUTHOR_STATUS_ERROR;
		return(FALSE);
	    }
	    astruct->status = AAA_AUTHOR_STATUS_PASS_ADD;
	}
	if (((info->flags & USER_PRIV) >> 28) != DEFAULT_PRIV) {
	    sprintf(buf, "%d", ((info->flags & USER_PRIV) >> 28));
	    ret = aaa_add_AV(&astruct->in_args, &avail, AAA_ATTR_priv_lvl,
			     buf, AAA_AV_MAND);
	    if (!ret) {
		astruct->status = AAA_AUTHOR_STATUS_ERROR;
		return(FALSE);
	    }
	    astruct->status = AAA_AUTHOR_STATUS_PASS_ADD;
	}

        /* 
         * Check for callback information 
         */
        astruct->status = aaa_scan_callback(astruct, info);

    } else { /* ! (service=shell, cmd=NULL) */
	/* if service == ppp||slip, clear IP address (until local support) */
	if (STREQ(service, AAA_VAL_SVC_ppp) || 
	    STREQ(service, AAA_VAL_SVC_slip)) {
	    astruct->in_args = aaa_remove_attr(astruct->out_args,
					       AAA_ATTR_addr);
	    astruct->status = AAA_AUTHOR_STATUS_PASS_REPL;
	} else {
	    /* Accept everything and add nothing new. */
	    astruct->status = AAA_AUTHOR_STATUS_PASS_ADD;
	}

        /* 
         * Check for callback information if this is ppp/lcp or slip
         * or arap. 
	 */
	if ((STREQ(service, AAA_VAL_SVC_ppp) && 
	     STREQ(protocol, AAA_VAL_PROT_lcp)) ||
	    STREQ(service, AAA_VAL_SVC_arap) ||
	    STREQ(service, AAA_VAL_SVC_slip)) {
	    astruct->status = aaa_scan_callback(astruct, info);      
	}
    }

    astruct->seq_num++;
    return(TRUE);
}


/*
 * NOTICE!!! - aaa_do_author() locks the ustruct that is passed in.  If you
 * add a new return path in aaa_do_author(), it is your responsibility to
 * unlock the ustruct before you return!
 *
 * Also, when passing in the pointer to the ustruct, make sure that the
 * pointer value comes directly from the tty struct or lcpstate struct.
 * to ensure that your ustruct has not been freed by another code path.
 */
boolean
aaa_do_author (userstruct *ustruct, authorstruct *astruct,
	       enum AUTHOR_TYPE type, int priv_level)
{
    int i, list_num;
    boolean ret;

    /*
     * Sanity checks.
     */
    if (!ustruct || !astruct) {
        if (aaa_author_debug) {
	    buginf("\nAAA/AUTHOR : internal info is invalid: 0x%x/0x%x",
		   astruct, ustruct);
	}
	return(FALSE);
    }

    /*
     * Lock the ustruct - just in case something weird happens
     */
    aaa_lock_user(ustruct);

    if (aaa_author_debug) {
	buginf("\nAAA/AUTHOR (%u): user='%s'",
	       astruct->session_id, ustruct->user);
    }

    astruct->type = type;

    if (aaa_author_debug) {
	for (i = 0; astruct->out_args[i]; i++) {
	    buginf("\nAAA/AUTHOR (%u): send AV %s", 
		   astruct->session_id, astruct->out_args[i]);
	}
    }

    if (astruct->in_args) {
	aaa_clear_attrs(&astruct->in_args);
    }
    setstring(&astruct->server_admin_msg, NULL);

    astruct->seq_num = 1;	/* Set starting sequence number */
    aaa_randomize_id(&astruct->session_id);

    list_num = type;
    if (type == AAA_AUTHOR_TYPE_CMD) {
	if (priv_level < PRIV_MIN || priv_level > PRIV_MAX) {
	    buginf("\n%% AAA/AUTHOR (%u): bad privilege level.",
		   astruct->session_id);
	    aaa_unlock_user(&ustruct);
	    return(FALSE);
	}

	if (aaa_author_debug) {
	    buginf("\nAAA/AUTHOR priv=%d", priv_level);
	}

	list_num += priv_level;
    }

    /*
     * Loop through all methods until we get a non-error status.
     */
    ret = FALSE;
    for (i = 0; ((i < NUM_AUTHOR_METHS) && !ret); i++) {
	setstring(&astruct->server_msg, NULL);
	switch(author_lists[list_num].methods[i]) {
	case AAA_AUTHOR_METH_RADIUS:
	case AAA_AUTHOR_METH_TAC_PLUS:
	    if (aaa_author_debug) {
		char *p = name_radius;
		if (author_lists[list_num].methods[i] == 
			 AAA_AUTHOR_METH_TAC_PLUS)
		    p = name_tacplus;
		buginf("\nAAA/AUTHOR (%u): Method=%s",
		       astruct->session_id, p);
	    }
	    reg_invoke_do_author(author_lists[list_num].methods[i], &ret,
					   ustruct, astruct);
	    break;
	case AAA_AUTHOR_METH_KRB5_INSTANCE:
	    if (aaa_author_debug) {
		char *p = name_kerberos;
		buginf("\nAAA/AUTHOR (%u): Method=%s",
		       astruct->session_id, p);
	    }
	    if ((ustruct->acc_method == AAA_ACC_METH_KRB5_TELNET)
		|| (ustruct->acc_method == AAA_ACC_METH_KRB5)) {
		reg_invoke_do_author(author_lists[list_num].methods[i], &ret,
					   ustruct, astruct);
	    } else {
		ret = TRUE;
		astruct->status = AAA_AUTHOR_STATUS_PASS_ADD;
		astruct->seq_num++;
	    }
	    break;
	case AAA_AUTHOR_METH_IF_AUTHEN:
	    if (aaa_author_debug) {
		buginf("\nAAA/AUTHOR (%u): Method=IF_AUTHEN",
		       astruct->session_id);
	    }
	    ret = TRUE;		/* not an error, so return true */
	    if (aaa_get_cred(ustruct, NULL, NULL, NULL, NULL, NULL)) {
		astruct->status = AAA_AUTHOR_STATUS_PASS_ADD;
	    } else {
		astruct->status = AAA_AUTHOR_STATUS_FAIL;
		setstring_named(&astruct->server_msg, author_failed_msg,
				"AAA FAIL 8");
	    }
	    astruct->seq_num++;
	    break;
	case AAA_AUTHOR_METH_LOCAL:
	    if (aaa_author_debug) {
		buginf("\nAAA/AUTHOR (%u): Method=LOCAL", astruct->session_id);
	    }
	    ret = localuser_do_author(ustruct, astruct);
	    break;
	case AAA_AUTHOR_METH_NONE:
	    if (aaa_author_debug) {
		buginf("\nAAA/AUTHOR (%u): Method=NONE", astruct->session_id);
	    }
	    ret = TRUE;
	    astruct->status = AAA_AUTHOR_STATUS_PASS_ADD;
	    astruct->seq_num++;
	    break;
	case AAA_AUTHOR_METH_NOT_SET:
	    if (aaa_author_debug) {
		buginf("\nAAA/AUTHOR (%u): Method=NOT_SET", 
		       astruct->session_id);
	    }
	    ret = TRUE;
	    if (i == 0)
		astruct->status = AAA_AUTHOR_STATUS_PASS_ADD;
	    else {
		astruct->status = AAA_AUTHOR_STATUS_FAIL;
		if (aaa_author_debug) {
		    buginf("\nAAA/AUTHOR (%u): no methods left to try",
			   astruct->session_id);
		}
	    }
	    astruct->seq_num++;
	    break;
	default:
	    buginf("\n%%Unknown authorization method %d set for list %d at %lx",
		   author_lists[list_num].methods[i], list_num,
		   &author_lists[list_num]);
	}
	debug_author_status(astruct);
    }

    if (!ret || astruct->status == AAA_AUTHOR_STATUS_ERROR) {
	if (astruct->server_admin_msg && aaa_author_debug)
	    buginf("\nAAA/AUTHOR: Authorization error msg: %s",
		   astruct->server_admin_msg);
	if (!astruct->server_msg)
	    setstring_named(&astruct->server_msg, author_failed_msg,
			    "AAA FAIL 9");
	aaa_unlock_user(&ustruct);
	return(FALSE);
    }

    aaa_unlock_user(&ustruct);
    return(TRUE);
}

void
aaa_cleanup_author (authorstruct *astruct)
{
    aaa_clear_attrs(&astruct->in_args);
    aaa_clear_attrs(&astruct->out_args);

    /* free checks for NULL */
    free(astruct->server_msg);
    free(astruct->server_admin_msg);
    memset(astruct, 0, sizeof(authorstruct));
}

void
aaa_clear_attrs (char ***attrsp)
{
    int i;
    char **array;

    if (attrsp && *attrsp) {
	array = *attrsp;
	for (i = 0; array[i]; i++)
	    free(array[i]);
	free(array);
	*attrsp = NULL;
    }
}

char **
aaa_remove_attr (char **attrs, char *attr)
{
    char **out, *ptr;
    int num1, num2, len;

    if (!attrs)
	return(NULL);

    for(num1 = 0; attrs[num1]; num1++);		/* loop to get count */
    out = malloc_named((num1 + 1) * sizeof(char *),
		       "AAA ATTR Array");
    if (!out)
	return(NULL);

    for(num1 = 0, num2 = 0; attrs[num1]; num1++) {
	len = strlen(attr);
	if (!strncmp(attr, attrs[num1], len) &&
	    ((attrs[num1][len] == '*') || (attrs[num1][len] == '=')))
	    continue;
	ptr = malloc_named(strlen(attrs[num1]) + 1, "AAA Attr strings");
	if (!ptr) {
	    aaa_clear_attrs(&out);
	    return(NULL);
	}
	strcpy(ptr, attrs[num1]);
	out[num2++] = ptr;
    }
    return(out);
}


void
aaa_set_attr (char ***a, char *in_attr, char *in_val, enum AV_TYPE in_type)
{
    int i;
    char *new_ptr, **arg;
    char *attr, *val;
    enum AV_TYPE t;

    if (!a || !*a) {
        return;
    }
    arg = *a;
    for (i = 0; arg[i]; i++) {
        if (aaa_split_AV(arg[i], &attr, &val, &t)) {
            if (!strcmp(attr, in_attr)) {
                break;
            }
        }
    }
    if (arg[i]) {
	new_ptr = malloc_named(strlen(in_attr) +1 + strlen(in_val) +1,
			       "AAA Attr Set");
	if (!new_ptr)
	    return;
	sprintf(new_ptr, "%s%c%s", in_attr,
		in_type == AAA_AV_MAND ? '=' : '*', in_val);
        free(arg[i]);
        arg[i] = new_ptr;
    } else {
	int avail = 0;
	aaa_add_AV(a, &avail, in_attr, in_val, in_type);
    }
}


void
aaa_merge_attrs (char **attrs1, char **attrs2, char ***out_attrs)
{
    int len1, len2;
    char **out;

    if (!attrs1 || !attrs2 || !out_attrs)
	return;

    for(len1 = 0; attrs1[len1]; len1++);
    for(len2 = 0; attrs2[len2]; len2++);
    out = malloc_named((len1 + len2 + 1) * sizeof(char *),
		       "AAA Attr Merge");
    if (!out)
	return;

    for(len1 = 0; attrs1[len1]; len1++) {
	out[len1] = strdup_named(attrs1[len1], "AAA 30");
	if (!out[len1]) {
	    aaa_clear_attrs(&out);
	    return;
	}
    }
    for(len2 = 0; attrs2[len2]; len1++, len2++) {
	out[len1] = strdup_named(attrs2[len2], "AAA 31");
	if (!out[len1]) {
	    aaa_clear_attrs(&out);
	    return;
	}
    }
    aaa_clear_attrs(out_attrs);
    *out_attrs = out;
    return;
}


char **
aaa_copy_attrs (char **attrs)
{
    char **out;
    int num;

    if (!attrs)
	return(NULL);

    for(num = 0; attrs[num]; num++);
    out = malloc_named((num + 1) * sizeof(char *), "AAA Attr Cpy");
    if (!out)
	return(NULL);

    for(num = 0; attrs[num]; num++) {
	out[num] = strdup_named(attrs[num], "AAA 32");
	if (!out[num]) {
	    aaa_clear_attrs(&out);
	    return(NULL);
	}
    }
    return(out);
}

/*
 * Returns true or false depending on whether authorization must be
 * done.  priv_level is only necessary if type == AAA_AUTHOR_TYPE_CMD.
 *
 * If idb is non-NULL this may be a dialer interface and we need to
 * know if this is an incoming or outgoing call. Only incoming calls
 * are subject to authorization. 
 *
 * Lastly, if this is a console line and the user on it has
 * authenticated, don't require any authorization so that the good
 * guys can't lock themselves out of the box. 
 */
boolean
aaa_author_needed (enum AUTHOR_TYPE type, int priv_level, hwidbtype *idb,
		   tt_soc *tty)
{
    dialerdbtype *ddb;
    int list_num;

    list_num = type;

    /* if we are trying to authorize a configuration command, but have
     * 'no aaa authorization config-commands', then just return FALSE
     *
     * If there is no csb (this should really never happen), then
     * continue with normal command authorization, so we don't open
     * a security hole.
     */
    if (type == AAA_AUTHOR_TYPE_CMD) {
	if (!tty->csb && aaa_author_debug) {
	    buginf("\nAAA/AUTHOR: no csb defined!");
	} else {
	    if ( (tty->csb->mode != exec_mode) &&
		 (!aaa_author_conf_commands) ) {
		if (aaa_author_debug) {
		    buginf("\nAAA/AUTHOR: config command authorization "
			   "not enabled");
		}
		return(FALSE);
	    }
	}
    }
    
    if (type == AAA_AUTHOR_TYPE_CMD) {
	if (priv_level < PRIV_MIN || priv_level > PRIV_MAX) {
	    buginf("\n%% AAA: bad privilege level for authorization check.");
	    return(FALSE);
	}
	list_num += priv_level;
    }

    if (author_lists[list_num].methods[0] == AAA_AUTHOR_METH_NOT_SET) {
	return(FALSE);
    }

    if (!idb && tty && (tty->type == CTY_LINE) &&
	aaa_get_cred(tty->ustruct, NULL, NULL, NULL, NULL, NULL)) {
	if (aaa_author_debug) {
	    buginf("\nAAA/AUTHOR: authenticated console user is permitted");
	}
	return(FALSE);
    }

    /* If this is a network connection via a dialer which is placing
     * an outgoing call, don't require authorization */
    ddb = idb ? get_dialerdb(idb) : NULL;
    if ((type == AAA_AUTHOR_TYPE_NET) && 
	(ddb != NULL) && 
 	!reg_invoke_dialer_is_answer(ddb->dialer, ddb)) {
	return(FALSE);
    }

    return(TRUE);
}

boolean
aaa_add_AV (char ***ptr, int *avail, char *attr, char *val, enum AV_TYPE type)
{
    char **out_array = NULL, **in_array;
    int num;

    if (attr == NULL)
	return(FALSE);	/* Illegal */

    if (*ptr == NULL) {
	out_array = malloc_named(AAA_AV_INCREMENT * sizeof(char *),
				 "AAA AV Add");
	if (!out_array)
	    return(FALSE);
	*ptr = out_array;
	num = 0;
	*avail = AAA_AV_INCREMENT - 1;
    } else {
	in_array = *ptr;
	for (num = 0; in_array[num]; num++);	/* current number of AVs */
	if (*avail == 0) {
	    out_array = malloc_named((num + 1 + AAA_AV_INCREMENT) * \
				     sizeof(char *), "AAA AV Out");
	    if (!out_array)
		return(FALSE);
	    bcopy(in_array, out_array, (num * sizeof(char *)));
	    /*
	     * Accounting starts with a static array.  Detect this and
	     * don't free it.  Yech.
	     */
	    if (mem_lockable(in_array))
		free(in_array);
	    *ptr = out_array;
	    *avail = AAA_AV_INCREMENT;
	} else {
	    out_array = in_array;
	}
    }

    out_array[num] = malloc_named(strlen(attr) + 1 + strlen(val) + 1,
				  "AAA AV Last");
    if (!out_array[num]) {
	return(FALSE);
    }
    (*avail)--;
    sprintf(out_array[num], "%s%c%s",
	    attr, (type == AAA_AV_MAND) ? '=' : '*', val);

    return(TRUE);
}

boolean
aaa_split_AV (char *av_string, char **attrp, char **valp, enum AV_TYPE *typep)
{
    char *ptr;

    for (ptr = av_string; ptr && *ptr; ptr++) {
	if ((*ptr == '*') || (*ptr == '=')) {
	    if (attrp) {
		*attrp = string_getnext();
		sstrncpy(*attrp, av_string, ptr - av_string + 1);
	    }
	    if (valp)
		*valp = ptr + 1;
	    if (typep) {
		if (*ptr == '=')
		    *typep = AAA_AV_MAND;
		else
		    *typep = AAA_AV_OPT;
	    }
	    return(TRUE);
	}
    }
    return(FALSE);
}


char *
aaa_get_value (char **args, char *attr)
{
    int i;
    char *lattr, *lval;

    if (!args)
	return(NULL);

    for (i = 0; args[i]; i++) {
	if (aaa_split_AV(args[i], &lattr, &lval, NULL)) {
	    if (!strcmp(attr, lattr))
		return(lval);
	}
    }
    return(NULL);
}

/*
 * Accounting routines
 */
#ifdef NOBODY_CALLS_ME
boolean
aaa_do_acct (userstruct *ustruct, acctstruct *astruct,
	     enum AUTHOR_TYPE type, int priv_level)
{
    int i, list_num;
    boolean ret;

    setstring(&astruct->server_msg, NULL);
    setstring(&astruct->server_admin_msg, NULL);

    astruct->seq_num = 1;	/* Set starting sequence number */
    aaa_randomize_id(&astruct->session_id);

    list_num = type;
    if (type == AAA_AUTHOR_TYPE_CMD) {
	if (priv_level < PRIV_MIN || priv_level > PRIV_MAX) {
	    buginf("\n%% AAA: bad privilege level for authorization check.");
	    return(FALSE);
	}
	list_num += priv_level;
    }

    /*
     * Loop through all methods until we get a non-error status.
     */
    ret = FALSE;
    for (i = 0; ((i < NUM_ACCT_METHS) && !ret); i++) {
	switch(acct_lists[list_num].methods[i]) {
	case AAA_AUTHOR_METH_RADIUS:
	case AAA_AUTHOR_METH_TAC_PLUS:
	    if (!ustruct->no_acct) {
		reg_invoke_do_acct(acct_lists[list_num].methods[i], &ret,
				   ustruct, astruct);
	    }
	    break;
	case AAA_AUTHOR_METH_NOT_SET:
	case AAA_AUTHOR_METH_NONE:
	    ret = TRUE;
	    astruct->status = AAA_ACCT_STATUS_SUCCESS;
	    astruct->seq_num++;
	    break;
	default:
	    /* Shouldn't happen */
	    ret = FALSE;
	    astruct->status = AAA_ACCT_STATUS_ERROR;
	    astruct->seq_num++;
	    buginf("\n%% AAA: invalid accounting method.");
	    break;
	}
    }

    if (!ret || (astruct->status == AAA_ACCT_STATUS_ERROR)) {
	if (astruct->server_admin_msg && aaa_acct_debug)
	    buginf("\nAAA/ACCT: Accounting error msg: %s",
		   astruct->server_admin_msg);
	return(FALSE);
    }

    return(TRUE);
}
#endif /* NOBODY_CALLS_ME */

/*
 * Returns 0 or an auth method type depending on whether accounting must be
 * done.  priv_level is only necessary if type == AAA_AUTHOR_TYPE_CMD
 */
enum AUTHOR_METHOD aaa_acct_needed (enum AUTHOR_TYPE type, int priv_level)
{
    int list_num;

    list_num = type;
    if (type == AAA_AUTHOR_TYPE_CMD) {
	if (priv_level < PRIV_MIN || priv_level > PRIV_MAX) {
	    buginf("\n%% AAA: bad privilege level for accounting check.");
	    return(0);
	}
	list_num += priv_level;
    }

    if ((acct_lists[list_num].methods[0] == AAA_AUTHOR_METH_NOT_SET) ||
	(acct_lists[list_num].methods[0] == AAA_AUTHOR_METH_NONE))
	return(AAA_AUTHOR_METH_NOT_SET);

    return(acct_lists[list_num].methods[0]);
}

#ifdef NOBODY_CALLS_ME
void
aaa_cleanup_acct (acctstruct *astruct)
{
#if 0 /* PINAVS */
    aaa_clear_attrs(&astruct->out_args);
#endif
    memset(astruct, 0, sizeof(acctstruct));
}
#endif /* NOBODY_CALLS_ME */


/*
 * Scan the username information and build the corresponding
 * A-V lists
 */ 
int
aaa_scan_callback (authorstruct *astruct, usernametype *callback_info)
{
   int avail = 0;
   char buf[256];
   boolean ret;

   /*
    * Ignore if callback is not enabled
    */
    if (!callback_info || !(callback_info->flags & USER_CALLBACK_DIALSTRING))
       return(astruct->status);

   /*
    * Build callback information in A-V lists.
    */

   /* 
    * Dial string info (could be NULL)
    */
   ret = aaa_add_AV(&astruct->in_args, &avail, AAA_ATTR_callback,
		    callback_info->dial_string, AAA_AV_MAND);

   if (!ret) 
       return(AAA_AUTHOR_STATUS_ERROR);

   /*
    * nocallback-verify info
    */
   if (callback_info->flags & USER_NOCALLBACK_VERIFY)
       ret = aaa_add_AV(&astruct->in_args, &avail, 
                        AAA_ATTR_callback_noverify,
			"1", AAA_AV_MAND);

   if (!ret) 
       return(AAA_AUTHOR_STATUS_ERROR);

   /*
    * callback-rotary 
    */
   if (callback_info->flags & USER_CALLBACK_ROTARY) {
       sprintf(buf, "%d", callback_info->callback_rotary_or_line);
       ret = aaa_add_AV(&astruct->in_args, &avail, 
                        AAA_ATTR_callback_rotary,
			buf, AAA_AV_MAND);
   }

   if (!ret) 
       return(AAA_AUTHOR_STATUS_ERROR);

   /*
    * callback-line info
    */
   if (callback_info->flags & USER_CALLBACK_LINE) {
       sprintf(buf, "%d", callback_info->callback_rotary_or_line);
       ret = aaa_add_AV(&astruct->in_args, &avail, 
                            AAA_ATTR_callback_line,
			    buf, AAA_AV_MAND);
   }  
        
   if (!ret)
      return(AAA_AUTHOR_STATUS_ERROR);

   return(AAA_AUTHOR_STATUS_PASS_ADD);
}

/*
 * idbname2portval
 * convert an idb namestring (ie S0:1) to a value used in 16 bit port
 * identifiers used by radius, xtacacs, etc, as follows:
 * Value:	Meaning:
 * 00ttt	TTYttt or interface asyncttt
 * 10xxx	Ordinary interface Serialxxx
 * 2ppcc	Interface PRIpp (Serialpp), channel cc
 * 3bb0c	Interface BRIbb, channel C
 * 4zzzz	reserved
 * 5zzzz	reserved
 * 6nnss	Other. Interface ???nn:ss
 */
ushort
idbname2portval (char *name)
{ 
    char *unit, *subunit;
    int idbunit, port, channel = 0;

    for (unit=name; *unit; unit++)
	if (*unit >= '0' && *unit <= '9')
	    break;	/* Find start of numeric part of port */
    idbunit = atoi(unit);
    for (subunit=unit; *subunit; subunit++)
	if (*subunit == ':')
	    break;	/* Skip to end of first unit number */
    if (*subunit == ':')
	channel = atoi(subunit+1);
    switch (*name) {
      case 'P':
      case 'S':		/* Serial interface */
	if (*subunit) {
	    port = AAA_IFVAL_DIVISOR*AAA_IFVAL_PRI +
		   100*idbunit + channel; /* T1/E1 */
	} else {
	    port = AAA_IFVAL_DIVISOR*AAA_IFVAL_SYNC + idbunit; /* Serial */
	}
	break;
      case 'B':
	port = AAA_IFVAL_DIVISOR*AAA_IFVAL_BRI +
	       100*idbunit + channel; /* BRI */
	break;
      default:
	port = AAA_IFVAL_DIVISOR*AAA_IFVAL_OTHER +
	       100*idbunit + channel; /* "Other" */
	break;
    }
    return(port);
}


/*
 * aaa_chap_rresponse
 *
 * Wrapper function for caller, eg. PPP, L2F, MLPVT to check a CHAP
 * response.  
 *
 * Return TRUE, if the response passes, else return FALSE
 */
boolean aaa_chap_rresponse (userstruct **ustruct, authenstruct *astruct,
			    char *user, hwidbtype *idb, uchar* response,
			    uchar challenge_id, uchar *challenge,
			    uchar challenge_size, char *access_list) {

    char *rem = NULL;
    char *namestring = NULL;
    char *data = NULL;
    boolean ret;

    bzero(astruct, sizeof(authenstruct));

    if (idb) {
	if (idb->idb_tty) {
	    astruct->tty = idb->idb_tty;
	    rem = tty_get_remote_addr(idb->idb_tty);
	}
	if (is_isdn(idb) && (idb->isdn_info->remote_src == ISDN_SRC_ISDN)) {
	    rem = idb->isdn_info->remote_number;
	}
	namestring = idb->hw_namestring;
    }
  
    ret = aaa_create_user(ustruct, user, NULL, namestring, rem,
			  AAA_AUTHEN_TYPE_CHAP, AAA_SVC_PPP, PRIV_USER);
    if (ret) {
	data = malloc_named(1 + challenge_size + MD5_LEN,
			    "Check CHAP Response");
	if (!data)
	    ret = FALSE;
    }

    if (ret) {
	/*
	 * Call aaa_start_login() with the challenge value and
	 * peer's response, to check whether the authenication passed
	 */
	data[0] = challenge_id;
	bcopy(challenge, &data[1], challenge_size);
	bcopy(response, &data[1 + challenge_size], MD5_LEN);
	astruct->user_data = data;
	astruct->user_data_len = 1 + challenge_size + MD5_LEN;

	ret = aaa_start_login(*ustruct, astruct, access_list,
			      AAA_AUTHEN_ACT_LOGIN, AAA_SVC_PPP);
	astruct->user_data = NULL;
	astruct->user_data_len = 0;
	free(data);
	
	if (astruct->status != AAA_AUTHEN_STATUS_PASS)
	    ret = FALSE;
    }

    return ret;
}

/*
 * aaa_chap_rchallenge
 *
 * Wrapper function for caller, eg. PPP, L2F, MLPVT to response to a 
 * challenge.
 *
 * Return TRUE if we can create a response, else FALSE
 */
boolean aaa_chap_rchallenge (userstruct **ustruct, authenstruct *astruct,
			     char *user, hwidbtype *idb, 
			     uchar challenge_id, uchar *challenge,
			     uchar challenge_size, char *access_list) {
    char *rem = NULL;
    char *namestring = NULL;
    char *data = NULL;
    boolean ret;

    bzero(astruct, sizeof(authenstruct));
    if (idb) {
	if (idb->idb_tty) {
	    astruct->tty = idb->idb_tty;
	    rem = tty_get_remote_addr(idb->idb_tty);
	}
	if (is_isdn(idb) && (idb->isdn_info->remote_src == ISDN_SRC_ISDN)) {
	    rem = idb->isdn_info->remote_number;
	}
	namestring = idb->hw_namestring;
    }

    ret = aaa_create_user(ustruct, user, NULL, namestring, rem,
			  AAA_AUTHEN_TYPE_CHAP, AAA_SVC_PPP, PRIV_USER);
    if (ret) {
	data = malloc_named(challenge_size + 1, "CHAP Challenge Response");
	if (!data)
	    ret = FALSE;
    }
    if (ret) {
	data[0] = challenge_id;
	bcopy(challenge, &data[1], challenge_size);
	astruct->user_data = data;
	astruct->user_data_len = challenge_size + 1;
	ret = aaa_start_login(*ustruct, astruct,access_list,
			      AAA_AUTHEN_ACT_SENDAUTH, AAA_SVC_PPP);
	astruct->user_data = NULL;
	astruct->user_data_len = 0;
	free(data);
	
	if (astruct->status != AAA_AUTHEN_STATUS_PASS)
	    ret = FALSE;
    }

    return (ret);
}

/* Support for one time passwords.  Do a regular login query with user
 * and password, specifically to support the one-time (single-line)
 * hack.
 *
 * Return TRUE if the one time password was accepted.  
*/
boolean
aaa_otp_login (hwidbtype *idb, tt_soc *tty, char *user, char *otp, 
		       char *acc_list_name)
{
    authenstruct authen;
    userstruct *ustruct = NULL;
    char *rem = NULL;
    boolean ret = FALSE;
    char *hw_name = NULL;
    char tty_str[10];

    if (idb) {
	hw_name = idb->hw_namestring;
    } else {
	if (tty) {
	    sprintf(tty_str, "tty%d", tty->ttynum);
	    hw_name = tty_str;
	}
    }

    if (aaa_authen_debug) {
	buginf("\nAAA/AUTHEN/OTP %s: start %s*%s", hw_name, user, otp);
    }

    memset(&authen, 0, sizeof(authen));

    if (tty) {
	authen.tty = tty;
	rem = tty_get_remote_addr(tty);
    }
    if (idb && is_isdn(idb) && (idb->isdn_info->remote_src == ISDN_SRC_ISDN)) {
	rem = idb->isdn_info->remote_number;
    }

    ret = aaa_create_user(&ustruct, user, NULL, hw_name, rem,
			  AAA_AUTHEN_TYPE_ASCII, AAA_SVC_LOGIN, PRIV_USER);

    aaa_pick_authen_method(acc_list_name, AAA_SVC_LOGIN, &authen, ustruct);
    if (!(authen.methods && authen.methods[0])) {
	ret = FALSE;
    }

    if (ret) {
	/* 
	 * If the method being used isn't T+ or Radius, we should
	 * declare an error.
	 */
	switch (authen.methods[0]) {
	case AAA_ACC_METH_TACACSPLUS:
	case AAA_ACC_METH_RADIUS:
	    break;

	default:
	    if (aaa_authen_debug) {
		buginf("\nAAA/AUTHEN/OTP: "
		       "Only TACACS or RADIUS are legal OTP methods");
	    }
	    ret = FALSE;
	}
    }
    
    memset(&authen, 0, sizeof(authen));

    if (ret) {
	ret = aaa_start_login(ustruct, &authen, acc_list_name,
			      AAA_AUTHEN_ACT_LOGIN, AAA_SVC_LOGIN);
    }

    /* We *MUST* receive a getpass in response to our start login */
    if (ret && (authen.status != AAA_AUTHEN_STATUS_GETPASS)) {
	ret = FALSE;
    }

    if (ret) {
	authen.user_reply = otp;
	aaa_cont_login(ustruct, &authen);
	authen.user_reply = NULL;

	/* Anything except a PASS is bad news */
	if (authen.status != AAA_AUTHEN_STATUS_PASS) {
	    ret = FALSE;
	}
    }

    if (ustruct) {
	aaa_cleanup_login(ustruct, &authen);
	aaa_free_user(&ustruct);
    }

    if (aaa_author_debug) {
	buginf("\nAAA/AUTHEN/OTP %s: OTP login for user %s %s.",
	       hw_name, user, (ret ? "succeeded" : "failed"));
    }
    return(ret);
}


/*
 * Table of aaa attribute names used by accounting.  This is used for
 * translation of "protocol independent" accounting info into aaa/tacacs+
 * style av pairs, and is used by both tacacs+ (for building packets) and
 * aaa accounting (for "show accounting" and friends.)  This this global
 * location; systems can be built with either of the above not included.
 */

char * const aaa_attr_names[AAA_ATTR_MAX] = { "unknown",
AAA_ATTR_service, AAA_ATTR_starttime, "<obsolete>", AAA_ATTR_elapsedtime,
AAA_ATTR_status, AAA_ATTR_priv_lvl, AAA_ATTR_cmd, AAA_ATTR_protocol,
AAA_ATTR_cmd_arg, AAA_ATTR_bytesin, AAA_ATTR_bytesout, AAA_ATTR_paksin,
AAA_ATTR_paksout, AAA_ATTR_addr, AAA_ATTR_task_id,
AAA_ATTR_callback, AAA_ATTR_callback_noverify, AAA_ATTR_callback_line,
AAA_ATTR_callback_rotary, AAA_ATTR_reason, AAA_ATTR_event,
AAA_ATTR_timezone };

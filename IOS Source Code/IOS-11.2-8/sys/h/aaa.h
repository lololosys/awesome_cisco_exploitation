/* $Id: aaa.h,v 3.9.18.12 1996/09/12 04:01:57 tkolar Exp $
 * $Source: /release/112/cvs/Xsys/h/aaa.h,v $
 *------------------------------------------------------------------
 * aaa.h -- system authentication, authorization and accounting parameters
 *
 * August 1994, David Carrel
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: aaa.h,v $
 * Revision 3.9.18.12  1996/09/12  04:01:57  tkolar
 * CSCdi62277:  L2F tunnel authentication over Radius fails
 * Branch: California_branch
 * Allow cleartext passwords to be passed from Radius
 *
 * Revision 3.9.18.11  1996/09/11  01:46:17  che
 * CSCdi66795:  Double authentication with NASI and TACACS+
 * Branch: California_branch
 * Bypass exec authentication if we have already done NASI authentication.
 *
 * Revision 3.9.18.10  1996/08/27  18:22:33  sbelair
 * CSCdi60087:  AAA UNKNOWN for protocol 0x8027
 * Branch: California_branch
 * Add ATTR_VAL definitions for osicp and deccp.
 *
 * Revision 3.9.18.9  1996/08/15  21:42:36  billw
 * CSCdi65576:  regress - problem found in code review for CSCdi63788
 * Branch: California_branch
 * "Copy" savedauthor by mem_lock() and pointer copy.
 * Make it "const", add comments and casts as appropriate so no one
 * breaks this without concerted effort!
 *
 * Revision 3.9.18.8  1996/07/30  01:41:38  billw
 * CSCdi59105:  AAA authenthenication errors during continue login cause
 * failure
 * Branch: California_branch
 *
 * Revision 3.9.18.7  1996/07/05  19:07:27  snyder
 * CSCdi62128:  change some char*[] to const and save on data space
 * Branch: California_branch
 *
 * Revision 3.9.18.6  1996/06/26  22:44:25  billw
 * CSCdi59376:  AAA local prompts not configurable
 * Branch: California_branch
 *
 * Revision 3.9.18.5  1996/06/05  00:01:33  tkolar
 * CSCdi55297:  Document L2F VPDN A/V pairs
 * Branch: California_branch
 * Replace magic strings with defines.
 *
 * Revision 3.9.18.4  1996/05/28  16:00:02  vandys
 * CSCdi58491:  Require accounting event for L2F tunnel creation
 * Branch: California_branch
 *
 * Revision 3.9.18.3  1996/05/01  02:06:21  perryl
 * CSCdi56162:  MultiChassis MLP and VPN AAA calls mismatched
 * Branch: California_branch
 * Add wrapper functions aaa_chap_rresponse() and aaa_chap_rchallenge(),
 * for L2F and MLPVT to do CHAP style authenication with the new AAA API.
 *
 * Revision 3.9.18.2  1996/04/29  09:37:20  fox
 * CSCdi55358:  PPP subsystem needs to be more modular
 * Branch: California_branch
 * Move Network Control Protocol (NCP) support from PPP subsystem to
 * individual subsystems.  Attach NCP subsystems to respective protocol
 * subsystems.  Add registry calls to network protocol init code to
 * register default NCP support for NCPs not requiring option negotiation.
 *
 * Revision 3.9.18.1  1996/04/19  15:15:00  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commmit
 *
 * Revision 2.1.6.9  1996/04/16  03:53:44  che
 * Branch: IosSec_branch
 * Code changes mandated by code review comments.
 *
 * Revision 2.1.6.8  1996/04/10  23:06:32  lol
 * Branch: IosSec_branch
 *         Incorporate code review comments.
 *
 * Revision 2.1.6.7  1996/03/05  04:43:12  che
 * Branch: IosSec_branch
 * Kerberos Phase 2 commit.
 *
 * Revision 2.1.6.6  1996/02/28  06:14:30  lol
 * Branch: IosSec_branch
 * TACACS+ processing.
 *
 * Revision 2.1.6.5  1996/02/21  03:16:11  che
 * Branch: IosSec_branch
 * Sync to 11.1 Mainline (02/20/96)
 *
 * Revision 2.1.6.4  1996/01/23  05:22:07  che
 * Branch: IosSec_branch
 * Sync with 11.1 mainline. (960122)
 *
 * Revision 2.1.6.3  1995/12/19  03:46:57  che
 * Branch: IosSec_branch
 * Re-sync to Arkansas to fix things that broke.
 *
 * Revision 2.1.6.2  1995/12/17  01:33:43  che
 * Branch: IosSec_branch
 * Sync to Arkansas
 *
 * Revision 2.1.6.1  1995/11/17  16:41:43  carrel
 * Branch: IosSec_branch
 * Sync to Arkansas
 *
 * Revision 3.9  1996/02/09  09:02:59  billw
 * CSCdi43680:  RADIUS doesnt support magic number ip address values
 * also add authstruct->auth_idb, and change auth->tty to auth->auth_tty
 *
 * Revision 3.8  1996/01/23  22:39:28  billw
 * CSCdi43679:  RADIUS support does not include CHAP
 * Do it in a way that will be obsolete in California
 *
 * Revision 3.7  1996/01/12  02:40:29  billw
 * CSCdi44858:  use #define for AAA attribute values
 * Bring the AV pairs used up to spec level...
 *
 * Revision 3.6  1996/01/04  22:14:56  lol
 * CSCdi43469:  per-user route wont use default interface
 *
 * Revision 3.5  1995/12/24  09:35:35  che
 * CSCdi46019:  Store Kerberos credentials in Kerberos library cred cache
 * Use Kerberos library credentials cache structures and functions to
 * manipulate user credentials cache.
 *
 * Revision 3.4  1995/12/13  02:59:19  billw
 * CSCdi42536:  aaa_acct not proper subsystem
 * Move some code around, change some calls to reg_invokes, etc.
 *
 * Revision 3.3  1995/12/12  06:00:47  billw
 * CSCdi41773:  Login (username) promtps different with local override
 * confiured
 *
 * Revision 3.2  1995/11/17  09:11:37  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:34:38  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.12  1995/11/08  20:58:20  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.11  1995/10/02  20:42:47  lol
 * CSCdi36244:  Console line failing authorization should be more forgiving
 *              Authenticated console users now bypass authorization.
 *
 * Revision 2.10  1995/09/16  21:38:56  carrel
 * CSCdi40334:  Bus error crash in AAA accounting code
 * Added comments and changed args to aaa_dup_ifneeded()
 *
 * Revision 2.9  1995/08/31  19:48:46  lol
 * CSCdi38919:  tacacs+ ip pooling not working
 *
 * Revision 2.8  1995/08/29  04:14:56  billw
 * CSCdi39403:  AAA Accounting: No start_time attribute
 * Include configured timezone as well.
 *
 * Revision 2.7  1995/08/08  20:50:20  lol
 * CSCdi37361:  AAA should not require auth for originating PPP connections
 *
 * Revision 2.6  1995/07/26  14:14:42  billw
 * CSCdi37036:  No AAA Accounting records generated for system events
 * generate "system" records for startup, shutdown, and change in the
 * state of "aaa accounting system" configuration.
 *
 * Revision 2.5  1995/07/18  00:56:41  billw
 * CSCdi36940:  AAA should keep cumulative statistics
 * This patch does accounting stats only...
 *
 * Revision 2.4  1995/07/14  07:02:26  hampton
 * Convert Terminal Server to fully use the passive timers macros instead
 * of referencing the system clock directly.  [CSCdi37093]
 *
 * Revision 2.3  1995/06/15 22:57:15  syiu
 * CSCdi35686:  Async callback AAA authorization and accounting phase not
 * working
 *
 * Revision 2.2  1995/06/15  03:42:12  lol
 * CSCdi35713:  AAA subsystems needs more debugging output
 *              Add
 *                 aaa debug authentication
 *                 aaa debug authorization
 *                 aaa debug accounting
 *
 * Revision 2.1  1995/06/07  20:34:26  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#ifndef __H_AAA_H__
#define __H_AAA_H__

#include "address.h"

extern boolean old_access_control;	/* Is old access control in effect? */
extern boolean kerberos5_is_installed;
extern boolean tacacsplus_is_installed;
extern boolean radius_is_installed;
extern boolean aaa_acct_suppress_null_username;
extern boolean aaa_acct_nested;

extern char *username_prompt;
extern char *password_prompt;
extern const char old_password_prompt[];
extern const char new_password_prompt[];
extern const char auth_failed[];
extern const char author_failed_msg[];

extern ulong aaa_user_create, aaa_user_frees, aaa_user_acctleft;

#define AAA_MAX_REPLY_LEN	1024

#define MAX_ACC_METHS	4	/* Maximum number of access methods */
#define MAX_ACC_LISTS	8	/* Maximum number of access method lists */
#define MAX_ACC_LISTS_LOGIN	MAX_ACC_LISTS
#define MAX_ACC_LISTS_ENABLE	1
#define MAX_ACC_LISTS_PPP	MAX_ACC_LISTS
#define MAX_ACC_LISTS_ARAP	MAX_ACC_LISTS
#define MAX_ACC_LISTS_RCMD	1
#define MAX_ACC_LISTS_NASI	MAX_ACC_LISTS

/*
 * For autehntications that don't have named lists, define the name
 * that should be passed to aaa_start_login.
 */

#define ACC_LIST_DEFAULT "default"

struct access_method_list {
    boolean valid;
    int	methods[MAX_ACC_METHS];
    char name[32];
};

/*
 * WARNING/NOTE:  Many of the enum values below are part of external 
 * specifications.  DO NOT change any values unless you know what you are
 * doing.  In particular, consult the TACACS+ specification.
 */

enum AUTHEN_STATUS {
    AAA_AUTHEN_STATUS_PASS	= 0x01,
    AAA_AUTHEN_STATUS_FAIL	= 0x02,
    AAA_AUTHEN_STATUS_GETDATA	= 0x03,
    AAA_AUTHEN_STATUS_GETUSER	= 0x04,
    AAA_AUTHEN_STATUS_GETPASS	= 0x05,
    AAA_AUTHEN_STATUS_RESTART	= 0x06,
    AAA_AUTHEN_STATUS_ERROR	= 0x07,
    AAA_AUTHEN_STATUS_NOSNDPASS = 0x100	/* Internal AAA level error */
};

enum AUTHEN_TYPE {		/* Authentication method being used */
    AAA_AUTHEN_TYPE_NONE	= 0x00,
    AAA_AUTHEN_TYPE_ASCII	= 0x01,
    AAA_AUTHEN_TYPE_PAP		= 0x02,
    AAA_AUTHEN_TYPE_CHAP	= 0x03,
    AAA_AUTHEN_TYPE_ARAP	= 0x04
};

/*
 * AAA_ACC_METH_KRB5_TELNET specifies that you want to authenticate using
 * the Telnet RFC1416 specification for Kerberos authentication.  This
 * method is only valid for for "aaa authentication login" and can only
 * be specified as the first access control method to attempt.
 */
enum ACC_METHOD {		/* Access control method to use */
    AAA_ACC_METH_NOT_SET	= 0x00,
    AAA_ACC_METH_NONE		= 0x01,
    AAA_ACC_METH_KRB5		= 0x02,
    AAA_ACC_METH_LINE		= 0x03,
    AAA_ACC_METH_ENABLE		= 0x04,
    AAA_ACC_METH_LOCAL		= 0x05,
    AAA_ACC_METH_TACACSPLUS	= 0x06,
    AAA_ACC_METH_IFNEEDED	= 0x07,
    AAA_ACC_METH_GUEST		= 0x08,
    AAA_ACC_METH_AUTHGUEST	= 0x09,
    AAA_ACC_METH_RADIUS		= 0x10,
    AAA_ACC_METH_RCMD		= 0x20,
    AAA_ACC_METH_KRB5_TELNET	= 0x40
};

/*
 * This is the maximum NUMBER of methods that AAA can use, which isn't the
 * same as the max possible value.  It's used by the registry code to set
 * up the case statement.
 */
#define AAA_METHODS_MAX 20


enum AAA_SERVICES {
    AAA_SVC_NONE		= 0x00,
    AAA_SVC_LOGIN		= 0x01,
    AAA_SVC_ENABLE		= 0x02,
    AAA_SVC_PPP			= 0x03,
    AAA_SVC_ARAP		= 0x04,
    AAA_SVC_PT			= 0x05,
    AAA_SVC_RCMD		= 0x06,
    AAA_SVC_X25			= 0x07,
    AAA_SVC_NASI		= 0x08
};

/*
 * Specifies the type of credentials contained within this structure.
 * Currently, Kerberos is the only authentication mechanism we support
 * that uses credentials.  Kerberos passes the address to the pointer
 * to the free function to remain consistant with the internal Kerberos
 * libraries.
 */

enum CRED_TYPE { KERBEROS = 1 };

struct credentials {
    int ref_cnt;			
    void (*free_funct)(void *free_cred);
    enum CRED_TYPE cred_type;
    void *cred;
};

struct userstruct_ {
    char *user;
    char *ruser;
    char *port;
    char *rem_addr;
    int  priv_lvl;
    enum AUTHEN_TYPE 	authen_type;
    enum ACC_METHOD	acc_method;
    struct credentials *cred;
    enum AAA_SERVICES	service;
    addrtype server;
    char *password;		/* save for ARAP */
    int sharecount;		/* For multiple process handling */
    queuetype acctQ;		/* accountable actions in progress */
    const void *savedauthor;	/* Saved authorization data from authent
				 * Note that this is a pointer that is
				 * duplicated via mem_lock() and pointer
				 * copying, so it must be malloc'ed, and
				 * must be read-only
				 */
    tinybool authen_config;     /* true if we are doing a (radius)
				 * authentication just to get author
				 * info for NAS configuration
				 */
    tinybool no_acct;		/* Do not do accounting for this ustruct */
};

enum AUTH_ACTIONS {
    AAA_AUTHEN_ACT_LOGIN	= 0x1,
    AAA_AUTHEN_ACT_CHPASS	= 0x2,
    AAA_AUTHEN_ACT_SENDPASS	= 0x3,
    AAA_AUTHEN_ACT_SENDAUTH	= 0x4
};

#define AAA_RESP_FLAG_NOECHO	0x01

/* WARNING!  */
/* Although aaa_cleanup_login() will free memory allocated to this structure,
 * it WILL NOT recurse down all pointers.
 *
 * So if you assign a structure to the OPAQUE field, you are responsible
 * for freeing up any memory you allocate!!!
 */
struct authenstruct_ {
    int session_id;
    enum AUTH_ACTIONS	action;
    int seq_num;
    char *user_reply;
    char *user_data;
    uint user_data_len;
    char *server_msg;
    char *server_data;
    uint server_data_len;
    enum AUTHEN_STATUS status;
    void *opaque;	/* SEE NOTES BELOW BEFORE USING */
    int *methods;	/* List of authent methods to try (NOT malloced!) */
    void *connection_handle;
    tt_soc *tty;
    uchar flags;
};

/* NOTES for authenstruct.opaque above.
 * The opaque field above is for use by individual authentication methods
 * to save state.  The rules for it's use is that it must only ever point
 * to a block of malloc-ed memory, and that memory must NOT contain any
 * pointers to more memory.  If a connection is aborted, AAA will cleanup
 * this memory by doing a single free() of the memory pointed to by opaque.
 */

#define DES_BLOCK_SIZE  8

/* Auth structures for different authentication types */
typedef struct aaa_arap_auth_data_ {
    char myrandom[DES_BLOCK_SIZE];
    char theirrandom[DES_BLOCK_SIZE];
    char theirresult[DES_BLOCK_SIZE];
} aaa_arap_auth_data;

/*
 * Authorization
 */

#define AAA_AV_INCREMENT 5	/* Number of AV to malloc at a time */

enum AV_TYPE {
    AAA_AV_MAND,
    AAA_AV_OPT
};

enum AUTHOR_STATUS {
    AAA_AUTHOR_STATUS_PASS_ADD	= 0x01,
    AAA_AUTHOR_STATUS_PASS_REPL	= 0x02,
    AAA_AUTHOR_STATUS_FAIL	= 0x10,
    AAA_AUTHOR_STATUS_ERROR	= 0x11
};

enum AUTHOR_TYPE {
    AAA_AUTHOR_TYPE_SHELL	= 0,
    AAA_AUTHOR_TYPE_NET		= 1,
    AAA_AUTHOR_TYPE_CONN	= 2,
    AAA_AUTHOR_TYPE_SYSTEM	= 3,	/* accounting only */
    AAA_AUTHOR_TYPE_CMD		= 4	/* Must be last one */
};
#define AUTHOR_TYPE_LAST	AAA_AUTHOR_TYPE_CMD

#define NUM_AUTHOR_METHS 	7	/* include NOT_SET */
enum AUTHOR_METHOD {
    AAA_AUTHOR_METH_NOT_SET = 0,
    AAA_AUTHOR_METH_NONE,
    AAA_AUTHOR_METH_TAC_PLUS,
    AAA_AUTHOR_METH_LOCAL,
    AAA_AUTHOR_METH_IF_AUTHEN,
    AAA_AUTHOR_METH_RADIUS,
    AAA_AUTHOR_METH_KRB5_INSTANCE
};

struct author_method_list {
    boolean valid;
    enum AUTHOR_METHOD methods[NUM_AUTHOR_METHS];
};

struct authorstruct_ {
    int session_id;
    int seq_num;
    char *server_msg;
    char *server_admin_msg;
    enum AUTHOR_STATUS status;
    enum AUTHOR_TYPE type;
    hwidbtype *auth_idb;
    tt_soc *auth_tty;
    char **in_args;	/* NULL terminated */
    char **out_args;	/* NULL terminated */
};

enum ACCT_STATUS {
    AAA_ACCT_STATUS_SUCCESS	= 0x01,
    AAA_ACCT_STATUS_ERROR	= 0x02
};

enum ACCT_ACTION {
    AAA_ACCT_ACT_START_STOP,
    AAA_ACCT_ACT_WAIT_START,
    AAA_ACCT_ACT_STOP_ONLY
};

/* We use authorization methods for accounting, but the parser limits
 * the valid methods
 */
#define NUM_ACCT_METHS		3	/* TAC_PLUS, RADIUS, and NOT_SET */

struct acct_method_list {
    boolean valid;
    enum ACCT_ACTION act;
    enum AUTHOR_METHOD methods[NUM_ACCT_METHS];
};

struct acctstruct_ {
    int session_id;
    int seq_num;
    char *server_msg;
    char *server_admin_msg;
    enum ACCT_ACTION act;
    enum ACCT_STATUS status;
    tt_soc *tty;
    acctinfo *info;
    int flags;
};

/*
 * Accounting flags that are put in tacacs+/etc packets
 */
#define ACCT_FLAG_MORE		0x01
#define ACCT_FLAG_START		0x02
#define ACCT_FLAG_STOP		0x04
#define ACCT_FLAG_WATCHDOG	0x08

/*
 * Attribute strings. 
 *
 * NOTE: if you change this list, you *MUST* update
 * the tacacs+ protocol specification in CMS and also
 * the tacacs+ source distribution users_guide docs.
 */
#define AAA_STRSIZE(a) ((sizeof a) - 1)
#define AAA_ATTR_service	"service"
#define AAA_ATTR_protocol	"protocol"
#define AAA_ATTR_cmd		"cmd"
#define AAA_ATTR_cmd_arg	"cmd-arg"
#define AAA_ATTR_acl		"acl"
#define AAA_ATTR_inacl		"inacl"
#define AAA_ATTR_outacl		"outacl"
#define AAA_ATTR_addr		"addr"
#define AAA_ATTR_routing	"routing"
#define AAA_ATTR_route		"route"
#define AAA_ATTR_timeout	"timeout"
#define AAA_ATTR_idletime	"idletime"
#define AAA_ATTR_autocmd	"autocmd"
#define AAA_ATTR_noescape	"noescape"
#define AAA_ATTR_nohangup	"nohangup"
#define AAA_ATTR_priv_lvl	"priv-lvl"
#define AAA_ATTR_rem_user	"remote_user"
#define AAA_ATTR_rem_host	"remote_host"
#define AAA_ATTR_callback	"callback-dialstring"
#define AAA_ATTR_callback_noverify "nocallback-verify"
#define AAA_ATTR_callback_line	"callback-line"
#define AAA_ATTR_callback_rotary "callback-rotary"
#define AAA_ATTR_addr_pool      "addr-pool"
#define AAA_ATTR_addr_pool_default "default"
#define AAA_ATTR_zonelist	"zonelist"
#define AAA_ATTR_starttime	"start_time"
#define AAA_ATTR_stoptime	"stop_time"
#define AAA_ATTR_elapsedtime	"elapsed_time"
#define AAA_ATTR_status		"status"
#define AAA_ATTR_bytesin	"bytes_in"
#define AAA_ATTR_bytesout	"bytes_out"
#define AAA_ATTR_paksin		"paks_in"
#define AAA_ATTR_paksout	"paks_out"
#define AAA_ATTR_task_id	"task_id"
#define AAA_ATTR_timezone	"timezone"
#define AAA_ATTR_tunnel_id	"tunnel-id"
#define AAA_ATTR_nas_password	"nas-password"
#define AAA_ATTR_gw_password	"gw-password"
#define AAA_ATTR_ip_addresses	"ip-addresses"
#define AAA_ATTR_x25_addresses	"x25-addresses"
#define AAA_ATTR_frame_relay	"frame-relay"
#define AAA_ATTR_source_ip	"source-ip"
#define AAA_ATTR_old_prompts	"old-prompts"
/*
 * Maybe change when "system" accounting is normalized
 */
#define AAA_ATTR_reason		"reason"
#define AAA_ATTR_event		"event"


/*
 * Value Strings. NOTE: if you change this list, you *must* update
 * the tacacs+ protocol specification in CMS and also
 * the tacacs+ source distribution users_guide docs.
 */
#define AAA_VAL_null		""
#define AAA_VAL_true		"true"
#define AAA_VAL_false		"false"
#define AAA_VAL_SVC_shell	"shell"
#define AAA_VAL_SVC_arap	"arap"
#define AAA_VAL_SVC_ppp		"ppp"
#define AAA_VAL_SVC_slip	"slip"
#define AAA_VAL_SVC_system	"system"
#define AAA_VAL_SVC_connect	"connection"
#define AAA_VAL_SVC_ttydaemon	"tty-daemon"
#define AAA_VAL_SVC_vpdn	"vpdn"
#define AAA_VAL_PROT_lcp	"lcp"
#define AAA_VAL_PROT_ip		"ip"
#define AAA_VAL_PROT_ipx	"ipx"
#define AAA_VAL_PROT_atalk	"atalk"
#define AAA_VAL_PROT_vines	"vines"
#define AAA_VAL_PROT_xremote	"xremote"
#define AAA_VAL_PROT_tn3270	"tn3270"
#define AAA_VAL_PROT_telnet	"telnet"
#define AAA_VAL_PROT_rlogin	"rlogin"
#define AAA_VAL_PROT_lat	"lat"
#define AAA_VAL_PROT_pad	"pad"
#define AAA_VAL_PROT_osicp	"osicp"
#define AAA_VAL_PROT_deccp	"deccp"
#define AAA_VAL_PROT_nbfcp      "nbf"
#define AAA_VAL_PROT_unknown	"unknown"
#define AAA_VAL_EVNT_net_acct	"net_acct"
#define AAA_VAL_EVNT_sh_acct	"shell_acct"
#define AAA_VAL_EVNT_conn_acct	"conn_acct"
#define AAA_VAL_EVNT_cmd_acct	"cmd_acct"
#define AAA_VAL_EVNT_sys_acct	"sys_acct"
#define AAA_VAL_EVNT_clock	"clock_change"


/*
 * per-user information for AAA
 *
 */
struct per_user_info_ {
    ipaddrtype network; /* network portion of per-user route */
    ipaddrtype mask;    /* mask portion of per-user route */
    ipaddrtype gateway; /* gateway of per-user route */
};

/*
 * Data definitions for accounting
 */
#define AAA_ACCT_NAVS 30	/* Number of AV pairs in acct struct */
#define AAA_ACCT_DATASIZE 300	/* Amount of protocol-independent AV info */

struct acctinfo_ {
    struct acctinfo_ *next;	/* Accounting information */
    enum AUTHOR_METHOD method;
    short flags;
    short type;			/* Type of acct info (exec, net, etc) */
    sys_timestamp starttime;
    sys_timestamp stoptime;
    idbtype *idb;		/* Interface for NET accounting */
    int task_id;
    int  packets_in;		/* Saved start value, computed end values */
    int  packets_out;
    int  bytes_in;
    int  bytes_out;
    /*
     * Acocunting AV pairs, stored in a protocol independent manner.
     * This data array will hold the whole AV pair including value, so
     * it's longer than the array of pointers used for (eg) authorization
     * attribute/value pairs.
     */
    uchar *acct_dptr;		/* Pointer to end of data */
    uchar *acct_end_write;      /* End of data - ACCT_FLAG_START only */
    uchar acct_data[AAA_ACCT_DATASIZE];
    /*
     * info used by separate accounting info writing fork
     */
    userstruct *ustruct;	/* backward pointer */
    short rec_flags;		/* Flags for (eg) tacacs+ */
};

#define ACCT_F_STARTSENT 0x0001  /* Start record sent successfully */
#define ACCT_F_FIRSTDONE 0x0002	 /* initial packet/etc counts have been done */

enum ACTTSTR_TYPES {
    ACCT_TYPE_EXEC,		/* Exec record */
    ACCT_TYPE_CONN,		/* Connection record */
    ACCT_TYPE_NET,		/* Network record */
    ACCT_TYPE_CMD,		/* Command */
    ACCT_TYPE_SYS,		/* System events (reload/restart/etc) */
    ACCT_TYPE_MAX		/*  placeholder */
};

typedef struct aaa_acct_cfgtype_ {
    uchar doit:1;			/* generate some acccounting records */
    uchar start:1;		/* Do start as well as end acct records */
    uchar wait:1;		/* Wait for confirmation */
} acct_cfgtype;

/*
 * acct_data contains attribute value information in a protocol independent
 * format that looks sorta like this:
 */
struct acct_attr_ {
    uchar value_length;		/* length of value */
    uchar attr_ident;		/* Which attribute */
    uchar value_fmt;		/* what type */
    uchar attr_value[1];	/* value of the attribute*/
};

/*
 * Make sure attr_names[AAA_ATTR_MAX] has the corresponding
 * ascii tsrings defined also
 */
#define AAA_ATTR_SERVICE	1
#define AAA_ATTR_STARTTIME	2
#define AAA_ATTR_PORT		3
#define AAA_ATTR_ELAPSTIME	4
#define AAA_ATTR_STATUS		5
#define AAA_ATTR_PRIV		6
#define AAA_ATTR_CMD		7
#define AAA_ATTR_PROTO		8
#define AAA_ATTR_CMDARG		9
#define AAA_ATTR_BYTESIN	10
#define AAA_ATTR_BYTESOUT	11
#define AAA_ATTR_PAKSIN		12
#define AAA_ATTR_PAKSOUT	13
#define AAA_ATTR_ADDR		14
#define AAA_ATTR_TASK		15
#define AAA_ATTR_CALLBACK	16
#define AAA_ATTR_CALLBACK_NOVERIFY 17
#define AAA_ATTR_CALLBACK_LINE     18
#define AAA_ATTR_CALLBACK_ROTARY   19
#define AAA_ATTR_REASON		20
#define AAA_ATTR_EVENT		21
#define AAA_ATTR_TIMEZONE	22
#define AAA_ATTR_MAX		23      /* Maximum number of attributes */


#define AAA_TYPE_BYTE	0
#define AAA_TYPE_INT	1	/* (4 byte) number */
#define AAA_TYPE_STR	2	/* ascii string */
#define AAA_TYPE_IP     3	/* ip address */
#define AAA_TYPE_IPX	4	/* ipx address */
#define AAA_TYPE_ETHER	6	/* ethernet address */
#define AAA_TYPE_AT	7	/* Appletalk address */

#define AAA_SERV_EXEC 0
#define AAA_SERV_PPP  1
#define AAA_SERV_ARAP 2
#define AAA_SERV_SLIP 3

/*
 * Offset values for 16bit conversions of interface names
 * 00ttt	TTYttt or interface asyncttt
 * 10xxx	Ordinary interface Serialxxx
 * 2ppcc	Interface PRIpp (Serialpp), channel cc
 * 3bb0c	Interface BRIbb, channel C
 * 4zzzz	reserved
 * 5zzzz	reserved
 * 6nnss	Other. Interface ???nn:ss
 */
#define AAA_IFVAL_DIVISOR 10000
#define AAA_IFVAL_ASYNC 0
#define AAA_IFVAL_SYNC  1
#define AAA_IFVAL_PRI   2
#define AAA_IFVAL_BRI   3
#define AAA_IFVAL_OTHER 6

/*
 * Function Prototypes
 */
extern void aaa_close_connection(userstruct *u, void **handle);
extern boolean aaa_create_user (userstruct **ustruct, char *user, char *ruser,
				char *port, char *rem_addr, 
				enum AUTHEN_TYPE authen_type,
				enum AAA_SERVICES service, int priv_lvl);
extern boolean aaa_dup_user (userstruct **ustruct1, userstruct **ustruct2,
			     enum AAA_SERVICES new_service, int priv_lvl);
extern void aaa_lock_user(userstruct *ustruct);
extern void aaa_unlock_user(userstruct **ustruct);
extern void aaa_free_user (userstruct **ustruct);
extern boolean aaa_get_user_p (userstruct *ustruct, char **user);
extern boolean aaa_get_cred (userstruct *ustruct, char **user,
			     enum AUTHEN_TYPE *authen_type, 
			     enum ACC_METHOD *acc_method, void **cred,
			     char **password);
extern boolean aaa_authen_first_method(enum AAA_SERVICES, char *,
				       enum ACC_METHOD);
extern boolean aaa_dup_ifneeded(tt_soc *, char *, userstruct **,
				enum AAA_SERVICES);
extern boolean aaa_start_login (userstruct *ustruct, authenstruct *astruct,
				char *acc_list_name, 
				enum AUTH_ACTIONS action,
				enum AAA_SERVICES service);
extern boolean aaa_cont_login (userstruct *ustruct, authenstruct *astruct);
extern void aaa_cleanup_login (userstruct *ustruct, authenstruct *authen);
extern void aaa_abort_login (userstruct *ustruct, authenstruct *authen,
			     char *reason);
extern boolean aaa_is_list_defined(char *list_name, enum AAA_SERVICES service);
extern boolean aaa_simple_login_check(tt_soc *, char *user, char *pw,
				      char *remu, boolean save_ustruct,
				      enum AAA_SERVICES service);
extern boolean local_override;
extern void aaa_randomize_id(int *);

/* Authorization */
extern boolean aaa_author_needed(enum AUTHOR_TYPE type, int priv_level,
				 hwidbtype* idb, tt_soc *tty);
extern boolean aaa_do_author(userstruct *ustruct, authorstruct *astruct,
			     enum AUTHOR_TYPE type, int priv_level);
extern void aaa_cleanup_author(authorstruct *astruct);
extern boolean aaa_add_AV(char ***ptr, int *free, char *attr, char *val,
			  enum AV_TYPE type);
extern boolean aaa_split_AV(char *av_string, char **attrp, char **valp,
			    enum AV_TYPE *typep);
extern char *aaa_get_value(char **args, char *attr);
extern void aaa_clear_attrs(char ***attrsp);
extern char **aaa_copy_attrs(char **attrs);
extern char **aaa_remove_attr(char **attrs, char *attr);
extern void aaa_merge_attrs(char **attrs1, char **attrs2, char ***out_attrs);
extern void aaa_set_attr(char ***a, char *, char *, enum AV_TYPE);

/* Accounting */
extern void aaa_attr_add(acctinfo*, int, void*, int, int);
extern enum AUTHOR_METHOD aaa_acct_needed(enum AUTHOR_TYPE type, int priv_level);
#ifdef NOBODY_CALLS_ME
extern boolean aaa_do_acct(userstruct *ustruct, acctstruct *astruct,
			   enum AUTHOR_TYPE type, int priv_level);
extern void aaa_cleanup_acct(acctstruct *astruct);
#endif /* NOBODY_CALLS_ME */
extern struct acct_method_list acct_lists[];
extern char *const aaa_attr_names[];

extern void aaa_init (void);
extern void aaa_parser_init (void);

extern boolean aaa_chap_rresponse (userstruct **, authenstruct *,
				   char *, hwidbtype *, uchar *,
				   uchar, uchar *, uchar, char *);

extern boolean  aaa_chap_rchallenge (userstruct **, authenstruct *,
				     char *, hwidbtype *, 
			             uchar, uchar *, uchar, char *);

extern ushort idbname2portval (char *);

extern boolean aaa_otp_login (hwidbtype *, tt_soc *, char *, 
				      char *, char *);

#endif /* __H_AAA_H__ */


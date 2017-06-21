/* $Id: parser_actions.c,v 3.13.4.25 1996/09/11 23:07:08 snyder Exp $
 * $Source: /release/112/cvs/Xsys/parser/parser_actions.c,v $
 *------------------------------------------------------------------
 * P A R S E R _ A C T I O N S . C  (formerly A C T I O N S . C)
 *
 * Action functions for tokens.
 *
 * Copyright (c) 1991-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: parser_actions.c,v $
 * Revision 3.13.4.25  1996/09/11  23:07:08  snyder
 * CSCdi68880:  more const stuff
 *              120 out of image, 144 data
 * Branch: California_branch
 *
 * Revision 3.13.4.24  1996/09/10  03:22:27  snyder
 * CSCdi68568:  more constant opportuniites
 *              216 out of data
 *               52 out of image
 * Branch: California_branch
 *
 * Revision 3.13.4.23  1996/09/09  01:34:17  xtang
 * CSCdi56632:  unknown clock rates on 4T PA, FSIP
 * Branch: California_branch
 *         add ALT_HELPS to use csb->which in different
 *         driver procedures, and print the
 *         driver specific info. In this case, the clockrate
 *         option table, and csb->which = CFG_INT_CLOCKRATE
 *
 * Revision 3.13.4.22  1996/08/28  13:05:07  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.13.4.21  1996/08/12  16:06:32  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.13.4.20  1996/07/29  21:32:44  rchandra
 * CSCdi64437:  BGP: community display is not user-friendly
 * Branch: California_branch
 * - add user friendly community display
 * - add update throttling code (CSCdi64451)
 * - allow 'set community <additive>' during redistribution into BGP
 *
 * Revision 3.13.4.19  1996/07/19  01:33:38  snyder
 * CSCdi63443:  Dont need 2 copies of Month and Day Names
 *              Saves 220 bytes, 76 in data segment
 * Branch: California_branch
 *
 * Revision 3.13.4.18  1996/07/15  13:38:37  widmer
 * CSCdi52477:  Cannot enter dialer map in privilege level 2
 * Branch: California_branch
 * Allow entering privilege commands past NVGENS macro
 *
 * Revision 3.13.4.17  1996/07/09  21:07:42  etrehus
 * CSCdi45163:  transmitter-delay help message is confusing
 * Branch: California_branch
 *
 * Revision 3.13.4.16  1996/07/08  22:39:48  mwu
 * CSCdi60922:  11.2 Core Features Commit
 * Branch: California_branch
 *
 * Revision 3.13.4.15  1996/06/20  03:11:46  widmer
 * CSCdi56826:  ambiguous commands in EXEC mode
 * Branch: California_branch
 * Make Priv_Nohelp keywords act more like Priv_Hidden
 *
 * Revision 3.13.4.14  1996/06/17  23:36:09  hampton
 * Implement the long sought parser "default" keyword.  [CSCdi60510]
 * Branch: California_branch
 *
 * Revision 3.13.4.13  1996/06/17  08:35:24  jhernand
 * Commit Modular Cobra to California_branch
 *
 * Revision 3.13.4.12  1996/05/30  22:44:25  hampton
 * Add parser support for accepting per-protocol access list number
 * ranges.  [CSCdi59067]
 * Branch: California_branch
 *
 * Revision 3.13.4.11  1996/05/21  09:59:20  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.13.4.10  1996/05/19  00:24:49  rpratt
 * CSCdi57306:  HTML config has problems in new platform
 * Branch: California_branch
 *
 * Revision 3.13.4.9  1996/05/17  11:39:33  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.12.2.5  1996/05/05  23:41:50  ppearce
 * Sync to IbuMod_Calif_baseline_960504
 *
 * Revision 3.12.2.4  1996/04/03  21:12:58  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.12.2.3  1996/04/01  04:41:52  bchan
 * CSCdi53188:  appn parser multiple definition
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.12.2.1  1996/03/17  18:19:21  ppearce
 * Sync IbuMod_Calif_branch to V111_1_3
 *
 * Revision 3.13.4.8  1996/05/13  15:17:30  widmer
 * CSCdi57327:  novell keyword not hidden by PROTO* parser macros
 * Branch: California_branch
 *
 * Revision 3.13.4.7  1996/05/04  01:36:54  wilber
 * Branch: California_branch
 *
 * Integrated Routing and Bridging
 * - Bridge-group Virtual Interface parser/configuration support.
 *
 * Revision 3.13.4.6  1996/04/27  06:39:08  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 3.13.4.5  1996/04/23  16:48:12  widmer
 * CSCdi55084:  chat-script does not take back slash /.
 * Branch: California_branch
 *
 * Revision 3.13.4.4  1996/04/23  16:46:53  widmer
 * CSCdi54787:  New string escape sequence parsing breaks old configs
 * Branch: California_branch
 *
 * Revision 3.3.2.6  1996/04/26  01:22:12  syiu
 * Branch: Dial1_branch
 * Dial1_branch to IosSec_postmerge_960419
 *
 * Revision 3.3.2.5  1996/04/17  03:58:10  syiu
 * Branch: Dial1_branch
 * Sync Dial1_branch to V111_1_3
 *
 * Revision 3.3.2.4  1996/04/06  21:24:13  dblair
 * Branch: Dial1_branch
 * Fix CSCdi53768 and added restart lcp when ppp multilink command
 * processed, and fix mutlilink not negotiated at reload problem.
 *
 * Revision 3.3.2.3  1996/03/01  22:26:37  perryl
 * Branch: Dial1_branch
 * Disallow UI configuration for virtual access interface.
 * Allow show and clear command for virtual access interface.
 * Disallow clear command for virtual template interface.
 *
 * Revision 3.3.2.2  1996/02/23  19:45:43  perryl
 * Branch: Dial1_branch
 * Increase the size of help buffer in interface_action to accommodate
 * virtual-template.
 * Correct the max unit for virtual-template.
 *
 * Revision 3.3.2.1  1996/01/18  05:00:01  perryl
 * Branch: Dial1_branch
 * Virtual Access interface services
 *
 * Revision 3.13.4.3  1996/04/15  21:20:38  bcole
 * CSCdi54649:  IP Host Modularity, IP Automatic Stub Routing feature
 * commit
 * Branch: California_branch
 *
 * Revision 3.13.4.2  1996/04/08  22:07:18  john
 * CSCdi53685:  Allow encoded characters in parser string macro
 * Branch: California_branch
 *
 * Revision 3.13.4.1  1996/03/18  21:33:37  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.6.2.5  1996/03/16  07:22:35  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.6.2.4  1996/03/14  01:22:39  gstovall
 * Branch: DeadKingOnAThrone_branch
 * The King is now ready for action.
 *
 * Revision 3.6.2.3  1996/03/13  01:51:37  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.6.2.2  1996/03/07  10:40:15  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.6.2.1  1996/02/20  16:47:59  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.13  1996/03/09  22:57:00  widmer
 * CSCdi47890:  HTTP server doesnt work
 * Add HTTP server
 *
 * Revision 3.12  1996/02/22  22:37:07  widmer
 * CSCdi47731:  sh serial and sh controller serial ? give wrong number of
 * ports
 * Back out CSCdi34558
 *
 * Revision 3.11  1996/02/21  17:52:34  widmer
 * CSCdi48011:  parser range validation of interface values is incorrect
 * Set max unit for channel interfaces to 2
 *
 * Revision 3.10  1996/02/20  18:02:16  widmer
 * CSCdi49363:  Drop function name from INVALID_NVGEN_CHECK
 *
 * Revision 3.9  1996/02/16  22:42:30  widmer
 * CSCdi49239:  Make time and timesec real parser macros
 *
 * Revision 3.8  1996/02/07  21:15:25  widmer
 * CSCdi48343:  Remove unused #ifdef STANDALONE from parser code
 *
 * Revision 3.7  1996/02/01  06:08:01  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.6  1996/01/22  06:58:01  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.5  1996/01/16  21:18:06  widmer
 * CSCdi33940:  show extended command doesnt work
 * Add keyword_trans macro that doesn't accept the keyword
 *  unless commands have been added to the link point.
 *
 * Revision 3.4  1996/01/16  09:33:50  billw
 * CSCdi46868:  latgroup.o should not be in core subsystem
 * oops.  Forgot one.
 *
 * Revision 3.3  1996/01/11  03:51:24  kao
 * CSCdi43878:  IS_VIP() macro should be used
 *
 * Revision 3.2  1995/12/17  18:35:18  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.1  1995/12/15  03:46:18  thille
 * Parser organizational cleanup.  Rename core parser files to be
 * parser_<foo>.  Start by adding placeholders for the new filenames.
 *
 * OLD LOG as actions.c:
 * ---------------------
 * Revision 3.12  1995/12/14  22:59:37  widmer
 * CSCdi45606:  Unit_Number macro does not work with subif flag
 *
 * Revision 3.11  1995/12/14  13:07:35  widmer
 * CSCdi45510:  Add General_String macro to replace other string macros
 * Change Var_String, String, Txt_Line, and Text to use General_String
 * Add fail argument to Priv_Test
 * Change Neg1 to use Keyword_Id
 * Remove Sense macro
 *
 * Revision 3.10  1995/12/03  17:59:16  thille
 * CSCdi44905:  ciscopro targets not in 11.1
 * Put them in.
 *
 * Revision 3.9  1995/12/03  16:42:25  widmer
 * CSCdi44243:  privilege exec level 0 help doesnt show up in config
 * Add sense to parsenode
 * Change help command to privilege level 0
 *
 * Revision 3.8  1995/11/28  20:26:32  widmer
 * CSCdi44264:  Change save_line() to use variable-length arguments
 * Remove dead code also.
 *
 * Revision 3.7  1995/11/28  19:50:45  widmer
 * CSCdi44251:  Replace Ifelse(csb->in_help)/Eval with Func
 *
 * Revision 3.6  1995/11/27  21:24:51  widmer
 * CSCdi43607:  Add keyword_option macro to parse multiple keywords
 * Also create permit_deny macro using keyword_option macro to parse
 * permit/deny keywords.
 * Change "ip security multilevel" command to use keyword_option macro.
 *
 * Revision 3.5  1995/11/22  21:46:53  widmer
 * CSCdi42888:  Cannot place  at beginning of cud field for x25 host
 * command
 * add %S to print out strings that are parsed by STRING macro.
 * Change x25 cud NV generation to use %S
 * Change STRING macro to escape quotes with \"
 *
 * Revision 3.4  1995/11/20  23:09:58  wmay
 * CSCdi44029:  Fast ethernet doesnt appear in slot order on high
 * end - include in HW_INTS list, not AFTER_HW_INT
 *
 * Revision 3.3  1995/11/17  18:41:51  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:20:08  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:49:25  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.12  1995/11/08  21:20:00  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.11  1995/10/20  00:48:50  wmay
 * CSCdi39845:  show interface displays serials in a non-intuitive manner
 * Forgot Group async interfaces in iftype list.
 *
 * Revision 2.10  1995/10/18  17:23:16  wmay
 * CSCdi39845:  show interface displays serials in a non-intuitive manner
 * Sort high end idb's by slot/slotunit/vc, low end by unit/vc.
 *
 * Revision 2.9  1995/10/04  16:04:55  tkolar
 * CSCdi34979:  Parser conflict when creating an interface on the fly
 * Don't copy IFNAME into SETOBJ(string,1)
 *
 * Revision 2.8  1995/10/03  00:56:39  irfan
 * CSCdi37688:  garbage in show lines output
 * fix max range value in ttyline_action()
 *
 * Revision 2.7  1995/08/25  15:26:10  widmer
 * CSCdi38824:  terminal download does not play nice with privleges.
 * Enable changing privilege of terminal only commands.
 *
 * Revision 2.6  1995/08/06  14:23:52  jacobt
 * CSCdi36607:  need show memory summary command
 *
 * Revision 2.5  1995/08/03  21:52:03  hampton
 * Convert the parser to use the chunk manager.  This should allow the
 * parser to continue to work in low memory situations.  [CSCdi38208]
 *
 * Revision 2.4  1995/06/21 09:00:18  smackie
 * Rename malloc memory pool derivatives so that they're prefixed with
 * malloc_ for consistency. (CSCdi36222)
 *
 * Revision 2.3  1995/06/20  06:39:57  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Invoke a "show mem fail alloc" when the parser can't get enough memory
 * to parse a command.
 *
 * Revision 2.2  1995/06/15  15:19:06  widmer
 * CSCdi35889:  parser transition structures should be static
 *
 * Revision 2.1  1995/06/07  22:00:41  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* XXX There are several calls to copy_varstring() where the length
 * XXX of the source string is not strictly checked against the dest
 * XXX string length.
 */

#include "master.h"
#include "../ui/common_strings.h"
#include <ctype.h>
#undef   toupper			/* use library function call */
#include <string.h>
#include <ciscolib.h>
#include "interface_private.h"
#include "ttysrv.h"
#include "packet.h"
#include "../os/chunk.h"
#include "config.h"
#include "parser.h"
#include "logger.h"
#include "../if/network.h"
#include "sys_registry.h"
#include "../dev/flash_registry.h"
#include "../parser/msg_parser.c"	/* Not a typo, see logger.h */
#include "../parser/parser_defs_parser.h"
#include "../les/parser_defs_hub.h"
#include "../parser/parser_actions.h"
#include "connect.h"		/* conn_vectorQ */
#include "../iprouting/route.h"	/* PROC_* */
#include "../ui/service.h"
#include "../parser/parser_privilege.h"
#include "../ip/ip.h"
#include "../os/free.h"
#include "../lat/lat_public.h"
#include "../if/if_vtemplate.h"
#include "ctype.h"
#include "../tcp/http.h"
#include "../appn/appn_subsys.h"
#include "../os/clock.h"

/*
 * Interface type structure used to select interface types
 * in specific contexts.
 */
typedef struct iftype_ {
    uint64 type;
    const char *name;
    const char *descr;
    uint base_unit;		/* 0 for most, 1 for IDBTYPE_ASYNC, */
				/* VTYBase for IDBTYPE_VTY_ASYNC */
    int max_unit;		/* max unit number */
    int max_vc;
    boolean dynamic_interface;	/* TRUE if dynamic interface, FALSE if not */
    boolean subinterfaces_allowed;
    uint listplace;
} iftype;


#define	CHECK_RANGE_BOUNDS()\
    if (arg->upper >= arg->lower) {\
	lower = arg->lower;\
	upper = arg->upper;\
    } else {\
	errmsg(&msgsym(BADRANGE, PARSER), arg->lower, arg->upper,\
	       (csb->nvgen ? csb->nv_command : csb->line));\
	lower = arg->upper;\
	upper = arg->lower;\
    }

#define	CHECK_RANGE_BOUNDS_ALIST()\
    if (arg->upper >= arg->lower) {\
	lower = arg->lower;\
	upper = arg->upper;\
    } else {\
	errmsg(&msgsym(BADRANGE, PARSER), arg->lower, arg->upper,\
	       (csb->nvgen ? csb->nv_command : csb->line));\
	lower = arg->upper;\
	upper = arg->lower;\
    }\
    if (arg->upper2 >= arg->lower2) {\
	lower2 = arg->lower2;\
	upper2 = arg->upper2;\
    } else {\
	errmsg(&msgsym(BADRANGE, PARSER), arg->lower2, arg->upper2,\
	       (csb->nvgen ? csb->nv_command : csb->line));\
	lower2 = arg->upper2;\
	upper2 = arg->lower2;\
    }

#define PA_HELP_STRING	"Port Adaptor number"


/*
 *  Local variables
 */
keyword_options permitdeny_options[] = {
    { "deny", "Specify packets to reject", FALSE },
    { "permit", "Specify packets to forward", TRUE },
    { NULL, NULL, 0 }
};

/*
 * Flag which lines have received configuration as a group.  When NVGEN'ing,
 * this permits their parse chain generation to be skipped.
 */
ulong tty_cfg_rotor;

/*
 * Global definition of "<cr>", shared by local function and AAA command
 * authorization.
 */
char *parser_eol_string = "<cr>";

/*
 *  Forward declarations
 */
static void parser_bail_out(parseinfo *, void (*)(parseinfo *), const char *);
static void set_error_index(parseinfo *);
static idbtype *find_interface(parseinfo *, iftype *, uint,uint,uint,uint, int);
static char *hex_to_byte(char *s, char *out);
static boolean match_interface_unit(parseinfo *, iftype *, int *, uint*,
				    uint *, uint *, uint *,
				    interface_struct * const, char *);
static boolean match_subcard_in_slot(uint slot);
static boolean match_spaces(char *buf, int *pi);
static boolean match_grouplist(char *buf, int *pi, char *num);
static boolean match_signed_dec(char *, int, int, int *, int *);
static boolean match_general_addr(parseinfo *, int *, void *,
				  uint, uint, const char *);
static boolean process_partial_protocol_action(parseinfo *, int *,
					       struct rtrname *,
					       routerprotocolinfo *,
					       routerparse_struct *const);
static void save_ambig_int(parseinfo *, const char *, int);
static void save_ambig_uint(parseinfo *, const char *, uint);
static void save_ambig_nstring(parseinfo *, const char *, const char *, uint);
static void save_ambig_string_flag(parseinfo *, const char *,
				   const char *, boolean); 
static void save_help_short(parseinfo *, const char *, boolean);
static void save_help_msg(parseinfo *, const char *);
static void nvgen_action(parseinfo *, transition *, void (*)(parseinfo *));

/* The parser match routines use simpler C routines to match the smaller
 * pieces of tokens.  These sub-routines come in two flavors:
 *
 * XXX_match	returns (int) the number of chars it matched;
 *		use conclude_match to back over a successful match
 * XXX_check	returns (boolean) the status of the check;
 *		consumes no input
 */

/* These simple match and check routines could be made inline:
 *
 * terminator_check	Did user hit RETURN, start a comment, or request help?
 * eol_check		Did user hit RETURN or start a comment here?
 * help_check		Did user request help here?
 *			Don't use this directly, try one of the next ones;
 * long_help_check	Did user request long help here?
 *			i.e, is this BOL or was last char W/S?
 * short_help_check	Did user request short help here?
 *			Opposite of long_help_check
 */

static boolean terminator_check (parseinfo *csb)
{
    return(is_terminator(csb->line[csb->line_index]));
}

boolean eol_check (parseinfo *csb)
{
    return(!csb->in_help && terminator_check(csb));
}

boolean help_check (parseinfo *csb)
{
    return(csb->in_help && !csb->nvgen && terminator_check(csb));
}

static boolean long_help_check (parseinfo *csb)
{
    return(help_check(csb) &&
	(csb->line_index == 0 || isspace(csb->line[csb->line_index - 1])));
}

static boolean short_help_check (parseinfo *csb)
{
    return(help_check(csb) && !long_help_check(csb));
}

static boolean priv_check (parseinfo *csb, trans_priv *priv)
{
    /*
     * Only process keywords which are visible to the user at their current
     * priv level.
     */
    if (csb->priv < priv->cur_priv) {
	return(FALSE);
    }

    if ((TRANS_PRIV_FLAGS_GET(priv->flags) & PRIV_INTERACTIVE) &&
	(csb->flags & CONFIG_HTTP)) {
	return(FALSE);
    }

    /* Modify the current keyword privilege
     * Don't allow internal commands unless they are enabled.
     */

    if ((TRANS_PRIV_FLAGS_GET(priv->flags) & PRIV_INTERNAL) &&
	!internal_cmd_enable) {
	return(FALSE);
    }

    /*
     * Don't allow keywords on subinterfaces unless they're supported.
     */
    if ((csb->mode == subinterface_mode) &&
	csb->interface &&
	is_subinterface(csb->interface) &&
	(!(TRANS_PRIV_FLAGS_GET(priv->flags) & PRIV_SUBIF))) {
	return(FALSE);
    }
    if (csb->in_help) {
	/* Don't display keywords which are hidden */
	if (TRANS_PRIV_FLAGS_GET(priv->flags) &
	    (PRIV_HIDDEN | PRIV_UNSUPPORTED)) {
	    return(FALSE);
	}
    }
    /* Don't do NV generation on keywords that are obsolete */
    if (csb->nvgen &&
	!csb->priv_set &&
	(TRANS_PRIV_FLAGS_GET(priv->flags) & PRIV_NONVGEN)) {
	return(FALSE);
    }
  
    if ((TRANS_PRIV_FLAGS_GET(priv->flags) &
	 (PRIV_HIDDEN | PRIV_UNSUPPORTED)) ||
	((TRANS_PRIV_FLAGS_GET(priv->flags) & PRIV_NOHELP) &&
	 (csb->in_help == PARSER_NO_HELP))) {
	csb->command_visible = FALSE;
    }
    if (TRANS_PRIV_FLAGS_GET(priv->flags) & PRIV_UNSUPPORTED) {
	csb->unsupported = TRUE;
    }
    return(TRUE);
}

static void keyword_flag_modify (parseinfo *csb, trans_priv *priv, uint *flags)
{
    /* If this is a duplicate keyword, as indicated by PRIV_DUPLICATE in
     * the keyword privileges, then reset KEYWORD_HELP_CHECK.  This will
     * prevent match_partial_keyword() from supplying any help for this
     * keyword.
     */

    if ((TRANS_PRIV_FLAGS_GET(priv->flags) & PRIV_DUPLICATE) ||
	(TRANS_PRIV_FLAGS_GET(priv->flags) & PRIV_NOHELP) ||
	((TRANS_PRIV_FLAGS_GET(priv->flags) & PRIV_USER_HIDDEN) &&
	 (csb->priv == PRIV_USER) &&
	 !stdio->tty_help)) {
	*flags &= ~KEYWORD_HELP_CHECK;
    }
}

#define	PARSER_HTTP_FORM_UNKOWN		0
#define	PARSER_HTTP_FORM_NUMBER		1
#define	PARSER_HTTP_FORM_ADDRESS	2
#define	PARSER_HTTP_FORM_LINE		3
#define	PARSER_HTTP_FORM_WORD		4

static void parser_http_form (parseinfo *csb, uint type,
			      const char *short_help, const char *long_help)
{
    const char *action;
    const char *fill;
    const char *help;

    switch (type) {
      case PARSER_HTTP_FORM_NUMBER:
	action = "NUMBER";
	fill = " ";
	help = short_help;
	break;
      case PARSER_HTTP_FORM_ADDRESS:
	action = "ADDRESS";
	fill = " ";
	help = short_help;
	break;
      case PARSER_HTTP_FORM_LINE:
	action = "LINE";
	help = fill = "";
	break;
      case PARSER_HTTP_FORM_WORD:
      default:
	action = "WORD";
	help = fill = "";
	break;
    }

    /* End CLI Form first, and start new form after */
    save_line(csb, &csb->help_save,
	      "</FORM><DT><FORM METHOD=POST ACTION=%s/%s>"
	      "<INPUT TYPE=SUBMIT VALUE=%s> %s%s"
	      "<INPUT TYPE=TEXT NAME=%s>"
	      "</FORM><DD>%s\t<FORM>",
	      csb->http_state->uri, action, action,
	      help, fill, action, long_help);
}

boolean match_char (char *cp, int *offset, char c)
{
    if (*(cp + *offset) == c) {
	(*offset)++;
	return(TRUE);
    }
    return(FALSE);
}

/*
 * In effect, combines the match_whitespace ... csb->line_index += j; if
 * (j | eol_check) sequences.
 */
boolean match_whitespace2 (char *cp, int *offset, boolean in_help)
{
    if (isspace(cp[*offset])) {
	while (isspace(cp[*offset])) {
	    (*offset)++;
	}
	return(TRUE);
    }
    if ((!in_help) && is_terminator(cp[*offset])) {
	return(TRUE);
    }
    return(FALSE);
}

boolean get_name (char *buf, char *name, int *pi, int buflen)
{
    char *sp, *dp;
    int i;

    /* Copy the name into local working storage */
    sp = buf;
    dp = name;
    for (i = 0; i < buflen; i++) {
	if (*sp == ' ' || *sp == '\0') {
	    break;
	}
	*dp++ = *sp++;
    }
    *dp = '\0';
    if (i == 0)
	return(FALSE);
    if (i == buflen && (*sp != ' ' || *sp != '\0')) {
	return(FALSE);
    }

    *pi = i;
    return(TRUE);
}

/* NONE does just what its name implies */

void NONE_action (parseinfo *csb)
{
}

/* link_point is a place holder.  It should never be reached. */

void link_point_action (parseinfo *csb)
{
    errmsg(&msgsym(LINKPOINT, PARSER));
}

/*
 * no_alt is used at the end of a list of alternative tokens.
 *
 * There are several actions performed in no-alt-action, depending on the
 * state of the parser.
 *
 * If performing NV generation, we return(i.e. do nothing).
 *
 * If providing help, we must check to see if more than one command has
 * provided help (i.e. the command is ambiguous.)  However, we can have
 * multiple options generated by a single command.  We keep track of all
 * this by storing, each time through no_alt, the length of the help text
 * generated so far.  If the length has increased since the last no_alt,
 * we increment csb->multiple_funcs.  Thus we count how many token chains
 * have generated help.
 *
 * If no-alt-action is called when setting keyword privileges, then
 * don't treat it as an error.  Increment csb->multiple_funcs so that
 * token chains that have to check for previously matched tokens
 * will work correctly.
 *
 * When no-alt-action is encountered in normal processing, increase
 * error_index to our current line position if it's smaller.
 * This indicates the deepest character in the command line we reached.
 * parse_cmd() checks csb->multiple_funcs first to determine if an error
 * occured.  It will be zero if no command match occurred, and greater
 * than one if the command input is ambiguous.  If no command matched,
 * error_index is deepest character we reached in the parse, and tells
 * where to put an error marker.
 */

void no_alt_action (parseinfo *csb)
{
    ambig_struct *ap;

    if (csb->command_visible) {
	ap = &csb->visible_ambig;
    } else {
	ap = &csb->hidden_ambig;
    }

    /* NV generation */
    if (csb->nvgen) {
	return;
    }

    /*
     * Priv set processing.  If csb->priv_set is not PRIV_NULL, then don't
     * treat being called as an error.  Increment csb->multiple_funcs and
     * return.
     */
    if (csb->priv_set) {
	increment_multiple_funcs(csb, "no_alt_action 1");
	return;
    }

    /* Help processing.  Check the ambiguity buffers against one another.
     * If ambig is non-null, we've matched some input and need to
     * compare against ambig_save to see if it is ambiguous.
     * This yields two cases:
     * 1. ambig_save == NULL, indicating that this is the first match
     *    and that multiple_funcs should be incremented.
     * 2. ambig_save != NULL, indicating that this is ambiguous and
     *    that multiple_funcs should be incremented again.
     */
    if (parser_ambig_debug) {
	buginf("\n1 Ambig       buf '%s'", ap->ambig);
	buginf("\n1 Ambig_save  buf '%s'", ap->ambig_save);
    }
/***************************************************************************
 ***************************************************************************
 *
 *
 *  Before you touch the ambiguity code, make sure you know what you
 *  are doing, or else it will break
 *
 *  Here are some cases to case the behavior of the code

en

! output should "interface  internals"
show ip ospf int?

! ambiguous
show ip ospf int ?

conf term
! output should be "x25  x29  xns"
service pad
x?
! ambiguous
x ?
! output should be "x25  x29"
x2?
! ambiguous
x2 ?
! output should be "route  routing"
xns rout?
! ambiguous
xns rout ?
! output should be "default-gateway  default-network"
ip default?
! ambiguous
ip default ?

end
!
!
! To test the hidden keyword ambiguity stuff
! output should be "where"
disable
w?
! should match "where"
w
! this command should also work
who
!
! output should be "snap"
enable
conf term
interface Ethernet 0
arp s?
! should match "arp snap"
arp s
! this command should also work
arp smds
!
! broken
slip /compressed ?

 *
 *
 *
 ***************************************************************************
 ***************************************************************************/
 
    if (ap->ambig[0]) {
	if (ap->ambig_save[0]) {
	    uint i = strlen(ap->ambig_save);
	    boolean ambig_save_longer = FALSE;

	    if (strlen(ap->ambig) < i) {
		ambig_save_longer = TRUE;
		i = strlen(ap->ambig);
	    }

	    if (strncmp(ap->ambig, ap->ambig_save, i) != 0) {
		if (parser_ambig_debug) {
		    buginf("\nAmbig case 1");
		}
		/*
		 *  If there is something in the ambig_save buffer
		 *  and it is NOT the same as the ambig_save
		 *  buffer, so the keyword is matches or and we
		 *  increase multiple_funcs and copy the ambig
		 *  buffer to ambig_save buffer.
		 */
		increment_multiple_funcs(csb, "no_alt_action 2");

		/* Copy everything */
		strcpy(ap->ambig_save, ap->ambig);

		/* Clear ambig */
		ap->ambig[0] = '\0';

	    } else {
		if (parser_ambig_debug) {
		    buginf("\nAmbig case 2");
		}
		/*
		 *  If there is something in the ambig buffer
		 *  and it is the same as in the ambig_save buffer,
		 *  then we have a duplicate keyword and everything
		 *  is ok, but we need to copy the longer string to
		 *  ambig_save.
		 */
		if (!ambig_save_longer) {
		    strcpy(ap->ambig_save, ap->ambig);
		}
	    }
	} else {
	    if (parser_ambig_debug) {
		buginf("\nAmbig case 3");
	    }
	    /*
	     *  We didn't match anything before, so this is
	     *  the first match, and multiple_funcs has already
	     *  been set.
	     */
	    if (csb->in_help) {
		increment_multiple_funcs(csb, "no_alt_action 3");
	    }

	    /* Copy everything */
	    strcpy(ap->ambig_save, ap->ambig);

	    /* Clear ambig */
	    ap->ambig[0] = '\0';

	}
    } else {
	if (parser_ambig_debug) {
	    buginf("\nAmbig case 4");
	}
	/*
	 *  If the ambig buffer is empty,
	 *  then we are at the end of a parse chain,
	 *  so clear the ambig_save buffer
	 */
	ap->ambig_save[0] = '\0';
    }
    if (parser_ambig_debug) {
	buginf("\n2 Ambig       buf '%s'", ap->ambig);
	buginf("\n2 Ambig_save  buf '%s'", ap->ambig_save);
    }
    if ((parser_help_debug && csb->in_help) || parser_ambig_debug) {
	print_multiple_funcs(csb);
	buginf("\nCurrent help string:\n");
	print_buf(csb->help_save, TRUE, TRUE);
	buginf("\n");
    }
    /* Normal Error processing */
    set_error_index(csb);
}

/*
 * NOP - Generally used as a label
 *
 * Differs from NONE action in that it takes it's accepting state.
 */

void NOP_action (parseinfo *csb, transition *mine)
{
    push_node(csb, mine->alternate);
    push_node(csb, mine->accept);
};

/*
 * KEYWORD matches a unique substring of a specified string
 *
 * arg->minmatch is the minimum required number of matching characters
 *
 */
void keyword_action (parseinfo *csb, transition *mine,
		     keyword_struct * const arg)
{
    general_keyword_struct argout = {
	arg->str, arg->help, arg->priv, MAXUINT, 0, 0,
	(KEYWORD_WS_OK | KEYWORD_HELP_CHECK)};
    general_keyword_action(csb, mine, &argout);
}

void keyword_optws_action (parseinfo *csb, transition *mine,
			   keyword_struct * const arg)
{
    general_keyword_struct argout = {
	arg->str, arg->help, arg->priv, MAXUINT, 0, 0,
	(KEYWORD_WS_OK | KEYWORD_NO_WS_OK | KEYWORD_HELP_CHECK)};
    general_keyword_action(csb, mine, &argout);
}

void keyword_nows_action (parseinfo *csb, transition *mine,
			  keyword_struct * const arg)
{
    general_keyword_struct argout = {
	arg->str, arg->help, arg->priv, MAXUINT, 0, 0,
	(KEYWORD_NO_WS_OK | KEYWORD_HELP_CHECK)};
    general_keyword_action(csb, mine, &argout);
}

void keyword_mm_action (parseinfo *csb, transition *mine,
			keyword_mm_struct * const arg)
{
    general_keyword_struct argout = {
	arg->str, arg->help, arg->priv, MAXUINT, 0, arg->minmatch,
	(KEYWORD_WS_OK | KEYWORD_HELP_CHECK)};
    general_keyword_action(csb, mine, &argout);
}

void keyword_id_action (parseinfo *csb, transition *mine,
			keyword_id_struct * const arg)
{
    general_keyword_struct argout = {
	arg->str, arg->help, arg->priv, arg->offset, arg->val, 0,
	(KEYWORD_WS_OK | KEYWORD_HELP_CHECK)};
    general_keyword_action(csb, mine, &argout);
}

void keyword_id_mm_action (parseinfo *csb, transition *mine,
			keyword_id_mm_struct * const arg)
{
    general_keyword_struct argout = {
	arg->str, arg->help, arg->priv, arg->offset, arg->val, arg->minmatch,
	(KEYWORD_WS_OK | KEYWORD_HELP_CHECK)};
    general_keyword_action(csb, mine, &argout);
}

void keyword_orset_action (parseinfo *csb, transition *mine,
			keyword_id_struct * const arg)
{
    general_keyword_struct argout = {
	arg->str, arg->help, arg->priv, arg->offset, arg->val, 0,
	(KEYWORD_WS_OK | KEYWORD_HELP_CHECK | KEYWORD_OR_SET)};
    general_keyword_action(csb, mine, &argout);
}

void keyword_ortest_action (parseinfo *csb, transition *mine,
			keyword_id_struct * const arg)
{
    general_keyword_struct argout = {
	arg->str, arg->help, arg->priv, arg->offset, arg->val, 0,
	(KEYWORD_WS_OK | KEYWORD_HELP_CHECK | KEYWORD_OR_SET | KEYWORD_TEST)};
    general_keyword_action(csb, mine, &argout);
}

void keyword_trans_action (parseinfo *csb, transition *mine,
			   keyword_struct * const arg)
{
    if (mine && mine->accept && mine->accept->accept &&
	(mine->accept->accept->alternate != &pname(link_trans))) {
	keyword_action(csb, mine, arg);
    } else {
	push_node(csb, mine->alternate);
    }
}

void general_keyword_action (parseinfo *csb, transition *mine,
			     general_keyword_struct * const arg)
{
    int i = 0;
    uint flags;


    /* Setup to check the alternates. */
    push_node(csb, mine->alternate);

    if ((arg->flags & KEYWORD_TEST) &&
	(arg->offset != MAXUINT)) {
	if (arg->flags & KEYWORD_OR_SET) {
	    if (*CSBVAR(csb, arg->offset, uint) & arg->val) {
		return;
	    }
	} else {
	    if (*CSBVAR(csb, arg->offset, uint)) {
		return;
	    }
	}
    }

    /* Check for privilege to use this keyword */
    if (priv_check(csb, arg->priv) == FALSE) {
	return;
    }

    if (csb->nvgen) {
	/*
	 * Doing NV generation.  Save the transition struct and the keyword
	 * text for the NV generation function.
	 */

	/*
	 * This function check if the config of current
	 * keyword should be distilled, and perform processing
	 * if yes.
	 */
	reg_invoke_proc_flash_config(csb, *(uint *)(arg->priv));

	if (arg->offset != MAXUINT) {
	    if (arg->flags & KEYWORD_OR_SET) {
		*CSBVAR(csb, arg->offset, uint) |= arg->val;
	    } else {
		*CSBVAR(csb, arg->offset, uint) = arg->val;
	    }
	}
	if (!csb->priv_set || nvgen_privilege(csb, arg->priv, arg->str)) {
	    nvgen_token(csb, mine, arg->str);
	}
	return;
    }

    /* Make a copy of the flags word because it may originate in ROM
     * and we may need to modify the flags before matching the keyword.
     */
    flags = arg->flags;
    keyword_flag_modify(csb, arg->priv, &flags);

    /*
     * If we are at the end of the input stream and we're doing help, add the
     * long help message, which is just the concatenation of the search string
     * and the help text. We fall-through in any case.  If long help is being
     * requested, nothing matches and we return.
     * 
     * Otherwise, we are expecting to process a token. If the token matching
     * fails, due to an unexpected end of the input stream or due to a
     * different command, we will fall through the test and transition to the
     * alternates.
     *
     * The alternates are processed in any case so that we
     * exhaustively check all possible commands against the input
     * stream.  See the parser README for more information on how the
     * commands are uniquely matched.
     */

    /*
     * Check to see if the input matches this token.  If there is no match,
     * we skip processing this token and try the alternates.
     */

    i = 0;
    if (match_partial_keyword(csb, arg->str, &i, arg->help,
			      arg->minmatch, flags)) {
	csb->line_index += i;

	if (csb->priv_set) {
	    /*
	     * Modify the current keyword privilege
	     */
	    priv_push(csb, arg->priv);
	}
	csb->last_priv = arg->priv->cur_priv;
	if (csb->last_priv > csb->highest_priv) {
	    csb->highest_priv = csb->last_priv;
	}
	if (arg->offset != MAXUINT) {
	    if (arg->flags & KEYWORD_OR_SET) {
		*CSBVAR(csb, arg->offset, uint) |= arg->val;
	    } else {
		*CSBVAR(csb, arg->offset, uint) = arg->val;
	    }
	}
#ifdef	DEBUG_KEYWORD
	printf("\nkeyword_action: Pushing accept");
#endif	DEBUG_KEYWORD
	push_node(csb, mine->accept);
	csb->line_index -= i;
    } else {
	if (parser_ambig_debug && i) {
	    buginf("\nDidn't match keyword '%s'", arg->str);
	}
#ifdef	DEBUG_KEYWORD
	printf("\nkeyword_action: match_partial_keyword returned FALSE");
#endif	DEBUG_KEYWORD
    }
}
  
void keyword_option_action (parseinfo *csb,
			    transition *mine,
			    keyword_option_struct * const arg)
{
    uint i;
    uint o;
    uint *var = CSBVAR(csb, arg->offset, uint);
    boolean matched;
    uint flags = KEYWORD_HELP_CHECK | KEYWORD_WS_OK;

    push_node(csb, mine->alternate);

    INVALID_NVGEN_CHECK;

    if (priv_check(csb, arg->priv) == FALSE) {
	return;
    }

    keyword_flag_modify(csb, arg->priv, &flags);

    o = 0;
    matched = FALSE;
    while (arg->options[o].keyword && !matched) {
	if (arg->flags & KEYWORD_TEST) {
	    if (arg->flags & KEYWORD_OR_SET) {
		if (*var & arg->options[o].val) {
		    o++;
		    continue;
		}
	    } else {
		if (*var) {
		    o++;
		    continue;
		}
	    }
	}
	i = 0;
	if (match_partial_keyword(csb, arg->options[o].keyword, &i,
				  arg->options[o].help, 0, flags)) {
	    csb->line_index += i;
	    if (csb->priv_set) {
		priv_push(csb, arg->priv);      /* change priv level */
	    }
	    csb->last_priv = arg->priv->cur_priv;
	    if (csb->last_priv > csb->highest_priv) {
		csb->highest_priv = csb->last_priv;
	    }
	    if (arg->offset != MAXUINT) {
		if (arg->flags & KEYWORD_OR_SET) {
		    *var |= arg->options[o].val;
		} else {
		    *var = arg->options[o].val;
		}
	    }
	    push_node(csb, mine->accept);
	    csb->line_index -= i;
	    matched = TRUE;
	} else {
	    if (parser_ambig_debug) {
		buginf("\nDidn't match keyword '%s'", arg->options[o].keyword);
	    }
	}
	o++;
    }
}

/*
 * INTERFACE matches a given interface name with an existing interface
 *
 * Note that interface Null0 is handled specially in this code.  It cannot
 * be added to the idbQ because other portions of the router code
 * walk the queue and depend on only 'real' interfaces being on the queue.
 * The global idbtype *nullidb references Null0's idb.
 */

/* This array really shouldn't be here.  The list should be global.
 * Likewise, the IFTYPE_xx defines are in parser_defs.h.  They
 * should be elsewhere, and used to tag each IDB.  An interface
 * type was added to the idb after we took our snapshot of the
 * sources and maybe it can be used instead.
 */

/*
 *  IFTYPE_VTEMPLATE and IFTYPE_VACCESS must be before any interface
 *   which is capable of doing multilink ppp, ( currently serial, async,
 *   BRI, Dialer) because configs for vtemplate and vaccess must be
 *   completed before processing other interfaces during a reload.
 */

#define BEFORE_HW_INTS  0
#define HW_INT          1
#define AFTER_HW_INTS   2
#define NEVER_DISPLAY   3

static iftype iftypes[] = {
    {IFTYPE_LOOPBACK, IFNAME_LOOPBACK, "Loopback interface",
         0, 0, 0, TRUE, FALSE, BEFORE_HW_INTS },
    {IFTYPE_TUNNEL, IFNAME_TUNNEL, "Tunnel interface",
         0, 0, 0, TRUE, FALSE, BEFORE_HW_INTS},
    {IFTYPE_ETHER, IFNAME_ETHER, "IEEE 802.3",
         0, 0, 0, FALSE, TRUE, HW_INT},
    {IFTYPE_FDDI, IFNAME_FDDI, "ANSI X3T9.5",
         0, 0, 0, FALSE, TRUE, HW_INT},
    {IFTYPE_VTEMPLATE, 	IFNAME_VTEMPLATE, "Virtual Template interface",
         0, 0, 0, TRUE, TRUE, HW_INT},    
    {IFTYPE_VACCESS, 	IFNAME_VACCESS, "Virtual Access interface",
         0, 0, 0, TRUE, TRUE, HW_INT},
    {IFTYPE_HSSI, IFNAME_HSSI, "High Speed Serial Interface",
         0, 0, 0, FALSE, TRUE, HW_INT},
    {IFTYPE_NULL, IFNAME_NULL, "Null interface",
         0, 0, 0, TRUE, FALSE, BEFORE_HW_INTS},
    {IFTYPE_SERIAL, IFNAME_SERIAL, "Serial",
         0, 0, 0, FALSE, TRUE, HW_INT},
    {IFTYPE_TOKENRING, IFNAME_TOKENRING, "IEEE 802.5",
         0, 0, 0, FALSE, TRUE, HW_INT},
    {IFTYPE_VIRTUAL, IFNAME_VIRTUAL,
         "Virtual interface", 0, 0, 0, TRUE, TRUE, AFTER_HW_INTS},
    {IFTYPE_BRI, IFNAME_BRI, "ISDN Basic Rate Interface",
         0, 0, 0, FALSE, TRUE, HW_INT},
    {IFTYPE_ASYNC, IFNAME_ASYNC, "Async interface",
         0, 0, 0, TRUE, TRUE, AFTER_HW_INTS},
    {IFTYPE_VTY_ASYNC, IFNAME_VTY_ASYNC, "Virtual Async interface",
         0, 0, 0, TRUE, TRUE, AFTER_HW_INTS},
    {IFTYPE_ASYNC_GROUP, IFNAME_ASYNC_GROUP, "Async Group interface",
	 0, 0, 0, TRUE, TRUE, AFTER_HW_INTS},
    {IFTYPE_DIALER, IFNAME_DIALER, "Dialer interface",
         0, 0, 0, TRUE, TRUE, AFTER_HW_INTS},
    {IFTYPE_ATM, IFNAME_ATM, "ATM interface",
         0, 0, 0, FALSE, TRUE, HW_INT },
    {IFTYPE_CHANNEL, IFNAME_CHANNEL, "IBM Channel interface",
         0, 0, 0, FALSE, TRUE, HW_INT},
    {IFTYPE_PCBUS, IFNAME_PCBUS, "PCbus interface",
         0, 0, 0, FALSE, TRUE, HW_INT},
    {IFTYPE_LEX, IFNAME_LEX, "Lex interface",
         0, 0, 0, TRUE, FALSE, AFTER_HW_INTS},
    {IFTYPE_FASTETHER, IFNAME_FASTETHER, "FastEthernet IEEE 802.3",
         0, 0, 0, FALSE, TRUE, HW_INT},
    {IFTYPE_BVI, IFNAME_BVI, "Bridge-Group Virtual Interface",
 	 0, 0, 0, TRUE, FALSE, AFTER_HW_INTS},
    {IFTYPE_POSIP, IFNAME_POSIP, "Packet over Sonet",
         0, 0, 0, FALSE, TRUE, HW_INT},
    {IFTYPE_NONE, NULL, NULL, 0, 0, 0, FALSE, FALSE, AFTER_HW_INTS},
};

/*
 * find_hwidb_by_name :
 * Return a hwidb pointer for an interface that matches the name
 */
static hwidbtype *find_hwidb_by_name (iftype *ift, int unit, uint slot,
				      int vc)
{
    hwidbtype *ifp;

    switch (idb_name_format()) {

    case SLOTTED_IDB_NAMES:
    case EXTENDED_SLOTTED_IDB_NAMES:

	FOR_ALL_HWIDBS(ifp) {
	    if ((strcmp(ifp->name, ift->name) == 0) &&
		((ift->dynamic_interface && ifp->unit == unit) ||
		 ((ift->dynamic_interface == FALSE) && 
		  (((int)ifp->slotunit) == unit) && (ifp->slot == slot) &&
		  (ifp->vc == vc)))) {
		/* Valid interface */
		break;
	    }
	}
	return(ifp);

    default:

	FOR_ALL_HWIDBS(ifp) {
	    if ((strcmp(ifp->name, ift->name) == 0) &&
		(ifp->unit == unit)) {
		/*
		 * Wait a minute. If we have configured VCs, check for
		 * valid VCs.
		 */
		if (vc != NO_VC) {
		    if (ifp->vc == vc)
			/* Valid interface */
			break;
		} else {
		    /* Valid interface */
		    break;
		}
	    }
	}
	return(ifp);
    }
    return(NULL);
}

/*
 * Find an IDB that matches the supplied name, unit, slot, vc and subinterface.
 * Returns NULL if the IDB isn't found.
 *
 * If the subinterface number is nonzero, create_idb is TRUE, and the IDB
 * does not exist, it will create one.
 *
 * Returns -1 if the operation cannot be completed.
 */
static idbtype *find_interface (parseinfo *csb, iftype *ift, uint unit,
				uint slot, uint vc, uint subif, int create_idb)
{			    
    hwidbtype *ifp;
    idbtype *swifp;

    ifp = find_hwidb_by_name(ift, unit, slot, vc);
    swifp = firstsw_or_null(ifp);

    if (swifp && swifp->hwptr &&
	(swifp->hwptr->status & IDB_DELETED) &&
	!create_idb) {
	/*
	 * Found a match, but it's deleted, so put everything
	 * back where we found it and pretend it didn't happen.
	 */
	swifp = NULL;
	ifp = NULL;
    }

    if (ifp == NULL) {
	if ((unit == 0) && (strcmp(ift->name, IFNAME_NULL) == 0)) {
	    /*
	     * Null idb; Don't add it to idbQ (kills router
	     * in compute_loads())
	     */
	    return(nullidb);
	} else {
	    if (create_idb) {
		if (reg_invoke_create_idb(&ift->type, csb)) {
		    csb->createidb_unit = unit;
		} else {
		    swifp = (idbtype *) -1;
		}
	    } else {
		swifp = (idbtype *) -1;
	    }
	}
    } else {
	/*
	 * Found the matching hardware interface.  Now look for the
	 * appropriate subinterface, if any.  If it isn't there, and
	 * create_idb is true, and we're allowed to, create one.
	 */
	if (subif) {
	    if (!idb_subif_allowed_here(ifp)) {
		swifp = (idbtype *) -1;
	    } else {
		FOR_ALL_SWIDBS_ON_HW(ifp, swifp) {
		    if (swifp->sub_number == subif) {
			return(swifp);
		    }
		}
	    }
	    if (create_idb) {
		if (!idb_subif_allowed_here(ifp)) {
		    swifp = (idbtype *) -1;
		} else {
		    if (!idb_can_create_more()) {
			errmsg(&msgsym(CREATEINT, PARSER));
			swifp = (idbtype *) -1;
		    } else {
			swifp = idb_create_subif(ifp->firstsw, subif);
		    }
		}
	    }
	}
    }
    return(swifp);
}

/*
 * parse_interface
 * Given a interface ifp, iftype for that interface, some other
 * variables, this routine will nvgen for that interface and its
 * subinterfaces
 */
static void do_interface_parse (hwidbtype *ifp,
				iftype *ift,
				parseinfo *csb,
				transition *mine,
				interface_struct * const arg,
				char *cp)
{
    idbtype *swifp;
    char buffer[32];

    if (arg->flag & PARSE_UNIT_ONLY) {
	sprintf(buffer, "%d", ifp->unit);
	strcpy(cp, buffer);
    } else {
	strcpy(cp, ifp->hw_namestring);
    }
    *CSBVAR(csb,arg->offset,idbtype *) = ifp->firstsw;
#ifdef  DEBUG_NVGEN
    printf("Interface NVGEN: %s\n", csb->nv_command);
#endif  DEBUG_NVGEN
    push_node(csb, mine->accept);

    /* Generate entries for all subinterfaces. */

    FOR_ALL_SWIDBS_ON_HW(ifp, swifp) {
	if (is_first_swidb(swifp)) {
	    continue;
	}
	strcpy(cp, swifp->namestring);
	*CSBVAR(csb,arg->offset,idbtype *) = swifp;
	push_node(csb, mine->accept);
    }
}
/*
 * interface_parse
 * This routine will parse all interfaces that have a match_listplace
 * value in the listplace field in the iftypes list that is defined above.
 * It will display all of each type in hwidb list.
 */
static void interface_parse (parseinfo *csb,
			     transition *mine,
			     interface_struct * const arg,
			     char *cp,
			     uint match_listplace)
{
    hwidbtype *ifp;
    iftype *ift;

    for (ift = iftypes; ift->type; ift++) {
	if ((ift->type & arg->valid_ifs) == 0) {
	    continue;
	}
	if (ift->listplace != match_listplace)
	    continue;
	FOR_ALL_HWIDBS(ifp) {
	    if (strcmp(ift->name, ifp->name) != 0)
		continue;
	    if (ifp->status & IDB_DELETED)
		continue;
	    do_interface_parse(ifp, ift, csb, mine, arg, cp);
	}
	/* The null interface is not on the idbQ and is only
	 * output if 'no ip unreachable' must be generated.
	 */
	if (nullidb &&
	    nullidb->hwptr &&
	    (strcmp(ift->name, nullidb->hwptr->name) == 0)) {
	    strcpy(cp, nullidb->namestring);
	    *CSBVAR(csb,arg->offset,idbtype *) = nullidb;
	    push_node(csb, mine->accept);
	}
    }
}
/*
 * hw_interface_parse
 * This will basically do the same thing as interface_parse, except
 * that it will loop through the HWIDBs, then search the iftype array
 * to see if the interface type for that hwidb is a match_listplace
 * Since the hwidb's are now in slot order, this has the results of
 * displaying everything in slot order.
 *
 * Note that for the low end, hw_interface_parse is defined to be
 * interface_parse, above
 */
static void hw_interface_parse (parseinfo *csb,
				transition *mine,
				interface_struct * const arg,
				char *cp,
				uint match_listplace)
{
#ifndef MAX_IF_SLOT
    interface_parse(csb, mine, arg, cp, match_listplace);
#else
    hwidbtype *ifp;
    iftype *ift;

    FOR_ALL_HWIDBS(ifp) {
	if (ifp->status & IDB_DELETED)
	    continue;
	for (ift = iftypes; ift->type; ift++) {
	    if ((ift->type & arg->valid_ifs) == 0) {
		continue;
	    }
	    if (ift->listplace != match_listplace)
		continue;
	    if (strcmp(ift->name, ifp->name) != 0)
		continue;
	    do_interface_parse(ifp, ift, csb, mine, arg, cp);
	}
    }
#endif
}

void interface_action (parseinfo *csb, transition *mine,
		       interface_struct * const arg)
{
    uint64 types_present = IFTYPE_NONE;
    char *cp;
    hwidbtype *ifp;
    idbtype *swifp;
    iftype *ift;
    int i, oldi, j;
    parsenode *pp;
    queuetype *t1;	/* Save the csb->tokenQ here during NV generation */
    queuetype t2;
    char buffer[64];
    uint unit;
    uint slot = 0;
    uint vc = NO_VC;
    uint subif = 0;

    /* Setup for the alternate transition */
    push_node(csb, mine->alternate);

    /*
     * If this is the first time into interface_action, build a list of
     * interface types present in the system.  We also keep track of the
     * maximum unit number for each interface, to provide help and loop with.
     * Note that our treatment of this leaves open the possibility of skipped
     * unit numbers, (i.e. Ethernet1 and Ethernet3 exist, but not Ethernet2).
     * This is deliberate.
     */
    if (types_present == IFTYPE_NONE) {
	for (ift = iftypes; ift->type; ift++) {
	    boolean in_idbQ = FALSE;

	    FOR_ALL_HWIDBS(ifp) {
		if (strcmp(ift->name, ifp->name) == 0) {
		    in_idbQ = TRUE;
		    types_present |= ift->type;
		    if (ifp->unit > ift->max_unit) {
			ift->max_unit = ifp->unit;
			if ((ift->type == IFTYPE_CHANNEL) &&
			    (ift->max_unit < 2)) {
			    ift->max_unit = 2;
			}
		    }
		    if (ifp->vc > ift->max_vc) {
			ift->max_vc = ifp->vc;
		    }
		}
	    }


	    ift->base_unit = 0;		/* Base unit is 0 for most types. */

           if ((ift->type == IFTYPE_VTEMPLATE) &&
                ((in_idbQ == TRUE) || (arg->flag & PARSE_CREATE_IDB))) {
                types_present |= IFTYPE_VTEMPLATE;
                ift->base_unit = 1;
                ift->max_unit = VTEMPLATE_MAXVTEMPLATE_UNIT;
	    }
	    if ((ift->type == IFTYPE_VACCESS) &&
		(types_present & IFTYPE_VACCESS)) {
                ift->base_unit = 1;
	    }    
            if ((ift->type == IFTYPE_ASYNC_GROUP) &&
                ((in_idbQ == TRUE) || (arg->flag & PARSE_CREATE_IDB))) {
                types_present |= IFTYPE_ASYNC_GROUP;
                ift->base_unit = 1;
                ift->max_unit = nttylines + nauxlines;
	    }
	    if ((ift->type == IFTYPE_VTY_ASYNC) &&
		(in_idbQ == TRUE)) {
		ift->base_unit = VTYBase;
		/* 
		 * VTYBase is where vty lines starts with respect to 
		 * other lines on the router (nttylines + aux + con)
		 * nvtylines = Total configured vty lines on the router
		 * The -1 is needed because the vtylines starts at 0
		 */
		ift->max_unit = nvtylines + VTYBase - 1;
	    }
	    if (ift->type == IFTYPE_NULL) {
		types_present |= IFTYPE_NULL;
		ift->max_unit = 0;
	    }
	    if ((ift->type == IFTYPE_ASYNC) &&
		((in_idbQ == TRUE) || (arg->flag & PARSE_CREATE_IDB))) {
		types_present |= IFTYPE_ASYNC;
		ift->base_unit = reg_invoke_async_int_base_unit();
                if (ift->base_unit == 0)
                    ift->base_unit = 1;
		ift->max_unit = nttylines + nauxlines;
	    }
	    if ((ift->type == IFTYPE_DIALER) &&
		((in_idbQ == TRUE) || (arg->flag & PARSE_CREATE_IDB))) {
		types_present |= IFTYPE_DIALER;
		ift->max_unit = 255;
	    }
	    if ((ift->type == IFTYPE_TUNNEL) &&
		((in_idbQ == TRUE) || (arg->flag & PARSE_CREATE_IDB))) {
		types_present |= IFTYPE_TUNNEL;
		ift->max_unit = MAXINT;
	    }
	    if ((ift->type == IFTYPE_LOOPBACK) &&
		((in_idbQ == TRUE) || (arg->flag & PARSE_CREATE_IDB))) {
		types_present |= IFTYPE_LOOPBACK;
		ift->max_unit = MAXINT;
	    }
	    if ((ift->type == IFTYPE_LEX) &&
		((in_idbQ == TRUE) || (arg->flag & PARSE_CREATE_IDB))) {
		types_present |= IFTYPE_LEX;
		ift->max_unit = MAXINT;
	    }
 	    if ((ift->type == IFTYPE_BVI) &&
 		((in_idbQ == TRUE) || (arg->flag & PARSE_CREATE_IDB))) {
 		types_present |= IFTYPE_BVI;
 		ift->max_unit = MAXINT;
 	    }
	}
    }

    /*
     * Be paranoid if the unit_only argument is declared, and check to be
     * sure that only one interface type was specified.
     */
    if (arg->flag & PARSE_UNIT_ONLY) {
	for (i = 0; i < 32; i++) {
	    if ((arg->valid_ifs & ~IFTYPE_SUBIF) == (1ULL << i)) {
		break;
	    }
	}
	if (i == 32) {
	    errmsg(&msgsym(MULTIPLEIFS, PARSER));
	    return;   
	}
    }

    if (csb->nvgen) {
	/*
	 * Find the end of the existing NV generated command, append a space,
	 * and the full interface name.  Set csb->interface to be the current
	 * interface then save the accepting state.
	 * 
	 * Because the stack is push-down, the interfaces will be stored in
	 * reverse order. Use a temporary stack to store the interface nodes,
	 * then copy to the real stack.
	 * 
	 * PLAN: 
	 * 1. Save the existing csb->tokenQ. 
	 * 2. Create a temporary stack. 
	 * 3. Save the interface nodes onto this stack (push_node() calls). 
	 * 4. Copy the nodes to the saved tokenQ by popping from the temporary
	 *    stack and pushing onto the real stack (thus reversing the order 
	 *    of the nodes.) 
	 * 5. Free the temporary queue header. 
	 * 6. Restore csb->tokenQ.
	 */

	for (cp = csb->nv_command; *cp; cp++) {
	    ;	/* Null body */
	}
	*cp++ = ' ';
	*cp = '\0';
	t1 = csb->tokenQ;	/* Save the existing stack */
	queue_init(&t2, 0);
	csb->tokenQ = &t2;

	/*
	 * Display all the interfaces that come before the HW interfaces,
	 * then display the hardware interfaces, then follow with the
	 * ones defined after
	 */
	interface_parse(csb, mine, arg, cp, BEFORE_HW_INTS);
	hw_interface_parse(csb, mine, arg, cp, HW_INT);
	interface_parse(csb, mine, arg, cp, AFTER_HW_INTS);


	/* Copy the nodes onto the real stack */
	while (! QUEUEEMPTY(csb->tokenQ)) {
	    pp = dequeue(csb->tokenQ);
	    requeue(t1, pp);
	}
	csb->tokenQ = t1;
	*CSBVAR(csb,arg->offset,idbtype *) = NULL;
	*--cp = '\0';		/* Stomp on added space */
	return;
    }

    /* 
     * We're parsing an interface, so loop through all the types and
     * try to match: keyword(type-name) optional-whitespace
     * decimal(unit-number) After a sucessful parse, we still have to
     * run through the IDB list to actually find the interface in
     * question.  Right now, this should always succeed, since
     * interfaces are numbered sequentially.  After finding the IDB,
     * we store it in csb->idb1 (note: this is hardwired into the
     * code) and accept.  BTW, the only reason we get away with just
     * pushing these things onto the stack without returning between
     * each one is push_node's saving of idb1 in the parsenode
     * structure.  That behavior is required for NVgeneration, and is
     * not typical of parse variables.
     */

    oldi = 0;
    if (arg->flag & PARSE_INTERFACE_KEYWORD) {
	boolean found_interface = FALSE;
	/*
	 * Make sure there are some valid interfaces 
	 * before parsing the keyword
	 */
	for (ift = iftypes; ift->type != 0; ift++) {
	    if (ift->type & arg->valid_ifs & types_present) {
		found_interface = TRUE;
		break;
	    }
	}
	if (!found_interface ||
	    !match_partial_keyword(csb, arg->keyword, &oldi, arg->help, 0,
				   (KEYWORD_WS_OK|KEYWORD_HELP_CHECK))) {
	    return;
	}
    }

    for (ift = iftypes; ift->type != IFTYPE_NONE; ift++) {
	if ((ift->type & arg->valid_ifs & types_present) == 0) {
	    /*
	     * Do not skip the interface if it is a virtual access
	     * interface with the VTEMPLATE_STATUS_CONFIG set
	     */
	    if((csb->interface) &&
	       (csb->interface->hwptr->type == IDBTYPE_VACCESS)) {
		if (!(csb->interface->vaccess_status &
		      VTEMPLATE_STATUS_CONFIG))
		    continue;
	    }
	    else
		continue;
	}

	i = oldi;
	if ((arg->flag & PARSE_UNIT_ONLY) ||
	    match_partial_keyword(csb, ift->name, &i, ift->descr, 0,
				  (KEYWORD_WS_OK|KEYWORD_NO_WS_OK|
				   KEYWORD_HELP_CHECK))) {

	    sprintf(buffer, "%s interface number", ift->name);
	    csb->line_index += i;
	    j = 0;
	    if (match_interface_unit(csb, ift, &j, &unit,
				     &slot, &vc, &subif, arg, buffer)) {

		swifp = find_interface(csb, ift, unit, slot, vc, subif,
				       arg->flag & PARSE_CREATE_IDB);
		if ((swifp != (idbtype *) -1) && ((swifp != NULL) ||
						  (arg->flag &
						   PARSE_CREATE_IDB))) {
		    /*
		     * Success ! Got a number AND it could be an
		     * interface AND it does actually match one !
		     */
		    csb->line_index += j;
		    *CSBVAR(csb,arg->offset,idbtype *) = swifp;
		    if (swifp == NULL) {
			csb->createidb_var = arg->offset;
		    }
		    push_node(csb, mine->accept);
		    csb->line_index -= j;
		} else {
		    if (!(arg->flag & PARSE_CREATE_IDB) &&
			(ift->type & (IFTYPE_ASYNC | IFTYPE_DIALER | IFTYPE_LEX
				      | IFTYPE_LOOPBACK | IFTYPE_TUNNEL |
				      IFTYPE_BVI))) {
			set_error_index(csb);
		    }
		}
	    }
	    csb->line_index -= i;
	}			/*** if (match_partial_keyword *** */
    }				/*** for (ift = iftypes; ift->type != 0	***/
}

/* WHITESPACE matches at least one whitespace char, maybe more
 * EOL is acceptable as whitespace.
 */

void whitespace_action (parseinfo *csb, transition *mine)
{
    int i;

    push_node(csb, mine->alternate);
    if (csb->nvgen) {
	/*
	 * Don't add whitespace here.  This function is often called between
	 * tokens which don't generate any output.
	 */
	push_node(csb, mine->accept);

	return;
    }
    i = 0;

    if (match_whitespace2(&csb->line[csb->line_index], &i, csb->in_help)) {
	csb->line_index += i;
	push_node(csb, mine->accept);
	csb->line_index -= i;
    }
    /* No point to setting error_index: if the above fails we haven't
    scanned any whitespace successfully. */
}

/* PRINT always matches and prints its string, followed by a newline */

void print_action (parseinfo *csb, transition *mine,
		   print_struct * const arg)
{
    push_node(csb, mine->accept);
    if (csb->nvgen) {
	return;
    }

    if (arg->str) {
	printf(arg->str);
    }
}

/* EOL prints help and does func call at EOL
 *
 * A push-down stack of parseinfo structures is used to save the state of
 * the current parse.  A new parseinfo struct is used for each possible
 * command so that successive partial matching commands do not overwrite
 * data stored in the parseinfo struct of a fully matching command.
 */

void eol_action (parseinfo *csb, transition *mine, void (*func) (parseinfo *))
{
    parseinfo *savepd;
    char *short_help = parser_eol_string;

    if (parser_ambig_debug && !csb->nvgen) {
	buginf("\nEol Action");
    }
    /*
     * If setting a command's privileges, indicate that the command was
     * successful and return.
     */
    if (csb->priv_set) {
	increment_multiple_funcs(csb, "eol_action 1");
	return;
    }
    if (csb->nvgen) {
	if (func) {
	    (void) func(csb);
	}
    } else {
	if (csb->in_help) {
	    if (help_check(csb)) {
		if (csb->flags & CONFIG_HTTP) {
		    save_help_long(csb, "CR", NULL);
		} else {
		    save_help_long(csb, short_help, NULL);
		}
		save_ambig_string_flag(csb, short_help, short_help, FALSE);
		no_alt_action(csb);
	    }
	    return;
	}
	if (eol_check(csb)) {

	    increment_multiple_funcs(csb, "eol_action 2");

	    savepd = chunk_malloc(parseinfo_chunks);
	    if (savepd == NULL) {
		parser_bail_out(csb, func, "eol_action");
		return;
	    }

	    /*
	     * Make a copy of the current parse data (savepd) before
	     * modifying current info. This implements a push-down
	     * stack where the top of the stack is the current
	     * (possibly invalid) parse state.  Any elements below the
	     * top are parse data from matching commands.  Only one
	     * element should be on the stack if a command matches the
	     * input.
	     */

	    save_ambig_string_flag(csb, short_help, short_help, FALSE);

	    *savepd = *csb;		/* Struct copy */

	    csb->next = savepd;
	    savepd->action_func = func;
	} else {

	    /*
	     * This command did not accept.  Remove it from the linked list of
	     * accepting command states.  Don't discard the current error_index
	     * since it indicates that we came this far in the input before
	     * failing.
	     */
	    set_error_index(csb);
	}
	no_alt_action(csb);
    }				/* if (csb->nvgen) */
}

/* EOLS - prints help and does func call at EOL.  Sets csb->which with
 * the EVAL macro so that 'val' is not constrained to be a constant.
 */

void eols_action (parseinfo *csb, transition *mine,
		  eols_struct * const arg)
{
    csb->which = arg->subfunc;
    eol_action(csb, mine, arg->func);
}

void eolns_action (parseinfo *csb, transition *mine, void (*func) (parseinfo *))
{
    csb->which = 0;
    eol_action(csb, mine, func);
}
  
/* ALT_HELP does func call at EOL which
 * prints platform specific help.
 */

void alt_help_action (parseinfo *csb, transition *mine, void (*func) (parseinfo *))
{
  if (csb->in_help) {
    if (help_check(csb)) {
      if (func) {
        (void) func(csb);
        return;
      }
    }
  }

  no_alt_action(csb);
}

/* ALT_HELPS - does platform specific func call at EOL.  Sets csb->which with
 * the EVAL macro so that 'val' is not constrained to be a constant.
 */
void alt_helps_action (parseinfo *csb, transition *mine,
                eols_struct * const arg)
{
    csb->which = arg->subfunc;
    alt_help_action(csb, mine, arg->func);
}

void general_number_short_help (uint *flags, uint lower,
				uint upper, char *help)
{
    general_number_short_help_http(flags, lower, upper, help, FALSE);
}

void general_number_short_help_http (uint *flags, uint lower, uint upper,
				     char *help, boolean doing_http)
{
    char *open_bracket = "<";
    char *close_bracket = ">";

    if (doing_http) {
	open_bracket = close_bracket = "";
    }

    switch (*flags & (NUMBER_HEX|NUMBER_OCT|NUMBER_DEC)) {
      default:
      case 0:
	errmsg(&msgsym(NUMHELP, PARSER), "<none>");
	*flags |= (NUMBER_HEX|NUMBER_OCT|NUMBER_DEC);
      case NUMBER_HEX | NUMBER_OCT | NUMBER_DEC:
      case NUMBER_HEX | NUMBER_OCT:
      case NUMBER_HEX | NUMBER_DEC:
      case NUMBER_OCT | NUMBER_DEC:
      case NUMBER_DEC:
	sprintf(help, "%s%u-%u%s", open_bracket, lower, upper, close_bracket);
	break;

      case NUMBER_HEX:
	switch (*flags & (HEX_ZEROX_OK|HEX_NO_ZEROX_OK)) {
	  default:
	  case 0:
	    errmsg(&msgsym(NUMHELP, PARSER), "hex");
	    *flags |= HEX_ZEROX_OK;
	  case HEX_ZEROX_OK:
	  case HEX_ZEROX_OK | HEX_NO_ZEROX_OK:	/* why not just put this case here */
	    sprintf(help, "%s0x%x-0x%x%s",
		    open_bracket, lower, upper, close_bracket);
	    break;
	  case HEX_NO_ZEROX_OK:
	    sprintf(help, "%s%x-%x%s", open_bracket,
		    lower, upper, close_bracket);
	    break;
	}
	break;

      case NUMBER_OCT:
	switch (*flags & (OCT_ZERO_OK|OCT_NO_ZERO_OK)) {
	  default:
	  case 0:
	    errmsg(&msgsym(NUMHELP, PARSER), "octal");
	    *flags |= OCT_ZERO_OK;
	  case OCT_ZERO_OK:
	  case OCT_ZERO_OK | OCT_NO_ZERO_OK:	/* why not just put this case here */
	    sprintf(help, "%s0%o-0%o%s", open_bracket,
		    lower, upper, close_bracket);
	    break;
	  case OCT_NO_ZERO_OK:
	    sprintf(help, "%s%o-%o%s", open_bracket, lower, upper,
		    close_bracket);
	    break;
	}
	break;
    }
}

/*
 * GENERAL_NUMBER matches a ranged, unsigned number, minimum one digit, and
 * assigns it to a int.  It also handles help and whitespace.  If the
 * input begins with '0x' and contains at least one hex digit, the
 * conversion is from Hex.  If the leading digit is '0', and the
 * number is considered octal. The <types> field of the number_struct
 * is a flag variable
 * indicating what types of input (hex, octal, decimal) are allowed.
 *
 * The following function takes additional structure arguments that
 * allow whitespace and help checking to be turned off.
 */

/* Scan a number and accept. Types of numbers (hex, decimal, octal) and
 * allowable range are specified via <*arg>. <*arg> also specifies via
 * another field whether whitespace after is to be tested for (required)
 * or not. Yet another field specifies whether help checks are to be
 * done.
 */

void general_number_action (parseinfo *csb, transition *mine,
			    number_struct * const arg)
{
    uint i = 0;
    uint num = 0;
    char short_help[11];
    uint lower;
    uint upper;

    push_node(csb, mine->alternate);

    CHECK_RANGE_BOUNDS();

    if ((arg->flags & NUMBER_NV_RANGE) && csb->nvgen) {
	queuetype *t1 = csb->tokenQ;
	queuetype t2;
	uint l = strlen(csb->nv_command);

	for (i=lower; i<= upper; i++) {
	    *CSBVAR(csb, arg->offset, uint) = i;
	    sprintf(short_help, "%d", i);

	    csb->nv_command[l] = '\0';
	    csb->tokenQ = &t2;
	    queue_init(csb->tokenQ, 0);
	    nvgen_token(csb, mine, short_help);
	    parse_token(csb);
	    csb->tokenQ = t1;
	}
	return;
    }

    /*
     * We should never be called for NV generation
     */
    INVALID_NVGEN_CHECK;

    /* match the number */
    if (match_number(csb, lower, upper, &i, &num, arg->flags, arg->help)) {
#ifdef	DEBUG_NUMBER
	printf("\ngeneral_number_action: matched number %d", num);
#endif	DEBUG_NUMBER

	*CSBVAR(csb, arg->offset, uint) = num;

	csb->line_index += i;
	push_node(csb, mine->accept);
	csb->line_index -= i;

    }
}

void signed_number_action (parseinfo *csb, transition *mine,
			   signed_number_struct * const arg)
{
    int i = 0;
    int num = 0;
    char short_help[32];
    int lower;
    int upper;

    push_node(csb, mine->alternate);

    /*
     * We should never be called for NV generation
     */
    INVALID_NVGEN_CHECK;

    CHECK_RANGE_BOUNDS();

    sprintf(short_help,"<%d - %d>", lower, upper);

    if (help_check(csb)) {
	save_help_long(csb, short_help, arg->help);
	return;
    }

    /* match the number */
    i = 0;
    if (match_signed_dec(&csb->line[csb->line_index],
			 lower, upper, &i, &num) &&
	match_whitespace2(&csb->line[csb->line_index], &i, csb->in_help)) {

	*CSBVAR(csb, arg->offset, int) = num;

	save_ambig_int(csb, short_help, num);

	csb->line_index += i;
	push_node(csb, mine->accept);
	csb->line_index -= i;
    } else {
	help_or_error(csb, i, short_help, NO_COMMAND_COMPLETION);
    }
}

void number_func_action (parseinfo *csb, transition *mine,
			 number_func_struct * const arg)
{
    dynamic_number_struct new_arg =
	{
	    arg->offset, 0, MAXUINT, arg->help,
	    (NUMBER_HEX | HEX_ZEROX_OK | NUMBER_OCT | OCT_ZERO_OK |
	     NUMBER_DEC | NUMBER_WS_OK | NUMBER_HELP_CHECK)
	};

    if (!csb->priv_set &&
	!(csb->flags & COMMAND_FUNCTION) &&
	arg->func) {
	arg->func(csb, &(new_arg.lower), &(new_arg.upper));
    }
    general_number_action(csb, mine, &new_arg);
}

void number_help_func_action (parseinfo *csb, transition *mine,
			      number_help_func_struct * const arg)
{
  dynamic_number_struct new_arg =
    {
      arg->offset, 0, MAXUINT, "",
      (NUMBER_HEX | HEX_ZEROX_OK | NUMBER_OCT | OCT_ZERO_OK |
       NUMBER_DEC | NUMBER_WS_OK | NUMBER_HELP_CHECK)
    };
  
  if (!csb->priv_set &&
      !(csb->flags & COMMAND_FUNCTION) &&
      arg->func) {
    arg->func(csb, &new_arg.lower, &new_arg.upper, &new_arg.help);
  }
  general_number_action(csb, mine, &new_arg);
}

/*
 * HELP
 */

void help_action (parseinfo *csb, transition *mine,
		  help_struct * const arg)
{
    /*
     * If doing NV generation, go to alternate (skip help output). If we're not
     * in help mode, then obviously skip to alternate. If not at the end of
     * line, we're not doing help, so go to alternate. Otherwise, this is a
     * valid help request and we add the current text to the help output
     * buffer.  But the text is only added if it exists. Transition to
     * alternate when the help string is saved to allow other tokens at this
     * level to generate help output.
     */

    if (help_check(csb)) {
	if (csb->flags & CONFIG_HTTP) {
	    save_line(csb, &csb->help_save, "<DT>%s", arg->str);
	} else {
	    save_help_msg(csb, arg->str);
	}
    }
    push_node(csb, mine->alternate);
}

/* Display places its message into the help buffer no matter what.
 * It's used mostly in cases where we've decided to discard the rest of
 * the input line and produce a generic help/error message.
 * THIS FUNCTION PRODUCES HELP WHEN in_help IS NOT SET.
 */

void display_action (parseinfo *csb, transition *mine,
		     help_struct * const arg)
{
    save_help_msg(csb, arg->str);
    push_node(csb, mine->alternate);
    return;
}

/* TTYLINE matches a tty line number, in either octal or decimal format
 * as determined by the global variable 'decimaltty_enable'.  It must
 * be followed by whitespace or EOL.
 */
typedef struct line_keyword_ {
    const char *word;
    const char *help;
    uint type;
    int *exists;
} line_keyword;

static const line_keyword line_keywords[] = {
    { "aux", "Auxiliary line", CON_AUX, &nauxlines },
    { "console", "Primary terminal line", CON_CONSOLE, NULL },
    { "tty", "Terminal controller", CON_TTY, &nttylines },
    { "vty", "Virtual terminal", CON_VTY, NULL },
    { NULL, NULL, 0, NULL }
};

/*
 * mark_range()
 *	Update NVGEN counter for given range of lines
 *
 * This code assumes that tty_cfg_rotor will never wrap.  I believe
 * this is a valid assumption, given a 32-bit range and the number
 * of configurations which can be generated per second.
 * Note last is actually 1 more than last, for compatibility with most
 * of the tty code's csb->line1, csb->line2.
 */
void
mark_range (int first, int last)
{
    int i;
    tt_soc *tt;
    ulong highest = 0L, ceiling = 0L;
    boolean found_one, have_ceiling = FALSE;

    /*
     * Assign it to the lines involved in the range
     */
    for (;;) {
	/*
	 * Find next highest tag
	 */
	found_one = FALSE;
	for (i = first; i < last; ++i) {
	    /*
	     * Look at next slot.  Skip empty slots.
	     */
	    tt = MODEMS[i];
	    if (tt == NULL) {
		continue;
	    }

	    /*
	     * Ignore entries which have already been processed;
	     * we work our way down from the highest numbered.
	     */
	    if (have_ceiling && (tt->cfg_counter >= ceiling)) {
		continue;
	    }

	    /*
	     * See if this is the highest seen yet of those values
	     * below the "ceiling".
	     */
	    if (!found_one || (tt->cfg_counter > highest)) {
		found_one = TRUE;
		highest = tt->cfg_counter;
	    }
	}

	/*
	 * If we've updated all distinct config versions, we're
	 * finished.
	 */
	if (!found_one) {
	    return;
	}

	/*
	 * Allocate a new unique version, and assign it out
	 */
	tty_cfg_rotor += 1L;
	for (i = first; i < last; ++i) {
	    tt = MODEMS[i];
	    if (tt && (tt->cfg_counter == highest)) {
		tt->cfg_counter = tty_cfg_rotor;
	    }
	}

	/*
	 * This value becomes the new ceiling
	 */
	ceiling = highest;
	have_ceiling = TRUE;
    }
}

void ttyline_action (parseinfo *csb, transition *mine,
		     ttyline_struct * const arg)
{
    int i;
    uint num1 = 0;
    uint num2 = 0;
    uint flags;
    uint max;
    int oldi;
    uint type;
    int j;
    char help[80];
    uint base = 0;
    uint tmp_base = 0;

    push_node(csb, mine->alternate);

    if (csb->nvgen) {

	/*
	 * NV generation of tty lines. We only need to note the line number to
	 * be used and let the command action routine perform the generation of
	 * the command string.
	 * 
	 * Plan: 1. Loop, setting up the line numbers and pushing the accepting
	 * node. 1. Return
	 */

#ifdef DEBUG_NVGEN
	printf("ALLlines = %d\n", ALLlines);
#endif
	for (i=0; i<ALLlines; i++) {
	    if (MODEMS[i]) {
		*CSBVAR(csb, arg->first, uint) = i;
		recurse_node(csb, mine->accept);
	    }
	}
	return;
    }

    flags = (decimaltty_enable ? NUMBER_DEC :
	     NUMBER_OCT|OCT_ZERO_OK|OCT_NO_ZERO_OK) |
	       NUMBER_WS_OK | NUMBER_HELP_CHECK;

    i = 0;
    oldi = i;
    type = 0;
    for (j=0; line_keywords[j].word != NULL; j++) {
	i = oldi;
	if ((!(line_keywords[j].exists) || *line_keywords[j].exists) &&
	    match_partial_keyword(csb, line_keywords[j].word, &i,
				  line_keywords[j].help, 0,
				  (KEYWORD_WS_OK|KEYWORD_HELP_CHECK))) {
	    type = line_keywords[j].type;
	    break;
	}
    }

    if (!type) {
	type = arg->flags & CON_ABSOLUTE;
    }
    
    if (type) {
	switch (type) {
          case CON_AUX:
	    max = nauxlines - 1;
	    base = AuxBase;
            break;
          case CON_CONSOLE:
	    max = CONSOLE_LINENUM;
	    base = CONSOLE_LINENUM;
            break;
          case CON_VTY:
	    /*
	     * when deleting lines, we can only delete from
	     * the existing ones.
	     */
	    if (csb->sense) {
		max = maxvtylines - 1;
	    } else {
		max = nvtylines - 1;
	    }
	    base = VTYBase;
            break;
          case CON_TTY:
	    /*
	     * Notice that TTYs are special, because "line tty 1" is the
	     * same as "line 1", and the minimum tty number is 0.
	     * This is to preserve one-based numbering of the external
	     * tty lines and such.  (effectively, the "tty" keyword is
	     * a numeric no-op, but it does restrict the range of possible
	     * arguments supplied.)
	     */
	    max = nttylines;
	    base = 0;
            break;
          case CON_ABSOLUTE:
          default:
            max = ALLlines - 1;
	    base = 0;
            break;
        }
    } else {
	max = ALLlines - 1;
    }

    sprintf(help, "%s%s", ((arg->flags & CON_RANGE) ? "First " : ""),
			   arg->help);
    if (type == CON_TTY) {
	/*
	 * See massive comment above on how TTYs are special!
	 */
	tmp_base =  (CONSOLE_LINENUM + nconlines);
    } else {
	tmp_base = 0;
    }

    if (match_number(csb, tmp_base, max, &i, &num1, flags, help)) {
	*CSBVAR(csb, arg->first, uint) = num1 + base;

	if ((arg->flags & CON_RANGE) &&
	    ((num1 + 1) <= max)) {
	    sprintf(help, "Last %s", arg->help);
	    if (match_number(csb, num1+1, max, &i, &num2, flags, help)) {
		*CSBVAR(csb, arg->last, uint) = num2 + base;
		csb->line_index += i;
		push_node(csb, mine->accept);
		csb->line_index -= i;
		return;
	    }
	}
	if (arg->flags & CON_RANGE) {
	    *CSBVAR(csb, arg->last, uint) = num1 + base;
	}

	csb->line_index += i;
	push_node(csb, mine->accept);
	csb->line_index -= i;
    }
}


/*
 * HUB matches an ethernet hub port number in decimal.  
 */
void hub_action (parseinfo *csb, transition *mine,
		     hub_struct * const arg)
{
    uint i = 0;
    uint num1 = 0;
    uint num2 = 0;
    uint flags;
    uint max = 0;
    char help[80];

    push_node(csb, mine->alternate);

    if (csb->val1 == HUB_ETHER) {
        max = netherhubports;
    } 

    flags = NUMBER_DEC | NUMBER_WS_OK | NUMBER_HELP_CHECK;

    sprintf(help, "%s%s", "First ", arg->help);
    if (match_number(csb, 1, max, &i, &num1, flags, help)) {
	*CSBVAR(csb, arg->first, uint) = num1;

	if ((num1 + 1) <= max) {
	    sprintf(help, "Last %s", arg->help);
	    if (match_number(csb, num1+1, max, &i, &num2, flags, help)) {
		*CSBVAR(csb, arg->last, uint) = num2;
		csb->line_index += i;
		push_node(csb, mine->accept);
		csb->line_index -= i;
		return;
	    }
	}
	*CSBVAR(csb, arg->last, uint) = num1;

	csb->line_index += i;
	push_node(csb, mine->accept);
	csb->line_index -= i;
    }
}

/* CHARACTER matches a single character */

void char_action (parseinfo *csb, transition *mine,
		  char_struct * const arg)
{
    char ch[2];

    push_node(csb, mine->alternate);

    /* Long help is the responsibility of the calling macro. */

    INVALID_NVGEN_CHECK;

    if (csb->line[csb->line_index] == arg->character) {

	ch[0] = arg->character;
	ch[1] = '\0';
	if (parser_ambig_debug) {
	    buginf("\nMatched character '%c'", arg->character);
	    buginf("\ncsb->command_visible = %d", csb->command_visible);
	}
	save_ambig_string(csb, ch, ch);

	csb->line_index++;
	push_node(csb, mine->accept);
	csb->line_index--;
    }

    /*
     * Note that short help check and error processing
     * are NOT appropriate here.
     */
}

/*
 * CHAR_NUMBER takes either a character or an integer value
 * representing a character
 */
void char_number_action (parseinfo *csb, transition *mine, 
			 number_struct * const arg)
{
    char short_help[32];
    int i = 0;
    uint num;
    uint lower;
    uint upper;

    push_node(csb, mine->alternate);
    INVALID_NVGEN_CHECK;

    CHECK_RANGE_BOUNDS();

    sprintf(short_help, "CHAR or <%d-%d>", lower, upper);

    if (help_check(csb)) {
	save_help_long(csb, short_help, arg->help);
	return;
    }

    /* Try to match the number first */
    num = 0;
    if (! match_number(csb, lower, upper, &i, &num,
		       (NUMBER_HEX | HEX_ZEROX_OK | NUMBER_OCT | OCT_ZERO_OK |
			NUMBER_DEC | NUMBER_NO_WS_OK | NUMBER_HELP_CHECK),
		       NULL)) {

	/* No number, try a character */
	if (csb->line[csb->line_index] &&
	    !isspace(csb->line[csb->line_index])) {

	    /* We have a Non-null character, try accepting it */
	    num = (int) csb->line[csb->line_index];
	    i = 1;
	}
    }

    if (i) {
	if (match_whitespace2((csb->line + csb->line_index),
			      &i, csb->in_help)) {

	    *CSBVAR(csb, arg->offset, uint) = num;

	    csb->line_index += i;
	    push_node(csb, mine->accept);
	    csb->line_index -= i;
	    
	    return;
	}
    }
    help_or_error(csb, i, short_help, NO_COMMAND_COMPLETION);
}

/* FUNC calls the named function if we're doing NV generation, or saves the
 * pointer to the function and the current arguments strucuture if we're
 * consuming input.
 *
 * If we're consuming, try the accepting chain, then check csb->multiple_funcs.
 * If it changed, then EOL was encountered and we leave the argument struct
 * on the parsedata list.  Otherwise, we didn't see an EOL, so remove the
 * structure from the list.  This ensures that the only structures in
 * the list are those corresponding to FUNCs along the chain leading to
 * an accepted EOL.
 *
 * If we're doing NV generation, call the function directly.  This allows the
 * function at the end of commands which are only keyword based to to
 * automatically perform NV generation without having to explicitly include an
 * NVGEN() transition in the command chain definition.
 */

void func_action (parseinfo *csb, transition *mine, void (*func) (parseinfo *))
{
    INVALID_NVGEN_CHECK;

    if (!csb->in_help && func) {
	(void) func(csb);
    }
    push_node(csb, mine->accept);
}

/* SET takes a pointer to a variable and a constant expression to set
 * that variable to.  It is considerably more efficient than EVAL.
 *
 * Likewise, TESTVAR tests a variable for its relation with a constant
 * expression.  It, in turn, is more efficient than ASSERT.
 */

void set_action (parseinfo *csb, transition *mine,
		 set_struct * const arg)
{
    *CSBVAR(csb, arg->offset, uint) = arg->val;
    push_node(csb, mine->accept);
}

/* Use a temporary variable in case the variable changes during the
 * parse (such as checking csb->multiple_funcs when searching for help
 * strings).
 */
void test_action (parseinfo *csb, transition *mine,
		  test_struct * const arg)
{
    uint tmp = *CSBVAR(csb, arg->offset, uint);

    push_node(csb, mine->alternate);
    if (tmp == arg->val)
	push_node(csb, arg->equal);
    if (tmp != arg->val)
	push_node(csb, arg->notequal);
    if (tmp < arg->val)
	push_node(csb, arg->lt);
    if (tmp > arg->val)
	push_node(csb, arg->gt);
}

void test_boolean_action (parseinfo *csb, transition *mine,
			  test_bool_struct * const arg)
{
    push_node(csb, mine->alternate);
    if (*(arg->var)) {
	push_node(csb, arg->nonzero);
    } else {
	push_node(csb, arg->zero);
    }
}

void test_int_action (parseinfo *csb, transition *mine, 
		      test_int_struct * const arg)
{
    push_node(csb, mine->alternate);
    if (*(arg->var)) {
	push_node(csb, arg->nonzero);
    } else {
	push_node(csb, arg->zero);
    }
}

void test_func_action (parseinfo *csb, transition *mine,
		       test_func_struct * const arg)
{
    push_node(csb, mine->alternate);
    if (arg->func()) {
	push_node(csb, arg->nonzero);
    } else {
	push_node(csb, arg->zero);
    }
}

void test_line_action (parseinfo *csb, transition *mine,
		       test_line_struct * const arg)
{
    int i;
    boolean success = TRUE;

    push_node(csb, mine->alternate);
    for (i=csb->line1; (i < csb->line2) && success; i++) {
	switch (MODEMS[i]->type) {
	  case CTY_LINE:
	    if (!(arg->type & CON_CONSOLE)) {
		success = FALSE;
	    }
	    break;
	  case VTY_LINE:
	    if (!(arg->type & CON_VTY)) {
		success = FALSE;
	    }
	    break;
	  case AUX_LINE:
	    if (!(arg->type & CON_AUX)) {
		success = FALSE;
	    }
	    break;
	  case TTY_LINE:
	  default:
	    if (!(arg->type & CON_TTY)) {
		success = FALSE;
	    }
	    break;
	}
    }
    if (success) {
	push_node(csb, arg->nonzero);
    } else {
	push_node(csb, arg->zero);
    }
}

void test_idbstatus_action (parseinfo *csb, transition *mine,
			    test_idbstatus_struct * const arg)
{
    push_node(csb, mine->alternate);
    if (csb->priv_set || (csb->flags & COMMAND_FUNCTION)) {
	push_node(csb, arg->false);
	push_node(csb, mine->accept);
    } else {
	if (csb->interface && csb->interface->hwptr) {
	    if (csb->interface->hwptr->status & arg->val) {
		push_node(csb, mine->accept);
	    } else {
		push_node(csb, arg->false);
	    }
	}
    }
}

void general_string_action (parseinfo *csb, transition *mine,
			    general_string_struct * const arg)
{
    int i;
    boolean ws;
    const char *short_help = (arg->flags & STRING_WS_OK) ?
	"LINE" :  "WORD";

    push_node(csb, mine->alternate);
    INVALID_NVGEN_CHECK;

    if (help_check(csb)) {
	if (arg->flags & STRING_HELP_CHECK) {
	    if (csb->flags & CONFIG_HTTP) {
		parser_http_form(csb, (arg->flags & STRING_WS_OK) ?
				 PARSER_HTTP_FORM_LINE :
				 PARSER_HTTP_FORM_WORD,
				 short_help, arg->help);
	    } else {
		save_help_long(csb, short_help, arg->help);
	    }
	}
	return;
    }

    i = copy_varstring(csb->line + csb->line_index,
		       CSBVAR(csb, arg->offset, char),
		       PARSEBUF, arg->flags);

    if (i) {
	ws = match_whitespace2(csb->line + csb->line_index, &i, csb->in_help);
	if (ws || (csb->flags & CONFIG_HTTP) || (arg->flags & STRING_WS_OK)) {
	    save_ambig_string(csb, short_help, CSBVAR(csb, arg->offset, char));
	    csb->line_index += i;
	    push_node(csb, mine->accept);
	    csb->line_index -= i;
	}

	if (!ws || (arg->flags & STRING_WS_OK)) {
	    if (arg->flags & STRING_HELP_CHECK) {
		help_or_error(csb, i, short_help, NO_COMMAND_COMPLETION);
	    } else {
		csb->line_index += i;
		set_error_index(csb);
		csb->line_index -= i;
	    }
	}
    }
}

/*
 * NVGEN - nvgen_action
 *
 * Call the NV generation function.
 */
void nvgen_action (parseinfo *csb, transition *mine, void (*func)(parseinfo *))
{
    if (csb->nvgen) {
	/*
	 * If csb->priv_set is set, we currently don't go farther than
	 * we can NV generate, so stop here!
	 */
	if (!csb->priv_set && func) {
	    (void) func(csb);
	}
    } else {
	push_node(csb, mine->accept);
    }
}

/*
 * NVGENS - nvgens_action
 *
 *
 */
void nvgens_action (parseinfo *csb, transition *mine,
		    nvgens_struct * const arg)
{
    csb->which = arg->subfunc;
    nvgen_action(csb, mine, arg->func);
}

void nvgenns_action (parseinfo *csb, transition *mine, void (*func)(parseinfo *))
{
    csb->which = 0;
    nvgen_action(csb, mine, func);
}

/*
 * nvgen_token
 *
 * Find the end of the existing command and a space if this isn't the
 * first token in the command.  Then
 * add the token to the command line.  Continue the generation by
 * transitioning to the accepting node.
 * When we return, reset the command line to its state before this token
 * was added and transition to the alternate node.
 */
void nvgen_token (parseinfo *csb, transition *mine, const char *token)
{
    char *cp;

    if (!csb->nvgen) {
	errmsg(&msgsym(NVGEN, PARSER));
	return;
    }

    /*
     * The alternate transition should have already been pushed onto the stack
     * before calling nvgen_token.
     */
    for (cp = csb->nv_command; *cp; cp++);	/* Null body */

    /* Add the current token and go to the accepting node */
    if (cp != csb->nv_command) {
	*cp = ' ';			/* Add a space unless at beginning */
	strcpy(cp+1, token);
    } else {
	strcpy(cp, token);
    }

#ifdef	DEBUG
    fprintf(stderr, "    NV_Command: '%s'\n", csb->nv_command);
#endif	DEBUG

    push_node(csb, mine->accept);
    *cp = '\0';				/* Restore prior state */
    return;
}

/* NOPREFIX
 * Go to 'accept' if there isn't a 'no' prefix.
 * Consume the remainder of the input and go to 'alternate' if there
 * was a 'no' prefix.
 */

void noprefix_action (parseinfo *csb, transition *mine)
{
    int i;

    if (csb->nvgen) {
	push_node(csb, mine->accept);
	return;
    }

    if ((csb->sense == TRUE) && (csb->set_to_default == FALSE)) {
	push_node(csb, mine->accept);
    } else {
	for (i = 0; csb->line[csb->line_index + i] != '\n' &&
	     csb->line[csb->line_index + i]; i++) {
	}
	csb->line_index += i;
	push_node(csb, mine->alternate);
	csb->line_index -= i;
    }
}

void params_action (parseinfo *csb, transition *mine,
		    params_struct * const arg)
{
    char *cp,
       *cp_save;
    int i = 0,
        k = 0;
    uint num;
    uint lower;
    uint upper;
    uint flags = KEYWORD_WS_OK | KEYWORD_HELP_CHECK;

    /* Make sure we process any alternates */
    push_node(csb, mine->alternate);

    /* Check for privilege to use this keyword */
    if (priv_check(csb, arg->priv) == FALSE) {
	return;
    }

    if (csb->nvgen) {
	if (csb->priv_set) {
	    if (nvgen_privilege(csb, arg->priv, arg->str)) {
		/*
		 * The next token is a number, but we are currently
		 * stopping at NVGEN points, which a number is, so stop!
		 */
	    }
	    return;
	}
	for (cp = csb->nv_command; *cp; cp++) {
	    ;	/* Null body */
	}
	cp_save = cp;
	    
	/* Add a space if needed */
	if (cp != csb->nv_command) {
	    *cp++ = ' ';
	    *cp = 0;
	}

	/*
	 * Add the current token and call the command action routine
	 */
	strcpy(cp, arg->str);

	csb->which = arg->subfunc;
	nvgen_action(csb, mine, arg->func);

	/*
	 * Restore the original command line before processing alternates
	 */
	*cp_save = 0;
	return;
    }

    /**********  KEYWORD  **********/
    /*
     * Check to see if the input matches this token. If there is no match,
     * we skip processing this token and try the alternates.
     */
    keyword_flag_modify(csb, arg->priv, &flags);
    if (match_partial_keyword(csb, arg->str, &i, arg->keyhelp, 0, flags)) {
	csb->line_index += i;

	if (csb->priv_set) {
	    /*
	     * Modify the current keyword privilege
	     */
	    priv_push(csb, arg->priv);
	}
	csb->last_priv = arg->priv->cur_priv;
	if (csb->last_priv > csb->highest_priv) {
	    csb->highest_priv = csb->last_priv;
	}
	/********  NVGENS   **********/
	csb->which = arg->subfunc;

	if ((csb->sense == FALSE) && (arg->flags & NO_NUMBER)) {
	    /********  NOPREFIX  *********/
	    /*
	     * We've parsed a 'no' prefix and must consume the remainder of
	     * the line, then look for EOL.
	     */
	    for (k = 0; csb->line[csb->line_index + k] != '\n' &&
		 csb->line[csb->line_index + k]; k++) {
		/* Null body */
	    }
	    /********  EOLS  ***********/
	    csb->line_index += k;
	    csb->which = arg->subfunc;
	    eol_action(csb, mine, arg->func);
	    csb->line_index -= k;
	    /******  End EOLS  *********/
	    /******** End NOPREFIX ********/
	} else {
	    /********  DECIMAL  **********/
	    k = 0;

	    CHECK_RANGE_BOUNDS();

	    if (match_number(csb, lower, upper, &k, &num,
			     arg->flags, arg->varhelp)) {

		*CSBVAR(csb, arg->offset, uint) = num;

		/********  EOLS  ***********/
		csb->line_index += k;
		csb->which = arg->subfunc;
		eol_action(csb, mine, arg->func);
		csb->line_index -= k;
		/******  End EOLS  *********/
	    } else {
		/********  NO_ALT  ***********/
		csb->line_index += k;
		no_alt_action(csb);
		csb->line_index -= k;
		/*********  End NO_ALT    **********/
	    }
	    /*********  End DECIMAL   **********/
	}

	csb->line_index -= i;
	/*********  End NVGENS    **********/
    }
}

void params_action_alist (parseinfo *csb, transition *mine,
			  params_struct_alist * const arg)
{
    char *cp,
       *cp_save;
    int i = 0,
        k = 0;
    uint num;
    uint lower, lower2;
    uint upper, upper2;
    uint flags = KEYWORD_WS_OK | KEYWORD_HELP_CHECK;

    /* Make sure we process any alternates */
    push_node(csb, mine->alternate);

    /* Check for privilege to use this keyword */
    if (priv_check(csb, arg->priv) == FALSE) {
	return;
    }

    if (csb->nvgen) {
	if (csb->priv_set) {
	    if (nvgen_privilege(csb, arg->priv, arg->str)) {
		/*
		 * The next token is a number, but we are currently
		 * stopping at NVGEN points, which a number is, so stop!
		 */
	    }
	    return;
	}
	for (cp = csb->nv_command; *cp; cp++) {
	    ;	/* Null body */
	}
	cp_save = cp;
	    
	/* Add a space if needed */
	if (cp != csb->nv_command) {
	    *cp++ = ' ';
	    *cp = 0;
	}

	/*
	 * Add the current token and call the command action routine
	 */
	strcpy(cp, arg->str);

	csb->which = arg->subfunc;
	nvgen_action(csb, mine, arg->func);

	/*
	 * Restore the original command line before processing alternates
	 */
	*cp_save = 0;
	return;
    }

    /**********  KEYWORD  **********/
    /*
     * Check to see if the input matches this token. If there is no match,
     * we skip processing this token and try the alternates.
     */
    keyword_flag_modify(csb, arg->priv, &flags);
    if (match_partial_keyword(csb, arg->str, &i, arg->keyhelp, 0, flags)) {
	csb->line_index += i;

	if (csb->priv_set) {
	    /*
	     * Modify the current keyword privilege
	     */
	    priv_push(csb, arg->priv);
	}
	csb->last_priv = arg->priv->cur_priv;
	if (csb->last_priv > csb->highest_priv) {
	    csb->highest_priv = csb->last_priv;
	}
	/********  NVGENS   **********/
	csb->which = arg->subfunc;

	if ((csb->sense == FALSE) && (arg->flags & NO_NUMBER)) {
	    /********  NOPREFIX  *********/
	    /*
	     * We've parsed a 'no' prefix and must consume the remainder of
	     * the line, then look for EOL.
	     */
	    for (k = 0; csb->line[csb->line_index + k] != '\n' &&
		 csb->line[csb->line_index + k]; k++) {
		/* Null body */
	    }
	    /********  EOLS  ***********/
	    csb->line_index += k;
	    csb->which = arg->subfunc;
	    eol_action(csb, mine, arg->func);
	    csb->line_index -= k;
	    /******  End EOLS  *********/
	    /******** End NOPREFIX ********/
	} else {
	    /********  DECIMAL  **********/
	    k = 0;

	    CHECK_RANGE_BOUNDS_ALIST();

	    if (match_number(csb, lower, upper, &k, &num,
			     arg->flags, arg->varhelp)) {

		*CSBVAR(csb, arg->offset, uint) = num;

		/********  EOLS  ***********/
		csb->line_index += k;
		csb->which = arg->subfunc;
		eol_action(csb, mine, arg->func);
		csb->line_index -= k;
		/******  End EOLS  *********/
	    } else if (match_number(csb, lower2, upper2, &k, &num,
				    arg->flags & ~NUMBER_HELP_CHECK,
				    NULL)) {

		/* Convert to other numbering scheme and save */
		*CSBVAR(csb, arg->offset, uint) = num + lower - lower2;

		/********  EOLS  ***********/
		csb->line_index += k;
		csb->which = arg->subfunc;
		eol_action(csb, mine, arg->func);
		csb->line_index -= k;
		/******  End EOLS  *********/
	    } else {
		/********  NO_ALT  ***********/
		csb->line_index += k;
		no_alt_action(csb);
		csb->line_index -= k;
		/*********  End NO_ALT    **********/
	    }
	    /*********  End DECIMAL   **********/
	}

	csb->line_index -= i;
	/*********  End NVGENS    **********/
    }
}

boolean match_minusone (char *buf, int *pi)
{
    if ((buf[*pi] == '-') && (buf[1 + *pi] == '1')) {
	(*pi) += 2;
	return(TRUE);
    }
    return(FALSE);
}

/*
 * Match a decimal short unsigned int in a range
 */
boolean match_decimal (char *buf, uint low, uint high, int *pi, ushort *pvalue)
{
    ushort value = 0;
    uint i;

    buf += *pi;
    *pvalue = 0;

    i = 0;
    if (buf[i] == '+')
	i++;

    for (; isdigit(buf[i]); i++) {
	if ((value > USHORT_MAX / 10) ||
	    ((value == USHORT_MAX / 10) &&
	     ((buf[i] - '0') > USHORT_MAX % 10))) {
	    *pi += i;
	    return(FALSE);
	}
	value = 10 * value + buf[i] - '0';
	if (value > high) {
	    *pi += i;
	    return(FALSE);
	}
    }

    if (i && (value >= low)) {
	*pi += i;
	*pvalue = value;
	return(TRUE);
    }
    return(FALSE);
}

/*
 * Scan octal as an unsigned int.  Error if too many
 * octal digits
 */
boolean match_octal (char *buf, uint low, uint high, int *pi, ushort * pvalue)
{
    uint i = 0;
    ulong value = 0L;

    /* Fail if there is not at least one octal digit */
    *pvalue = 0L;
    buf += *pi;

    if (!isodigit(buf[i])) {
	return(FALSE);
    }

    for (; isodigit(buf[i]); i++) {
	if (value > USHORT_MAX / 8) {
	    *pi += i;
	    return(FALSE);
	}
	value = 8 * value + buf[i] - '0';
	if (value > high) {
	    *pi += i;
	    return(FALSE);
	}
    }
    if (i && (value >= (ulong) low)) {
	*pvalue = (ushort) value;
	*pi += i;
	return(TRUE);
    }
    return(FALSE);
}

/*
 * ASSUMES: no leading 0x.
 *
 */
boolean match_hexadecimal (char *buf, int *pi, ushort * pvalue)
{
    int i;
    char c;
    ushort value = 0;

    buf += *pi;
    *pvalue = 0;

    for (i = 0, c = toupper(buf[i]); isxdigit(c); i++, c = toupper(buf[i])) {
	if (value > USHORT_MAX / 16) {
	    *pi += i;
	    return(FALSE);
	}
	value = value * 16 + CVHTOB(c);
    }

    if (i > 0) {
	(*pi) += i;
	*pvalue = value;
	return(TRUE);
    }
    return(FALSE);
}


/*
 * Try to match an hexadecimal ulong, with value
 * between (or equal to) <low> and <high>. <*pi> is incremented
 * by the number of characters in the match.
 *
 * ASSUMES: no leading 0x.
 *
 */
boolean match_ulong_hexadecimal (char *buf, ulong low, ulong high,
				 uint *pi, ulong *pvalue)
{
    uint i;
    ulong value = 0L;
    char c;

    buf += *pi;
    *pvalue = 0L;

    for (i = 0, c = toupper(buf[i]); isxdigit(c); i++, c = toupper(buf[i])) {
	if (value > ULONG_MAX / 16) {
	    /* Too many non-zero digits, max out. */
	    *pi += i;
	    return(FALSE);
	}
	value = value * 16 + CVHTOB(c);
	if (value > high) {
	    *pi += i;
	    return(FALSE);
	}
    }

    if (i && (value >= low)) {
	*pvalue = value;
	*pi += i;
	return(TRUE);
    }
    return(FALSE);
}



/*
 * Scan octal as an unsigned LONG int. Error if too many octal
 * digits.
 */
boolean match_ulong_octal (char *buf, ulong low,
			   ulong high, uint *pi, ulong *pvalue)
{
    uint i = 0;
    ulong value = 0L;

    /* Fail if there is not at least one octal digit */
    *pvalue = 0L;
    buf += *pi;

    if (!isodigit(buf[i])) {
	return(FALSE);
    }

    for (; isodigit(buf[i]); i++) {
	/* Make sure we don't have a 33-rd bit */
	if (value > ULONG_MAX / 8) {
	    *pi += i;
	    return(FALSE);
	}
	value = 8 * value + buf[i] - '0';
	if (value > high) {
	    *pi += i;
	    return(FALSE);
	}
    }
    if (i && (value >= low)) {
	*pvalue = value;
	*pi += i;
	return(TRUE);
    }
    return(FALSE);
}

/*
 * Try to match an unsigned LONG int,
 * with value between (or equal to) <low> and
 * <high>. <*pi> is incremented by the number of characters in
 * the match.
 */
boolean match_ulong_decimal (char *buf,ulong low, ulong high,
			     uint *pi, ulong *pvalue)
{
    uint i;
    ulong value = 0L;
    char c;

    buf += *pi;
    *pvalue = 0L;

    i = 0;
    if (buf[i] == '+')
	i++;

    for (c = buf[i]; isdigit(c); c = buf[++i]) {
	if ((value > (ULONG_MAX / 10)) ||
	    ((value == (ULONG_MAX / 10)) &&
	     (((ulong)(c - '0')) > (ULONG_MAX % 10)))) {
	    /* Too many chars, max out. */
	    *pi += i;
	    return(FALSE);
	}
	value = value * 10 + (ulong) (c - '0');
	if (value > high) {
	    *pi += i;
	    return(FALSE);
	}
    }

    if (i && (value >= low)) {
	*pvalue = value;
	*pi += i;
	return(TRUE);
    }
    return(FALSE);
}


/*
 * Try to match an signed int,
 * with value between (or equal to) <low> and
 * <high>. <*pi> is incremented by the number of characters in
 * the match.
 *
 * Here's a command to test this function with
    int e0
    ip irdp
    ip irdp address 1.1.1.1 -2147483649		! fail
    ip irdp address 1.1.1.2 -2147483648		! succeed
    ip irdp address 1.1.1.3 -1			! succeed
    ip irdp address 1.1.1.4 -0			! succeed
    ip irdp address 1.1.1.5 -			! fail
    ip irdp address 1.1.1.6			! fail
    ip irdp address 1.1.1.7 0			! succeed
    ip irdp address 1.1.1.8 1			! succeed
    ip irdp address 1.1.1.9 2147483647		! succeed
    ip irdp address 1.1.1.10 2147483648		! fail
 */
static boolean match_signed_dec (char *buf, int low, int high,
				 int *pi, int *pvalue)
{
    int i;
    int value = 0;
    char c;
    int sign;

    buf += *pi;
    *pvalue = 0;

    i = 0;
    if (buf[i] == '-') {
	sign = -1;
	i++;
    } else {
	sign = 1;
    }
    if (buf[i] == '+')
	i++;

    for (c = buf[i]; isdigit(c); c = buf[++i]) {
	if (((value*10 + (c - '0')) < value) &&
	    ((sign != -1) || ((value*10 + (int)(c - '0')) != 1<<31))) {
	    /* Too many chars, max out. */
	    *pi += i;
	    return(FALSE);
	}
	value = value * 10 + (int) (c - '0');
	if ((value*sign) > high) {
	    *pi += i;
	    return(FALSE);
	}
    }

    *pi += i;
    /*
     * if (number is positive and we parsed one or more numbers) or
     *    (number is negative and we parsed '-' plus one or more numbers)
     */
     if (((sign == 1) && (i > 0)) ||
	 ((sign == -1) && (i > 1))) {

	 /*
	  *  if (number is greater than or equal to the lower boundary)
	  */
	 if ((value*sign) >= low) {
	     *pvalue = value*sign;
	     return(TRUE);
	 }
    }
    return(FALSE);
}

/* 
 * SNPAADDR - get an SNPA address 
 */

void snpaaddr_action (parseinfo *csb, transition *mine,
		      addrparse_struct * const arg)
{
    addrparse_struct station_ieee48 = {arg->offset, arg->help,
				       ADDR_PARSE(STATION_IEEE48),
				       arg->flag};
    addrparse_struct station_smds64 = {arg->offset, arg->help,
				       ADDR_PARSE(STATION_SMDS64),
				       arg->flag};
    addrparse_struct station_x121 = {arg->offset, arg->help,
				     ADDR_PARSE(STATION_X121),
				     arg->flag};

    switch (GETOBJ(idb,1)->hwptr->enctype) {
      case ET_ARPA:
      case ET_SAP:
      case ET_SNAP:
	generic_addr_action(csb, mine, &station_ieee48);
	break;

      case ET_SMDS:
	generic_addr_action(csb, mine, &station_smds64);
	break;

      case ET_X25:
	generic_addr_action(csb, mine, &station_x121);
	break;

      case ET_HDLC:
      default:
	printf("\n%%CLNS: Static IS & ES entries may not be "
	       "made for %s interfaces",
	       encstring(GETOBJ(idb,1)->hwptr->enctype));
	break;
    }
}

/* ------------------------------------------------------------ */
/* ROUTER-related code follows.                                 */
/* ------------------------------------------------------------ */

/*
 * Different commands to test the parsing in router_action()
 *
en
rHoz
show ip route ?
show ip route rip?
show ip route rip ?
show ip route rip
show ip route bgp?
show ip route bgp ?
show ip route bgp 3?
show ip route bgp 32 ?
show ip route bgp 32
show ip route ospf?
show ip route ospf ?
show ip route ospf 2?
show ip route ospf 23 ?
show ip route ospf 23
show ip route isis?
show ip route isis ?
show ip route isis
show ip route isis foo?
show ip route isis foobar ?
show ip route isis foobar
conf term
router ?
router rip?
router rip ?
router rip
no router rip
autonomous-system 22
router egp?
router egp ?
router egp 4?
router egp 45 ?
router egp 45
no router egp 45
router ospf?
router ospf ?
router ospf 6?
router ospf 67 ?
router ospf 67
no router ospf 67
router iso-igrp?
router iso-igrp ?
router iso-igrp
no router iso-igrp
router iso-igrp ack?
router iso-igrp acknot ?
router iso-igrp acknot
no router iso-igrp acknot
router ospf 234
redistribute ?
redistribute rip?
redistribute rip ?
redistribute rip
redistribute igrp?
redistribute igrp ?
redistribute igrp 2?
redistribute igrp 23 ?
redistribute igrp 23
redistribute ospf?
redistribute ospf ?
redistribute ospf 4?
redistribute ospf 45 ?
redistribute ospf 45
redistribute isis?
redistribute isis ?
redistribute isis
no router isis
router ospf 234
redistribute isis foo?
redistribute isis foobar ?
redistribute isis foobar
 *
 */

routerprotocolinfo routerprotocols[] =
{
    {PROC_BGP, RTRNAME_BGP, "Border Gateway Protocol (BGP)", ROARG_AS},
    {PROC_CONNECTED, RTRNAME_CONNECTED, "Connected", ROARG_NONE},
    {PROC_EGP, RTRNAME_EGP, "Exterior Gateway Protocol (EGP)", ROARG_AS},
    {PROC_IGRP, RTRNAME_IGRP, "Interior Gateway Routing Protocol (IGRP)",
	 ROARG_AS},
    {PROC_RIP, RTRNAME_RIP, "Routing Information Protocol (RIP)",
	 ROARG_NONE},
    {PROC_ISIS, RTRNAME_ISIS, "ISO IS-IS", ROARG_TAG},
    {PROC_ISOIGRP, RTRNAME_ISOIGRP, "IGRP for OSI networks", ROARG_TAG},
    {PROC_NLSP, RTRNAME_NLSP, "IPX Netware Link Services Protocol", 
	 ROARG_TAG},
    {PROC_OSPF, RTRNAME_OSPF, "Open Shortest Path First (OSPF)", ROARG_PID},
    {PROC_STATIC, RTRNAME_STATIC, "Static routes", ROARG_NONE},
    {PROC_IGRP2, RTRNAME_IGRP2, 
	 "Enhanced Interior Gateway Routing Protocol (EIGRP)", ROARG_AS},
    {PROC_ODR, RTRNAME_ODR, "On Demand stub Routes", ROARG_NONE},
    {PROC_MOBILE, RTRNAME_MOBILE, "Mobile routes", ROARG_NONE},
    {PROC_FSTATIC, RTRNAME_FSTATIC, "Floating static routes", ROARG_NONE},
    {0, NULL, NULL, 0}
};



void router_action (parseinfo *csb, transition *mine,
		    routerparse_struct * const arg)
{
    routerprotocolinfo *pi;
    struct rtrname *prname;
    char *cp;
    int i;
    int j;

    push_node(csb, mine->alternate);

    if (csb->nvgen) {

	for (pi = routerprotocols; pi->str; pi++) {
	    /*
	     * NVGEN of router commands.  For each routing protocol type, as
	     * selected by the above loop, find each running process and push
	     * the accepting node for it.  We delay output of the actual
	     * command until router_command() is called. This is because as
	     * each node is popped, router_command() will be called.  It
	     * outputs the command line and pushes a node for the top of the
	     * routing parse tree.  The router subcommands are then generated.
	     * When that tree is exhausted, the next router node is popped and
	     * the cycle repeats until all routing commands have been
	     * processed. This is the same basic operation as used in
	     * interface_action().
	     */
	    for (cp = csb->nv_command; *cp; cp++) {
		/* Null body */
	    }

	    if (router_names == NULL) {
		/* Ain't none */
		return;
	    }

	    for (i=0; router_names[i].type != RMATCH_NONE; i++ ){
		if (arg->protocol & router_names[i].type) {
		    router_names[i].func(csb, pi->str, cp, mine->accept);
		}
	    }

	    SETOBJ(pdb,1) = NULL;
	    SETOBJ(int,1) = 0;
	    *cp = '\0';
	}
	return;
    }

    prname = CSBVAR(csb, arg->offset, struct rtrname);
    for (pi = routerprotocols; pi->str; pi++) {
	uint help_flags;

	if (arg->protocol & RMATCH_NO_KEY_HELP) {
	    help_flags = KEYWORD_WS_OK;
	} else {
	    help_flags = KEYWORD_WS_OK | KEYWORD_HELP_CHECK;
	}
	i = 0;
	if ((pi->protocol & arg->protocol) &&
	    match_partial_keyword(csb, pi->str, &i, pi->help, 0, help_flags)) {
	    csb->line_index += i;
	    prname->myname = pi->str;
	    prname->arg_specified = FALSE;
	    prname->protocol = pi->protocol;
	    j = 0;
	    if (((arg->protocol & RMATCH_ARGS) &&
		 process_partial_protocol_action(csb, &j, prname, pi, arg)) ||
		(arg->protocol & RMATCH_NO_ARGS)) {
		csb->line_index += j;
		push_node(csb, mine->accept);
		csb->line_index -= j;
	    }
	    csb->line_index -= i;
	}
    }
}

/*
 * We got a protocol keyword partial match. This function does the rest of
 * the necessary matching.
 */
static boolean process_partial_protocol_action (parseinfo *csb, int *i,
						struct rtrname *prname,
						routerprotocolinfo *pi,
						routerparse_struct *const arg)
{
    int finished_match = 0;
    ushort asval;
    int len;
    char *short_help;
    char *long_help;
    int doing_long_help = 0;
    uint lbound;
    char *buf;

    doing_long_help = help_check(csb);
    buf = &csb->line[csb->line_index];

    switch (pi->type) {
      case ROARG_NONE:
	/* no other argument expected */
	finished_match = 1;
	short_help = "";
	long_help = "";
	break;
      case ROARG_AS:
	/* Get AS (autonomous system) argument. */
	/* EGP: cisco source allows AS to be 0 */
	
	lbound = (!strcasecmp(RTRNAME_EGP, pi->str)) ? 0 : 1;
	short_help = lbound ? "<1-65535>" : "<0-65535>";
	if (doing_long_help) {
	    if (!strcasecmp(RTRNAME_EGP, pi->str)) {
		long_help = "Remote autonomous system number";
	    } else {
		long_help = "Autonomous system number";
	    }
	    if (csb->flags & CONFIG_HTTP) {
		parser_http_form(csb, PARSER_HTTP_FORM_NUMBER,
				 short_help, long_help);
		return(FALSE);
	    }
	} else {
	    long_help = "";
	    if (match_decimal(buf, lbound, 65535, i, &asval) &&
		match_whitespace2(buf, i, csb->in_help)) {
		finished_match = 1;
		prname->as = (ushort) asval;
		prname->arg_specified = TRUE;
		prname->tag[0] = '\0';
		save_ambig_uint(csb, short_help, asval);
	    }
	}
	break;

      case ROARG_PID:
	/* Get Process ID */
	short_help = "<1-65535>";
	if (doing_long_help) {
	    long_help = "Process ID";
	    if (csb->flags & CONFIG_HTTP) {
		parser_http_form(csb, PARSER_HTTP_FORM_NUMBER,
				 short_help, long_help);
		return(FALSE);
	    }
	} else {
	    long_help = "";
	    if (match_decimal(buf, 1, 65535, i, &asval) &&
		match_whitespace2(buf, i, csb->in_help)) {

		finished_match = 1;
		prname->as = (ushort) asval;
		prname->arg_specified = TRUE;
		prname->tag[0] = '\0';
		save_ambig_uint(csb, short_help, asval);
	    }
	}
	break;

      case ROARG_TAG:
	short_help = "WORD";
	if (doing_long_help) {
	    if (arg->protocol & RMATCH_NO_TAG) {
		/* no match, so no help */
		long_help = "";
	    } else {
		long_help = "ISO routing area tag";
		if (csb->flags & CONFIG_HTTP) {
		    parser_http_form(csb, PARSER_HTTP_FORM_WORD,
				     short_help, long_help);
		    return(FALSE);
		}
	    }
	    finished_match = 1;
	} else {
	    long_help = "";
	    if (!(arg->protocol & RMATCH_NO_TAG)) {
		len = copy_varstring(buf + *i, prname->tag,
				     MAXTOKLEN, STRING_QUOTED);
	    } else {
		len = 0;
	    }
	    if (len > 0) {
		*i += len;
		if (match_whitespace2(buf, i, csb->in_help)) {
		    finished_match = 1;
		    prname->as = 0;
		    prname->arg_specified = TRUE;
		    save_ambig_string(csb, short_help, buf + *i);
		}
	    } else {
		if (!(arg->protocol & RMATCH_TAG_REQUIRED)) {
		    /*
		     * Null tag is acceptable, so accept anyway.
		     */
		    finished_match = 1;
		    prname->arg_specified = TRUE;
		    save_ambig_string(csb, short_help, buf + *i);
		}
		prname->tag[0] = '\0';
		prname->as = 0;
	    }
	}
	break;

      default:
	/* Ack */
	short_help = "";
	long_help = "";
	break;
    }

    if (finished_match) {
	if (csb->in_help && long_help[0]) {
	    save_help_long(csb, short_help, long_help);
	}
	return(TRUE);
    } else {
	if (doing_long_help) {
	    save_help_long(csb, short_help, long_help);
	} else {
	    help_or_error(csb, *i, short_help, NO_COMMAND_COMPLETION);
	}
	return(FALSE);
    }
}

/* ------------------------------------------------------------ */

boolean match_partial_keyword (parseinfo *csb, const char *keyword,
				      int *pi, const char *help,
				      int minmatch, uint flags)
{
    int i;
    char *buf = csb->line + csb->line_index + *pi;

#ifdef	DEBUG_KEYWORD
    printf("\nmatch_partial_keyword: Entering");
#endif	DEBUG_KEYWORD
    csb->line_index += *pi;
    if (long_help_check(csb)) {
	if (flags & KEYWORD_HELP_CHECK) {
	    save_help_long(csb, keyword, help);
	}
#ifdef	DEBUG_KEYWORD
	printf("\nmatch_partial_keyword: Long help");
#endif	DEBUG_KEYWORD
	csb->line_index -= *pi;
	return(FALSE);
    }
    csb->line_index -= *pi;
	
    for (i = 0; (toupper(buf[i]) == toupper(keyword[i])) && buf[i]; i++) {
	;                       /* Null body */
    }
    (*pi) += i;
#ifdef	DEBUG_KEYWORD
    printf("\nmatch_partial_keyword: Matched %d letters of keyword %s", i, keyword);
#endif	DEBUG_KEYWORD
    if (i > 0) {
	if ((!minmatch || (i >= minmatch)) &&
	    (((flags & KEYWORD_WS_OK) &&
	     match_whitespace2(csb->line + csb->line_index, pi, csb->in_help))
	     || (flags & KEYWORD_NO_WS_OK))) {

#ifdef	DEBUG_KEYWORD
	    printf("\nmatch_partial_keyword: Matched whitespace");
#endif	DEBUG_KEYWORD
	    csb->line_index += *pi;
	    if ((flags & KEYWORD_NO_WS_OK) && (short_help_check(csb))) {
		if (flags & KEYWORD_HELP_CHECK) {
		    save_help_short(csb, keyword, SUPPORT_COMMAND_COMPLETION);
		}
		csb->line_index -= *pi;
		return(FALSE);
	    }
	    if (parser_ambig_debug) {
		buginf("\nMatched keyword '%s'", keyword);
		buginf("\ncsb->command_visible = %d", csb->command_visible);
	    }
	    save_ambig_string(csb, keyword, keyword);
	    csb->line_index -= *pi;
	    csb->flags |= KEYWORD_MATCHED;
	    return(TRUE);

	} else {
#ifdef	DEBUG_KEYWORD
	    printf("\nmatch_partial_keyword: Didn't match whitespace");
#endif	DEBUG_KEYWORD
	    if ((flags & KEYWORD_HELP_CHECK) && (flags & KEYWORD_WS_OK)) {
		help_or_error(csb, *pi, keyword, SUPPORT_COMMAND_COMPLETION);
	    } else {
		csb->line_index += *pi;
		set_error_index(csb);
		csb->line_index -= *pi;
	    }
	}
    } else {
	/* didn't match any characters, set error index */
	csb->line_index += *pi;
	set_error_index(csb);
	csb->line_index -= *pi;
    }
    return(FALSE);
}

static void string_escape (char **from, char **to, uint *i)
{
    uint count;
    uint value;
    char *pfrom = *from;
    uint pi = *i;
    
    switch (**from) {
      default:
	/* Put back backslash */
	**to = '\\';
	(*to)++;
	if (!*from) {
	    break;
	}
	/* Fall through */
      case '"':
	**to = **from;
	(*to)++;
	(*from)++;
	(*i)++;
	break;
      case 'x':
	pfrom++;
	pi++;
	count = 0;
	value = 0;
	while ((count < 2) && *pfrom && isxdigit(*pfrom)) {
	    value = (value*16) + CVHTOB(*pfrom);
	    pfrom++;
	    pi++;
	    count++;
	}
	if (count != 2) {
	    **to = '\\';
	    (*to)++;
	    **to = **from;
	    (*from)++;
	    (*i)++;
	} else {
	    **to = value;
	    *from = pfrom;
	    *i = pi;
	}
	(*to)++;
	break;
      case '0': case '1': case '2': case '3':
      case '4': case '5': case '6': case '7':
	value = 0;
	count = 0;
	while ((count < 3) && *pfrom &&
	       (*pfrom >= '0') && (*pfrom <= '7')) {
	    value = (value*8) + (*pfrom - '0');
	    pfrom++;
	    pi++;	
	    count++;
	}
	if (count != 3) {
	    **to = '\\';
	    (*to)++;
	    **to = **from;
	    (*from)++;
	    (*i)++;
	} else {
	    **to = value;
	    *from = pfrom;
	    *i = pi;
	}
	(*to)++;
	break;
    }
}
/*
 * Copies at most <maxchar-1> non-whitespace characters from input buffer
 * to output buffer. Returns string length.  If the string is quoted,
 * then it may contain any character, including whitespace.
 */
uint copy_varstring (char *from, char *to, uint maxchar, uint flags)
{
    uint i;	/* The number of characters matched */

    if ((*from == '"') &&
	(flags & STRING_QUOTED) &&
	(!(flags & STRING_WS_OK))) {

	/* Quoted string */
	from++;
	for (i = 1; i < maxchar; i++) {
	    if (*from == '\0') {
		printf("\nWarning: Assumed end-quote for quoted string");
		break;
	    }
	    /* If we find a quote, it must be backslash escaped to be kept.
	     * Terminate the string otherwise.
	     */
	    if (*from == '"') {
		i++;		/* advance past trailing quote */
		i = min(i, (maxchar - 1));
		*to = '\0';
		return(i);
	    }
	    if (*from == '\\') {
		from++;
		string_escape(&from, &to, &i);
	    } else {
		*to++ = *from++;
	    }
	}
    } else {
	/* Just get the next token */
	for (i = 0; (i < maxchar) && *from; i++) {
	    if (*from == '\\') {
		from++;
		string_escape(&from, &to, &i);
	    } else {
		if (!(flags & STRING_WS_OK) && isspace(*from)) {
		    break;
		}
		*to++ = *from++;
	    }
	}
    }

    i = min(i, (maxchar - 1));
    *to = '\0';

    return(i);
}


void grouplist_action (parseinfo *csb, transition *mine,
		       string_struct * const arg)
{
    int i = 0;
    int j = 0;
    char *str;
    char *short_help = "<grouplist>";

    push_node(csb, mine->alternate);

    INVALID_NVGEN_CHECK;

    if (help_check(csb)) {
	save_help_long(csb, short_help, arg->help);
	return;
    }

    str = CSBVAR(csb, arg->offset, char);
    memset(str, 0, 256/8);
    if (match_grouplist(&csb->line[csb->line_index], &i, str) &&
	match_whitespace2(csb->line + csb->line_index + i, &j, csb->in_help)) {

	save_ambig_nstring(csb, short_help, &(csb->line[csb->line_index]), i);
	csb->line_index += (i + j);
	push_node(csb, mine->accept);
	csb->line_index -= (i + j);

    } else {
	/*
	 * The match failed, whitespace missing, etc.
	 */
	memset(str, 0, 256/8);
	help_or_error(csb, i + j, "<grouplist>", NO_COMMAND_COMPLETION);
    }
}

/* Match grouplist number or range. */
static boolean match_grouplist_item (char *buf, int *pi,
				     char *num, boolean first)
{
    ushort start, stop;
    int i = *pi;

    i = *pi;
    if (!match_decimal(buf, 0, 255, &i, &start)) {
	char name[128];
	/* Cannot match initial number. */

	/*
	 * Try it as a named group code. We won't worry about "all" (which
	 * precedes a group list) or "enable/disable" (which follow a group
	 * list), because the rest of the parser should handle them. And if
	 * someone defines a named group with one of those names, they deserve
	 * what'll happen.
	 */

	/*
	 * We assume find_lat_group scans a name terminated in whitespace, and
	 * puts the char count into i. It returns TRUE if it finds the group
	 * name, FALSE if not.
	 */
	if (!reg_invoke_match_group_name(buf, &i, num, name)) {
	    int j = 0;

	    if (name[0] == '\0') {
		return(FALSE);
	    }
	    if (first && (toupper(name[0]) == 'A') &&
		(toupper(name[1]) == 'L')) {
		start = 0;
		stop = 255;
		*pi += j;
	    } else if (!first) {
		return(FALSE);
	    } else {
		printf("\n%%Invalid group-list name '%s'", name);
		return(FALSE);
	    }
	}
#ifdef DEBUG_GL
	printf("\nMatched item starting at %s\n", buf + *pi);
#endif
	*pi = i;
	return(TRUE);
    }

    if (match_char(buf, &i, '-')) {
	/* Looks like a range. */
	if (!match_decimal(buf, 0, 255, &i, &stop)) {
	    /* No match: don't alter *pi. */
	    return(FALSE);
	}
#ifdef DEBUG_GL
	printf("\nMatched item starting at %s\n", buf + *pi);
#endif
    } else {
	stop = start;
    }

#ifdef DEBUG_GL
    printf("\nMatched item starting at %s\n", buf + *pi);
#endif

    *pi = i;
    for (i = start; i <= stop; i++) {
	num[ i >> 3 ] |= 1 << (i & 7);
    }
    return(TRUE);
}

/*
 * Match characters as a whitespace-separated list of DECIMAL numbers
 * between 0 and 255. There may also be ranges of such numbers, indicated
 * with a hyphen (20-24, for example).
 *
 * RETURNS: TRUE if there is some initial part of a list. I.e. does NOT check
 * for correct whitespace/eol termination.
 *
 */
static boolean match_grouplist (char *buf, int *pi, char *num)
{
    int i = *pi;

    buf += i;

    if (!match_grouplist_item(buf, &i, num, TRUE)) {
	/* Cannot get initial list item. */
	return(FALSE);
    }

    *pi = i;

    /* Match space, number pairs. */
    while (match_spaces(buf, &i) &&
	   match_grouplist_item(buf, &i, num, FALSE)) {
	/* If matched the pair, update *pi. */
	*pi = i;
    }

    /* End: how terminated ? */
    return(TRUE);
}

static boolean match_spaces (char *buf, int *pi)
{
    int k;

    buf += *pi;
    for (k = 0; isspace(buf[k]); k++) {
	;			/* Null body */
    }

    *pi += k;

    return((k > 0) ? TRUE : FALSE);
}

/*
 * Recognize single or paired hex digits in buf. Convert each pair to a
 * byte, and store that byte in outbuf. maxlen is the most chars that can
 * be stored in outbuf.
 *
 * RETURNS: number of characters matched. *pchars is the number of chars
 * stored.
 *
 */
static int match_hex_digits (char *buf, char *outbuf, int maxlen, int *pchars)
{
    int i;
    char *s;
    char *ptr = buf;

    for (i = 0; i < maxlen; i++) {
	s = hex_to_byte(ptr, outbuf);
	if (s == NULL) {
	    break;
	}
	ptr = s;
	outbuf++;
    }
    *pchars = i;
    return(ptr - buf);
}

/*
 * Converts one or two hex digits from s to a hex number, and stores it in
 * out. Returns pointer to next non-consumed char (NULL if first char is
 * not a hex digit or if it is end of string).
 */
static char *hex_to_byte (char *s, char *out)
{
    int i = 0;

    match_spaces(s, &i);
    s += i;
    *out = 0;

    if ((*s == '\0') || !isxdigit(*s)) {
	return(NULL);
    }

    *out = CVHTOB(*s);
    s++;

    if ((*s != '\0') && isxdigit(*s)) {
	*out = (*out << 4) + CVHTOB(*s);
	s++;
    }
    return(s);
}

void hexdata_action (parseinfo *csb, transition *mine,
		     hexdata_struct * const arg)
{
    int i;
    int stored = 0;
    char short_help[] = "Hex-data";

    push_node(csb, mine->alternate);

    INVALID_NVGEN_CHECK;

    if (help_check(csb)) {
	save_help_long(csb, short_help, arg->help);
	return;
    }

    i = match_hex_digits(csb->line + csb->line_index,
			 CSBVAR(csb, arg->bufoffset, char),
			 arg->bufmax, &stored);
    if (i) {
	(void) match_whitespace2(csb->line + csb->line_index,
				 &i, csb->in_help);
    }

    /*
     * The following catches all help ?'s in the middle of a stream.
     */

    csb->line_index += i;
    if (help_check(csb)) {
	save_help_short(csb, short_help, NO_COMMAND_COMPLETION);
    } else {
	if (stored > 0) {
	    /* success */
	    /* string output stored already */
	    *CSBVAR(csb, arg->numoffset, uint) = stored;
	    push_node(csb, mine->accept);
	} else {
	    set_error_index(csb);
	}
    }
    csb->line_index -= i;
}

/*
 * Like char_action, but deals with characters which might be taken
 * as designating end of line (like '#'), confusing any (prior)
 * help_check full help check. These characters can only be correctly
 * processed by not allowing alternative interpretations !!!
 */
void literal_char_action (parseinfo *csb, transition *mine,
			  char_struct2 * const arg)
{
    push_node(csb, mine->alternate);

    INVALID_NVGEN_CHECK;

    if (csb->line[csb->line_index] == arg->character) {
	csb->line_index++;
	push_node(csb, mine->accept);
	csb->line_index--;
    } else {
	/* Can only have help if the character is NOT found. */
	if (help_check(csb)) {
	    save_help_short(csb, arg->help, NO_COMMAND_COMPLETION);
	}
    }
}

void help_or_error (parseinfo *csb, int count, const char *msg,
		    boolean do_command_completion)
{
    csb->line_index += count;
    if (help_check(csb)) {
	save_help_short(csb, msg, do_command_completion);
    } else {
	/* Not doing help.  This is an error. */
	set_error_index(csb);
    }
    csb->line_index -= count;
}

static void set_error_index (parseinfo *csb)
{
    if (csb->error_index < csb->line_index) {
	csb->error_index = csb->line_index;
    }
}

static void match_number_hex (char *buf, uint low, uint high,
			      int *pi, uint *pvalue, uint *flags)
{
    char c;
    int i = 0;
    uint ret = 0;

    buf += *pi;
    *pvalue = 0;
    *flags &= ~NUMBER_HEX;

#ifdef	DEBUG_NUMBER
    printf("\nmatch_number_hex: Entering");
    printf("\nmatch_number_hex: buf = '%s'", buf);
#endif	DEBUG_NUMBER
    if ((buf[0] == '0') && (toupper(buf[1]) == 'X')) {
	if (*flags & HEX_ZEROX_OK) {
	    /* saw '0x' and it is ok */
	    i += 2;
#ifdef	DEBUG_NUMBER
	    printf("\nmatch_number_hex: matched '0x'");
#endif	DEBUG_NUMBER
	    ret = HEX_ZEROX_MATCHED;
	} else {
	    /* saw '0x' and it is not ok, return false */
#ifdef	DEBUG_NUMBER
	    printf("\nmatch_number_hex: matched '0x' and not ok");
#endif	DEBUG_NUMBER
	    return;
	}
    } else {
	if (*flags & HEX_NO_ZEROX_OK) {
	    /* didn't see '0x' and that is ok */
#ifdef	DEBUG_NUMBER
	    printf("\nmatch_number_hex: didn't match '0x'");
#endif	DEBUG_NUMBER
	    ret = HEX_ZEROX_NOT_MATCHED;
	} else {
	    /* didn't see '0x' and it is not ok, return false */
#ifdef	DEBUG_NUMBER
	    printf("\nmatch_number_hex: didn't match '0x' and not ok");
#endif	DEBUG_NUMBER
	    return;
	}
    }

#ifdef	DEBUG_NUMBER
    printf("\nmatch_number_hex: trying char '%c'", buf[i]);
#endif	DEBUG_NUMBER
    /* i has already been initialized */
    for (c = toupper(buf[i]); isxdigit(c); i++, c = toupper(buf[i])) {
	if (*pvalue > ULONG_MAX / 16) {
	    /* Too many non-zero digits, max out. */
#ifdef	DEBUG_NUMBER
	    printf("\nmatch_number_hex: too many chars %d", i);
#endif	DEBUG_NUMBER
	    *pi += i;
	    return;
	}
	*pvalue = *pvalue * 16 + CVHTOB(c);
	if (*pvalue > high) {
#ifdef	DEBUG_NUMBER
	    printf("\nmatch_number_hex: value > high i = %d", i);
#endif	DEBUG_NUMBER
	    *pi += i;
	    return;
	}
#ifdef	DEBUG_NUMBER
	printf("\nmatch_number_hex: trying char '%c'", buf[i+1]);
#endif	DEBUG_NUMBER
    }

    if (i && (*pvalue >= low)) {
	*pi += i;
	*flags |= ret;
	*flags |= NUMBER_HEX;
#ifdef	DEBUG_NUMBER
	printf("\nmatch_number_hex: matched number 0x%x", *pvalue);
#endif	DEBUG_NUMBER
	return;
    }
#ifdef	DEBUG_NUMBER
    if (!i) {
	printf("\nmatch_number_hex: matched 0 numbers");
    } else {
	printf("\nmatch_number_hex: value < low");
    }
#endif	DEBUG_NUMBER
    return;
}
	
static void match_number_oct (char *buf, uint low, uint high,
			      int *pi, uint *pvalue, uint *flags)
{
    int i = 0;
    uint ret = 0;

    buf += *pi;
    *pvalue = 0;
    *flags &= ~NUMBER_OCT;

#ifdef	DEBUG_NUMBER
    printf("\nmatch_number_oct: Entering");
    printf("\nmatch_number_oct: buf = '%s'", buf);
#endif	DEBUG_NUMBER
    if (buf[0] == '0') {
	if (*flags & OCT_ZERO_OK) {
#ifdef	DEBUG_NUMBER
	    printf("\nmatch_number_oct: matched '0'");
#endif	DEBUG_NUMBER
	    ret = OCT_ZERO_MATCHED;
	} else {
#ifdef	DEBUG_NUMBER
	    printf("\nmatch_number_oct: matched '0' and not ok");
#endif	DEBUG_NUMBER
	    return;
	}
    } else {
	if (*flags & OCT_NO_ZERO_OK) {
#ifdef	DEBUG_NUMBER
	    printf("\nmatch_number_oct: didn't match '0'");
#endif	DEBUG_NUMBER
	    ret = OCT_ZERO_NOT_MATCHED;
	} else {
#ifdef	DEBUG_NUMBER
	    printf("\nmatch_number_oct: didn't match '0' and not ok");
#endif	DEBUG_NUMBER
	    return;
	}
    }

#ifdef	DEBUG_NUMBER
    printf("\nmatch_number_oct: trying char '%c'", buf[i]);
#endif	DEBUG_NUMBER
    for (i=0; isodigit(buf[i]); i++) {
	/* Make sure we don't have a 33-rd bit */
	if (*pvalue > ULONG_MAX / 8) {
	    *pi += i;
#ifdef	DEBUG_NUMBER
	    printf("\nmatch_number_oct: too many chars %d", i);
#endif	DEBUG_NUMBER
	    return;
	}
	*pvalue = 8 * *pvalue + buf[i] - '0';
	if (*pvalue > high) {
	    *pi += i;
#ifdef	DEBUG_NUMBER
	    printf("\nmatch_number_oct: value > high  i = %d", i);
#endif	DEBUG_NUMBER
	    return;
	}
#ifdef	DEBUG_NUMBER
	printf("\nmatch_number_oct: trying char '%c'", buf[i+1]);
#endif	DEBUG_NUMBER
    }
    if (i && (*pvalue >= low)) {
	*pi += i;
	*flags |= ret;
	*flags |= NUMBER_OCT;
#ifdef	DEBUG_NUMBER
	printf("\nmatch_number_oct: matched num 0%o", *pvalue);
#endif	DEBUG_NUMBER
	return;
    }
#ifdef	DEBUG_NUMBER
    if (!i) {
	printf("\nmatch_number_oct: matched 0 numbers");
    } else {
	printf("\nmatch_number_oct: value < low");
    }
#endif	DEBUG_NUMBER
    return;
}

static void match_number_dec (char *buf, uint low, uint high,
			      uint *pi, uint *pvalue, uint *flags)
{
    uint i = 0;

    buf += *pi;
    *pvalue = 0;
    *flags &= ~NUMBER_DEC;

#ifdef	DEBUG_NUMBER
    printf("\nmatch_number_dec: Entering");
    printf("\nmatch_number_dec: buf = '%s'", buf);
    printf("\nmatch_number_dec: trying char '%c'", buf[i]);
#endif	DEBUG_NUMBER
    for (i=0; isdigit(buf[i]); i++) {
	if ((*pvalue > (ULONG_MAX / 10)) ||
	    ((*pvalue == (ULONG_MAX / 10)) &&
	     ((ulong)(buf[i] - '0') > (ULONG_MAX % 10)))) {
	    /* Too many chars, max out. */
	    *pi += i;
#ifdef	DEBUG_NUMBER
	    printf("\nmatch_number_dec: too many chars %d", i);
#endif	DEBUG_NUMBER
	    return;
	}
	*pvalue = *pvalue * 10 + (uint) (buf[i] - '0');
	if (*pvalue > high) {
	    *pi += i;
#ifdef	DEBUG_NUMBER
	    printf("\nmatch_number_dec: value > high  i = %d", i);
#endif	DEBUG_NUMBER
	    return;
	}
#ifdef	DEBUG_NUMBER
	printf("\nmatch_number_dec: trying char '%c'", buf[i+1]);
#endif	DEBUG_NUMBER
    }

    if (i && (*pvalue >= low)) {
	*pi += i;
	*flags |= NUMBER_DEC;
#ifdef	DEBUG_NUMBER
	printf("\nmatch_number_dec: matched number '%d", *pvalue);
#endif	DEBUG_NUMBER
	return;
    }
#ifdef	DEBUG_NUMBER
    if (!i) {
	printf("\nmatch_number_dec: matched 0 numbers");
    } else {
	printf("\nmatch_number_dec: value < low");
    }
#endif	DEBUG_NUMBER
    return;
}

boolean match_number (parseinfo *csb, uint low, uint high, uint *pi,
		      uint *pvalue, uint flags, const char *long_help)
{
    char *buf;
    uint hex_num, oct_num, dec_num;
    uint hex_pi, oct_pi, dec_pi;
    char short_help[SHORTHELP_LEN];
    boolean matched;

    buf = csb->line + csb->line_index;

    /*
     * Construct short help message, in case we need it
     */
    general_number_short_help_http(&flags, low, high, short_help,
				   (csb->flags & CONFIG_HTTP));

    /* Do initial, long help check */
    csb->line_index += *pi;		/* Needed for help_check */
    if ((flags & NUMBER_HELP_CHECK)  && help_check(csb)) {
	if (csb->flags & CONFIG_HTTP) {
	    parser_http_form(csb, PARSER_HTTP_FORM_NUMBER,
			     short_help, long_help);
	} else {
	    save_help_long(csb, short_help, long_help);
	}
	csb->line_index -= *pi;
	return(FALSE);
    }
    csb->line_index -= *pi;

    hex_pi = oct_pi = dec_pi = *pi;

#ifdef	DEBUG_NUMBER
    printf("\nmatch_number: entering");
    printf("\nmatch_number: buf = '%s'", buf);
    printf("\nmatch_number: buf = 0x%x  pi = %d  buf[0] = '%c'", buf, *pi, buf[0]);
#endif	DEBUG_NUMBER
    if (flags & NUMBER_HEX) {
	match_number_hex(buf, low, high, &hex_pi, &hex_num, &flags);
    }
    if (flags & NUMBER_OCT) {
	match_number_oct(buf, low, high, &oct_pi, &oct_num, &flags);
    }
    if (flags & NUMBER_DEC) {
	match_number_dec(buf, low, high, &dec_pi, &dec_num, &flags);
    }

#define	MATCH_NUMBER_SET(val, index, ret)\
    *pvalue = val;\
    *pi = index;\
    matched = ret

#define	MAX3(n1, n2, n3)\
    ((n1 > n2) ? ((n1 > n3) ? n1 : n3) : ((n2 > n3) ? n2 : n3))

#ifdef	DEBUG_NUMBER
    printf("\nmatch_number: Dec matched %d chars", dec_pi);
    printf("\nmatch_number: Oct matched %d chars", oct_pi);
    printf("\nmatch_number: Hex matched %d chars", hex_pi);
#endif	DEBUG_NUMBER

    switch (flags & (NUMBER_HEX|NUMBER_OCT|NUMBER_DEC)) {
      default:
      case 0:	/* No Matches */
#ifdef	DEBUG_NUMBER
	printf("\nmatch_number: No matches");
#endif	DEBUG_NUMBER
	MATCH_NUMBER_SET(0, MAX3(hex_pi, oct_pi, dec_pi), FALSE);
	break;

      case NUMBER_HEX:	/* Hex Only */
#ifdef	DEBUG_NUMBER
	printf("\nmatch_number: Matched hex");
	printf("\nmatch_number: Returning hex");
#endif	DEBUG_NUMBER
	MATCH_NUMBER_SET(hex_num, hex_pi, TRUE);
	break;

      case NUMBER_OCT:	/* Octal Only */
#ifdef	DEBUG_NUMBER
	printf("\nmatch_number: Matched oct");
	printf("\nmatch_number: Returning oct");
#endif	DEBUG_NUMBER
	MATCH_NUMBER_SET(oct_num, oct_pi, TRUE);
	break;

      case NUMBER_DEC:	/* Decimal Only */
#ifdef	DEBUG_NUMBER
	printf("\nmatch_number: Matched decimal");
	printf("\nmatch_number: Returning decimal");
#endif	DEBUG_NUMBER
	MATCH_NUMBER_SET(dec_num, dec_pi, TRUE);
	break;

      case NUMBER_HEX | NUMBER_OCT:	/* Hex and Octal */
#ifdef	DEBUG_NUMBER
	printf("\nmatch_number: Matched hex and octal");
#endif	DEBUG_NUMBER
	if (flags & HEX_ZEROX_MATCHED) {
	    /* This case is impossible unless whitespace is optional */
#ifdef	DEBUG_NUMBER
	    printf("\nmatch_number: Returning hex");
#endif	DEBUG_NUMBER
	    MATCH_NUMBER_SET(hex_num, hex_pi, TRUE);
	} else if (flags & OCT_ZERO_MATCHED) {
#ifdef	DEBUG_NUMBER
	    printf("\nmatch_number: Returning oct");
#endif	DEBUG_NUMBER
	    MATCH_NUMBER_SET(oct_num, oct_pi, TRUE);
	} else {
	    /*
	     * We matched a hex and octal, but didn't get a '0' or '0x',
	     * so we have no idea which one it could be, so I'm going
	     * to guess an Octal.  If anyone wants to change this,
	     * go right ahead.
	 */	
#ifdef	DEBUG_NUMBER
	    printf("\nmatch_number: Returning oct");
#endif	DEBUG_NUMBER
	    MATCH_NUMBER_SET(oct_num, oct_pi, TRUE);
	}
	break;

      case NUMBER_HEX | NUMBER_DEC:	/* Hex and Decimal */
#ifdef	DEBUG_NUMBER
	printf("\nmatch_number: Matched hex and decimal");
#endif	DEBUG_NUMBER
	if (flags & HEX_ZEROX_MATCHED) {
#ifdef	DEBUG_NUMBER
	    printf("\nmatch_number: Returning hex");
#endif	DEBUG_NUMBER
	    MATCH_NUMBER_SET(hex_num, hex_pi, TRUE);
	} else {
	    /*
	     * Pretty simple.
	     */
#ifdef	DEBUG_NUMBER
	    printf("\nmatch_number: Returning decimal");
#endif	DEBUG_NUMBER
	    MATCH_NUMBER_SET(dec_num, dec_pi, TRUE);
	}
	break;

      case NUMBER_OCT | NUMBER_DEC:	/* Octal and Decimal */
#ifdef	DEBUG_NUMBER
	printf("\nmatch_number: Matched octal and decimal");
#endif	DEBUG_NUMBER
	if (flags & OCT_ZERO_MATCHED) {
#ifdef	DEBUG_NUMBER
	    printf("\nmatch_number: Returning oct");
#endif	DEBUG_NUMBER
	    MATCH_NUMBER_SET(oct_num, oct_pi, TRUE);
	} else {
	    /*
	     * Straightforward
	     */	
#ifdef	DEBUG_NUMBER
	    printf("\nmatch_number: Returning decimal");
#endif	DEBUG_NUMBER
	    MATCH_NUMBER_SET(dec_num, dec_pi, TRUE);
	}
	break;

      case NUMBER_HEX | NUMBER_OCT | NUMBER_DEC:	/* All three */
#ifdef	DEBUG_NUMBER
	printf("\nmatch_number: Matched hex, octal, and decimal");
#endif	DEBUG_NUMBER
	if (flags & HEX_ZEROX_MATCHED) {
#ifdef	DEBUG_NUMBER
	    printf("\nmatch_number: Returning hex");
#endif	DEBUG_NUMBER
	    MATCH_NUMBER_SET(hex_num, hex_pi, TRUE);
	} else if (flags & OCT_ZERO_MATCHED) {
#ifdef	DEBUG_NUMBER
	    printf("\nmatch_number: Returning oct");
#endif	DEBUG_NUMBER
	    MATCH_NUMBER_SET(oct_num, oct_pi, TRUE);
	} else {
#ifdef	DEBUG_NUMBER
	    printf("\nmatch_number: Returning decimal");
#endif	DEBUG_NUMBER
	    MATCH_NUMBER_SET(dec_num, dec_pi, TRUE);
	}
	break;
    }

    if (matched) {
	if (((flags & NUMBER_WS_OK) &&
	     match_whitespace2(buf, pi, csb->in_help)) ||
	    (flags & NUMBER_NO_WS_OK)) {
#ifdef	DEBUG_NUMBER
	    printf("\nmatch_number: Matched whitespace");
#endif	DEBUG_NUMBER
	    save_ambig_uint(csb, short_help, *pvalue);
	    return(TRUE);
	} else {
	    /*
	     * We didn't match whitespace, so throw the matches
	     * out the window and return the max number of 
	     * characters matched
	     */
	    *pi = MAX3(hex_pi, oct_pi, dec_pi);
#ifdef	DEBUG_NUMBER
	    printf("\nmatch_number: Didn't match whitespace");
#endif	DEBUG_NUMBER
	}
    }
    if (flags & NUMBER_HELP_CHECK) {
	help_or_error(csb, *pi, short_help, NO_COMMAND_COMPLETION);
    } else {
	csb->line_index += *pi;
	set_error_index(csb);
	csb->line_index -= *pi;
    }
    return(FALSE);
}
    
void generic_addr_noflag_action (parseinfo *csb, transition *mine,
				 addrparse_nf_struct * const arg)
{
    addrparse_struct out_arg = {arg->offset, arg->help, arg->type, 0};

    generic_addr_action(csb, mine, &out_arg);
}

static boolean match_general_addr (parseinfo *csb, int *pi, void *addrp,
				   uint addr_type, uint flag,
				   const char *long_help)
{
    char *short_help = NULL;
    match_gen_func func = NULL;
    int i;
    idbtype *idb = csb->interface;

    for (i=0; addr_funcs[i].addr_type != ADDR_ILLEGAL; i++) {
	if (addr_type == addr_funcs[i].addr_type) {
	    short_help = addr_funcs[i].get_short_help(csb, flag, idb);
	    func = addr_funcs[i].func;
	    break;
	}
    }

    if (addr_funcs[i].addr_type == ADDR_ILLEGAL) {
	/* No sir, I don't like it */
	return(FALSE);
    }

    if (help_check(csb)) {
	if (csb->flags & CONFIG_HTTP) {
	    parser_http_form(csb, PARSER_HTTP_FORM_ADDRESS,
			     short_help, long_help);
	} else {
	    save_help_long(csb, short_help, long_help);
	}
	return(FALSE);
    }

    if ((*func)(csb, &csb->line[csb->line_index],pi,addrp,
		csb->in_help, flag, idb)) {
	i = *pi;
	if (match_whitespace2(&csb->line[csb->line_index], pi, csb->in_help)) {
	    save_ambig_nstring(csb, short_help,
			       &(csb->line[csb->line_index]), i);
	    return(TRUE);
	}
    }
    ((addrtype *)addrp)->type = 0;
    ((addrtype *)addrp)->length = 0;
    if (!(flag & ADDR_NO_SHORT_HELP)) {
	help_or_error(csb, *pi, short_help, NO_COMMAND_COMPLETION);
    }
    return(FALSE);
}



void generic_addr_action (parseinfo *csb, transition *mine,
			  addrparse_struct * const arg)
{
    void *addrp;
    int i;
    
    push_node(csb, mine->alternate);

    INVALID_NVGEN_CHECK;

    addrp = CSBVAR(csb, arg->offset, void);

    i = 0;
    if (match_general_addr(csb, &i, addrp, arg->type, arg->flag, arg->help)) {
	csb->line_index += i;
	push_node(csb, mine->accept);
	csb->line_index -= i;

    }
}

/* This function is used to match protocol specs against actual protocols.
 * The first argument (code) points to a two-character code to match.
 * The second argument (link_codes) points to a NUL-terminated string
 *   of codes for a particular protocol.
 * Returns TRUE if code was found in link_codes
 *         FALSE if we reached the end of link_codes without a match
 */

static boolean match_one_spec (const char *code, const char *link_codes)
{
    const char *next_code;

    for (next_code = link_codes; *next_code != '\0'; next_code += 2) {
	if (next_code[0] == code[0] && next_code[1] == code[1]) {
	    return(TRUE);
	}
    }
    return(FALSE);
}

/* This function is used to match a complete protocol spec against a
 * single link protocol.  It goes through the spec one code at a time,
 * attempting to match each against the protocol's list of matching codes.
 * It also handles the special codes PMATCH_ALL, PMATCH_INCLUDE,
 * and PMATCH_EXCLUDE.
 *
 * The last matching code is the one that governs.  I tried this
 * with the first matching code, but then you had to write things
 * like PMATCH_EXCLUDE PMATCH_CONS PMATCH_INCLUDE PMATCH_DGRAM
 * instead of just PMATCH_DGRAM PMATCH_EXCLUDE PMATCH_CONS
 */

static boolean match_link (const char *spec, const char *link_codes)
{
    const char *next_spec;
    boolean result = FALSE;	/* default is to exclude non-matches... */
    boolean including = TRUE;	/* ...and include matches */

    for (next_spec = spec; *next_spec != '\0'; next_spec += 2) {
	if (match_one_spec(next_spec, link_codes))
	    result = including;
	else if (match_one_spec(next_spec, PMATCH_ALL))
	    result = including;
	else if (match_one_spec(next_spec, PMATCH_INCLUDE))
	    including = TRUE;
	else if (match_one_spec(next_spec, PMATCH_EXCLUDE))
	    including = FALSE;
    }
    return(result);
}

/* This function attempts to match a protocol keyword, determined by the
 * spec passed in as the fourth argument.  Help is handled here.
 */

static int match_protocol (parseinfo *csb, int *pi, uint *link_type,
			   const char *match_spec, uint *addr_type)
{
    link_name *link;
    int j;
    int matched = 0;
    char *word;
    int i;
    int matched_pi;

    if (link_names == NULL) {
	return(matched);
    }
    i = *pi;
    matched_pi = *pi;
    for (j=0; link_names[j].link_type != LINK_ILLEGAL; j++) {
	link = &link_names[j];
	if (match_link(match_spec, link->codes)) {
	    *pi = i;
	    if (match_partial_keyword(csb, link->name, pi, link->help,
				      link->min_match,
				      link->help ?
				      KEYWORD_HELP_CHECK | KEYWORD_WS_OK :
				      KEYWORD_WS_OK)) {
		matched++;
		matched_pi = *pi;
		*link_type = link->link_type;
		*addr_type = link->addr_type;
		word = csb->line + csb->line_index;

		if ((strncasecmp(link->name, word, strlen(link->name)) == 0)) {
		    return(TRUE);
		}
	    }
	}
    }

    /* None of the standard names matched?  If we're not doing help,
     * go ahead and default to the old syntax of an IP address
     * without preceeding keyword.
     */

    if (matched == 0) {
	if (csb->in_help) {
	    *link_type = LINK_ILLEGAL;
	}
	return(matched);
    }

    /* This code doesn't behave exactly as you might expect.  Instead
     * of trying to parse to the end of the line for each match,
     * we punt and signal an ambiguous condition right here if
     * the protocol name can't be uniquely resolved.  The code
     * is a lot simpler than it otherwise would be, but sometimes we
     * might signal an ambiguity on something that could be resolved
     * through look-ahead.
     */

    if (matched > 1) {
	*link_type = LINK_ILLEGAL;
	increment_multiple_funcs(csb, "match_protocol 1");
	increment_multiple_funcs(csb, "match_protocol 2");
	return(matched);
    }
    *pi = matched_pi;
    return(matched);
}    

void protoname_action (parseinfo *csb, transition *mine,
		       protoname_struct * const arg)
{
    int i;
    uint *link_type;
    uint addr_type;

    push_node(csb, mine->alternate);

    INVALID_NVGEN_CHECK;

    link_type = CSBVAR(csb, arg->variable, uint);

    i = 0;
    if (match_protocol(csb, &i, link_type, arg->match_spec, &addr_type) == 1) {
	csb->line_index += i;
	push_node(csb, mine->accept);
	csb->line_index -= i;
    }
}

void protoaddr_action (parseinfo *csb, transition *mine,
		       protoaddr_struct * const arg)
{
    uint *link_type;
    void *addrp;
    int i, j;
    uint addr_type;

    push_node(csb, mine->alternate);

    INVALID_NVGEN_CHECK;

    link_type = CSBVAR(csb, arg->num_offset, uint);
    addrp = CSBVAR(csb, arg->addr_offset, void);

    i = 0;
    if ((j = match_protocol(csb, &i, link_type, arg->match_spec,
			    &addr_type)) > 1) {
	return;
    }
    if (j == 0) {
	if (match_link(arg->match_spec, PMATCH_IP)) {
	    *link_type = LINK_IP;
	    addr_type = ADDR_IP;
	} else {
	    /* didn't match anything and can't default to IP so it's an
	     * error, return */
	    return;
	}
    }

    csb->line_index += i;
    j = 0;
    if ((addr_type == ADDR_ILLEGAL) ||
	match_general_addr(csb, &j, addrp, addr_type, ADDR_PROTO_ADDR, 
			   "Protocol specific address")) {
	csb->line_index += j;
	push_node(csb, mine->accept);
	csb->line_index -= j;
    }
    csb->line_index -= i;
}

void multiple_funcs_action (parseinfo *csb, transition *mine,
			    test_struct * const arg)
{
    push_node(csb, mine->alternate);
    if (test_multiple_funcs(csb, '=', 0)) {
	push_node(csb, arg->equal);
    } else {
	push_node(csb, arg->notequal);
    }
}

void save_ambig_string (parseinfo *csb, const char *help, const char *str)
{
    save_ambig_string_flag(csb, help, str, TRUE);
}

static void save_ambig_nstring (parseinfo *csb, const char *help,
				const char *str, uint size)
{
    char *tmp = NULL;
    uint length;

    if (csb->flags & COMMAND_AUTHORIZE) {
	length =  strlen(str);
	if (length > size) {
	    length = size;
	}
	tmp = malloc(length + 1);
	if (tmp) {
	    sstrncpy(tmp, str, length+1);
	}
    } else {
	tmp = NULL;
    }

    save_ambig_string_flag(csb, help, tmp, TRUE);
    if (tmp) {
	free(tmp);
    }
}

static void save_ambig_int (parseinfo *csb, const char *help, int num)
{
    char str[12];

    sprintf(str, "%d", num);
    save_ambig_string_flag(csb, help, str, TRUE);
}

static void save_ambig_uint (parseinfo *csb, const char *help, uint num)
{
    char str[12];

    sprintf(str, "%u", num);
    save_ambig_string_flag(csb, help, str, TRUE);
}

static void save_ambig_string_flag (parseinfo *csb, const char *help,
				    const char *ambig_string, boolean space)
{
    ambig_struct *ap;
    char *str;
    uint new_length = strlen(help) + (space ? 1 : 0);
    uint old_length;

    if (csb->command_visible) {
	ap = &csb->visible_ambig;
    } else {
	ap = &csb->hidden_ambig;
    }

    if (parser_ambig_debug) {
	buginf("\nsaving string '%s' in ambig buffer '%s'", help, ap->ambig);
    }

    old_length = strlen(ap->ambig);
    str = ap->ambig + old_length;
    if ((ap->ambig != str) && (*(str-1) == -1)) {
	/* Already marked as too big, so skip */
    } else {
	if ((new_length + old_length + 2) >= PARSEBUF) {
	    /* Too big, mark as too big and skip */
	    if ((old_length + 2) >= PARSEBUF) {
		str = ap->ambig + PARSEBUF - 2;
	    }
	    *str++ = -1;
	    *str = '\0';
	} else {
	    strcat(str, help);
	    if (space) {
		strcat(str, " ");
	    }
	}
	if (csb->flags & COMMAND_AUTHORIZE) {
	    old_length = strlen(csb->nv_command);
	    new_length = strlen(ambig_string);
	    if ((new_length + old_length + 2) < PARSEBUF) {
		strcat(csb->nv_command, ambig_string);
		if (space) {
		    strcat(csb->nv_command, " ");
		}
	    }
	}
    }
}

static void save_help_msg (parseinfo *csb, const char *help)
{
    if (csb->in_help == PARSER_HELP) {
	save_line(csb, &csb->help_save, "%s", help);
    }
}

static void save_help_short (parseinfo *csb, const char *help,
			     boolean do_command_completion)
{
    if (((csb->in_help == PARSER_HELP) ||
	 (do_command_completion == SUPPORT_COMMAND_COMPLETION)) &&
	(!reg_invoke_parser_short_help_filtered(csb, help))) {
	save_line(csb, &csb->help_save, "%s\t", help);
    }
}

void encode_str (const char *src, char *dst, uint max_len)
{
    uint i;
    char *encode;

    for (i=0; (i<max_len-3) && *src; i++, src++) {
	encode = strchr(HTTP_UNSAFE_CHARS, *src);
	if (encode) {
	    *dst++ = '%';
	    *dst++ = tohexchar(*src >> 4);
	    *dst++ = tohexchar(*src & 0x0f);
	    i += 2;
	}
	else {
	    *dst++ = *src;
	}
    }
    *dst = *src;
}

void save_help_long (parseinfo *csb, const char *short_help,
		     const char *long_help)
{
    char uri_str[HTTP_MAX_URI_LEN];
    char help_str[SHORTHELP_LEN];

    if ((csb->in_help == PARSER_HELP) &&
	(!reg_invoke_parser_long_help_filtered(csb, short_help))) {
	if (csb->flags & CONFIG_HTTP) {
	    if (parser_http_debug && (csb->flags & CONFIG_HTTP)) {
		buginf("\nHTTP: Adding keyword '%s' '%s'",
		       csb->nv_command, short_help);
	    }

            /* encode any illegal chars in the uri or short_help strings*/
	    encode_str(csb->http_state->uri, uri_str, HTTP_MAX_URI_LEN);
            encode_str(short_help, help_str, SHORTHELP_LEN);
 
	    save_line(csb, &csb->help_save,
		      "<DT><A HREF=%s/%s%s>%s</A><DD>%s\t",
		      uri_str,
		      (help_str && (help_str[0] == '/')) ? "\\" : "",
		      help_str, short_help, long_help);
	} else {
	    save_line(csb, &csb->help_save, "  %s", short_help);
	    if (long_help) {
		save_line(csb, &csb->help_save, "\t%s", long_help);
	    }
	    save_line(csb, &csb->help_save, "\n");
	}
    }
}

/*
 * match_transport - Handle matching of transport keywords
 */

static int match_transport (parseinfo *csb,
			    int *pi,
			    ulong *transport_return_mask,
			    ulong transport_ignore_mask)
{
    connvector *connv;
    int i;
    int matched = 0;
    char *word;
    int matched_pi;
    ulong matched_transport = 0;

    i = *pi;			/* Get parse index */
    matched_pi = *pi;		/* And matched parse index */

    for (connv = (connvector *)conn_vectorQ.qhead;
	 connv;
	 connv = connv->next) {

	if (connv->type & transport_ignore_mask)
	  continue;		/* Skip this if in the ignore list */

	*pi = i;

	if (match_partial_keyword(csb, connv->name, pi, connv->help,
				  0, KEYWORD_HELP_CHECK | KEYWORD_WS_OK)) {
	    matched++;		/* Count another match */
	    matched_pi = *pi;	/* Save index */
	    matched_transport = connv->type;
	    word = csb->line + csb->line_index;

	    /*
	     * If we have an exact match, and we are not doing help,
	     * get out.
	     */

	    if ((strncmp(connv->name, word, strlen(connv->name)) == 0) &&
		! help_check(csb)) {
		break;
	    }
	}
    }

    if (matched == 1) {
	*pi = matched_pi;
	*transport_return_mask |= matched_transport;
    }

    return(matched);
}

/*
 * transport_action - Action routine for TRANSPORT() macro
 */

void transport_action  (parseinfo *csb, transition *mine,
			transport_struct * const arg)
{
    ulong *transport_mask;
    ulong ignore_mask;
    int i;
    int j;

    push_node(csb, mine->alternate); /* Push the alternate transition */

    INVALID_NVGEN_CHECK;

    transport_mask = CSBVAR(csb, arg->trans_var, ulong);
    ignore_mask = 0;		/* Nothing to ignore initially */

    j = 0;			/* Initialize cumulative parse pointer */
    while (TRUE) {
	i = 0;
	if (match_transport(csb, &i, transport_mask, ignore_mask) == 1) {
	    j += i;
	    csb->line_index += i;
	    ignore_mask |= *transport_mask;
	} else
	  break;
	if (arg->oneonly)
	  break;		/* Get out if only one transport wanted */
    }

    if (j) {			/* If we got some sort of match */
	push_node(csb, mine->accept); /* Push the accept node */
	csb->line_index -= j;	/* Now fix the line index */
    }
}

static boolean match_subinterface (parseinfo *csb, int *i, uint *subif,
				   interface_struct * const arg,
				   uint match_nows_flag, char *help,
				   uint lower, uint upper,
				   iftype *ift)
{
    uint oldi;
    char buffer[32];

    oldi = *i;
    if ((arg->valid_ifs & IFTYPE_SUBIF) && ift->subinterfaces_allowed) {
	if (match_char(&csb->line[csb->line_index], i, '.')) {
	    if (match_number(csb, 0, 0xffffffff, i, subif,
			     match_nows_flag, help)) {
		if (match_whitespace2(&csb->line[csb->line_index], i,
				      csb->in_help)) {
                    csb->subif_specified = TRUE;
		    return(TRUE);
		} else {
		    general_number_short_help(&match_nows_flag, 0,
					      0xffffffff, buffer);
		    help_or_error(csb, *i, buffer,
				  NO_COMMAND_COMPLETION);
		    return(FALSE);
		}
	    }
	    return(FALSE);
	} else {
	    csb->line_index += *i;
	    if (short_help_check(csb)) {
		save_help_short(csb, ".", NO_COMMAND_COMPLETION);
	    }
	    csb->line_index -= *i;
	}
    }
    *i = oldi;
    if (match_whitespace2(&csb->line[csb->line_index],
			  i, csb->in_help)) {
	return(TRUE);
    } else {
	general_number_short_help(&match_nows_flag, lower, upper, buffer);
	help_or_error(csb, *i, buffer, NO_COMMAND_COMPLETION);
	return(FALSE);
    }
    return(FALSE);
}

static boolean idb_name_match (int fmt, parseinfo *csb, int *i,
			       uint *slot, uint match_flag, char *help)
{
    switch (fmt) {

#if defined(MAX_IF_SLOT)

    case SLOTTED_IDB_NAMES:
    case EXTENDED_SLOTTED_IDB_NAMES:
	return(match_number(csb, 0, MAX_IF_SLOT, i, slot, match_flag, help));

#endif /* MAX_IF_SLOT */

    default:
	return(FALSE);
    }
}

static boolean match_interface_unit (parseinfo *csb, iftype *ift, int *i,
				     uint *unit, uint *slot, uint *vc,
				     uint *subif,
				     interface_struct * const arg, char *help)
{
    uint match_nows_flag = (NUMBER_HEX | HEX_ZEROX_OK | NUMBER_OCT |
			    OCT_ZERO_OK | NUMBER_DEC |
			    NUMBER_NO_WS_OK | NUMBER_HELP_CHECK);
    boolean dynamic_interface = FALSE, match = FALSE;
    int fmt;
    uint pa_number = 0;

    *slot = 0;
    *unit = 0;
    *subif = 0;

    fmt = idb_name_format();
    if (fmt != NONSLOTTED_IDB_NAMES) {
	dynamic_interface = ift->dynamic_interface;
    } else {
	dynamic_interface = TRUE;
    }

    if (dynamic_interface) {
	if (!match_number(csb, ift->base_unit, ift->max_unit, i, unit,
                             match_nows_flag, help)) {
	    return(FALSE);
	}
    } else {
    	match = idb_name_match(fmt, csb, i, slot, match_nows_flag, help);
    }

    if (fmt == EXTENDED_SLOTTED_IDB_NAMES) {
	if (dynamic_interface || match) {
            if (dynamic_interface ||
            match_char(&csb->line[csb->line_index], i, '/')) {

		if (!dynamic_interface && match_subcard_in_slot(*slot)) {
                    if (match_number(csb, PA_BAY_0, PA_BAY_1, i,
                        &pa_number, match_nows_flag, PA_HELP_STRING)) {

			if (!match_char(&csb->line[csb->line_index], i, '/')) {
			    help_or_error(csb, *i, "/", NO_COMMAND_COMPLETION);
			    return(FALSE);
 
			}
 
                        if (!match_number(csb, ift->base_unit,
                                              ift->max_unit,
                                              i, unit, match_nows_flag,
                                              help)) {
                                return(FALSE);
                        }
 
                        /*
                         * Let's generate the right unit number.
                         */
                        *unit = (pa_number * PA_VIRTUAL_SLOT_BOUNDARY)
			    + *unit;
 
		    } else {
			return(FALSE);
		    }
		} else if (!dynamic_interface) {
		    if (!match_number(csb, ift->base_unit, ift->max_unit, i, 
			unit, match_nows_flag, help)) {
			return(FALSE);
		    }
		}
			
                /* get the optional VC number */
                if (csb->line[csb->line_index + *i] == ':') {
		    (*i)++;
                    if (match_number(csb, 0, ift->max_vc, i, vc,
                                 match_nows_flag, help)) {
                        return(match_subinterface(csb, i, subif, arg,
                                             match_nows_flag, help,
                                             0, ift->max_vc, ift));
                    } else {
                        return(FALSE);
                    }
                } else {
                    csb->line_index += *i;
                    if (short_help_check(csb)) {
                        save_help_short(csb, ":", NO_COMMAND_COMPLETION);
                    }
                    csb->line_index -= *i;
                }
                return(match_subinterface(csb, i, subif, arg,
                                          match_nows_flag, help,
                                          ift->base_unit, ift->max_unit, ift));
               }
                return(FALSE);
            }
            help_or_error(csb, *i, "/", NO_COMMAND_COMPLETION);
            return(FALSE);
    } else {
    	if (dynamic_interface || match) {
	    if (dynamic_interface ||
	    match_char(&csb->line[csb->line_index], i, '/')) {

	    	if (dynamic_interface ||
		    match_number(csb, ift->base_unit, ift->max_unit, i, unit,
			     match_nows_flag, help)) { 

		    /* get the optional VC number */
		    if (csb->line[csb->line_index + *i] == ':') {
		    	(*i)++;
		    	if (match_number(csb, 0, ift->max_vc, i, vc,
				     match_nows_flag, help)) {
			    return(match_subinterface(csb, i, subif, arg,
						  match_nows_flag, help,
						  0, ift->max_vc, ift));
		    	} else {
			    return(FALSE);
		        }
		    } else {
		    	csb->line_index += *i;
		    	if (short_help_check(csb)) {
			    save_help_short(csb, ":", NO_COMMAND_COMPLETION);
		        }
		        csb->line_index -= *i;
		    }
		    return(match_subinterface(csb, i, subif, arg, 
					  match_nows_flag, help,
					  ift->base_unit, ift->max_unit, ift));
	        }
	        return(FALSE);
	    }
	    help_or_error(csb, *i, "/", NO_COMMAND_COMPLETION);
	    return(FALSE);
	}
	return(FALSE);
    }
}

static boolean match_subcard_in_slot (uint slot)
{
    hwidbtype *ifp;

    /*
     * Use the first idb as the representative for this slot.
     */
    FOR_ALL_HWIDBS(ifp) {
	if (ifp->slot == slot) {
	    if (is_vip(ifp)) {
		return(TRUE);
            }
	    else {
		return(FALSE);
            }
	}
    }
    return(FALSE);
	    
}

void (*reload_command_ptr)(parseinfo *) = NULL;
void (*enable_command_ptr)(parseinfo *) = NULL;

static void parser_bail_out (parseinfo *csb, void (*func)(parseinfo *),
			     const char *func_name)
{
    if (((*enable_command_ptr != NULL) && (func == *enable_command_ptr)) ||
	((*reload_command_ptr != NULL) && (func == *reload_command_ptr))) {
	func(csb);
    } else {
	show_proc_memory(csb);
	show_memory_allocfail(csb);
	show_memory_by_alloc_pc();
    }
}


#ifdef DEBUG
void print_tokenQ (parseinfo *csb)
{
    parsenode *tos;
    for (tos = (parsenode *) csb->tokenQ->qhead; tos; tos = tos->next) {
	printf("\nNode:\n");
	printf("\tcommand '%s'\n", tos->nv_command);
	printf("\tOBJ(int,1) %d OBJ(idb,1) 0x%x '%s'\n",
	       tos->val1, tos->idb1, tos->idb1 ? tos->idb1->namestring : "");
	if (tos->val1 == PDB_OSI) {
	    printf("\tpdb1 0x%x '%s'\n", tos->pdb1,
		   tos->pdb1 ? ((clns_pdbtype *) tos->pdb1)->name : "");
	} else {
	    printf("\tpdb1 0x%x '%s'\n", tos->pdb1,
		   tos->pdb1 ? ((pdbtype *) tos->pdb1)->name : "");
	}
    }
}
#endif	/* DEBUG */

void priv_action (parseinfo *csb, transition *mine, priv_struct * const arg)
{
    push_node(csb, mine->alternate);
    if (priv_check(csb, arg->priv)) {
	if (csb->priv_set) {
	    priv_push(csb, arg->priv);
	}
	csb->last_priv = arg->priv->cur_priv;
	if (csb->last_priv > csb->highest_priv) {
	    csb->highest_priv = csb->last_priv;
	}
	push_node(csb, mine->accept);
    } else {
	push_node(csb, arg->fail);
    }
}

void mode_action (parseinfo *csb, transition *mine, mode_struct * const arg)
{
    parser_mode **pmp = CSBVAR(csb, arg->offset, parser_mode *);
    int i = 0;

    push_node(csb, mine->alternate);

    if (csb->nvgen) {
	nv_mode(csb, mine, arg);
    } else {
	if (match_mode(csb, &i, pmp, arg->flags)) {
	    csb->line_index += i;
	    push_node(csb, mine->accept);
	    csb->line_index -= i;
	}
    }
}

void month_action (parseinfo *csb, transition *mine,
		    month_struct * const arg)
{
    int *var = CSBVAR(csb, arg->offset, int);
    int i;
    static const char *short_help = "MONTH";

    push_node(csb, mine->alternate);

    INVALID_NVGEN_CHECK;

    if (help_check(csb)) {
	save_help_long(csb, short_help, arg->help);
	return;
    }

    for (i=0; i < MONTHS_IN_YEAR; i++) {
	int j = 0;
	if (match_partial_keyword(csb, long_month_name[i], &j, NULL, 3, 
				  KEYWORD_WS_OK)) {
	    csb->line_index += j;
	    *var = i + 1;
	    push_node(csb, mine->accept);
	    csb->line_index -= j;
	    break;
	}
    }
}


void day_action (parseinfo *csb, transition *mine,
		    day_struct * const arg)
{
    int *var = CSBVAR(csb, arg->offset, int);
    int i;
    static const char *short_help = "DAY";

    push_node(csb, mine->alternate);

    INVALID_NVGEN_CHECK;

    if (help_check(csb)) {
	save_help_long(csb, short_help, arg->help);
	return;
    }

    for (i=0; i < DAYS_IN_WEEK; i++) {
	int j = 0;
	if (match_partial_keyword(csb, long_day_name[i], &j, NULL, 3, 
				  KEYWORD_WS_OK)) {
	    csb->line_index += j;
	    *var = i;
	    push_node(csb, mine->accept);
	    csb->line_index -= j;
	    break;
	}
    }
}

void timesec_action (parseinfo *csb, transition *mine, timesec_struct *arg)
{
    uint *var1 = CSBVAR(csb, arg->var1, uint);
    uint *var2 = CSBVAR(csb, arg->var2, uint);
    uint *var3;
    char *short_help;
    uint i;
    static uint flags = NUMBER_DEC | NUMBER_NO_WS_OK;

    push_node(csb, mine->alternate);

    INVALID_NVGEN_CHECK;

    if (arg->var3 == MAXUINT) {
	short_help = "hh:mm";
	var3 = 0;
    } else {
	short_help = "hh:mm:ss";
	var3 = CSBVAR(csb, arg->var3, uint);
    }

    if (help_check(csb)) {
	save_help_long(csb, short_help, arg->help);
    }

    i = 0;
    if (match_number(csb, 0, 23, &i, var1, flags, NULL) &&
	match_char(&csb->line[csb->line_index], &i, ':') &&
	match_number(csb, 0, 59, &i, var2, flags, NULL) &&
	((arg->var3 == MAXUINT) ||
	 (match_char(&csb->line[csb->line_index], &i, ':') &&
	  match_number(csb, 0, 59, &i, var3, flags, NULL))) &&
	match_whitespace2(&csb->line[csb->line_index], &i, csb->in_help)) {
	csb->line_index += i;
	push_node(csb, mine->accept);
	csb->line_index -= i;
	return;
    }

    if (i) {
	help_or_error(csb, i, short_help, NO_COMMAND_COMPLETION);
    }
}

/*
 * bgp_community_action
 * 
 * action routine to accept BGP communities in aa:nn format
 */
void bgp_community_action (parseinfo *csb, transition *mine, 
			   bgp_community_struct *arg)
{
    uint *var1 = CSBVAR(csb, arg->var1, uint);
    uint *var2 = CSBVAR(csb, arg->var2, uint);
    char *short_help;
    uint i;
    static uint flags = NUMBER_DEC | NUMBER_NO_WS_OK;

    push_node(csb, mine->alternate);

    INVALID_NVGEN_CHECK;

    short_help = "aa:nn";

    if (help_check(csb)) {
	save_help_long(csb, short_help, arg->help);
    }

    i = 0;
    if (match_number(csb, 0, MAXUSHORT, &i, var1, flags, NULL) &&
	match_char(&csb->line[csb->line_index], &i, ':') &&
	match_number(csb, 0, MAXUSHORT, &i, var2, flags, NULL) &&
	match_whitespace2(&csb->line[csb->line_index], &i, csb->in_help)) {
	csb->line_index += i;
	push_node(csb, mine->accept);
	csb->line_index -= i;
	return;
    }

    if (i) {
	help_or_error(csb, i, short_help, NO_COMMAND_COMPLETION);
    }
}

void comment_action (parseinfo *csb, transition *mine)
{
    push_node(csb, mine->accept);
    if (csb->nvgen && !csb->priv_set) {
	nv_write(TRUE, "!");
    }
}

void testexpr_action (parseinfo *csb, transition *mine, testexpr_struct * const arg)
{
    /* always push the alternate */
    push_node(csb, mine->alternate);

    if (csb->priv_set || (csb->flags & COMMAND_FUNCTION)) {
	/*
	 * Parser state may not be what test expression expects,
	 * so avoid testing the expression.
	 */
	push_node(csb, mine->accept);
	push_node(csb, arg->false);
    } else {
	if (arg->func(csb, mine)) {
	    push_node(csb, mine->accept);
	} else {
	    push_node(csb, arg->false);
	}
    }
}

/* APPN Network names */
/*
 * match_type_a_char
 */
static boolean match_type_a_char (char c)
{
    if (((c >= 'A') && (c <= 'Z')) ||
        ((c >= '0') && (c <= '9')) ||
        (c == '#') || (c == '$') || (c == '@'))
       return(TRUE);
    else
       return(FALSE);
}

static boolean match_pcid (char c)
{
    if (((c >= 'A') && (c <= 'F')) ||
        ((c >= '0') && (c <= '9')))
       return(TRUE);
    else
       return(FALSE);
}

/*
 * Parse and check for an APPN Fully-Qualified Type-A String Name
 */
void appn_fqname_action (parseinfo *csb, transition *mine,
		    string_struct * const arg)
{
    char *from, *to;
    uint length, i;
    static const char *short_help = "NETID.NAME";

    push_node(csb, mine->alternate);

    INVALID_NVGEN_CHECK;

    if (help_check(csb)) {
	save_help_long(csb, short_help, arg->help);
	return;
    }

    from = csb->line + csb->line_index;
    to   = CSBVAR(csb, arg->offset, char);
    length = 0;

    /* copy NETID */
    if (isdigit(*from)) {
        help_or_error(csb, length, short_help, NO_COMMAND_COMPLETION);   
        return;
    }
    for (i = 0; i < 8; i++) {
        if (islower(*from)) 
           *from = toupper(*from);
        if (!match_type_a_char(*from)) {
        	break;
        }
        *to++ = *from++;
	}
    length = i;

    /* copy PERIOD */   
    if (!length || (*from != '.')) {
       	help_or_error(csb, length, short_help, NO_COMMAND_COMPLETION);
        return;
    } else {
        length++;
        *to++ = *from++;
    }

    /* copy NAME */
    if (isdigit(*from)) {
        help_or_error(csb, length, short_help, NO_COMMAND_COMPLETION);   
        return;
    }
    for (i = 0; i < 8; i++) {
        if (islower(*from)) 
           *from = toupper(*from);
        if (!match_type_a_char(*from)) {
        	break;
        }
        *to++ = *from++;
	}
    if (!i) {
       	help_or_error(csb, length + i, short_help, NO_COMMAND_COMPLETION);
        return;
    } else {
        length += i;
        *to = '\0';
    }

    /* check for end of name */
    if (match_whitespace2(csb->line + csb->line_index, &length, csb->in_help)) {
        save_ambig_string(csb, short_help, CSBVAR(csb, arg->offset, char));
        csb->line_index += length;
        push_node(csb, mine->accept);
        csb->line_index -= length;
    } else {
        help_or_error(csb, length, short_help, NO_COMMAND_COMPLETION);
        return;
    }
}
/*
 * Parse and check for an APPN Fully-Qualified Type-A String Name or "*" character.
 */
void appn_name_star_action (parseinfo *csb, transition *mine,
		    string_struct * const arg)
{
    char *from, *to;
    boolean splat_seen = FALSE, dot_seen = FALSE;
    uint length, i;
    static const char short_help[] = "NETID.NAME";
    static const char short_help2[] = "NAME";

    push_node(csb, mine->alternate);

    INVALID_NVGEN_CHECK;

    if (help_check(csb)) {
        if (GETOBJ(int,1) == APPN_SHOW_ISR)
    	   save_help_long(csb, short_help2, arg->help);
        else
    	   save_help_long(csb, short_help, arg->help);
	return;
    }

    from = csb->line + csb->line_index;
    to   = CSBVAR(csb, arg->offset, char);
    length = 0;

    /* copy NETID */
    if (isdigit(*from)) {
        if (GETOBJ(int,1) == APPN_SHOW_ISR)      
           help_or_error(csb, length, short_help2, NO_COMMAND_COMPLETION);   
        else
           help_or_error(csb, length, short_help, NO_COMMAND_COMPLETION);   
        return;
    }


    for (i = 0; i < 8; i++) {
      if (islower(*from)) 
	*from = toupper(*from);
      if (!match_type_a_char(*from)) 
	break;
      *to++ = *from++;
    }
       
    if (*from == '*') {
      splat_seen = TRUE;
      i++;
      *to++ = *from++;
    }

    if (splat_seen == TRUE) {
      for (; i < 8; i++) {
        if (islower(*from)) 
	  *from = toupper(*from);
        if (!match_type_a_char(*from)) 
	  break;
	*to++ = *from++;
      }
    }

    length = i;
    
    if (*from == '.') 
      dot_seen = TRUE;
   

  
    /* We've gotten to the dot or end of string */
    if (i == 0) {
      if (GETOBJ(int,1) == APPN_SHOW_ISR) 
	help_or_error(csb, length + i, short_help2, NO_COMMAND_COMPLETION);
      else
	help_or_error(csb, length + i, short_help, NO_COMMAND_COMPLETION);
      return;
    }

    if ((splat_seen && !dot_seen) ||
	(GETOBJ(int,1) == APPN_SHOW_ISR)) {
      /* If we are in ISR show mode, we will allow non-qualified name
	 to be input.  But we will also allow a FQname.  So this case
	 covers length == 0, ie non-qualified name. */
      /* check for end of name */
      if (match_whitespace2(csb->line + csb->line_index, &length, csb->in_help)) {
	save_ambig_string(csb, short_help2, CSBVAR(csb, arg->offset, char));
	csb->line_index += length;
	push_node(csb, mine->accept);
	csb->line_index -= length;
      } else {
	help_or_error(csb, length, short_help2, NO_COMMAND_COMPLETION);
	return;
      }
    }
    
    if (dot_seen) {	
      length++;
      *to++ = *from++;
    }

    /* copy NAME */
    if (isdigit(*from)) {
      if (GETOBJ(int,1) == APPN_SHOW_ISR) 
	help_or_error(csb, length, short_help2, NO_COMMAND_COMPLETION);   
      else
	help_or_error(csb, length, short_help, NO_COMMAND_COMPLETION);   
      return;
    }
    
    for (i = 0; i < 8; i++) {
      if (islower(*from)) 
	*from = toupper(*from);
      if (!match_type_a_char(*from)) 
	break;
      *to++ = *from++;
    }

    if (*from == '*') {
      splat_seen = TRUE;
      *to++ = *from++;
      i++;
    }

    if (splat_seen == TRUE) {
      for (; i < 8; i++) {
        if (islower(*from)) 
	  *from = toupper(*from);
        if (!match_type_a_char(*from)) 
	  break;
	*to++ = *from++;
      }
    }

    if (i == 0) {
      if (GETOBJ(int,1) == APPN_SHOW_ISR) 
	help_or_error(csb, length + i, short_help2, NO_COMMAND_COMPLETION);
      else
	help_or_error(csb, length + i, short_help, NO_COMMAND_COMPLETION);
      return;
    } else {
      length += i;
      *to = '\0';
    }

    /* check for end of name */
    if (match_whitespace2(csb->line + csb->line_index, &length, csb->in_help)) {
      if (GETOBJ(int,1) == APPN_SHOW_ISR) 
	save_ambig_string(csb, short_help2, CSBVAR(csb, arg->offset, char));
      else
	save_ambig_string(csb, short_help, CSBVAR(csb, arg->offset, char));
      csb->line_index += length;
      push_node(csb, mine->accept);
      csb->line_index -= length;
    } else {
      if (GETOBJ(int,1) == APPN_SHOW_ISR) 
	help_or_error(csb, length, short_help2, NO_COMMAND_COMPLETION);
      else
	help_or_error(csb, length, short_help, NO_COMMAND_COMPLETION);
      return;
    }
}

/*
 * Parse and check for an APPN Type-A Character String
 */
void appn_type_a_name_action (parseinfo *csb, transition *mine,
		    string_struct * const arg)
{
    uint i;
    char *to, *from;
    static const char short_help[] = "NAME";

    push_node(csb, mine->alternate);

    INVALID_NVGEN_CHECK;

    if (help_check(csb)) {
	save_help_long(csb, short_help, arg->help);
	return;
    }

    from = csb->line + csb->line_index;
    to   = CSBVAR(csb, arg->offset, char);
    i = 0;

    /* copy NAME */
    if (isdigit(*from)) {
        help_or_error(csb, i, short_help, NO_COMMAND_COMPLETION);   
        return;
    }
    for (i = 0; i < 8; i++) {
        if (islower(*from)) 
           *from = toupper(*from);
        if (!match_type_a_char(*from)) {
        	break;
        }
        *to++ = *from++;
	}

    if (!i) {
       	help_or_error(csb, i, short_help, NO_COMMAND_COMPLETION);
        return;
    } else {
        *to = '\0';
    }

    /* check for end of name */
    if (match_whitespace2(csb->line + csb->line_index, &i, csb->in_help)) {
        save_ambig_string(csb, short_help, CSBVAR(csb, arg->offset, char));
        csb->line_index += i;
        push_node(csb, mine->accept);
        csb->line_index -= i;
    } else {
        help_or_error(csb, i, short_help, NO_COMMAND_COMPLETION);
        return;
    }
}

/*
 * Parse and check for an APPN Type-A Character String including "*" character.
 */
void appn_type_a_name_star_action (parseinfo *csb, transition *mine,
		    string_struct * const arg)
{
    uint i;
    char *to, *from;
    static const char short_help[] = "NAME";

    push_node(csb, mine->alternate);

    INVALID_NVGEN_CHECK;

    if (help_check(csb)) {
	save_help_long(csb, short_help, arg->help);
	return;
    }

    from = csb->line + csb->line_index;
    to   = CSBVAR(csb, arg->offset, char);
    i = 0;

    /* copy NAME */
    if (isdigit(*from)) {
        help_or_error(csb, i, short_help, NO_COMMAND_COMPLETION);   
        return;
    }
    for (i = 0; i < 8; i++) {
        if (islower(*from)) 
           *from = toupper(*from);
        if (!match_type_a_char(*from)) {
        	break;
        }
        *to++ = *from++;
	}

    if (*from == '*')  {
       i++;
       *to++ = *from++;
       *to = '\0';
    } else
       if (!i) {
           help_or_error(csb, i, short_help, NO_COMMAND_COMPLETION);
           return;
       } else
           *to = '\0';

    /* check for end of name */
    if (match_whitespace2(csb->line + csb->line_index, &i, csb->in_help)) {
        save_ambig_string(csb, short_help, CSBVAR(csb, arg->offset, char));
        csb->line_index += i;
        push_node(csb, mine->accept);
        csb->line_index -= i;
    } else {
        help_or_error(csb, i, short_help, NO_COMMAND_COMPLETION);
        return;
    }
}


/*
 * Parse and check for an APPN Type-A Character String including "*" character
 */
void appn_pcid_action (parseinfo *csb, transition *mine,
		    string_struct * const arg)
{
    uint i;
    char *to, *from;
    static const char short_help[] = "NAME";

    push_node(csb, mine->alternate);

    INVALID_NVGEN_CHECK;

    if (help_check(csb)) {
	save_help_long(csb, short_help, arg->help);
	return;
    }

    from = csb->line + csb->line_index;
    to   = CSBVAR(csb, arg->offset, char);
    i = 0;

    for (i = 0; i < 16; i++) {
        if (islower(*from)) 
           *from = toupper(*from);
        if (!match_pcid(*from)) {
        	break;
        }
        *to++ = *from++;
	}

    if (*from == '*')  {
       i++;
       *to++ = *from++;
       *to = '\0';
    } else 
       if (!i) {
          help_or_error(csb, i, short_help, NO_COMMAND_COMPLETION);
          return;
       } else
          *to = '\0';

    /* check for end of name */
    if (match_whitespace2(csb->line + csb->line_index, &i, csb->in_help)) {
        save_ambig_string(csb, short_help, CSBVAR(csb, arg->offset, char));
        csb->line_index += i;
        push_node(csb, mine->accept);
        csb->line_index -= i;
    } else {
        help_or_error(csb, i, short_help, NO_COMMAND_COMPLETION);
        return;
    }
}

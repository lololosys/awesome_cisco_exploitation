/* $Id: parser_actions.h,v 3.6.4.10 1996/09/09 01:34:19 xtang Exp $
 * $Source: /release/112/cvs/Xsys/parser/parser_actions.h,v $
 *------------------------------------------------------------------
 * P A R S E R _ A C T I O N S . H  (formerly A C T I O N S . H)
 *
 * Copyright (c) 1991-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: parser_actions.h,v $
 * Revision 3.6.4.10  1996/09/09  01:34:19  xtang
 * CSCdi56632:  unknown clock rates on 4T PA, FSIP
 * Branch: California_branch
 *         add ALT_HELPS to use csb->which in different
 *         driver procedures, and print the
 *         driver specific info. In this case, the clockrate
 *         option table, and csb->which = CFG_INT_CLOCKRATE
 *
 * Revision 3.6.4.9  1996/08/12  16:06:34  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.6.4.8  1996/07/29  21:32:46  rchandra
 * CSCdi64437:  BGP: community display is not user-friendly
 * Branch: California_branch
 * - add user friendly community display
 * - add update throttling code (CSCdi64451)
 * - allow 'set community <additive>' during redistribution into BGP
 *
 * Revision 3.6.4.7  1996/07/09  21:07:44  etrehus
 * CSCdi45163:  transmitter-delay help message is confusing
 * Branch: California_branch
 *
 * Revision 3.6.4.6  1996/06/18  17:45:01  snyder
 * CSCdi60476:  CSCdi32916 could have been CPU specific
 * Branch: California_branch
 *         Saves space on non-MIP images
 *
 * Revision 3.6.4.5  1996/05/30  22:44:29  hampton
 * Add parser support for accepting per-protocol access list number
 * ranges.  [CSCdi59067]
 * Branch: California_branch
 *
 * Revision 3.6.4.4  1996/05/21  09:59:21  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.6.4.3  1996/05/17  11:39:37  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.5.2.4  1996/05/05  23:41:52  ppearce
 * Sync to IbuMod_Calif_baseline_960504
 *
 * Revision 3.5.2.3  1996/04/01  04:41:54  bchan
 * CSCdi53188:  appn parser multiple definition
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.5.2.1  1996/03/17  18:19:23  ppearce
 * Sync IbuMod_Calif_branch to V111_1_3
 *
 * Revision 3.6.4.2  1996/05/09  14:40:20  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.6.4.1.8.1  1996/04/27  07:17:30  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.6.4.1  1996/04/08  22:07:23  john
 * CSCdi53685:  Allow encoded characters in parser string macro
 * Branch: California_branch
 *
 * Revision 3.6.16.1  1996/04/08  02:00:12  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.6.8.1  1996/03/22  09:41:24  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.6  1996/03/09  22:57:03  widmer
 * CSCdi47890:  HTTP server doesnt work
 * Add HTTP server
 *
 * Revision 3.5  1996/02/20  18:02:18  widmer
 * CSCdi49363:  Drop function name from INVALID_NVGEN_CHECK
 *
 * Revision 3.4  1996/02/16  22:42:32  widmer
 * CSCdi49239:  Make time and timesec real parser macros
 *
 * Revision 3.3.18.1  1996/03/05  05:50:46  rlowe
 * Apply LANE content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.3.14.1  1996/02/27  21:11:34  cakyol
 * non sync sync of LE_Cal_V111_0_16_branch to V111_1_0_3 yielding
 * LE_Cal_V111_1_0_3_branch
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.3  1996/01/16  21:18:08  widmer
 * CSCdi33940:  show extended command doesnt work
 * Add keyword_trans macro that doesn't accept the keyword
 *  unless commands have been added to the link point.
 *
 * Revision 3.2.10.1  1996/01/11  19:52:39  cakyol
 * non sync sync now complete
 *
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.2  1995/12/17  18:35:19  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.1  1995/12/15  03:46:19  thille
 * Parser organizational cleanup.  Rename core parser files to be
 * parser_<foo>.  Start by adding placeholders for the new filenames.
 *
 * OLD LOG as actions.h:
 * ---------------------
 * Revision 3.7  1995/12/16  05:12:04  widmer
 * CSCdi45745:  Need to modularize ethernet address parsing more
 *
 * Revision 3.6  1995/12/15  22:28:08  widmer
 * CSCdi45685:  Need priv flag for interactive commands
 * Add Priv_Interactive to flag interactive commands
 *
 * Revision 3.5  1995/12/14  13:07:39  widmer
 * CSCdi45510:  Add General_String macro to replace other string macros
 * Change Var_String, String, Txt_Line, and Text to use General_String
 * Add fail argument to Priv_Test
 * Change Neg1 to use Keyword_Id
 * Remove Sense macro
 *
 * Revision 3.4  1995/11/27  21:24:56  widmer
 * CSCdi43607:  Add keyword_option macro to parse multiple keywords
 * Also create permit_deny macro using keyword_option macro to parse
 * permit/deny keywords.
 * Change "ip security multilevel" command to use keyword_option macro.
 *
 * Revision 3.3  1995/11/17  18:41:57  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:20:13  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:49:28  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:20:03  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  22:00:47  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 *			Macros
 ******************************************************************/

/* Handy, limits.h type stuff: */
#define	ULONG_MAX		0xffffffffL
#define	USHORT_MAX		0xffff
#define SHORTHELP_LEN		32

#define	isodigit(c)		(((c) >= '0') && ((c) <= '7'))
#define is_terminator(c)	(((c)=='\0') || ((c)=='!') || ((c)==';'))
#define CVHTOB(c)	((int) c - \
			 (((c >= '0') && (c <= '9')) ? \
			  '0' : \
			  (((c >= 'a') && (c <= 'f')) ? \
				 'a'-10 : \
				 'A'-10)))

#define	INVALID_NVGEN_CHECK\
    if (csb->nvgen) {\
	if (!csb->priv_set) {\
	    errmsg(&msgsym(INVLDNVGEN, PARSER), csb->nv_command);\
	}\
	return;\
    }


/******************************************************************
 *			Defines
 ******************************************************************/

/* Command completion flags */
#define NO_COMMAND_COMPLETION           0
#define SUPPORT_COMMAND_COMPLETION      1

#define	ADDR_SMDS	101
#define	ADDR_SNPA	102

#define	ADDR_PARSE(n)	0xf000+n

#define	ADDR_NAME_LOOKUP	0x00000001
#define	ADDR_LIL_ENDIAN		0x00000002
#define	ADDR_HOST_ADDR_ONLY	0x00000004
#define	ADDR_HOST_NET_ONLY	0x00000008
#define	ADDR_PROTO_ADDR		0x00000010
#define	ADDR_NO_SHORT_HELP	0x80000000
#define	ADDR_ALL		0xffffffff

/* Flag values for number_struct types field. */

#define	NUMBER_HEX		0x00000001 /* hexadecimal number	     */
#define	NUMBER_OCT		0x00000002 /* octal number		     */
#define	NUMBER_DEC		0x00000004 /* decimal number		     */
#define	NO_NUMBER		0x00000008 /* don't parse number if !sense   */
#define	HEX_ZEROX_OK		0x00000010 /* hex number can have '0x000'    */
#define	HEX_NO_ZEROX_OK		0x00000020 /* hex number can have no '0x000' */
#define	HEX_ZEROX_MATCHED	0x00000040 /* hex number matched '0x000'     */
#define	HEX_ZEROX_NOT_MATCHED	0x00000080 /* hex number didn't match '0x000'*/
#define	OCT_ZERO_OK		0x00000100 /* oct number can have '0'        */
#define	OCT_NO_ZERO_OK		0x00000200 /* oct number can have no '0'     */
#define	OCT_ZERO_MATCHED	0x00000400 /* oct number matched '0'	     */
#define	OCT_ZERO_NOT_MATCHED	0x00000800 /* oct number didn't match '0'    */

#define	SIGNED_NUMBER		0x00001000 /* parse signed number            */

#define	NUMBER_WS_OK		0x00100000 /* number can have whitespace     */
#define	NUMBER_NO_WS_OK		0x00200000 /* number can have no whitespace  */
#define	KEYWORD_WS_OK		0x00400000 /* keyword can have whitespace    */
#define	KEYWORD_NO_WS_OK	0x00800000 /* keyword can have no whitespace */

#define	NUMBER_HELP_CHECK	0x01000000 /* give help			     */
#define	KEYWORD_HELP_CHECK	0x02000000 /* give help			     */

#define	KEYWORD_OR_SET		0x04000000 /* OR value instead of set	     */
#define	KEYWORD_TEST		0x08000000 /* Test value before parsing      */
#define	NUMBER_NV_RANGE		0x10000000 /* Do NV of number range	     */

#define	STRING_WS_OK		NUMBER_WS_OK /* Whitespace allowed in string */
#define	STRING_HELP_CHECK	NUMBER_HELP_CHECK /* Provide help for string */
#define	STRING_QUOTED		0x20000000 /* Quotes aren't included         */

/*
 * Command privileges.  The privilege levels can be 1-255 (stored
 * as uchar).  Privilege level 0 is reserved
 * to indicate that normal processing of keywords should
 * be done.  If 'priv_set' is non-zero, then the keywords which match
 * should copy 'priv_set' into their 'priv' variable.
 *
 */

/*
 * Privilege level is stored in a unsigned integer broken
 * down like the following.
 *
 * bits 0-3	current privilege level
 * bits 4-7	original privilege level when privilege level has been changed
 * bits 8	indicates current privilege level has changed
 * bits 9-22	unused
 * bits 23		indicate distilled configuration
 * bits 24-31	privilege modification flags
 *
 */

#define	PRIV_MIN	0x0
#define	PRIV_NULL	0x0		/* PRIV level means nothing. */
#define	PRIV_USER	0x1		/* No special privileges */
#define	PRIV_OPR	0xf		/* Operator */
#define	PRIV_CONF	0xf		/* Allowed to do configuration */
#define	PRIV_ROOT	0xf		/* The Supreme Administrator */
#define	PRIV_MAX	0xf
#define	PRIV_INTERACTIVE 0x00400000	/* Interactive command */
#define	PRIV_CHANGED	0x00000100	/* Privilege level has changed */
#define PRIV_DISTILLED	0x00800000	/* Configuration to be distilled */
#define	PRIV_INTERNAL	0x01000000	/* Command "for internal use only" */
#define	PRIV_UNSUPPORTED 0x02000000	/* Hidden unsupported command */
#define	PRIV_USER_HIDDEN 0x04000000	/* Hidden command from disabled user */
#define	PRIV_SUBIF	0x08000000	/* Allowed on subinterfaces */
#define	PRIV_HIDDEN	0x10000000	/* Hidden commands.  Subtree rooted at
					 * this keyword is not searched. */
#define	PRIV_DUPLICATE	0x20000000	/* Keyword is a duplicate for
					 * split parse chains */
#define	PRIV_NONVGEN	0x40000000	/* Keywords that do not NVGEN */
#define	PRIV_NOHELP	0x80000000	/* Keywords that do not provide help */
#define	DEFAULT_PRIV	PRIV_USER	/* Default user privilege level */

#define TRANS_PRIV_PRIV_MASK    0xf
#define	TRANS_PRIV_FLAGS_OFFSET	9
#define TRANS_PRIV_FLAGS_GET(priv) ((priv) << TRANS_PRIV_FLAGS_OFFSET)
#define TRANS_PRIV_FLAGS_SET(priv) ((priv) >> TRANS_PRIV_FLAGS_OFFSET)

/*
 *
 */
#define	RMATCH_IP		0x00100000
#define	RMATCH_OSI		0x00200000
#define	RMATCH_NOVELL		0x00400000
#define	RMATCH_NONE		0x80000000

#define	RMATCH_ARGS		0x01000000
#define	RMATCH_NO_ARGS		0x02000000
#define	RMATCH_TAG_REQUIRED	0x04000000
#define	RMATCH_NO_TAG		0x08000000
#define	RMATCH_NO_KEY_HELP	0x10000000

#define ROARG_NONE		1
#define ROARG_AS		2
#define ROARG_TAG		3
#define	ROARG_PID		4

/*
 * Interface parsing flags
 */
#define	PARSE_INTERFACE_NOFLAG	0x0000	/* No flags */
#define	PARSE_UNIT_ONLY		0x0001	/* Parse interface unit number only */
#define	PARSE_INTERFACE_KEYWORD	0x0002	/* Parse keyword common_str_interface before */
#define	PARSE_CREATE_IDB	0x0004	/* Create dynamic intfce on success */


/*
 * Defines used for console handling
 */
#define CON_CONSOLE           0x00000001
#define CON_AUX               0x00000002
#define CON_TTY               0x00000004
#define CON_VTY               0x00000008
#define CON_ABSOLUTE          0x0000ffff
#define CON_RANGE             0x10000000

/******************************************************************
 *			Data Structures
 ******************************************************************/


typedef struct trans_priv_ {
     uint cur_priv : 4;
     uint def_priv : 4;
     uint PACKED(flags : 23);
     uint PACKED(priv_changed : 1);
#ifdef OBJECT_4K
     uint pad;   /* see CSCdi32916, before eliminating this pad */
#endif
} trans_priv;

typedef const struct general_keyword_struct_ {
    const char *str;	/* The string to match */
    const char *help;	/* Help string */
    trans_priv *priv;	/* Privilege and flags of this keyword */
    uint offset;	/* CSB offset to store value if keyword matches */
    uint val;		/* Value to store */
    uint minmatch;	/* min number of chars required to match */
    uint flags;		/* white space and help flags */
} general_keyword_struct;

typedef const struct keyword_struct_ {
    const char *str;
    const char *help;
    trans_priv *priv;
} keyword_struct;

typedef const struct keyword_mm_struct_ {
    const char *str;
    const char *help;
    trans_priv *priv;
    uint   minmatch;
} keyword_mm_struct;

typedef const struct keyword_id_struct_ {
    const char *str;	/* The keyword */
    const char *help;	/* Help string for this keyword */
    trans_priv *priv;	/* Privilege of this keyword */
    uint   offset;	/* CSB offset to store value if keyword matches */
    uint  val;		/* Value to store */
} keyword_id_struct;

typedef const struct keyword_id_mm_struct_ {
    const char *str;	/* The keyword */
    const char *help;	/* Help string for this keyword */
    trans_priv *priv;	/* Privilege of this keyword */
    uint   offset;	/* CSB offset to store value if keyword matches */
    uint  val;		/* Value to store */
    uint   minmatch;     /* minimum number of char to match */
} keyword_id_mm_struct;

typedef const struct keyword_options_ {
    const char *keyword;
    const char *help;
    uint val;
} keyword_options;

extern keyword_options permitdeny_options[];

typedef const struct keyword_option_struct_ {
    keyword_options *options;
    uint offset;
    trans_priv *priv;
    uint flags;
} keyword_option_struct;

typedef struct dynamic_number_struct_ {
    uint offset;	/* CSB offset to store parsed number */
    uint lower;		/* Lower limit we'll accept */
    uint upper;		/* Upper limit */
    const char *help;	/* The long help string */
    uint flags;		/* flags: hex, octal, decimal allowed,
			   whitespace allowed, to help or not to help	*/
} dynamic_number_struct;

typedef const struct dynamic_number_struct_ number_struct;

typedef const struct signed_number_struct_ {
    uint offset;	/* CSB offset to store parsed number */
    signed int lower;	/* Lower limit we'll accept */
    signed int upper;	/* Upper limit */
    const char *help;	/* The long help string */
} signed_number_struct;

typedef void (*parser_number_func)(parseinfo *, uint *, uint *);
typedef const struct number_func_struct_ {
    uint offset;		/* CSB offset to store parsed number */
    parser_number_func func;	/* Function to call to set range */
    const char *help;		/* The long help string */
} number_func_struct;

typedef void (*parser_number_help_func)(parseinfo *, uint *, uint *,const char **);
typedef const struct number_help_func_struct_ {
    uint offset;		   /* CSB offset to store parsed number */
    parser_number_help_func func;  /* Function to set range and help string */
} number_help_func_struct;


#define PARAMS_STRUCT_COMMON \
    const char *str;	/* Keyword to match */ \
    uint offset;	/* CSB offset to store parsed number */ \
    uint lower;		/* Lower range of parsed number */ \
    uint upper;		/* Upper range of parsed number */ \
    void *func;		/* func to call if parse is successful */ \
    uint subfunc; 	/* Subfunction code */ \
    const char *keyhelp;/* Keyword help string */ \
    const char *varhelp;/* Variable help string */ \
    trans_priv *priv;	/* Privilege of the keyword */ \
    uint flags;		/* Look for trailing whitespace, no whitespace, etc */

typedef const struct params_struct_ {
    PARAMS_STRUCT_COMMON
} params_struct;

typedef const struct params_struct_alist {
    PARAMS_STRUCT_COMMON
    uint lower2;	/* Lower range of alternate parsed number */
    uint upper2;	/* Upper range of alternate parsed number */
} params_struct_alist;

typedef const struct print_struct_ {
    const char * str;
} print_struct;

typedef const struct hub_struct_ {
    uint first;		/* CSB offset to first number */
    uint last;		/* CSB offset to last number */
    const char *help;	/* The long help string */
} hub_struct;

typedef const struct ttyline_struct_ {
    uint first;		/* CSB offset to first number */
    uint last;		/* CSB offset to last number */
    uint flags;		/* line type */
    const char *help;	/* The long help string */
} ttyline_struct;

typedef const struct help_struct_ {
  const char *str;
  transition *eol;
} help_struct;

typedef const struct char_struct_ {
  const character;
} char_struct;

typedef const struct string_struct_ {
    uint offset;	/* CSB offset to store parsed string */
    const char *help;	/* Help message for this string */
} string_struct;

typedef const struct general_string_struct_ {
    uint offset;	/* CSB offset to store parsed string */
    const char *help;	/* Help message for this string */
    uint flags;		/* Flags affecting how string is parsed */
} general_string_struct;

typedef const struct set_struct_ {
    uint offset;	/* CSB offset to store value */
    uint val;		/* Value to store */
} set_struct;

typedef const struct test_struct_ {
    uint offset;	/* CSB offset to numeric variable to test */
    uint	val;	/* The value to test it against */
    transition *equal;	/* Go here if == */
    transition *notequal;	/* Go here if != */
    transition *lt;		/* Go here if < */
    transition *gt;		/* Go here if > */
} test_struct;

typedef const struct test_bool_struct_ {
    boolean *var;		/* Pointer to the variable to be tested */
    transition *zero;		/* Go here if variable == NULL */
    transition *nonzero;	/* Go here if variable != NULL */
} test_bool_struct;

typedef const struct test_int_struct_ {
    int *var;			/* Pointer to the variable to be tested */
    transition *zero;		/* Go here if variable == NULL */
    transition *nonzero;	/* Go here if variable != NULL */
} test_int_struct;

typedef const struct test_func_struct_ {
    boolean (* func)(void);
    transition *zero;           /* Go here if variable == NULL */
    transition *nonzero;        /* Go here if variable != NULL */
} test_func_struct;

typedef const struct test_line_struct_ {
    uint type;			/* bit field of valid line types */
    transition *zero;		/* Go here if line range is valid */
    transition *nonzero;	/* Go here if line range is invalid */
} test_line_struct;

typedef const struct test_idbstatus_struct_ {
    transition *false;
    uint val;
} test_idbstatus_struct;

typedef const struct eols_struct_ {
    void	*func;
    uint	subfunc;
} eols_struct;

typedef const struct nvgens_struct_ {
    void *func;
    uint subfunc;
} nvgens_struct;

typedef const struct interface_struct_ {
    uint offset;	       /* Offset of idb in csb */
    uint64 valid_ifs;	       /* Which interfaces are valid in this context */
    uint flag;		       /* Interface parsing flags */
    const char *keyword;       /* Optional keyword to parse */
    const char *help;	       /* Help for optional keyword */
} interface_struct;

typedef const struct addrparse_struct_ {
    uint offset;	/* CSB offset to store parsed address */
    const char *help;	/* Help string for this address */
    uint type;		/* Type of addres to parse */
    uint flag;		/*  How to parse (e.g. addr only, addr or hostname) */
} addrparse_struct;


/* Same thing as an addrparse_struct, except without the flag */
typedef const struct addrparse_nf_struct_ {
    uint offset;	/* CSB offset to store parsed address */
    const char *help;	/* Help string for this address */
    uint type;		/* Type of addres to parse */
} addrparse_nf_struct;

typedef const struct ipmask_struct_ {
    uint mask;		/* CSB offset to store parsed mask */
    const char *help;	/* Help string */
} ipmask_struct;

typedef const struct ipaddrmask_struct_ {
    uint addr;		/* CSB offset to store parsed address */
    uint mask;		/* CSB offset to store parsed mask */
    const char *help;	/* Help string */
} ipaddrmask_struct;

typedef const struct hexdata_struct_ {
    uint bufoffset;		/* CSB offset to store parsed hex bytes */
    uint numoffset;		/* CSB offset for number of bytes stored */
    const char *help;	/* Help string for this address */
    uint bufmax;		/* number of bytes in output buffer */
} hexdata_struct;

typedef const struct routerprotocolinfo_ {
    uint protocol;	/* Protocol define */
    const char *str;	/* keyword (router type) to match */
    const char *help;	/* help string */
    uint type;		/* 3rd arg: NONE, AS or OSI TAG */
} routerprotocolinfo;

typedef const struct routerparse_struct_ {
    uint offset;	/* CSB offset for parsed router info */
    uint protocol;	/* List of protocols to match */
} routerparse_struct;

typedef const struct char_struct2_ {
    const char character;
    const char *help;
} char_struct2;

/* Month of the year */

typedef const struct month_struct_ {
    uint offset;	/* CSB offset */
    const char *help;
} month_struct;

/* Day of the week */

typedef const struct day_struct_ {
    uint offset;	/* CSB offset */
    const char *help;
} day_struct;

typedef const struct timesec_struct_ {
    uint var1;
    uint var2;
    uint var3;
    const char *help;
} timesec_struct;

typedef const struct bgp_community_struct_ {
    uint var1;
    uint var2;
    const char *help;
} bgp_community_struct;

/* TCP & UDP port information */

typedef const struct portinfo_ {
    ushort port;			/* port number */
    const char *str;			/* port name */
    const char *help;			/* help string */
    uchar minmatch;			/* min match */
} portinfo;

typedef const struct portparse_struct_ {
    uint offset;			/* CSB offset to store parsed port */
} portparse_struct;

typedef boolean (* testexpr_func)(parseinfo *, transition *);
typedef const struct testexpr_struct_ {
    transition *false;
    const testexpr_func func;
} testexpr_struct;

/* New protocol name/address parsing scheme
 *
 * The specification of which protocols are valid for parsing is done
 * using a string, interpreted as a series of two-character codes.
 * My convension is that protocol groups are named with two capital
 * letters, and individual protocols are named with a capital and
 * a lowercase letter.  However, this convension is nowhere enforced
 * or relied on by code.  It serves mearly to help me keep these
 * names unique.
 *
 * Any changes to this list should be reflected in the link_names
 * table in actions.c that maps codes (both individual and group)
 * to names and LINK_... macros.
 */

#define PMATCH_EXCLUDE		"E:"	/* Exclude following protocols */
#define PMATCH_INCLUDE		"I:"	/* Include following protocols */

#define PMATCH_ALL		"AL"	/* All protocols */
#define PMATCH_DGRAM		"DG"	/* All datagram protocols */

#define PMATCH_AARP		"Aa"	/* AppleTalk ARP */
#define PMATCH_APOLLO		"Ap"
#define PMATCH_APPLETALK	"At"
#define PMATCH_ARP		"Ar"
#define PMATCH_BRIDGE		"Br"
#define PMATCH_CDP              "Cd"
#define PMATCH_CLNS		"Cl"
#define PMATCH_CLNS_ALL_ES	"Ce"
#define PMATCH_CLNS_ALL_IS	"Ci"
#define PMATCH_COMPRESSED_TCP	"Ct"
#define PMATCH_CMNS		"Cm"
#define PMATCH_DECNET		"De"
#define PMATCH_DECNET_NODE	"Dn"
#define PMATCH_DECNET_ROUTER_L1	"Dr"
#define PMATCH_DECNET_ROUTER_L2	"Da"
#define PMATCH_DLSW             "Dl"
#define PMATCH_DECNET_PRIME_ROUTER "Dp"
#define PMATCH_IP		"Ip"
#define PMATCH_LLC2		"Ll"
#define PMATCH_NOVELL		"No"
#define PMATCH_PAD		"Pa"
#define PMATCH_QLLC             "Ql"
#define PMATCH_RSRB		"Rs"	/* Remote Source-Route Bridging */
#define PMATCH_SNAPSHOT		"Sn"
#define PMATCH_STUN		"St"	/* Serial Tunnel */
#define PMATCH_VINES		"Vi"
#define PMATCH_XNS		"Xn"
#define PMATCH_BSTUN            "Bs"   /* block serial tunnel */
#define PMATCH_VTP		"Vt"

typedef const struct protoname_struct_ {
    uint variable;		/* CSB offset to store parsed link type */
    const char *match_spec;	/* Specification of acceptable protocols */
} protoname_struct;

typedef const struct protoaddr_struct_ {
    uint num_offset;
    uint addr_offset;
    const char *match_spec;
} protoaddr_struct;

typedef struct link_name_ {
    enum LINK link_type;
    const char *codes;
    const char *name;
    const char *help;
    uint min_match;
    uint addr_type;
} link_name;

typedef struct addr_func_ {
    uint addr_type;
    short_help_func get_short_help;
    match_gen_func func;
} addr_func;

typedef struct router_name_ {
    uint type;
    void (*func)(parseinfo *, const char *, char *, transition *);
} router_name;

typedef const struct transport_struct_ {
    uint trans_var;		/* CSB variable to return value to */
    boolean oneonly;		/* TRUE if only one transport name allowed */
} transport_struct;

typedef struct priv_struct_ {
    trans_priv *priv;
    transition *fail;
} priv_struct;

typedef const struct mode_struct_ {
    uint offset;
    uint flags;
} mode_struct;

/******************************************************************
 *              External variables
 ******************************************************************/

extern addr_func *addr_funcs;
extern link_name *link_names;
extern router_name *router_names;
extern routerprotocolinfo routerprotocols[];

/******************************************************************
 * 		Function prototypes
 ******************************************************************/

extern void nvgen_token(parseinfo *, transition *, const char *);
boolean help_check(parseinfo *);
void help_or_error(parseinfo *, int, const char *, boolean);

boolean match_whitespace2(char *, int *, boolean);
boolean match_minusone(char *, int *);
boolean match_number(parseinfo *, uint, uint, uint *,
		     uint *, uint, const char *);
boolean match_decimal(char *, uint, uint, int *, ushort *);
boolean match_octal(char *, uint, uint, int *, ushort *);
boolean match_hexadecimal(char *, int *, ushort *);
boolean match_ulong_decimal(char *, ulong, ulong, uint *, ulong *);
boolean match_ulong_octal(char *, ulong, ulong, uint *, ulong *);
boolean match_ulong_hexadecimal(char *, ulong, ulong, uint *, ulong *);
boolean match_char(char *, int *, char);
boolean get_name(char *buf, char *name, int *pi, int buflen);
void general_number_short_help(uint *, uint, uint, char *);
void general_number_short_help_http(uint *, uint, uint, char *, boolean);

void save_ambig_string(parseinfo *, const char *, const char *);
void save_help_long(parseinfo *, const char *, const char *);
uint copy_varstring(char *, char *, uint, uint);


void snpaaddr_action(parseinfo *, transition *, addrparse_struct * const);
void xnshostaddr_action(parseinfo *, transition *, addrparse_struct * const);
void grouplist_action(parseinfo *, transition *, string_struct * const);
void ip_router_action(parseinfo *, transition *, routerparse_struct * const);
void router_action(parseinfo *, transition *, routerparse_struct * const);
void literal_char_action(parseinfo *, transition *, char_struct2 * const);
void protoname_action(parseinfo *, transition *, protoname_struct * const);
void protoaddr_action(parseinfo *, transition *, protoaddr_struct * const);
void transport_action(parseinfo *, transition *, transport_struct * const);
void multiple_funcs_action(parseinfo *, transition *, test_struct * const);
void display_action(parseinfo *, transition *, help_struct * const);
void NONE_action(parseinfo *);
void no_alt_action(parseinfo *);
void link_point_action(parseinfo *);
void NOP_action(parseinfo *, transition *);
void general_keyword_action(parseinfo *, transition *,
			    general_keyword_struct * const);
void keyword_action(parseinfo *, transition *, keyword_struct * const);
void keyword_optws_action(parseinfo *, transition *, keyword_struct * const);
void keyword_nows_action(parseinfo *, transition *, keyword_struct * const);
void keyword_mm_action(parseinfo *, transition *, keyword_mm_struct * const);
void keyword_id_action(parseinfo *, transition *, keyword_id_struct * const);
void keyword_id_mm_action(parseinfo *, transition *, keyword_id_mm_struct * const);
void keyword_orset_action(parseinfo *, transition *, keyword_id_struct * const);
void keyword_ortest_action(parseinfo *, transition *, keyword_id_struct * const);
void keyword_option_action(parseinfo *, transition *, keyword_option_struct * const);
void keyword_trans_action(parseinfo *, transition *, keyword_struct *const);
void whitespace_action(parseinfo *, transition *);
void print_action(parseinfo *, transition *, print_struct * const);
void eol_action(parseinfo *, transition *, void (*)(parseinfo *));
void eols_action(parseinfo *, transition *, eols_struct * const);
void eolns_action(parseinfo *, transition *, void (*)(parseinfo *));
void alt_help_action(parseinfo *, transition *, void (*)(parseinfo *));
void alt_helps_action(parseinfo *, transition *, eols_struct * const);
void help_action(parseinfo *, transition *, help_struct * const);
void ttyline_action(parseinfo *, transition *, ttyline_struct * const);
void hub_action(parseinfo *, transition *, hub_struct * const);
void char_action(parseinfo *, transition *, char_struct * const);
void char_number_action(parseinfo *, transition *, number_struct * const);
void func_action(parseinfo *, transition *, void (*)(parseinfo *));
void set_action(parseinfo *, transition *, set_struct * const);
void test_action(parseinfo *, transition *, test_struct * const);
void test_boolean_action(parseinfo *, transition *, test_bool_struct * const);
void test_int_action(parseinfo *, transition *, test_int_struct * const);
void test_func_action(parseinfo *, transition *, test_func_struct * const);
void test_line_action(parseinfo *, transition *, test_line_struct * const);
void test_idbstatus_action(parseinfo *, transition *, test_idbstatus_struct * const);
void nvgens_action(parseinfo *, transition *, nvgens_struct * const);
void nvgenns_action(parseinfo *, transition *, void (*)(parseinfo *));
void noprefix_action(parseinfo *, transition *);
void params_action(parseinfo *, transition *, params_struct * const);
void params_action_alist(parseinfo *, transition *, params_struct_alist * const);
void interface_action(parseinfo *, transition *, interface_struct * const);
void controller_action(parseinfo *, transition *, void *);
void general_number_action(parseinfo *, transition *, number_struct * const);
void signed_number_action(parseinfo *, transition *,
			  signed_number_struct * const);
void number_func_action(parseinfo *, transition *, number_func_struct * const);
void number_help_func_action(parseinfo *, transition *, number_help_func_struct * const);
void generic_addr_action(parseinfo *, transition *, addrparse_struct * const);
void generic_addr_noflag_action(parseinfo *, transition *,
				addrparse_nf_struct * const);
void ipmask_action (parseinfo *, transition *, ipmask_struct * const);
void ipaddrmask_action (parseinfo *, transition *,
			ipaddrmask_struct * const);
void hexdata_action(parseinfo *, transition *, hexdata_struct * const);
boolean eol_check(parseinfo *);
void priv_action(parseinfo *, transition *, priv_struct * const);
void mode_action(parseinfo *, transition *, mode_struct *const);
void day_action(parseinfo *, transition *, day_struct * const);
void month_action(parseinfo *, transition *, month_struct * const);
void timesec_action(parseinfo *, transition *, timesec_struct * const);
void bgp_community_action(parseinfo *, transition *, 
			  bgp_community_struct *const);
void comment_action(parseinfo *, transition *);
void testexpr_action(parseinfo *, transition *, testexpr_struct * const);
void general_string_action(parseinfo *, transition *,
			   general_string_struct * const);

boolean match_partial_keyword(parseinfo *, const char *, int *pi,
				     const char *, int, uint);
extern void mark_range(int, int);

/*
 * mode.c
 */
extern void nv_mode(parseinfo *, transition *, mode_struct * const);
extern uint match_mode(parseinfo *, int *, parser_mode **, uint);

void appn_fqname_action(parseinfo *, transition *, string_struct * const);
void appn_type_a_name_action(parseinfo *, transition *, string_struct * const);
void appn_name_star_action(parseinfo *, transition *, string_struct * const );
void appn_type_a_name_action(parseinfo *, transition *, string_struct * const);
void appn_type_a_name_star_action(parseinfo *, transition *, string_struct * const);
void appn_pcid_action (parseinfo *, transition *, string_struct * const );

/*
 * eol string used by AAA command authorization
 */
extern char *parser_eol_string;

/* $Id: cfg_queue-list.h,v 3.5.6.1 1996/05/21 09:49:39 thille Exp $
 * $Source: /release/112/cvs/Xsys/if/cfg_queue-list.h,v $
 *------------------------------------------------------------------
 * Configuration commands for custom queueing
 *
 * February 1993, Bruce Cole
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_queue-list.h,v $
 * Revision 3.5.6.1  1996/05/21  09:49:39  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.5  1996/03/04  09:52:34  fred
 * CSCdi50233:  Cant Increase Queue 0s Size
 *         enable custom queue-list to address queue 0
 *
 * Revision 3.4  1996/02/01  23:33:32  widmer
 * CSCdi45325:  Change Keyword/Set to use Keyword_Id macro
 *
 * Revision 3.3  1996/01/18  02:12:55  tli
 * CSCdi47063:  show ip cache cannot be restricted to subinterfaces
 * Change IFTYPE_ANY to IFTYPE_HWIDB
 *
 * Revision 3.2  1995/11/17  09:26:21  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:48:23  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  10:53:13  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  22:11:56  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/**********************************************************************
 * This parse-chain section is used by several commands.  There
 * are several tests performed to determine command specific
 * processing.
 **********************************************************************/

/* gt <size>
 * lt <size>
 */
NUMBER	(qlist_size, prilist_eol, no_alt,
	 OBJ(int,6), 0, 0xffff,
	 "Packet size (include MAC encapsulation bytes)");
KEYWORD_ID (qlist_lt, qlist_size, cfg_prilist_extend_here,
	    OBJ(int,5), PRIORITY_LT,
	    "lt", "Classify packets less than a specified size", PRIV_CONF);

KEYWORD_ID (qlist_gt, qlist_size, qlist_lt,
	    OBJ(int,5), PRIORITY_GT,
	    "gt", "Classify packets greater than a specified size", PRIV_CONF);

/* [args] (see above) are only valid for the 'queue-list protocol' command,
 * and then only for the 'bridge' protocol.
 */
TESTVAR	(qlist_args_test, qlist_gt, prilist_stun_args,
	 NONE, NONE, NONE, OBJ(int,2), PRICMD_PROTOCOL);
SET	(qlist_args, qlist_args_test, OBJ(int,5), PRIORITY_UNSPEC );

/*
 * <queue-keyword>
 *
 * The documentation says that the <queue-keyword> is optional if
 * "default" was specified, however the old code always required it.
 * We implement what the old code did.
 *
 * Note that the 'no queue-list interface' command code requires the
 * priority, but never uses it.  We may wish to add a test for it too.
 *
 * OBJ(int,4) = Queue #
 */

/* Always require the <queue-keyword> - see above comment.
 *TESTVAR	(qlist_qeol, prilist_eol, no_alt,
 *	 NONE, NONE, NONE, OBJ(int,2), PRICMD_DEFAULT);
 */
NUMBER (qlist_qnum, qlist_args, no_alt,
	OBJ(int,4), 0, PRIORITY_MAXTYPE-1, "queue number");



/************************************************************************
 * Command specific parse-chains.
 ************************************************************************/

/************************************************************************
 * queue-list <list> stun <queue #> <stun-group> <stun-addr>
 * no queue-list <list> stun <queue #> <stun-group> <stun-addr>
 *
 * OBJ(int,1) = <list>
 * OBJ(int,2) = PRICMD_STUN
 * OBJ(int,3) = <stun-group>
 * OBJ(int,4) = Queue #
 * (*OBJ(paddr,1)) = <stun-addr>
 */

NVGENS	(qlist_stun_nvgen, qlist_qnum,
	 prioritylist_command, TRUE);
KEYWORD_ID (qlist_stun1, qlist_stun_nvgen, no_alt,
	    OBJ(int,2), PRICMD_STUN,
	    "stun", 
	    "Establish priorities for stun packets", PRIV_CONF);


/************************************************************************
 * queue-list <list> queue <queue #> {limit <#> [byte-count <#>] |
 *				      byte-count <#> [limit <#>]}
 * no queue-list <list> queue <queue #> {limit <#> [byte-count <#>] |
 *				      byte-count <#> [limit <#>]}
 *
 * OBJ(int,1) = <list>
 * OBJ(int,2) = PRICMD_QCUSTOM
 * OBJ(int,3) = <Queue #>
 * OBJ(int,4) = Byte count parsed
 * OBJ(int,5) = <Queue byte count>
 * OBJ(int,6) = Queue length parsed
 * OBJ(int,7) = <Queue length>
 */

pdecl(qlist_limit);

SPLIT (qlist_limit_test, prilist_eol, no_alt, OBJ(int,6), TRUE);
SPLIT (qlist_both_test, prilist_eol, qlist_limit, OBJ(int,6), TRUE);
NUMBER (qlist_bcount_val, qlist_both_test, no_alt, OBJ(int,5), 0, 0xFFFFFF,
	"size in bytes");
KEYWORD_ID (qlist_bcount, qlist_bcount_val, qlist_limit_test,
	OBJ(int,4), TRUE, "byte-count",
	"Specify size in bytes of a particular queue", PRIV_CONF);

SPLIT (qlist_bcount_test, prilist_eol, qlist_bcount, OBJ(int,4), TRUE);
NUMBER (qlist_limit_val, qlist_bcount_test, no_alt, OBJ(int,7), 0, 32767,
	"number of queue entries");
KEYWORD_ID (qlist_limit, qlist_limit_val, qlist_bcount_test,
	OBJ(int,6), TRUE,
	"limit", "Set queue entry limit of a particular queue", PRIV_CONF);

NUMBER (qlist_qnum1, qlist_limit, no_alt,
	OBJ(int,3), 0, PRIORITY_MAXTYPE-1, "queue number");
NVGENS (qlist_qsize_nvgen, qlist_qnum1, prioritylist_command, TRUE);
KEYWORD_ID (qlist_qsize, qlist_qsize_nvgen, qlist_stun1,
	OBJ(int,2), PRICMD_QCUSTOM,
	"queue", "Configure parameters for a particular queue", PRIV_CONF);

/************************************************************************
 * queue-list <list> protocol <protocol-name> <queue #> [args]
 * no queue-list <list> protocol <protocol-name> <queue #> [args]
 *
 * OBJ(int,1) = <list>
 * OBJ(int,2) = PRICMD_PROTOCOL
 * OBJ(int,3) = <protocol-name>
 * OBJ(int,4) = <queue #>
 *
 * Get the protocol name, then go to the above common chains to get the
 * queue-keyword.
 */

TESTVAR	(qlist_proto_test, prilist_eol, no_alt,
	 NONE, NONE, NONE, sense, FALSE );

PROTONAME (qlist_protoname, qlist_qnum, qlist_proto_test,
	   OBJ(int, 3), PMATCH_ALL PMATCH_EXCLUDE
	   PMATCH_DECNET_PRIME_ROUTER PMATCH_QLLC);

KEYWORD_ID (qlist_protocol, qlist_protoname, qlist_qsize,
	OBJ(int,2), PRICMD_PROTOCOL,
	"protocol", "priority queueing by protocol", PRIV_CONF);


/************************************************************************
 * queue-list <list> interface <interface-name> <queue #>
 * no queue-list <list> interface <interface-name> <queue #>
 *
 * OBJ(int,1) = <list>
 * OBJ(int,2) = PRICMD_INTERFACE
 * OBJ(int,3) is unused
 * OBJ(int,4) = Queue #
 * if_name = <interface-name>
 *
 */
INTERFACE (qlist_if_name, qlist_qnum, no_alt,
	   OBJ(idb,1), IFTYPE_HWIDB     );
NVGENS	(qlist_if_nvgen, qlist_if_name, 
	 prioritylist_command, TRUE);
KEYWORD_ID (qlist_interface, qlist_if_nvgen, qlist_protocol,
	    OBJ(int,2), PRICMD_INTERFACE,
	    common_str_interface, 
	    "Establish priorities for packets from a named interface",
	    PRIV_CONF); 

/************************************************************************
 * queue-list <list> lowest-custom <queue #>
 * no queue-list <list> lowest-custom [<queue #>]
 *
 * OBJ(int,1) = <list>
 * OBJ(int,2) = PRICMD_LOWEST
 * OBJ(int,3) is unused
 * OBJ(int,4) = Queue #
 *
 * If we parse the 'lowest-custom' keyword, then check for NVGEN, and if
 * parsing, branch to gather the <queue-keyword>.
 */

NOPREFIX(qlist_lowest_noprefix, qlist_qnum, prilist_eol);
NVGENS	(qlist_lowest_nvgen, qlist_lowest_noprefix, 
	 prioritylist_command, TRUE);
KEYWORD_ID (qlist_lowest, qlist_lowest_nvgen, qlist_interface,
	    OBJ(int,2), PRICMD_LOWEST,
	    "lowest-custom", 
	    "Set lowest number of queue to be treated as custom", PRIV_CONF);

/************************************************************************
 * queue-list <list> default <queue #>
 * no queue-list <list> default [<queue #>]
 *
 * OBJ(int,1) = <list>
 * OBJ(int,2) = PRICMD_DEFAULT
 * OBJ(int,3) is unused
 * OBJ(int,4) = Queue #
 *
 * If we parse the 'default' keyword, then check for NVGEN, and if
 * parsing, branch to gather the <queue-keyword>.
 */

NOPREFIX(qlist_def_noprefix, qlist_qnum, prilist_eol);
NVGENS	(qlist_def_nvgen, qlist_def_noprefix, 
	 prioritylist_command, TRUE);
KEYWORD_ID (qlist_default, qlist_def_nvgen, qlist_lowest,
	    OBJ(int,2), PRICMD_DEFAULT,
	    "default", "Set custom queue for unspecified datagrams",
	    PRIV_CONF); 


/*************************************************************************/
/*
 * List number
 */

NUMBER (qlist_num, qlist_default, no_alt, OBJ(int, 1), 1,
	PRIORITY_MAXLIST-1, "Queue list number");

/*
 * Mark this as custom queueing.
 */

NVGENS	(qlist_nvgen, qlist_num, prioritylist_command, FALSE);

KEYWORD_ID(config_queuelist, qlist_nvgen, ALTERNATE,
	   OBJ(int, 16), QUEUE_CUSTOM,
	   "queue-list", "Build a custom queue list", PRIV_CONF);

#undef	ALTERNATE
#define	ALTERNATE	config_queuelist

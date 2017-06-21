/* $Id: cfg_priority-list.h,v 3.4.20.1 1996/05/21 09:49:38 thille Exp $
 * $Source: /release/112/cvs/Xsys/if/cfg_priority-list.h,v $
 *------------------------------------------------------------------
 * C F G _ P R I O R I T Y L I S T . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_priority-list.h,v $
 * Revision 3.4.20.1  1996/05/21  09:49:38  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.4  1996/02/01  23:33:31  widmer
 * CSCdi45325:  Change Keyword/Set to use Keyword_Id macro
 *
 * Revision 3.3  1996/01/18  02:12:54  tli
 * CSCdi47063:  show ip cache cannot be restricted to subinterfaces
 * Change IFTYPE_ANY to IFTYPE_HWIDB
 *
 * Revision 3.2  1995/11/17  09:26:20  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:48:22  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  10:53:13  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  22:11:35  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

EOLS	(prilist_eol, prioritylist_command, FALSE);

/**********************************************************************
 * This parse-chain section is used by several commands.  There
 * are several tests performed to determine command specific
 * processing.
 **********************************************************************/


/**********************************************************************
 * Args for 'priority-list stun <priority> [ address <stun-group> <stun-addr> ]
 * The args are parsed here because, unlike the majority of the other
 * variations of this command, the args are after the priority level.
 * NOTE: this form of the command is obsolete
 */

/* [ address <stun-group> <stun-addr> ] */
STUNADDR (prilist_stun_addr, prilist_eol, no_alt,
	  OBJ(paddr,1), "A STUN address");
EVAL	(prilist_stun_group_eval, prilist_stun_addr,
	 csb->stungroup = GETOBJ(int, 6));
NUMBER	(prilist_stun_group, prilist_stun_group_eval, no_alt,
	 OBJ(int, 6), 1, 255, "A valid STUN group number");
KEYWORD_ID(prilist_stun_addrkwd, prilist_stun_group, no_alt,
	   OBJ(int, 5), PRIORITY_ADDRESS, "address",
	   "Allow specification of a STUN group & address",
	   PRIV_CONF);

TESTVAR (prilist_stun_args, prilist_stun_addrkwd, prilist_eol,
	 NONE, NONE, NONE, OBJ(int,2), PRICMD_STUN);

/**********************************************************************
 *
 *
 * [<args>]
 * Valid arguments are:
 *
 *	gt <byte-count>
 *	lt <byte-count>
 *
 * OBJ(int,5) = PRIORITY_GT, PRIORITY_LT
 * OBJ(int,6) = <byte-count>
 *
 * First we check if we're parsing the 'priority-list <list> protocol' 
 * command.  Then we check for extended commands.
 *
 * All other commands transition to prilist_eol.
 */

/*
 * Extend here for protocol specific classification.  Everything above this
 * is also shared with custom queueing. 
 */

LINK_TRANS (cfg_prilist_extend_here, prilist_eol);

/* gt <size>
 * lt <size>
 */
NUMBER	(prilist_size, prilist_eol, no_alt,
	 OBJ(int,6), 0, 0xffff,
	 "Packet size (include MAC encapsulation bytes)");
KEYWORD_ID (prilist_lt, prilist_size, cfg_prilist_extend_here,
	    OBJ(int,5), PRIORITY_LT,
	    "lt", "Prioritize packets less than a specified size", PRIV_CONF);

KEYWORD_ID (prilist_gt, prilist_size, prilist_lt,
	    OBJ(int,5), PRIORITY_GT,
	    "gt", "Prioritize packets greater than a specified size",
	    PRIV_CONF);

/* [args] (see above) are only valid for the 'priority-list protocol' command,
 * and then only for the 'bridge' and 'ip' protocols.
 */
TESTVAR	(prilist_args_test, prilist_gt, prilist_stun_args,
	 NONE, NONE, NONE, OBJ(int,2), PRICMD_PROTOCOL);
SET	(prilist_args, prilist_args_test, OBJ(int,5), PRIORITY_UNSPEC);

/*
 * <queue-keyword>
 *
 * The documentation says that the <queue-keyword> is optional if
 * "default" was specified, however the old code always required it.
 * We implement what the old code did.
 *
 * Note that the 'no priority-list interface' command code requires the
 * priority, but never uses it.  We may wish to add a test for it too.
 *
 * OBJ(int,4) = PRIORITY_HIGH, PRIORITY_MEDIUM, PRIORITY_NORMAL, PRIORITY_LOW
 */

/* Always require the <queue-keyword> - see above comment.
 *TESTVAR	(prilist_qeol, prilist_eol, no_alt,
 *	 NONE, NONE, NONE, OBJ(int,2), PRICMD_DEFAULT);
 */
KEYWORD_ID (prilist_low, prilist_args, no_alt,
	 OBJ(int,4), PRIORITY_LOW,
	 "low", "", PRIV_CONF);
KEYWORD_ID (prilist_normal, prilist_args, prilist_low,
	 OBJ(int,4), PRIORITY_NORMAL,
	 "normal", "", PRIV_CONF);
KEYWORD_ID (prilist_medium, prilist_args, prilist_normal,
	 OBJ(int,4), PRIORITY_MEDIUM,
	 "medium", "", PRIV_CONF);
KEYWORD_ID (prilist_high, prilist_args, prilist_medium,
	 OBJ(int,4), PRIORITY_HIGH,
	 "high", "", PRIV_CONF);



/************************************************************************
 * Command specific parse-chains.
 ************************************************************************/

/***********************************************************************
 * priority-list <list> queue-limit
 *	<high-lim> <medium-lim> <normal-lim> <low-lim>
 * no priority-list <list> queue-limit
 *	[<high-lim> <medium-lim> <normal-lim> <low-lim>]
 *
 * OBJ(int,1) = <list>
 * OBJ(int,2) = PRICMD_QLIMIT
 * OBJ(int,3) = <high-lim>
 * OBJ(int,4) = <medium-limit>
 * OBJ(int,5) = <normal-limit>
 * OBJ(int,6) = <low-lim>
 */

NUMBER	(prilist_lowQ, prilist_eol, no_alt,
	 OBJ(int,6), 0, 32767, "Lower limit");
NUMBER	(prilist_normalQ, prilist_lowQ, no_alt,
	 OBJ(int,5), 0, 32767, "Normal limit");
NUMBER	(prilist_mediumQ, prilist_normalQ, no_alt,
	 OBJ(int,4), 0, 32767, "Medium limit");
NUMBER	(prilist_highQ, prilist_mediumQ, no_alt,
	 OBJ(int,3), 0, 32767, "High limit");

NOPREFIX (prilist_queue_noprefix, prilist_highQ, prilist_eol);
NVGENS	(prilist_queue_nvgen, prilist_queue_noprefix, 
	 prioritylist_command, FALSE);
KEYWORD_ID (prilist_queuelimit, prilist_queue_nvgen, no_alt, 
	 OBJ(int,2), PRICMD_QLIMIT,
	 "queue-limit", "Set queue limits for priority queues", PRIV_CONF);

/************************************************************************
 * priority-list <list> stun <priority> <stun-group> <stun-addr>
 * no priority-list <list> stun <priority> <stun-group> <stun-addr>
 *
 * OBJ(int,1) = <list>
 * OBJ(int,2) = PRICMD_STUN
 * OBJ(int,3) = <stun-group>
 * OBJ(int,4) = PRIORITY_HIGH, PRIORITY_MEDIUM, PRIORITY_NORMAL, PRIORITY_LOW
 * (*OBJ(paddr,1)) = <stun-addr>
 */

NVGENS	(prilist_stun_nvgen, prilist_high, 
	 prioritylist_command, FALSE);
KEYWORD_ID (prilist_stunpri, prilist_stun_nvgen, prilist_queuelimit,
	    OBJ(int,2), PRICMD_STUN,
	    "stun", 
	    "Establish priorities for stun packets", PRIV_CONF|PRIV_HIDDEN);


/************************************************************************
 * priority-list <list> protocol <protocol-name> <queue-keyword> [args]
 * no priority-list <list> protocol <protocol-name> <queue-keyword> [args]
 *
 * OBJ(int,1) = <list>
 * OBJ(int,2) = PRICMD_PROTOCOL
 * OBJ(int,3) = <protocol-name>
 * OBJ(int,4) = <queue-keyword>
 *
 * Get the protocol name, then go to the above common chains to get the
 * queue-keyword.
 */

TESTVAR	(prilist_proto_test, prilist_eol, no_alt,
	 NONE, NONE, NONE, sense, FALSE );

PROTONAME (prilist_protoname, prilist_high, prilist_proto_test,
	   OBJ(int, 3), PMATCH_ALL PMATCH_EXCLUDE
	   PMATCH_DECNET_PRIME_ROUTER PMATCH_QLLC);

KEYWORD_ID (prilist_protocol, prilist_protoname, prilist_stunpri,
	OBJ(int,2), PRICMD_PROTOCOL,
	"protocol", "priority queueing by protocol", PRIV_CONF);


/************************************************************************
 * priority-list <list> interface <interface-name> <queue-keyword>
 * no priority-list <list> interface <interface-name> <queue-keyword>
 *
 * OBJ(int,1) = <list>
 * OBJ(int,2) = PRICMD_INTERFACE
 * OBJ(int,3) is unused
 * OBJ(int,4) = PRIORITY_HIGH, PRIORITY_MEDIUM, PRIORITY_NORMAL, PRIORITY_LOW
 * if_name = <interface-name>
 *
 */
INTERFACE (prilist_if_name, prilist_high, no_alt,
	   OBJ(idb,1), IFTYPE_HWIDB     );
NVGENS	(prilist_if_nvgen, prilist_if_name, 
	 prioritylist_command, FALSE);
KEYWORD_ID (prilist_interface, prilist_if_nvgen, prilist_protocol,
	    OBJ(int,2), PRICMD_INTERFACE,
	    common_str_interface, 
	    "Establish priorities for packets from a named interface",
	    PRIV_CONF);


/************************************************************************
 * priority-list <list> default <queue-keyword>
 * no priority-list <list> default [<queue-keyword>]
 *
 * OBJ(int,1) = <list>
 * OBJ(int,2) = PRICMD_DEFAULT
 * OBJ(int,3) is unused
 * OBJ(int,4) = PRIORITY_HIGH, PRIORITY_MEDIUM, PRIORITY_NORMAL, PRIORITY_LOW
 *
 * If we parse the 'default' keyword, then check for NVGEN, and if
 * parsing, branch to gather the <queue-keyword>.
 */

NOPREFIX(prilist_def_noprefix, prilist_high, prilist_eol);
NVGENS	(prilist_def_nvgen, prilist_def_noprefix, 
	 prioritylist_command, FALSE);
KEYWORD_ID (prilist_default, prilist_def_nvgen, prilist_interface,
	    OBJ(int,2), PRICMD_DEFAULT,
	    "default", "Set priority queue for unspecified datagrams",
	    PRIV_CONF);


/*************************************************************************/
/*
 * List number
 */

NUMBER	(prilist_num, prilist_default, no_alt,
	 OBJ(int, 1), 1, PRIORITY_MAXLIST-1, "Priority list number");

/*
 * Mark this as priority queueing.
 */

NVGENS	(prilist_nvgen, prilist_num, prioritylist_command, FALSE);

KEYWORD_ID(config_prioritylist, prilist_nvgen, ALTERNATE,
	   OBJ(int, 16), QUEUE_PRIORITY,
	   "priority-list", "Build a priority list", PRIV_CONF);

#undef	ALTERNATE
#define	ALTERNATE	config_prioritylist

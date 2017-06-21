/*
 * $Id: parser.h,v 3.17.4.18 1996/09/14 00:03:17 sseal Exp $
 * $Source: /release/112/cvs/Xsys/h/parser.h,v $
 *------------------------------------------------------------------
 * 
 *	P A R S E R . H
 *
 * Parser structures, definitions, and externals.
 *
 * Copyright (c) 1985-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: parser.h,v $
 * Revision 3.17.4.18  1996/09/14  00:03:17  sseal
 * CSCdi61196:  ipx route-cache options need clean up
 * Branch: California_branch
 *
 * Revision 3.17.4.17  1996/09/05  13:59:39  widmer
 * CSCdi67994:  Remove names from parser link points
 * Branch: California_branch
 *
 * Revision 3.17.4.16  1996/07/12  02:01:09  widmer
 * CSCdi52582:  parser should remove the trailing spaces in the filename
 * Branch: California_branch
 *
 * Revision 3.17.4.15  1996/07/03  01:58:53  chifei
 * CSCdi58333:  Some frame relay SVC commands are available in
 * non-enterprise images
 * Branch: California_branch
 * Modularized "frame svc" and "frame lapf" commands for FR SVC subsystem
 *
 * Revision 3.17.4.14  1996/06/17  23:30:25  hampton
 * Implement the long sought parser "default" keyword.  [CSCdi60510]
 * Branch: California_branch
 *
 * Revision 3.17.4.13  1996/06/16  21:12:06  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 3.17.4.12  1996/06/03  23:22:54  ahh
 * CSCdi59119:  IP: NAT should support named access-lists
 * And extended access-lists and non-contiguous address pools.
 * Branch: California_branch
 *
 * Revision 3.17.4.11  1996/05/30  22:44:07  hampton
 * Add parser support for accepting per-protocol access list number
 * ranges.  [CSCdi59067]
 * Branch: California_branch
 *
 * Revision 3.17.4.10  1996/05/29  21:07:03  rzagst
 * CSCdi58657:  map-class command broken
 * Branch: California_branch
 *
 * Revision 3.17.4.9  1996/05/21  06:27:49  mordock
 * Branch: California_branch
 * Improve snmp modularity via creating services to register interfaces
 * and chassis cards.
 * Add syslog mib.
 *
 * Revision 3.17.4.8  1996/05/17  11:16:04  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.16.2.6  1996/05/05  23:05:31  ppearce
 * Sync to IbuMod_Calif_baseline_960504
 *
 * Revision 3.16.2.5  1996/04/26  07:52:56  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.16.2.4  1996/04/19  02:39:47  lmetzger
 * Branch: IbuMod_Calif_branch
 *
 * Updating Initial Version of Response Time Reporter
 *
 * Revision 3.16.2.3  1996/04/03  14:25:55  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.16.2.2  1996/03/22  19:08:01  lmetzger
 * Initial Version of Response Time Reporter
 *
 * Revision 3.16.2.1  1996/03/17  17:48:03  ppearce
 * Sync IbuMod_Calif_branch to V111_1_3
 *
 * Revision 3.17.4.7  1996/05/15  05:31:37  fowler
 * Frame relay VC queuing and rate enforcement
 * Branch: California_branch
 *
 * Revision 3.17.4.6  1996/05/14  06:29:02  achopra
 * CSCdi47186:  Implement clear atm arp command.
 * Branch: California_branch
 *
 * Revision 3.17.4.5  1996/05/09  14:13:30  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.17.4.4  1996/05/04  00:42:29  wilber
 * Branch: California_branch
 *
 * Integrated Routing and Bridging
 * - Rip out MTR transparent bridging support.
 * - New interafce type: BVI(Bridge-group Virtual Interface).
 * - New fast-switching flavor: FS_BVI.
 * - Introduce idb->tbridge_bridge_linktypes, idb->user_configured_mtu and
 *   pak->irb_rcvd_input.
 *
 * Constrained Multicast Flooding
 * - Declare tbifd_type.
 * - Define the CMF parser link points.
 * - Declare the return_one stub function.
 * - Declare the Transparent Bridging registry.
 * - Add tbridge_cmf to the fastswitch registry.
 *
 * Name Access List
 *
 * Revision 3.17.4.3.2.1  1996/04/27  06:35:29  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.17.4.3  1996/04/25  23:11:16  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.17.4.2  1996/04/15  21:13:36  bcole
 * CSCdi54649:  IP Host Modularity, IP Automatic Stub Routing feature
 * commit
 * Branch: California_branch
 *
 * Revision 3.17.4.1  1996/04/02  05:36:15  dkatz
 * CSCdi36031:  Running out of memory running 2 or more EIGRP ASs
 * Branch: California_branch
 * EIGRP Jumbo Patch.
 *
 * Revision 3.17.20.1  1996/04/08  01:46:02  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.17.10.2  1996/03/25  02:19:09  cakyol
 * sync LE_Syn_Cal_V111_1_3_branch to LE_Cal_V111_1_0_3_branch
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.17.10.1  1996/03/22  09:35:48  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.14.2.2  1996/03/22  03:15:35  bbenson
 * Branch: LE_Cal_V111_1_0_3_branch
 * Rearrange cadillac fins on the VTP command parser.  Install
 * electroglide subcommand modes for vtp-domain and vtp-database commands.
 * Running lights coming in next commit.
 *
 * Revision 3.14.2.1  1996/02/27  20:41:24  cakyol
 * non sync sync of LE_Cal_V111_0_16_branch to V111_1_0_3 yielding
 * LE_Cal_V111_1_0_3_branch
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.14  1996/02/09  07:57:10  jjohnson
 * CSCdi48524:  Need mechanism to display RMON tables
 * parse chains are in place.  checkpoint work so that anke can take over.
 *
 * Revision 3.13  1996/02/07  16:11:12  widmer
 * CSCdi48285:  Remove function name from bad_parser_subcommand
 *
 * Revision 3.12  1996/01/12  20:04:04  sjackson
 * CSCdi44704:  modem busyout command is needed
 * Command added as an extension of the line modem autoconfigure
 *
 * Revision 3.11.4.2  1996/01/11  19:51:59  cakyol
 * non sync sync now complete
 *
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.11.4.1  1996/01/11  01:45:01  cakyol
 * non sync sync of LE_Calif_branch to V111_0_16 yielding
 *     LE_Cal_V111_0_16_branch
 *
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.11  1995/12/17  18:26:45  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.10  1995/12/16  00:13:46  sdurham
 * CSCdi44645:  MAC illegal address trap needs snmp-server host and enable
 * cli support.
 *    moved all parser commands related to mib to appropriate mib
 *    subsystem.
 *
 * Revision 3.9  1995/12/09  21:57:21  mbeesley
 * CSCdi45202:  VIP/PCI parse chains need slight reorganization
 *
 * Revision 3.8.4.1  1995/12/23  03:58:52  dcheng
 * Branch: LE_Calif_branch
 * Added support for VTP.
 *
 * Revision 3.8  1995/12/03  20:59:59  widmer
 * CSCdi44243:  privilege exec level 0 help doesnt show up in config
 * Fix typo
 *
 * Revision 3.7  1995/12/03  16:42:11  widmer
 * CSCdi44243:  privilege exec level 0 help doesnt show up in config
 * Add sense to parsenode
 * Change help command to privilege level 0
 *
 * Revision 3.6  1995/12/01  15:56:11  widmer
 * CSCdi44724:  Remove nv_top argument to parser_add_mode()
 * Clean up "no" handling at beginning of modes
 * Fix appn modes
 * Removed unneded pdecl's
 *
 * Revision 3.5  1995/11/28  20:26:17  widmer
 * CSCdi44264:  Change save_line() to use variable-length arguments
 * Remove dead code also.
 *
 * Revision 3.4  1995/11/27  16:23:36  widmer
 * CSCdi33598:  WAN commands (eg X.25) in parser even when Subsystem not
 * included
 * Create parser link for interface encapsulation types
 *
 * Revision 3.3  1995/11/17  09:13:13  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:24:28  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:35:56  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.11  1995/11/08  20:58:57  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.10  1995/10/18  17:14:09  wmay
 * CSCdi39845:  show interface displays serials in a non-intuitive manner
 *
 * Revision 2.9  1995/09/27  21:53:11  izhu
 * CSCdi41172:  7000 dir/show flash command not consistant w/ 7500
 *
 * Revision 2.5  1995/07/12  07:18:16  tli
 * Revision 2.8  1995/09/25  08:34:25  tli
 * CSCdi40960:  ISP jumbo patch 2
 *
 * Revision 2.7  1995/08/17  01:42:44  tli
 * CSCdi38878:  DHCP: change ip local-pool to ip local pool
 *
 * Revision 2.6  1995/08/03  21:51:51  hampton
 * Convert the parser to use the chunk manager.  This should allow the
 * parser to continue to work in low memory situations.  [CSCdi38208]
 *
 * Revision 2.5  1995/07/12 07:18:16  tli
 * CSCdi36426:  Policy routing will not disable on an interface
 * - Policy routed packets were creating fastswitching cache entries.
 *   That's not till _next_ release.
 * - Add counters to record packet and byte count on a per route map entry
 *   basis.
 * - Marketing identified a catastrophic deficiency.  Implemented "set ip
 *   tos" and "set ip precedence" to cover this deficiency.
 *
 * Revision 2.4  1995/06/30  20:24:36  rchandra
 * CSCdi36620:  BGP requires IBGP neighbors to be fully meshed
 *
 * Revision 2.3  1995/06/19  21:28:40  dstine
 * CSCdi36040:  fix parser.h to be easily extensible
 *
 * Revision 2.2  1995/06/15  15:18:22  widmer
 * CSCdi35889:  parser transition structures should be static
 *
 * Revision 2.1  1995/06/07  20:36:16  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __PARSER_H__
#define __PARSER_H__

#define pname(x) PARSER_ ## x
#define pdecl(x) extern transition pname(x)
#define	lpdecl(x) extern dynamic_transition pname(x);

typedef struct parsenode_ {
  struct parsenode_ *next;	/* Next stack entry */
  transition *node;		/* The node */
  int line_index;		/* Where we are in the input buffer */
  char nv_command[PARSEBUF];	/* The command generated to this point */
  struct idbtype_ *idb1;	/* An interface */
  struct cdbtype_ *cdb1;	/* A controller */
  void *pdb1;			/* Routing protocol */
  void *udb1, *udb2;		/* User defined data block for generic sub commands */
  route_maptype *routemap1;	/* Route-map */
  uint val1;			/* Used for Terminal line numbers */
  void *appn_object_verb;       /* for appn config commands */
  uint appn_object_type;        /* for appn config commands */
  char *sp1;			/* String pointer (used for router NVGEN) */
  char visible_ambig[PARSEBUF];	/* Visible command ambiguity detection */
  char hidden_ambig[PARSEBUF];	/* Hidden command Ambiguity detection */
  boolean command_visible;	/* TRUE if current keyword is visible */
  boolean unsupported;		/* TRUE if current keyword is unsupp. */
  parser_mode *pmp;		/* Parser mode */
  boolean sense;		/* FALSE if 'no' parsed.  Also FALSE if 'default'
				 * parsed because the off state is the default for
				 * most commands. */
  boolean set_to_default;	/* TRUE if 'default' parsed */
} parsenode;

/*
 * ID and STRing type, used for lookup of the string, given its ID.
 * Arrays of these items are typically used in access lists.  The
 * last item in the array should have 'str = NULL'.
 */
typedef struct id_string_ {
    ulong id;
    char *str;
} id_string;

typedef struct id_subid_string_ {
    ulong id;
    ulong subid;
    char *str;
} id_subid_string;

pdecl(NONE);
pdecl(no_alt);
pdecl(link_point);
pdecl(link_trans);

extern chunk_type *parseinfo_chunks;
extern parseinfo *get_csb(const char *);
extern void free_csb(parseinfo **csb);
struct idbtype_ *findidb(parseinfo *csb);
extern void set_multiple_funcs(parseinfo *csb, int value);
extern void increment_multiple_funcs(parseinfo *csb, char *str);
extern int test_multiple_funcs(parseinfo *csb, char test, int value);
extern int test_visible_multiple_funcs(parseinfo *csb, char test, int value);
extern int test_invisible_multiple_funcs(parseinfo *csb, char test, int value);
extern void print_multiple_funcs(parseinfo *csb);
extern void report_error(parseinfo *, int, boolean);

/* Return values from parse_cmd() used by report_error() */
#define PARSE_ERROR_NOERR     0x0  /* Success */
#define PARSE_ERROR_AMBIG     0x1  /* Ambiguous command */
#define PARSE_ERROR_NOMATCH   0x2  /* True error; didn't match any command */
#define PARSE_ERROR_UNKNOWN   0x4  /* Didn't match anything while doing help */
#define	PARSE_ERROR_ALIAS     0x8  /* Didn't match expanded alias */
#define	PARSE_ERROR_LOCKED    0x10 /* Config was locked down too long */

#define NON_UNIQUE	1		/* Non-unique token in help */
#define PARSE_FAIL	2		/* Parse failed */

#define CSBOFF(elem) ((int) &((parseinfo*)NULL)->elem)
#define CSBVAR(csb, offset, type)  ((type *) ((u_char*) csb + offset))

/*
 *  Values for csb->in_help
 */
#define	PARSER_NO_HELP			0
#define	PARSER_HELP			1<<0
#define	PARSER_COMMAND_COMPLETION	1<<1

/*
 * Values for mode volume
 */
#define	MODE_VERBOSE	1
#define	MODE_SILENT	2

/*
 * Type of parser
 */
#define PARSER_CLI_DEFAULT		0
#define PARSER_CLI_TCL			1
#define PARSER_CLI_OVERRIDE		2

/*
 * Extending the parser.
 * Some brave soul should turn this into an enum and rearrange things
 * intelligently. 
 */
enum {
    PARSE_LIST_END=0,			/* End of list - not a real code */
    PARSE_ADD_EXEC_CMD,
    PARSE_ADD_SHOW_CMD,
    PARSE_ADD_CLEAR_CMD,
    PARSE_ADD_DEBUG_CMD,
    PARSE_ADD_DEBUG_INT_ATM_CMD,
    PARSE_ADD_TERM_CMD,			/* "term xxx" commands, not saved */
    PARSE_ADD_TEST_CMD,

    PARSE_ADD_CFG_TOP_CMD,		/* top level config commands */
    PARSE_ADD_CFG_INT_CMD,		/* interface */
    PARSE_ADD_CFG_INT_AS_CMD,		/* interface, after serial stuff */
    PARSE_ADD_CFG_LINE_CMD,		/* line commands */
    PARSE_ADD_CFG_ROUTER_CMD,		/* router commands */
    PARSE_ADD_CFG_ROUTING_CMD,		/* routing commands */
    PARSE_ADD_LINE_TERM_CMD,		/* shared "term xxx" & line commands */
    PARSE_ADD_CFG_ACCLIST_CMD,		/* access list commands */
    PARSE_ADD_CFG_ROUTER_COMMAND,	/* the actual "router" command */
    PARSE_ADD_CFG_XNS_ROUTING_CMD,	/* xns routing commands */
    PARSE_ADD_CFG_NOVELL_ROUTING_CMD,	/* novell routing commands */
    PARSE_ADD_CFG_NOVELL_ROUTER_CMD,	/* novell router commands */
	
    PARSE_ADD_CLEAR_IP_CMD,		/* ip clear commands */
    PARSE_ADD_DEBUG_IP_CMD,		/* ip debug commands */
    PARSE_ADD_TEST_IP_CMD,		/* ip test commands */
    PARSE_ADD_SHOW_IP_CMD,		/* ip show commands */
    PARSE_ADD_CFG_TOP_IP_CMD,		/* ip config commands */
    PARSE_ADD_CFG_INT_IP_CMD,		/* ip interface commands */
    PARSE_ADD_CFG_RTG_IP_CMD,		/* ip routing commands */
    PARSE_ADD_GLOBAL_CMD,		/* global commands */
    PARSE_ADD_SHOW_CONTROLLER_CMD,	/* show controller commands */
    PARSE_ADD_FREE26,
    PARSE_ADD_CFG_LINE_MODEM_CMD,	/* config line modem commands */
    PARSE_ADD_CFG_LINE_MODEM_AUTOCONFIGURE_CMD, /* line modem autoconfigure */
    PARSE_ADD_CFG_LINE_MODEM_BUSYOUT_CMD,	/* line modem busyout */
    PARSE_ADD_PING_CMD,			/* exec ping commands */
    PARSE_ADD_COPY_CMD,			/* copy commands */
    PARSE_ADD_COPYFILE_CMD,		/* copy file commands */

    PARSE_ADD_ROUTING_IP_CMD,		/* the ip routing command */
    PARSE_ADD_INTERFACE_IP1_CMD,	/* the ip interface command */
    PARSE_ADD_INTERFACE_IP2_CMD,	/* the ip2 interface command */
    PARSE_ADD_CFG_LAST_CMD,		/* top level config commands at end */

    PARSE_ADD_CFG_NBIOS_CMD,		/* netbios config commands */
    PARSE_ADD_CFG_INT_NBIOS_CMD,	/* netbios interface commands */
    PARSE_ADD_CFG_INT_BRIDGE,		/* bridge-group int commands */
    PARSE_ADD_CFG_DECNET,		/* decnet config commands */
    PARSE_ADD_CFG_DECNET_ROUTING,	/* decnet routing config commands */
    PARSE_ADD_NULLINT_CMD,		/* null int commands */
    PARSE_ADD_CFG_INT_FDDI_CMD,		/* fddi interface config commands */
    PARSE_ADD_CFG_CONTROLLER_CMD,	/* controller commands */
    PARSE_ADD_CFG_CTRLLR_CMD,		/* controller command */
    PARSE_ADD_CFG_BEFORE_RTR_CMD,	/* config command before rtr cmd */
    PARSE_ADD_SHOW_IPX_CMD,		/* novell show commands */
    PARSE_ADD_SHOW_BRIDGE_CMD,		/* bridging show commands */
    PARSE_ADD_SHOW_CLNS_CMD,		/* clns show commands */
    PARSE_ADD_CFG_MAPLIST,		/* map-list command */
    PARSE_ADD_CFG_MAPLIST_CMD,		/* static map list config commands */
    PARSE_ADD_CFG_MAPCLASS,		/* map-class command */
    PARSE_ADD_CFG_MAPCLASS_CMD,		/* static map class config commands */
    PARSE_ADD_CFG_MAPCLASS_VCQ_CMD,	/* static map class fr VCQ commands */
    PARSE_ADD_CFG_MAPCLASS_SVC_CMD,	/* static map class fr SVC commands */
    PARSE_ADD_CFG_STATIC_MAP,		/* static map config commands */
    PARSE_ADD_CLEAR_IPX_CMD,		/* novell clear commands */
    PARSE_ADD_CFG_SNMP_CMD,		/* snmp config commands */
    PARSE_ADD_CFG_RMON_CMD,		/* rmon config commands */
    PARSE_ADD_SHOW_RMON_CMD,		/* rmon show command */
    PARSE_ADD_SHOW_RMON_RET,		/* return for show rmon command */
    PARSE_ADD_CFG_TRANS_IN_CMD,
    PARSE_ADD_CFG_TRANS_IN_RETURN,
    PARSE_ADD_CFG_TRANS_OUT_CMD,
    PARSE_ADD_CFG_TRANS_OUT_RETURN,
    PARSE_ADD_SHOW_SRB_CMD,		/* show source-bridge commands */
    PARSE_ADD_CFG_TOP_AC_CMD,		/* config commands after controllers */
    PARSE_ADD_CFG_SERVICE_CMD,		/* additional "service" commands */
    PARSE_ADD_CLEAR_ATALK_CMD,		/* atalk clear commands */
    PARSE_ADD_DEBUG_ATALK_CMD,		/* atalk debug commands */
    PARSE_ADD_CFG_INT_ATALK_CMD,	/* atalk interface commands */
    PARSE_ADD_CFG_TOP_ATALK_CMD,	/* atalk ip config commands */
    PARSE_ADD_SHOW_ATALK_CMD,		/* atalk show commands */
    PARSE_ADD_CFG_TOP_ATALK_ROUTING_CMD,
    PARSE_ADD_CFG_ATALK_ROUTING_CMD,
    PARSE_ADD_ATALK_PROTOCOL_CMD,
    PARSE_TCL_INCOMPLETE_COMMAND,	/* tcl command is incomplete */
    PARSE_ADD_CFG_INT_IP2_CMD,		/* ip2 interface commands */
    PARSE_ADD_SHOW_EXTENDED_CMD,	/* show extended commands */
    PARSE_ADD_FREE71,
    PARSE_ADD_CFG_TOP_SRB_CMD,		/* source bridge commands */
    PARSE_ADD_CFG_PT_SLIP_PPP_CMD,	/* global vasync SLIP/PPP commands */
    PARSE_ADD_CFG_AFTER_SRB_CMD,	/* Add config commands after SRB   */
    PARSE_ADD_PRIORITY_OPTIONS,
    PARSE_ADD_FR_DELIST_OPTIONS,
    PARSE_ADD_FR_DELIST_END_OPTIONS,
    PARSE_ADD_CFG_INT_INARP_CMD,
    PARSE_ADD_CFG_INT_SVC_CMD,		/* 'fr svc' */
    PARSE_ADD_CFG_INT_LAPF_CMD,		/* fr lapf commands */
    PARSE_ADD_CFG_INT_VCQ_CMD,    
    PARSE_ADD_CFG_FRCLASS_CMD,
    PARSE_ADD_CFG_CDP,			/* Cisco Discovery Protocol */
    PARSE_ADD_CFG_HUB_CMD,		/* Hub config commands */
    PARSE_ADD_DEBUG_IPX_CMD,		/* ipx debug commands */
    PARSE_ADD_CFG_INT_IPX_CMD,		/* ipx interface commands */
    PARSE_ADD_CFG_TOP_IPX_CMD,		/* ipx global config commands */
    PARSE_ADD_ROUTER_IPX_CMD,		/* ipx router commands */
    PARSE_ADD_TEST_SSE_CMD,		/* telnet exit point */
    PARSE_ADD_TELNET_EXIT_CMD,
    PARSE_ADD_WRITE_CMD,		/* to add dynamic write commands */
    PARSE_ADD_CFG_BOOT_SYS_CMD,		/* to add boot sys sub-commands */
    PARSE_ADD_CFG_BOOT_CONFIG_CMD,	/* to add boot config commands */
    PARSE_ADD_CFG_FILE_CMD,		/* to add config file command */
    PARSE_ADD_COPY_RUNNING_CMD,		/* extending copy running-config */
    PARSE_ADD_COPY_STARTUP_CMD,		/* extending copy startup-config */
    PARSE_ADD_COPY_FLASH_CMD,		/* extending copy flash */
    PARSE_ADD_COPY_BFLASH_CMD,		/* extending copy bootflash */
    PARSE_ADD_COPY_RCP_CMD,		/* extending copy rcp */
    PARSE_ADD_COPY_MOP_CMD,		/* extending copy mop cmd */
    PARSE_ADD_COPY_TFTP_CMD,		/* extending copy tftp cmd */
    PARSE_ADD_COPY_RUNNING_FILE_CMD,	/* extending copy running-config */
    PARSE_ADD_COPY_STARTUP_FILE_CMD,	/* extending copy startup-config */
    PARSE_ADD_COPY_FLASH_FILE_CMD,	/* extending copy flash */
    PARSE_ADD_COPY_BFLASH_FILE_CMD,	/* extending copy bootflash */
    PARSE_ADD_COPY_RCP_FILE_CMD,	/* extending copy rcp */
    PARSE_ADD_COPY_MOP_FILE_CMD,	/* extending copy mop cmd */
    PARSE_ADD_COPY_TFTP_FILE_CMD,	/* extending copy tftp cmd */
    PARSE_ADD_VERIFY_FILE_CMD,		/* to add verify file cmd */
    PARSE_ADD_ERASE_FILE_CMD,		/* to add erase file cmd */
    PARSE_ADD_CFG_SCHED_CMD,		/* to add "scheduler" subcmd */
    PARSE_ADD_SHOW_FLASH_FILE_CMD,	/* to add new show flash cmd */
    PARSE_ADD_CFG_ROUTER_NEIGHBOR_CMD,	/* neighbor command extensions */
    PARSE_ADD_CFG_ROUTER_TIMER_CMD,	/* timers command extensions */
    PARSE_ADD_CFG_ROUTER_DISTANCE_CMD,
    PARSE_ADD_CFG_SNMP_HOST_CMD,	/* snmp host config command */
    PARSE_ADD_CFG_SNMP_HOST_EXIT,	/* return for snmp host cfg cmd */
    PARSE_ADD_CFG_LOG_CMD,           /* syslog history config cmd */
    PARSE_ADD_SHOW_LOG_CMD,         /* syslog history show cmd */
    PARSE_ADD_CFG_TACACS,		/* tacacs config commands */
    PARSE_ADD_CFG_INT_ATM_CMD,		/* ATM interface config commands */
    PARSE_ADD_CFG_INT_PVC_INARP_CMD,    /* ATM inarp command */
    PARSE_ADD_CFG_INT_PVC_INARP_EXIT,   /* ATM inarp command return */
    PARSE_ADD_CLEAR_INT_ATM_CMD,	/* ATM clear command */
    PARSE_ADD_SHOW_INT_ATM_CMD,         /* ATM interface show commands */
    PARSE_ADD_CFG_INT_IPROUTEC_CMD,	/* IP route-cache int command */
    PARSE_ADD_CFG_INT_IPXROUTEC_CMD,	/* IPX route-cache int command */
    PARSE_ADD_CFG_INT_IPROUTEC_EXIT,	/* IP route-cache int exit */
    PARSE_ADD_CFG_INT_IPXROUTEC_EXIT,	/* IPX route-cache int exit */
    PARSE_ADD_CFG_INT_IP_SPLIT_CMD,	/* IP split horizon extensions */
    PARSE_ADD_CFG_SNMP_ENABLE_CMD,	/* snmp enable config command */
    PARSE_ADD_CFG_SNMP_ENABLE_EXIT,	/* return for snmp enable cfg cmd */
    PARSE_ADD_EXEC_CONNECT_CMD,		/* connect/telnet options */
    PARSE_ADD_CFG_ROUTER_INITIAL_CMD,   /* to add initial router cmd */
    PARSE_ADD_EXEC_CONNECT_EXIT,	/* connect/telnet return point */
    PARSE_ADD_CFG_INT_AS2_CMD,		/* Next after ADD_CFG_INT_AS_CMD */
    PARSE_ADD_CFG_ROUTEMAP_SET_CMD,	/* route map set */
    PARSE_ADD_CFG_ROUTEMAP_MATCH_CMD,	/* route map match */
    PARSE_ADD_CFG_ROUTEMAP_SET_IP_CMD,	/* route map set ip */
    PARSE_ADD_CFG_CFGTBL_CMD,		/* ATM Lane Lecs commands */
    PARSE_ADD_PING_EXIT,		/* Ping command exit */
    PARSE_ADD_GLOBAL_EXIT,		/* Global command exit */
    PARSE_ADD_TEST_PAS_CMD,             /* pas test commands */
    PARSE_ADD_SHOW_PAS_CMD,             /* pas show commands */
    PARSE_ADD_SHOW_PCI_CMD,             /* pci show commands */
    PARSE_ADD_TELNET_PORT_EXIT,
    PARSE_ADD_RLOGIN_EOL_EXIT,
    PARSE_ADD_CFG_RTMAP_SET_EXIT,
    PARSE_ADD_CFG_RTMAP_MATCH_EXIT,
    PARSE_ADD_CFG_IP_LOCAL_CMD,		/* ip local commands */
    PARSE_ADD_SHOW_IP_LOCAL_CMD,	/* show ip local commands */
    PARSE_ADD_CFG_INT_ISDN_CMD,
    PARSE_ADD_CFG_RTTMON_ENTRY_CMD,     /* Response Time Reporter Entry Cfgs */
    PARSE_ADD_CFG_RTTMON_CMD,           /* Response Time Reporter Base Cfgs */
    PARSE_ADD_CFG_EXCEP_PROTO_CMD,	/* exception protocol command */
    PARSE_ADD_CFG_EXCEP_PROTO_EXIT,	/* more exception protocol */
    PARSE_ADD_DEBUG_TCP_CMD,		/* debug ip tcp xyz command */
    PARSE_ADD_INT_ENCAPS_CMD,		/* encaps interface command */
    PARSE_ADD_INT_ENCAPS_EXIT,		/* encaps interface cmd exit */
    PARSE_ADD_SHOW_FLASH_FILESYS_CMD,	/* add filesys option for 7000
					   show flash command */
    PARSE_ADD_CFG_TARP,
    PARSE_ADD_CFG_VLAN,                 /* Virtual LANs */
    PARSE_ADD_CFG_HUB_PORT_CMD,		/* port config command for
					  hublets */
    PARSE_ADD_CFG_SNMP_TRAP_FREQ_CMD,	/* snmp trap freq config command */

    PARSE_ADD_CFG_SNMP_TRAP_FREQ_EXIT,	/* return for snmp trap freq cfg cmd */

    PARSE_ADD_CFG_BRIDGE_CMF_CMD,	/* Constrained Multicast Flooding */
    PARSE_ADD_CFG_BRIDGE_CMF_EXIT,
    PARSE_ADD_DEBUG_TBRIDGE_CMF_CMD,
    PARSE_ADD_SHOW_BRIDGE_CMF_CMD,
    PARSE_ADD_SHOW_BRIDGE_CMF_EXIT,
    PARSE_ADD_CLEAR_BRIDGE_CMF_CMD,
    PARSE_ADD_CLEAR_BRIDGE_CMF_EXIT,

    PARSE_ADD_CFG_VTP_DATABASE_CMD,	/* define VLAN Trunk Proto info */
    PARSE_ADD_CFG_VTP_DOMAIN_CMD,	/* domains for VTP */
    PARSE_ADD_DEBUG_MODEM_CMD,          /* debug modem comands */

    PARSE_ADD_SHOW_TECHSUPPORT_CMD,	/* show tech-support commands */
    PARSE_ADD_SHOW_TECHSUPPORT_EXIT	/* show tech-support exit */

};


#define	PARSER_ENTRY_LINK_POINT	1
#define	PARSER_EXIT_LINK_POINT	2

/*
 * Uncomment this define to add parser link point names for debugging
 */
/*#define	PARSER_DEBUG_LINKS*/

typedef struct chain_link_ {
    uint id;
#ifdef	PARSER_DEBUG_LINKS
    const char *name;
#endif	/* PARSER_DEBUG_LINKS */
    dynamic_transition *link_point;
    uint type;
} chain_link;

typedef struct chain_list_ {
    struct chain_list_ *next;
    chain_link cp;
} chain_list;

extern chain_link chain_gang[];

/*
 * List structure for parser extension
 */
typedef const struct {
    uint type;			/* Type code */
    dynamic_transition *commands;	/* Transition pointer */
} parser_extension_request;

#ifdef	PARSER_DEBUG_LINKS
#define	parser_add_commands(def, trans, str)\
        parser_add_commands_func(def, trans, str)
#define	parser_add_command_list(list, str)\
        parser_add_command_list_func(list, str)
#define	parser_add_link_point(def, str, trans)\
        parser_add_link_point_func(def, str, trans)
#define	parser_add_link_exit(def, str, trans)\
        parser_add_link_exit_func(def, str, trans)

extern boolean parser_add_commands_func(uint, dynamic_transition *, const char *);
extern boolean parser_add_command_list_func(parser_extension_request *,
					    const char *);
extern boolean parser_add_link_point_func(uint, const char *, transition *);
extern boolean parser_add_link_exit_func(uint, const char *, transition *);
#else	/* PARSER_DEBUG_LINKS */
#define	parser_add_commands(def, trans, str)\
        parser_add_commands_func(def, trans)
#define	parser_add_command_list(list, str)\
        parser_add_command_list_func(list)
#define	parser_add_link_point(def, str, trans)\
        parser_add_link_point_func(def, trans)
#define	parser_add_link_exit(def, str, trans)\
        parser_add_link_exit_func(def, trans)

extern boolean parser_add_commands_func(uint, dynamic_transition *);
extern boolean parser_add_command_list_func(parser_extension_request *);
extern boolean parser_add_link_point_func(uint, transition *);
extern boolean parser_add_link_exit_func(uint, transition *);
#endif	/* PARSER_DEBUG_LINKS */


typedef void * (*mode_save_var_func)(parseinfo *);
typedef void (*mode_reset_var_func)(parseinfo *, void *);
typedef boolean (*mode_http_init_func)(parseinfo *);


extern boolean parser_link_point_exists(uint);

extern parser_mode *parser_add_mode(const char *, const char *, const char *,
				    boolean, boolean, const char *,
				    mode_save_var_func, mode_reset_var_func,
				    transition *,
				    mode_http_init_func);
extern parser_mode *get_mode_tmp(const char *, const char *, const char *,
				 boolean, boolean,
				 transition *);
extern void free_mode(parser_mode *);
extern parser_mode *get_alt_mode(parser_mode *);
extern void *mode_save_vars(parseinfo *);
extern void mode_reset_vars(parseinfo *, void *);
extern boolean priv_changed_in_mode(parser_mode *);
extern void set_priv_changed_in_mode(parser_mode *, boolean);
extern boolean mode_http_init(parseinfo *);

/*
 * The big 5 well known parser modes
 */
extern parser_mode *exec_mode;
extern parser_mode *config_mode;
extern parser_mode *interface_mode;
extern parser_mode *router_mode;
extern parser_mode *line_mode;

/*
 * And other modes which need to be referenced quickly
 */
extern parser_mode *subinterface_mode;
extern parser_mode *string_1_mode;

/*
 * Miscellaneous routine declarations.
 */
void parse_token(parseinfo *);
void push_node(parseinfo *, transition *);
void recurse_node(parseinfo *, transition *);
int parse_cmd(parseinfo *);
boolean batch_input(parseinfo *, char *, int, char *, char *);
void print_caret(int);
void save_line_init(save_buf*);
void save_line(parseinfo *, save_buf*, const char*, ...);
void print_buf(save_buf, boolean, boolean);
char *lookup_id_string(id_string const *, ulong);
void nv_add_id_string(id_string const *, ulong);
int printf_id_string(id_string const *, ulong);
char *lookup_id_subid_string(id_subid_string const *, ulong, ulong);
void nv_add_id_subid_string(id_subid_string const *, ulong, ulong);
int printf_id_subid_string(id_subid_string const *, ulong, ulong);
extern void bad_parser_subcommand(parseinfo *, uint);
char *last_config_time_string(void);
char *last_write_mem_time_string(void);
void note_write_mem_time(void);
void free_csb_mem(parseinfo *);

/*
 * Parser Mode functions
 */
extern void parser_modes_init(void);
extern parser_mode *get_mode_byname(const char *, uint);
extern boolean set_mode_byname(parser_mode **, const char *, uint);
extern queuetype *get_mode_aliases(parser_mode *);
extern queuetype *get_mode_defaliases(parser_mode *);
extern const char *get_mode_prompt(parseinfo *);
extern boolean test_mode_config_subcommand(parseinfo *);
extern transition *get_mode_nv(parseinfo *);
extern transition *get_mode_top(parser_mode *);
extern const char *get_mode_name(parser_mode *);
extern void display_mode_aliases(parseinfo *, parser_mode *);
extern void parser_add_main_modes(void);

/*
 * Alias functions
 */
extern void add_default_alias(parser_mode *, const char *, const char *);
extern void show_mode_aliases(parseinfo *, queuetype *, const char *);
extern void free_aliases(queuetype *);
extern boolean expand_alias(parseinfo *);

/*
 *  Global parser debug variables
 */
extern boolean parser_help_debug;
extern boolean parser_ambig_debug;
extern boolean parser_mode_debug;
extern boolean parser_alias_debug;
extern boolean parser_priv_debug;
extern boolean parser_http_debug;

/*
 * Should we have a prompt in config mode.
 */
extern boolean config_prompt;

#endif	/* __PARSER_H_ */

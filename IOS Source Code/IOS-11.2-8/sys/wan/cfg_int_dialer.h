/* $Id: cfg_int_dialer.h,v 3.7.4.4 1996/08/06 00:37:31 asb Exp $
 * $Source: /release/112/cvs/Xsys/wan/cfg_int_dialer.h,v $
 *------------------------------------------------------------------
 * C F G _ I N T _ D I A L E R . H
 *
 * Copyright (c) 1992-1997 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_int_dialer.h,v $
 * Revision 3.7.4.4  1996/08/06  00:37:31  asb
 * CSCdi40543:  MBRI dialer rotary cannot detect down BRI
 * Branch: California_branch
 *
 * Revision 3.7.4.3  1996/07/24  17:55:09  suresh
 * CSCdi62055:  Dialer profile error message misleading
 * Branch: California_branch
 * Code Cleanup for change from dialer user to dialer profile.
 *
 * Revision 3.7.4.2  1996/06/03  16:28:55  syiu
 * CSCdi59217:  Virtual template and dialer commands should be excluded
 * Branch: California_branch
 *
 * Revision 3.7.4.1  1996/04/27  06:35:40  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 3.5.8.4  1996/04/25  23:57:44  syiu
 * Branch: Dial1_branch
 * Dial1_branch to IosSec_postmerge_960419
 *
 * Revision 3.5.8.3  1996/04/23  00:24:01  suresh
 * Branch: Dial1_branch
 * Add Dialer Profiles support to Dial1_branch
 *
 * Revision 3.5.8.2  1996/04/16  23:59:41  syiu
 * Branch: Dial1_branch
 * Sync Dial1_branch to V111_1_3
 *
 * Revision 3.5.8.1  1996/02/23  21:31:09  lbustini
 * Branch: Dial1_branch
 * Move dialer fields out of the idb.
 *
 * Revision 3.7  1996/03/07  22:22:24  widmer
 * CSCdi50995:  help broken for dialer map command
 *
 * Revision 3.6  1996/02/26  06:53:12  dmccowan
 * CSCdi45288:  need appn configuration changes to support multilink ppp
 * Add configuration option for APPN over a dialer interface
 *
 * Revision 3.5  1995/12/22  02:31:22  suresh
 * CSCdi43120:  no dialer load-threshold requires a threshold number to
 * work
 * Fixed the parser to handle this problem
 *
 * Revision 3.4  1995/12/22  01:28:37  suresh
 * CSCdi29641:  Dialer dialer-group accepts 0 but does not work
 * Changed the dialer-group range to 1-10; cleaned code so that
 * dialer-list and dialer-group accesses are consistent
 *
 * Revision 3.3  1995/11/29  01:58:53  dblair
 * CSCdi41333:  DDR Callback and Multilink PPP clean up
 *
 * Revision 3.2  1995/11/17  18:00:59  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:47:57  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/09/30  05:06:03  dblair
 * CSCdi41333:  DDR Callback initial commit
 * Adding Callback feature for DDR interfaces.
 *
 * Revision 2.2  1995/09/20  22:38:41  dblair
 * CSCdi40745:  PPP Multilink feature commit
 *
 * Revision 2.1  1995/07/02  01:45:50  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.3  1995/06/21  18:16:38  widmer
 * CSCdi36227:  Fix parse chain alternates
 *
 * Revision 2.2  1995/06/21  03:12:44  gstovall
 * CSCdi35731:  Cleanup unused cruft in idb
 * Nuke some unused vectors and variables from the hwidb.
 *
 * Revision 2.1  1995/06/07  22:04:35  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 * dialer-group <num>
 * no dialer-group
 */
PARAMS_KEYONLY( cfg_int_dialergroup_kwd, NONE, "dialer-group",
	        OBJ(int,1), DIALLST_MINLIST, DIALLST_MAXLIST,
	        dialergroup_command, 0,
	        "Assign interface to dialer-list",
	        "Dialer list number", PRIV_CONF);
TEST_MULTIPLE_FUNCS(cfg_int_dialergroup, cfg_int_dialergroup_kwd, NONE, NONE);

/***************************************************************
 * no dialer
 */
EOLS	(int_dialer_no_eol, dialer_command, NOPREFIX_DIALER);
TESTVAR	(int_dialer_notest, NONE, int_dialer_no_eol,
	 NONE, NONE, NONE, sense, TRUE);
NVGENS	(int_dialer_nvgen, int_dialer_notest,
	 dialer_command, NOPREFIX_DIALER);
TEST_IDBSTATUS(int_dialer_no, NONE, int_dialer_nvgen, no_alt, IDB_ISDN);

/***************************************************************
 * dialer pool-member <num> [priority p] [min-link m] [max-link n]
 * no dialer pool-member
 */
EOLS	(int_dialer_pool_member_eol, dialer_command, DIALER_POOL_MEMBER);

pdecl(int_dialer_pool_member_options);

NUMBER	(int_dialer_pool_member_max_val, int_dialer_pool_member_options, no_alt,
	 OBJ(int,4), DIALER_MINPOOLMAXLINK, DIALER_MAXPOOLMAXLINK, 
	 "Number of B channels");
KEYWORD (int_dialer_pool_member_max, int_dialer_pool_member_max_val, no_alt, 
	 "max-link", "Maximum number of B channels for pool", PRIV_CONF);
NUMBER	(int_dialer_pool_member_min_val, int_dialer_pool_member_options, no_alt,
	 OBJ(int,3), DIALER_MINPOOLMINLINK, DIALER_MAXPOOLMINLINK, 
	 "Number of B channels");
KEYWORD (int_dialer_pool_member_min, int_dialer_pool_member_min_val, 
	 int_dialer_pool_member_max,
	 "min-link", "Minimum number of B channels for pool", PRIV_CONF);
ASSERT	(int_dialer_pool_member_isdn, int_dialer_pool_member_min,
	 NONE, is_isdn(csb->interface->hwptr));
NUMBER	(int_dialer_pool_member_pri_val, int_dialer_pool_member_options, no_alt,
	 OBJ(int,2), DIALER_MINPOOLPRIORITY, DIALER_MAXPOOLPRIORITY,
	 "Priority");
KEYWORD	(int_dialer_pool_member_pri, int_dialer_pool_member_pri_val, 
	 int_dialer_pool_member_isdn,
	 "priority", "Priority of interface in pool", PRIV_CONF);
NOP	(int_dialer_pool_member_options, int_dialer_pool_member_pri, 
	 int_dialer_pool_member_eol);
NOPREFIX(int_dialer_no_pool_member, int_dialer_pool_member_options, 
	 int_dialer_pool_member_eol);
NUMBER	(int_dialer_pool_member_num, int_dialer_no_pool_member, no_alt,
	 OBJ(int,1), DIALER_MINPOOL, DIALER_MAXPOOL, "Dialer pool number");
NVGENS	(int_dialer_pool_member_nvgen, int_dialer_pool_member_num, 
	 dialer_command, DIALER_POOL_MEMBER);
KEYWORD (int_dialer_pool_member_kwd, int_dialer_pool_member_nvgen, NONE,
	 "pool-member", "Specify dialer pool membership", PRIV_CONF);
ASSERT	(int_dialer_pool_member_test, int_dialer_pool_member_kwd, NONE,
	 (!dtr_dialer(csb->interface->hwptr) &&
	  !huntgroup_member(csb->interface->hwptr)));

/***************************************************************
 * dialer pool <num>
 * no dialer pool
 */
EOLS	(int_dialer_pool_eol, dialer_command, DIALER_POOL);
NUMBER	(int_dialer_pool_num, int_dialer_pool_eol, no_alt,
	 OBJ(int,1), DIALER_MINPOOL, DIALER_MAXPOOL, "Dialer pool number");
NVGENS	(int_dialer_pool_nvgen, int_dialer_pool_num, 
	 dialer_command, DIALER_POOL);
KEYWORD (int_dialer_pool_kwd, int_dialer_pool_nvgen, NONE,
	 "pool", "Specify dialer pool to be used", PRIV_CONF);
TEST_IDBSTATUS(int_dialer_pool, int_dialer_pool_kwd, 
	       int_dialer_pool_member_kwd, int_dialer_no, IDB_DIALER);

/***************************************************************
 * dialer max-call <num>
 * no dialer max-call
 */
PARAMS_KEYONLY  (int_dialer_max_call, no_alt,
                "max-call", OBJ(int,1), DIALER_MINCALL, DIALER_MAXCALL,
                dialer_command, DIALER_MAX_CALL,
                "Specify maximum number of calls",
                "Maximum number of calls", PRIV_CONF);
ASSERT	(int_dialer_max_call_test, int_dialer_max_call, int_dialer_pool,
	 is_profile(csb->interface->hwptr));

/***************************************************************
 * [no] dialer load-threshold <threshold> [ either | outbound | inbound ]
 *
 * GETOBJ(int,1) = <threshold>
 * GETOBJ(int,2) : balanced, outbound, inbound
 */
EOLS	(int_dialer_load_eol, dialer_command, DIALER_LOAD);
KEYWORD_ID(int_dialer_load_inbound, int_dialer_load_eol,
	   int_dialer_load_eol,
	   OBJ(int,2), DIALER_THRESH_INBOUND,
	   "inbound", 
	   "Threshold decision based on inbound traffic only", 
	   PRIV_CONF);
KEYWORD_ID(int_dialer_load_outbound, int_dialer_load_eol,
	   int_dialer_load_inbound,
	   OBJ(int,2), DIALER_THRESH_OUTBOUND,
	   "outbound", 
	   "Threshold decision based on outbound traffic only", 
	   PRIV_CONF);
KEYWORD_ID(int_dialer_load_balanced, int_dialer_load_eol,
	   int_dialer_load_outbound,
	   OBJ(int,2), DIALER_THRESH_EITHER,
	   "either", 
	   "Threshold decision based on max of inbound and outbound traffic", 
	   PRIV_CONF);

NUMBER	(int_dialer_load_size, int_dialer_load_balanced, no_alt,
	 OBJ(int,1), 1, 255, "Load threshold to place another call");
NOPREFIX (int_dialer_no_load, int_dialer_load_size, int_dialer_load_eol);
NVGENS	(int_dialer_load_nvgen, int_dialer_no_load, dialer_command, 
	 DIALER_LOAD);
KEYWORD (int_dialer_load, int_dialer_load_nvgen, NONE,
	 "load-threshold", 
	 "Specify threshold for placing additional calls", PRIV_CONF);

ASSERT (int_dialer_load_test, int_dialer_load, int_dialer_max_call_test,
        huntgroup_leader(csb->interface->hwptr));

/***************************************************************
 * [no] dialer priority <priority>
 *
 * GETOBJ(int,1) = <priority>
 */

PARAMS_KEYONLY  (int_dialer_priority, no_alt,
                "priority", OBJ(int,1), 0, 255,
                dialer_command, DIALER_PRIORITY,
                "Specify priority for use in dialer group",
                "Priority for dialing use", PRIV_CONF);

ASSERT        (int_dialer_priority_test, int_dialer_priority, 
	       int_dialer_load_test, !is_profile(csb->interface->hwptr));

/***************************************************************
 * [no] dialer hold-queue <size> [ timeout <num> ]
 *
 * GETOBJ(int,1) = <size>
 * GETOBJ(int,2) = <num>
 */
EOLS	(int_dialer_holdq_eol, dialer_command, DIALER_HOLDQ);
NUMBER	(int_dialer_holdq_timeout_length, int_dialer_holdq_eol, no_alt,
	 OBJ(int,2), 1, DIALER_MAX_TIMER, "Holdq timeout in seconds is this value + enable time");
KEYWORD (int_dialer_holdq_timeout, int_dialer_holdq_timeout_length,
	 int_dialer_holdq_eol, "timeout",
	 "Specify length of time data is held while interface is connecting",
	 PRIV_CONF);
NUMBER	(int_dialer_holdq_size, int_dialer_holdq_timeout, int_dialer_holdq_eol,
	 OBJ(int,1), 0, DIALER_MAX_NUM_HOLDQ,
	 "Specify size of output hold queue");
NVGENS	(int_dialer_holdq_nvgen, int_dialer_holdq_size, dialer_command, 
	 DIALER_HOLDQ);

KEYWORD (int_dialer_holdq, int_dialer_holdq_nvgen, int_dialer_priority_test,
         "hold-queue", "Configure output hold queue", PRIV_CONF);

/***************************************************************
 * [no] dialer rotary-group <group>
 *
 * GETOBJ(int,1) = <group>
 */
EOLS	(int_dialer_rotary_eol, dialer_command, DIALER_HUNT);
NUMBER	(int_dialer_rotary_grp, int_dialer_rotary_eol, no_alt,
	 OBJ(int,1), 0, 255, "Rotary group number");
NVGENS	(int_dialer_rotary_nvgen, int_dialer_rotary_grp, dialer_command, 
	 DIALER_HUNT);

KEYWORD (int_dialer_rotary, int_dialer_rotary_nvgen, NONE,
         "rotary-group", "Add to a dialer rotary group", PRIV_CONF);
ASSERT  (int_dialer_rotary_test, int_dialer_rotary, NONE,
	 !dialpool_member(csb->interface->hwptr));
TEST_IDBSTATUS(int_dialer_assert_rotary, NONE, int_dialer_rotary_test,
	       int_dialer_holdq, IDB_DIALER);

/***************************************************************
 * [no] dialer map <mapaddr>
 *	    [ { name <hostname> |
 *              class <classname> |
 *		speed { 56 | 64 } |
 *		modem-script <script> |
 *		system-script <script> |
 *		broadcast |
 *		<dial-string> ]
 *              spc ]           semi permanent connection*		
 *
 * OBJ(int,1) =	    <mapaddr> link type
 * OBJ(paddr,1) =   <mapaddr> address
 * OBJ(int,2) =	    MAP_NAME | MAP_SPEED
 * OBJ(int,3) =	    56 or 64
 * OBJ(string,1) =  <dial-string>
 * OBJ(string,2) =  <hostname>
 * OBJ(string,3) =  <modem-script>
 * OBJ(string,4) =  <system-script>
 * OBJ(int,4) =	    TRUE if "broadcast" option given
 * OBJ(int,5) =     TRUE if "spc" option given
 * OBJ(string,5) =  <classname>
 *
 *  Do not add anything with obj(int, greater than 10)
 */
EOLS	(int_dialer_map_eol, dialer_command, DIALER_MAP);

pdecl( int_dialer_map_opts);
    
STRING	(int_dialer_map_string, int_dialer_map_eol, int_dialer_map_eol,
	 OBJ(string,1), "Dialer string");
TEST_MULTIPLE_FUNCS(int_dialer_map_string_test, int_dialer_map_string,
		    NONE, NONE);
                             
/* broadcast */
KEYWORD_ID(int_dialer_map_broad, int_dialer_map_opts, 
           int_dialer_map_string_test,
	   OBJ(int,4), TRUE,
	   "broadcast",
	   "Broadcasts should be forwarded to this address",
	   PRIV_CONF|PRIV_SUBIF);

/* spc :  semi permanent connecions */
KEYWORD_ID(int_dialer_map_spc, int_dialer_map_opts,
	   int_dialer_map_broad,
           OBJ(int,5),TRUE,
	   "spc",
	   "Semi Permanent Connections",
	   PRIV_CONF);

/* system-script <script> */
STRING	(int_dialer_map_sscript_script, int_dialer_map_opts, no_alt,
	 OBJ(string,4), "Regular expression");
KEYWORD_ID(int_dialer_map_sscript, int_dialer_map_sscript_script,
	   int_dialer_map_spc,
	   OBJ(int,10), TRUE,
	   "system-script",
	   "Specify regular expression to select system dialing script",
	   PRIV_CONF);

/* modem-script <script> */
STRING	(int_dialer_map_mscript_script, int_dialer_map_opts, no_alt,
	 OBJ(string,3), "Regular expression");
KEYWORD_ID(int_dialer_map_mscript, int_dialer_map_mscript_script,
	   int_dialer_map_sscript,
	   OBJ(int,10), TRUE,
	   "modem-script",
	   "Specify regular expression to select modem dialing script",
	   PRIV_CONF);

/* speed { 56 | 64 } */
KEYWORD_ID(int_dialer_map_speed_64, int_dialer_map_opts, no_alt,
	   OBJ(int,2), 64,
	   "64", "64K bps", PRIV_CONF);
KEYWORD_ID(int_dialer_map_speed_56, int_dialer_map_opts,
	   int_dialer_map_speed_64,
	   OBJ(int,2), 56,
	   "56", "56K bps", PRIV_CONF);
KEYWORD_ID(int_dialer_map_speed, int_dialer_map_speed_56, 
	   int_dialer_map_mscript,
	   OBJ(int,10), TRUE,
	   "speed", "Set dialer speed", PRIV_CONF);

/* class <classname> */
STRING	(int_dialer_map_class_name, int_dialer_map_opts, no_alt,
	 OBJ(string,5), "name of dialer map class");
KEYWORD_ID(int_dialer_map_class, int_dialer_map_class_name,
	   int_dialer_map_speed,
	   OBJ(int,10), TRUE,
	   "class", "dialer map class", PRIV_CONF);

/* name <hostname> */
STRING	(int_dialer_map_name_name, int_dialer_map_opts, no_alt,
	 OBJ(string,2), "Hostname to map");
KEYWORD_ID(int_dialer_map_name, int_dialer_map_name_name, int_dialer_map_class,
	   OBJ(int,10), TRUE,
	   "name", "Map to a host", PRIV_CONF);

NOP	(int_dialer_map_opts, int_dialer_map_name, NONE);

/* <mapaddr> */
KEYWORD_ID(int_dialer_map_llc2, int_dialer_map_name, no_alt,
	   OBJ(int,1), LINK_LLC2,
	   "llc2", "LLC2", PRIV_CONF);
NSAPADDR(int_dialer_map_clns_addr, int_dialer_map_name, no_alt,
         OBJ(paddr,1), "NSAP address");
KEYWORD_ID(int_dialer_map_clns, int_dialer_map_clns_addr, int_dialer_map_llc2,
	   OBJ(int,1), LINK_CLNS,
	   "clns", "ISO CLNS", PRIV_CONF);
PROTOADDR(int_dialer_map_map, int_dialer_map_name, int_dialer_map_clns,
	 OBJ(int,1), OBJ(paddr,1), PMATCH_DGRAM PMATCH_EXCLUDE 
	 PMATCH_AARP PMATCH_APOLLO PMATCH_ARP PMATCH_CLNS PMATCH_QLLC); 
NVGENS	(int_dialer_map_nvgen, int_dialer_map_map,
	 dialer_command, DIALER_MAP);

KEYWORD (int_dialer_map, int_dialer_map_nvgen, no_alt,
         "map", "Define multiple dial-on-demand numbers", PRIV_CONF);
ASSERT  (int_dialer_map_test, int_dialer_map, int_dialer_assert_rotary,
	 !is_profile(csb->interface->hwptr));

/***************************************************************
 * dialer caller <string>
 * no dialer caller
 */
EOLS    (int_dialer_caller_eol, dialer_command, DIALER_CALLER);
STRING  (int_dialer_caller_string, int_dialer_caller_eol, no_alt,
         OBJ(string,1), "Dialer caller string");
NVGENS  (int_dialer_caller_nvgens, int_dialer_caller_string,
         dialer_command, DIALER_CALLER);

KEYWORD (int_dialer_caller, int_dialer_caller_nvgens, int_dialer_map_test,
	 "caller", "Specify telephone number to be screened",
	 PRIV_CONF);
/***************************************************************
 * dialer string <string>
 * no dialer string
 */

EOLS	(int_dialer_string_eol, dialer_command, DIALER_STRING);
STRING	(int_dialer_class_string, int_dialer_string_eol, no_alt,
	 OBJ(string,2), "Class String");
KEYWORD	(int_dialer_class, int_dialer_class_string, int_dialer_string_eol,
	 "class", "Map Class for the Dialer String", PRIV_CONF);
STRING	(int_dialer_string_string, int_dialer_class, no_alt,
	 OBJ(string,1), "Dialer string");
NVGENS	(int_dialer_string_nvgens, int_dialer_string_string,
	 dialer_command, DIALER_STRING);
KEYWORD	(int_dialer_string, int_dialer_string_nvgens, int_dialer_caller,
	 "string", "Specify telephone number to be passed to DCE device",
	 PRIV_CONF);

/***************************************************************
 * dialer wait-for-carrier-time <num>
 * no dialer wait-for-carrier-time
 */
PARAMS_KEYONLY(int_dialer_wait, int_dialer_string, "wait-for-carrier-time",
	       OBJ(int,1), 1, DIALER_MAX_TIMER,
	       dialer_command, DIALER_CARRIER,
	       "How long the router will wait for carrier",
	       "Wait for carrier time in seconds", PRIV_CONF);

/***************************************************************
 * dialer enable-timeout <num>
 * no dialer enable-timeout
 */
PARAMS_KEYONLY(int_dialer_enable, int_dialer_wait, "enable-timeout",
	       OBJ(int,1), 1, DIALER_MAX_TIMER,
	       dialer_command, DIALER_REENABLE,
	       "Set length of time an interface stays down"
	       " before it is available for dialing",
	       "Enable timeout in seconds", PRIV_CONF);

/***************************************************************
 * dialer fast-idle <num>
 * no dialer fast-idle
 */
PARAMS_KEYONLY(int_dialer_fast_idle, int_dialer_enable, "fast-idle",
	       OBJ(int,1), 1, DIALER_MAX_TIMER,
	       dialer_command, DIALER_FAST_IDLE,
	       "Set idle time before disconnecting line with an"
	       " unusually high level of contention",
	       "Fast idle in seconds", PRIV_CONF);

/***************************************************************
 * dialer idle-timeout <num>
 * no dialer idle-timeout
 */
PARAMS_KEYONLY(int_dialer_idle_timeout, int_dialer_fast_idle, "idle-timeout",
	       OBJ(int,1), 1, DIALER_MAX_TIMER,
	       dialer_command, DIALER_IDLE,
	       "Set idle time before disconnecting line",
	       "Idle timeout in seconds", PRIV_CONF);

/***************************************************************
 * [no] dialer callback-secure
 */
EOLS	(int_dialer_callbacksecure_eol, dialer_command,
	 DIALER_CALLBACK_SECURE);
NVGENS	(int_dialer_callbacksecure_nvgens, int_dialer_callbacksecure_eol,
	 dialer_command,
	 DIALER_CALLBACK_SECURE);
KEYWORD	(int_dialer_callbacksecure, int_dialer_callbacksecure_nvgens, NONE,
	 "callback-secure", "Enable callback security", PRIV_CONF);
ASSERT (int_dialer_callbacksecure_ass, int_dialer_callbacksecure, 
	int_dialer_idle_timeout,
	(!dtr_dialer(csb->interface->hwptr)));

/***************************************************************
 * [no] dialer remote-name <name>
 */
EOLS	(int_dialer_remote_name_eol, dialer_command, DIALER_REMOTE_NAME);
STRING  (int_dialer_remote_name_name, int_dialer_remote_name_eol, no_alt,
         OBJ(string,1), "Remote Name");
NOPREFIX(int_dialer_remote_name_noprefix, int_dialer_remote_name_name,
         int_dialer_remote_name_eol);
NVGENS	(int_dialer_remote_name_nvgens, int_dialer_remote_name_noprefix, 
	 dialer_command, DIALER_REMOTE_NAME);
KEYWORD	(int_dialer_remote_name, int_dialer_remote_name_nvgens, NONE,
	 "remote-name", "Configure a remote name", PRIV_CONF);
TEST_IDBSTATUS(int_dialer_assert_remote_name, int_dialer_remote_name, NONE,
	       int_dialer_callbacksecure_ass, IDB_DIALER);

/***************************************************************
 * [no] dialer dtr
 */
EOLS	(int_dialer_dtr_eol, dialer_command, DIALER_DTR);

NVGENS	(int_dialer_dtr_nvgens, int_dialer_dtr_eol, dialer_command,
	 DIALER_DTR);

KEYWORD	(int_dialer_dtr, int_dialer_dtr_nvgens, NONE,
	 "dtr", "Set DTR dialing for interface", PRIV_CONF);

TEST_IDBSTATUS(int_dialer_assert_dtr, NONE, int_dialer_dtr,
	      int_dialer_assert_remote_name, (IDB_ASYNC|IDB_DIALER|IDB_ISDN));


/***************************************************************
 * [no] dialer rotor-style [ priority | best ]
 */

EOLS	(int_dialer_rotor_eol, dialer_command, DIALER_ROTOR);

SET	(int_dialer_rotor_set, int_dialer_rotor_eol, OBJ(int,1),
	 DIALER_ROTOR_PRIORITY);

KEYWORD_ID  (int_dialer_rotor_priority, int_dialer_rotor_eol,
	     int_dialer_rotor_set,
	     OBJ(int,1), DIALER_ROTOR_PRIORITY,
	     "priority", "priority [first free] order", PRIV_CONF);

KEYWORD_ID  (int_dialer_rotor_best, int_dialer_rotor_eol,
	     int_dialer_rotor_priority,
	     OBJ(int,1), DIALER_ROTOR_BEST,
	     "best", "best [truly]", PRIV_CONF);

NVGENS	(int_dialer_rotor_nvgens, int_dialer_rotor_best, dialer_command,
	 DIALER_ROTOR);

KEYWORD	(int_dialer_rotor, int_dialer_rotor_nvgens, NONE,
	 "rotor", "Set outbound rotor order", PRIV_CONF);

TEST_IDBSTATUS(int_dialer_assert_rotor, NONE, int_dialer_rotor,
	       int_dialer_assert_dtr, (IDB_ASYNC|IDB_ISDN|IDB_SYNC));

/***************************************************************
 * [no] dialer in-band [no-parity | odd-parity]
 */
EOLS	(int_dialer_inband_eol, dialer_command, DIALER_INBAND);
SET     (int_dialer_inband_set, int_dialer_inband_eol, OBJ(int,1),
         DIALER_INBAND_NOPARITY);
KEYWORD_ID (int_dialer_inband_odd, int_dialer_inband_eol,
	    int_dialer_inband_set,
            OBJ(int,1), DIALER_INBAND_ODDPARITY,
            "odd-parity", "Odd parity", PRIV_CONF);
KEYWORD_ID (int_dialer_inband_nop, int_dialer_inband_eol,
	    int_dialer_inband_odd,
            OBJ(int,1), DIALER_INBAND_NOPARITY,
            "no-parity", "No parity", PRIV_CONF);
NVGENS  (int_dialer_inband_nvgens, int_dialer_inband_nop, dialer_command,
         DIALER_INBAND);
KEYWORD (int_dialer_inband, int_dialer_inband_nvgens, NONE,
         "in-band", "Set v.25bis dialing for interface", PRIV_CONF);
TEST_IDBSTATUS(int_dialer_assert_inband, NONE, int_dialer_inband,
	       int_dialer_assert_rotor, IDB_ISDN);

/***************************************************************
 * Note that the following keyword has "dialer-group" as its
 * alternate.  "dialer-group" is at the top of this file.
 */
KEYWORD	(cfg_int_dialer, int_dialer_assert_inband, cfg_int_dialergroup,
	 "dialer", "Dial-on-demand routing (DDR) commands", PRIV_CONF);

ASSERT (cfg_int_dialer_test_encap, cfg_int_dialer, NONE,
	dialer_encap(csb->interface->hwptr->enctype));
ASSERT	(cfg_int_dialer_test_backup, cfg_int_dialer_test_encap, ALTERNATE,
	 (csb->interface->hwptr->secondary == NULL));
ASSERT (cfg_int_dialer_test_intf, cfg_int_dialer_test_backup, ALTERNATE,
	(!(csb->interface->hwptr->status & IDB_VTEMPLATE) && 
         (csb->interface->hwptr->status & IDB_SERIAL) &&
	 ((csb->interface->hwptr->status & IDB_ISDN) || 
	   ((csb->interface->hwptr->type != IDBTYPE_T1) &&
	    (csb->interface->hwptr->type != IDBTYPE_E1)))));

#undef	ALTERNATE
#define	ALTERNATE	cfg_int_dialer_test_intf

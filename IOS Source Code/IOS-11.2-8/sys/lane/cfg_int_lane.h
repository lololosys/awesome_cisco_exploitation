/* $Id: cfg_int_lane.h,v 3.3.46.5 1996/06/06 18:38:53 michellw Exp $
** $Source: /release/112/cvs/Xsys/lane/cfg_int_lane.h,v $
**------------------------------------------------------------------
** parser/cfg_int_lane.h:
**
** October, 1994, Chris Parrott
** November, 1994 Gee Akyol
**
** Copyright (c) 1994-1996, 1997 by cisco Systems, Inc.
** All rights reserved.
**------------------------------------------------------------------
** $Log: cfg_int_lane.h,v $
 * Revision 3.3.46.5  1996/06/06  18:38:53  michellw
 * CSCdi59652:  make generic function for creating/deleting LEC
 * Branch: California_branch
 * The functions, lec_create_client() and lec_delete_client(), are
 * created to do the job.
 *
 * Revision 3.3.46.4  1996/06/04  01:00:25  ddecapit
 * CSCdi59395:  lec_srb_running referenced incorrectly
 * Branch: California_branch
 *
 * Revision 3.3.46.3  1996/05/17  21:14:54  cakyol
 * CSCdi58041: the elan name in "lane server-bus ethernet name"
 *             should be mandatory.
 * CSCdi57537: parse chains need to be re-ordered for LANE config
 *             commands.
 * Branch: California_branch
 *
 * Revision 3.3.46.2  1996/05/09  14:32:03  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.3.46.1.18.1  1996/04/27  07:05:58  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.3.46.1.8.7  1996/04/26  00:29:58  michellw
 * Branch: LE_Cal_V112_0_2_branch
 * Add a new sub-interface config command to configure
 * the LEC idle timer. "lane client idle-time IDLE-TIME".
 * IDLE-TIME is in the range of 2 to 120 seconds.
 *
 * Revision 3.3.46.1.8.6  1996/04/23  17:20:42  rbadri
 * CSCdi54446:  Lane static le-arps once configured cannot be removed
 * Branch: LE_Cal_V112_0_2_branch
 * Move the NOPREFIX to include the mac_address for which we are
 * deleting the le_arp entry.
 *
 * Revision 3.3.46.1.8.5  1996/04/22  17:34:47  rbadri
 * Branch: LE_Cal_V112_0_2_branch
 * cfg_int_lane.h is prettied up. Observe indentation from now on!
 * Make TokenRing parse commands visible only if TRLANE is in.
 *
 * Revision 3.3.46.1.8.4  1996/04/16  02:44:01  cakyol
 * goodbye to PVC support for LANE.
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.3.46.1.8.3  1996/04/12  06:51:56  kjadams
 * Branch: LE_Cal_V112_0_2_branch
 * remove ability to configure token ring lane on cat5k atm card.  
 *
 * Revision 3.3.46.1.8.2  1996/04/08  14:51:26  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.3.46.1.8.1  1996/04/08  01:56:24  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.3.46.1  1996/03/18  20:32:09  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.50.2  1996/03/25  02:19:54  cakyol
 * sync LE_Syn_Cal_V111_1_3_branch to LE_Cal_V111_1_0_3_branch
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.3.50.1  1996/03/22  22:53:34  rlowe
 * Non-sync of Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.3.26.2  1996/03/19  20:47:34  cakyol
 * make "lane config ...." command backward compatible with
 * the older versions.  So, now we accept both forms of
 * "lane config [database] NAME" but we only nvgen the new one,
 * "lane config database NAME".  As a result of this however, now
 * one can NOT use a NAME which matches with any of the keywords
 * "database", "auto-config-atm-address", "fixed-config-atm-address"
 * and "config-atm-address" in any beginning character positions.
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.3.26.1  1996/02/27  21:09:46  cakyol
 * non sync sync of LE_Cal_V111_0_16_branch to V111_1_0_3 yielding
 * LE_Cal_V111_1_0_3_branch
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.3.6.4  1996/02/22  09:05:21  cakyol
 * the new LECS config command syntax enabled provisionally
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.3.6.3  1996/02/06  22:19:08  cakyol
 * cleanup for the last code review:
 * - get rid of "#ifdef PARANOID...#endif".  Leave some in
 *   permanently, take some out completely.
 * - get rid of all "if 0....endif" type constructs.
 * - debug messages speeded up by using variable argument
 *   macro expansion.
 * - most fatal type errors are converted to crashdump with
 *   __FILE__ and __LINE__ printing.
 * - in the LECS, do *NOT* update the table of global LECS
 *   addresses, if any outstanding signalling requests remain.
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.3.6.2  1996/01/17  20:26:14  cakyol
 * - Index_add now returns the inserted element or the one already
 *   present.  More convenient for handling duplicates.
 * - Tidy up the parser help strings that involve atm address
 *   template.
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.3.6.1  1996/01/11  01:45:55  cakyol
 * non sync sync of LE_Calif_branch to V111_0_16 yielding
 *     LE_Cal_V111_0_16_branch
 *
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.3  1995/12/07  17:45:18  widmer
 * CSCdi45134:  Replace occurences of Ifelse with appropriate Test* macros
 *
 * Revision 3.2.10.1  1995/12/29  01:06:19  cakyol
 * Initial commits for:
 *
 * LANE Fault Tolerant LECS Servers
 * LANE Fault Tolerant LES Servers
 * HSRP over LANE Client
 *
 * Branch: LE_Calif_branch
 *
 * Revision 3.2  1995/11/17  17:49:50  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:22:28  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3.46.1  1996/02/03  07:06:57  rlowe
 * Non-sync sync of Synalc_catchup_branch to V111_0_5.
 * Branch: Synalc_catchup_pretal_branch
 *
 * Revision 2.3.28.1  1995/09/11  01:56:14  rlowe
 * Non-sync sync of Synalc2_branch to LE2_V110_1_0_3_branch,
 * yielding Synalc2_LE2_V110_1_0_3_branch.
 *
 * Branch: Synalc2_LE2_V110_1_0_3_branch
 *
 * Revision 2.3.12.1  1995/09/02  01:57:04  bbenson
 * Branch: Synalc2_branch
 * Sync in LANE for the new improved Synalc branch.
 *
 * Revision 2.3  1995/07/18  01:09:44  cakyol
 * CSCdi37230:  lane default-atm-address command should be a show command
 * - fixed.  Here is the summary:
 *
 *         show lane default-atm-address
 *                 prints the possible lane components on ALL atm
 *                 interfaces.
 *
 *         show lane default-atm-address interface atm1/0
 *                 prints the possible lane components on the
 *                 specified main-interface.
 *
 *         show lane default-atm-address interface atm1/0.1
 *                 prints the possible lane components on the
 *                 specified sub-interface.
 *
 * Revision 2.2  1995/07/17  22:34:50  cakyol
 * CSCdi37207:  auto addresses should be displayed without running the
 * components
 * - fixed.  The interface/subinterface command "lane
 * default-atm-addresses"
 *   now displays all would be auto addresses for all the lane components
 *   on the specific interface.
 *
 * Revision 2.1  1995/06/07  21:18:30  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _CFG_INT_LANE_H_
#define _CFG_INT_LANE_H_

/*
** OVERLOADED COMMANDS START HERE.  THESE CAN BE ISSUED EITHER
** AT THE MAIN OR THE SUB INTERACE.
*/

/******************************************************************
**
** [no] lane client ethernet VLAN-ID [ELAN-NAME]
** [no] lane client [ethernet | tokenring] 
**	[segment SEGMENT bridge BRIDGE] [ELAN-NAME]
**
** VLAN-ID is mandatory for Synergy Boxes but Not needed for
** Routers
**
** OBJ(int, 1) = LANE_TYPE_ETHERNET or LANE_TYPE_TOKEN_RING
** OBJ(int, 2) = Required for TokenRing - virtual ring number
** OBJ(int, 3) = Required for TokenRing - bridge to virtual ring.
** OBJ(int, 4) = VLAN-ID for Synergy color.
** OBJ(string, 1) = Optional symbolic name of the emulated LAN
*/

EOLS (lane_lec_eol, lane_interface_commands, LANE_CMD_LEC);

STRING (lane_lec_elan_name, lane_lec_eol, lane_lec_eol,
        OBJ (string, 1), "Name of the emulated LAN" );

NUMBER (lane_vlanid, lane_lec_elan_name, no_alt, OBJ (int, 4), 
	1, LANE_MAX_VLANID, "VLAN ID");

NUMBER (lane_lec_bridge_num, lane_lec_elan_name, no_alt,
        OBJ (int, 3), 1, LANE_MAX_BRIDGENUM, "Bridge number");

KEYWORD (lane_lec_bridge_kw,  lane_lec_bridge_num, no_alt,
	 "bridge", "Bridge number to the virtual ring", PRIV_SUBIF);

NUMBER (lane_lec_seg_num, lane_lec_bridge_kw, no_alt,
	OBJ(int, 2), 1, LANE_MAX_SEGMENTID, "virtual ring number");

KEYWORD (lane_lec_seg_kw,  lane_lec_seg_num, no_alt,
	 "segment", "Virtual ring segment id", PRIV_SUBIF); 

NOPREFIX (lane_lec_tr_noprefix, lane_lec_seg_kw, lane_lec_eol);

KEYWORD_ID (lane_lec_tokenring, lane_lec_tr_noprefix, no_alt,
	    OBJ(int, 1), LANE_TYPE_TOKEN_RING, "tokenring",
	    "Configure the LEC to emulate Token Ring", PRIV_SUBIF);

NOPREFIX (lane_lec_ether_noprefix, lane_lec_elan_name, lane_lec_eol);
 
IFELSE (lane_lec_check_trlane, lane_lec_tokenring, no_alt, tr_lane_present); 

KEYWORD_ID (lane_lec_ethernet, lane_lec_ether_noprefix, lane_lec_check_trlane,
	    OBJ(int, 1), LANE_TYPE_ETHERNET, "ethernet",
	    "Configure the LEC to emulate Ethernet", PRIV_SUBIF);

NOPREFIX (lane_lec_syn_noprefix, lane_vlanid, lane_lec_eol);
 
KEYWORD_ID (lane_lec_syn_ethernet, lane_lec_syn_noprefix, no_alt,
	    OBJ(int, 1), LANE_TYPE_ETHERNET, "ethernet",
	    "Configure the LEC to emulate Ethernet", PRIV_SUBIF);
 
TEST_EXPR (lane_lec_assert, lane_lec_ethernet, lane_lec_syn_ethernet,
	   no_alt, cpu_type != CPU_SYNALC);
 
NVGENS (lane_lec_nvgen, lane_lec_assert,
        lane_interface_commands, LANE_CMD_LEC);

EOLS (lane_lec_idle_time_eol, lane_interface_commands, LANE_CMD_IDLE_TIME);

NUMBER (lane_lec_get_idle_time, lane_lec_idle_time_eol, no_alt, 
	OBJ (int, 1), 2, 120, "Idle Timer (in seconds)");

NOPREFIX (lane_lec_idle_time_noprefix, lane_lec_get_idle_time, 
	  lane_lec_idle_time_eol);

NVGENS  (lane_lec_idle_time_nvgen, lane_lec_idle_time_noprefix,
         lane_interface_commands, LANE_CMD_IDLE_TIME);

KEYWORD (lane_lec_idle_time, lane_lec_idle_time_nvgen, lane_lec_nvgen,
	 "idle-time", "Set LEC Idle Timer", 
	 PRIV_SUBIF | PRIV_NOHELP);

KEYWORD (lane_lec, lane_lec_idle_time, no_alt,
	 "client", "Create a LAN Emulation Client", PRIV_SUBIF);

/******************************************************************
**
** [no] lane server-bus {ethernet | tokenring [segment SEGMENT]}
**      ELAN-NAME
**
** OBJ(int, 1) = LANE_TYPE_ETHERNET or LANE_TYPE_TOKEN_RING 
** OBJ(string, 1) = symbolic name of the emulated LAN. 
*/

EOLS (lane_les_eol, lane_interface_commands, LANE_CMD_LES_BUS);

STRING (lane_les_elan_name, lane_les_eol, no_alt,
	OBJ(string, 1), "Name of the emulated LAN" );

NUMBER (lane_les_segid, lane_les_elan_name, no_alt,
	OBJ(int, 2), 1, LANE_MAX_SEGMENTID, "local segment number");

KEYWORD (lane_les_segid_kw, lane_les_segid, no_alt,
        "segment", "Server's local Segment ID",
	 PRIV_SUBIF);

KEYWORD_ID (lane_les_tokenring, lane_les_segid_kw, no_alt, 
	    OBJ(int, 1), LANE_TYPE_TOKEN_RING, "tokenring", 
	    "Emulate Token Ring", PRIV_SUBIF);

IFELSE (lane_les_check_trlane, lane_les_tokenring, no_alt, tr_lane_present); 

KEYWORD_ID (lane_les_ethernet, lane_les_elan_name, lane_les_check_trlane,
	    OBJ(int, 1), LANE_TYPE_ETHERNET, "ethernet", 
	    "Emulate Ethernet", PRIV_SUBIF); 
 
KEYWORD_ID (lane_les_syn_ethernet, lane_les_elan_name, no_alt, 
	    OBJ(int, 1), LANE_TYPE_ETHERNET, "ethernet", 
	    "Emulate Ethernet", PRIV_SUBIF); 

TEST_EXPR (lane_les_assert, lane_les_ethernet, lane_les_syn_ethernet,
	   no_alt, cpu_type != CPU_SYNALC);
 
NOPREFIX (lane_les_noprefix, lane_les_assert, lane_les_eol);

NVGENS (lane_les_nvgen, lane_les_noprefix,
        lane_interface_commands, LANE_CMD_LES_BUS);

KEYWORD (lane_les,  lane_les_nvgen, lane_lec,
	 "server-bus", 
         "LAN Emulation Server & Broadcast and Unknown Server", 
         PRIV_SUBIF);

/******************************************************************
**
** [no] lane bus-atm-address NSAP-TEMPLATE
**
** OBJ(maskedAddr, 1) = BUS nsap address
**
*/

EOLS (lane_bus_nsap_eol, lane_interface_commands, LANE_CMD_BUS_NSAP);

MASKEDATMNSAPADDR (lane_bus_nsap_addr, lane_bus_nsap_eol, no_alt, 
		   OBJ(maskedAddr, 1), "BUS ATM address template");

NOPREFIX (lane_bus_nsap_noprefix, lane_bus_nsap_addr, 
	  lane_bus_nsap_eol);

NVGENS (lane_bus_nsap_nvgen, lane_bus_nsap_noprefix, 
	lane_interface_commands, LANE_CMD_BUS_NSAP);

KEYWORD (lane_bus_nsap, lane_bus_nsap_nvgen, lane_les,
	 "bus-atm-address", "Set BUS's ATM address",
	 PRIV_SUBIF); 

/******************************************************************
**
** [no] lane server-atm-adddress NSAP-ADDRESS
**
** OBJ(maskedAddr, 1) = LES nsap address
*/

EOLS (lane_les_nsap_eol, lane_interface_commands, LANE_CMD_LES_NSAP);

MASKEDATMNSAPADDR (lane_les_nsap_addr, lane_les_nsap_eol, no_alt, 
		   OBJ(maskedAddr, 1), "Server (LES) ATM address template");

NOPREFIX (lane_les_nsap_noprefix, lane_les_nsap_addr, 
	  lane_les_nsap_eol);

NVGENS (lane_les_nsap_nvgen, lane_les_nsap_noprefix, 
	lane_interface_commands, LANE_CMD_LES_NSAP);

KEYWORD (lane_les_nsap, lane_les_nsap_nvgen, lane_bus_nsap,
	 "server-atm-address", "set Server's (LES) ATM address",
	 PRIV_SUBIF); 

/******************************************************************
**
** [no] lane le-arp MAC-ADDRESS NSAP-ADDRESS
**
** OBJ(hwaddr, 1) = MAC address
** OBJ(hwaddr,2) = NSAP address
*/

EOLS (lane_le_arp_eol, lane_interface_commands, LANE_CMD_LE_ARP);

ATMNSAPADDR (lane_le_arp_nsap_addr, lane_le_arp_eol, no_alt, 
	     OBJ(hwaddr,2), "ATM address");

NOPREFIX (lane_le_arp_noprefix, lane_le_arp_nsap_addr, 
	  lane_le_arp_eol);

ENADDR  (lane_le_arp_macaddr, lane_le_arp_noprefix, no_alt, 
	 OBJ(hwaddr, 1), "6-Octet MAC address");


NVGENS (lane_le_arp_nvgen, lane_le_arp_macaddr, 
	lane_interface_commands, LANE_CMD_LE_ARP);

KEYWORD (lane_le_arp, lane_le_arp_nvgen, lane_les_nsap,
        "le-arp", 
	"Add a MAC to NSAP mapping in the LE-ARP table", 
	PRIV_SUBIF); 

/******************************************************************
**
** [no] lane client-atm-address NSAP-TEMPLATE
**
** OBJ(maskedAddr, 1) = LEC nsap address
** 
*/

EOLS (lane_lec_nsap_eol, lane_interface_commands, LANE_CMD_LEC_NSAP);

MASKEDATMNSAPADDR (lane_lec_nsap_addr, lane_lec_nsap_eol, no_alt, 
		   OBJ(maskedAddr, 1), "Client (LEC) ATM address template");

NOPREFIX (lane_lec_nsap_noprefix, lane_lec_nsap_addr, 
	  lane_lec_nsap_eol);

NVGENS (lane_lec_nsap_nvgen, lane_lec_nsap_noprefix, 
	lane_interface_commands, LANE_CMD_LEC_NSAP);

KEYWORD_MM (lane_lec_nsap, lane_lec_nsap_nvgen, lane_le_arp,
            "client-atm-address", "Set the LE Client ATM address",
	    PRIV_SUBIF, 7); 

/******************************************************************
**
**	    !!! BE VERY CAREFUL WITH THIS COMMAND !!!
**	        DO NOT TOUCH IT UNLESS YOU KNOW EXACTLY
**              WHAT IS HAPPENING.
**
** Associate [disassociate] an nsap/config table with [from] 
** an interface.  Note that the DATABASE_TABLE_NAME makes sense
** ONLY if "config" keyword has been specified.
**
** [no] lane [config] fixed-config-atm-address | 
**		      auto-config-atm-address |
**		      config-atm-address MASKED_NSAP_ADDRESS |
**		      [database] DATABASE_TABLE_NAME
**
** OBJ (int, 1) = "config" keyword parsed (or not)
** OBJ (int, 2) = subcommand variation
** OBJ (int, 3) = "database" keyword parsed (or not)
** OBJ (maskedAddr, 1) = MASKED_NSAP_ADDRESS
**
** The logic of this command is very convoluted to make sure 
** that:
**
** 1 - "database" keyword option can NOT appear in the 
**     parser help message if the "config" keyword has NOT 
**     been specified.  So, the parse tree is dynamically
**     controlled depending on what has been typed so far in
**     the command.
**
** 2 - To maintain backward compatibility, the string
**     "lane config DATABASE_TABLE_NAME", should also be
**     parsable, but only the new version should be NVGEN'ned.
*/

EOLS (configDatabaseNameEols, lecs_interface_function,
    LANE_LECS_BIND_TABLE);

STRING (configDatabaseName, configDatabaseNameEols, no_alt,
	OBJ(string, 1), "the name of the Config Server (LECS) database");

NOPREFIX (cfgDbaseNopfx, configDatabaseName, configDatabaseNameEols);

NVGENS (cfgDbaseNvgens, cfgDbaseNopfx, 
	lecs_interface_function, LANE_LECS_BIND_TABLE);

TEST_MULTIPLE_FUNCS (testKeywordsFirst, cfgDbaseNvgens, no_alt, NONE);

PRIV_TEST (cfgDatabaseKeyword_nonvgen, testKeywordsFirst, no_alt,
	   NONE, PRIV_NONVGEN);

/*
** If the "database" keyword is parsed, set OBJ(int, 3) to TRUE
** and go to reading the database name.  Otherwise, try and match
** a keyword first.
*/
KEYWORD_ID (cfgDatabaseKeyword, cfgDbaseNvgens, 
	    cfgDatabaseKeyword_nonvgen, OBJ(int, 3), TRUE,
	    "database", 
	    "bind a Config Server (LECS) database to the interface",
	    PRIV_CONF);

/* 
** If "config" keyword has been parsed, we can also
** now have a database name as well.  Otherwise that
** choice is not available and go straight thru to
** the next keyword option, which is "lane_lec_nsap".
*/
IFELSE (configKwdCheck, cfgDatabaseKeyword, lane_lec_nsap, 
	GETOBJ(int, 1));

EOLS (specificLecsAddressEols, lecs_interface_function,
      LANE_LECS_BIND_ADDR);

NOPREFIX (specificLecsAddressNopfx, specificLecsAddressEols,
	  specificLecsAddressEols);

MASKEDATMNSAPADDR (specificLecsAddress, specificLecsAddressNopfx, 
		   no_alt, OBJ(maskedAddr, 1), 
		   "Config Server (LECS) ATM address template");

NVGENS (specificLecsAddressNvgens, specificLecsAddress,
	lecs_interface_function, LANE_LECS_BIND_ADDR);

KEYWORD_ID_MM (LaneLecsBindSpecific, specificLecsAddressNvgens, 
	       configKwdCheck, OBJ(int, 2), LANE_LECS_BIND_ADDR,
	       "config-atm-address", "Bind a specific LECS address",
	       PRIV_CONF | PRIV_SUBIF, 7);

EOLS (LaneLecsAutoFixedEols, lecs_interface_function, 
      LANE_LECS_BIND_ADDR);

NOPREFIX (LaneLecsAutoFixedNopfx, LaneLecsAutoFixedEols, 
	  LaneLecsAutoFixedEols);

NVGENS (LaneLecsAutoFixedNvgens, LaneLecsAutoFixedNopfx,
	lecs_interface_function, LANE_LECS_BIND_ADDR);

KEYWORD_ID (LaneLecsBindAuto, LaneLecsAutoFixedNvgens, 
	    LaneLecsBindSpecific, OBJ(int, 2), LANE_LECS_BIND_AUTO,
	    "auto-config-atm-address",
	    "Bind the auto generated LECS address",
	    PRIV_CONF | PRIV_SUBIF);

KEYWORD_ID (LaneLecsBindFixed, LaneLecsAutoFixedNvgens,
	    LaneLecsBindAuto, OBJ(int, 2), LANE_LECS_BIND_FIXED,
	    "fixed-config-atm-address",
	    "let the LECS listen on the well known address IF master",
	    PRIV_CONF | PRIV_SUBIF);

/*
** MAIN-INTERFACE COMMANDS START HERE.  THESE CAN *** NOT *** BE
** ISSUED AT THE SUB INTERFACE MODE.  WE MADE SURE OF THIS WITH
** THE "IFELSE" MACRO CHECK BELOW.
*/

/******************************************************************
**
** [no] lane global-lecs-address UNMASKED_NSAP
**
** OBJ(hwaddr, 1) = UNMASKED_NSAP
*/

EOLS (globalLecsAddrEol, lecs_interface_function, 
      LANE_LECS_GLOBAL_ADDR);

NOPREFIX (globalLecsAddrNoPfx, globalLecsAddrEol, 
	  globalLecsAddrEol);

ATMNSAPADDR (readGlobalLecsAddress, globalLecsAddrNoPfx, no_alt, 
	     OBJ(hwaddr, 1), "ATM address of the global LECS");

NVGENS (globalLecsAddrNvgen, readGlobalLecsAddress,
	lecs_interface_function, LANE_LECS_GLOBAL_ADDR);

KEYWORD (globalLecsAddr, globalLecsAddrNvgen, LaneLecsBindFixed,
	 "global-lecs-address",   
	 "enter a backup/spare LECS address in the system",
	 PRIV_CONF);

/******************************************************************
**
** See if the "config" keyword is encountered or not ?
** If so, set OBJ(int, 1) to TRUE.
*/

KEYWORD_ID_MM (LaneTestConfigKeyword, LaneLecsBindFixed, 
	       globalLecsAddr, OBJ(int, 1), TRUE,
	       "config", 
	       "target the command at the Config Server (LECS)",
	       PRIV_CONF, 6);

/******************************************************************
**
** [no] mac-address-plan { same | different } [MAC_ADDRESS_BASE]
**
** OBJ(hwaddr, 1) = MAC_ADDRESS_BASE
*/

EOLS (LMAEolDiffWithout, lane_mac_address_plan, LANE_MACPLAN_DIFF);

EOLS (LMAEolDiffWith, lane_mac_address_plan, 
      LANE_MACPLAN_DIFF_NEWADDR);

ENADDR (LMADiffGetBase, LMAEolDiffWith, LMAEolDiffWithout,
	OBJ(hwaddr, 1), "the mac addr base to be used for clients");

KEYWORD (LMADiffWithout, LMADiffGetBase, no_alt,
	 "different", "let all clients have different mac addresses",
	 PRIV_CONF);

EOLS (LMAEolSameWithout, lane_mac_address_plan, LANE_MACPLAN_SAME);

EOLS (LMAEolSameWith, lane_mac_address_plan, 
      LANE_MACPLAN_SAME_NEWADDR);

ENADDR (LMASameGetBase, LMAEolSameWith, LMAEolSameWithout,
	OBJ(hwaddr, 1), "the mac addr to be used for all clients");

KEYWORD (LMASameWithout, LMASameGetBase, LMADiffWithout,
	 "same", "let all clients have the same mac address", PRIV_CONF);

NOPREFIX (LMANoPfx, LMASameWithout, LMAEolSameWithout);

NVGENS (LMABaseNvgen, LMANoPfx, lane_mac_address_plan, 
	LANE_MACPLAN_SAME);

KEYWORD (LMABase, LMABaseNvgen, LaneTestConfigKeyword,
	 "mac-address-plan", 
	 "define mac addr assignment method for clients", PRIV_CONF);

/*
** Direct the command to the appropriate context depending on
** whether the context is a main or a sub-interface
**
** Currently the "mac-address-plan" feature is disabled,
** this is why we jump straight to "LaneTestConfigKeyword" above.
** If you want to enable processing of this feature, simply
** replace "LaneTestConfigKeyword" with "LMABase" below.
*/
IFELSE (lane_if_test, LaneLecsBindFixed, LaneTestConfigKeyword, 
	is_subinterface (csb->interface));

KEYWORD (lane, lane_if_test, NONE, 
	 "lane", "Modify LANE parameters", PRIV_CONF | PRIV_SUBIF);

/*
** initialize OBJ(int, 3) to FALSE.  This variable
** indicates whether the "database" keyword has
** been parsed for the "lane config ...." command.
*/
SET (lane_init2, lane, OBJ(int, 3), FALSE);

/*
** initialize OBJ(int, 1) to FALSE.  This variable
** indicates whether the config keyword has been
** parsed or not.  This keyword indicates whether
** the command is intended for the LECS or not.
*/
SET (lane_init, lane_init2, OBJ(int, 1), FALSE);

ASSERT (lane_test, lane_init, ALTERNATE, 
	is_atm(csb->interface->hwptr));

#undef	ALTERNATE
#define	ALTERNATE	lane_test

#endif _CFG_INT_LANE_H_

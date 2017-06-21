/*
 * $Id: cfg_cfgtbl_subcommands.h,v 3.2.58.2 1996/08/12 16:04:18 widmer Exp $
 * $Source: /release/112/cvs/Xsys/lane/cfg_cfgtbl_subcommands.h,v $
 *------------------------------------------------------------------
 * lane lecs config table subcommands
 *
 * November 1994, Cihangir Akyol (Gee)
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: cfg_cfgtbl_subcommands.h,v $
 * Revision 3.2.58.2  1996/08/12  16:04:18  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2.58.1  1996/05/09  14:32:01  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.2.86.1  1996/04/27  07:05:56  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.2.76.5  1996/04/24  14:46:10  kjadams
 * CSCdi55471:  token ring breaks no [name NAME] [server-atm-address ADDR]
 * command
 * Branch: LE_Cal_V112_0_2_branch
 * fix the no [name NAME] [server-atm-address ADDR] to include
 * no name NAME local-seg-id SEGMENT for TR LANE - the name is required
 * for removing the local-seg-id.
 *
 * Revision 3.2.76.4  1996/04/23  22:25:44  cakyol
 * back out the token ring mods from "no [name NAME]
 *     [server-atm-address ATM_ADDRESS]" config server
 *     database command.
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.2.76.3  1996/04/12  06:49:27  kjadams
 * CSCdi54415:  cannot remove local-seg-id via no command
 * Branch: LE_Cal_V112_0_2_branch
 * Parse chain fixed to allow removing local-seg-id from elan
 *
 * Revision 3.2.76.2  1996/04/08  14:51:24  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.2.76.1  1996/04/08  01:56:22  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.2.64.2  1996/03/27  22:57:50  cakyol
 * Added some more 11.2 backward compatibility code that I missed.
 * This involves the "name NAME server-atm-address ADDRESS" lane
 * database command.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.2.64.1  1996/03/22  22:53:31  rlowe
 * Non-sync of Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.2.48.1  1996/03/05  06:42:23  rlowe
 * Apply LANE content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.2.36.1  1996/02/27  21:09:45  cakyol
 * non sync sync of LE_Cal_V111_0_16_branch to V111_1_0_3 yielding
 * LE_Cal_V111_1_0_3_branch
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.2.14.3  1996/01/26  02:02:23  cakyol
 * more code review mods (still more to come):
 *
 * - "show lane config" now shows all the details of the
 *   connected LESs, such as elan name, relative priority
 *   and whether active or not.
 * - in the lecs finder, protect against getnext going past
 *   the end of the lecs addresses table, by checking the
 *   incoming oid every time.
 * - in the lecs finder, add a parameter to the user interface
 *   limiting the number of lecs addresses the caller is
 *   interested in.
 * - use a static global buffer for debugging messages; saves
 *   stack space.
 * - make "les-timeout" and "lane global-lecs-address" commands
 *   hidden.
 * - prepend "atmSig_" to the 2 new atm signalling api function
 *   names.
 * - prepend "lsv_" to some LES functions for consistency.
 * - more white space, if...else formatting cleanup in some
 *   of the files.
 *
 * Branch: LE_Cal_V111_0_16_branch
 *
 *
 * Revision 3.2.14.2.2.1  1996/03/19  00:37:57  kjadams
 * Branch: TRlane_branch
 * Added new subcommand for route descriptors in the config table and for
 * the setting of the segment id associated with a TR ELAN.
 *
 * Revision 3.2.14.2  1996/01/17  20:26:12  cakyol
 * - Index_add now returns the inserted element or the one already
 *   present.  More convenient for handling duplicates.
 * - Tidy up the parser help strings that involve atm address
 *   template.
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.2.14.1  1996/01/11  01:45:54  cakyol
 * non sync sync of LE_Calif_branch to V111_0_16 yielding
 *     LE_Cal_V111_0_16_branch
 *
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.2.10.1  1995/12/29  01:06:18  cakyol
 * Initial commits for:
 *
 * LANE Fault Tolerant LECS Servers
 * LANE Fault Tolerant LES Servers
 * HSRP over LANE Client
 *
 * Branch: LE_Calif_branch
 *
 * Revision 3.2  1995/11/17  17:49:46  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:22:26  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/13  01:07:03  cakyol
 * CSCdi37016:  lecs access policy should allow for restricted elans
 * - done.  The main point is that restricted elans cannot now be
 *   joined by name only.  Explicit atm address or mac address
 *   is required.
 *
 * Revision 2.1  1995/06/07  21:18:25  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
**
** delete one specific server atm address or an entire elan
**
** no [name NAME] [server-atm-address ADDRESS]
**
** OBJ(string, 1) = elan name (or NULL if NOT parsed)
** OBJ(hwaddr, 1) = ADDRESS
**  or
** no name NAME local-seg-id SEGMENT
** OBJ(string, 1) = elan name - required for local-seg-id
** OBJ(int, 1) = segment number
*/

EOLS (neg_les_segment_eol, lecs_cfgtbl_function,
      LANE_LECS_CMD_ELAN_SET_SEGMENT);
 
EOLS (neg_saadelete_eol, lecs_cfgtbl_function, 
      LANE_LECS_SAA_DELETE);

KEYWORD (neg_local_seg_id, neg_les_segment_eol, neg_saadelete_eol,
    "local-seg-id", "local segment number for this elan",
    PRIV_CONF);

ATMNSAPADDR (neg_elan_nsapaddr, neg_saadelete_eol, no_alt,
	     OBJ(hwaddr,1), 
	     "Server (LES) ATM address to be deleted");

KEYWORD (neg_saa_no_name, neg_elan_nsapaddr, no_alt,
    "server-atm-address",
    "introduce the les-nsap address to be deleted",
    PRIV_CONF);

KEYWORD (neg_saa, neg_elan_nsapaddr, neg_local_seg_id,
	 "server-atm-address", 
	 "introduce the les-nsap address to be deleted",
	 PRIV_CONF);

STRING (neg_elanname, neg_saa, no_alt, 
	OBJ(string,1),
	"Name of emulated lan to be deleted");

KEYWORD (negative, neg_elanname, neg_saa_no_name,
	 "name", "introduce elan name to be deleted",
	 PRIV_CONF);


/******************************************************************
** set the elan les timeout 
**
** name NAME les-timeout TIMEOUT
**
** OBJ(string, 1) = elan name
** OBJ(int, 1)    = timeout
*/

EOLS (eol_les_timeout, lecs_cfgtbl_function, 
      LANE_LECS_CMD_ELAN_SET_TIMEOUT);

NUMBER (read_timeout, eol_les_timeout, no_alt,
	OBJ(int,1), 0, 10000,
	NULL /*"LES grace period timeout in seconds"*/);

KEYWORD (elan_les_timeout, read_timeout, no_alt,
	 "les-timeout", NULL /*"introduce the les timeout grace period"*/,
	 PRIV_CONF|PRIV_HIDDEN);

/******************************************************************
** set elan restricted status
**
** name NAME ( restricted | un-restricted )
**
** OBJ(string, 1) = elan name
** OBJ(int, 1)    = restricted status
*/

EOLS (eol_setr, lecs_cfgtbl_function, LANE_LECS_CMD_ELAN_SETR);

KEYWORD_ID (elan_setunr, eol_setr, elan_les_timeout,
	    OBJ(int,1), ELAN_NOT_RESTRICTED,
	    "un-restricted", 
	    "make this elan open to access by name only",
	    PRIV_CONF);

KEYWORD_ID (elan_setr, eol_setr, elan_setunr,
	    OBJ(int, 1), ELAN_RESTRICTED,
	    "restricted", 
	    "make this elan closed to access by name only",
	    PRIV_CONF);


/******************************************************************
** set the elan segment number
**
** name NAME local-seg-id SEGMENT
**
** OBJ(string, 1) = elan name
** OBJ(int, 1)    = segment number
*/

EOLS (eol_les_segment, lecs_cfgtbl_function,
      LANE_LECS_CMD_ELAN_SET_SEGMENT);

NUMBER (get_elan_les_segment, eol_les_segment, no_alt,
   OBJ(int, 1), LANE_MIN_SEGMENTID, LANE_MAX_SEGMENTID,
   "segment number of this emulated Token Ring LAN");

KEYWORD (elan_les_segment, get_elan_les_segment, elan_setr,
    "local-seg-id", "local segment number for this emulated TR LAN",
    PRIV_CONF);


/******************************************************************
** rename a elan
**
** name OLDNAME new-name NEWNAME
**
** OBJ(string, 1) = old name
** OBJ(string, 2) = new name
*/

EOLS (eol_newname, lecs_cfgtbl_function, 
      LANE_LECS_CMD_ELAN_RENAME);

STRING (read_newname, eol_newname, no_alt,
	OBJ(string, 2), "new name of the elan");

KEYWORD (elan_newname, read_newname, elan_les_segment,
	 "new-name", "introduce the new name for this elan",
	 PRIV_CONF);

/******************************************************************
**
** name NAME server-atm-address ADDRESS \
**	[restricted] [index INDEX]
**
** OBJ(string, 1) = name of elan
** OBJ(hwaddr, 1) = UNMASKED_NSAP_ADDRESS
** OBJ(int, 1)	   = elan restricted status
** OBJ(int, 2)	   = server address index
*/

EOLS (elancreate_eol, lecs_cfgtbl_function, 
      LANE_LECS_CMD_ELAN_CREATE);

NUMBER (read_priority, elancreate_eol, no_alt,
	OBJ(int, 2), 0, 10000,
	"relative index (hence the priority) of the LES address");

KEYWORD (addr_priority, read_priority, elancreate_eol,
	 "index", "specify relative priority of the Server (LES)",
	 PRIV_CONF);

KEYWORD_ID (pos_elan_restricted, addr_priority, addr_priority,
	    OBJ(int, 1), ELAN_RESTRICTED,
	    "restricted", 
	    "make this elan closed to access by name only",
	    PRIV_CONF);

ATMNSAPADDR (pos_elan_nsapaddr, pos_elan_restricted, no_alt,
	     OBJ(hwaddr, 1), 
	     "Server (LES) ATM address for this elan");

/* initialize OBJ(int, 2) to LOWEST_PRIO */
KEYWORD_ID (server_atm_address, pos_elan_nsapaddr, elan_newname,
            OBJ(int, 2), LOWEST_PRIO,
	    "server-atm-address", 
	    "the les-nsap address for this elan",
	    PRIV_CONF);

STRING (pos_elanname, server_atm_address, no_alt,
	OBJ(string, 1), "Name of emulated lan");

/* initialize OBJ(int, 1) to ELAN_NO_CHANGE */
KEYWORD_ID (positive, pos_elanname, ALTERNATE,
	    OBJ(int, 1), ELAN_NO_CHANGE,
	    "name", 
	    "introduce the name of elan being created or changed",
	    PRIV_CONF);

TESTVAR(cfgtbl_elan_ifelse, negative, positive, NONE, NONE, NONE,
	sense, FALSE);

/******************************************************************
**
** [no] route-desc SEGMENT_ID bridge BRIDGE_NUMBER name ELAN_NAME
**
** OBJ (int, 1) = SEGMENT_ID
** OBJ (int, 2) = BRIDGE_NUMBER
** OBJ (string, 1) = ELAN_NAME
*/

EOLS (addlec_rd_eol, lecs_cfgtbl_function, LANE_LECS_CMD_ADD_RD);

NOPREFIX (lec_rd_nopfx, addlec_rd_eol, addlec_rd_eol);

STRING (addlec_rd_elanname, lec_rd_nopfx, addlec_rd_eol,
	OBJ(string,1), "Name of elan to which the Route desc belongs");

KEYWORD (addLec_rd_name_kw, addlec_rd_elanname, no_alt,
    "name", "name of the elan to which this client will belong",
    PRIV_CONF);

NUMBER  (addlec_rd_bridge, addLec_rd_name_kw, no_alt,
	 OBJ(int,2), LANE_MIN_BRIDGENUM, LANE_MAX_BRIDGENUM,
        "bridge number");

KEYWORD (addLec_rd_bridge_kw, addlec_rd_bridge, no_alt,
    "bridge", "bridge id for the route descriptor",
    PRIV_CONF);

NUMBER  (lec_rd_seg, addLec_rd_bridge_kw, no_alt,
	 OBJ(int,1), LANE_MIN_SEGMENTID, LANE_MAX_SEGMENTID,
        "segment id");

KEYWORD (addLec_rd, lec_rd_seg, cfgtbl_elan_ifelse,
    "route-desc", "route descriptor: segment and bridge number - of LEC",
    PRIV_CONF);


/******************************************************************
**
** [no] mac-address MAC_ADDR name ELAN_NAME
**
** OBJ(hwaddr, 1) = MAC_ADDR
** OBJ(string, 1) = ELAN_NAME
*/

EOLS (addlec_mac_eol, lecs_cfgtbl_function, LANE_LECS_CMD_ADD_MAC);

STRING (addlec_mac_elanname, addlec_mac_eol, no_alt,
	OBJ(string,1), "Name of elan");

KEYWORD (addlecmac_elan_kw, addlec_mac_elanname, no_alt,
	 "name", "the elan name for the LEC",
	 PRIV_CONF);

NOPREFIX (lec_mac_addr_nopfx, addlecmac_elan_kw, addlec_mac_eol);

ENADDR (lec_mac_addr, lec_mac_addr_nopfx, no_alt,
	OBJ(hwaddr,1), "6-octet LEC mac address");

KEYWORD (addlec_mac, lec_mac_addr, addLec_rd, 
	 "mac-address", "the mac address of LEC",
	 PRIV_CONF);

/******************************************************************
**
** [no] client-atm-address MASKED_NSAP_ADDRESS name ELAN_NAME
**
** OBJ(maskedAddr, 1) = MASKED_NSAP_ADDRESS
** OBJ(string, 1) = ELAN_NAME
*/

EOLS (addlec_nsap_eol, lecs_cfgtbl_function, LANE_LECS_CMD_ADD_NSAP);

STRING (addlec_nsap_elanname, addlec_nsap_eol, no_alt,
	OBJ(string,1), "Name of elan");

KEYWORD (addlecnsap_elan_kw, addlec_nsap_elanname, no_alt,
	 "name", "the elan name for the LEC",
	 PRIV_CONF);

NOPREFIX (lec_nsap_addr_nopfx, addlecnsap_elan_kw, addlec_nsap_eol);

MASKEDATMNSAPADDR (lec_nsap_addr, lec_nsap_addr_nopfx, no_alt,
	           OBJ(maskedAddr, 1), 
		   "Client (LEC) ATM address template");

KEYWORD (addlec_nsap, lec_nsap_addr, addlec_mac,
	 "client-atm-address", "the nsap address of LEC",
	 PRIV_CONF);

/******************************************************************
**
** [no] default-name ELAN_NAME
**
** OBJ(string, 1) = ELAN_NAME
*/

EOLS (cfgtbl_default_elan_eol, lecs_cfgtbl_function, LANE_LECS_CMD_DFLT);

STRING (cfgtbl_default_elanname, cfgtbl_default_elan_eol, no_alt,
	OBJ(string,1), "Name of default elan");

NOPREFIX (cfgtbl_default_elanname_nopfx, cfgtbl_default_elanname, 
	  cfgtbl_default_elan_eol);

KEYWORD_MM (cfgtbl_default_elan, cfgtbl_default_elanname_nopfx, addlec_nsap,
	    "default-name", "set the default elan",
	    PRIV_CONF, 8);


/******************************************************************
**
** display-databases
**
** displays the list of all config tables, with the one in
** current context clearly indicated.
*/

EOLS (cfgtbl_name_eol, lecs_cfgtbl_function, LANE_LECS_CFGTBL_DISPLAY);

KEYWORD (cfgtbl_name, cfgtbl_name_eol, cfgtbl_default_elan,
	 "display-databases",
	 "command to display all the lecs database tables",
	 PRIV_CONF);

/******************************************************************
**
** atm-nsap-exercise
**
** just there to debug the clns parsing routines
*/

EOLNS (masked_exercise_eol, lecs_cfgtbl_function);

MASKEDATMNSAPADDR (masked_exercise_addr, masked_exercise_eol, no_alt,
	     	   OBJ(maskedAddr,1), NULL /*"clns address template"*/);

KEYWORD (masked_exercise, masked_exercise_addr, cfgtbl_name,
	 "atm-nsap-exercise", NULL /*"a masked nsap address"*/,
	 PRIV_CONF|PRIV_HIDDEN);

#undef	ALTERNATE
#define	ALTERNATE	masked_exercise


/* $Id: exec_show_vtp.h,v 1.2.58.2 1996/08/12 16:09:18 widmer Exp $
 * $Source: /release/112/cvs/Xsys/vtp/exec_show_vtp.h,v $
 *------------------------------------------------------------------
 * exec_show_vtp.h - CLI show command header file for VTP.
 *
 * Dec 1995, Dean Cheng 
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_show_vtp.h,v $
 * Revision 1.2.58.2  1996/08/12  16:09:18  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 1.2.58.1  1996/05/09  14:48:19  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 1.2.84.1  1996/04/27  07:38:21  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.2.74.1  1996/04/08  02:42:10  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.2.64.1  1996/03/22  09:44:50  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.2.48.1  1996/03/05  05:37:21  rlowe
 * Apply LANE content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.2.36.1  1996/02/27  21:20:15  cakyol
 * non sync sync of LE_Cal_V111_0_16_branch to V111_1_0_3 yielding
 * LE_Cal_V111_1_0_3_branch
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 1.2.4.1  1996/01/11  01:47:46  cakyol
 * non sync sync of LE_Calif_branch to V111_0_16 yielding
 *     LE_Cal_V111_0_16_branch
 *
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 1.1.2.2  1996/01/03  01:49:16  dcheng
 * Submit CLI code for VTP.
 * Branch: LE_Calif_branch
 *
 * Revision 1.1.2.1  1995/12/23  02:27:00  dcheng
 * Branch: LE_Calif_branch
 *
 * Revision 1.1  1995/12/23  02:24:18  dcheng
 * Initial revision
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * show vtp pruning [vlan <vlan-id> domain <domain-name>]
 * 
 * OBJ(int, 1) = <vlan-id>
 * OBJ(string, 1) = <domain-name>
 *****************************************************************/

EOLS (show_vtp_pruning_vlan_eol, show_vtp, SHOW_VTP_PRUNING_VLAN);
EOLS (show_vtp_pruning_eol, show_vtp, SHOW_VTP_PRUNING);
STRING (show_vtp_pruning_domain_name, show_vtp_pruning_vlan_eol,
	no_alt, OBJ(string,1), "A VTP domain name");
KEYWORD (show_vtp_pruning_vlan_domain, show_vtp_pruning_domain_name, no_alt,
	 "domain", "Domain for the specified VLAN", PRIV_USER);
NUMBER (show_vtp_pruning_vlan_number, show_vtp_pruning_vlan_domain,
	no_alt, OBJ(int,1), 1, 1005, "A VTP VLAN number");
KEYWORD (show_vtp_pruning_vlan, show_vtp_pruning_vlan_number,
	 show_vtp_pruning_eol, "vlan", "VLAN specific info",
	 PRIV_USER);
KEYWORD (show_vtp_pruning, show_vtp_pruning_vlan, no_alt, "pruning",
	 "VTP Pruning information", PRIV_USER);

/******************************************************************
 * show vtp database [vlan <vlan-id>] [domain <domain-name>]
 * 
 * OBJ(int,1) = TRUE if vlan present
 * OBJ(int,2) = <vlan-id>                
 * OBJ(int,3) = TRUE if domain present
 * OBJ(string,1) = <domain-name>
 *****************************************************************/
EOLS   (show_vtp_database_eol, show_vtp, SHOW_VTP_DATABASE);

SET    (show_vtp_database_set2, show_vtp_database_eol, OBJ(int,3), TRUE);
STRING (show_vtp_database_domain_name, show_vtp_database_set2, no_alt,
        OBJ(string,1), "A VTP domain name");
KEYWORD (show_vtp_database_domain, show_vtp_database_domain_name,
         show_vtp_database_eol, "domain", "VTP specific domain",PRIV_USER);

SET (show_vtp_database_set1, show_vtp_database_domain, OBJ(int,1), TRUE);
NUMBER (show_vtp_database_vlan_number, show_vtp_database_set1, no_alt,
        OBJ(int,2), 1, 1005,  "A VTP VLAN number");
KEYWORD (show_vtp_database_vlan, show_vtp_database_vlan_number,
         show_vtp_database_domain, "vlan", "VTP specific vlan", PRIV_USER);

KEYWORD (show_vtp_database, show_vtp_database_vlan, show_vtp_pruning,
	 "database", "VTP database", PRIV_USER);

/******************************************************************
 * show vtp statistics [domain <domain-name>]
 * 
 * OBJ(int,1) = TRUE if domain present
 * OBJ(string,1) = <domain-name>
 *****************************************************************/
EOLS	(show_vtp_statistics_eol, show_vtp, SHOW_VTP_STATISTICS);
SET    (show_vtp_statistics_set, show_vtp_statistics_eol, OBJ(int,1), TRUE);
STRING (show_vtp_statistics_domain_name, show_vtp_statistics_set, no_alt,
        OBJ(string,1), "An VTP domain name");
KEYWORD (show_vtp_statistics_domain, show_vtp_statistics_domain_name,
         show_vtp_statistics_eol, "domain", "VTP specific domain statistics",PRIV_USER);
KEYWORD (show_vtp_statistics, show_vtp_statistics_domain, show_vtp_database,
	 "statistics", "VTP statistics", PRIV_USER);

/******************************************************************
 * show vtp trunk [domain <domain-name>]
 *
 * OBJ(int,1) = TRUE if domain present
 * OBJ(string,1) = <domain-name>
 *****************************************************************/ 
EOLS	(show_vtp_trunk_eol, show_vtp, SHOW_VTP_TRUNK);
SET    (show_vtp_trunk_set, show_vtp_trunk_eol, OBJ(int,1), TRUE);
STRING (show_vtp_trunk_domain_name, show_vtp_trunk_set, no_alt,
        OBJ(string,1), "An VTP domain name");
KEYWORD (show_vtp_trunk_domain, show_vtp_trunk_domain_name,
         show_vtp_trunk_eol, "domain", "VTP specific domain trunk",PRIV_USER);
KEYWORD (show_vtp_trunk, show_vtp_trunk_domain, show_vtp_statistics,
	 "trunk", "VTP trunk status and configuration", PRIV_USER);

/******************************************************************
 * show vtp status [domain <domain-name>]
 *
 * OBJ(int,1) = TRUE if domain present
 * OBJ(string,1) = <domain-name>
 *****************************************************************/
EOLS	(show_vtp_status_eol, show_vtp, SHOW_VTP_STATUS);
SET    (show_vtp_status_set, show_vtp_status_eol, OBJ(int,1), TRUE);
STRING (show_vtp_status_domain_name, show_vtp_status_set, no_alt,
        OBJ(string,1), "An VTP domain name");
KEYWORD (show_vtp_status_domain, show_vtp_status_domain_name,
         show_vtp_status_eol, "domain", "VTP specific domain status",PRIV_USER);
KEYWORD (show_vtp_status, show_vtp_status_domain, show_vtp_trunk,
	 "status", "VTP domain status", PRIV_USER);

/******************************************************************
 * show vtp
 *****************************************************************/
EOLNS	(show_vtp_eol, show_vtp);
KEYWORD (show_vtp, show_vtp_status, ALTERNATE,
	 "vtp", "VTP information", PRIV_USER|PRIV_USER_HIDDEN);

#undef	ALTERNATE
#define	ALTERNATE	show_vtp

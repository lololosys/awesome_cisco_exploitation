/* $Id: cfg_int_vtp.h,v 1.2.58.2 1996/05/21 22:29:11 cyoung Exp $
 * $Source: /release/112/cvs/Xsys/vtp/cfg_int_vtp.h,v $
 *------------------------------------------------------------------
 * cfg_int_vtp.h - CLI config file for VTP trunk interface.
 *
 * Dec 1995, Dean Cheng 
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_int_vtp.h,v $
 * Revision 1.2.58.2  1996/05/21  22:29:11  cyoung
 * Now it returns a code
 * CSCdi56442:  vtp_download_info needs return code
 * Branch: California_branch
 *
 * Revision 1.2.58.1  1996/05/09  14:48:11  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 1.2.84.1  1996/04/27  07:38:13  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.2.74.1  1996/04/08  02:42:01  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.2.64.2  1996/03/25  02:21:31  cakyol
 * sync LE_Syn_Cal_V111_1_3_branch to LE_Cal_V111_1_0_3_branch
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.2.36.3  1996/03/15  18:57:50  cyoung
 * No form of vtp trunk no longer requires domain info.
 * vtp trunk command not executed until all configuration read.
 * clean up trunk info if default VLAN additions fail.
 * Move vtp_intf_enable function to vtp process.
 * more vtp configure support.
 * add subsystem dependancies and sequence.
 * Fix bug with modifying an existing vlan from CLI.
 * Fix pointer arithmetic in vtp_prepare_storage.
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 1.2.36.2  1996/03/05  01:54:33  cyoung
 * Add vtp configured interface command.  Provide function to retrieve
 * SNMP stats. Fix problem which tries to access NULL pointer when
 * adding a domain after all have been deleted.  Make vtp_upload_info
 * more useful.  Put in beginning TFTP support.
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 1.2.36.1  1996/02/27  21:20:11  cakyol
 * non sync sync of LE_Cal_V111_0_16_branch to V111_1_0_3 yielding
 * LE_Cal_V111_1_0_3_branch
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 1.2.4.1  1996/01/11  01:47:44  cakyol
 * non sync sync of LE_Calif_branch to V111_0_16 yielding
 *     LE_Cal_V111_0_16_branch
 *
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 1.1.2.2  1996/01/03  01:49:15  dcheng
 * Submit CLI code for VTP.
 * Branch: LE_Calif_branch
 *
 * Revision 1.1.2.1  1995/12/23  02:08:59  dcheng
 * Branch: LE_Calif_branch
 *
 * Revision 1.1  1995/12/23  02:07:52  dcheng
 * Initial revision
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/***************************************************************
 * [no] vtp trunk domain <domain-name>
 * [no] vtp trunk pruning-disable <vlan-id>
 * [no] vtp configured
 *
 ****************************************************************/
EOLS	(vtp_trunk_eol, vtp_intfc_command, VTP_TRUNK);
EOLS	(vtp_config_eol, vtp_intfc_command, VTP_CONFIGURED);
EOLS    (vtp_trunk_pruning_eol, vtp_intfc_command, VTP_TRUNK_PRUNING);

NUMBER (vtp_trunk_pruning_vlan, vtp_trunk_pruning_eol, no_alt,
         OBJ(int,1), 1, 1005, "index of VLAN to make pruning ineligible");
NVGENS (vtp_trunk_pruning_nvgen, vtp_trunk_pruning_vlan, vtp_intfc_command,
	VTP_TRUNK_PRUNING);
KEYWORD (vtp_trunk_pruning, vtp_trunk_pruning_nvgen, no_alt, 
	 "pruning-disable", "Make a VLAN on this trunk pruning ineligible",
	 PRIV_CONF);
STRING (vtp_trunk_domain_name, vtp_trunk_eol, no_alt, OBJ(string,1),
        "name of the VTP management domain");
NVGENS	(vtp_trunk_nvgen, vtp_trunk_domain_name, vtp_intfc_command,
	 VTP_TRUNK);
NOPREFIX (vtp_trunk_noprefix, vtp_trunk_nvgen, vtp_trunk_eol);
KEYWORD	(vtp_trunk_domain, vtp_trunk_noprefix, vtp_trunk_pruning,
	 "domain", "VTP trunk domain", PRIV_CONF);
NVGENS	(vtp_config_nvgen, vtp_config_eol, vtp_intfc_command,
	 VTP_CONFIGURED);
KEYWORD	(vtp_int_config, vtp_config_nvgen, no_alt,
	 "configurable", "Put a sub-interface under VTP control",
	 PRIV_CONF|PRIV_SUBIF); 
KEYWORD	(vtp_int_trunk, vtp_trunk_domain, vtp_int_config,
	 "trunk", "Enable VTP on interface", PRIV_CONF);
KEYWORD	(vtp_int, vtp_int_trunk, ALTERNATE,
	 "vtp", "VTP interface subcommands", PRIV_CONF|PRIV_SUBIF);

#undef	ALTERNATE
#define	ALTERNATE	vtp_int

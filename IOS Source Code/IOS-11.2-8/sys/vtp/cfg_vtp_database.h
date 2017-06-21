/* $Id: cfg_vtp_database.h,v 1.1.6.1 1996/05/09 14:48:12 rbadri Exp $
 * $Source: /release/112/cvs/Xsys/vtp/cfg_vtp_database.h,v $
 *------------------------------------------------------------------
 * cfg_vtp_database.h - CLI config file for VTP global command.
 *
 * Dec 1995, Dean Cheng 
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_vtp_database.h,v $
 * Revision 1.1.6.1  1996/05/09  14:48:12  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 1.1.52.1  1996/04/27  07:38:14  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.40.1  1996/04/08  02:42:03  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.24.1  1996/03/25  02:21:33  cakyol
 * sync LE_Syn_Cal_V111_1_3_branch to LE_Cal_V111_1_0_3_branch
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/22  03:15:53  bbenson
 * Branch: LE_Cal_V111_1_0_3_branch
 * Rearrange cadillac fins on the VTP command parser.  Install
 * electroglide subcommand modes for vtp-domain and vtp-database commands.
 * Running lights coming in next commit.
 *
 * Revision 1.2.36.1  1996/02/27  21:20:12  cakyol
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
 * Revision 1.1.2.2  1996/01/03  01:49:16  dcheng
 * Submit CLI code for VTP.
 * Branch: LE_Calif_branch
 *
 * Revision 1.1.2.1  1995/12/23  02:12:24  dcheng
 * Branch: LE_Calif_branch
 *
 * Revision 1.1  1995/12/23  02:11:10  dcheng
 * Initial revision
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/******************************************************************************
 * vtp-database domain <domain-name>
 * no vtp-database domain <domain-name>
 *
 *****************************************************************************/
EOLS	(cfg_vtp_database_eol, vtp_config_commands, VTP_DATABASE);
STRING (cfg_vtp_database_domain_name, cfg_vtp_database_eol, no_alt, 
	OBJ(string,1), "Name of the VTP management domain");
NVGENS	(cfg_vtp_database_nvgen, cfg_vtp_database_domain_name, 
	 vtp_config_commands, VTP_DATABASE);
KEYWORD (cfg_vtp_database_domain, cfg_vtp_database_nvgen, no_alt, "domain",
        "VTP domain to configure", PRIV_CONF);
KEYWORD_ID (cfg_vtp_database, cfg_vtp_database_domain, ALTERNATE,
	    leave_subcommand, TRUE,
	    "vtp-database", "Configure VTP database", PRIV_CONF);

#undef  ALTERNATE
#define ALTERNATE cfg_vtp_database



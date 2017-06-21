/* $Id: cfg_vtp_domain.h,v 1.1.6.1 1996/05/09 14:48:16 rbadri Exp $
 * $Source: /release/112/cvs/Xsys/vtp/cfg_vtp_domain.h,v $
 *------------------------------------------------------------------
 * cfg_vtp_domain.h - CLI config file for VTP domain global command
 *
 * March 1996, Brad Benson
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_vtp_domain.h,v $
 * Revision 1.1.6.1  1996/05/09  14:48:16  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 1.1.52.1  1996/04/27  07:38:18  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.40.1  1996/04/08  02:42:06  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.24.1  1996/03/25  02:21:36  cakyol
 * sync LE_Syn_Cal_V111_1_3_branch to LE_Cal_V111_1_0_3_branch
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/22  03:15:57  bbenson
 * Branch: LE_Cal_V111_1_0_3_branch
 * Rearrange cadillac fins on the VTP command parser.  Install
 * electroglide subcommand modes for vtp-domain and vtp-database commands.
 * Running lights coming in next commit.
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************************
 * vtp-domain <domain-name>
 * no vtp-domain <domain-name>
 *
 * OBJ(string,1) = <domain-name>
 * OBJ(int,1) = 
 *
 *****************************************************************************/
EOLS	(cfg_vtp_domain_eol, vtp_config_commands, VTP_DOMAIN);

STRING (cfg_vtp_domain_name, cfg_vtp_domain_eol, no_alt, OBJ(string,1),
          "Name of the VTP management domain");

NVGENS	(cfg_vtp_domain_nvgen, cfg_vtp_domain_name, vtp_config_commands, VTP_DOMAIN);

KEYWORD_ID(cfg_vtp_domain, cfg_vtp_domain_nvgen, ALTERNATE, 
	   leave_subcommand, TRUE, "vtp-domain", 
	   "Define/Modify a management domain and its parameters",
	   PRIV_CONF);

#undef  ALTERNATE
#define ALTERNATE cfg_vtp_domain



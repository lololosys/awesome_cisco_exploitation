/* $Id: cfg_vtp.h,v 1.2.58.1 1996/05/09 14:48:12 rbadri Exp $
 * $Source: /release/112/cvs/Xsys/vtp/cfg_vtp.h,v $
 *------------------------------------------------------------------
 * cfg_vtp.h - CLI config file for VTP global command.
 *
 * Dec 1995, Dean Cheng 
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_vtp.h,v $
 * Revision 1.2.58.1  1996/05/09  14:48:12  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 1.2.84.1  1996/04/27  07:38:14  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.2.74.1  1996/04/08  02:42:02  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.2.64.2  1996/03/25  02:21:32  cakyol
 * sync LE_Syn_Cal_V111_1_3_branch to LE_Cal_V111_1_0_3_branch
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.2.36.3  1996/03/08  18:18:53  cyoung
 * Remove 'vtp network' command.
 * Add tftp server and file name parameters to 'server' keyword
 *  and support.
 * Check managementDomainIndex range in MIB code.
 * Change to client on reception of any error (not just NVRAM error).
 * Set the updater when configured from the CLI.
 * Set the appropriate state of a VLAN in vtp_edit_vlan
 * Remove unneeded function declarations in vtp_dep.h
 * Fix memory corruption in vtp_upload_info.
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 1.2.36.2  1996/03/07  02:34:54  cyoung
 * Added vtp network command.
 * Cleanup database subcommand looping.
 * Don't require ring keyword for fddi type vlans.
 * Choose better values for default VLAN fields.
 * Assign domain index before inserting a new domain into the list.
 * Assign correct state when going through vtp_edit_vlan.
 * Add support for removing configuration as a result of VLAN deletion.
 * Rename spanning tree defines to match the MIB.
 * nv_gen support for all VTP commands.
 * set the parent_valid field correctly.
 * Branch: LE_Cal_V111_1_0_3_branch
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

LINK_TRANS(conf_vtp_extend_here, no_alt);

/******************************************************************************
 * vtp database domain <domain-name>
 * no vtp database domain <domain-name>
 *
 *****************************************************************************/
EOLS	(vtp_database_eol, vtp_config_commands, VTP_DATABASE);
STRING (vtp_database_domain_name, vtp_database_eol, no_alt, OBJ(string,1),
          "Name of the VTP management domain");
NVGENS	(vtp_database_nvgen, vtp_database_domain_name,
	 vtp_config_commands, VTP_DATABASE);
KEYWORD (vtp_database_domain, vtp_database_nvgen, no_alt, "domain",
	 "Management domain name of the database", PRIV_CONF);
KEYWORD	(vtp_database, vtp_database_domain, no_alt,
	 "database", "VTP database configuation command", PRIV_CONF);

/******************************************************************************
 * vtp running
 * no vtp running
 *
 *****************************************************************************/
EOLS	(conf_vtp_running_eol, vtp_config_commands, VTP_RUNNING);
NVGENS	(conf_vtp_nvgen, conf_vtp_running_eol, vtp_config_commands, VTP_RUNNING);
KEYWORD	(conf_vtp_running, conf_vtp_nvgen, vtp_database,
	 "run", "VTP trun on/off command", PRIV_CONF);

/******************************************************************************
 * [no] vtp domain <domain-name> [{server | client}]
 *                               [name <new-domain-name>]
 *                               [advertisement-interval <retransmit-seconds>]
 *                               [password <password>]
 *
 * OBJ(string,1) = <domain-name>
 * OBJ(int,1) = Logical OR of VTP_DOMAIN_PASSWORD, VTP_DOMAIN_ADVT_INT,
 *              VTP_DOMAIN_NEW_NAME
 * OBJ(paddr, 1) = <TFTP server>
 * OBJ(string,2) = <new-domain-name>
 * OBJ(int,2) = <retransmit-seconds>
 * OBJ(string,3) = <password>
 * OBJ(int,3) = VTP_SERVER or VTP_CLIENT
 * OBJ(string, 4) = <TFTP file name>
 *****************************************************************************/
 
pdecl(vtp_domain_server);     /* Forward reference to allow looping */
 
EOLS    (vtp_domain_eol, vtp_config_commands, VTP_DOMAIN);
 
/* password <password> */
TEXT    (vtp_domain_password_val, vtp_domain_server, no_alt,
         OBJ(string,3), "Password (8 - 64 chars)");
KEYWORD_OR (vtp_domain_password, vtp_domain_password_val,
           vtp_domain_eol,  OBJ(int,1), VTP_DOMAIN_PASSWORD,
           "password", "Set VTP password", PRIV_CONF);
 
/* advertisement-interval */
NUMBER (vtp_domain_advt_int_val, vtp_domain_server, no_alt,
         OBJ(int,2), 120, 600, "Seconds");
KEYWORD_OR (vtp_domain_advt_int, vtp_domain_advt_int_val,
            vtp_domain_password, OBJ(int,1), VTP_DOMAIN_ADVERT_INT,
           "advt-int", "VTP advertisement interval", PRIV_CONF);
 
/* name */
STRING (vtp_domain_new_name_val, vtp_domain_server, no_alt,
	OBJ(string,2), "Set new VTP domain name"); 
KEYWORD_OR (vtp_domain_new_name, vtp_domain_new_name_val,
           vtp_domain_advt_int, OBJ(int,1), VTP_DOMAIN_NEW_NAME,
           "name", "Set new VTP domain name", PRIV_CONF);
 
KEYWORD_ID (vtp_domain_client, vtp_domain_new_name, vtp_domain_new_name,
           OBJ(int, 3), VTP_CLIENT, "client", "VTP client", PRIV_CONF);
STRING (vtp_domain_tftp_file, vtp_domain_server, no_alt, 
	OBJ(string, 4), "File name for the VTP database (max 40 char).");
IPADDR_NAME (vtp_domain_tftp_server, vtp_domain_tftp_file, no_alt,
	     OBJ(paddr, 1), "Name/address of TFTP server.");
KEYWORD_ID (vtp_domain_server, vtp_domain_tftp_server,
	    vtp_domain_client, OBJ(int, 3), VTP_SERVER, "server", 
	    "VTP server", PRIV_CONF); 
 
NOPREFIX (vtp_domain_noprefix, vtp_domain_server, vtp_domain_eol);
STRING (vtp_domain_name, vtp_domain_noprefix, no_alt, OBJ(string,1),
          "Name of the VTP management domain");
NVGENS (vtp_domain_nvgen, vtp_domain_name, vtp_config_commands, VTP_DOMAIN);
KEYWORD (vtp_domain, vtp_domain_nvgen, conf_vtp_running,
          "domain", "Define/Modify a management domain and its parameters",
          PRIV_CONF);
 
KEYWORD	(configure_vtp, vtp_domain, NONE, "vtp",
	 "Global VTP configuration commands", PRIV_CONF);

#undef  ALTERNATE
#define ALTERNATE configure_vtp



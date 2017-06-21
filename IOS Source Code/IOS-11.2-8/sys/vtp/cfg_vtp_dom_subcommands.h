/* $Id: cfg_vtp_dom_subcommands.h,v 1.1.6.2 1996/06/18 00:50:50 cyoung Exp $
 * $Source: /release/112/cvs/Xsys/vtp/cfg_vtp_dom_subcommands.h,v $
 *------------------------------------------------------------------
 * cfg_vtp_dom_subcommands.h - CLI header file for VTP domain subcommands.
 *
 * March 1996, Brad Benson
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_vtp_dom_subcommands.h,v $
 * Revision 1.1.6.2  1996/06/18  00:50:50  cyoung
 * CSCdi60069:  router crashes when writing vtp info to tftp file
 * CSCdi59372:  wrong behavior when conflicting vlan info is received
 * CSCdi59933:  RSP crashes for creating-deleting-recreating vtp-domains
 * CSCdi60044:  7000 image crash
 * CSCdi60394:  [sh vtp trunk] crashes 7000
 * CSCdi60528:  rsp can't change vtp database until someone else does
 * CSCdi60541:  Attempt to delete a vlan crashes image
 * CSCdi60632:  cannot set password for vtp management-domain
 * CSCdi60384:  configuration timestamp is not displayed correctly
 * Plus some Cat5k bug fixes rolled in as well as dependant code clean up.
 * Branch: California_branch
 *
 * Revision 1.1.6.1  1996/05/09  14:48:15  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 1.1.52.1  1996/04/27  07:38:17  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.40.1  1996/04/08  02:42:05  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.24.1  1996/03/25  02:21:35  cakyol
 * sync LE_Syn_Cal_V111_1_3_branch to LE_Cal_V111_1_0_3_branch
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/22  03:15:56  bbenson
 * Branch: LE_Cal_V111_1_0_3_branch
 * Rearrange cadillac fins on the VTP command parser.  Install
 * electroglide subcommand modes for vtp-domain and vtp-database commands.
 * Running lights coming in next commit.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************************
 * vtp-domain subcommands
 * 		[{server | client}]
 *              [interval <retransmit-seconds>]
 *              [password <password>]
 *
 * OBJ(int,1) = Logical OR of VTP_DOMAIN_PASSWORD, VTP_DOMAIN_ADVT_INT,
 *              VTP_DOMAIN_NEW_NAME
 * OBJ(paddr, 1) = <TFTP server>
 * OBJ(string,2) = <new-domain-name>
 * OBJ(int,2) = <retransmit-seconds>
 * OBJ(string,3) = <password>
 * OBJ(int,3) = VTP_SERVER or VTP_CLIENT
 * OBJ(string, 4) = <TFTP file name>
 *****************************************************************************/

/*
 * client
 */
EOLS (vtp_domain_client_eol, vtp_config_commands, VTP_DOMAIN_CLIENT);

NOPREFIX (vtp_domain_client_noprefix, vtp_domain_client_eol, 
	  vtp_domain_client_eol);

NVGENS (vtp_domain_client_nvgen, vtp_domain_client_noprefix,
	vtp_config_commands, VTP_DOMAIN_CLIENT);

KEYWORD (vtp_domain_client, vtp_domain_client_nvgen,
	 no_alt, "client", 
	 "Set VTP client mode", PRIV_CONF);
 
/*
 * server <tftp-server> <file-name>
 */
EOLS (vtp_domain_server_eol, vtp_config_commands, VTP_DOMAIN_SERVER);

STRING (vtp_domain_tftp_file, vtp_domain_server_eol, no_alt,
	OBJ(string, 4), "File name for the VTP database (max 40 char).");

IPADDR_NAME (vtp_domain_tftp_server, vtp_domain_tftp_file, no_alt,
	     OBJ(paddr, 1), "Name/address of TFTP server.");

NOPREFIX (vtp_domain_server_noprefix, vtp_domain_tftp_server,
	  vtp_domain_server_eol);

NVGENS (vtp_domain_server_nvgen, vtp_domain_server_noprefix,
	vtp_config_commands, VTP_DOMAIN_SERVER);

KEYWORD (vtp_domain_server, vtp_domain_server_nvgen,
	 vtp_domain_client, "server", 
	 "Set VTP server mode", PRIV_CONF);
 
/* 
 * password <password> 
 */
EOLS (vtp_domain_password_eol, vtp_config_commands, VTP_DOMAIN_PASSWORD);

TEXT (vtp_domain_password_val, vtp_domain_password_eol, no_alt,
      OBJ(string,3), "Password (8 - 64 chars)");

TEST_MULTIPLE_FUNCS(vtp_domain_password_test, vtp_domain_password_val,
		    vtp_domain_password_eol, NONE);

NOPREFIX (vtp_domain_password_noprefix, vtp_domain_password_test, 
	  vtp_domain_password_eol);

NVGENS (vtp_domain_password_nvgen, vtp_domain_password_noprefix,
	vtp_config_commands, VTP_DOMAIN_PASSWORD);

KEYWORD (vtp_domain_password, vtp_domain_password_nvgen,
	 vtp_domain_server, "password", 
	 "Set VTP password", PRIV_CONF);

/* 
 * VTP pruning enable/disable
 */
EOLS (vtp_domain_pruning_eol, vtp_config_commands,
      VTP_DOMAIN_PRUNING); 

KEYWORD (vtp_domain_pruning, vtp_domain_pruning_eol,
	 vtp_domain_password, "pruning", "Enable pruning for this"
	 " domain", PRIV_CONF); 
/* 
 * VTP advertisement interval 
 */
EOLS (vtp_domain_interval_eol, vtp_config_commands, VTP_DOMAIN_INTERVAL);

NUMBER (vtp_domain_interval_val, vtp_domain_interval_eol, no_alt,
         OBJ(int,2), 120, 600, "Seconds");

NOPREFIX (vtp_domain_interval_noprefix, vtp_domain_interval_val,
	  vtp_domain_interval_eol);

NVGENS (vtp_domain_interval_nvgen, vtp_domain_interval_noprefix, 
	vtp_config_commands, VTP_DOMAIN_INTERVAL);

KEYWORD (vtp_domain_interval, vtp_domain_interval_nvgen, 
	 vtp_domain_pruning, "interval", 
	 "Time interval (sec) of VTP adverts", PRIV_CONF);

#undef	ALTERNATE
#define	ALTERNATE	vtp_domain_interval








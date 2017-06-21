/* $Id: cfg_ip_tftp.h,v 3.1.2.2 1996/04/30 20:55:13 jenny Exp $
 * $Source: /release/112/cvs/Xsys/ip/cfg_ip_tftp.h,v $
 *------------------------------------------------------------------
 * IP TFTP commands
 *
 * January 1996, Bruce Cole
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_ip_tftp.h,v $
 * Revision 3.1.2.2  1996/04/30  20:55:13  jenny
 * CSCdi52475:  TFTP: cannot specify source address
 * Branch: California_branch
 * Create interface for tftp boot and source interface commands if they
 * are not there when the commands are parsed.
 *
 * Revision 3.1.2.1  1996/04/15  21:15:26  bcole
 * CSCdi54649:  IP Host Modularity, IP Automatic Stub Routing feature
 * commit
 * Branch: California_branch
 *
 * Revision 3.1  1996/04/15  01:40:20  bcole
 * Placeholders for 11.2 features.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************************
 *	ip tftp boot-interface <interface>
 *	no ip tftp boot-interface 
 */
EOLS	(cip_tftp_boot_eol, ip_tftp_command, IP_TFTP_BOOT_INTERFACE);
FORWARD_INTERFACE (cip_tftp_boot_name, cip_tftp_boot_eol, no_alt,
		   OBJ(idb,1), IFTYPE_ANYSUB);
NOPREFIX(cip_tftp_boot_noprefix, cip_tftp_boot_name,
	 cip_tftp_boot_eol);
NVGENS	(cip_tftp_boot_nvgen, cip_tftp_boot_noprefix,
	 ip_tftp_command, IP_TFTP_BOOT_INTERFACE);
KEYWORD	(cip_tftp_boot, cip_tftp_boot_nvgen, NONE,
	 "boot-interface", "Force interface to use for TFTP booting",
	 PRIV_CONF|PRIV_HIDDEN);

/******************************************************************************
 *	ip tftp claim-netascii
 *	no ip tftp claim-netascii
 */
EOLS	(cip_tftp_eol, ip_tftp_command, IP_TFTPMODE);
KEYWORD	(cip_tftp_mode, cip_tftp_eol, cip_tftp_boot,
	 "claim-netascii", "tftp write operation claims netascii",
	 PRIV_CONF|PRIV_HIDDEN);


/******************************************************************************
 *      ip tftp source-interface <interface>
 *      no ip tftp source-interface <interface>
 *      OBJ(idb,1) -- IDB to pull source IP address from
 */
EOLS	(cip_tftp_source_eol, ip_tftp_command, IP_TFTP_SOURCE);
FORWARD_INTERFACE (cip_tftp_source_name, cip_tftp_source_eol, no_alt,
		   OBJ(idb,1), IFTYPE_ANYSUB);
NOPREFIX (cip_tftp_source_noprefix, cip_tftp_source_name,
          cip_tftp_source_eol);
NVGENS	(cip_tftp_source_nvgen, cip_tftp_source_noprefix,
         ip_tftp_command, IP_TFTP_SOURCE);
KEYWORD	(cip_tftp_source, cip_tftp_source_nvgen, cip_tftp_mode,
         "source-interface",
         "Specify interface for source address in TFTP connections",
         PRIV_CONF);
KEYWORD	(cip_tftp, cip_tftp_source, ALTERNATE, 
	 "tftp", "tftp configuration commands",
	 PRIV_CONF);

#undef	ALTERNATE
#define	ALTERNATE	cip_tftp

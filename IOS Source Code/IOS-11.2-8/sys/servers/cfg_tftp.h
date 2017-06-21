/* $Id: cfg_tftp.h,v 3.2 1995/11/17 18:50:45 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/servers/cfg_tftp.h,v $
 *------------------------------------------------------------------
 * C F G _ T F T P . H
 *
 * Copyright (c) 1992-1995 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_tftp.h,v $
 * Revision 3.2  1995/11/17  18:50:45  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:11:27  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/04  01:56:43  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.1  1995/06/07  22:14:13  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * [no] tftp-server system <filename> [<list>]
 * [no] tftp-server rom alias <filename> [<list>]
 * [no] tftp-server flash <filename1> [alias <filename2>] [<list>]
 *
 * OBJ(int,1) = <list>
 * OBJ(string,1) = <filename>, <filename1>
 * OBJ(string,2) = <filename2>, NULL for no alias
 */

/*
 * tftp-server flash
 */
EOLS	(conf_tftp_flash_eol, tftp_service, TFTP_FLASH);
NUMBER	(conf_tftp_flash_list, conf_tftp_flash_eol, conf_tftp_flash_eol,
	 OBJ(int,1), MINFASTACC, MAXFASTACC,
	 "IP access list of requesting hosts");

STRING	(conf_tftp_flash_alias_file, conf_tftp_flash_list, no_alt,
	 OBJ(string,2), "Flash file alias");
KEYWORD (conf_tftp_flash_alias, conf_tftp_flash_alias_file,
	 conf_tftp_flash_list, 
	 "alias", "Flash file alias", PRIV_CONF);

NOPREFIX(conf_tftp_flash_noprefix, conf_tftp_flash_alias,
	 conf_tftp_flash_eol);

STRING	(conf_tftp_flash_file, conf_tftp_flash_noprefix, no_alt,
	 OBJ(string,1), "Flash filename");
NVGENS	(conf_tftp_flash_nvgen, conf_tftp_flash_file, tftp_service,
	 TFTP_FLASH);
KEYWORD	(conf_tftp_flash, conf_tftp_flash_nvgen, no_alt,
	 "flash", "Allow flash file TFTP load requests",
	 PRIV_CONF);

/*
 * tftp-server rom
 */

EOLS	(conf_tftp_rom_eol, tftp_service, TFTP_ROM);
NUMBER	(conf_tftp_rom_list, conf_tftp_rom_eol, conf_tftp_rom_eol,
	 OBJ(int,1), MINFASTACC, MAXFASTACC,
	 "IP access list of requesting hosts");
NOPREFIX(conf_tftp_rom_noprefix, conf_tftp_rom_list, conf_tftp_rom_eol);
STRING	(conf_tftp_rom_file, conf_tftp_rom_noprefix, no_alt,
	 OBJ(string,1), "ROM alias");

KEYWORD (conf_tftp_rom_alias, conf_tftp_rom_file, no_alt,
	 "alias", "ROM alias", PRIV_CONF);
NVGENS	(conf_tftp_rom_nvgen, conf_tftp_rom_alias, tftp_service, TFTP_ROM);
KEYWORD	(conf_tftp_rom, conf_tftp_rom_nvgen, conf_tftp_flash,
	 "rom", "Allow ROM image TFTP load requests",
	 PRIV_CONF);

/*
 * tftp-server system
 */
EOLS	(conf_tftp_sys_eol, tftp_service, TFTP_SYSTEM);
NUMBER	(conf_tftp_sys_list, conf_tftp_sys_eol, conf_tftp_sys_eol,
	 OBJ(int,1), MINFASTACC, MAXFASTACC,
	 "IP access list of requesting hosts");
NOPREFIX(conf_tftp_sys_noprefix, conf_tftp_sys_list, conf_tftp_sys_eol);
STRING	(conf_tftp_sys_file, conf_tftp_sys_noprefix, no_alt,
	 OBJ(string,1), "System filename");

NVGENS	(conf_tftp_sys_nvgen, conf_tftp_sys_file, tftp_service, TFTP_SYSTEM);
KEYWORD	(conf_tftp_sys, conf_tftp_sys_nvgen, conf_tftp_rom,
	 "system", "Allow system image TFTP load requests",
	 PRIV_CONF|PRIV_HIDDEN);

/******************************************************************/
KEYWORD	(conf_tftp, conf_tftp_sys, ALTERNATE,
	 "tftp-server", "Provide TFTP service for netload requests",
	 PRIV_CONF);

#undef	ALTERNATE
#define	ALTERNATE	conf_tftp

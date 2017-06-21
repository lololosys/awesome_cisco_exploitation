/* $Id: cfg_boot.h,v 3.4.28.3 1996/06/17 22:37:01 widmer Exp $
 * $Source: /release/112/cvs/Xsys/parser/cfg_boot.h,v $
 *------------------------------------------------------------------
 * C F G _ B O O T . H
 *
 * Copyright (c) 1992-1997 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_boot.h,v $
 * Revision 3.4.28.3  1996/06/17  22:37:01  widmer
 * CSCdi60252:  boot system commands doesn;t attempt to resolve names
 * Branch: California_branch
 * Change Ipaddr to Ipaddr_Name
 *
 * Revision 3.4.28.2  1996/05/17  11:39:10  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.4.14.2  1996/05/05  23:41:33  ppearce
 * Sync to IbuMod_Calif_baseline_960504
 *
 * Revision 3.4.14.1  1996/05/02  22:06:50  hampton
 * Remove support for old Multibus token ring cards.
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.4.28.1  1996/04/15  21:20:33  bcole
 * CSCdi54649:  IP Host Modularity, IP Automatic Stub Routing feature
 * commit
 * Branch: California_branch
 *
 * Revision 3.4  1996/01/18  02:14:14  tli
 * CSCdi47063:  show ip cache cannot be restricted to subinterfaces
 * Change IFTYPE_ANY to IFTYPE_HWIDB
 *
 * Revision 3.3  1995/12/14  13:07:42  widmer
 * CSCdi45510:  Add General_String macro to replace other string macros
 * Change Var_String, String, Txt_Line, and Text to use General_String
 * Add fail argument to Priv_Test
 * Change Neg1 to use Keyword_Id
 * Remove Sense macro
 *
 * Revision 3.2  1995/11/17  18:42:26  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:49:47  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:01:51  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "file.h"
#include "../os/boot.h"

/*
 * XXX add flash [<file>]
 *
 * This command is complex.  From os/boot.c:
 *====
     * If the command has one argument, then we use that as a filename
     * and use the default boot protocol.
     *
     * If the command has two arguments, then we check to see if the
     * first one looks like a protocol name. If it does, then we use
     * the second argument as a filename, and use the selected protocol.
     * If it doesn't look like a protocol name, we use the default
     * protocol and assume that the first argument is a filename and
     * the second argument is a hostname.
     *
     * If the command has more than two arguments, then it is new
     * style, and the first parameter is required to be a protocol
     * selector.
 *====
 *
 * The parse chain state diagram looks like this:
 *
 *              |-tftp--(OBJ(int,2)=TFTP)-|        |-<ipaddr>---------------|
 *              |                         |-<file>-|                        |
 *              |-------(OBJ(int,2)=TFTP)-|        |------------------------|
 * boot-|<type>-|                                                           |--EOL
 *      |       |                                  |-<enaddr>-|-<interface>-|
 *      |       |-mop--(OBJ(int,2)=MOP)--<file>----|          |             |
 *      |       |                                  |----------|-------------|
 *      |       |                                                           |
 *      |       |-flash--(OBJ(int,2)=FLASH-|--<file>------------------------|
 *      |       |                          |--------------------------------|
 *      |       |                                                           |
 *      |       |                                                           |
 *      |       |--if(sense==FALSE)-----------------------------------------|
 *      |                                                                   |
 *      |system----rom------------------------------------------------------|
 */


/******************************************************************
 * [no] boot {<type>} tftp <filename> [<ipaddr>]
 * [no] boot {<type>} rcp  <filename> [<ipaddr>]
 * [no] boot {<type>} mop <filename> [{ <enaddr> | <interface> }]
 * [no] boot {<type>} flash [<filename>]
 * [no] boot system rom
 * no boot bootstrap <type>
 *
 * <type> = { bootstrap | host | network | system }
 *
 * csb->which = BOOT_FILE
 * OBJ(int,1) = BOOT_BOOT, BOOT_HOST, BOOT_MODULE, BOOT_NET, BOOT_SYS
 * OBJ(int,2) = FILE_ACCESS_MOP, FILE_ACCESS_TFTP, FILE_ACCESS_FLASH,
                FILE_ACCESS_ROM, FILE_ACCESS_RCP
 * OBJ(string,1) = <filename>
 * (*OBJ(paddr,1)) = <ipaddr>
 * (*OBJ(hwaddr,1)) = <enaddr>
 * OBJ(idb,1) = <interface>
 */
EOLS	(conf_boot_eol, boot_command, BOOT_FILE);

/* [<ipaddr>] */
IPADDR_NAME(conf_boot_tuba_ipaddr, conf_boot_eol, conf_boot_eol,
	    OBJ(paddr,1),
	    "Address from which to download the boot config file");

/* <file> */
/*
 * make the default tftp for backwards compatibility w/ old boot roms...
 */
SET     (conf_boot_def_file_set, conf_boot_tuba_ipaddr,
         OBJ(int,2), FILE_ACCESS_TFTP);

STRING	(conf_boot_def_file, conf_boot_def_file_set, no_alt,
	 OBJ(string,1), "System image filename");
TESTVAR	(conf_boot_default, conf_boot_def_file, no_alt,
	 NONE, NONE, NONE,
	 OBJ(int,2), BOOT_NONE);

LINK_TRANS(conf_boot_sys_extend_here, no_alt);

/* flash [<file>] */
STRING	(conf_boot_flash_file, conf_boot_eol, conf_boot_eol,
	 OBJ(string,1), "Configuration filename");
KEYWORD_ID (conf_boot_flash_kwd, conf_boot_flash_file,conf_boot_sys_extend_here,
	    OBJ(int,2), FILE_ACCESS_FLASH,
	    "flash", "Boot from flash memory", PRIV_CONF);
PRIV_TEST(conf_boot_flash_priv, conf_boot_flash_kwd, NONE, NONE,
	  PRIV_CONF | PRIV_HIDDEN);
TESTVAR	(conf_boot_flash_test, conf_boot_flash_kwd, conf_boot_flash_priv,
	 NONE, NONE, NONE, OBJ(int,1), BOOT_SYS);
NOP	(conf_boot_flash, conf_boot_flash_test, conf_boot_default);

/* tftp <file> */
STRING	(conf_boot_tftp_file, conf_boot_tuba_ipaddr, no_alt,
	 OBJ(string,1), "Configuration filename");
KEYWORD_ID (conf_boot_tftp, conf_boot_tftp_file, conf_boot_flash,
	    OBJ(int,2), FILE_ACCESS_TFTP,
	    "tftp", "Boot from a tftp server", PRIV_CONF);

/* rom */
KEYWORD_ID (conf_boot_rom, conf_boot_eol, NONE,
	    OBJ(int,2), FILE_ACCESS_ROM,
	    "rom", "Boot from rom", PRIV_CONF);
PRIV_TEST (rom_privtest, conf_boot_rom, NONE, NONE, PRIV_HIDDEN);
TEST_FUNC (conf_boot_rom_test_bootflash_exists, 
	   rom_privtest, conf_boot_rom, NONE, bootflash_exists);
TESTVAR	(conf_boot_rom_test, conf_boot_rom_test_bootflash_exists, 
	 NONE, NONE, NONE, conf_boot_tftp,
	 OBJ(int,1), BOOT_SYS);

/* mop <file> [<enaddr>] [<interface>] */
INTERFACE (conf_boot_mop_if, conf_boot_eol, conf_boot_eol,
	   OBJ(idb,1), IFTYPE_HWIDB     );
ENADDR	(conf_boot_mop_enaddr, conf_boot_mop_if, conf_boot_mop_if,
	 OBJ(hwaddr,1), "MAC layer address of host to boot from");
STRING	(conf_boot_mop_file, conf_boot_mop_enaddr, no_alt,
	 OBJ(string,1), "Configuration filename");
KEYWORD_ID (conf_boot_mop, conf_boot_mop_file, conf_boot_rom_test,
	    OBJ(int,2), FILE_ACCESS_MOP,
	    "mop", "Boot from a Decnet MOP server", PRIV_CONF);

/* EOL? */
TESTVAR	(conf_boot_noprefix, conf_boot_eol, NONE,
	 NONE, NONE, conf_boot_mop,
	    sense, FALSE);
NVGENS	(conf_boot_nvgen, conf_boot_noprefix, boot_command, BOOT_FILE);


LINK_TRANS(conf_boot_config_extend_here, no_alt);

/* { bootstrap | host | module | network | system } */

KEYWORD_ID (conf_boot_module, conf_boot_nvgen, conf_boot_config_extend_here,
	    OBJ(int,1), BOOT_MODULE,
	    "module", "", PRIV_OPR | PRIV_NONVGEN | PRIV_HIDDEN);
KEYWORD_ID (conf_boot_sys, conf_boot_nvgen, conf_boot_module,
	    OBJ(int,1), BOOT_SYS,
	    "system", "System image file", PRIV_CONF);
KEYWORD_ID (conf_boot_net, conf_boot_nvgen, conf_boot_sys,
	    OBJ(int,1), BOOT_NET,
	    "network", "Network-wide config file", PRIV_CONF);
KEYWORD_ID (conf_boot_host, conf_boot_nvgen, conf_boot_net,
	    OBJ(int,1), BOOT_HOST,
	    "host", "Router-specific config file", PRIV_CONF);
KEYWORD_ID (conf_boot_boot, conf_boot_nvgen, conf_boot_host,
	    OBJ(int,1), BOOT_BOOT,
	    "bootstrap", "Bootstrap image file", PRIV_CONF);

/******************************************************************
 * boot buffersize <bytes>
 * no boot buffersize [<bytes>]
 *
 * OBJ(int,1) = <bytes>
 */
PARAMS_KEYONLY (conf_boot_bufsize, conf_boot_boot,
	       "buffersize", OBJ(int,1), 0, -1,
		boot_command, BOOT_BUFFERSIZE,
		"Specify the buffer size for netbooting a config file",
		"Buffer size", PRIV_CONF);

/******************************************************************/
KEYWORD	(conf_boot, conf_boot_bufsize, ALTERNATE,
	 "boot", "Modify system boot parameters", PRIV_CONF);

#undef	ALTERNATE
#define	ALTERNATE	conf_boot

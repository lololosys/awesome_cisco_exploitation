/* $Id: cfg_int_snmp.h,v 3.2 1995/11/17 18:58:17 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/snmp/cfg_int_snmp.h,v $
 *------------------------------------------------------------------
 * C F G _ I N T _ S N M P . H
 *
 * Copyright (c) 1994-1995 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_int_snmp.h,v $
 * Revision 3.2  1995/11/17  18:58:17  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:16:03  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/04  01:57:07  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.2  1995/06/21  18:16:56  widmer
 * CSCdi36227:  Fix parse chain alternates
 *
 * Revision 2.1  1995/06/07  22:07:22  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * snmp trap link-status 	(default)
 * [no] snmp trap link-status
 */
EOLS	(ci_snmp_trap_link_eol, snmp_int_service, SNMP_NOLINK);
KEYWORD	(ci_snmp_trap_link, ci_snmp_trap_link_eol, no_alt,
	 "link-status", 
	 "Allow SNMP LINKUP and LINKDOWN traps", PRIV_CONF);

/******************************************************************/
KEYWORD	(ci_snmp_trap, ci_snmp_trap_link, no_alt,
	 "trap", "Allow a specific SNMP trap", PRIV_CONF);

/******************************************************************/
KEYWORD	(conf_int_snmp, ci_snmp_trap, ALTERNATE,
	 "snmp", "Modify SNMP interface parameters", PRIV_CONF);

#undef	ALTERNATE
#define	ALTERNATE	conf_int_snmp

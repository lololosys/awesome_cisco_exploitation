/* $Id: exec_configure.h,v 3.4 1996/03/09 22:58:02 widmer Exp $
 * $Source: /release/111/cvs/Xsys/ui/exec_configure.h,v $
 *------------------------------------------------------------------
 * E X E C _ C O N F I G U R E . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_configure.h,v $
 * Revision 3.4  1996/03/09  22:58:02  widmer
 * CSCdi47890:  HTTP server doesnt work
 * Add HTTP server
 *
 * Revision 3.3  1996/01/30  19:16:05  foster
 * CSCdi47228:  HSA slave image needs config parsing disabled
 *
 * Revision 3.2  1995/11/17  17:47:50  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:41:01  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/04  01:59:24  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.1  1995/06/07  22:16:50  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 * configure [{ terminal | memory | overwrite-network | network }]
 *
 * The 'memory' keyword only appears if the system has NV memory.
 *
 * csb->which = PARSER_CONF_TERM, PARSER_CONF_MEM, PARSER_CONF_NET,
 *		PARSER_CONF_NONE
 */
EOLS	(exec_config_eol, manual_configure, PARSER_CONF_NONE);

LINK_TRANS	(exec_config_extend_here, exec_config_eol);

EOLS	(exec_config_net_eol, manual_configure, PARSER_CONF_NET);
KEYWORD_ID (exec_config_net, exec_config_net_eol, exec_config_extend_here,
	    OBJ(int,1), COPY_TFTP,
	    "network", "Configure from a TFTP network host", PRIV_ROOT);

EOLS	(exec_config_overwritenet_eol, manual_configure,
	 PARSER_CONF_OVERWRITE_NET );
KEYWORD_ID (exec_config_overwritenet, exec_config_overwritenet_eol,
	    exec_config_net, OBJ(int,1), COPY_TFTP, "overwrite-network", 
	    "Overwrite NV memory from TFTP network host",
	    PRIV_ROOT);

EOLS	(exec_config_mem_eol, manual_configure, PARSER_CONF_MEM);
KEYWORD	(exec_config_memkwd, exec_config_mem_eol, NONE,
	 "memory", "Configure from NV memory", PRIV_ROOT);
ASSERT	(exec_config_mem, exec_config_memkwd, exec_config_overwritenet,
	 nvsize);

EOLS	(exec_config_term_eol, manual_configure, PARSER_CONF_TERM);
KEYWORD	(exec_config_term, exec_config_term_eol, exec_config_mem,
	 "terminal", "Configure from the terminal", PRIV_ROOT);

EOLS	(exec_config_http_eol, manual_configure, PARSER_CONF_HTTP);
KEYWORD	(exec_config_http, exec_config_http_eol, no_alt,
	 "http", "Configure from http", PRIV_ROOT);

IFELSE	(exec_config_ifelse, exec_config_http, exec_config_term,
	 (csb->flags & CONFIG_HTTP));

KEYWORD	(exec_configure, exec_config_ifelse, NONE,
	 "configure", "Enter configuration mode", PRIV_ROOT);

ASSERT	(exec_configure_test, exec_configure, ALTERNATE,
	 !reg_invoke_slave_test());

#undef	ALTERNATE
#define	ALTERNATE	exec_configure_test

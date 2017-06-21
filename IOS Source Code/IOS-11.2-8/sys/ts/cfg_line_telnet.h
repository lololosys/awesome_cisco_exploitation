/* $Id: cfg_line_telnet.h,v 3.3 1996/02/01 23:34:46 widmer Exp $
 * $Source: /release/111/cvs/Xsys/ts/cfg_line_telnet.h,v $
 *------------------------------------------------------------------
 * C F G _ L I N E _ T E L N E T . H
 *
 * Copyright (c) 1992-1997 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_line_telnet.h,v $
 * Revision 3.3  1996/02/01  23:34:46  widmer
 * CSCdi45325:  Change Keyword/Set to use Keyword_Id macro
 *
 * Revision 3.2  1995/11/17  18:53:57  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:38:47  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/04  01:58:50  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.2  1995/06/09  23:40:10  achopra
 * CSCdi04836:  Provide ip-on-break functionality on TS
 *
 * Revision 2.1  1995/06/07  22:10:49  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/***************************************************************
 * telnet { break-on-ip | refuse-negotiations | speed |
 *	    sync-on-break | transparent | ip-on-break }
 *
 */
EOLS	(line_telnet_eol, telnet_command, PARSER_LINE_TELNET);

KEYWORD_ID(telnet_ipbreak, line_telnet_eol, no_alt,
           OBJ(int,1), TN_IPBREAK,
           "ip-on-break",
           "Send interrupt signal when break is received", PRIV_USER);

KEYWORD_ID(telnet_transparent, line_telnet_eol, telnet_ipbreak,
	   OBJ(int,1), TN_TRANS,
	   "transparent",
	   "Send a CR as a CR followed by a NULL "
	   "instead of a CR followed by a LF",
	   PRIV_USER);
KEYWORD_ID(telnet_sync, line_telnet_eol, telnet_transparent,
	   OBJ(int,1), TN_SYNC,
	   "sync-on-break",
	   "Send a Telnet Synchronize signal after "
	   "receiving a Telnet Break signal",
	   PRIV_USER);

SET	(telnet_speed_max_set, line_telnet_eol, OBJ(int,3), 38400);
NUMBER	(telnet_speed_max, line_telnet_eol, telnet_speed_max_set,
	 OBJ(int,3), 1, -1, "Maximum speed");
NUMBER	(telnet_speed_default, telnet_speed_max, telnet_speed_max_set,
	 OBJ(int,2), 1, -1, "Default speed");
NOPREFIX(telnet_speed_noprefix, telnet_speed_default, line_telnet_eol);
NVGENS	(telnet_speed_nvgen, telnet_speed_noprefix,
	 telnet_command, PARSER_LINE_TELNET);
KEYWORD_ID(telnet_speed, telnet_speed_nvgen, telnet_sync,
	   OBJ(int,1), TN_SPEED,
	   "speed", "Specify line speeds", PRIV_USER);

KEYWORD_ID(telnet_refuse, line_telnet_eol, telnet_speed,
	   OBJ(int,1), TN_REFUSE, "refuse-negotiations",
	   "Suppress negotiations of Telnet Remote Echo "
	   "and Suppress Go Ahead options",
	   PRIV_USER);
KEYWORD_ID(telnet_breakip, line_telnet_eol, telnet_refuse,
	   OBJ(int,1), TN_BREAKIP,
	   "break-on-ip",
	   "Send break signal when interrupt is received", PRIV_USER);

KEYWORD	(line_telnet, telnet_breakip, ALTERNATE,
	 "telnet",
	 "Telnet protocol-specific configuration", PRIV_USER);

#undef	ALTERNATE
#define	ALTERNATE	line_telnet

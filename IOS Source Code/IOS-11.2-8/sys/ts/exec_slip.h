/* $Id: exec_slip.h,v 3.3.46.1 1996/08/12 16:08:37 widmer Exp $
 * $Source: /release/112/cvs/Xsys/ts/exec_slip.h,v $
 *------------------------------------------------------------------
 * E X E C _ S L I P . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_slip.h,v $
 * Revision 3.3.46.1  1996/08/12  16:08:37  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.3  1995/12/15  22:28:19  widmer
 * CSCdi45685:  Need priv flag for interactive commands
 * Add Priv_Interactive to flag interactive commands
 *
 * Revision 3.2  1995/11/17  18:54:41  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:39:17  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/10/11  05:59:28  fox
 * CSCdi40958:  PPP address negotiation doesnt work with dialer interfaces
 * Call ip_pooling_available() with a NULL pointer if VTY is not yet setup
 *
 * Revision 2.2  1995/09/18  19:38:44  fox
 * CSCdi40523:  Add IP Address Pooling support for ISDN and other sync
 * interfaces
 * Modify IP address pooling to support all Point-to-Point interfaces
 * Remove Odd/Even Address feature
 * Add 'ppp negotiate' command
 *
 * Revision 2.1  1995/07/04  01:59:00  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.1  1995/06/07  22:26:42  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * slip { default | <ipaddr> | <ip-hostname> } { /compressed } { /routing }
 * ppp { default | <ipaddr> | <ip-hostname> | negotiate } { /compressed } { /routing }
 *
 * OBJ(int,1)	= TRUE => /compressed was specified
 * OBJ(int,2)	= TRUE => /routing was specified
 * OBJ(int,3)	= TRUE => ppp specified
 * OBJ(int,4)	= TRUE => default specified
 * OBJ(int,5)	= TRUE => negotiate specified
 * OBJ(int,8)	& SLIP_NONE 		 => No argument (0x0)
 *		& SLIP_OPTION_COMPRESSED => /compressed was specified (0x1)
 *		& SLIP_OPTION_ROUTING	 => /routing was specified (0x2)
 *		& SLIP_OPTION_DEFAULT	 => default was specified  (0x4)
 *	      	& SLIP_OPTION_IPADDR	 => ipaddr/hostname was specified (0x8)
 *	      	& SLIP_OPTION_NEGOTIATE	 => negotiate was specified (0x10)
 *
 * (*OBJ(paddr,1)) = <ipaddr>
 * OBJ(string,1) = <ip-hostname>
 */

pdecl (exec_slip_opts);

EOLNS	(exec_slip_eol, slip_exec_command);

EVAL(exec_slip_ipaddr_set, exec_slip_eol,
     GETOBJ(int, 8) |= SLIP_OPTION_IPADDR);
STRING (exec_slip_ipaddr, exec_slip_ipaddr_set, NONE,
	    OBJ(string,1), "Desired IP address or hostname");
ASSERT (exec_slip_address, exec_slip_ipaddr, NONE,
        (stdio->slip_flags & SLIP_DYNAMIC));
TEST_MULTIPLE_FUNCS(exec_slip_chkmult, exec_slip_address,
		    no_alt, exec_slip_eol);

SET	(exec_slip_negotiate_set, exec_slip_eol, OBJ(int, 5), TRUE);
KEYWORD_OR(exec_slip_negotiate_kwd, exec_slip_negotiate_set, NONE,
	   OBJ(int, 8), SLIP_OPTION_NEGOTIATE,
	   "negotiate", "Use PPP negotiated IP address", PRIV_USER);
ASSERT (exec_slip_negotiate, exec_slip_negotiate_kwd, exec_slip_chkmult,
	GETOBJ(int,3));

SET	(exec_slip_default_set, exec_slip_eol, OBJ(int, 4), TRUE);
KEYWORD_OR(exec_slip_default_kwd, exec_slip_default_set, NONE,
	   OBJ(int, 8), SLIP_OPTION_DEFAULT,
	   "default", "Use configured default IP address", PRIV_USER);
ASSERT (exec_slip_default, exec_slip_default_kwd, exec_slip_negotiate,
	((stdio->tty_idb && stdio->tty_idb->firstsw &&
	 ip_pooling_available(stdio->tty_idb->firstsw)) ||
	 ip_pooling_available(NULL)));

/* The address (or default) must be specified as the first argument on
 * the line, and dynamic address assignment must be supported.
 */

SET	(exec_slip_routing_set, exec_slip_opts, OBJ(int,2), TRUE);
KEYWORD_OR(exec_slip_routing_kwd, exec_slip_routing_set, NONE,
	   OBJ(int,8), SLIP_OPTION_ROUTING,
	   "/routing", "Enable dynamic routing", PRIV_USER);
ASSERT (exec_slip_routing, exec_slip_routing_kwd, NONE,
        ((stdio->slip_flags & SLIP_DYN_ROUTING) &&
	 ((GETOBJ(int, 8) & SLIP_OPTION_ROUTING) == 0)));

SET	(exec_slip_compressed_set, exec_slip_opts, OBJ(int,1), TRUE);
KEYWORD_OR(exec_slip_compressed_kwd, exec_slip_compressed_set, NONE,
	   OBJ(int,8), SLIP_OPTION_COMPRESSED,
	   "/compressed", "Turn on IP header compression", PRIV_USER);
ASSERT (exec_slip_compressed, exec_slip_compressed_kwd, exec_slip_routing,
        (((stdio->tty_idb && stdio->tty_idb->tcp_compress) ||
          reg_invoke_slip_pt_supports_compression(stdio)) &&
          ((GETOBJ(int, 8) & SLIP_OPTION_COMPRESSED) == 0)));

NOP (exec_slip_opts, exec_slip_compressed, exec_slip_default);

KEYWORD_ID(exec_ppp, exec_slip_opts, ALTERNATE,
	   OBJ(int,3), TRUE,
	   "ppp", "Start IETF Point-to-Point Protocol (PPP)",
	   PRIV_USER | PRIV_INTERACTIVE);

KEYWORD	(exec_slip, exec_slip_opts, exec_ppp,
	 "slip", "Start Serial-line IP (SLIP)",
	 PRIV_USER | PRIV_INTERACTIVE);

#undef	ALTERNATE
#define	ALTERNATE	exec_slip

/* $Id$
 * $Source$
 *------------------------------------------------------------------
 * Clear TCP parser chain
 * 
 * December 1996, Jenny Yuan
 *
 * Copyright (c) 1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log$
 *------------------------------------------------------------------
 * $Endlog$
 */

LINK_TRANS (clear_tcp_extend, no_alt);

/*
 * clear tcp local <local addr> <local port> remote <remote addr> <port>
 */
EOLS	(clear_tcp_addr_eol, clear_tcp, CLEAR_TCP_BY_PORT);
NUMBER	(clear_tcp_fport, clear_tcp_addr_eol, no_alt,
	OBJ(int,2), 1, (1 << 16) - 1, "Remote TCP port"); 
IPADDR_NAME (clear_tcp_faddr, clear_tcp_fport, no_alt,
	    OBJ(paddr,2), "Remote host name or IP address"); 
KEYWORD	(clear_tcp_remote, clear_tcp_faddr, no_alt,
	"remote", "Remote host address/port", PRIV_OPR);
NUMBER	(clear_tcp_lport, clear_tcp_remote, no_alt,
	OBJ(int,1), 1, (1 << 16) - 1, "Local TCP port"); 
IPADDR_NAME (clear_tcp_laddr, clear_tcp_lport, no_alt,
	    OBJ(paddr,1), "Local host name or IP address"); 
KEYWORD	(clear_tcp_local, clear_tcp_laddr, no_alt,
	 "local", "Local host address/port", PRIV_OPR);

/*
 * clear tcp line ...
 */
EOLS	(clear_tcp_line_eol, clear_tcp, CLEAR_TCP_BY_LINE);
TTYLINE (clear_tcp_line_num, clear_tcp_line_eol, no_alt,
	OBJ(int,1), "Line number");
KEYWORD	(clear_tcp_line, clear_tcp_line_num, clear_tcp_local,
	 "line", "TTY line", PRIV_OPR);

/*
 * clear tcp tcb <tcb address>
 */
EOLS	(clear_tcp_tcb_eol, clear_tcp, CLEAR_TCP_BY_TCB);
OPT_HEXNUM (clear_tcp_tcb_addr, clear_tcp_tcb_eol, no_alt,
	    OBJ(int, 1), "TCB address");
KEYWORD	(clear_tcp_tcb, clear_tcp_tcb_addr, clear_tcp_line,
	 "tcb", "TCB address", PRIV_OPR);

/******************************************************************
 * clear tcp
 */
KEYWORD (clear_tcp, clear_tcp_tcb, ALTERNATE,
	 "tcp", "Clear a TCP connection or statistics", PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	clear_tcp

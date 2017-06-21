/* $Id: tcp_chain.c,v 3.7.4.3 1996/05/21 10:06:24 thille Exp $
 * $Source: /release/112/cvs/Xsys/tcp/tcp_chain.c,v $
 *------------------------------------------------------------------
 * TCP parser chains
 *
 * May 1993, Robert Widmer
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: tcp_chain.c,v $
 * Revision 3.7.4.3  1996/05/21  10:06:24  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.7.4.2  1996/04/15  21:23:44  bcole
 * CSCdi54649:  IP Host Modularity, IP Automatic Stub Routing feature
 * commit
 * Branch: California_branch
 *
 * Revision 3.7.4.1  1996/03/28  00:16:06  jenny
 * CSCdi51402:  TCP Path-MTU Discovery needs to be dynamic
 * Branch: California_branch
 * Implemented TCP PMTU dynamic discovery and PMTU timers (RFC 1191).
 * Moved ip tcp parser command into a seperated parser chain in TCP for
 * modularity.
 *
 * Revision 3.7  1996/03/10  07:43:33  ahh
 * CSCdi50817:  TCP: add ttcp to some images
 * (Patch 1 of 2)
 *
 * Revision 3.6  1996/02/22  14:39:01  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.5  1996/01/29  07:30:22  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.4  1996/01/25  11:22:52  smackie
 * Registry code gets some much-needed hot-oil massaging, free liposuction
 * and a makeover.  (CSCdi47101)
 *
 *    o Compress registry structures. Saves over 120K for all platforms
 *    o Add registry subsystems and remove feature registry initializion
 *      from registry.c to fix woeful initialization scaling problems
 *    o Remove unused and seemingly unloved registry debugging code
 *    o Add registry memory statistics to shame people into action
 *
 * Revision 3.3  1995/12/17  18:39:19  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  18:48:30  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:35:41  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/07/01  12:56:35  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize ip, ipmulticast, tcp
 *
 * Revision 2.2  1995/06/17  06:26:30  ahh
 * CSCdi35734:  TCP: Long overdue code reorganization
 * Separate tcp.h into public and private header files.  Shuffle functions
 * around into more logical groupings.  Rename functions to meet naming
 * conventions (say goodbye to StudlyCaps).  Delete unneeded externs.
 * Trim off old log messages.
 *
 * Revision 2.1  1995/06/07  23:05:39  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "config.h"
#include "parser.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../tcp/tcp.h"
#include "../tcp/tcpinternal.h"
#include "../tcp/rcp.h"
#include "../parser/parser_defs_tcpip.h"
#include "../parser/parser_defs_snmp.h"
#include "../parser/parser_defs_parser.h"
#include "../parser/parser_defs_exec.h"
#include "registry.h"
#include "../snmp/snmp_api.h"
#include "../snmp/snmp_registry.regh"
#include "interface_private.h"
#include "../tcp/tcp_debug.h"
#include "telnet_debug.h"
#include "../ip/ip_actions.h"
#include "../ip/ip_macros.h"


/*
 * tcp_command
 * Process TCP config commands
 */

void tcp_command (parseinfo *csb)
{
    switch (csb->which) {
    default:
	bad_parser_subcommand(csb, csb->which);
	break;
    case IP_TCP_CHUNKSZ:		    /* tcp chunk-size */
	if (csb->nvgen) {
	    nv_write(tcp_chunksize,
		     "%s %d", csb->nv_command, tcp_chunksize);
	    return;
	}
	tcp_chunksize = csb->sense ? GETOBJ(int,1) : 0;
	break;
	
    case IP_TCP_WINDSZ:		    /* tcp window-size */
	if (csb->nvgen) {
	    nv_write(tcp_defrcvwnd != TCP_DEFRCVWND,
		     "%s %d", csb->nv_command, tcp_defrcvwnd);
	    return;
	}
	tcp_defrcvwnd = csb->sense ? GETOBJ(int,1) : TCP_DEFRCVWND;
	break;
	
    case IP_TCP_FINWAITTM:
	if (csb->nvgen) {
	    nv_write(tcp_finwaittm != DEF_FINWAITTM, "%s %d",
		     csb->nv_command, tcp_finwaittm);
	    return;
	}
	
	if (csb->sense) 
	    tcp_finwaittm = GETOBJ(int,1);
	else
	    tcp_finwaittm = DEF_FINWAITTM;
	break;
	
    case IP_TCP_QUEUEMAX:
	if (csb->nvgen) {
	    nv_write(tcp_queuemax != DEF_QUEUEMAX_USER, "%s %d",
		     csb->nv_command, tcp_queuemax);
	    return;
	}
	
	if (csb->sense) 
	    tcp_queuemax = GETOBJ(int,1);
	else
	    tcp_queuemax = DEF_QUEUEMAX_USER;
	break;
	
    case IP_TCP_SYNWAIT:
	if (csb->nvgen) {
	    nv_write(tcp_synwait != DEF_SYNWAIT, "%s %d",
		     csb->nv_command, tcp_synwait);
	    return;
	}
	
	if (csb->sense) 
	    tcp_synwait = GETOBJ(int,1);
	else
	    tcp_synwait = DEF_SYNWAIT;
	break;

    case IP_TCP_PATHMTU:
	if (csb->nvgen) {
	    if (tcp_pathmtu != DEF_PATHMTU) {
	    	nv_write(TRUE, "%s", csb->nv_command);
	    	if (tcp_pathmtu_ager != DEF_PATHMTU_AGER) { 
		    if (tcp_pathmtu_ager == NO_PATHMTU_AGER)
		    	nv_add(TRUE, " age-timer infinite");
		    else 
		    	nv_add(TRUE, " age-timer %d", tcp_pathmtu_ager);
		}
	    }	
	    return;
	} 
	tcp_pathmtu = csb->sense;
	/*
	 * If TCP Pathmtu-Discovery is configured, get the age-timer 
	 * and start PMTUTIMEOUT timer as needed. Otherwise, stop the
	 * timer.
	 */
	if (tcp_pathmtu) {
	    tcp_pathmtu_ager = GETOBJ(int,1);
	    /* 
  	     * If age-timer is not "infinite", start PMTUTIMEOUT timer.
	     * Otherwise, stop the timer. 
	     */
	    if (tcp_pathmtu_ager != NO_PATHMTU_AGER) {
	    	tcp_pmtutimer_on();
	    } else {
	    	tcp_pmtutimer_off();
	    }
	} else { 
	    tcp_pmtutimer_off();
	}
	break;

    case IP_TELNET_SOURCE:
	if (csb->nvgen) {
	    if (telnet_source_idb) {
		nv_write(TRUE, "%s %s", csb->nv_command,
			 telnet_source_idb->namestring);
	    }
	} else {
	    if (csb->sense)
		telnet_source_idb = GETOBJ(idb,1);
	    else
		telnet_source_idb = NULL;
	}
	break;

    case IP_TCP_TUNNEL:
	if (csb->nvgen) {
	    nv_write(tcptunnel_enable != DEF_TUNNEL_ENABLE, "%s%s",
		     DEF_TUNNEL_ENABLE ? "no " : "", csb->nv_command);
	} else {
	    tcptunnel_enable = csb->sense;
	}
	break;
    }	
}

/*
 * clear_tcp
 * Process clear tcp commands
 */
void 
clear_tcp (parseinfo *csb)
{
    addrtype *laddr, *faddr;
    ushort lport, fport;
    tcbtype *tcb;
    int line;
    boolean tcp_cleared;

    switch (csb->which) {
	default:
            bad_parser_subcommand(csb, csb->which);
            break;
	case CLEAR_TCP_BY_PORT: {     	/* clear TCP by addr/ports */
	    /*
	     * Get local address and local port
	     */
	    laddr = GETOBJ(paddr, 1);
	    lport = GETOBJ(int, 1);

	    /*
	     * Get remote address and remote port
	     */
	    faddr = GETOBJ(paddr, 2);
	    fport = GETOBJ(int, 2);
	    if (confirm("")) {
		tcp_cleared = clear_tcp_by_port_addr(laddr, lport, faddr, 
						     fport);
		if (! tcp_cleared) {
		    printf("\n%%Clear TCP failed: %s:%d -> %s:%d not found",
			    address_string(laddr), lport, 
			    address_string(faddr), fport);
		} else {
		    printf("\n [OK]");
		}
	    }
	    break;
	}
	case CLEAR_TCP_BY_TCB: {	/* clear TCP by TCB address */
	    /*
	     * Get TCB address.
	     */
	    tcb = (tcbtype *)(GETOBJ(int, 1));
	    if (confirm("")) {
		tcp_cleared = clear_tcp_by_tcb(tcb);
		if (! tcp_cleared)
		    printf("\n%%Clear TCP failed: TCB 0x%x not found", tcb);
		else
		    printf("\n [OK]");
	    }
	    break;
	}
	case CLEAR_TCP_BY_LINE: {	/* clear TCP by line number */
	    /*
	     * Get tty line number.
	     */
	    line = GETOBJ(int, 1);
	    if (confirm("")) {
		tcp_cleared = clear_tcp_by_line(line);
		if (! tcp_cleared)
		    printf("\n%%Clear TCP failed: line %d doesn't exist or doesn't have TCP", line);
		else
		    printf("\n [OK]");
	    }
	    break;
	}
    }
}


/*
 * Parse chains for TCP show commands
 */
#define	ALTERNATE	NONE
#include "exec_show_tcp.h"
#include "exec_show_refuse.h"
LINK_POINT(tcp_show_commands, ALTERNATE);
#undef	ALTERNATE

/*
 * Parse chains for TCP config commands
 */
#define ALTERNATE       NONE
#include "cfg_ip_tcp.h"
LINK_POINT(tcp_cfg_commands, ALTERNATE);
#undef  ALTERNATE


/*
 * Parse chains for TCP test commands
 */
#define	ALTERNATE	NONE
#include "exec_test_tcp.h"
LINK_POINT(tcp_test_commands, ALTERNATE);
#undef	ALTERNATE

/*
 * Parse chain for TCP debug commands 
 */
#define	ALTERNATE	NONE
#include "exec_debug_telnet.h"
LINK_POINT(tcp_debug_commands, ALTERNATE);
#undef	ALTERNATE

#define	ALTERNATE	NONE
#include "exec_debug_tcp.h"
LINK_POINT(tcp_ip_debug_commands, ALTERNATE);
#undef	ALTERNATE

/*
 * Parse chains for TCP exec commands
 */
#define	ALTERNATE	NONE
#include "exec_telnet.h"
LINK_POINT(tcp_exec_commands, ALTERNATE);
#undef	ALTERNATE

/*
 * Parse chains for TCP clear commands
 */
#define ALTERNATE	NONE
#include "exec_clear_tcp.h"
LINK_POINT(tcp_clear_commands, ALTERNATE);
#undef  ALTERNATE

/*
 * Parse chains for TCP snmp-server host command
 */
LINK_EXIT(cfg_snmp_host_tty_exit, no_alt);
KEYWORD_OR(cfg_snmp_host_tty, cfg_snmp_host_tty_exit, NONE,
	   OBJ(int,1), (1 << TRAP_TTY),
	   "tty", "Allow TCP connection traps", PRIV_CONF);
LINK_POINT(cfg_snmp_host_tty_entry, cfg_snmp_host_tty);

static void tty_snmp_host_nvgen (ulong flags)
{
    nv_add(flags & (1 << TRAP_TTY), " tty");
}


/*
 * Parse chain registration array for TCP
 */
static parser_extension_request tcp_chain_init_table[] = {
    { PARSE_ADD_EXEC_CMD, &pname(tcp_exec_commands) },
    { PARSE_ADD_DEBUG_CMD, &pname(tcp_debug_commands) },
    { PARSE_ADD_SHOW_CMD, &pname(tcp_show_commands) },
    { PARSE_ADD_TEST_CMD, &pname(tcp_test_commands) },
    { PARSE_ADD_DEBUG_IP_CMD, &pname(tcp_ip_debug_commands) },
    { PARSE_ADD_CFG_TOP_IP_CMD, &pname(tcp_cfg_commands) },
    { PARSE_ADD_CLEAR_CMD, &pname(tcp_clear_commands) },
    { PARSE_ADD_CFG_SNMP_HOST_CMD, &pname(cfg_snmp_host_tty_entry) },
    { PARSE_ADD_CFG_SNMP_HOST_EXIT,
	  (dynamic_transition *) &pname(cfg_snmp_host_tty_exit) },
    { PARSE_LIST_END, NULL }
};

/*
 * tcp_parser_init - Initialize TCP parser support
 */
void tcp_parser_init (void)
{
    parser_add_command_list(tcp_chain_init_table, "tcp");
    parser_add_link_point(PARSE_ADD_EXEC_CONNECT_CMD,
			  "connect options",
			  &pname(exec_telnet_opts_extend_here));
    parser_add_link_point(PARSE_ADD_DEBUG_TCP_CMD,
 			  "debug ip tcp",
 			  &pname(debug_ip_tcp_extend));
    parser_add_link_exit(PARSE_ADD_TELNET_EXIT_CMD, "tcp telnet entry",
			 &pname(exec_telnet_entry));
    parser_add_link_exit(PARSE_ADD_RLOGIN_EOL_EXIT,
			 "rlogin eol exit",
			 &pname(exec_rlogin_return));
    parser_add_link_exit(PARSE_ADD_TELNET_PORT_EXIT,
			 "telnet port exit",
			 &pname(exec_telnet_port_return));
    parser_add_link_exit(PARSE_ADD_EXEC_CONNECT_EXIT,
			 "connect options exit",
			 &pname(exec_telnet_return_here));
    if (registry_populated(REG_SNMP))
	reg_add_snmp_host_nvgen(tty_snmp_host_nvgen, "tty_snmp_host_nvgen");
}
      

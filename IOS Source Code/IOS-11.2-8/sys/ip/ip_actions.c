/* $Id: ip_actions.c,v 3.8.12.4 1996/08/28 12:49:23 thille Exp $
 * $Source: /release/112/cvs/Xsys/ip/ip_actions.c,v $
 *------------------------------------------------------------------
 * IP specific parsing functions.
 *
 * April 1993,  Robert Widmer
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: ip_actions.c,v $
 * Revision 3.8.12.4  1996/08/28  12:49:23  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.8.12.3  1996/07/29  22:45:37  snyder
 * CSCdi64484:  declare arrays const in sub_ipcore
 *              moves 1344 out of data, 20 out of image
 * Branch: California_branch
 *
 * Revision 3.8.12.2  1996/06/29  00:11:10  snyder
 * CSCdi61694:  change some char * to char [] in ip
 * Branch: California_branch
 *     save 116 bytes
 *
 * Revision 3.8.12.1  1996/05/21  09:51:02  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.8  1996/02/20  18:02:08  widmer
 * CSCdi49363:  Drop function name from INVALID_NVGEN_CHECK
 *
 * Revision 3.7  1996/02/07  21:15:04  widmer
 * CSCdi48343:  Remove unused #ifdef STANDALONE from parser code
 *
 * Revision 3.6  1996/01/19  00:57:15  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.5  1995/12/17  18:29:25  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.4  1995/12/10  00:17:33  pst
 * CSCdi45332:  parser ipaddr_mask macros dont accept /32 and dont set
 * ADDR_IP
 *
 * Revision 3.3  1995/12/01  09:32:02  tli
 * CSCdi44465:  cosmetic bug with bootps port number
 * 69->67  What was I thinking?
 *
 * Revision 3.2  1995/11/17  09:33:13  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:55:07  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.6  1995/11/08  21:04:05  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.5  1995/07/13  21:39:00  ahh
 * CSCdi36962:  UDP parsing of tacacs-ds port name broken
 * Restore "tacacs-ds" as a keyword, but hide it and map it to TACACS_PORT
 * for backwards compatibility.
 *
 * Revision 2.4  1995/07/12  07:18:37  tli
 * CSCdi36426:  Policy routing will not disable on an interface
 * - Policy routed packets were creating fastswitching cache entries.
 *   That's not till _next_ release.
 * - Add counters to record packet and byte count on a per route map entry
 *   basis.
 * - Marketing identified a catastrophic deficiency.  Implemented "set ip
 *   tos" and "set ip precedence" to cover this deficiency.
 *
 * Revision 2.3  1995/07/08  22:14:55  tli
 * CSCdi36841:  TCP port name lpd not used in nvram
 *
 * Revision 2.2  1995/06/19  01:06:21  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.1  1995/06/07  20:56:47  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "interface_private.h"
#include "config.h"
#include "parser.h"
#include "logger.h"
#include "../parser/msg_parser.c"	/* Not a typo, see logger.h */
#include "../parser/parser_actions.h"
#include "ip_actions.h"
#include "packet.h"
#include "ip.h"
#include "parser_defs_ip.h"
#include "../parser/parser_defs_access.h"

#include "../iprouting/route.h"
#include "name.h"

/*
 * Forward declarations
 */
static boolean match_ip_name(char *, int *, ulong *, int, uint);


char *ip_addr_short_help (parseinfo *csb, uint flag, idbtype *idb)
{
    if (flag & ADDR_NAME_LOOKUP) {
	return("Hostname or A.B.C.D");
    } else {
	return("A.B.C.D");
    }
}

boolean match_ip_addr (parseinfo *csb, char *buf, int *pi, void *addr_pointer,
                       int in_help, uint flag, idbtype *idb)
{
    addrtype *addrp = (addrtype *)addr_pointer;
    ushort num[4];
    boolean matched;

    if (match_decimal(buf, 0, 255, pi, &num[0]) &&
        match_char(buf, pi, '.') &&
        match_decimal(buf, 0, 255, pi, &num[1]) &&
        match_char(buf, pi, '.') &&
        match_decimal(buf, 0, 255, pi, &num[2]) &&
        match_char(buf, pi, '.') &&
        match_decimal(buf, 0, 255, pi, &num[3])) {

        addrp->ip_addr = ((((ulong) num[0]) << 24) |
                          (((ulong) num[1]) << 16) |
                          (((ulong) num[2]) << 8)  |
                          ((ulong) num[3]));
        matched = TRUE;
    } else {
        if (flag & ADDR_NAME_LOOKUP) {
            matched = match_ip_name(buf,pi,&(addrp->ip_addr),in_help, flag);
        } else {
            matched = FALSE;
        }
    }

    if (matched) {
        addrp->type = ADDR_IP;
        addrp->length = ADDRLEN_IP;
        return(TRUE);
    }
    return(FALSE);
}

static boolean match_ip_name (char *buf, int *pi, ulong *addr,
                              int in_help, uint flag)
{
    char *name;
    int i;
    nametype *entry;
    int dummy;

    name = malloc(PARSEBUF);
    if (name == NULL) {
        return(FALSE);
    }

    if (get_name(&buf[*pi], name, &i, PARSEBUF-1)) {
        if (in_help) {
            *pi += i;
            free(name);
            return(TRUE);
        }
        if ((entry = (nametype *)name_lookup(name, &dummy, NAM_IP))) {
            *pi += i;
            *addr = entry->v.ip.adr[0].addr.ip_address;
            free(name);
            return(TRUE);
        }
    }
    free(name);
    return(FALSE);
}

void ipaddrmask_action (parseinfo *csb, transition *mine,
                        ipaddrmask_struct * const arg)
{
    addrtype *addrp = CSBVAR(csb, arg->addr, addrtype);
    addrtype *maskp = CSBVAR(csb, arg->mask, addrtype);
    int i;
    uint mask_number = 0;
    static const char short_help[] = "A.B.C.D  {/nn || A.B.C.D}";
    static const char prefix_help[] = "IP prefix and prefix mask";

    push_node(csb, mine->alternate);
    INVALID_NVGEN_CHECK;

    if (help_check(csb)) {
	save_help_long(csb, short_help, arg->help);
	return;
    }

    i = 0;
    if (match_ip_addr(csb, &csb->line[csb->line_index], &i, addrp,
		      csb->in_help, 0, csb->interface)) {
	if (match_char(&csb->line[csb->line_index], &i, '/')) {
	    if (match_number(csb, 0, 32, &i, &mask_number,
			     NUMBER_DEC | NUMBER_WS_OK | NUMBER_HELP_CHECK,
			     "Length of prefix mask")) {
		uint n = 1<<31;

		while (mask_number--) {
		    maskp->ip_addr |= n;
		    n = n>>1;
		}

		maskp->type = ADDR_IP;
		maskp->length = ADDRLEN_IP;

		csb->line_index += i;
		push_node(csb, mine->accept);
		csb->line_index -= i;
	    }
	} else {
	    if (match_whitespace2(&csb->line[csb->line_index],
				  &i, csb->in_help)) {
		csb->line_index += i;
		if (help_check(csb)) {
		    save_help_long(csb, short_help, prefix_help);
		    csb->line_index -= i;
		    return;
		}
		csb->line_index -= i;

		if (match_ip_addr(csb, &csb->line[csb->line_index], &i, maskp,
				  csb->in_help, 0, csb->interface)) {
		    if (match_whitespace2(&csb->line[csb->line_index],
					  &i, csb->in_help)) {
			csb->line_index += i;
			push_node(csb, mine->accept);
			csb->line_index -= i;
		    } else {
			help_or_error(csb, i, prefix_help,
				      NO_COMMAND_COMPLETION);
		    }
		} else {
		    help_or_error(csb, i, prefix_help,
				  NO_COMMAND_COMPLETION);
		}
	    } else {
		/*
		 * Two short helps in one string, don't try
		 * this at home kids
		 */
		help_or_error(csb, i, "/\tIP prefix mask",
			      NO_COMMAND_COMPLETION);
	    }
	}
    } else {
	help_or_error(csb, i, short_help, NO_COMMAND_COMPLETION);
    }
}

void ipmask_action (parseinfo *csb, transition *mine,
		    ipmask_struct * const arg)
{
    addrtype *maskp = CSBVAR(csb, arg->mask, addrtype);
    int i;
    uint mask_number = 0;
    static const char short_help[] = "/nn or A.B.C.D";
    static const char prefix_help[] = "IP prefix mask";

    push_node(csb, mine->alternate);
    INVALID_NVGEN_CHECK;

    if (help_check(csb)) {
	save_help_long(csb, short_help, arg->help);
	return;
    }

    i = 0;
    if (match_char(&csb->line[csb->line_index], &i, '/')) {
	if (match_number(csb, 0, 32, &i, &mask_number,
			 NUMBER_DEC | NUMBER_WS_OK | NUMBER_HELP_CHECK,
			 "Length of prefix mask")) {
	    uint n = 1<<31;

	    while (mask_number--) {
		maskp->ip_addr |= n;
		n = n>>1;
	    }

	    maskp->type = ADDR_IP;
	    maskp->length = ADDRLEN_IP;

	    csb->line_index += i;
	    push_node(csb, mine->accept);
	    csb->line_index -= i;
	}
    } else {
	if (match_whitespace2(&csb->line[csb->line_index], &i, csb->in_help)) {
	    csb->line_index += i;
	    if (help_check(csb)) {
		save_help_long(csb, short_help, prefix_help);
		csb->line_index -= i;
		return;
	    }
	    csb->line_index -= i;

	    if (match_ip_addr(csb, &csb->line[csb->line_index], &i, maskp,
			      csb->in_help, 0, csb->interface)) {
		if (match_whitespace2(&csb->line[csb->line_index],
				      &i, csb->in_help)) {
		    csb->line_index += i;
		    push_node(csb, mine->accept);
		    csb->line_index -= i;
		} else {
		    help_or_error(csb, i, prefix_help, NO_COMMAND_COMPLETION);
		}
	    } else {
		help_or_error(csb, i, prefix_help, NO_COMMAND_COMPLETION);
	    }
	} else {
	    /*
	     * Two short helps in one string, don't try
	     * this at home kids
	     */
	    help_or_error(csb, i, "/\tIP prefix mask", NO_COMMAND_COMPLETION);
	}
    }
}

/*
 * transport_port_action
 * Action routine to parse an arbitrary port number.
 */

static void transport_port_action (parseinfo *csb, transition *mine,
			    portparse_struct * const arg, portinfo *porttable)
{
    int *port = CSBVAR(csb, arg->offset, int);
    int i;
    portinfo *pi;

    push_node(csb, mine->alternate);

    INVALID_NVGEN_CHECK;

    i = 0;
    if (match_number(csb, 0, MAXUSHORT, &i, port,
		     (NUMBER_HEX | NUMBER_DEC | NUMBER_OCT | HEX_ZEROX_OK |
		      OCT_ZERO_OK | NUMBER_WS_OK | NUMBER_HELP_CHECK),
		     "Port number")) {
	csb->line_index += i;
	push_node(csb, mine->accept);
	csb->line_index -= i;
    }

    for (pi = porttable; pi->str; pi++) {
	i = 0;
	if (match_partial_keyword(csb, pi->str, &i, pi->help, pi->minmatch,
				  (KEYWORD_WS_OK | KEYWORD_HELP_CHECK))) { 
	    *port = pi->port;
	    csb->line_index += i;
	    push_node(csb, mine->accept);
	    csb->line_index -= i;
	}
    }
}

const id_string tcp_ports[] = {
    {BGP_PORT, BGP_PORT_NAME},
    {CHARGEN_PORT, CHARGEN_PORT_NAME},
    {CMD_PORT, CMD_PORT_NAME},
    {DAYTIME_PORT, DAYTIME_PORT_NAME},
    {DISCARD_PORT, DISCARD_PORT_NAME},
    {DNS_PORT, DNS_PORT_NAME},
    {ECHO_PORT, ECHO_PORT_NAME},
    {EXEC_PORT, EXEC_PORT_NAME},
    {FINGER_PORT, FINGER_PORT_NAME},
    {FTP_PORT, FTP_PORT_NAME},
    {FTPD_PORT, FTPD_PORT_NAME},
    {GOPHER_PORT, GOPHER_PORT_NAME},
    {IDENT_PORT, IDENT_PORT_NAME},
    {IRC_PORT, IRC_PORT_NAME},
    {HOSTNAME_PORT, HOSTNAME_PORT_NAME},
    {KLOGIN_PORT, KLOGIN_PORT_NAME},
    {KSHELL_PORT, KSHELL_PORT_NAME},
    {LOGIN_PORT, LOGIN_PORT_NAME},
    {NICNAME_PORT, NICNAME_PORT_NAME},
    {NNTP_PORT, NNTP_PORT_NAME},
    {POP2_PORT, POP2_PORT_NAME},
    {POP3_PORT, POP3_PORT_NAME},
    {PRINTER_PORT, PRINTER_PORT_NAME},
    {SMTP_PORT, SMTP_PORT_NAME},
    {SUNRPC_PORT, SUNRPC_PORT_NAME},
    {SYSLOG_PORT, SYSLOG_PORT_NAME},
    {TACACS_PORT, TACACS_PORT_NAME},
    {TALK_PORT, TALK_PORT_NAME},
    {TELNET_PORT, TELNET_PORT_NAME},
    {UUCP_PORT, UUCP_PORT_NAME},
    {WWW_PORT, WWW_PORT_NAME},
    {0, NULL}
};

const id_string udp_ports[] = {
    {BIFF_PORT, BIFF_PORT_NAME},
    {BOOTPC_PORT, BOOTPC_PORT_NAME},
    {BOOTPS_PORT, BOOTPS_PORT_NAME},
    {DISCARD_PORT, DISCARD_PORT_NAME},
    {DNS_PORT, DNS_PORT_NAME},
    {DNSIX_PORT, DNSIX_PORT_NAME},
    {ECHO_PORT, ECHO_PORT_NAME},
    {IEN116_PORT, IEN116_PORT_NAME},
    {MOBILEIP_PORT, MOBILEIP_PORT_NAME},
    {NBNS_PORT, NBNS_PORT_NAME},
    {NBDS_PORT, NBDS_PORT_NAME},
    {NTP_PORT, NTP_PORT_NAME},
    {RIP_PORT, RIP_PORT_NAME},
    {SNMP_PORT, SNMP_PORT_NAME},
    {SNMP_TRAP_PORT, SNMP_TRAP_PORT_NAME},
    {SUNRPC_PORT, SUNRPC_PORT_NAME},
    {SYSLOG_PORT, SYSLOG_PORT_NAME},
    {TACACS_PORT, TACACS_PORT_NAME},
    {TALK_PORT, TALK_PORT_NAME},
    {TFTP_PORT, TFTP_PORT_NAME},
    {TIME_PORT, TIME_PORT_NAME},
    {WHO_PORT, WHO_PORT_NAME},
    {XDMCP_PORT, XDMCP_PORT_NAME},
    {0, NULL}
};

static portinfo tcpports [] =
{
    {BGP_PORT, BGP_PORT_NAME, "Border Gateway Protocol (179)", 0},
    {CHARGEN_PORT, CHARGEN_PORT_NAME, "Character generator (19)", 0},
    {CMD_PORT, CMD_PORT_NAME, "Remote commands (rcmd, 514)", 0},
    {DAYTIME_PORT, DAYTIME_PORT_NAME, "Daytime (13)", 0},
    {DISCARD_PORT, DISCARD_PORT_NAME, "Discard (9)", 0},
    {DNS_PORT, DNS_PORT_NAME, "Domain Name Service (53)", 0},
    {ECHO_PORT, ECHO_PORT_NAME, "Echo (7)", 0},
    {EXEC_PORT, EXEC_PORT_NAME, "Exec (rsh, 512)", 0},
    {FINGER_PORT, FINGER_PORT_NAME, "Finger (79)", 0},
    {FTP_PORT, FTP_PORT_NAME, "File Transfer Protocol (21)", 0},
    {FTPD_PORT, FTPD_PORT_NAME,
	 "FTP data connections (used infrequently, 20)", 4},
    {GOPHER_PORT, GOPHER_PORT_NAME, "Gopher (70)", 0},
    {IDENT_PORT, IDENT_PORT_NAME, "Ident Protocol (113)", 0},
    {IRC_PORT, IRC_PORT_NAME, "Internet Relay Chat (194)", 0},
    {HOSTNAME_PORT, HOSTNAME_PORT_NAME, "NIC hostname server (101)", 0},
    {KLOGIN_PORT, KLOGIN_PORT_NAME, "Kerberos login (543)", 0},
    {KSHELL_PORT, KSHELL_PORT_NAME, "Kerberos shell (544)", 0},
    {LOGIN_PORT, LOGIN_PORT_NAME, "Login (rlogin, 513)", 0},
    {NICNAME_PORT, NICNAME_PORT_NAME, "Nicname (43)", 0},
    {NNTP_PORT, NNTP_PORT_NAME, "Network News Transport Protocol (119)", 0},
    {POP2_PORT, POP2_PORT_NAME, "Post Office Protocol v2 (109)", 0},
    {POP3_PORT, POP3_PORT_NAME, "Post Office Protocol v3 (110)", 0},
    {PRINTER_PORT, PRINTER_PORT_NAME, "Printer service (515)", 0},
    {SMTP_PORT, SMTP_PORT_NAME, "Simple Mail Transport Protocol (25)", 0},
    {SUNRPC_PORT, SUNRPC_PORT_NAME, "Sun Remote Procedure Call (111)", 0},
    {SYSLOG_PORT, SYSLOG_PORT_NAME, "Syslog (514)", 0},
    {TACACS_PORT, TACACS_PORT_NAME, "TAC Access Control System (49)", 0},
    {TALK_PORT, TALK_PORT_NAME, "Talk (517)", 0},
    {TIME_PORT, TIME_PORT_NAME, "Time (37)", 0},
    {TELNET_PORT, TELNET_PORT_NAME, "Telnet (23)", 0},
    {UUCP_PORT, UUCP_PORT_NAME, "Unix-to-Unix Copy Program (540)", 0},
    {WWW_PORT, WWW_PORT_NAME, "World Wide Web (HTTP, 80)", 0},
    {0, NULL, NULL, 0}
};

const id_string precedence_values[] = {
    {PRECEDENCE_NETWORK, PRECEDENCE_NETWORK_STR},
    {PRECEDENCE_INTERNET, PRECEDENCE_INTERNET_STR},
    {PRECEDENCE_CRITICAL, PRECEDENCE_CRITICAL_STR},
    {PRECEDENCE_FLASHOVER, PRECEDENCE_FLASHOVER_STR},
    {PRECEDENCE_FLASH, PRECEDENCE_FLASH_STR},
    {PRECEDENCE_IMMEDIATE, PRECEDENCE_IMMEDIATE_STR},
    {PRECEDENCE_PRIORITY, PRECEDENCE_PRIORITY_STR},
    {PRECEDENCE_ROUTINE, PRECEDENCE_ROUTINE_STR},
    {0, NULL}
};

const id_string tos_values[] = {
    {IP_TOS_NORMAL, TOS_NORMAL_NAME},
    {IP_TOS_LOW_COST, TOS_COST_NAME},
    {IP_TOS_HIGH_RELY, TOS_RELIABLE_NAME},
    {IP_TOS_HIGH_TPUT, TOS_THROUGH_NAME},
    {IP_TOS_LOW_DELAY, TOS_DELAY_NAME},
    {0, NULL}
};

/*
 * tcpport_action
 * Action routine to parse an arbitrary TCP port number.
 */

void tcpport_action (parseinfo *csb, transition *mine,
		     portparse_struct * const arg)
{
    transport_port_action(csb, mine, arg, tcpports);
}

static portinfo udpports [] = {
    {BIFF_PORT, BIFF_PORT_NAME, "Biff (mail notification, comsat, 512)", 0},
    {BOOTPC_PORT, BOOTPC_PORT_NAME,
	 "Bootstrap Protocol (BOOTP) client (68)", 0},
    {BOOTPS_PORT, BOOTPS_PORT_NAME,
	 "Bootstrap Protocol (BOOTP) server (67)", 0}, 
    {DISCARD_PORT, DISCARD_PORT_NAME, "Discard (9)", 0},
    {DNS_PORT, DNS_PORT_NAME, "Domain Name Service (DNS, 53)", 0},
    {DNSIX_PORT, DNSIX_PORT_NAME, "DNSIX security protocol auditing (195)",
	 4}, 
    {ECHO_PORT, ECHO_PORT_NAME, "Echo (7)", 0},
    {IEN116_PORT, IEN116_PORT_NAME, "IEN116 name service (obsolete, 42)", 0},
    {MOBILEIP_PORT, MOBILEIP_PORT_NAME, "Mobile IP registration (434)", 0},
    {NBNS_PORT, NBNS_PORT_NAME, "NetBios name service (137)", 0},
    {NBDS_PORT, NBDS_PORT_NAME, "NetBios datagram service (138)", 0},
    {NTP_PORT, NTP_PORT_NAME, "Network Time Protocol (123)", 0},
    {RIP_PORT, RIP_PORT_NAME,
	 "Routing Information Protocol (router, in.routed, 520)", 0},
    {SNMP_PORT, SNMP_PORT_NAME,
	 "Simple Network Management Protocol (161)", 0},
    {SNMP_TRAP_PORT, SNMP_TRAP_PORT_NAME, "SNMP Traps (162)", 5},
    {SUNRPC_PORT, SUNRPC_PORT_NAME, "Sun Remote Procedure Call (111)", 0},
    {SYSLOG_PORT, SYSLOG_PORT_NAME, "System Logger (514)", 0},
    {TACACS_PORT, TACACS_PORT_NAME, "TAC Access Control System (49)", 0},
    {TALK_PORT, TALK_PORT_NAME, "Talk (517)", 0},
    {TFTP_PORT, TFTP_PORT_NAME, "Trivial File Transfer Protocol (69)", 0},
    {TIME_PORT, TIME_PORT_NAME, "Time (37)", 0},
    {WHO_PORT, WHO_PORT_NAME, "Who service (rwho, 513)", 0},
    {XDMCP_PORT, XDMCP_PORT_NAME,
	 "X Display Manager Control Protocol (177)", 0},
    {0, NULL, NULL, 0} 
};

/*
 * udpport_action
 * Action routine to parse an arbitrary UDP port number.
 */

void udpport_action (parseinfo *csb, transition *mine,
		     portparse_struct * const arg)
{
    transport_port_action(csb, mine, arg, udpports);
}

/*
 * nv_add_tcp_port
 * Add a TCP port to nvram
 */

void nv_add_tcp_port (uint port)
{
    nv_add_id_string(tcp_ports, port);
}

/*
 * nv_add_udp_port
 * Add a UDP port to nvram
 */

void nv_add_udp_port (uint port)
{
    nv_add_id_string(udp_ports, port);
}

/*
 * printf_tcp_port
 * Print a TCP port
 */

void printf_tcp_port (uint port)
{
    printf_id_string(tcp_ports, port);
}

/*
 * printf_udp_port
 * Print a UDP port 
 */

void printf_udp_port (uint port)
{
    printf_id_string(udp_ports, port);
}


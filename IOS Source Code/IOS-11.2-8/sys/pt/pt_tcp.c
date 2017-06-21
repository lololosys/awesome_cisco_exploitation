/* $Id: pt_tcp.c,v 3.7.20.4 1996/07/06 05:56:03 thille Exp $
 * $Source: /release/112/cvs/Xsys/pt/pt_tcp.c,v $
 *------------------------------------------------------------------
 * TCP specific Protocol Translator code
 *
 * November 1993, Scott Mackie, Eman Schaffer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: pt_tcp.c,v $
 * Revision 3.7.20.4  1996/07/06  05:56:03  thille
 * CSCdi62136:  seq: iphost and req: iphost duplicated and waste memory
 * Branch: California_branch
 * Put copies of the strings in common_strings.c/h and reference them
 * instead of having multiple copies of the literals.  Save 660 bytes.
 *
 * Revision 3.7.20.3  1996/06/19  16:24:01  swenson
 * CSCdi60436:  translate: tcp source-interface added when ipx used
 * Branch: California_branch
 * Only check for source-interface if tcp is the outgoing protocol
 *
 * Revision 3.7.20.2  1996/05/29  22:32:02  swenson
 * CSCdi58675:  ip alias error message too cryptic for protocol translation
 * Branch: California_branch
 *
 * Revision 3.7.20.1  1996/04/15  21:22:14  bcole
 * CSCdi54649:  IP Host Modularity, IP Automatic Stub Routing feature
 * commit
 * Branch: California_branch
 *
 * Revision 3.7  1996/02/01  06:08:27  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.6  1996/01/24  20:40:48  jenny
 * CSCdi47046:  TCP/IP need code clean up (part 3)
 * . removed ip/tuba.c, ip/tuba_default.c and ip/tuba.h
 * . removed all includes for ip/tuba.h.
 * . removed more TUBA related code.
 *
 * Revision 3.5  1996/01/23  23:45:28  vandys
 * CSCdi45547:  Desire rotary behavior on PT TCP destinations
 * Add rotor option to randomize destination for TCP host lists
 *
 * Revision 3.4  1995/11/22  05:56:20  eschaffe
 * CSCdi44200:  !23 ports in the translate command should override ip host
 * port
 *
 * Revision 3.3  1995/11/20  23:22:04  ahh
 * CSCdi41919:  PT: cannot specify source interface for TCP connections
 * Add source-interface option to outgoing TCP translation and telnet.
 * Also convert some parser defines to enums.
 *
 * Revision 3.2  1995/11/17  19:00:12  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:01:56  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/10/24  05:07:27  eschaffe
 * CSCdi41300:  translate commands keyword options should nv-genn in lower
 * case
 *
 * Revision 2.2  1995/10/13  21:48:09  eschaffe
 * CSCdi18102:  Require one translate statment to multiple hosts
 *         Add "host-name" Telnet/TCP outgoing option.
 *
 * Revision 2.1  1995/06/07  22:35:29  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "packet.h"
#include "registry.h"
#include "../pt/pt_registry.h"
#include "subsys.h"
#include "address.h"
#include "name.h"
#include "ttysrv.h"
#include "connect.h"
#include "../ip/ip.h"
#include "../tcp/tcp.h"
#include "config.h"
#include "../ui/debug.h"
#include "parser.h"
#include "parser_defs_pt.h"
#include "../tcp/telnet.h"
#include "translate.h"
#include "../pt/pt_debug.h"
#include "access.h"
#include "interface_private.h"
#include "pt_tcp.h"
#include "../ip/ip_registry.h"


/*
 * TCP Protocol Translator subsystem header
 */

#define PT_TCP_MAJVERSION 1
#define PT_TCP_MINVERSION 0
#define PT_TCP_EDITVERSION  1

SUBSYS_HEADER(pt_tcp,
	      PT_TCP_MAJVERSION, PT_TCP_MINVERSION, PT_TCP_EDITVERSION,
	      pt_tcp_init, SUBSYS_CLASS_PROTOCOL,
	      subsys_seq_iphost,
	      subsys_req_iphost);

/*
 * pt_tcp_init
 *
 * Subsystem initialization point
 */

void pt_tcp_init (subsystype *subsys)
{
    /*
     * Add parser commands for this subsystem
     */
    pt_tcp_parser_init();

    /*
     * Add service points for this subsystem
     */
    reg_add_pt_translate_command(TRANS_PROT_TCP,
				 pt_tcp_command,
				 "pt_tcp_command");

    reg_add_pt_add_alias(TRANS_TCP,
			 pt_tcp_add_alias,
			 "pt_tcp_add_alias");

    reg_add_pt_compare_entry(TRANS_TCP,
			     pt_tcp_compare_entry,
			     "pt_tcp_compare_entry");

    reg_add_pt_delete_alias(TRANS_TCP,
			    pt_tcp_delete_alias,
			    "pt_tcp_delete_alias");

    reg_add_pt_show_translate(TRANS_TCP,
			      pt_show_tcp_transinfo,
			      "pt_show_tcp_transinfo");

    reg_add_pt_nv_translate_entry(TRANS_TCP,
				  pt_nv_tcp_transinfo,
				  "pt_nv_tcp_transinfo");

    reg_add_pt_translate_string(TRANS_TCP, pt_tcp_trans_string,
				"pt_tcp_trans_string");
}

/*
 * pt_tcp_commmand
 *
 * Registry service point for protocol translator parser chain
 */

int pt_tcp_command (parseinfo *csb,
			    translatetype *trans,
			    int dir)
{
    addrtype	*addr;
    trans_tcpinfo *tcpinfo;
    int		binary;
    int		port;
    int		stream;
    int		printer;

    tcpinfo = (dir == IN) ? &trans->in.tcp : &trans->out.tcp;

    if (GETOBJ(int,6) == TRANS_PROT_OLD) {
	tcpinfo->tcpflags |= TRANSF_TCP_BINARY;  /* old style assumes binary */
    }

    if (dir == IN) {
	addr = GETOBJ(paddr,1);
	binary = GETOBJ(int,2);
	port = GETOBJ(int,3);
	stream = GETOBJ(int,4);
	printer = GETOBJ(int,5);
    } else {
	addr = GETOBJ(paddr,2);
	binary = GETOBJ(int,7);
	port = GETOBJ(int,8);
	stream = GETOBJ(int,9);
	printer = 0;
	if (GETOBJ(int,10)) {
	    tcpinfo->tcpflags |= TRANSF_TCP_IACHACK;
	}
	if (GETOBJ(int,11)) {
	    tcpinfo->tcpflags |= TRANSF_TCP_HOST_NAME;
	}
	if (GETOBJ(int,12)) {
	    tcpinfo->tcpflags |= TRANSF_TCP_ROTOR;
	}
	tcpinfo->source_idb = GETOBJ(idb,1);
    }
    if (tcpinfo->tcpflags & TRANSF_TCP_HOST_NAME) {
	tcpinfo->tcpnptr = name_cache_lookup(NULL, addr, NAM_IP);
    } else {
	tcpinfo->ipaddress = addr->ip_addr;
    }

    tcpinfo->tcpport = port ? port: TELNET_PORT; 

    if (stream) {
	tcpinfo->tcpflags |= TRANSF_TCP_STREAM;
    }
    if (binary) {
	tcpinfo->tcpflags |= TRANSF_TCP_BINARY;
    }
    if (printer) {
	tcpinfo->tcpflags |= TRANSF_TCP_PRINTER;

	if (dir == IN) {
	    /*
	     * The TCP local option "printer" implies the global option "quiet"
	     */
	    trans->flags |= TRANSF_GLOB_QUIET;
	}
    }
    return(TRANS_TCP);
}

/*
 * pt_show_tcp_transinfo
 * 
 * Show TCP related translate information
 */
void pt_show_tcp_transinfo (translatetype *trans, int dir)
{
    trans_tcpinfo *tcp = ((dir == IN) ? &trans->in.tcp : &trans->out.tcp);

    if (tcp->tcpnptr)
	printf("TCP %s ", tcp->tcpnptr->name);
    else
	printf("TCP %i ", tcp->ipaddress);
    printf("Port %d%s%s%s%s%s%s", tcp->tcpport,
	   tcp->tcpflags & TRANSF_TCP_STREAM ? " Stream" : "",
	   tcp->tcpflags & TRANSF_TCP_BINARY ? " Binary" : "",
	   tcp->tcpflags & TRANSF_TCP_PRINTER ? " Printer" : "",
	   tcp->tcpflags & TRANSF_TCP_IACHACK ? " Multibyte-IAC" : "",
           tcp->tcpflags & TRANSF_TCP_HOST_NAME ? " Host-name" : "",
           tcp->tcpflags & TRANSF_TCP_ROTOR ? " Rotor" : ""
	   );
    if (tcp->source_idb)
	printf(" Source-interface %s", tcp->source_idb->namestring);
}
/*
 * pt_nv_tcp_transinfo
 * nv_add TCP related translate information
 */
void pt_nv_tcp_transinfo (translatetype *trans, int dir)
{
    trans_tcpinfo *tcp = ((dir == IN) ? &trans->in.tcp : &trans->out.tcp);

    if (tcp->tcpnptr)
	nv_add(TRUE, " tcp %s", tcp->tcpnptr->name);
    else
	nv_add(TRUE, " tcp %i", tcp->ipaddress);
    nv_add(tcp->tcpport != TELNET_PORT, " port %d", tcp->tcpport);
    nv_add(tcp->tcpflags & TRANSF_TCP_STREAM, " stream");
    nv_add(tcp->tcpflags & TRANSF_TCP_BINARY, " binary");
    nv_add(tcp->tcpflags & TRANSF_TCP_PRINTER, " printer");
    nv_add(tcp->tcpflags & TRANSF_TCP_IACHACK, " multibyte-iac");
    nv_add(tcp->tcpflags & TRANSF_TCP_HOST_NAME, " host-name");
    nv_add(tcp->tcpflags & TRANSF_TCP_ROTOR, " rotor");
    if (tcp->source_idb)
	nv_add(TRUE, " source-interface %s", tcp->source_idb->namestring);
}

/*
 * pt_tcp_add_alias
 *
 */ 

boolean pt_tcp_add_alias (translatetype *trans)
{
    if (!reg_invoke_ipalias_insert(trans->in.tcp.ipaddress, TCPPAD_ALIASPORT,
				   trans)) {
	/*
	 * The addition failed. Whine about it to the user
	 */
	printf("\n%%Translate: Can't use %i - in use or "
	       "not on a connected subnet", trans->in.tcp.ipaddress);
	return(FALSE);
    }

    /*
     * The addition succeeded
     */
    return(TRUE);
}

/*
 * pt_tcp_compare_entry
 *
 */ 

boolean pt_tcp_compare_entry (translatetype *trans1,
				      translatetype *trans2)
{
    /*
     * Check to see if this translation entry matches
     */
    if ((trans1->in.tcp.ipaddress != trans2->in.tcp.ipaddress) ||
	(trans1->in.tcp.tcpport != trans2->in.tcp.tcpport))
	return(FALSE);

    return(TRUE);
}
/*
 * pt_tcp_delete_alias
 *
 */

boolean pt_tcp_delete_alias (translatetype *trans)
{
    translatetype *trans_look;
    /*
     * First check for other translations that might be using this alias.
     * this will happen if there are more than one translations from the
     * same address, using different TCP ports.
     */
    for (trans_look = (translatetype *)translateQ->qhead;
         trans_look; trans_look = trans_look->next)
	if ((trans != trans_look) &&
	    (INPROT(trans_look->type) == TRANS_TCP) &&
	    (trans->in.tcp.ipaddress == trans_look->in.tcp.ipaddress)) {
	    /*
	     * update the alias table with (perhaps) new data.  This
	     * doesn't have to point to any particular translate command,
	     * as long as it isn't the one we just deleted!
	     */
	    reg_invoke_ipalias_insert(trans->in.tcp.ipaddress,
				      TCPPAD_ALIASPORT, trans_look);
	    return(TRUE);
	}
    reg_invoke_ipalias_delete(trans->in.tcp.ipaddress);
    return(TRUE);
}

/*
 * pt_tcp_trans_string
 * Take an int and return a string of the protocol name
 */
char *pt_tcp_trans_string (int prot)
{
    return("tcp");
}

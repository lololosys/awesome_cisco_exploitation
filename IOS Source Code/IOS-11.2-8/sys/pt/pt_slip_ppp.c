/* $Id: pt_slip_ppp.c,v 3.9.10.2 1996/04/02 17:19:30 swenson Exp $
 * $Source: /release/112/cvs/Xsys/pt/pt_slip_ppp.c,v $
 *------------------------------------------------------------------
 * pt_slip_ppp.c SLIP/PPP specific Protocol Translator code
 *
 * May 1994, Eman Schaffer
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: pt_slip_ppp.c,v $
 * Revision 3.9.10.2  1996/04/02  17:19:30  swenson
 * CSCdi52228:  translate command restart-time option not written to nvram
 * Branch: California_branch
 *
 * Revision 3.9.10.1  1996/03/18  21:45:23  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.5.2.3  1996/03/16  07:19:35  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.5.2.2  1996/03/07  10:41:20  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.5.2.1  1996/02/20  17:00:43  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.9  1996/02/22  05:20:26  fox
 * CSCdi35908:  Should allow CHAP and PAP clients to same dialin pool
 * Add support for multiple acceptable inbound PPP authentication methods
 *
 * Revision 3.8  1996/02/13  08:15:40  dstine
 * CSCdi48797:  Extraneous definitions in interface_private.h
 *
 * Revision 3.7  1996/02/01  06:08:22  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.6  1996/01/29  07:29:33  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.5  1996/01/22  06:58:51  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.4  1996/01/10  03:04:03  billw
 * CSCdi46650:  translate ... ppp authent ... nvgen broken in aaa newmodel
 * Save the method list name if new_model is on.
 *
 * Revision 3.3  1995/11/18  17:42:37  fox
 * CSCdi43045:  ppp and slip exec-level commands are broken
 * When checking for a duplicate peer address, don't check our own
 * interface.
 *
 * Revision 3.2  1995/11/17  19:00:04  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:01:45  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.5  1995/10/24  05:07:26  eschaffe
 * CSCdi41300:  translate commands keyword options should nv-genn in lower
 * case
 *
 * Revision 2.4  1995/09/18  19:38:04  fox
 * CSCdi40523:  Add IP Address Pooling support for ISDN and other sync
 * interfaces
 * Modify IP address pooling to support all Point-to-Point interfaces
 * Remove Odd/Even Address feature
 * Add 'ppp negotiate' command
 *
 * Revision 2.3  1995/08/10  19:54:56  eschaffe
 * CSCdi38356:  Add missing space between use-tacacs and keepalive/mtu
 * options
 *
 * Revision 2.2  1995/08/10  17:05:03  eschaffe
 * CSCdi37604:  Spurious memory access messages on 4500 using protocol
 * translation
 *
 * Revision 2.1  1995/06/07  22:35:20  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#include "master.h"
#include <ciscolib.h>
#include "packet.h"
#include "interface_private.h"
#include "registry.h"
#include "sys_registry.h"
#include "pt_registry.h"
#include "subsys.h"
#include "address.h"
#include "name.h"
#include "ttysrv.h"
#include "connect.h"
#include "config.h"
#include "../if/network.h"
#include "../ui/debug.h"
#include "parser.h"
#include "parser_defs_pt.h"
#include "translate.h"
#include "pt_debug.h"
#include "../ip/ip.h"
#include "../tcp/tcp.h"
#include "access.h"
#include "aaa.h"
#include "pt_slip_ppp.h"
#include "../ts/if_slip.h"
#include "vtyasync.h"
#include "../if/ether.h"
#include "../wan/ppp.h"


/*
 * SLIP Protocol Translator subsystem header
 */

#define PT_SLIP_PPP_MAJVERSION 1
#define PT_SLIP_PPP_MINVERSION 0
#define PT_SLIP_PPP_EDITVERSION  1

SUBSYS_HEADER(pt_slip_ppp, 
	      PT_SLIP_PPP_MAJVERSION, PT_SLIP_PPP_MINVERSION, 
	      PT_SLIP_PPP_EDITVERSION, 
	      pt_slip_ppp_init, SUBSYS_CLASS_PROTOCOL,
	      "seq: ppp, iphost",
	      "req: ppp, iphost");

/*
 * pt_slip_ppp_init
 *
 * Subsystem initialization point
 */

void pt_slip_ppp_init (subsystype *subsys)
{
    /*
     * Add parser commands for this subsystem
     */
    pt_slip_ppp_parser_init();
    pt_vty_async_init();

    /*
     * Add service points for this subsystem
     */
    reg_add_pt_translate_command(TRANS_PROT_SLIP_PPP,
				 pt_slip_ppp_command,
				 "pt_slip_ppp_command");

    reg_add_pt_add_alias(TRANS_SLIP_PPP,
			 pt_slip_ppp_add_alias,
			 "pt_slip_ppp_add_alias");

    reg_add_pt_show_translate(TRANS_SLIP_PPP,
			      pt_show_slip_ppp_transinfo,
			      "pt_show_slip_ppp_transinfo");

    reg_add_pt_nv_translate_entry(TRANS_SLIP_PPP,
				  pt_nv_slip_ppp_transinfo,
				  "pt_nv_slip_ppp_transinfo");

    reg_add_pt_translate_string(TRANS_SLIP_PPP, pt_async_trans_string,
				"pt_async_trans_string");
}

/*
 * pt_slip_ppp_commmand
 *
 * Registry service point for protocol translator parser chain
 */

int pt_slip_ppp_command (parseinfo *csb,
			 translatetype *trans,
			 int dir)
{
    trans_slip_ppp_info *slip_ppp;
    int         proto = 0;
    addrtype    *addr = NULL;
    int		passive = FALSE;
    int		routing = FALSE;
    int		headercompressed = FALSE;
    int         use_tacacs = FALSE;
    char	*aaa_method = NULL;
    int         keepalive_set = FALSE;
    int         mtu_set = FALSE;
    idbtype     *ipx_idb = NULL;
    int         confreq_resttime = FALSE;

    slip_ppp =  &trans->out.slip_ppp;

    if (dir == OUT) {
	proto = GETOBJ(int,8);
        addr = GETOBJ(paddr,7);
        routing = GETOBJ(int,9);
        headercompressed = GETOBJ(int,10);
        passive = GETOBJ(int,11);
	if (old_access_control) {
	    use_tacacs = GETOBJ(int,15);
	} else {
	    char *list_name;
	    switch (GETOBJ(int,15)) {
	    case 0:
	    case 1:
		list_name = aaa_method = "default";
		break;
	    case 2:
		list_name = aaa_method = GETOBJ(string,4);
		break;
	    default:
		list_name = NULL;
		buginf("\n%% PPP: AAA: bad value in authentication list.");
		break;
	    }
	    if ((csb->resolvemethod == RES_MANUAL) && list_name &&
		!aaa_is_list_defined(list_name, AAA_SVC_PPP)) {
		printf("AAA: Warning authentication list \"%s\" is not "
		       "defined for PPP.\n", list_name);
	    }
	}
	keepalive_set = GETOBJ(int,16);
	mtu_set = GETOBJ(int,18);
	confreq_resttime = GETOBJ(int,20);
	ipx_idb = GETOBJ(idb,1);
    }
    else {
	/*
	 * Don't accept incoming SLIP or PPP translations 
	 */
	printf("\n%%Translate: "
	       "Incoming SLIP/PPP translation is not supported");
	return(TRANS_NONE);
    }

    slip_ppp->proto = proto;

    /*
     * If Dynamic addressing is not specified, get the pool name and
     * obtain an address from DHCP or a LOCAL pool later on at the startup 
     * of the vty-async connection.  
     */
    addr->ip_addr ? slip_ppp->ipaddress = addr->ip_addr :
    setstring(&slip_ppp->poolname, GETOBJ(string,5));

    if (passive){
	slip_ppp->slip_ppp_flags |= TRANSF_SLIP_PPP_PASSIVE;
    }
    if (headercompressed){
	slip_ppp->slip_ppp_flags |= TRANSF_SLIP_PPP_HEADERCOMPRESSED;
    }
    if (routing){
	slip_ppp->slip_ppp_flags |= TRANSF_SLIP_PPP_ROUTING;
    }
    if (proto == PPP_PROTO) {
	if (csb->uiarray[0]) {
	    int i;
	    slip_ppp->slip_ppp_flags |= TRANSF_PPP_AUTH;
	    for (i = 0; i < PPP_AUTH_NUM; i++)
		slip_ppp->authtypes[i] = csb->uiarray[i];
	}
	if (old_access_control) {
	    if (use_tacacs)
		slip_ppp->slip_ppp_flags |= TRANSF_SLIP_PPP_USE_TACACS;
	} else {
	    if (aaa_method)
		sstrncpy(slip_ppp->aaa_method, aaa_method,
			 sizeof(slip_ppp->aaa_method));
	}
    }
    slip_ppp->keepalive_period = keepalive_set ? GETOBJ(int,17) : 
                                 KEEPALIVE_DEFAULT;
    slip_ppp->mtu = mtu_set ? GETOBJ(int,19): MAXETHERSIZE;
    slip_ppp->ipx_idb = ipx_idb;
    slip_ppp->confreq_resttime = confreq_resttime ? GETOBJ(int,21)*ONESEC : DEFTIMEOUT;
    return(TRANS_SLIP_PPP);		/* SLIP or PPP */
}

/*
 * pt_show_slip_ppp_transinfo
 * 
 * Show SLIP or PPP related translate information
 */
void pt_show_slip_ppp_transinfo (translatetype *trans, int dir)
{
    trans_slip_ppp_info *slip_ppp = &trans->out.slip_ppp;

    printf("%s ", slip_ppp->proto == SLIP_PROTO ? "SLIP" : "PPP");
    if (slip_ppp->ipaddress)
	printf("%i",slip_ppp->ipaddress);
    else {
        printf("IP-pool");
        if (slip_ppp->poolname)
            printf(" Scope-name %s", slip_ppp->poolname);
    }
    if (slip_ppp->ipx_idb)
        printf(" IPX-client %s", slip_ppp->ipx_idb->namestring);

    printf("%s%s%s", slip_ppp->slip_ppp_flags & TRANSF_SLIP_PPP_ROUTING ?
	   " Routing" : "",
           slip_ppp->slip_ppp_flags & TRANSF_SLIP_PPP_HEADERCOMPRESSED ?
	   "\n                Header-compression" : "",
           slip_ppp->slip_ppp_flags & TRANSF_SLIP_PPP_PASSIVE ? 
	   " Passive\n               " : "");
    if (slip_ppp->slip_ppp_flags & TRANSF_PPP_AUTH) {
	int i;
	printf(" Authentication");
	for (i = 0; (i < PPP_AUTH_NUM) && slip_ppp->authtypes[i]; i++) {
	    if (slip_ppp->authtypes[i] == PPP_AUTH_PAP)
		printf(" pap");
	    else if (slip_ppp->authtypes[i] == PPP_AUTH_CHAP)
		printf(" chap");
	}
    }
    printf("%s", ((slip_ppp->slip_ppp_flags & TRANSF_SLIP_PPP_USE_TACACS) &&
	    old_access_control) ? " Use-tacacs\n               " : "");

    if (MAXETHERSIZE != slip_ppp->mtu)
	printf(" Mtu %d", slip_ppp->mtu);
    if (KEEPALIVE_DEFAULT != slip_ppp->keepalive_period)
	printf(" Keepalive %d", slip_ppp->keepalive_period);
    if (DEFTIMEOUT != slip_ppp->confreq_resttime)
	 printf(" Restart-timer %d", slip_ppp->confreq_resttime/ONESEC);
}
/*
 * pt_nv_slip_ppp_transinfo
 * nv_add SLIP or PPP related translate information
 */
void pt_nv_slip_ppp_transinfo (translatetype *trans, int dir)
{
    trans_slip_ppp_info *slip_ppp = &trans->out.slip_ppp;

    nv_add(TRUE, " %s", slip_ppp->proto == SLIP_PROTO ? "slip" : "ppp");
    if (slip_ppp->ipaddress)
	nv_add(TRUE, " %i", slip_ppp->ipaddress);
    else {
        nv_add(TRUE, " ip-pool");
        nv_add(slip_ppp->poolname != NULL, " scope-name %s",
               slip_ppp->poolname);
    }
    if (slip_ppp->ipx_idb)
	nv_add(TRUE, " ipx %s", slip_ppp->ipx_idb->namestring);
    nv_add(slip_ppp->slip_ppp_flags & TRANSF_SLIP_PPP_ROUTING, " routing");
    nv_add(slip_ppp->slip_ppp_flags & TRANSF_SLIP_PPP_HEADERCOMPRESSED,
	   " header-compression");
    nv_add(slip_ppp->slip_ppp_flags & TRANSF_SLIP_PPP_PASSIVE, " passive");
    if (slip_ppp->slip_ppp_flags & TRANSF_PPP_AUTH) {
	int i;
        nv_add(TRUE, " authentication");
	for (i = 0; (i < PPP_AUTH_NUM) && slip_ppp->authtypes[i]; i++) {
	    nv_add((slip_ppp->authtypes[i] == PPP_AUTH_PAP), " pap");
	    nv_add((slip_ppp->authtypes[i] == PPP_AUTH_CHAP), " chap");
	}
    }
    if (old_access_control) {
	nv_add((slip_ppp->slip_ppp_flags & TRANSF_SLIP_PPP_USE_TACACS),
		" use-tacacs");
    } else {
	nv_add((slip_ppp->slip_ppp_flags & TRANSF_PPP_AUTH), " %s",
	       trans->out.slip_ppp.aaa_method);
    }
    nv_add(slip_ppp->keepalive_period != KEEPALIVE_DEFAULT, 
	   " keepalive %d", slip_ppp->keepalive_period); 
    nv_add(slip_ppp->mtu != MAXETHERSIZE, " mtu %d", slip_ppp->mtu);
    nv_add(slip_ppp->confreq_resttime != DEFTIMEOUT,
	" restart-timer %d", slip_ppp->confreq_resttime/ONESEC);
}
/*
 * pt_slip_ppp_add_alias
 * SLIP/PPP is accepted as output protocol only.  
 * Before we add it, do few sanity check on the address.
 *
 */
boolean pt_slip_ppp_add_alias (translatetype *trans)
{
    translatetype *trans_look, *next;
    ipaddrtype addr = trans->out.slip_ppp.ipaddress;

    /*
     * Make sure the address is not configured by another translation
     */
    for (trans_look = (translatetype *)translateQ->qhead;
         trans_look;
	 trans_look = next) {
        next = trans_look->next;
        if (OUTPROT(trans->type) == OUTPROT(trans_look->type)) {
	    /*
	     * Check to see if this entry in the list matches the one we're
	     * looking for
	     */
	    if ((addr) && (trans_look->out.slip_ppp.ipaddress == addr)) {
		printf("\n%% Address %i is claimed by another translation"
		       " command", addr);
		return(FALSE);
	    }
	}
	continue;
    }
    /*
     * If we have an address, sanity check it.
     */
    if (addr) {
	if (!check_other_asyncs(NULL, addr)) {
	    printf("\n%% Address %i in use on another interface", addr);
	    return(FALSE);
	}
	if (!check_slip_addr(addr)) {
	    printf("\n%% Address %i is already in use", addr);
	    return(FALSE);
	}
    }
    /*
     * The addition succeeded
     */
    return(TRUE);
}

/*
 * pt_async_trans_string
 * Take an int and return a string of the protocol name
 */
char *pt_async_trans_string (int prot)
{
    return("async");
}

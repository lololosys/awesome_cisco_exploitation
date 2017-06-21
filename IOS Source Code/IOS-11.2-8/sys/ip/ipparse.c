/* $Id: ipparse.c,v 3.16.4.19 1996/08/07 09:00:20 snyder Exp $
 * $Source: /release/112/cvs/Xsys/ip/ipparse.c,v $
 *------------------------------------------------------------------
 * ipparse.c -- configure interface IP parameters
 * 
 * June 1988, Eric B. Decker
 * June 1989, David R. Hampton (full blown ip security)
 *
 * Copyright (c) 1988-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ipparse.c,v $
 * Revision 3.16.4.19  1996/08/07  09:00:20  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.16.4.18  1996/08/06  00:05:04  rbadri
 * CSCdi65153:  ip mtu command not saved in NVRAM
 * Branch: California_branch
 * Save the value whether it is a sub-interface or not.
 *
 * Revision 3.16.4.17  1996/07/30  14:42:30  michellw
 * CSCdi64226:  Support larger max_frame_size (>1516) for LANE
 * Branch: California_branch
 * - The max_frame_size will be set to 1516 for ethernet and 4544 for TR
 *   if mtu is not configured on the sub-interface; otherwise, it will be
 *   set to based on the configured mtu.
 * - The lane module will not come up if the configured mtu is not valid.
 * - Add one more parameter (swidb) to the reg_invoke_media_change_mtu
 *   registry function.
 * - 1577 will come up with the default AIP mtu size (4470) if mtu is not
 *   configured on the sub-interface.
 *
 * Revision 3.16.4.16  1996/07/10  09:04:40  bcole
 * CSCdi60685: Access-List check broken on subinterface when fast
 * switching
 * Branch: California_branch
 *
 * Make sure we leave the IP access-list flags in the correct state after
 * making a config change to one of the subinterfaces.
 *
 * Revision 3.16.4.15  1996/06/28  23:17:52  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.16.4.14  1996/06/27  03:27:23  mleelani
 * CSCdi60876:  Multicast fastswitching flag should be compatible with
 * unicast
 * Branch: California_branch
 * Reflect unicast fastswitching polarity
 *
 * Revision 3.16.4.13  1996/06/17  23:32:35  hampton
 * Implement the long sought parser "default" keyword.  [CSCdi60510]
 * Branch: California_branch
 *
 * Revision 3.16.4.12  1996/05/23  07:37:25  myeung
 * CSCdi58029:  OSPF crash in ospf_interface_up() when unconfiguring OSPF
 * Branch: California_branch
 * - Add boolean parameter to ip_address_change registry to indicate if
 *   the change is for secondary or primary address.
 *
 * Revision 3.16.4.11  1996/05/21  09:51:35  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.16.4.10  1996/05/13  23:21:14  ahh
 * CSCdi56426:  NAT: EFT bugs and feedback
 * Fix hash algorithm, fix address block coalescing, reduce up-front
 * memory allocation, drop untranslatable packets, clean up debugs.
 * Branch: California_branch
 *
 * Revision 3.16.4.9  1996/05/09  14:31:02  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.16.4.8  1996/05/04  01:04:40  wilber
 * Branch: California_branch
 *
 * Integrated Routing and Bridging
 * - ARP, IP protocol, fast-switching and smf support for the Bridge-group
 *   Virtual Interface.
 * - Set up bridge/route indicators based on CRB/IRB is enabled or not.
 *
 * Constrained Multicast Flooding
 * - Convert tbridge flood array to a linked list of tbifd's.
 *
 * Name Access List
 *
 * Revision 3.16.4.7  1996/04/30  19:51:21  cheilek
 * CSCdi49287:  Insufficent space for show ip interface brief, with
 * subinterfaces
 * Branch: California_branch
 * Remove extra spaces between columns to allow two extra characters for
 * interface column.
 *
 * Revision 3.16.4.6  1996/04/29  05:20:27  pst
 * CSCdi55722:  Flapping Interface causes persistent full cache
 * invalidations
 * Branch: California_branch
 *
 * Revision 3.16.4.5.4.1  1996/04/27  07:04:51  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.16.4.5  1996/04/15  21:16:36  bcole
 * CSCdi54649:  IP Host Modularity, IP Automatic Stub Routing feature
 * commit
 * Branch: California_branch
 *
 * Revision 3.16.4.4  1996/04/02  05:36:35  dkatz
 * CSCdi36031:  Running out of memory running 2 or more EIGRP ASs
 * Branch: California_branch
 * EIGRP Jumbo Patch.
 *
 * Revision 3.16.4.3.4.1  1996/04/08  01:55:36  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.16.4.3  1996/03/28  00:15:33  jenny
 * CSCdi51402:  TCP Path-MTU Discovery needs to be dynamic
 * Branch: California_branch
 * Implemented TCP PMTU dynamic discovery and PMTU timers (RFC 1191).
 * Moved ip tcp parser command into a seperated parser chain in TCP for
 * modularity.
 *
 * Revision 3.16.4.2  1996/03/21  22:50:32  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.16.4.1  1996/03/18  20:17:33  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.16.6.2  1996/03/28  17:26:03  rbadri
 * Branch: LE_Syn_Cal_V111_1_3_branch
 * MTU support for sub-interfaces (used by ATM interfaces)
 *
 * Revision 3.16.6.1  1996/03/22  09:40:17  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.16  1996/03/11  19:25:57  ahh
 * CSCdi43020:  netbooting from not-directly connected tftp-server fails
 * Allow user to force a boot interface when spurious redirects cause
 * our automagic booting to fail.
 *
 * Revision 3.15  1996/03/04  15:31:13  dkerr
 * CSCdi50623:  IP flow cache needs better accounting granularity
 * Better show output.  Also allow flow stats to be exported.
 *
 * Revision 3.14  1996/02/24  02:05:52  pst
 * CSCdi49774:  modularize bgp regexp access lists (final part)
 *
 * Revision 3.13.6.1  1996/03/05  06:14:30  rlowe
 * Apply LANE content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.13.2.1  1996/02/27  21:09:05  cakyol
 * non sync sync of LE_Cal_V111_0_16_branch to V111_1_0_3 yielding
 * LE_Cal_V111_1_0_3_branch
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.13  1996/02/08  20:11:39  thille
 * CSCdi48347: format string in inlines, code size impact
 * Pass 1 - Clean up some literal strings in inlines, duplicate literals,
 * etc.  11,356 bytes saved on gs7-j-m, 9336 bytes saved on igs-j-l
 *
 * Revision 3.12  1996/02/07  16:13:00  widmer
 * CSCdi48285:  Remove function name from bad_parser_subcommand
 *
 * Revision 3.11  1996/02/03  23:22:54  lol
 * CSCdi48129:  setting per-user acl resets all interfaces
 *         Back out previous patch
 *
 * Revision 3.10  1996/02/03  17:46:31  lol
 * CSCdi48129:  setting per-user acl resets all interfaces
 *         Only update the cache for the interface which changed
 *
 * Revision 3.9  1996/01/23  08:50:01  mmcneali
 * CSCdi47323:  vLAN encapsulated IP SSE switched.
 *
 * Faster Baby, Faster!
 *
 * Revision 3.8.2.1  1996/02/15  21:50:37  rbadri
 * Branch: LE_Cal_V111_0_16_branch
 * Add an additonal swidb parameter to the registry bump_cache_version
 *
 * Revision 3.8  1995/12/13  23:36:51  jenny
 * CSCdi45241:  show ip int brief does not fit in 80 columns
 * Use namestring instead of short_namestring to display subinterface
 * names.
 *
 * Revision 3.7  1995/12/13  22:57:30  lol
 * CSCdi39791:  access-lists applied by tacacs+ dont work when
 * fast-switched
 *
 * Revision 3.6  1995/12/02  10:10:03  pst
 * CSCdi44893:  fix up whitespace padding in "show ip interface brief"
 *
 * Revision 3.5  1995/11/21  07:41:20  jenny
 * CSCdi43195:  TFTP: cannot specify source address
 *
 * Revision 3.4  1995/11/20  23:21:50  ahh
 * CSCdi41919:  PT: cannot specify source interface for TCP connections
 * Add source-interface option to outgoing TCP translation and telnet.
 * Also convert some parser defines to enums.
 *
 * Revision 3.3  1995/11/18  03:28:56  pst
 * CSCdi43467:  unumbered interfaces dont pick up changes based upon
 * numbered change
 *
 * Revision 3.2  1995/11/17  09:35:52  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:57:42  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.9  1995/11/08  21:05:08  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.8  1995/09/25  08:32:25  tli
 * CSCdi40960:  ISP jumbo patch 2
 *
 * Revision 2.7  1995/09/18  19:36:00  fox
 * CSCdi40523:  Add IP Address Pooling support for ISDN and other sync
 * interfaces
 * Modify IP address pooling to support all Point-to-Point interfaces
 * Remove Odd/Even Address feature
 * Add 'ppp negotiate' command
 *
 * Revision 2.6  1995/09/02  18:17:54  raj
 * CSCdi39165:  WFQ minor alignment errors
 * The IP header is not necessarily word aligned.  Thus, retrieving word
 * length fields should be done with GETLONG to avoid alignment traps on
 * MIPS.
 *
 * Revision 2.5  1995/08/29  21:04:23  jenny
 * CSCdi38480:  UDP: should be able to disable toy servers
 * Added new knob "service udp-small-servers" to turn on/off ECHO, DISCARD,
 * and CHARGEN ports and new knob "ip bootp server" to turn on/off BOOTP
 * service.
 *
 * Revision 2.4  1995/08/09  00:20:35  kramling
 * CSCdi38439:  FEIP product is now supported in 11.0.  Update support to
 * 10.3.
 *
 * Revision 2.3  1995/06/19  01:06:40  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.2  1995/06/09  03:29:57  fred
 * Change parameters to ip_pak_mac_rewrite to support IP accounting
 * in the fast path.
 * Add support of FS_DIALER in that routine.
 * Extend ipparse.c registry that displays IP datagram to display
 * UDP/TCP port as well.
 *
 * Revision 2.1  1995/06/07  21:00:01  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include <ciscolib.h>
#include "interface_private.h"
#include "ctype.h"
#include "packet.h"
#include "config.h"
#include "registry.h"
#include "sys_registry.h"
#include "ip.h"
#include "ipoptions.h"
#include "ipaccount.h"
#include "ipfast.h"
#include "../tcp/tcp.h"
#include "name.h"
#include "access.h"
#include "ipaccess.h"
#include "ieee.h"
#include "../iprouting/route.h"
#include "../iprouting/bgp.h"
#include "../if/arp.h"
#include "../if/network.h"
#include "../if/ether.h"
#include "../ui/command.h"
#include "../parser/parser_defs_tcpip.h"
#include "../tcp/tcpcompress.h"
#include "boxtypes.h"
#include "../clns/isis_externs.h"
#include "parser.h"
#include "registry.h"
#include "ip_registry.h"
#include "../hes/sse_registry.h"
#include "../os/signal.h"
#include "ip_addrpool.h"
#include "tftp.h"
#include "../ui/common_strings.h"
#include "../os/init.h"
#include "ip_media.h"

#include "../vlan/vlan.h"

/*
 * External parsing functions
 */
extern void ip_defaultgw_command(parseinfo *);

/*
 * Forward
 */
static void ip_flow_cache_command(parseinfo *csb);


/*
 * Process IP top level configuration commands.
 */
void ip_command (parseinfo *csb)
{
    switch (csb->which) {
    default:
	bad_parser_subcommand(csb, csb->which);
	break;
	
    case IP_DEFAULTGATEWAY:	/* default-gateway */
	ip_defaultgw_command(csb);
	break;
    case IP_ROUTING:		/* routing */
	if (csb->nvgen) {
	    if (router_enable) {
		if (GETOBJ(int, 1))
		    nv_write(ip_host_routing, "%s", csb->nv_command);
		else {
		    if (boxtype == BOXTYPE_TS) {
			/* A CS defaults to "no ip routing", unlike a router */
			
			nv_write(ip_router_running && !ip_host_routing,
				 "%s", csb->nv_command);
		    } else {
			
			nv_write(!ip_router_running && !ip_host_routing,
				 "no %s", csb->nv_command);
		    }
		}
	    }
	} else if (csb->set_to_default) {
	    ip_routing((boxtype != BOXTYPE_TS), FALSE);
	} else {
	    if (router_enable)
		ip_routing(csb->sense, GETOBJ(int, 1));
	    else
		printf(noasm);
	}
	
	break;
	
    case IP_SRCROUTE:		/* source-route */
	if (csb->nvgen)
	    nv_write(!ip_srcroute, "no %s", csb->nv_command);
	else
	    ip_srcroute = csb->set_to_default || csb->sense;
	break;
	
    case IP_SUBNETZERO:		/* subnet-zero */
	if (csb->nvgen) 
	    nv_write(subnetzero_enable, csb->nv_command);
	else 
	    subnetzero_enable = csb->sense;
	break;
	
    case IP_SECURITY:
	ip_glob_sec_command(csb);
	break;
	
    case IP_FLOW_CACHE:
	ip_flow_cache_command(csb);
	break;

    case IP_GRATUITOUS_ARPS:
	if (csb->nvgen)
	    nv_write(!ip_gratuitous_arps, "no %s", csb->nv_command);
	else
	    ip_gratuitous_arps = csb->sense;
	break;

    }
}

/*
 * probe_command
 * Enable or disable HP probe options for this interface.
 */

void probe_command (parseinfo *csb)
{
    idbtype *idb;
    hwidbtype *hwidb;

    idb = csb->interface;
    hwidb = idb->hwptr;

    if (csb->nvgen) {
	nv_write(idb->ip_probe_name, csb->nv_command);
	return;
    }
    idb->ip_probe_name = csb->sense;
    (*hwidb->reset)(hwidb);
    if (hwidb->counters.resets > 0)
	hwidb->counters.resets--;
}

/*
 * rarp_server_command
 * Configure/deconfigure rarp server spoofing for this interface
 */

void rarp_server_command (parseinfo *csb)
{
    idbtype *idb;

    idb = csb->interface;
    if (csb->nvgen) {
	nv_write(idb->ip_rarp_server_addr, "%s %i", csb->nv_command,
		 idb->ip_rarp_server_addr);
	return;
    }
    idb->ip_rarp_server_addr = (csb->sense ? GETOBJ(paddr,1)->ip_addr : 0);
}

/*
 * helper_command
 * Add or delete entries in the helper-address list
 */

void helper_command (parseinfo *csb)
{
    ipaddrtype address;
    ipaddtype *ptr;
    idbtype *idb;

    idb = csb->interface;

    if (csb->nvgen) {
	for (ptr = (ipaddtype *) idb->ip_helpernetQ.qhead;
	     ptr;
	     ptr = ptr->next) {
	    nv_write(TRUE, "%s %i", csb->nv_command, ptr->address);
	}
	return;
    }

    /*
     * If we didn't parse an IP address and there was a preceding "no",
     * then clear out the helper address list.
     */
    if (GETOBJ(paddr,1)->length == 0) {
	if (!csb->sense) {
	    while ((ptr = dequeue(&idb->ip_helpernetQ))) {
		free(ptr);
	    }
	} else {
	    printf("Bad IP address");
	}
	return;
    }

    /*
     * At this point we should have an IP address stored in csb->number.
     * See if the address already exists.  If so and there was a preceding
     * "no" keyword then clear that entry out of the list.  Otherwise
     * just quietly return.
     */
    address = GETOBJ(paddr,1)->ip_addr;

    for (ptr = (ipaddtype *) idb->ip_helpernetQ.qhead; ptr; ptr = ptr->next) {
	if (ptr->address == address) {
	    if (!csb->sense) {
		unqueue(&idb->ip_helpernetQ, ptr);
		free(ptr);
	    }
	    return;
	}
    }

    /*
     * Only thing left to do is check for preceding "no".  If it doesn't
     * exist then add the address, otherwise just return.
     */
    if (csb->sense) {
	ptr = malloc(sizeof(ipaddtype));
	if (ptr) {
	    ptr->address = address;
	    enqueue(&idb->ip_helpernetQ, ptr);
	}
    }
}

/*
 * ip_routing
 * Start or stop IP routing
 */

void ip_routing (boolean on, boolean hostflag)
{
    hwidbtype *idb;
    idbtype *swidb;

    if (!on && ip_router_running) {
	if (hostflag) {
	    ip_host_routing = FALSE; /* Turn of host routing policy */
	    return;
	}
    }

    /*
     * If 
     * - ip routing was not really turned on or off
     * - ip host routing mode was not really changed
     * Do nothing and return now.
     */
    if ((on == ip_router_running) && (ip_host_routing == hostflag))
	return;

    reg_invoke_ip_routing(on, (on == ip_router_running));
    ip_router_running = on;
    ip_host_routing = hostflag;
    FOR_ALL_SWIDBS(swidb) {
	ip_rxtypes_update(swidb);
	reg_invoke_iprouting_route_adjust(swidb);
    }

    /*
     * The following enables or disables ip fast switching when
     * IP routing is enabled or disabled (needed for bridging).
     */
    FOR_ALL_HWIDBS(idb) {
	if (on) {
	    idb->ip_routecache = IP_DEFAULT_ROUTECACHE;
	} else {
	    idb->ip_routecache = 0;
	}
	reg_invoke_ip_platform_default(idb, on);
	reg_invoke_ipcache_setup_interface(idb);
    }

    /*
     * Clear out the ARP cache and ICMP redirect cache
     * Whenever we change our routing state, we should start off with
     * fresh caches.
     */
    arp_flush_cache(NULL);
    ip_redirect_flush_cache(NULL);
}

/*
 * ip access-command
 *
 * Note that in addition to being called via the parser, this routine
 * is also called by tacacs to (de-)install per-user IP acls.  The
 * special value GETOBJ(int,10) is used to signal to ip_access_command
 * that this is a tacacs per-user access list operation. 
 */

void ip_access_command (parseinfo *csb)
{
    uint per_user;
    idbtype *idb = csb->interface, *tmp_idb;
    hwidbtype *hwidb;
    boolean list_set, named_list_set;
    acl_headertype *old, *new, *tmpacl;
    int list_num;
    char *name;

    if (csb->nvgen) {
	/*
	 * In this case, access lists are really the temporary values
	 * Use the default values instead
	 */
	if (idb->ip_input_accesslist_def)
	    nv_write(TRUE, "%s %s in",
		     csb->nv_command, idb->ip_input_accesslist_def->name);
	if (idb->ip_output_accesslist_def)
	    nv_write(TRUE, "%s %s out",
		     csb->nv_command, idb->ip_output_accesslist_def->name);
	return;
    }

    /* Per-user acls do not set the default list. Just the temporary.
     * in-use ones.
     */
    per_user = GETOBJ(int,10);

    if (csb->sense) {
	list_num = GETOBJ(int,1);
	name = GETOBJ(string, 1);

	/*
	 * Reset per-user ACL
	 */
	if (per_user && (list_num == IP_ACL_USER_RESET)) {
	    if (csb->which == IN)
		new = idb->ip_input_accesslist_def;
	    else
		new = idb->ip_output_accesslist_def;
	} else {

	    /*
	     * If GETOBJ(int,1) is non-zero, we have an old-style ACL,
	     * otherwise it's a named ACL.
	     */

	    if (list_num) {
		new = access_find_or_create_acl(list_num);
	    } else {
		if (!is_valid_nacl(name)) {
		    printf(nacl_name_error);
		    return;
		}
		new = access_find_or_create_nacl(name, ACL_UNDEFINED);
	    }
	}

    } else {
	new = NULL;		/* Remove access-group */
    }

    if (csb->which == IN) {
	old = idb->ip_input_accesslist;
       	idb->ip_input_accesslist = new;
	if (!per_user) {
	    idb->ip_input_accesslist_def = new;
	}
    } else {
	old = idb->ip_output_accesslist;
	idb->ip_output_accesslist = new;
	if (!per_user) {
	    idb->ip_output_accesslist_def = new;
	}
    }

    /*
     * Input access list flag setting is a little obscure.
     *
     * The fast switching code is already checking a flag on the OUTPUT
     * interface to see if access lists are enabled,  but there is currently
     * no check on the input interface.  Rather than slow the code down in
     * the now ACL case,  what we do is turn on the fast input access list
     * flag on ALL output interfaces if ANY interface has fast input
     * access lists.  This means we take a performance hit in the case
     * where we have either output or input ACL's enabled,  but not in
     * the mainline case.
     */
    named_list_set = FALSE;
    list_set = FALSE;
    FOR_ALL_SWIDBS(tmp_idb) {
	tmpacl = tmp_idb->ip_input_accesslist;
	if (tmpacl) {
	    list_set = TRUE;
	    if ((tmpacl->type == ACL_IP_NAMED_EXTENDED) || 
		(tmpacl->type == ACL_IP_NAMED_SIMPLE) ||
		(tmpacl->type == ACL_UNDEFINED))
		named_list_set = TRUE;
	}
    }	    

    if (list_set) {
	FOR_ALL_HWIDBS(hwidb) {
	    hwidb->ip_fast_flags |= IP_FAST_INPUTACCESSLISTS;
	    if (named_list_set)
		hwidb->ip_fast_flags |= IP_FAST_INPUT_NACL;
	}
    } else {
	FOR_ALL_HWIDBS(hwidb) {
	    hwidb->ip_fast_flags &= ~IP_FAST_INPUTACCESSLISTS;
	    hwidb->ip_fast_flags &= ~IP_FAST_INPUT_NACL;
	}
    }

    /*
     * Check to see if output access list filtering is enabled on any
     * of our subinterfaces.  If so, set the flag for the entire hwidb.
     */
    list_set = FALSE;
    FOR_ALL_SWIDBS_ON_HW(idb->hwptr, tmp_idb) {
	if (tmp_idb->ip_output_accesslist) {
	    list_set = TRUE;
	    break;
	}
    }
    if (list_set)
	idb->hwptr->ip_fast_flags |= IP_FAST_ACCESSLISTS;
    else
	idb->hwptr->ip_fast_flags &= ~IP_FAST_ACCESSLISTS;
    
    if (idb->hwptr->status & IDB_CBUS)
	reg_invoke_ipcache_invalidate(idb->hwptr, NULL, FALSE);
    
    if ((csb->sense && (old != new)) || !csb->sense)
	reg_invoke_ip_access_group(idb, (csb->which == IN), new); 
}

/* 
 * This routine is called by Tacacs+ to (de-)install per-user IP acls
 * by faking up a parsed command.  The special value GETOBJ(int,10) is
 * used to signal to ip_access_command that this is a per-user list. 
 */
void
ip_set_per_user_acl (idbtype *swidb, int direction, uint val)
{
    parseinfo *csb = get_csb("Access-group CSB");

    if (csb == NULL) {
	return;
    }

    csb->interface = swidb;

    csb->val10 = 1;

    csb->nvgen = 0;         /* not doing nv-generation */
    csb->which = direction; /* IN or OUT */
    csb->sense = 1;         /* 1 means we're setting an acl */
    csb->val1 = val;        /* new acl value to be set */

    ip_access_command(csb);

    free_csb(&csb);
}

/*
 * ip accounting
 */

void ip_accounting_command (parseinfo *csb)
{
    idbtype *idb = csb->interface;

    if (csb->nvgen) {
	if (GETOBJ(int,1) >= NUM_IP_ACCOUNTING_TYPES) /* Parser cruft */
	    return;
	if (GETOBJ(int,1) == IP_ACCOUNTING_VIOLATIONS)
	    nv_write(idb->ip_violation_acctg, csb->nv_command);
	else
	    nv_write(idb->ip_output_acctg, csb->nv_command);
	return;
    }

    /*
     * Special kludge.  If the type is NUM_IP_ACCOUNTING_TYPES, this
     * says that they did either a "no ip accounting", meaning that we
     * should turn off all ip accounting, or just "ip accounting",
     * which we default to "ip accounting output".
     *
     * Note also that if we turn on violation accounting we disable
     * fastswitching on the interface.
     */

	if (GETOBJ(int,1) == NUM_IP_ACCOUNTING_TYPES) { 
	    idb->ip_output_acctg = csb->sense;
	    if (!csb->sense)
		idb->ip_violation_acctg = csb->sense;
	} else {
	    if (GETOBJ(int,1) == IP_ACCOUNTING_VIOLATIONS) {
		idb->ip_violation_acctg = csb->sense;
	    } else {
		idb->ip_output_acctg = csb->sense;
	    }
	} 

    if (!idb->ip_output_acctg)
	idb->hwptr->ip_fast_flags &= ~IP_FAST_ACCOUNTING;
    ip_acct_fixup(idb->ip_output_acctg);
    if (idb->ip_output_acctg)
	idb->hwptr->ip_fast_flags |= IP_FAST_ACCOUNTING;
    if (idb->hwptr->status & IDB_CBUS)
	reg_invoke_ipcache_invalidate(idb->hwptr, NULL, FALSE);
}

/* 
 * ip_split_horizon_command
 */

void ip_split_horizon_command (parseinfo *csb)
{
    idbtype *idb = csb->interface;

    if (csb->nvgen) {
	nv_write(idb->ip_split_disabled != idb->ip_split_disabled_def,
		 "%s%s", idb->ip_split_disabled ? "no " : "", csb->nv_command);
	return;
    }
    
    if (csb->set_to_default)
	idb->ip_split_disabled = idb->ip_split_disabled_def;
    else
	idb->ip_split_disabled = !(csb->sense);
}

/* ip_broadcast_command
 */

void ip_broadcast_command (parseinfo *csb)
{
    idbtype *idb = csb->interface;

    if (csb->nvgen) {
	nv_write(idb->ip_broadcast != -1L,
		 "%s %i", csb->nv_command, idb->ip_broadcast);
	return;
    }
    if (csb->sense) {
	idb->ip_broadcast = GETOBJ(paddr,1)->ip_addr;
    } else {
      idb->ip_broadcast = -1L;
  }
}

/*
 * ip directed-broadcast interface subcommand
 */

void ip_directbcast_command (parseinfo *csb)
{
    idbtype *idb = csb->interface;
    int list;

    if (csb->nvgen) {
	nv_write(!idb->ip_directbcast, "no %s", csb->nv_command);
	nv_write(IPBROADCASTLIST_CONFIGURED(idb), "%s %d", csb->nv_command,
		 idb->ip_directbcast);
	return;
    }
    if (csb->set_to_default) {
	idb->ip_directbcast = -1;
    } else if (!csb->sense) {
	idb->ip_directbcast = 0;
    } else {
	list = GETOBJ(int, 1);
	idb->ip_directbcast = list ? list : -1;
    }
}

/*
 * ip mask-reply interface subcommand
 */

void ip_maskreply_command (parseinfo *csb)
{
    idbtype *idb = csb->interface;

    if (csb->nvgen) {
	nv_write(idb->ip_maskreply, csb->nv_command);
    } else {
	idb->ip_maskreply = csb->sense;
    }
}

/*
 * ip_mtu_command
 * Change the IP MTU of an interface.
 */

void ip_mtu_command (parseinfo *csb)
{
    int mtu, old;
    idbtype *idb = csb->interface;

    if (csb->nvgen) {
	nv_write((idb->ip_mtu !=  def_proto_mtu(idb, LINK_IP)),
		 "%s %d", csb->nv_command, idb->ip_mtu);
	return;
    }

    if (!csb->sense) {
	mtu = idb->ip_mtu;
	idb->ip_mtu = def_proto_mtu(idb, LINK_IP);
	reg_invoke_ip_change_mtu(idb->hwptr, idb, idb->ip_mtu - mtu);
	reg_invoke_ipcache_invalidate(idb->hwptr, NULL, FALSE);
	return;
    }
    mtu = GETOBJ(int,1);
    if ((mtu < 64) || (mtu > max_proto_mtu(idb, LINK_IP))) {
	printf("\n%% Invalid IP MTU specified; minimum 64, maximum %d\n",
	       max_proto_mtu(idb, LINK_IP));
    } else {
	old = idb->ip_mtu;
	idb->ip_mtu = mtu;
	reg_invoke_ip_change_mtu(idb->hwptr, idb, old - mtu);
	reg_invoke_ipcache_invalidate(idb->hwptr, NULL, FALSE);
    }
}

/*
 * ip proxy-arp interface subcommand
 */

void ip_proxyarp_command (parseinfo *csb)
{
    idbtype *idb = csb->interface;

    if (csb->nvgen) {
	nv_write(idb->ip_proxy_disabled,
		 "no %s", csb->nv_command);
    } else {
	idb->ip_proxy_disabled = !csb->sense;
    }
}

/*
 * ip redirects interface subcommand
 */

void ip_redirects_command (parseinfo *csb)
{
    idbtype *idb = csb->interface;

    if (csb->nvgen) {
	nv_write(!idb->ip_redirect, "no %s", csb->nv_command);
    } else {
	idb->ip_redirect = csb->set_to_default || csb->sense;
	/*
	 * Remove old entries from IP cache for fastswitching on the same
	 * interface
	 */
	if (idb->ip_routecache_same && idb->ip_redirect)
	    reg_invoke_ipcache_invalidate(idb->hwptr, NULL, FALSE);
    }
}

/*
 * IP unnumbered interface subcommand
 */
void ip_unnumbered_command (parseinfo *csb)
{
    idbtype *idb = csb->interface;

    if (csb->nvgen) {
	if (idb->ip_unnumbered) 
	    nv_write(TRUE, "%s %s", csb->nv_command,
		     idb->ip_unnumbered->namestring); 
	return;
    }
    if (!is_p2p(idb)) {
	printf("Point-to-point (non-multi-access) interfaces only");
	return;
    }
    /*
     * Note : Because of integrated ISIS ip_address_change 
     * must be called after ip is disabled on that interface.
     */
    if (!csb->sense) {
	if (!idb->ip_address) {
	    ip_address_change(idb, 0, 0, FALSE, FALSE);
	    idb->ip_unnumbered = NULL;
	    idb->ip_enabled = csb->sense;
	}
    } else {
	if (idb->ip_address) {
	    printf("Remove IP address first");
	    return;
	}
	if (GETOBJ(idb,1)->ip_unnumbered) {
	    printf("Cannot use an unnumbered interface: %s", 
		   GETOBJ(idb,1)->namestring);
	    return;
	}
	if (idb == GETOBJ(idb,1)) {
	    printf("Cannot use self");
	    return;
	}
	idb->ip_unnumbered = GETOBJ(idb,1);
    }	   
    idb->ip_enabled = csb->sense;
    if (csb->sense)
	ip_address_change(idb, 0, 0, FALSE, TRUE);
    reg_invoke_ipcache_setup_interface(idb->hwptr);
}

/*
 * route cache interface subcommand
 * OBJ(int,1) = IP_CBUS_ROUTECACHE if cbus specified
 *            = IP_SSE_ROUTECACHE if sse specified
 *            = IP_FLOW_ROUTECACHE if flow specfied
 */
void routec_command (parseinfo *csb)
{
    hwidbtype *hwidb = csb->interface->hwptr;

    if (csb->nvgen) {
	if (GETOBJ(int,1)) {
	    (void) reg_invoke_ip_route_cache_cmd(GETOBJ(int,1), csb);
	} else {
	    if (!hwidb->ip_routecache) {
		nv_write(TRUE, "no %s", csb->nv_command);
	    }
	}
	return;
    }	

    if (csb->set_to_default) {
	hwidb->ip_routecache =
	    ip_router_running ? IP_FAST_ROUTECACHE : 0;
    } else if (GETOBJ(int,1)) {
	if (!reg_invoke_ip_route_cache_cmd(GETOBJ(int,1), csb)) {
	    return;
	}

    } else {
	hwidb->ip_routecache = csb->sense;
    }

    reg_invoke_ipcache_setup_interface(hwidb);
    reg_invoke_ipmulticast_fs_setup_interface(hwidb);
}

/*
 * [no] ip route-cache same-interface interface subcommand
 */
void routec_same_int_command (parseinfo *csb)
{
    idbtype *idb = csb->interface;

    if (!idb->ip_enabled)
      return;			/* NOP if IP is not enabled */
    
    if (csb->nvgen) {
	if (idb->ip_routecache_same_def)      /* If defaults to ok, then.. */
	    nv_write(!idb->ip_routecache_same,
		     "no ip route-cache same-interface");
	else
	    nv_write(idb->ip_routecache_same, "ip route-cache same-interface");
	return;
    }
	
    idb->ip_routecache_same = csb->sense;
    if (!csb->sense)
	reg_invoke_ipcache_invalidate(idb->hwptr, NULL, FALSE);
}

/*
 * IP unreachable interface subcommand
 */
void ip_unreachable_command (parseinfo *csb)
{
    if (csb->nvgen) {
	nv_write(csb->interface->ip_unreach == FALSE, 
		 "no %s", csb->nv_command);
	return;
    }
    csb->interface->ip_unreach = csb->set_to_default || csb->sense;
}

void ip_glob_sec_command (parseinfo *csb)
{
    int i, j;
    ip_sec_compartment_info new_info;

    if (csb->nvgen) {
	for (i = 0; i < ip_sec_info_entries; i++)
	    nv_write(TRUE, "%s %d %d %d", csb->nv_command,
		     ip_sec_info[i].source_id, ip_sec_info[i].max_bytes,
		     ip_sec_info[i].bit_default);
	return;
    }
    new_info.source_id = GETOBJ(int, 1);
    new_info.max_bytes = GETOBJ(int, 2);
    new_info.bit_default = GETOBJ(int, 3);
    if (csb->sense) {
	/*
	 * Overwrite existing info if source_id matches
	 */
	for (i = 0; i < ip_sec_info_entries; i++)
	    if (ip_sec_info[i].source_id == new_info.source_id) {
		if (!ip_eso_valid_max(new_info.max_bytes)) {
		    printf("NLESO byte limit smaller than existing"
			   " configured source.");
		    return;
		}
		ip_sec_info[i] = new_info;
		return;
	    }
	if (ip_sec_info_entries == MAX_NLESO) {
	    printf("No room for additional NLESO sources; %d sources max",
		   MAX_NLESO);
	    return;
	}
	ip_sec_info[ip_sec_info_entries++] = new_info;
    } else {
	for (i = 0; i < ip_sec_info_entries; i++)
	    if (ip_sec_info[i].source_id == new_info.source_id) {
		ip_sec_info_entries--;
		for (j = i; j < ip_sec_info_entries; j++)
		    ip_sec_info[j] = ip_sec_info[j+1];
		/*
		 * Need to remove all references for this NLESO since we now
		 * no longer know the max. # of bytes allowed
		 */
		ip_eso_remove_source(new_info.source_id);
		return;
	    }
	/*
	 * Silently fail to modify table if no match found
	 */
    }
}

/*
 * ip_security_ext_command
 */
void ip_security_ext_command (parseinfo *csb)
{
    idbtype *idb;
    queuetype *queue_ptr;
    ip_sec_eso_source *eso_source;
    uchar type, bytes, source, *compartment_info;
    int i;
    boolean found;
    
    idb = csb->interface;
    type = GETOBJ(int, 1);
    if (type == IP_SEC_AESO)
	queue_ptr = &idb->ip_sec_aeso;
    else
	queue_ptr = &idb->ip_sec_nleso;
    if (csb->nvgen) {
	for (eso_source = (ip_sec_eso_source *)queue_ptr->qhead;
	     eso_source; eso_source = eso_source->next) {
	    if (type != eso_source->type)
		continue;
	    nv_write(TRUE, "%s %d ", csb->nv_command, eso_source->source_id);
	    for (i = 0; i < eso_source->compartment_bytes; i++)
		nv_add(TRUE, "%02x", eso_source->compartment_info[i]);
	}
	return;
    }
    source = GETOBJ(int, 2);
    /*
     * Search for match from existing table
     */
    for (eso_source = (ip_sec_eso_source *)queue_ptr->qhead; eso_source;
	 eso_source = eso_source->next) {
	if (eso_source->source_id == source && eso_source->type == type)
	    break;
    }
    if (csb->sense) {
	bytes = GETOBJ(int, 3);
	if (bytes < 1 || bytes > MAX_COMPARTMENT_BYTES) {
	    printf("Invalid compartment data length");
	    return;
	}
	compartment_info = GETOBJ(string, 1);
	/*
	 * Make sure number of bytes specified is in range with system-wide
	 * per NLESO limits
	 */
	found = FALSE;
	for (i = 0; i < ip_sec_info_entries; i++) {
	    if (ip_sec_info[i].source_id != source)
		continue;
	    if (ip_sec_info[i].max_bytes < bytes) {
		printf("Compartment data exceeds length limit (%d bytes) configured in ESO default information table", ip_sec_info[i].max_bytes);
		return;
	    }
	    found = TRUE;
	    break;
	}
	if (!found) {
	    printf("NLESO source (%d) must first be defined in ESO default information table", source);
	    return;
	}
	/*
	 * Defining an IPSO ESO source implies we want to check extended
	 * IPSO on incoming packets
	 */
        idb->ip_sec_extended = IP_ESO_CHECK;
	if (eso_source) {
	    /*
	     * Modify existing entry
	     */
	    bcopy(compartment_info, eso_source->compartment_info, bytes);
	    eso_source->compartment_bytes = bytes;
	    return;
	}
	/*
	 * Add new entry
	 */
	eso_source = malloc(sizeof(ip_sec_eso_source));
	if (eso_source == NULL) {
	    printf(nomemory);
	    return;
	}
	eso_source->source_id = source;
	eso_source->type = type;
	bcopy(compartment_info, eso_source->compartment_info, bytes);
	eso_source->compartment_bytes = bytes;
	enqueue(queue_ptr, eso_source);
    } else {
	/*
	 * Delete entry
	 */
	if (eso_source)
	    p_unqueue(queue_ptr, eso_source);
    }
}

/*
 * ip_security_command
 */
void ip_security_command (parseinfo *csb)
{
    idbtype *idb;
    uchar new_lev_low, new_lev_high;
    uchar new_auth_low, new_auth_high;

    idb = csb->interface;

    if (csb->nvgen) {
	switch (csb->which) {
	case P_IPSECMODE_DEDICATED:
	    if (idb->ip_sec_default || idb->ip_sec_multilevel)
		return;
	    nv_write(TRUE, "%s %s %s", csb->nv_command,
		     security_string(idb->ip_sec_high),
		     authority_string(&idb->ip_sec_auth_high));
	    break;
	case P_IPSECMODE_MULTILEVEL:
	    if (idb->ip_sec_default || !idb->ip_sec_multilevel)
		return;
	    nv_write(TRUE, "%s %s %sto %s ", csb->nv_command,
		     security_string(idb->ip_sec_low),
		     authority_string(&idb->ip_sec_auth_low),
		     security_string(idb->ip_sec_high));
	    nv_add(TRUE, "%s", authority_string(&idb->ip_sec_auth_high));
	    break;
	case P_IPSECSTYLE_IMPLICIT:
	    if (idb->ip_sec_default)
		return;
	    if (idb->ip_sec_multilevel)
		nv_write(idb->ip_sec_implicit, "%s %s %s",
			 csb->nv_command,
			 security_string(idb->ip_sec_def_class),
			 authority_string(&idb->ip_sec_def_auth));
	    else {
		if ((idb->ip_sec_high == SECLVL_UNCLASSIFIED) &&
		    (idb->ip_sec_auth_high == AUTH_GENSER)) {
		    nv_write(!idb->ip_sec_implicit, "no %s", csb->nv_command);
		} else {
		    nv_write(idb->ip_sec_implicit, "%s", csb->nv_command);
		}
	    }
	    break;
	case P_IPSECSTYLE_ADD:
	    if (idb->ip_sec_default)
		return;
	    if (!idb->ip_sec_multilevel &&
		(idb->ip_sec_high == SECLVL_UNCLASSIFIED) &&
		(idb->ip_sec_auth_high == AUTH_GENSER)) {
		nv_write(idb->ip_sec_outstyle & SECSTYLE_ADD,
			 "%s", csb->nv_command);
	    } else {
		nv_write(!(idb->ip_sec_outstyle & SECSTYLE_ADD),
			 "no %s", csb->nv_command);
	    }
	    break;
	case P_IPSECSTYLE_STRIP:
	    nv_write(idb->ip_sec_outstyle & SECSTYLE_STRIP,
		     "%s", csb->nv_command);
	    break;
	case P_IPSECSTYLE_FIRST:
	    nv_write(idb->ip_sec_outstyle & SECSTYLE_FIRST,
		     "%s", csb->nv_command);
	    break;
	case P_IPSECSTYLE_IGNOREAUTH:
	    nv_write(idb->ip_sec_instyle  & SECSTYLE_IGNOREAUTH,
		     "%s", csb->nv_command);
	    break;
	case P_IPSECSTYLE_IGNORECIPSO:
	    nv_write(idb->ip_sec_cipso == IP_CIPSO_IGNORE, 
		     "%s", csb->nv_command);
	    break;
	case P_IPSEC_ALLOW_RESERVED:
	    nv_write(idb->ip_sec_allow_rsrvd, "%s", csb->nv_command);
	    break;
	case P_IPSECSTYLE_EXTSECOK:
	    nv_write((idb->ip_sec_extended == IP_ESO_CHECK),
		     "%s", csb->nv_command);
	    break;
	case P_IPSECSTYLE_IGNOREEXT:
	    nv_write((idb->ip_sec_extended == IP_ESO_IGNORE),
		     "%s", csb->nv_command);
	    break;
	}
	return;
    }

    reg_invoke_ipcache_invalidate(NULL, NULL, TRUE);

    /*
     * The command "no ip security" turns off all security on this
     * interface. The command "no ip security STYLE" turns off a
     * particular input allowance or style of output coercion.
     */
    if (csb->sense == FALSE) {
	switch (csb->which) {

	case P_IPSEC_DEFAULT:
	    /*
	     *  This is "no ip security"
	     */
	    idb->ip_sec_default    = TRUE;
	    idb->ip_sec_implicit   = TRUE;
	    idb->ip_sec_extended   = IP_ESO_DENY;
	    idb->ip_sec_cipso	   = IP_CIPSO_DENY;
	    idb->ip_sec_multilevel = FALSE;
	    idb->ip_sec_instyle    = SECSTYLE_NONE;
	    idb->ip_sec_outstyle   = SECSTYLE_NONE;
	    idb->ip_sec_low        = SECLVL_UNCLASSIFIED;
	    idb->ip_sec_high       = SECLVL_UNCLASSIFIED;
	    idb->ip_sec_def_class  = SECLVL_UNCLASSIFIED;
	    idb->ip_sec_auth_low   = AUTH_GENSER;
	    idb->ip_sec_auth_high  = AUTH_GENSER;
	    idb->ip_sec_def_auth   = AUTH_GENSER;
	    idb->ip_sec_allow_rsrvd= FALSE;
	    ip_eso_disable(idb);
	    break;

	case P_IPSEC_ALLOW_RESERVED:
	    idb->ip_sec_allow_rsrvd = FALSE;
	    break;

	case P_IPSECSTYLE_ADD:
	    idb->ip_sec_outstyle &= ~SECSTYLE_ADD;
	    break;

	case P_IPSECSTYLE_STRIP:
	    idb->ip_sec_outstyle &= ~SECSTYLE_STRIP;
	    break;

	case P_IPSECSTYLE_FIRST:
	    idb->ip_sec_outstyle &= ~SECSTYLE_FIRST;
	    break;

	case P_IPSECSTYLE_IMPLICIT:
	    idb->ip_sec_implicit = FALSE;
	    break;

	case P_IPSECSTYLE_IGNORECIPSO:
	    if (idb->ip_sec_cipso == IP_CIPSO_IGNORE)
		idb->ip_sec_cipso = IP_CIPSO_DENY;
	    break;

	case P_IPSECSTYLE_IGNOREAUTH:
	    idb->ip_sec_instyle &= ~SECSTYLE_IGNOREAUTH;
	    break;

	case P_IPSECSTYLE_EXTSECOK:
	    if (idb->ip_sec_extended == IP_ESO_CHECK)
		idb->ip_sec_extended = IP_ESO_DENY;
	    break;

	case P_IPSECSTYLE_IGNOREEXT:
	    if (idb->ip_sec_extended == IP_ESO_IGNORE)
		idb->ip_sec_extended = IP_ESO_DENY;
	    break;

	case P_IPSECMODE_MULTILEVEL:
	case P_IPSECMODE_DEDICATED:
	    printf("Use 'no ip security' to reset\n");
	    break;
	}

	/*
	 * recompute system security level
	 */
	recompute_idbsec(idb);
	recompute_syssec();
	return;
    }

    /*
     * "ip security <something>"  Find out what.
     */
    switch (csb->which) {

    case P_IPSEC_ALLOW_RESERVED:
        idb->ip_sec_allow_rsrvd = TRUE;
	break;

    case P_IPSECSTYLE_ADD:
	idb->ip_sec_outstyle &= ~SECSTYLE_STRIP;
	idb->ip_sec_outstyle |= SECSTYLE_ADD;
	break;
	
    case P_IPSECSTYLE_STRIP:
	if (idb->ip_sec_multilevel) {
	    printf("\nIllegal on multilevel interface: %s",
		   csb->line);
	    return;
	}
	idb->ip_sec_outstyle &= ~SECSTYLE_ADD;
	idb->ip_sec_outstyle |= SECSTYLE_STRIP;
	break;

    case P_IPSECSTYLE_FIRST:
	idb->ip_sec_outstyle |= SECSTYLE_FIRST;
	break;

    case P_IPSECSTYLE_IMPLICIT:
	new_lev_low = GETOBJ(int,1);
	new_auth_low = GETOBJ(int,2);
	if (idb->ip_sec_multilevel) {
	    if ((new_lev_low == 0) ||
		(new_auth_low == AUTH_NONE)) {
		  printf("\nImplicit needs a specific label when multilevel");
		  return;
	    }
	    if (!inclass(idb, new_lev_low) || !inauthl(idb, new_auth_low) ||
		!inauthh(idb, new_auth_low)) {
		printf("\nImplicit label outside interface range.");
		return;
	    }
	    idb->ip_sec_def_class = new_lev_low;
	    idb->ip_sec_def_auth = new_auth_low;
	} else {
	    if (new_lev_low && new_auth_low)
		printf("\nMultilevel not set; level and authority ignored");
	    idb->ip_sec_def_class = idb->ip_sec_low;
	    idb->ip_sec_def_auth = idb->ip_sec_auth_low;
	}
	idb->ip_sec_implicit = TRUE;
	break;

    case P_IPSECSTYLE_IGNORECIPSO:
	idb->ip_sec_cipso = IP_CIPSO_IGNORE;
	break;

    case P_IPSECSTYLE_IGNOREAUTH:
	if (idb->ip_sec_multilevel) {
	    printf("\nIllegal on multilevel interface: %s",
		   csb->line);
	    return;
	}
	idb->ip_sec_instyle |= SECSTYLE_IGNOREAUTH;
	break;

    case P_IPSECSTYLE_EXTSECOK:
	idb->ip_sec_extended = IP_ESO_CHECK;
	break;

    case P_IPSECSTYLE_IGNOREEXT:
	idb->ip_sec_extended = IP_ESO_IGNORE;
	break;

    /*
     * The commands DEDICATED and MULTILEVEL follow.  They reset the
     * values of all keywords above this point.
     */
    case P_IPSECMODE_MULTILEVEL:
	/*
	 * ip security multilevel low_level [auth ...] to
	 * high_level auth [auth ...]
	 */
	new_lev_low = GETOBJ(int,3);
	new_auth_low = GETOBJ(int,4);
	new_lev_high = GETOBJ(int,1);
	new_auth_high = GETOBJ(int,2);
	
	/*
	 * Sanity check the input values.  A low authority of NONE, and
	 * a high of only one organization yeilds a constant value.
	 */
	if (new_lev_low > new_lev_high) {
	    printf("\nCannot set low security level above high level");
	    return;
	}
	if (new_auth_high == AUTH_NONE) {
	    printf("\nCannot default high authority level");
	    return;
	}
	if ((new_auth_low & new_auth_high) != new_auth_low) {
	    printf("\nLow security authority must be subset of high authority");
	    return;
	}
	if ((new_auth_low == AUTH_NONE) &&
	    (new_auth_high == AUTH_GENSER || new_auth_high == AUTH_SIOP_ESI ||
	     new_auth_high == AUTH_SCI || new_auth_high == AUTH_NSA ||
	     new_auth_high == AUTH_DOE))
	    new_auth_low = new_auth_high;
	
	/*
	 *  Everything passed.  Set the interface.
	 */
	if ((new_lev_low == new_lev_high) && (new_auth_low == new_auth_high))
	    goto set_dedicated;
	idb->ip_sec_multilevel = TRUE;
	idb->ip_sec_low        = new_lev_low;
	idb->ip_sec_high       = new_lev_high;
	idb->ip_sec_def_class  = new_lev_high;
	idb->ip_sec_auth_low   = new_auth_low;
	idb->ip_sec_auth_high  = new_auth_high;
	idb->ip_sec_def_auth   = new_auth_high;
	idb->ip_sec_implicit   = FALSE;
	idb->ip_sec_outstyle   = SECSTYLE_ADD;
	idb->ip_sec_instyle    = SECSTYLE_NONE;
	break;
    case P_IPSECMODE_DEDICATED:
	/*
	 * ip security dedicated level auth [auth ...]
	 */
	new_lev_high = GETOBJ(int,1);
	new_auth_high = GETOBJ(int,2);
	
	/*
	 * Check the values just read.  An unclassified interface may
	 * default the authority to GENSER.
	 */
	if (new_auth_high == AUTH_NONE) {
	    if (new_lev_high == SECLVL_UNCLASSIFIED)
		new_auth_high = AUTH_GENSER;
	    else {
		printf("\nCannot default IP security authority");
		return;
	    }
	}
	
set_dedicated:
	/*
	 *  Everything passed.  Set the interface.
	 */
	idb->ip_sec_multilevel = FALSE;
	idb->ip_sec_low        = new_lev_high;
	idb->ip_sec_high       = new_lev_high;
	idb->ip_sec_def_class  = new_lev_high;
	idb->ip_sec_auth_low   = new_auth_high;
	idb->ip_sec_auth_high  = new_auth_high;
	idb->ip_sec_def_auth   = new_auth_high;
	idb->ip_sec_instyle    = SECSTYLE_NONE;
	if ((new_lev_high == SECLVL_UNCLASSIFIED) && (new_auth_high == AUTH_GENSER)) {
	    idb->ip_sec_implicit = TRUE;
	    idb->ip_sec_outstyle = SECSTYLE_NONE;
	} else {
	    idb->ip_sec_implicit = FALSE;
	    idb->ip_sec_outstyle = SECSTYLE_ADD;
	}
	break;
    }
    recompute_idbsec(idb);
    recompute_syssec();
}

/*
 * recompute_idbsec
 * Is this interface back to the default security level?
 * The default is Unclassified Genser with implicit labelling.
 */

void recompute_idbsec (idbtype *idb)
{
    if ((idb->ip_sec_high == SECLVL_UNCLASSIFIED) &&
	(idb->ip_sec_auth_high == AUTH_GENSER) &&
	!idb->ip_sec_multilevel &&
	!idb->ip_sec_outstyle &&
	!idb->ip_sec_instyle &&
	idb->ip_sec_implicit)
        idb->ip_sec_default = TRUE;
    else
        idb->ip_sec_default = FALSE;
}

/*
 *  recompute_syssec
 *  Recompute system security level.  The low system security level
 *  is the min of all low interface levels not including interfaces
 *  that have no security.  The high system security level is similar
 *  except the high interface levels are used.
 *
 *  We need to set ip_secopt_bytes if any interface has a non default
 *  security label or style.  This forces all internally generated
 *  packets to include room for an IP security option.
 */

void recompute_syssec (void)
{    
    idbtype *idb;

    ip_secsys_low = ip_secsys_high = SECNONE;
    ip_secsys_auth_low = AUTH_UNSET;
    ip_secsys_auth_high = AUTH_NONE;
    ip_secopt_bytes = 0;
    FOR_ALL_SWIDBS(idb) {
	if (!idb->ip_enabled)
	    continue;
	if (!idb->ip_sec_default)
	    ip_secopt_bytes = SECURITY_SIZE;
	if ((ip_secsys_low == SECNONE) || (idb->ip_sec_low < ip_secsys_low))
	    ip_secsys_low = idb->ip_sec_low;
	if ((ip_secsys_high == SECNONE) ||(idb->ip_sec_high > ip_secsys_high))
	    ip_secsys_high = idb->ip_sec_high;
	ip_secsys_auth_low &= idb->ip_sec_auth_low;
	ip_secsys_auth_high |= idb->ip_sec_auth_high;
    }
    if (ip_secsys_low == SECNONE) {
        ip_secsys_low = ip_secsys_high = SECLVL_UNCLASSIFIED;
	ip_secsys_auth_low = ip_secsys_auth_high = AUTH_GENSER;
    }
}

/*
 * ip_show_interface_brief
 * Display brief information about IP on an interface.
 */

void ip_show_interface_brief (idbtype *target)
{
    idbtype *idb;
    char *resolve;

    automore_enable(NULL);

    if (!reg_invoke_autoaddr_okay())
        printf("\nAny interface listed with OK? value \"NO\" does not "
	       "have a valid configuration\n");

    printf("\n%22s %15s %3s %6s %21s %8s", "Interface", "IP-Address",
	"OK?", "Method", "Status", "Protocol");

    FOR_ALL_SWIDBS(idb) {
	if (target && (target != idb))
	    continue;

	if (idb->hwptr->status & IDB_DELETED)
	    continue;

	if (idb->ip_address || idb->ip_unnumbered)
	    printf("\n%22s %15i ", idb->namestring,
		   idb->ip_address ?
		   idb->ip_address : idb->ip_unnumbered->ip_address);
	else
	    printf("\n%22s %15s ", idb->namestring, "unassigned");
	
	switch (idb->resolvemethod) {
	    case RES_NULL:
		resolve = "unset";
		break;
	    case RES_MANUAL:
		resolve = "manual";
		break;
	    case RES_NONVOL:
		resolve = "NVRAM";
		break;
	    case RES_CONFIG:
		resolve = "TFTP";
		break;
	    case RES_RARP:
		if (idb->hwptr->status & IDB_IEEEMASK)
		    resolve = "RARP";
		else
		    resolve = "SLARP";
		break;
	    case RES_BOOTP:
		resolve = "BOOTP";
		break;
	    default:
		resolve = "other";
		break;
	}

	printf("%3s %6s %21s %8s",
	       idb->ip_enabled && !idb->ip_address && !idb->ip_unnumbered ?
		   "NO" : "YES",
	       resolve, print_idbstate(idb), 
	       interface_up(idb) ? "up" : "down");
    }
    automore_disable();
}

/*
 * display_ip
 * Show the IP configuration of a single network interface
 */

static void display_ip (idbtype *idb)
{
    ipaddtype *ptr;
    hwidbtype *hwidb;
    int i;
    ipinfotype *ipinfo;

    if (!idb)
	return;
    hwidb = idb->hwptr;
    if (!hwidb)
	return;
    ipinfo = idb->ip_info;
    if (!ipinfo)
	return;
    printf(if_is_line_proto_is_msg, idb->namestring,
          print_idbstate(idb), interface_up(idb) ? "up" : "down");
    if (!idb->ip_enabled) {
	printf("\n  Internet protocol processing disabled");
	return;
    }
    if (idb->ip_address)
	printf("\n  Internet address is %i%m",
	  idb->ip_address, idb->ip_nets_mask);
    if (idb->ip_unnumbered)
	printf("\n  Interface is unnumbered.  Using address of %s (%i)",
		idb->ip_unnumbered->namestring,
		idb->ip_unnumbered->ip_address);
    printf("\n  Broadcast address is %i", idb->ip_broadcast);
    if (idb->ip_address) {
	printf("\n  Address determined by %s", ip_resolvestr(idb));
	if (idb->ip_resolvehost)
	    printf(" from host %i", idb->ip_resolvehost);
    }
    if (is_p2p(idb) && idb->ip_peer) {
        if (idb->ip_peer->ippeer_addr) {
	    printf("\n  Peer address is %i", idb->ip_peer->ippeer_addr);
	}
    }
    printf("\n  MTU is %d bytes", idb->ip_mtu);
    printf("\n  Helper address");
    if (idb->ip_helpernetQ.qhead == NULL)
	printf(" is not set");
    else {
	if (idb->ip_helpernetQ.qhead == idb->ip_helpernetQ.qtail) {
	    printf(" is ");
	} else {
	    printf("es are ");
	}
	for (ptr = (ipaddtype *) idb->ip_helpernetQ.qhead; ptr;
	                         ptr = ptr->next) {
	    if (ptr != (ipaddtype *) idb->ip_helpernetQ.qhead) {
		printf("\n                       ");
	    }
	    printf("%i", ptr->address);
	}
    }
    printf("\n  Directed broadcast forwarding is %sabled",
	   idb->ip_directbcast ? "en" : "dis");
    if (IPBROADCASTLIST_CONFIGURED(idb))
	printf(" - but restricted by access list %d", idb->ip_directbcast);
    for (ptr = (ipaddtype *) idb->ip_addressQ.qhead; ptr; ptr = ptr->next)
	printf("\n  Secondary address %i%m",
		ptr->address, ptr->mask);
    if (idb->ip_multiaddrQ.qhead) {
	i = 2;
	printf("\n  Multicast reserved groups joined:");
	for (ptr = (ipaddtype *) idb->ip_multiaddrQ.qhead;
	     ptr; ptr = ptr->next) {
	    if (!IPMULTICAST_RSVD(ptr->address))
		continue;
	    printf("%s%i", i++ % 6 ? " " : "\n      ", ptr->address);
	}
    }
    printf("\n  Outgoing access list is ");
    access_print_name(idb->ip_output_accesslist);
    if (idb->ip_output_accesslist_def != idb->ip_output_accesslist) {
	printf(", default is ");
	access_print_name(idb->ip_output_accesslist_def);
    }
    printf("\n  Inbound  access list is ");
    access_print_name(idb->ip_input_accesslist);
    if (idb->ip_input_accesslist_def != idb->ip_input_accesslist) {
	printf(", default is ");
	access_print_name(idb->ip_input_accesslist_def);
    }
    printf("\n  Proxy ARP is %sabled",
	   idb->ip_proxy_disabled ? "dis" : "en");
    print_security (idb);
    printf("\n  Split horizon is %sabled", idb->ip_split_disabled ? 
	   "dis" : "en");
    printf("\n  ICMP redirects are %s sent",
		    	idb->ip_redirect ? "always" : "never");
    printf("\n  ICMP unreachables are %s sent",
		    	idb->ip_unreach ? "always" : "never");
    printf("\n  ICMP mask replies are %s sent",
		    	idb->ip_maskreply ? "always" : "never");
    printf("\n  IP %s switching is %sabled",
	                (hwidb->ip_routecache & IP_CBUS_ROUTECACHE) ?
	                 "autonomous" : "fast",
		    	hwidb->ip_fastokay ? "en" : "dis");
    printf("\n  IP %s switching on the same interface is %sabled",
	   (hwidb->ip_routecache & IP_CBUS_ROUTECACHE) ?
	   "autonomous" : "fast",
	   (hwidb->ip_fastokay && idb->ip_routecache_same) ? "en" : "dis");

    if (registry_populated(REG_SSE) &&
	reg_invoke_sse_present()) {
	printf("\n  IP SSE switching is %sabled",
	       ((hwidb->ip_routecache & IP_SSE_ROUTECACHE) ||
		IS_SSE_SWITCHED_VLAN(idb)) &&
	       hwidb->ip_fastokay ? "en" : "dis");
    }

    if (reg_invoke_ip_turbo_idb_capable(hwidb)) {
	printf("\n  IP Optimum switching is %sabled",
	       (hwidb->ip_routecache & IP_OPTIMUM_ROUTECACHE) &&
	       hwidb->ip_fastokay ? "en" : "dis");
	
	printf("\n  IP Flow switching is %sabled",
	       (hwidb->ip_routecache & IP_FLOW_ROUTECACHE) &&
	       hwidb->ip_fastokay ? "en" : "dis");
    }
    if (IS_VIP(hwidb)) {
	printf("\n  IP Distributed switching is %sabled",
	       (hwidb->ip_routecache & IP_DISTRIBUTED_ROUTECACHE) &&
	       hwidb->ip_fastokay ? "en" : "dis");
    }
    
    printf("\n  IP multicast fast switching is %sabled", 
	   (idb->ip_multicast_fs) ? "en" : "dis");
    if (is_tokenring(hwidb))
	printf("\n  IP multicast token ring functional is %sabled",
	       idb->ip_multicast_use_functional ? "en" : "dis");
    printf("\n  Router Discovery is %sabled",
		    	idb->ip_irdp_enabled ? "en" : "dis");
    printf("\n  IP output packet accounting is %sabled",
		    	idb->ip_output_acctg ? "en" : "dis");
    printf("\n  IP access violation accounting is %sabled",
		    	idb->ip_violation_acctg ? "en" : "dis");
    if (idb->ip_output_acctg || idb->ip_violation_acctg)
	printf(", system threshold is %u", ip_acctg_info.threshold);
    printf("\n  TCP/IP header compression is ");
    if (hwidb->tcp_compress || idb->fr_tcp_hdr_comp) {
	if ((hwidb->tcp_compress &&
		(hwidb->tcp_compress->flags & THC_PERMACTIVE)) || 
		!(idb->fr_hdr_comp_passive))
	    printf("enabled");
	else
	    printf("passive");

	if (hwidb->tcp_compress)
	    printf(" and %scompressing",
		   (hwidb->tcp_compress->flags & THC_ACTIVE) ? "" : "not ");
    }
    else
	printf("disabled");
    printf("\n  Probe proxy name replies are %sabled",
	   idb->ip_probe_name ? "en" : "dis");
    printf("\n  Gateway Discovery is %sabled",
	   idb->ip_gdp_enabled ? "en" : "dis");
    printf("\n  Policy routing is %sabled",
	   ipinfo->policy_enabled ? "en" : "dis");
    if (ipinfo->policy_enabled && ipinfo->policy_routemap) {
	printf(", using route map %s", ipinfo->policy_routemap);
    }
    printf("\n  Network address translation is %sabled",
	   ipinfo->nat_enabled ? "en" : "dis");
    if (ipinfo->nat_enabled) {
	printf(", interface in domain %s",
	       ipinfo->nat_outside ? "outside" : "inside");
    }
}

/*
 * ip_show_interface
 * The "show ip interface" command.
 * If security is set up, summarize security first.
 */

void ip_show_interface (parseinfo *csb)
{
    idbtype *target, *idb;

    automore_enable(NULL);
    target = GETOBJ(idb,1);
    if (GETOBJ(int,1)) {
	ip_show_interface_brief(target);
	return;
    }
    if (!target && ip_secopt_bytes) {
	printf("\nSystem security is %s %s",
	       security_string(ip_secsys_low),
	       authority_string(&ip_secsys_auth_low));
	if ((ip_secsys_low != ip_secsys_high) ||
	     (ip_secsys_auth_low != ip_secsys_auth_high))
	  printf("\n                to %s %s",
		 security_string(ip_secsys_high),
		 authority_string(&ip_secsys_auth_high));
	FOR_ALL_SWIDBS(idb) {
	    printf("\n");
	    printf(if_is_line_proto_is_msg,
		idb->namestring,  print_idbstate(idb),
		interface_up(idb) ? "up" : "down");
	    if (!idb->ip_enabled) {
		printf("\n  Internet protocol processing disabled");
		continue;
	    }
	    print_security(idb);
	    automore_conditional(0);
	}
	automore_conditional(0);
    }
    show_swidb_chain(target, display_ip, 0);
    automore_disable();
}

static void ip_flow_cache_command (parseinfo *csb)
{
    if (reg_invoke_ip_flow_command_parse(csb))
	return;
    else if (!csb->nvgen)
	bad_parser_subcommand(csb, csb->which);
}

/*
 * ip_print_network_header
 *
 * Print IP header.
 */
void ip_print_network_header (paktype *pak, ushort **data)
{
    
    iphdrtype *ip;

    ip = (iphdrtype *) ipheadstart(pak);
    *data = (ushort *) ipdatastart(pak);
    if (0 == getfo(ip) &&
	(TCP_PROT == ip->prot || UDP_PROT == ip->prot)) {
	printf("\n  source: %i, destination: %i, id: 0x%04x, ttl: %d,"
	       "\n  TOS: %d prot: %d, source port %d, destination port %d",
	       GETLONG(&ip->srcadr), GETLONG(&ip->dstadr), ip->id, ip->ttl,
	       ip->tos, ip->prot, (*data)[0], (*data)[1]);
    } else {
	printf("\n  source: %i, destination: %i, id: 0x%04x, ttl: %d, prot: %d",
	       GETLONG(&ip->srcadr), GETLONG(&ip->dstadr), ip->id,
	       ip->ttl, ip->prot);
    }
}

/* 
 * The new parser function for udp broadcast parsing is now in
 * tcpip/udp.c, where the data structures it needs are defined.
 */

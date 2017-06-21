/* $Id: cdp_parse.c,v 3.9.8.10 1996/09/11 02:56:14 perryl Exp $
 * $Source: /release/112/cvs/Xsys/cdp/cdp_parse.c,v $
 *------------------------------------------------------------------
 * Support for CDP (Cisco Discovery Protocol)
 *
 * October 1994, Arun Sastry
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cdp_parse.c,v $
 * Revision 3.9.8.10  1996/09/11  02:56:14  perryl
 * CSCdi65039:  vtemplate loses cdp enable value after reload
 * Branch: California_branch
 * nvgen "cdp enable" for vtemplate if it is cdp enable.
 *
 * Revision 3.9.8.9  1996/09/04  16:46:32  dino
 * CSCdi67769:  Add CDP_REPEATER_DEVICE capability constant
 * Branch: California_branch
 *
 * Revision 3.9.8.8  1996/08/27  02:48:43  perryl
 * CSCdi62550:  CDP should be disabled on Vtemplate interfaces
 * Branch: California_branch
 * The default setting for virtual access and template interfaces
 * should be no cdp enable.
 *
 * Revision 3.9.8.7  1996/08/17  01:29:19  asastry
 * CSCdi63397:  CDP not respecting configurations on B-channels
 * Branch: California_branch
 *
 * Revision 3.9.8.6  1996/07/18  18:14:40  tylin
 * CSCdi62693:  Command cdp neighbors detail generates alignment
 * corrections (IPX)
 * Branch: California_branch
 *
 * Revision 3.9.8.5  1996/06/17  23:29:24  hampton
 * Implement the long sought parser "default" keyword.  [CSCdi60510]
 * Branch: California_branch
 *
 * Revision 3.9.8.4  1996/05/08  01:31:14  dawnli
 * CSCdi56020:  Spurious accesses in CDP
 * Branch: California_branch
 *
 * Revision 3.9.8.3  1996/04/30  21:29:55  dawnli
 * CSCdi53927:  Show cdp entry should accept wildcards
 * Branch: California_branch
 *
 * Revision 3.9.8.2  1996/04/15  21:13:11  bcole
 * CSCdi54649:  IP Host Modularity, IP Automatic Stub Routing feature
 * commit
 * Branch: California_branch
 *
 * Revision 3.9.8.1  1996/03/18  19:01:55  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.4  1996/03/16  06:29:40  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.2.26.3  1996/03/13  01:12:31  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.2.26.2  1996/03/07  08:39:29  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  00:20:18  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.9  1996/02/28  03:05:15  rbadri
 * CSCdi49394:  CDP can not be enabled on LANE interfaces
 * Enable CDP on ATM interfaces, when LANE is configured and
 * when the LANE client is operational.
 *
 * Revision 3.8  1996/02/13  08:10:58  dstine
 * CSCdi48797:  Extraneous definitions in interface_private.h
 *
 * Revision 3.7  1996/02/08  20:09:49  thille
 * CSCdi48347: format string in inlines, code size impact
 * Pass 1 - Clean up some literal strings in inlines, duplicate literals,
 * etc.  11,356 bytes saved on gs7-j-m, 9336 bytes saved on igs-j-l
 *
 * Revision 3.6  1996/02/07  16:10:24  widmer
 * CSCdi48285:  Remove function name from bad_parser_subcommand
 *
 * Revision 3.5  1996/02/05  03:33:55  mmcneali
 * CSCdi48150:  Cannot bridge between vLANs.
 *
 * Revision 3.4  1996/02/01  06:00:23  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.3  1996/01/29  07:26:38  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.2  1995/11/17  08:49:52  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:05:18  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.8  1995/11/08  20:51:39  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.7  1995/08/09  00:18:44  kramling
 * CSCdi38439:  FEIP product is now supported in 11.0.  Update support to
 * 10.3.
 *
 * Revision 2.6  1995/08/03  20:58:13  asastry
 * CSCdi38201:  CDP should use larger output buffer to display NSAPs.
 *
 * Revision 2.5  1995/07/17  23:16:05  hampton
 * Miscellaneous timer cleanups for CDP.  [CSCdi37215]
 *
 * Revision 2.4  1995/06/29 01:21:50  asastry
 * CSCdi36442:  <cdp enable> on interface should not override <no cdp run>
 *
 * Revision 2.3  1995/06/28  20:47:15  asastry
 * CSCdi36099:  CDP forces hardware reset for every defined subinterface
 *
 * Revision 2.2  1995/06/20  07:39:38  agt
 * CSCdi35820:  no cdp run is over-ridden when a new interface is
 * configured
 *
 * Revision 2.1  1995/06/07  20:15:09  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <ctype.h>
#include <string.h>
#include <ciscolib.h>
#include "sys_registry.h"
#include "interface_private.h"
#include "ieee.h"
#include "config.h"
#include "address.h"
#include "parser.h"
#include "../if/network.h"
#include "cdp_externs.h"
#include "parser_defs_cdp.h"
#include "../ui/debug.h"
#include "cdp_debug.h"
#include "../clns/clns_externs.h"
#include "../if/atm.h"
#include "../lane/lane.h"
#include "../wan/dialer.h"
#include "../ui/common_strings.h"

void cdp_command (parseinfo *csb)
{
    if (csb->nvgen) {
	switch (csb->which) {
	    case CDP_RUNNING:
	        nv_write (!cdp_running, "no %s", csb->nv_command);
		break;
	    case CDP_CONFIG_TIMER:
	        nv_write (cdp_xmit_frequency != CDP_DEF_XMIT_INTERVAL,
			  "%s %d", csb->nv_command, cdp_xmit_frequency);
		break;
	    case CDP_CONFIG_HOLDTIME:
	        nv_write (cdp_holdtime != CDP_DEF_TTL,
			  "%s %d", csb->nv_command, cdp_holdtime);
		break;
	    default:
	        bad_parser_subcommand(csb, csb->which);
	        break;
	}

	return;
    }

    switch (csb->which) {
        case CDP_RUNNING:
	    if (csb->set_to_default || csb->sense) {
		if (!cdp_running) {
		    cdp_start_process();
		}
	    }
	    else {
		cdp_running = FALSE;
		cdp_explicitly_disarmed = TRUE;

		/*
		 * "Unregister" CDP notification functions
		 */
		reg_delete_will_reload(cdp_reload_notify);
		reg_delete_swif_goingdown(cdp_if_down_notify);
		reg_delete_swif_comingup(cdp_if_up_notify);
	    }
	    break;
	case CDP_CONFIG_TIMER:
	    if (csb->sense) {
	        cdp_xmit_frequency = GETOBJ(int,1);
	    }
	    else {
		cdp_xmit_frequency = CDP_DEF_XMIT_INTERVAL;
	    }
	    break;
	case CDP_CONFIG_HOLDTIME:
	    if (csb->sense) {
	        cdp_holdtime = GETOBJ(int,1);
	    }
	    else {
		cdp_holdtime = CDP_DEF_TTL;
	    }
	    break;
	default:
	    bad_parser_subcommand(csb, csb->which);
	    break;
    }
}

/*
 * cdp_intfc_command
 *
 * Handle interface commands for CDP
 */
void cdp_intfc_command (parseinfo *csb)
{
    idbtype *idb;

    idb = csb->interface;

    if (csb->nvgen) {
	switch (csb->which) {
	    case CDP_ENABLE:
	        /*
		 * Don't NVGEN this line for unsupported interfaces.
		 */
		if (cdp_supported_int(idb)) {
		    nv_write(idb->cdp_enabled && CDP_DEFAULT_DISABLE_INT(idb),
			     "%s", csb->nv_command);
		    nv_write(!idb->cdp_enabled && !CDP_DEFAULT_DISABLE_INT(idb),
			     "no %s", csb->nv_command);
		}
		break;
	    default:
	        bad_parser_subcommand(csb, csb->which);
	        break;
	}

	return;
    }

    switch (csb->which) {
        case CDP_ENABLE:
	    if (csb->set_to_default) {
		csb->sense = CDP_DEFAULT_ENABLE_INT(idb);
	    }
	    /*
	     * If the system is still in reading configuation state,
	     * set cdp_update_timer to indicate any '[no] cdp enable'
	     * configuration has been read in and return without
	     * actually doing anything.
	     */
	    if (!cdp_ever_start) {
		GET_TIMESTAMP(idb->cdp_update_timer);
		idb->cdp_enabled = csb->sense;
		break;
	    }

	    if (csb->sense) {
		cdp_intf_enable(idb, system_configured);
	    }
	    else {
		cdp_intf_disable(idb);
	    }
	    reg_invoke_interface_cfg_modified(idb->hwptr);
	    break;
	default:
	    bad_parser_subcommand(csb, csb->which);
	    break;
    }
}

/* 
 * cdp_intf_enable 
 * 
 * Enable CDP on interface. If CDP process is not running and CDP has
 * not been explicitly turned off (by the command 'no cdp run'), start
 * the process.
 */
void cdp_intf_enable (idbtype *idb, boolean manually)
{ 
    /*
     * Print error for unsupported interfaces.
     * CDP can run over ATM-LANE interfaces as it is similar to ethernet.
     */
    if (!cdp_supported_int(idb)) {
	if (manually) {
	    printf("\n%% CDP is not supported on this interface, or for this "
		   "encapsulation");
	}
	return;
    }

    /*
     * If a 'no cdp run' command is in effect (i.e. the flag
     * 'cdp_explicitly_disarmed' is TRUE, don't start up the
     * process. This will prevent the CDP process from getting started
     * by an (interface level) CDP enable command even when a
     * 'no cdp run' command was entered by the user.
     */

    if (cdp_explicitly_disarmed) {
	if (manually) {
	    printf("\n%% Cannot enable CDP on this interface, since CDP "
	           "is not running");
	}
	return;
    }

    if (!cdp_running) {
	cdp_start_process();
    }

    if (!idb->cdp_enabled) {
	idb->cdp_enabled = TRUE;
	GET_TIMESTAMP(idb->cdp_update_timer);
	if (idb->hwptr->afilter_reset)
	    (*idb->hwptr->afilter_reset)(idb->hwptr);
	else
	    cdp_macaddrs_update(idb);
	if (!is_atm(idb->hwptr))
	    (*idb->hwptr->reset)(idb->hwptr);
    }
}

/* 
 * cdp_intf_disable
 * 
 * Disable CDP on interface
 *
 */
void cdp_intf_disable (idbtype *idb)
{
    idbtype *temp_idb;
    boolean found;

    found = FALSE;
    if (idb->cdp_enabled) {
	idb->cdp_enabled = FALSE;
    }

    /*
     * Check if there are any other interfaces with CDP enabled. If there is
     * none, get rid of the CDP process.
     */
    FOR_ALL_SWIDBS(temp_idb) {
        if (temp_idb->cdp_enabled) {
	    found = TRUE;
	    break;
	}
    }

    /*
     * No interface running CDP was found - delete the CDP process
     */
    if (!found) {
	cdp_running = FALSE;
	cdp_evbuginf("\nCDP-EV: No more interfaces running CDP - "
		     "removing CDP process");
    }
}

void show_cdp (parseinfo *csb)
{

    idbtype *idb;

    if (!cdp_running) {
	printf("%% CDP is not enabled");
	automore_disable();
	return;
    }

    idb = GETOBJ(idb, 1);
    switch (csb->which) {

        case SHOW_CDP_GLOBAL:
	    automore_enable(NULL);
	    show_cdpglobal();
	    automore_disable();
	    break;

        case SHOW_CDP_COUNTERS:
	    automore_enable(NULL);
	    show_cdp_counters();
	    automore_disable();
	    break;

        case SHOW_CDP_HOST:
	    automore_enable(NULL);
	    /*
	     * GETOBJ(string,1) is the name of the entry to be displayed
	     * GETOBJ(int,1) is TRUE if user asked for protocol info only
	     * GETOBJ(int,2) is TRUE if user asked for version info only
	     */
	    show_cdp_host(GETOBJ(string,1), GETOBJ(int,1), GETOBJ(int,2));
	    automore_disable();
	    break;
	    
        case SHOW_CDP_INTERFACE:
	    automore_enable(NULL);
	    /*
	     * If no interface specified, display list of interfaces
	     * on which CDP has been enabled.
	     */
	    if (!idb) {
		FOR_ALL_SWIDBS(idb) {
		    display_cdp_int_info(idb);
		}
		return;
	    }
	    show_swidb_chain(idb, display_cdp_int_info, 0);
	    automore_disable();
	    break;

        case SHOW_CDP_NEIGHBORS:
	    automore_enable(NULL);
	    show_cdp_neighbors(GETOBJ(idb,1), GETOBJ(int,1));
	    automore_disable();
	    break;
    }
}

/*
 * display_cdp_int_info 
 *
 * Display CDP-relevant interface information.
 */

void display_cdp_int_info (idbtype *idb)
{
    if (idb->cdp_enabled) {
	if (!cdp_supported_int(idb)) {
	    printf("\nCDP enabled, but not supported for %s encapsulation",
		   encstring(idb->hwptr->enctype));
	    return;
	}

        printf(if_is_line_proto_is_msg,
	       idb->namestring, print_idbstate(idb), 
               interface_up(idb) ? "up" : "down");

	default_encaps_display(idb);

        printf("\n  Sending CDP packets every %d seconds", cdp_xmit_frequency);
	printf("\n  Holdtime is %d seconds", cdp_holdtime);
    }

    return;
}

/*
 * show_cdpglobal
 *
 * Display all global CDP information
 */
void show_cdpglobal (void)
{
    printf("\nGlobal CDP information:");
    printf("\n\tSending CDP packets every %d seconds", cdp_xmit_frequency);
    printf("\n\tSending a holdtime value of %d seconds", cdp_holdtime);
    return;
}


/*
 * show_cdp_counters
 *
 * Display all CDP counters
 */
void show_cdp_counters (void)
{
    printf("\nCDP counters :");
    printf("\n\tPackets output: %d, Input: %d", cdp_traffic[CDP_OUTPUT],
	   cdp_traffic[CDP_INPUT]);
    printf("\n\tHdr syntax: %d, Chksum error: %d, Encaps failed: %d",
	   cdp_traffic[CDP_HDRSYNTAX], cdp_traffic[CDP_CHECKSUM],
	   cdp_traffic[CDP_ENCAPSFAILED]);
    printf("\n\tNo memory: %d, ", cdp_traffic[CDP_NOMEM]);
    printf("Invalid packet: %d, ", cdp_traffic[CDP_BADPACKET]);
    printf("Fragmented: %d", cdp_traffic[CDP_FRAG]);
    return;
}

/*
 * show_cdp_host
 *
 * Display info on the neighbor entries with matching name string.
 * If 'req_protocol' flag is TRUE, display protocol info for that entry.
 * If 'req_version' flag is TRUE, display version info for that entry.
 * Note that we can have multiple entries for a particular neighbor entry,
 * and a wildcard is allowed to be specified at the end of name string,
 * so we need to walk the entire hash table.
 *
 * While finding matching entries, lock an entry to be displayed,
 * and link it via cdp_entry_link 'reverse', which will be reversed to 'list'.
 * So that the display order is the same as walking hash table.
 * For each entry linked by 'list', display the requested information,
 * then free it.
 * Between mem_lock() and free(), field 'node' of cdp_entry_link, which points
 * to a locked entry, is only dereferenced, not assigned to other value.
 */
void show_cdp_host (char *entry, boolean req_protocol, boolean req_version)
{
    cdp_cache_type *cache_entry;
    int index;
    cdp_entry_link *list, *reverse, *swap;
    int	cache_len, entry_len = 0;
    boolean	wildcard = FALSE, to_display;

    if (!entry)
        return;

    entry_len = strlen(entry);
    if (entry_len == 0)
        return;

    if (entry[entry_len - 1] == '*') {
        wildcard = TRUE;
        entry_len--;
    }

    list = reverse = NULL;

    index = 0;
    cache_entry = cdp_walk_hash(&index, NULL);
    for (; cache_entry; cache_entry = cdp_walk_hash(&index, cache_entry)) {
        /*
         * Find the matching entries
         */
        cache_len = cache_entry->entry_name_len;
        if (wildcard && (cache_len > entry_len)) {
            cache_len = entry_len;
        }
        if (strncmp(cache_entry->entry_name, entry, cache_len) == 0) {
            to_display = TRUE;
            /*
             * Since all matching entries will have the same 'version',
             * we need to list and show only one if user specified 'version'.
             */
            if (req_version) {
                if (cache_entry->version) {
                    /*
                     * find exact matching entry
                     */
                    for (swap = reverse; swap; swap = swap->next) {
                        if (strncmp(cache_entry->entry_name,
                                    swap->node->entry_name,
                                    cache_entry->entry_name_len) == 0) {
                            to_display = FALSE;
                            break;
                        }
                    }
                }
                else
                    to_display = FALSE;
            }

            if (to_display) {
                swap = malloc(sizeof(cdp_entry_link));
                if (!swap)
                    break;
		
                mem_lock(cache_entry);		/* free it after displaying */
                swap->next = reverse;
                swap->node = cache_entry;
                reverse = swap;
            }
        }
    }
    /*
     * Reverse link so that the list has the same order as
     * the hash table is walked through.
     */
    while (reverse) {
        swap = reverse->next;
        reverse->next = list;
        list = reverse;
        reverse = swap;
    }

    while (list) {
        cache_entry = list->node;
        /*
         * Display protocol info if user specified 'protocol'. 
         */
        if (req_protocol) {
            printf("\nProtocol information for %s :",
                   cache_entry->entry_name );
            cdp_print_addresses_info(cache_entry);
        }
        /*
         * Display version info if user specified 'version'.
         */
        if (req_version) {
            printf("\n\nVersion information for %s :\n  %s\n",
                   cache_entry->entry_name, cache_entry->version);
        }
        /*
         * If neither, display all information about this entry
         */
        if (!req_protocol && !req_version) {
            show_one_cdp_neighbor(cache_entry, TRUE);
        }
        free(cache_entry);
        swap = list->next;
        free(list);
        list = swap;
    }
}

/*
 * show_cdp_neighbors
 *
 * Display all CDP neighbors. If an interface is specified, display only
 * those neighbors that are off of that interface. If 'detail' is specified,
 * display more detailed information.
 */
void show_cdp_neighbors (idbtype *idb, boolean detail)
{
    char *str ={ "\nDevice ID        Local Intrfce     Holdtme    Capability  Platform  Port ID"};

    cdp_cache_type *entry, *temp_entry;
    int index;

    if (!detail) {
        printf("\nCapability Codes: R - Router, T - Trans Bridge, "
               "B - Source Route Bridge"
               "\n                  S - Switch, H - Host, I - IGMP, r - Repeater");
	automore_enable(str);
    }

    index = 0;
    entry = cdp_walk_hash(&index, NULL);
    
    while (entry) {
	mem_lock(entry);
	/*
	 * If interface is specified, display only neighbors on that interface
	 */
	if (idb) {
	    if (entry->idb == idb) {
		show_one_cdp_neighbor(entry, detail);
	    }
	}
	else
	    show_one_cdp_neighbor(entry, detail);

        temp_entry = cdp_walk_hash(&index, entry);
        free(entry);
        entry = temp_entry;
    }

    if (!detail) {
	automore_disable();
    }
}

/*
 * show_one_cdp_neighbor
 *
 * Display specific CDP neighbor information. If 'detail' is specified, display
 * more detailed information. The structure in 'entry' is locked before this
 * function is called.
 */
#define CDP_CISCO_STRING "cisco "

void show_one_cdp_neighbor (cdp_cache_type *entry, boolean detail)
{
    char *cisco_string, *temp;
    
    cisco_string = CDP_CISCO_STRING;

    if (!entry)
	return;

    if (!entry->entry_name)
	return;

    if (!detail) {
        printf("\n%20s", entry->entry_name);
	printf("%15s", cdp_ret_int_abbr(entry->idb->namestring));
	printf("%8d", TIME_LEFT_SLEEPING(entry->expiration_time)/ONESEC);
	cdp_print_capabilities_info(entry->capabilities, detail);

	/*
	 * Skip the "cisco " part of the platform info, if it exists. The
	 * full string will be printed in the 'show cdp ne detail' command
	 * display. For example, "cisco 3000" will appear as "3000" - this
	 * will help conserve precious column width!
	 */
	temp = strstr(entry->platform, cisco_string);
	if (temp) {
	    temp = entry->platform + strlen(cisco_string);
	    printf("%10s", temp);
	}
	else
            printf("%10s", entry->platform);
	
        printf("%s", cdp_ret_int_abbr(entry->port_id));

        return;
    }

    printf("\n-------------------------");
    printf("\nDevice ID: ");
    printf("%s", entry->entry_name);
    printf("\nEntry address(es): ");
    cdp_print_addresses_info(entry);
    printf("\nPlatform: %s", entry->platform);
    printf(",  Capabilities: ");
    cdp_print_capabilities_info(entry->capabilities, detail);
    printf("\nInterface: %s", entry->idb->namestring);
    printf(",  Port ID (outgoing port): %s", entry->port_id);
    printf("\nHoldtime : %d sec",
	   TIME_LEFT_SLEEPING(entry->expiration_time)/ONESEC);

    if (entry->version) {
        printf("\n\nVersion :\n%s", entry->version);
    }

    printf("\n");
    return;
}

/*
 * cdp_print_address_info
 *
 * Print all the device's addresses.
 */
void cdp_print_addresses_info (cdp_cache_type *entry)
{
    uchar i;
    cdp_addr_info *cdp_addr;
    cdp_addr_structure *address;
    addrtype address2, *addr = &address2;
    ulong num_addr;

    cdp_addr = (cdp_addr_info *)(entry->addr);
    if (!cdp_addr)
        return;
    num_addr = cdp_addr->num_addr;
    address  = (cdp_addr_structure *)(cdp_addr->data);

    i = 0;
    while (i < num_addr) {	
	addr->type   = address->type;
	addr->length = address->length;
	bcopy(address->address, &addr->addr, addr->length);

	switch(addr->type) {
	    case ADDR_IP:
	        cdp_print_ip(addr);
	        break;
	    case ADDR_CLNS:
	        cdp_print_clns(addr);
	        break;
	    case ADDR_DECNET:
	        cdp_print_decnet(addr);
	        break;
	    case ADDR_APPLE:
	        cdp_print_apple(addr);
	        break;
	    case ADDR_NOVELL:
	        cdp_print_novell(addr);
		break;
            case ADDR_VINES:
	        break;
	    case ADDR_APOLLO:
	        break;
	}
	i++;

	/*
	 * Advance 'addr' to the next address.
	 */
	address = (cdp_addr_structure *)((uchar *)address +
			CDP_ADDRSTRUCT_OVERHEAD + address->length);
    }
}

/*
 * cdp_print_ip
 *
 * Print IP addresses. The data in the cache entry has the format
 * shown in "struct addrtype_" in 'address.h'.
 *
 */
void cdp_print_ip (addrtype *addr)
{
    printf("\n  IP address: %i", addr->ip_addr);
    return;
}

/*
 * cdp_print_clns
 *
 * Print CLNS addresses. The data in the cache entry has the format
 * shown in "struct addrtype_" in 'address.h'.
 */
void cdp_print_clns (addrtype *addr)
{
    uchar *temp, addr_string[100];
    int i;

    if (RUNNING(LINK_CLNS)) {
	reg_invoke_sprintf_address_addr(ADDR_CLNS, addr_string, addr);
	printf("\n  CLNS address: %s", addr_string);
    }
    else {
	printf("\n  CLNS address: ");
	temp = &addr->clns_addr[1];
	for (i = 1; i < addr->length; i++) {
	    if ((i > 2) && !(i & 1))
	        printf(".");
	    printf("%02x", *temp++);
	}
    }
}

/*
 * cdp_print_novell
 *
 * Print Novell address.
 */
void cdp_print_novell (addrtype *addr)
{
    uchar addr_string[50];
  
    if (RUNNING(LINK_NOVELL)) {
	reg_invoke_sprintf_address_addr(ADDR_NOVELL, addr_string, addr);
	printf("\n  Novell address: %s", addr_string);
    }
    else {
	/*
	 * Novell address has a 4 byte net number, followed by 6 bytes of
	 * node information.
	 */
	printf("\n  Novell address: %x.%e", addr->nov_addr.net,
					    addr->nov_addr.host);
    }
}

/*
 * cdp_print_decnet
 *
 * Print DECnet addresses.
 */
void cdp_print_decnet (addrtype *addr)
{
    uchar addr_string[50];

    if (RUNNING(LINK_DECNET)) {
	reg_invoke_sprintf_address_addr(ADDR_DECNET, addr_string,
					(struct addrtype_ *)addr);
	printf("\n  DECnet address: %s", addr_string);
    }
    else {
	printf("\n  DECnet address: %d.%d", (addr->decnet_addr >> 10),
					    (addr->decnet_addr & 0x3ff));
    }
}

/*
 * cdp_print_apple
 *
 * Print Appletalk addresses.
 */
void cdp_print_apple (addrtype *addr)
{
    uchar addr_string[50];
  
    if (RUNNING(LINK_APPLETALK)) {
	reg_invoke_sprintf_address_addr(ADDR_APPLE, addr_string, addr);
	printf("\n  Appletalk address: %s", addr_string);
    } else {
	printf("\n  Appletalk address: %d.%d", addr->apple_addr.net,
					       addr->apple_addr.host);
    }
}

/*
 * cdp_print_capabilities_info
 *
 * Print info on device's capabilities (i.e. router, SRT bridge, SRB bridge).
 * If 'detail' is FALSE, print only the abbreviated form, so that it can be
 * displayed by the 'show cdp ne' command.
 * 
 */
void cdp_print_capabilities_info (ulong value, boolean detail)
{
#define CDP_CAPAB_BUF_SIZE ((2*CDP_MAX_CAPABILITIES) + 1)
    
    char *temp, buf1[CDP_CAPAB_BUF_SIZE], buf2[CDP_CAPAB_BUF_SIZE];
    uchar i, len;

    temp = buf1;

    if (value & CDP_ROUTER_DEVICE) {
	if (detail)
	    printf("Router ");
	else {
	    sprintf(temp, "R ");
	    temp += 2;
        }
    }
    if (value & CDP_TB_DEVICE) {
	if (detail)
	    printf("Trans-Bridge ");
	else {
	    sprintf(temp, "T ");
	    temp += 2;
        }
    }
    if (value & CDP_SRB_DEVICE) {
	if (detail)
	    printf("Source-Route-Bridge ");
	else {
	    sprintf(temp, "B ");
	    temp += 2;
        }
    }
    if (value & CDP_SWITCH_DEVICE) {
	if (detail)
	    printf("Switch ");
	else {
	    sprintf(temp, "S ");
	    temp += 2;
        }
    }
    if (value & CDP_HOST_DEVICE) {
	if (detail)
	    printf("Host ");
	else {
	    sprintf(temp, "H ");
	    temp += 2;
        }
    }
    if (value & CDP_IGMP_DEVICE) {
	if (detail)
	    printf("IGMP ");
	else {
	    sprintf(temp, "I ");
	    temp += 2;
        }
    }

    if (value & CDP_REPEATER_DEVICE) {
	if (detail)
	    printf("Repeater ");
	else {
	    sprintf(temp, "r ");
	    temp += 2;
        }
    }

    if (!detail) {
	*temp = '\0';
    
	/*
	 * center this display and put formatted info in new buffer
	 */
	len = (CDP_CAPAB_BUF_SIZE - strlen(buf1))/2;
	temp = buf2;
	for (i = 0; i < len; i++, temp++)
	    sprintf(temp, " ");

	bcopy(buf1, temp, strlen(buf1));
	temp += (strlen(buf1));
    
	*temp = '\0';
	printf("%15s", buf2);    
    }

    return;
}

/*
 * cdp_ret_int_abbr
 *
 * Return pointer to abbreviated interface string. If it's a string we don't
 * know about, just return the original string. For example, "Ethernet1/0" will
 * be shortened to "Eth 1/0". This is used in the 'show cdp ne' display, where
 * column width is at a premium!
 */
#define CDP_ABBR_INT_NAME_LEN 4

char *cdp_ret_int_abbr (char *idb_name)
{
    char *temp, i;
    static char buf[50];
    
    /*
     * Skip till we see a number e.g. "Ethernet1/0"
     */
    i = 0;
    while (isalpha(idb_name[i]) && (i < strlen(idb_name)))
	i++;

    if (i == strlen(idb_name))
	return(idb_name);

    temp = &idb_name[i];
    /* 
     * 'temp' points to interface number. Copy this, as well as the
     * NULL terminator.
     */
    if (strstr(idb_name, "Serial")) {	
	bcopy("Ser ", buf, CDP_ABBR_INT_NAME_LEN);
	bcopy(temp, buf+CDP_ABBR_INT_NAME_LEN, strlen(temp)+1);
	return(buf);
    }
    else if (strstr(idb_name, "FastEther")) {
	bcopy("Fas ", buf, CDP_ABBR_INT_NAME_LEN);
	bcopy(temp, buf+CDP_ABBR_INT_NAME_LEN, strlen(temp)+1);
	return(buf);
    }
    else if (strstr(idb_name, "Ether")) {
	bcopy("Eth ", buf, CDP_ABBR_INT_NAME_LEN);
	bcopy(temp, buf+CDP_ABBR_INT_NAME_LEN, strlen(temp)+1);
	return(buf);
    }
    else if (strstr(idb_name, "Token")) {
	bcopy("Tok ", buf, CDP_ABBR_INT_NAME_LEN);
	bcopy(temp, buf+CDP_ABBR_INT_NAME_LEN, strlen(temp)+1);
	return(buf);
    }
    return(idb_name);
}

/*
 * cdp_clear_counters
 *
 * Clear all CDP counters
 */
void cdp_clear_counters (void)
{
    int i;

    for (i = 0; i < CDP_MAXTRAFFIC; i++)
	cdp_traffic[i] = 0;

    return;
}

/*
 * cdp_clear_table
 *
 * Clear CDP table
 */
void cdp_clear_table (void)
{
   int bucket;
   cdp_cache_type *entry, *next;

   for (bucket = 0; bucket < NETHASHLEN; bucket++) {
       for (entry = cdp_table[bucket]; entry; entry = next) {
	   next = entry->next;

	   cdp_adjbuginf("\nCDP-AD: Deleted table entry for %s, "
	                 "interface %s",
	                 entry->entry_name, entry->idb->namestring);

	   cdp_delete_cache_entry(entry);
       }
       cdp_table[bucket] = NULL;
   }
   
}

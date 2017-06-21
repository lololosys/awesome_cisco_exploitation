/* $Id: ipaddress.c,v 3.20.4.22 1996/09/05 23:59:36 rbadri Exp $
 * $Source: /release/112/cvs/Xsys/ip/ipaddress.c,v $
 *------------------------------------------------------------------
 * ipaddress.c -- IP address parsing and resolution routines
 *
 * June 1985, Kirk Lougheed
 *
 * Copyright (c) 1985-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ipaddress.c,v $
 * Revision 3.20.4.22  1996/09/05  23:59:36  rbadri
 * CSCdi64212:  martian table bogusly externed in a .c file.
 * Branch: California_branch
 * Use registry to adjust the martian table. Created a new boolean
 * to mark the validity of the entry.
 *
 * Revision 3.20.4.21  1996/08/30  20:12:53  sjackson
 * CSCdi63143:  ARP entry not removed after dial-in user hangs up
 * When user dials in flush cache of any existing entry
 * Branch: California_branch
 *
 * Revision 3.20.4.20  1996/08/28  12:49:45  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.20.4.19  1996/08/07  09:00:15  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.20.4.18  1996/08/06  08:27:24  gchristy
 * CSCdi64974:  IP Cache: delayed invalidation can create inconsistancies
 * Branch: California_branch
 *  - Back out changes made for CSCdi55725.
 *
 * Revision 3.20.4.17  1996/07/23  13:24:24  thille
 * CSCdi63329: Centralize logging for getbuffer() and pak_duplicate()
 * failures
 * Branch: California_branch
 * Shamelessly rip off Mr. Katz' code from CSCdi36037.  Provide
 * centralized logging for getbuffer() and pak_duplicate() failures and
 * then rip out the 9 bazillion different messages that where in the
 * various callers to report the failure.
 *
 * Revision 3.20.4.16  1996/07/18  04:00:32  dsavage
 * CSCdi61577:  network 192.0.0.0 not accepted by cisco ip code
 * Branch: California_branch
 * removed 192.0.0.x address from Martians table
 *
 * Revision 3.20.4.15  1996/06/28  23:17:39  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.20.4.14  1996/06/28  06:03:52  myeung
 * CSCdi61229:  Trying to turn on OSPF on IRB BVI interfaces crashes router
 * Branch: California_branch
 * - Make OSPF recognize BVI
 * - Improve OSPF interface detection so that it will not crash again
 *   when new type of virtual interface is added
 *
 * Revision 3.20.4.13  1996/06/24  22:53:16  perryl
 * CSCdi59423:  Reconfiguring virtual template causes configuration errors
 * Branch: California_branch
 *
 * Revision 3.20.4.12  1996/06/24  16:04:40  myeung
 * CSCdi55725:  OSPF CPU HOG
 * Branch: California_branch
 * - Modify IP cache invalidation code to allow delayed invalidation
 *
 * Revision 3.20.4.11  1996/06/17  23:32:11  hampton
 * Implement the long sought parser "default" keyword.  [CSCdi60510]
 * Branch: California_branch
 *
 * Revision 3.20.4.10  1996/05/23  07:37:22  myeung
 * CSCdi58029:  OSPF crash in ospf_interface_up() when unconfiguring OSPF
 * Branch: California_branch
 * - Add boolean parameter to ip_address_change registry to indicate if
 *   the change is for secondary or primary address.
 *
 * Revision 3.20.4.9  1996/05/10  01:33:01  wmay
 * CPP and ppp half-bridge commit
 * Branch: California_branch
 *
 * Revision 3.20.4.8  1996/04/29  20:43:15  jquan
 * CSCdi50033:  Crash after setup dialog intermitnently
 * Branch: California_branch. Add a new function
 * autoinstall_process_running.
 *
 * Revision 3.20.4.7  1996/04/24  05:56:07  jquan
 * CSCdi50033:  Crash after setup dialog intermitnently
 * Branch: California_branch. Remove the fix for now to solve the VIP
 * modularity problem.
 *
 * Revision 3.20.4.6  1996/04/23  01:11:42  jquan
 * CSCdi50033:  Crash after setup dialog intermitnently
 * Branch: California_branch. Add a new function
 * autoinstall_process_running() to determine whether the
 * autoinstall_over_serial is running or not.
 *
 * Revision 3.20.4.5  1996/04/15  21:16:12  bcole
 * CSCdi54649:  IP Host Modularity, IP Automatic Stub Routing feature
 * commit
 * Branch: California_branch
 *
 * Revision 3.20.4.4  1996/04/07  01:15:30  speakman
 * CSCdi48117:  ARPs for local non-bridge-group IP addresses dont get
 * bridged
 * Branch: California_branch
 * Modify the ARP code to bridge ARP's for any IP address not associated
 * with an interface in the same bridge group.  This permits CRB to be
 * used in a looped-back configuration to provide de facto IRB
 * functionality.
 *
 * Revision 3.20.4.3  1996/04/04  04:55:01  dino
 * CSCdi53046:  DR doesnt register when on shared-tree and spt-threshold
 *              configured
 * Branch: California_branch
 *
 * Revision 3.20.4.2  1996/04/02  05:36:29  dkatz
 * CSCdi36031:  Running out of memory running 2 or more EIGRP ASs
 * Branch: California_branch
 * EIGRP Jumbo Patch.
 *
 * Revision 3.20.4.1  1996/03/18  20:16:31  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.14.2.4  1996/03/16  06:58:13  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.14.2.3  1996/03/13  01:37:27  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.14.2.2  1996/03/07  09:46:12  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.14.2.1  1996/02/20  00:59:34  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.20  1996/03/09  22:55:43  widmer
 * CSCdi47890:  HTTP server doesnt work
 * Add HTTP server
 *
 * Revision 3.19  1996/02/27  04:37:34  mikel
 * CSCdi49954:  EIGRP: modulatiry violation
 *
 * Revision 3.18  1996/02/23  20:41:35  mikel
 * CSCdi48057:  EIGRP: No discovery of neighb on fddi when bridge-gr
 * defined
 * High-end FDDI interfaces were not adding the eigrp multicast to
 * the software multicast filter during a non-reset change.  Insure
 * the smf adds and deletes the eigrp multicast properly.
 *
 * Revision 3.17  1996/02/13  19:03:57  rja
 * CSCdi43596:  ARP throttling not working
 * Replumbed IPv4 ARP Request software so that the ARP throttling
 * sanity checks always occur and aren't accidentally bypassed by
 * some callers.  Removed one now dead function in ip_arp.c
 *
 * Revision 3.16  1996/02/04  08:41:55  billw
 * CSCdi48125:  find_path choses default interface very badly
 *
 * Revision 3.15  1996/01/24  20:15:03  jenny
 * CSCdi47046:  TCP/IP need code clean up (part 3)
 * . removed ip/tuba.c, ip/tuba_default.c and ip/tuba.h
 * . removed all includes for ip/tuba.h.
 * . removed more TUBA related code.
 *
 * Revision 3.14  1996/01/19  00:57:28  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.13  1996/01/11  19:51:17  pst
 * CSCdi46760:  IP address change callback called twice
 *
 * Revision 3.12  1996/01/11  02:26:42  jenny
 * CSCdi46513:  TCP/IP need code clean up (part 2)
 * . removed ip_tuba.c and clns_tuba.c.
 * . removed lots of tuba related registry stubs and code.
 *
 * Revision 3.11  1995/12/29  21:10:13  jenny
 * CSCdi46107:  Duplicate IP address not allowed on X.25, FR, SMDS &
 * ATM-DXI
 * CSCdi15734 and CSCdi43156 need to backed out. The change was based on
 * internal testing and an assumption that customers did not use duplicate
 * IP addresses. The fact is they *DO* use duplicate addresses in very
 * legitimate circumstances and we should allow them to continue to do
 * so.
 *
 * Revision 3.10  1995/12/28  18:27:37  jenny
 * CSCdi46011:  TCP/IP need to do code clean up.
 * This is the first patch of code changes for TCP/IP code cleanup.
 * . removed all references to TUBA_IP and TUBA_CLNS.
 * . renamed open_tuba_socket() to ip_open_socket().
 * . renamed tcp_tuba_open() to tcp_open().
 * . pass addrtype * instead of ipaddrtype for local and foreign addresses
 *   to tcp_open() for future portability.
 *
 * Revision 3.9  1995/12/28  05:59:15  kmoberg
 * CSCdi44169:  Frame Relay autoinstall crashes router with dlsw
 * bridge 1 config. Increase stack size for bootp_client. It should always
 * be the same size as init_process() because autoinstall runs config on
 * the bootp_client() stack.
 *
 * Revision 3.8  1995/12/08  00:54:25  irfan
 * CSCdi45224:  tacacs could use a specified source address
 * allow tacacs (TA, XTA, TA+) to use a specific (up) interface's IP
 * address.
 *
 * Revision 3.7  1995/11/26  08:15:44  mmcneali
 * CSCdi43630:  ISL VLAN Multicast changed--IOS gs7 images are DEAF
 *
 *         "no ip address" should not be generated on subinterfaces,
 *         creates backwards compatibility problems with earlier releases.
 *
 *         Explicitly indicate cache entries that are associated with a
 *         dot10 vLAN subinterface.
 *
 * Revision 3.6  1995/11/22  17:47:11  albright
 * CSCdi43791:  Helper addresses dont work over unnumbered interfaces
 *
 * Revision 3.5  1995/11/21  19:14:55  jenny
 * CSCdi43156:  Duplicate IP address not allowed on FR backup interface
 * Allow IP address for a configured backup interface to be on the same
 * net as one of the primary multipoint interface.
 *
 * Revision 3.4  1995/11/18  03:28:53  pst
 * CSCdi43467:  unumbered interfaces dont pick up changes based upon
 * numbered change
 *
 * Revision 3.3  1995/11/17  09:34:23  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:40:26  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:56:16  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.16  1995/11/08  21:04:32  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.15  1995/10/26  13:35:58  thille
 * CSCdi36960:  setup messages overwrite previous lines
 * Fix errors made by folks who apparently weren't aware that for some
 * reason we prepend \n on the front of lines of output instead of
 * appending it to the end.
 *
 * Revision 2.14  1995/10/24  06:55:11  tli
 * CSCdi42686:  IP: small code cleanup
 * Clean up a wee bit o' old time cruft.
 *
 * Revision 2.13  1995/10/20  18:05:55  albright
 * CSCdi42154:  ip helper-address fails on unnumbered interfaces
 *
 * Revision 2.12  1995/10/16  19:39:56  pst
 * CSCdi41543:  IP tunnel address not wholly erased
 *
 * Revision 2.11  1995/10/03  21:05:36  jenny
 * CSCdi15734:  Duplicate IP addresses allowed on X.25 and multipoint FR
 *
 * Revision 2.10  1995/09/19  18:37:12  smackie
 * Allow chunks to be created for a specified memory pool or with a
 * declared base alignment by allowing the data to be detached from
 * the chunk header and freelist. (CSCdi40637)
 *
 * Revision 2.9  1995/09/18  19:35:54  fox
 * CSCdi40523:  Add IP Address Pooling support for ISDN and other sync
 * interfaces
 * Modify IP address pooling to support all Point-to-Point interfaces
 * Remove Odd/Even Address feature
 * Add 'ppp negotiate' command
 *
 * Revision 2.8  1995/09/13  04:07:14  hampton
 * The Fall of the House of Msclock.  [CSCdi40182]
 *
 * Revision 2.7  1995/08/15 00:20:02  bcole
 * CSCdi38170:  Ping to HSRP IP address causes ICMP echo request storm
 *
 * Add notion of dynamic IP aliases, so that HSRP virtual IP address is
 * now pingable.  Eliminate packet forwarding problem for this virtual
 * IP address as a side effect.
 *
 * Revision 2.6  1995/08/09  00:20:30  kramling
 * CSCdi38439:  FEIP product is now supported in 11.0.  Update support to
 * 10.3.
 *
 * Revision 2.5  1995/08/03  07:42:07  hampton
 * EIGRP stops processing input.  Convert the IP socket input queue into a
 * watched_queue.  This allows the scheduler to see if there is input
 * waiting when the process dismisses.  [CSCdi38156]
 *
 * Revision 2.4  1995/06/21 03:28:56  gstovall
 * CSCdi35731:  Cleanup unused cruft in idb
 * Nuke some unused vectors and variables from the hwidb.
 *
 * Revision 2.3  1995/06/16  22:33:44  shankar
 * CSCdi35485:  autoinstall over Frame Relay breaks in 103-3.3
 *
 * Revision 2.2  1995/06/09  13:05:32  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 20:58:27  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <string.h>
#include <ciscolib.h>
#include "interface_private.h"
#include "packet.h"
#include "config_register.h"
#include "sys_registry.h"
#include "media_registry.h"
#include "ip_registry.h"
#include "../iprouting/route.h"
#include "ttysrv.h"
#include "config.h"
#include "file.h"
#include "address.h"
#include "name.h"
#include "ip.h"
#include "ip_media.h"
#include "../if/network.h"
#include "../if/ether.h"
#include "../srt/srt_registry.h"
#include "../wan/serial.h"
#include "../wan/dialer.h"
#include "../os/boot.h"
#include "logger.h"
#include "ipaddress.h"
#include "../iprouting/gdp.h"
#include "../servers/bootp.h"
#include "../ui/setup.h"
#include "../os/chunk.h"
#include "../os/hostname.h"
#include "../h/parser.h"
#include "ip_addrpool.h"
#include "../smf/smf_public.h"
#include "../if/msg_datalink.c"		/* Not a typo, see logger.h */
#include "../os/msg_system.c"		/* Not a typo, see logger.h */
#include "msg_ip.c"			/* Not a typo, see logger.h */
#include "../os/nv.h"

#include "../vlan/vlan.h"


static int autoinstall_pid = 0;
static parseinfo *csb_ptr = NULL;

static void autoinstall_terminate(void);
static void autoinstall_reset_serial_encap(void);

/*
 * Local Storage
 */
ipaddrtype default_gateway;		/* IP address of default gateway */
ipaddr_tabletype *ipaddr_hash_table[NETHASHLEN];
chunk_type *ipaddr_blocks;
int ip_background_pid;

/*
 * ipaddr_table_init
 * Initialize the hash table of IP addresses
 * These are interface addresses as well as aliases.
 */

void ipaddr_table_init (void)
{
    /*
     * Make sure main hash table is zeroed.
     * We use the chunk manager to allocate these blocks.
     */
    memset(ipaddr_hash_table, 0, sizeof(ipaddr_hash_table));
    ipaddr_blocks = chunk_create(sizeof(ipaddr_tabletype),IPADDR_TABLE_ENTRIES,
				 CHUNK_FLAGS_DYNAMIC,
				 NULL, 0,
				 "IP Addresses");
}

/*
 * ipaddr_table_insert
 * Add an entry to the hash table
 * Returns FALSE on failure
 */

boolean ipaddr_table_insert (ipaddrtype address, idbtype *swidb,
			     enum ipaddr_type type, ushort flags, ulong port,
			     void *data)
{
    ipaddr_tabletype *addr;
    int bucket;
    
    /*
     * Check for entry already in table.  If it exists, update with
     * current information and return.
     */
    bucket = nethash(address);
    for (addr = ipaddr_hash_table[bucket]; addr; addr = addr->next) {
	if ((addr->address == address) && (addr->swidb == swidb)) {
	    addr->type = type;
	    addr->port = port;
	    addr->addrflags = flags;
	    addr->data = data;
	    return(TRUE);
	}
    }

    /*
     * Not in table.  Add a new entry
     */
    addr = chunk_malloc(ipaddr_blocks);
    if (!addr)
	return(FALSE);
    addr->address = address;
    addr->swidb = swidb;
    addr->port = port;
    addr->type = type;
    addr->addrflags = flags;
    addr->data = data;
    addr->next = ipaddr_hash_table[bucket];
    ipaddr_hash_table[bucket] = addr;
    return(TRUE);
}

/*
 * ipaddr_table_delete
 * Remove an entry from the hash table.
 * Returns FALSE if entry wasn't found.
 * Used by alias support; see ipaddr_table_rebuild() for interfaces.
 */

boolean ipaddr_table_delete (ipaddrtype address, idbtype *swidb)
{
    ipaddr_tabletype *addr, *pred;
    int bucket;
    
    /*
     * Check for entry already in table.  If it exists, update with
     * current information and return.
     */
    bucket = nethash(address);
    pred = NULL;
    for (addr = ipaddr_hash_table[bucket]; addr; addr = addr->next) {
	if ((addr->address == address) && (addr->swidb == swidb)) {
	    if (!pred)
		ipaddr_hash_table[bucket] = addr->next;
	    else
		pred->next = addr->next;
	    chunk_free(ipaddr_blocks, addr);
	    return(TRUE);
	}
	pred = addr;
    }
    return(FALSE);
}

/*
 * ipaddr_table_rebuild
 * Rebuild the address table entries corresponding to an interface.
 */

void ipaddr_table_rebuild (idbtype *swidb)
{
    ipaddr_tabletype *addr, *pred;
    ipaddtype *ptr;
    int bucket;

    /*
     * Scan through entire table removing IP address entries
     * corresponding to this software interface.
     */
    for (bucket = 0; bucket < NETHASHLEN; bucket++) {
	addr = ipaddr_hash_table[bucket];
	pred = (ipaddr_tabletype *) &ipaddr_hash_table[bucket];
	while (addr) {
	    if ((addr->swidb == swidb) && (addr->type == IPADDR_INTERFACE)) {
		pred->next = addr->next;      /* link out current entry */
		chunk_free(ipaddr_blocks, addr);  /* flush entry */
		addr = pred;		      /* adjust addr to step to next */
	    }
	    pred = addr;		      /* remember predecessor */
	    addr = addr->next;
	}
    }

    /*
     * Insert primary and secondary addresses
     */
    if (!swidb->ip_enabled || swidb->ip_unnumbered)
	return;
    ipaddr_table_insert(swidb->ip_address, swidb, IPADDR_INTERFACE, 0, 0,
			NULL);
    for (ptr = (ipaddtype *) swidb->ip_addressQ.qhead; ptr; ptr = ptr->next)
	ipaddr_table_insert(ptr->address, swidb, IPADDR_INTERFACE, 0, 0, NULL);
}

/*
 * ip_address_change
 *
 * If an address on a numbered interface changes, we may have to do fixups
 * on all of the unnumbered interfaces pointing at it.
 *
 * This callback is always done *after* the notified change has been made;
 * if adding an address, it will be found in the IDB, and if removing one
 * it will have been deleted by now.  The only exception is when an
 * interface is deconfigured as unnumbered;  the unnumbered pointer will
 * still be present (since we're not passing in the address).
 *
 * This callback will normally be called with a valid address/mask pair.
 *
 * It is called with a 0/0 address/mask pair when an interface is configured
 * (or deconfigured) as unnumbered.  It is also called with 0/0 when all
 * addresses are deleted from the interface (after each address is deleted
 * individually).
 *
 * The boolean secondary indicate whether addr/mask is secondary address.
 */

void
ip_address_change (idbtype *target_idb,
		   ipaddrtype addr, ipaddrtype mask, boolean secondary,
		   boolean sense)
{
    idbtype *idb;

    reg_invoke_ip_address_change(target_idb, addr, mask, secondary, sense);

    FOR_ALL_SWIDBS(idb) {
	if (idb->ip_unnumbered == target_idb)
	    reg_invoke_ip_address_change(idb, addr, mask, secondary, sense);
    }
    reg_invoke_route_adjust_msg(target_idb);
    reg_invoke_route_adjust(target_idb);
}


/*
 * ip_setaddresses
 * Try very hard to resolve our interface addresses
 */

void ip_setaddresses (void)
{
    /*
     * Fork off any other methods of IP address discovery
     * (All methods should probably be hung off of this registry service
     * eventually...)
     */
    reg_invoke_ip_autoaddr_load();
    /*
     * Now we try probing the interfaces to get our address(es).
     */
    if (address_probe(MAXTRIES,2))
	return;

    /*
     * No go.  Try booting a host specific configuration file.
     */
    if (address_boot())
	return; 

    attempt_autoinstall();

    /*
     * Booting a configuration file either failed or didn't help.
     * Now we invoke the interactive resolver and continue probing
     * Meanwhile, Autoinstall will be attempted in the background. 
     * If it fails to resolve an interface, the autoinstall process 
     * will try to resolve interfaces using the resolve_interface() 
     * routine. 
     */
    start_setup();			/* Start up setup mode */
    process_sleep_for(2*ONESEC);	/* let it run */
    (void) ipisresolved();		/* see if that fixed it */
    
    /* 
     * If we started the interactive resolver, we shouldn't return
     * until setup_running() indicates that it's no longer running.
     * If we did return while resolve_all_protocols() was running,
     * then there is window of opportunity for init_process() to call
     * setup_abort() while we have nvram open.  Then any future calls 
     * to nv_getptr() will fail (as in CSCdi30645).
     */
    while (TRUE) {
	(void) ipisresolved();
	if (netsareup && !setup_running())
	    return;
	process_sleep_for(10 * ONESEC);
    }
}

/*
 * address_probe - use RARP, etc. to discover addresses and masks
 * Returns TRUE if all addresses resolved, FALSE otherwise.
 */
boolean address_probe (int count,	/* probe iterations */
		       ulong seconds)	/* seconds to pause after probing */
{
    int i;
    idbtype *idb;

    seconds *= ONESEC;
    for (i = 0; i < count; i++) {
	for (idb = SWIDBQ_HEAD; idb && !netsareup; idb = IDB_NEXT(idb)) {
	    hwidbtype *hwidb = idb->hwptr;
	    if (!idb->ip_enabled)
		continue;
	    if (!idb->ip_address && !idb->ip_unnumbered) {
		if (hwidb->status & IDB_ETHER) {
		    /*
		     * ask RARP server
		     */
		    reg_invoke_ip_rarp_send_request(idb, idb);
		}
		if (((hwidb->status & IDB_SERIAL) 
				&& (!(hwidb->status & IDB_ISDN))) 
				&&(hwidb->enctype == ET_HDLC))
		    ip_slarp_sendrequest(idb);	/* ask other side of HDLC */
	    }
	}
	process_wait_on_netsareup(seconds);

	(void) ipisresolved();		/* make sure it's up to date */
	if (netsareup)			/* exit now if nets are up */
	    break;
    }	
    return(netsareup);			/* return current netsareup value */
}

/*
 * address_boot
 * Try booting our configuration file to discover interface addresses
 * Returns TRUE if all addresses known, FALSE otherwise.
 */

boolean address_boot (void)
{
    if (!usable_interfaces()) 		/* need at least one up interface */
	return(FALSE);

    if (netconfig_enable && !netconfg.responder)
        (void) configure(&netconfgQ, &netconfg, FALSE);

    if (whoami()) {			/* get our authoritative name */
	if (netconfig_enable && !configure(&hostconfgQ, &hostconfg, FALSE))
	    return(FALSE);		/* return FALSE if boot failed */
    }

    (void) ipisresolved();

    if (stdio->statbits & CTRLC_ENBL) {
	stdio->statbits |= CONTROLC;	/* get out of setup if still in it */
	if (hostconfg.filename && hostconfg.ps.tuba.respondent.ip_addr) {
	    printf("\n\nNetbooted %s from %i, aborting setup.\n"
		, hostconfg.filename, hostconfg.ps.tuba.respondent.ip_addr);
	    errmsg(&msgsym(OUTSETUP, SYS), hostconfg.filename);
	}
    }
    return(netsareup);			/* TRUE or FALSE */
}

/*
 * ipisresolved
 * Make a quick pass over all interfaces to see if we've
 * resolved all Internet interfaces.
 */

boolean ipisresolved (void)
{
    idbtype *idb;

    FOR_ALL_SWIDBS(idb)
	if (idb->ip_enabled && !idb->ip_address && !idb->ip_unnumbered) 
	    return(FALSE);

    /*
     * All interfaces are up.  If this is the first time, then get more
     * routes. 
     */
    if (!netsareup) {
	netsareup = TRUE;
	process_set_boolean(watched_netsareup, TRUE);
	reg_invoke_clear_routes();
    }
    return(TRUE);
}

/* 
 * class_resolve
 * Given an idb with an IP address, determine subnet parameters.
 *
 * This routine is responsible for several things.  Primary input
 * consists of the ipaddr (idb->ip_address) and potentially a
 * subnet mask.  (idb->ip_nets_mask).  If the interface doesn't
 * have a subnet mask this later field will be zero.
 *
 * class_resolve is responsible for finalizing address and
 * mask cells in the idb structure when an interface first
 * gets its address or when it is changed.  
 */

void class_resolve (register idbtype *idb)
{
    ipaddrtype address;
    ipaddrtype majornet;
    
    /*
     * Make sure address and mask are rational.
     * If they aren't, clear address so resolution continues.
     */
    address = idb->ip_address;
    if (!good_address(address,idb->ip_nets_mask,
	 		(idb->ip_nets_mask ? TRUE : FALSE)))
   	if ((idb->ip_nets_mask != -1L) || (idb->hwptr->type != IDBTYPE_LB)) {
	    errmsg(&msgsym(CLASS, IP), address, idb->ip_nets_mask);
	    idb->ip_address = 0L;
	    return;
	}

    /*
     * If no subnet mask set, discover it from prior information
     * or take a reasonable guess.
     */
    if (!idb->ip_nets_mask) {
	/*
	 * We don't have a subnet mask.  Go find one.
	 */
	reg_invoke_ip_get_netsmask(idb->ip_address, &idb->ip_nets_mask);
    } else {
	/*
	 * There is an incoming subnet mask.  We will use it to force
	 * all interfaces on the same major net to the same value.
	 */
	majornet = get_majornet(address);
    }

    /*
     * Set up masks, initialize ARP cache, routing cache information.
     */
    idb->ip_majornet = get_majornet(idb->ip_address);
    idb->ip_majormask = get_majormask(idb->ip_address);
    ip_arp_add_interface(idb, idb->ip_address);
    reg_invoke_ipcache_setup_interface(idb->hwptr);

    /*
     * Figure out our preferred broadcast address.
     */
    if (configregister & CFG_ZEROS) {
	idb->ip_broadcast = 0L;		/* we want all zeros */
	if (configregister & CFG_SUBNET)/* maybe wants subnetting */
	    idb->ip_broadcast = (idb->ip_address & idb->ip_nets_mask);
    } else {
	idb->ip_broadcast = 0xFFFFFFFF;	/* we want all ones */
	if (configregister & CFG_SUBNET)/* maybe wants subnetting */
	    idb->ip_broadcast = (idb->ip_address | ~idb->ip_nets_mask);
    }

    /*
     * Make sure address lookup table is updated
     */
    ipaddr_table_rebuild(idb);
}

/*
 * mask_contiguous
 * Return TRUE if the given mask is left contiguous, FALSE otherwise.
 */

boolean mask_contiguous (ipaddrtype mask)
{
    while ((mask & 0xff000000) == 0xff000000)
	mask <<= 8;
    while (mask & 0x80000000)
        mask <<= 1;
    if (mask) 
        return(FALSE);
    return(TRUE);
}

/*
 * ip_bytesinmask
 * Given a netmask, return the number of bytes required to store the 
 * most significant bits of the ip address.
 */
int ip_bytesinmask (ipaddrtype netmask)
{
    /*
     * Do binary search.
     */
    if (netmask & 0xffff) {
        if (netmask & 0xff)
            return(4);
        return(3);
    } else {
        if (netmask & 0xff0000)
            return(2);
        return(1);
    }
}

/*
 * ip_bitcount2mask
 * Given a bit count, return an IP network mask left justified with 1's.
 */
ipaddrtype ip_bitcount2mask (int bit_count)
{
    return(0xffffffff << (32 - bit_count));
}

/*
 * good_address
 * Validate an address and/or network mask, return TRUE/FALSE.
 * Checks:
 *  - an address or mask is never all zeros or all ones
 *  - an address must not be a reserved IP address, i.e., a martian
 *  - the mask may not have zeros in the major network portion, that is,
 *    it must be consistent with the address class type.
 *  - the mask must be left contiguous
 */

boolean good_address (ipaddrtype address, ipaddrtype mask, boolean maskcheck)
{
    ipaddrtype basicmask, result;

    if (martian(address, TRUE))
	return(FALSE);
    if (maskcheck) {
	if ((mask == 0L) || (mask == -1L))
	    return(FALSE);
	if (!mask_contiguous(mask))
	    return(FALSE);
	basicmask = get_majormask(address);
	if (!basicmask)
	    return(FALSE);
	if ((mask | address) == -1)
	    return(FALSE);		/* all ones in host part */
	if ((address & ~mask) == 0)
	    return(FALSE);		/* all zeros in host part */
	if (!subnetzero_enable && (mask > basicmask)) {
	    result = (address & ~basicmask);	/* mask off major net bits */
	    if ((result & mask) == 0L)	/* mask off host bits */
		return(FALSE);		/* disallow subnet zero */
	}
    }
    return(TRUE);
}

/*
 * good_route_mask
 * Validate a network mask for use as a route, return TRUE/FALSE.
 * Checks:
 *  - a mask is not a supernet mask
 *  - the mask must be left contiguous
 */

boolean good_route_mask (ipaddrtype address, ipaddrtype mask, 
			 boolean subnetzero_allowed)
{
    ipaddrtype basicmask, result;

    if (!mask_contiguous(mask))
	return(FALSE);
    if (address & ~mask)
	return(FALSE);
    basicmask = get_majormask(address);
    /* check for subnet zero routes does not apply to supernets */
    if (!subnetzero_allowed && (mask > basicmask) && address) {
	result = (address & ~basicmask);	/* mask off major net bits */
	if ((result & mask) == 0L)	/* mask off host bits */
	    return(FALSE);		/* disallow subnet zero */
    }
    return(TRUE);
}

/*
 * get_majornet
 * Given an address, return the major net field
 */

ipaddrtype get_majornet (register ipaddrtype address)
{
    if (!(address & CLASS_A_BIT))
	return(address & CLASS_A_MASK);
    else if (!(address & CLASS_B_BIT))
	return(address & CLASS_B_MASK);
    else if (!(address & CLASS_C_BIT))
	return(address & CLASS_C_MASK);
    else
	return(0);
}

/*
 * get_majormask
 * Given an address, return the mask of major network number.
 */

ipaddrtype get_majormask (ipaddrtype address)
{
    if (!address)			/* net 0.0.0.0 */
	return(0);			/* mask 0.0.0.0 */
    if (!(address & CLASS_A_BIT))	/* Class A network */
	return(CLASS_A_MASK);
    else if (!(address & CLASS_B_BIT))	/* Class B network */
	return(CLASS_B_MASK);
    else if (!(address & CLASS_C_BIT))	/* Class C network */
	return(CLASS_C_MASK);
    else				/* unknown network class */
	return(MASK_HOST);
}

/*
 * ip_get_netsmask
 * Registry wrapper routine around get_majormask
 */

void ip_get_netsmask (ipaddrtype address, ipaddrtype *mask)
{
    *mask = get_majormask(address);
}

/*
 * parse_ip_address
 * Convert a string of the form A.B.C.D to an Internet address.
 * Return FALSE if parse fails, TRUE otherwise with "result" updated.
 */

boolean parse_ip_address (char *ptr, ipaddrtype *result)
{
    ulong address = 0;	         /* build address here */

    if (null(ptr))
	return(FALSE);
    ptr = remove_ws(ptr);	/* flush leading and trailing blanks */
    if ((ptr = octet(ptr,&address,'.')) &&
	(ptr = octet(ptr,&address,'.')) &&
	(ptr = octet(ptr,&address,'.')) &&
	(ptr = octet(ptr,&address,'\0'))) {
	*result = (ipaddrtype) address;
	return(TRUE);		/* a good IP address */
    } else
	return(FALSE);		/* an illegal address */
}

/*
 * parse_hostname
 * Convert a string of either A.B.C.D or a hostname into an Internet address.
 * Return FALSE if parse fails, TRUE otherwise with "result" updated.
 */

boolean parse_hostname (register char *ptr, ipaddrtype *result)
{
    int parsed, dummy;
    nametype *cp;

    parsed = parse_ip_address(ptr, result);
    if (!parsed) {
	cp = (nametype *)name_lookup(ptr, &dummy, NAM_IP);
	if (cp) {
	    *result = cp->v.ip.adr[0].addr.ip_address;
	    parsed = TRUE;
	}
    }
    return parsed;
}

boolean ip_parse_hostname (char *ptr, addrtype *addr)
{
    addr->type = ADDR_IP;
    return(parse_hostname(ptr, &addr->ip_addr));
}

/*
 * ip_address_duplicate
 * Verify that supplied address/mask pair is not the same as addresses
 * on other interfaces.  There are several exceptions noted below.
 *
 * This function should only be called during command processing.
 */

boolean ip_address_duplicate (ipaddrtype address,
			      ipaddrtype mask,
			      idbtype *ignore_idb,
			      boolean primary)
{

    idbtype    *idb;
    ipaddtype  *ptr;
    ipaddrtype addr, shortest;
    int dupcount;
    boolean dups_ok;

    /*
     * If bridging IP, duplicate addresses are OK. 
     */
    if (BRIDGING_IF(ignore_idb, LINK_IP))
	return (FALSE);
    addr = (address & mask);

    /*
     * Go through each interface checking primary address first, then
     * each secondary address for interface.
     */
    dupcount = 0;
    FOR_ALL_SWIDBS(idb) {
	hwidbtype *hwidb = idb->hwptr;
	hwidbtype *hwignore = hwidb_or_null(ignore_idb);

	/*
	 * Ignore interfaces where IP is disabled as well as the
         * interface that is changing.
	 */
        if ((idb == ignore_idb) || !idb->ip_enabled || (idb->ip_address == 0)
	    || (hwidb->state == IDBS_ADMINDOWN))
            continue;

	/*
	 * Duplicate addresses O.K. on serial interfaces.
	 * Also okay between an interface and its backup interfaces
	 * (which may or may not be serial).
	 * Also okay between simplex interface pairs.
	 * However, the number of these may be limited by the iprouting
	 * table.
	 */
	dups_ok = FALSE;
	if (((hwidb->status & IDB_SERIAL) && 
	     (hwignore->status & IDB_SERIAL)) ||
	    (hwignore->secondary && hwignore->secondary == hwidb) ||
	    (hwidb->secondary && hwidb->secondary == hwignore) ||
	    (ignore_idb->output_if && ignore_idb->output_if == idb) ||
	    (idb->output_if && idb->output_if == ignore_idb)) {
	    if ((addr == (idb->ip_address & idb->ip_nets_mask)) &&
		(mask == idb->ip_nets_mask)) {
		dupcount++;
		continue;
	    }
	    dups_ok = TRUE;
	}
	else if (!primary && (addr == (idb->ip_address & idb->ip_nets_mask))) {
	    printf("\n%i is assigned to %s", addr, idb->namestring);
	    return (TRUE);
	}
	shortest = SHORTEST_MASK(mask, idb->ip_nets_mask);
	if ((addr & shortest) == (idb->ip_address & shortest)) {
	    printf("\n%i overlaps with %s", addr, idb->namestring);
	    return(TRUE);
	}
	for (ptr = (ipaddtype *)idb->ip_addressQ.qhead; ptr; ptr = ptr->next) {
	    if (addr == (ptr->address & ptr->mask)) {
		if (dups_ok) {
		    dupcount++;
		    break;
		}
		printf("\n%i is assigned as a secondary address on %s", 
		       addr, idb->namestring);
		return (TRUE);
	    }
	    shortest = SHORTEST_MASK(mask, ptr->mask);
	    if ((addr & shortest) == (ptr->address & shortest)) {
	    	printf("\n%i overlaps with secondary address on %s", addr, 
		       idb->namestring);
	    	return(TRUE);
	    }
	}
        if (reg_invoke_ip_address_reserved(idb, address)) {
            printf("\n%i is assigned to other application on %s",
                   address, idb->namestring);
            return(TRUE);
        }
    }

    if (reg_invoke_iprouting_conn_plimit_exceed(dupcount)) {
	printf("\nToo many interfaces on subnet %i", address & mask);
	return(TRUE);
    }
    return (FALSE);
}

/*
 * show_ipsockets
 * Show IP socket data structure
 *
 * Warning: The formatting is screwed up if CLNS addressing is being used.
 *          The TUBA folks get to figure out how to fix this. -Kirk 
 *
 * WARNING: IPsocketQ should not be accessed directly as the print
  routine can suspend.
 */


void show_ipsockets (parseinfo *csb)
{
    ipsocktype *soc, *next_soc, *prev_soc;
    char *faddr, *laddr;
    static char* const ipsochdr =
	"Prot"
	"o    Remote      Port "
	"     Local       Port "
	" In Out Stat TTY OutputIF";

    automore_enable(ipsochdr);
    for (soc = IPsocketQ.qhead; soc != NULL; soc = next_soc) {
	mem_lock(soc);
	prev_soc = soc->next;
	faddr = string_getnext();
	if (soc->faddress.type != ADDR_ILLEGAL) {
	    sprintf(faddr, "%-3u %15i %-5u", soc->protocol,
		    soc->faddress.ip_addr, soc->fport);
	} else {
	    sprintf(faddr, "%-3u   --listen--", soc->protocol);
	}
	if (soc->laddress.type == ADDR_ILLEGAL) {
any:	    laddr = "--any--";
	} else {
	    if (soc->laddress.ip_addr == 0L) {
		goto any;
	    }
	    laddr = address_string(&soc->laddress);
	}
	printf("\n%25s %15s %-5u %-3d %-3d %-4x %-3t %s",
	       faddr, laddr, soc->lport,
	       process_queue_size(soc->inq), QUEUESIZE(&soc->outq),
	       soc->status, soc->soctty->ttynum,
	       (soc->if_output) ? soc->if_output->namestring : "");

	/*
	 * watch for the current socket being free'd out from under us
	 * while we were doing the printf
	 */
	next_soc  = soc->next;	/* get it while we know soc is valid */
	free(soc);

	/*
	 * if the next queue entry is NULL, then we are eather at the end
	 * of the queue, or current buffer was free'd from under us. to
	 * determine which we check to see if the soc->next pointer has
	 * changed.
	 */
	if ((next_soc == NULL) && (next_soc != prev_soc)) {
	    printf("\n  *** IP socket list changed during display\n");
	} 
    }
    automore_disable();
}

/*
 * ip_fixsocket_address
 * This is a wrapper function for fixup_ipsockets to call it from
 * generalized system registry.  It sends a message to the background
 * process to do the actual socket table adjustment
 */
void ip_fixsocket_address (idbtype *swidb, hwidbtype *hwidb)
{
    /* check to see if ip background is running to handle this request */ 
    if (ip_background_pid) {
	process_send_message(ip_background_pid, MSG_SOCKET_ADJUST, 0, 0);
    } else {
	/* No? then handle it right here... */
	fixup_ipsockets();
    }
}

/*
 * fixup_ipsockets
 * Scan IP socket queue, fixing up addressing information
 */

void fixup_ipsockets (void)
{
    ipsocktype* soc;

    if (!systeminit_flag) {
	/*
	 * If system is still booting, changing our source IP addresses
	 * for existing sockets is dealt with explicitly when necessary.
	 */
	return;
    }
    for (soc = (ipsocktype*) IPsocketQ.qhead; soc != NULL; soc = soc->next) {
	if ((soc->if_output == NULL) &&
	    !(soc->status & SOCK_SRC_SPECIFIED)) {
	    soc->laddress.ip_addr = 
		ip_best_local_address(soc->faddress.ip_addr, FALSE);
	}
    }
}

/*
 * ip_address_command
 * Set address and network mask of a network interface.
 *	"ip address <ip address> <subnet mask> [secondary]"
 *	"no ip address [<ip address> [<subnet mask> [secondary]]]"
 */

void ip_address_command (parseinfo *csb)
{
    ipaddrtype address, mask, oldaddress, oldmask;
    ipaddtype *ptr;
    idbtype *idb, *swidb;
    hwidbtype *hwidb;
    boolean mac_afilter_set;
    boolean cacheflush;

    idb = csb->interface;

    if (csb->nvgen) {

	/* Do not explicitly generate "no ip address" on subinterfaces */

	if ((!idb->ip_enabled) && is_first_swidb(idb)) {

	    nv_write(TRUE, "no %s", csb->nv_command);
	    return;
	}
	for (ptr = idb->ip_addressQ.qhead; ptr; ptr = ptr->next)
	    nv_write(TRUE, "%s %i %i secondary",
		     csb->nv_command, ptr->address, ptr->mask);

	nv_write(idb->ip_address, "%s %i %i",
		 csb->nv_command, idb->ip_address, idb->ip_nets_mask);
	return;
    }

    /* disallow IP Addresses on SDLC links  CSCdi71223  */
    if (is_sdlc(csb->interface->hwptr)) {
        printf("\n\n%% IP addresses may not be configured on SDLC links.\n");
        return;
  }


    /*
     * IF configuring an IP address on a LAN subinterface then ensure that the
     * subinterface has already been configured as a Virtual LAN interface.
     *
     * Enable "ip route-cache same-interface" for IP routing between vLAN
     * subinterfaces of the same hardware IDB.
     *
     * When initializing and reading the configuration from NV RAM or a 
     * configuration file let it go since the parser orders things such that
     * IP addressing comes first.
     */

    if ((is_subinterface(idb)) && (!IS_VLAN_CONFIGURED_SUBINTERFACE(idb)) &&
	is_ieee802(idb->hwptr) && (csb->resolvemethod == RES_MANUAL)) {

            printf ("\n\nConfiguring IP routing on a LAN subinterface is only"
                    " allowed if that\nsubinterface is already configured as"
                    " part of an IEEE 802.10 or ISL vLAN.\n");
            return;
    }

    cacheflush = FALSE;

    /*
     * If there was a preceding "default" or  
     * if we didn't parse an IP address and there was a preceding "no",
     * then we are disabling IP processing.  Flush routing table and ARP
     * cache. Set IP to be disabled on this interface and return.
     * Note : Because of integrated ISIS ip_address_change 
     * must be called after ip is disabled on that interface.
     */
    if (GETOBJ(paddr,1)->type != ADDR_IP || csb->set_to_default) {
	if (!csb->sense || csb->set_to_default) {
	    idb->ip_enabled = FALSE;
	    if (idb->ip_address) {
		reg_invoke_ipcache_invalidate_range(idb->ip_address,
						    idb->ip_nets_mask);
	    }
	    address = idb->ip_address;
	    idb->ip_address = 0L;	/* zero says look at ip_unnumbered */
	    if (address)
		ip_address_change(idb, address, idb->ip_nets_mask, 
				  FALSE, FALSE);
	    ip_rxtypes_update(idb);

	    /*
	     * Clear all secondary addresses.
	     */
	    while (!QUEUEEMPTY(&idb->ip_addressQ)) {
		ptr = dequeue(&idb->ip_addressQ);
		reg_invoke_ipcache_invalidate_range(ptr->address, ptr->mask);
		ip_address_change(idb, ptr->address, ptr->mask, TRUE, FALSE);
		free(ptr);
	    }
	    idb->ip_unnumbered = NULL;
	    ip_address_change(idb, 0, 0, FALSE, FALSE);
	    reg_invoke_clear_idb_routes(idb);
	    arp_flush_cache(idb);
	    recompute_syssec();
	    fixup_ipsockets();
	    ipaddr_table_rebuild(idb);
	}
	return;
    }
    address = GETOBJ(paddr,1)->ip_addr;
    mask = GETOBJ(paddr,2)->ip_addr;
    oldaddress = idb->ip_address;
    oldmask = idb->ip_nets_mask;

    if (martian(address,TRUE)) {
	printf("Not a valid host address - %i", address);
	return;
    }

    if (!good_address(address,mask,TRUE))
   	if ((mask != -1L) || (idb->hwptr->type != IDBTYPE_LB)) {
	    printf("Bad mask%#m for address %i", mask, address);
	    return;
	}

    /*
     * Process a secondary address command
     */
    if (GETOBJ(int,1)) {
	if (reg_invoke_ip_secondary(idb, csb->sense, address, mask))
	    ip_address_change(idb, address, mask, TRUE, csb->sense);
	ipaddr_table_rebuild(idb);
	return;
    }

    if (csb->sense) {
	/*
	 * If nothing changed, quietly return.
	 */
	if ((idb->ip_address == address) && (idb->ip_nets_mask == mask))
	    return;

        /* 
         * Check to see if new address is a duplicate on other interfaces.
	 *
	 * If we have learned one of our interface addresses dynamically,
	 * (such as via bootp) and we are processing new IP addresses
	 * specified from a configuration file during system initialization,
	 * then skip this check.  We don't want the dynamically
	 * specified IP address conflicting with the ultimate interface
	 * address configuration.
         */
        if (!(csb->resolvemethod == RES_CONFIG && !netsareup)) {
	    if (ip_address_duplicate(address, mask, idb, TRUE)) { 
		if (!interface_admin_down(idb))
		    return;
	    }
	}

	if (idb->ip_address) {	/* if interface already configured */
	    reg_invoke_ipcache_invalidate_range(idb->ip_address,
						idb->ip_nets_mask);
	    reg_invoke_iprouting_delete_conn_route(idb->ip_address,
						   idb->ip_nets_mask);
	    if ((idb->ip_address & idb->ip_nets_mask) != (address & mask))
		reg_invoke_clear_idb_routes(idb);
	    cacheflush = TRUE;
	}

	/*
	 * If new primary is old secondary, flush old secondary.
	 */
	for (ptr = idb->ip_addressQ.qhead; ptr; ptr = ptr->next) {
	    if (ptr->address == address) {
		unqueue(&idb->ip_addressQ, ptr);
		ip_address_change(idb, ptr->address, ptr->mask, TRUE, FALSE);
		free(ptr);
		break;
	    }
	}
    } else { 
	/*
	 *  If there are secondaries, don't allow deletion of primary.
	 */
	if (idb->ip_addressQ.qhead) {
	    printf("\nMust delete secondary before deleting primary");
	    return;
	}

	/*
	 * Disabling IP on this interface and an address was specified.
	 * Make sure correct address was specified.
	 * Note : Because of integrated ISIS ip_address_change 
	 * must be called after ip is disabled on that interface.
	 * We call ip_address_change once with the address (so we explicitly
	 * remove it) and once with no address (for those who want to know that
	 * all the addresses are gone).
	 */
	if ((idb->ip_address == address) && (idb->ip_nets_mask == mask)) {
	    idb->ip_enabled = FALSE;
	    reg_invoke_ipcache_invalidate_range(idb->ip_address,
						idb->ip_nets_mask);
	    idb->ip_address = 0L;
	    ip_address_change(idb, address, mask, FALSE, FALSE);
	    ip_rxtypes_update(idb);
	    ip_address_change(idb, 0, 0, FALSE, FALSE);
	    idb->ip_unnumbered = NULL;
	    reg_invoke_clear_idb_routes(idb);
	    arp_flush_cache(idb);
	    recompute_syssec();
	    fixup_ipsockets();
	} else
	    printf("\nInvalid address");
	ipaddr_table_rebuild(idb);
	return;
    }

    /*
     * If address changed, changed resolution method
     */
    if (idb->ip_address != address) {
	idb->resolvemethod = csb->resolvemethod;
	if (csb->resolvemethod == RES_CONFIG && hostconfg.responder
	    && hostconfg.proto == FILE_ACCESS_TFTP)
	    idb->ip_resolvehost = hostconfg.ps.tuba.respondent.ip_addr;
	else
	    idb->ip_resolvehost = 0L;
    }
    idb->ip_unnumbered = NULL;
    idb->ip_address = address;
    idb->ip_nets_mask = mask;
    reg_invoke_ipcache_invalidate_range(address, mask);
    if (!idb->ip_enabled && interface_up(idb)) {
	/*
	 * MAC address filter may need to set up IP multicast addresses.
	 */
	ip_rxtypes_update(idb);
	hwidb = idb->hwptr;
	if (hwidb->status & IDB_IEEEMASK) {
	    mac_afilter_set = FALSE;
	    FOR_ALL_SWIDBS_ON_HW(hwidb, swidb) {
		if (swidb->ip_enabled) {
		    mac_afilter_set = TRUE;
		    break;
		}
	    }
	    if (!mac_afilter_set) {
		idb->ip_enabled = TRUE;
		if (hwidb->set_extra_addrs) {
		    /*
		     * Functional addresses already set up by ip_multicast_ctl
		     */
		} else {
		    /*
		     * Update interface's MAC address filter.
		     * Address filter was not previously initialized with
		     * IP multicast addresses since IP was not enabled
		     * on the interface.
		     */
		    if (idb->hwptr->afilter_reset)
			(idb->hwptr->afilter_reset)(idb->hwptr);
		    else
			ip_macaddrs_update(idb);
		}
	    }
	}
    }
    idb->ip_enabled = TRUE;
    ip_rxtypes_update(idb);
    if (oldaddress)
	ip_address_change(idb, oldaddress, oldmask, FALSE, FALSE);
    ip_address_change(idb, address, mask, FALSE, TRUE);
    if (max_proto_mtu(idb, LINK_IP) < idb->ip_mtu)
 	idb->ip_mtu = max_proto_mtu(idb, LINK_IP);
    recompute_syssec();
    class_resolve(idb);
    fixup_ipsockets();
    (void) ipisresolved();
    if (cacheflush) 
	arp_flush_cache(idb);
}

/*
 * ip_interface_erase
 *
 * Wipe all traces of IP dealing with addresses off of a given interface
 * called by reg_invoke_swif_erase
 */

void ip_interface_erase (idbtype *idb)
{
    ipaddtype *ptr;

    if (idb->ip_address) {
	ip_address_change(idb, idb->ip_address, idb->ip_nets_mask, 
			  FALSE, FALSE);
	reg_invoke_ipcache_invalidate_range(idb->ip_address, idb->ip_nets_mask);
    }

    while ((ptr = dequeue(&idb->ip_addressQ)) != NULL) {
	ip_address_change(idb, ptr->address, ptr->mask, TRUE, FALSE);
	reg_invoke_ipcache_invalidate_range(ptr->address, ptr->mask);
	free(ptr);
    }

    idb->ip_address = 0L;
    idb->ip_enabled = FALSE;
    idb->ip_unnumbered = NULL;

    ip_rxtypes_update(idb);
    ip_address_change(idb, 0, 0, FALSE, FALSE);
    reg_invoke_clear_idb_routes(idb);
    arp_flush_cache(idb);
    recompute_syssec();
    fixup_ipsockets();
    ipaddr_table_rebuild(idb);
}


/*
 * pick_defaultidb
 * We don't have a good default interface, pick something.
 * We used to pick the first idb with ip running, which was usually a lan.
 * The order of interfaces on the idbq changed, so we now pick the idb
 * with the highest bandwidth.
 */

idbtype *pick_defaultidb (ipaddrtype network)
{
    idbtype *idb, *any_ipidb, *up_ipidb;

    any_ipidb = NULL;
    up_ipidb = NULL;
    network = get_majornet(network);
    FOR_ALL_SWIDBS(idb) {
	if (!idb->ip_enabled || idb->ip_address == 0L)
	    continue;
	if (!interface_up(idb)) {
	    if (any_ipidb == NULL || 
		(any_ipidb->visible_bandwidth < idb->visible_bandwidth))
		any_ipidb = idb;
	} else {
	    if (idb->ip_majornet == network)
		return(idb);
	    if (up_ipidb == NULL || 
		(up_ipidb->visible_bandwidth < idb->visible_bandwidth))
		up_ipidb = idb;
	}
    }
    /*
     * Prefer any up IP interfaces
     */
    if (up_ipidb)
	return(up_ipidb);
    return(any_ipidb);
}

/*
 * ip_best_local_address
 * Given a target IP address, return the best interface address.
 */

ipaddrtype ip_best_local_address (ipaddrtype target, boolean secondok)
{
    idbtype *idb;
    ipaddrtype next, result;

    /*
     * Try routing to that address.
     */
    next = target;
    idb = reg_invoke_ip_first_hop(&next, next, IP_PATH_DEFFLAGS);

    if (idb) {
	if (idb->ip_unnumbered)
	    return(idb->ip_unnumbered->ip_address);
        /*
         * If we get an interface with an address of zero,
         * reject it, and pick a new interface below
         */
       if (!idb->ip_address)
           idb = NULL;
    }
    if (!idb) {
	/*
	 * Could not find a path to destination IP address.  We must be
	 * routing IP at this point (otherwise find_path via
	 * ip_first_hop registry would have returned something).
	 * Make one last feeble attempt.  
	 * (Since we can't route to the destination anyways, does picking a
	 * reasonable source IP address really matter?)
	 */
	idb = pick_defaultidb(target);
	if (!idb)
	    return(0L);				/* No usable interfaces */
    }
    result = ip_best_ifaddress(target, idb, secondok);
    return(result ? result : idb->ip_address);
}

/*
 * ip_best_ifaddress
 * Given a target address and an idb pointer, return best local IP address.
 * "Best" is an address on a shared subnet followed by an address on a
 * shared major net.
 */

ipaddrtype ip_best_ifaddress (ipaddrtype target, idbtype *firstidb,
			      boolean secondok)
{
    ipaddrtype mask;
    ipaddtype *ptr;
    hwidbtype *hwidb, *netidb;
    idbtype *idb;

    hwidb = firstidb->hwptr;
    if ((hwidb != NULL) &&
	((netidb = get_netidb(hwidb)) != NULL) &&
	(hwidb != netidb))
	idb = netidb->firstsw;
    else
	idb = firstidb;

    /*
     * Look for subnet matches first
     */
    if ((target & idb->ip_nets_mask)==(idb->ip_address & idb->ip_nets_mask)) {
	return(idb->ip_address);
    }

    if (secondok)
	for (ptr = (ipaddtype *)idb->ip_addressQ.qhead; ptr; ptr = ptr->next) {
	    if ((target & ptr->mask) == (ptr->address & ptr->mask)) {
		return(ptr->address);
	    }
	}

    /*
     * No subnet matches, try major network
     */
    mask = get_majormask(target);
    target &= mask;
    if (target == idb->ip_majornet)
	return(idb->ip_address);
    if (secondok)
	for (ptr = (ipaddtype *)idb->ip_addressQ.qhead; ptr; ptr = ptr->next) {
	    if (target == (ptr->address & mask))
		return(ptr->address);
	}
    return(0L);
}

/*
 * martian
 * Returns "TRUE" if the address is one of Mill's Martians, i.e., it is not
 * a valid IP address for a host or network.
 * "addrflag" is TRUE if we are checking a host address, FALSE if we are
 * checking a network address (host portion not to be looked at).
 */

#define MAXMARTIANS (12)

struct martiantype {
    ipaddrtype address;		/* potential bad address */
    ipaddrtype mask;		/* address mask */
    tinybool net;		/* network check */
    tinybool valid_entry;
} static martians[MAXMARTIANS] = {
 /*
  * Class A specials
  */
 {0x00000000, 0xFF000000, FALSE, TRUE},/* reserved, 0.0.0.0,  255.0.0.0 */
 {0x7F000000, 0xFF000000, FALSE, TRUE},/* reserved, 127.0.0.0,  255.0.0.0 */
 {0x00000000, 0x80FFFFFF, TRUE, TRUE}, /* locl use, 0.0.0.0,  128.255.255.255 */
 {0x00FFFFFF, 0x80FFFFFF, FALSE, TRUE},/* bcast, 0.255.255.255, 128.255.255.255 */

 /*
  * Class B specials
  */
 {0x80000000, 0xFFFF0000, FALSE, TRUE},/* reserved, 128.0.0.0,  255.255.0.0 */
 {0xBFFF0000, 0xFFFF0000, FALSE, TRUE},/* reserved, 191.255.0.0,  255.255.0.0 */
 {0x80000000, 0xC000FFFF, TRUE, TRUE}, /* locl use, 128.0.0.0, 192.0.255.255 */
 {0x8000FFFF, 0xC000FFFF, FALSE, TRUE},/* bcast, 128.0.255.255, 192.0.255.255 */

 /*
  * Class C specials
  */
 {0xDFFFFF00, 0xFFFFFF00, FALSE, TRUE},/* C res 223.255.255.0, 255.255.255.0 */
 {0xC0000000, 0xE00000FF, TRUE, TRUE}, /* C locl use, 192.0.0.0,  224.0.0.255 */
 {0xC00000FF, 0xE00000FF, FALSE, TRUE},/* C locl bcast, 192.0.0.255, 224.0.0.255 */

 /*
  * Class D specials
  */
 {0xE0000000, 0xE0000000, FALSE, TRUE}, /* D reserved, 224.0.0.0, 224.0.0.0 */
};

boolean martian (register ipaddrtype address, boolean hostflag)
{
    register int i;

    for (i = 0; i < MAXMARTIANS; i++) {
	if ((address & martians[i].mask) == martians[i].address) {
	    /*
	     * Skip the entry in the table, if it has been marked invalid.
	     */
	    if (!martians[i].valid_entry)
		continue;
	    if (!hostflag && martians[i].net)
		continue;
	    return(TRUE);
	}
    }
    return(FALSE);
}

boolean ip_martian (addrtype *address, boolean hostflag)
{
    return(martian(address->ip_addr, hostflag));
}

/*
 * Function to mark the  first instance of old_address and old_mask in
 * martian table to valid/invlaid
 *
 * Returns FALSE if old_address and old_mask combination not found.
 * otherwise returns true.
 *
 * Used by the Cat5K ATM card to allow IP address of 127.x.x.x
 */
boolean ip_adjust_martian (ipaddrtype old_address, ipaddrtype old_mask,
                           boolean valid)
{
 
    int i;
 
    for(i=0; i<MAXMARTIANS; i++) {
        if (martians[i].address == old_address &&
	    martians[i].mask == old_mask) {
	    martians[i].valid_entry = valid;
            return TRUE;
        }
    }
    return FALSE;
}

/*
 * ip_defaultgw_command
 * Set up the default_gateway cell
 */

void ip_defaultgw_command (parseinfo *csb)
{
    if (csb->nvgen) {
	if (!reg_invoke_ip_gdpclient_enable())
	    nv_write(default_gateway != IPADDR_ZERO,
		     "%s %i", csb->nv_command, default_gateway);
	return;
    }
    if (csb->sense == FALSE) {
	default_gateway = IPADDR_ZERO;
        reg_invoke_ip_defaultgw_change(default_gateway);
    } else {
	if (martian(GETOBJ(paddr,1)->ip_addr, TRUE)) {
	    printf("\nInvalid host address");
	} else {
	    default_gateway = GETOBJ(paddr,1)->ip_addr;
            reg_invoke_ip_defaultgw_change(default_gateway);
	}
    }
}

/*
 * ip_broadcast
 * Returns TRUE/FALSE if address is broadcast on an interface
 * Warning:  Be very careful.  This is now being called at interrupt level
 *           by udp_fast_flood().
 */

boolean ip_broadcast (ipaddrtype target, idbtype *idb)
{
    ipaddtype *ptr;

    /*
     * Check preferred broadcast address.
     */
    if (target == idb->ip_broadcast)
	return(TRUE);

    /*
     * Check primary IP address.
     */
    if (ip_ifbroadcast(target, idb->ip_address, idb->ip_nets_mask))
	return(TRUE);
    /*
     * Check secondary IP addresses
     */
    for (ptr = (ipaddtype *) idb->ip_addressQ.qhead; ptr; ptr = ptr->next) {
	if (ip_ifbroadcast(target, ptr->address, ptr->mask))
	    return(TRUE);
    }
    return(FALSE);
}

/*
 * ip_multicast
 * Returns TRUE/FALSE if address is enabled multicast on an interface
 */
boolean ip_multicast (ipaddrtype target, idbtype *idb)
{
    ipmultiaddtype *ptr;

    if (IPMULTICAST(target))
	for (ptr = (ipmultiaddtype *) idb->ip_multiaddrQ.qhead; ptr;
	     ptr = ptr->next)
	    if (target == ptr->address)
		return(TRUE);

    return(FALSE);
}

/*
 * ip_ifnetwork
 * Return TRUE if target is one of the networks on this interface.
 */

boolean ip_ifnetwork (idbtype *idb, ipaddrtype target)
{
    ipaddtype *ptr;
    ipaddrtype address;

    if (idb->ip_unnumbered)
	idb = idb->ip_unnumbered;

    address = get_majornet(idb->ip_address);
    if (address == target)
	return(TRUE);

    for (ptr = (ipaddtype *) idb->ip_addressQ.qhead; ptr; ptr = ptr->next) {
	address = get_majornet(ptr->address);
	if (address == target)
	    return(TRUE);
    }
    return(FALSE);
}


/*
 * ip_ifaddress
 * Return TRUE if target is one of our interface addresses.
 */

boolean ip_ifaddress (idbtype *idb, ipaddrtype target)
{
    ipaddtype *ptr;

    if (!interface_up_simple(idb))
	return(FALSE);
    if (idb->ip_unnumbered)
	idb = idb->ip_unnumbered;
    if (idb->ip_address == target)
	return(TRUE);
    for (ptr = (ipaddtype *) idb->ip_addressQ.qhead; ptr; ptr = ptr->next) {
	if (ptr->address == target)
	    return(TRUE);
    }
    return(FALSE);
}

/*
 * ip_ouraddress
 * Returns TRUE if the target is one of our interface or alias addresses.
 */

boolean ip_ouraddress (ipaddrtype target)
{
    ipaddr_tabletype *addr;

    FOR_IPADDR_HASH_CHAIN(target, addr) {
	if (addr->address == target)
	    return(TRUE);
    }
    return(FALSE);
}


/*
 * ip_ouraddress_type
 * Returns the ipaddr_type if the target is one of
 * our interface, alias or pool addresses, or IPADDR_UNKNOWN if not.
 */

ipaddress_type ip_ouraddress_type (ipaddrtype target)
{
    ipaddr_tabletype *addr;

    FOR_IPADDR_HASH_CHAIN(target, addr) {
	if (addr->address == target)
	    return(addr->type);
   }
    return (IPADDR_UNKNOWN);
}


/*
 * ip_ouraddress_interface
 * Returns the interface pointer if the target is one of
 * our interface or standby addresses, or NULL if not.  This routine
 * will return NULL for an address that is ours but is not bound
 * to an interface (such as aliases), so beware!
 */

idbtype *ip_ouraddress_interface (ipaddrtype target)
{
    ipaddr_tabletype *addr;

    FOR_IPADDR_HASH_CHAIN(target, addr) {
	if (addr->address == target)
	    return(addr->swidb);
    }
    return(NULL);
}

/*
 * ipaddresstype_string
 * Returns the pointer to ipaddress_type string.
 * (i.e: printf can use this.)
 */

char *ipaddresstype_string (ipaddress_type type)
{
    switch (type) {

        case IPADDR_INTERFACE: return("interface");
        case IPADDR_ALIAS: return("alias");
        case IPADDR_DYN_ALIAS: return("dynamic alias");
        default: return("unknown");
    }
}

/*
 * ip_staticaliasaddr
 * Return TRUE if this is a static alias address.
 * Will return FALSE if the match is for an interface address
 */

boolean ip_staticaliasaddr (ipaddrtype address)
{
    ipaddr_tabletype *addr;

    FOR_IPADDR_HASH_CHAIN(address, addr) {
	if (addr->address == address)
	    return((addr->type == IPADDR_ALIAS));
    }
    return(FALSE);
}

/*
 * ip_aliasaddress
 * Return TRUE if this is either a dynamic or static alias address.
 * Will return FALSE if the match is for an interface address
 */

boolean ip_aliasaddress (ipaddrtype address)
{
    ipaddr_tabletype *addr;

    FOR_IPADDR_HASH_CHAIN(address, addr) {
	if (addr->address != address)
	    continue;
	if (addr->type == IPADDR_ALIAS)
	    return(TRUE);
	if (addr->type == IPADDR_DYN_ALIAS)
	    return(TRUE);
	return(FALSE);
    }
    return(FALSE);
}

/*
 * ip_up_ouraddress
 * Returns TRUE if the target is one of our 
 * interface address and that interface is not
 * administratively DOWN.  Handles possibility of multiple
 * interfaces with same IP address.
 */

boolean ip_up_ouraddress (ipaddrtype target)
{
    ipaddr_tabletype *addr;

    FOR_IPADDR_HASH_CHAIN(target, addr) {
	if ((addr->address == target) && (addr->type == IPADDR_INTERFACE)) {
	    if (!interface_admin_down(addr->swidb))
		return(TRUE);
	}
    }
    return(FALSE);
}	    

/*
 * ip_ifbroadcast
 * Is this is a broadcast address, given an interface address and mask?
 * Warning:  Be very careful.  This is now being called at interrupt level
 *           by udp_fast_flood() via ip_broadcast().
 */

boolean ip_ifbroadcast (ipaddrtype target, ipaddrtype address, ipaddrtype mask)
{
    ipaddrtype nettarget, majornet, majormask;

    /*
     * If we're playing weird fun and games with loopback interfaces or
     * some sort of host-route-sized tunnel, we're not broadcasting.
     */
    if (mask == -1L)
	return(FALSE);

    /*
     * Check for an all ones broadcast address
     */
    majornet = get_majornet(address);
    if ((target & ~mask) == ~mask) {
	nettarget = target & mask;		/* isolate network */
	if (nettarget == mask)
	    return(TRUE);			/* net is all ones */
	if (nettarget == (address & mask)) 
	    return(TRUE);			/* subnet broadcast */
        majormask = get_majormask(address);
	if (target == (majornet | ~majormask))
	    return(TRUE);			/* all subnets broadcast */
    }

    /*
     * Check for an all zeros broadcast address
     */
    if ((target & ~mask) == 0) {
	nettarget = target & mask;		/* isolate network */
	if (nettarget == 0)
	    return(TRUE);			/* net is all zeros */
	if (nettarget == (address & mask))
	    return(TRUE);			/* subnet broadcast */
	if (nettarget == majornet)
	    return(TRUE);			/* netwide broadcast */
    }
    return(FALSE);
}

/*
 * samecable
 * Returns idb pointer if address is directly accessible, i.e. we don't have
 * to go through a gateway to get there.  Returns NULL otherwise.
 */

idbtype *samecable (ipaddrtype address)
{
    idbtype *idb;

    FOR_ALL_SWIDBS(idb) {
	if (!idb->ip_enabled || !interface_up(idb))
	    continue;
	if (ip_address_on_idb(address, idb))
	    return(idb);
    }
    return(NULL);
}

/*
 * ip_address_on_idb
 *
 * Returns TRUE if address is attached on the input idb.
 */
boolean ip_address_on_idb (ipaddrtype address, idbtype *idb)
{

    ipaddtype *ptr;

    if (!idb->ip_address)
	return(FALSE);

    if ((idb->ip_address & idb->ip_nets_mask) == (address & idb->ip_nets_mask))
	return(TRUE);

    for (ptr = (ipaddtype *)idb->ip_addressQ.qhead; ptr; ptr = ptr->next) {
	if ((ptr->address & ptr->mask) == (address & ptr->mask))
	    return(TRUE);
    }
    return(FALSE);
}

/*
 * ipaddr_findidb
 * Given an IP address, find associated software idb.
 * Like samecable, but allow the interface to be down...
 */

idbtype *ipaddr_findidb (ipaddrtype address)
{
    idbtype *idb;

    FOR_ALL_SWIDBS(idb) {
	if (!idb->ip_enabled)
	    continue;
	if (ip_address_on_idb(address, idb)) 
	    return(idb);
    }
    return(NULL);
}

/*
 * ip_match_connected_subnet
 * Returns a connected subnet matching the given address.  Returns FALSE on
 * failure. 
 */

boolean ip_match_connected_subnet (ipaddrtype address, ipaddrtype *subnet,
				   ipaddrtype *subnet_mask)
{
    idbtype *idb;
    ipaddrtype mask;

    idb = samecable(address);
    if (!idb)
	return(FALSE);
    mask = idb->ip_nets_mask;
    *subnet = (address & mask);
    *subnet_mask = mask;
    return(TRUE);
}

/*
 * ip_resolvestr
 * Display IP resolve string
 */

char *ip_resolvestr (idbtype *idb)
{
    char *str;

    switch (idb->resolvemethod) {
        case RES_RARP: str = "Reverse ARP"; break;
        case RES_BOOTP: str = "BOOTP"; break;
        case RES_CONFIG: str = "configuration file"; break;
        case RES_MANUAL: str = "setup command"; break;
        case RES_NONVOL: str = "non-volatile memory"; break;
	case RES_HEURISTIC: str = "heuristics"; break;
        default: str = "unknown means"; break;
    }
    return(str);
}

/*
 * resolve_ipaddr
 * Determine the IP address of an interface
 */

void resolve_ipaddr (ipaddrtype *address,
		     ipaddrtype *mask,
		     ipaddrtype defaddress,
		     ipaddrtype *addrlist,
		     ipaddrtype *masklist)
{
    int status, i, j, bits = 0, maxbits, defbits;
    char buffer[MAXBUF], *cp, addrtype;
    ipaddrtype majornet = 0;
    boolean seenbefore = FALSE;
    idbtype *idb;
    
    while (!(stdio->statbits & CONTROLC)) {
	/*
	 * Get an address
	 */
	printf("\n    IP address for this interface");
	if (defaddress)
	    printf(" [%i]", defaddress);
	printf(": ");
	if (!rdtty(buffer, MAXBUF))
	    continue;
	if (null(buffer)) {
	    *address = defaddress;
	    if (!defaddress)
		continue;
	} else {
	    status = parse_ip_address(buffer, address);
	    if (!status || !*address || !good_address(*address, 0L, FALSE)) {
		cp = (char *) deblank(buffer);
		if (*cp != '?')
		    printf("\n%% Bad Internet address.");
		printf(setup_iphelp);
		continue;
	    }
	}

	/*
         * Get max bits in subnet mask, address class, default subnet mask.
	 */
	if ((*address & CLASS_A_BIT) == 0) {
	    addrtype = 'A';
	    maxbits = 24;
	    *mask = CLASS_A_MASK;
	} else if ((*address & CLASS_B_BIT) == 0) {
	    addrtype = 'B';
	    maxbits = 16;
	    *mask = CLASS_B_MASK;
	} else if ((*address & CLASS_C_BIT) == 0) {
	    addrtype = 'C';
	    maxbits = 8;
	    *mask = CLASS_C_MASK;
	} else {
	    addrtype = 'D';
	    maxbits = 0;
	    *mask = CLASS_D_MASK;
	}

	/*
	 * See if we've seen this network before.
	 * This catches attempts to alter the subnet mask and keeps the
	 * major networks in addrlist unique for the router subcommand network.
	 */
	majornet = (*address & *mask);
        defbits = 0;
	seenbefore = FALSE;
	if (addrlist && masklist) {
	    for ( ; *addrlist; addrlist++, masklist++) {
		if (majornet == *addrlist) {
		    defbits = *masklist;
		    seenbefore = TRUE;
		    break;
		}
	    }
	}
	if (!seenbefore) {
	    /*
	     * We haven't seen this network during this setup command.
	     * Check to see if is it exists in the currently running
	     * router so that we can get the default right.
	     */
	    FOR_ALL_SWIDBS(idb) {
		if (idb->ip_enabled && (idb->ip_majornet == majornet)) {
		    for (i = maxbits-1; i >= 0; i--) {
			if (idb->ip_nets_mask & (1 << i))
			    defbits++;
		    }
		    break;
		}
	    }
	}
	/*
	 * The following function call uses maxbits-2 because
	 * documentation thinks it is confusing for the help message to
	 * state one range of bits, but to enforce a different range.
	 */
	bits = get_num_in_range("    Number of bits in subnet field",
			10, 0, maxbits-2, defbits);
	if (seenbefore && (defbits != bits)) {
	    printf("%% Cannot have variable number of subnet bits\n");
	    continue;
	}
	if (bits == 1) {
	    printf("%% Too few subnet bits\n");
	    continue;
	}
	for (i = maxbits-1, j = bits; j; i--, j--) {
	    *mask |= (1 << i);
	}
	if (!good_address(*address, *mask, TRUE)) {
	    printf("%% Bad subnet mask\n");
	    continue;
	}

	/*
	 * cisco Training wants us to recite the information back to user.  
	 */
	printf("\n    Class %c network is %i, %d subnet bits; mask is%#m",
	       addrtype, majornet, bits, *mask);
	break;
    }

    /*
     * We have an address and the number of subnet bits
     * Enter address into list if we haven't seen it before.
     */
    if (addrlist && masklist && !seenbefore) {
	while (*addrlist) {
	    addrlist++;
	    masklist++;
	}
	*addrlist = majornet;
	*masklist = bits;
    }
}

/*
 * ip_multicast_ctl
 *
 * Join a particular IP multicast group. This function adds the mapped
 * IP group address to data link address to the address filter.
 */
void ip_multicast_ctl (idbtype *idb, ipaddrtype group, boolean join)
{
    ipmultiaddtype     *ptr;
    hwidbtype          *hwidb = idb->hwptr;
    ethermultiaddrtype hwaddr = ETHER_IP_MCAST;

    hwaddr.bottom23 = group;
    for (ptr = (ipmultiaddtype *) idb->ip_multiaddrQ.qhead; ptr;
	 ptr = ptr->next) {
	if (ptr->address == group) break;
    }

    if (join) {
	if (ptr) {
	    ptr->refcount++;
	    return;
	} else {
	    ptr = malloc(sizeof(ipmultiaddtype));
	    if (ptr) {
		ptr->address = group;
		ptr->mask = 0;	/* currently unused */
		ptr->refcount = 1;
		enqueue(&idb->ip_multiaddrQ, ptr);
	    } else {

		/*
		 * Couldn't get the memory.  Bail out.
		 */
		return;
	    }
	}
    } else {
	if (ptr) {
	    if (--ptr->refcount == 0) {
		unqueue(&idb->ip_multiaddrQ, ptr);
		free(ptr);
	    } else {

		/*
		 * More references.  Don't delete it quite yet.
		 */
		return;
	    }
	} else {

	    /*
	     * We can't remove an entry that doesn't exist.
	     */
	    return;
	}
    }

    /*
     * Now actually go tell the hardware about this.
     */
    if ((hwidb->status & IDB_IEEEMASK) || is_bvi(hwidb)) {
	if (hwidb->set_extra_addrs) {
	    (*hwidb->set_extra_addrs)(hwidb);
	} else if (join && (hwidb->status & IDB_FDDI)) {

	    /*
	     * When joining, add a single address to the address filter. We're
	     * avoiding resetting the interface.
	     */
	    if_multi_add(hwidb, idb, hwaddr.addr);

	    /* 
	     * Add any new ip multicast's to the smf
	     */
	    FOR_ALL_SWIDBS(idb) {
		ip_macaddrs_update(idb);
	    }
	} else {

	    /*
	     * There is no way to delete a single address from the address 
	     * filter. We have to reset in this case.
	     */
	    (*hwidb->reset)(hwidb);
	    if (!join) {
		if (is_bvi(hwidb)) {
		    reg_invoke_bvi_smf_delete(hwaddr.addr, 
					      idb,
					      SMF_MACTYP_IP_MCAST |
					      SMF_ACTION_RECEIVE,
					      IDB_BRIDGE);
		} else {
		    reg_invoke_tbridge_smf_delete(hwaddr.addr, 
						  idb,
						  SMF_MACTYP_IP_MCAST |
						  SMF_ACTION_RECEIVE);
		}
	    }
	}
    }
}

/*
 * ip_stay_shutdown
 * Called to see if the interface should stay in the shutdown state.
 * Return TRUE if there is a duplicate IP address in the system.
 */

static const char ip_stay[] ="\n%s: incorrect IP address assignment";

boolean ip_stay_shutdown (idbtype *idb)
{
    ipaddtype *ptr;

    if (systeminit_flag && idb->ip_enabled && idb->ip_address != 0L) {
	if (ip_address_duplicate(idb->ip_address, idb->ip_nets_mask,idb,TRUE)){
	    printf(ip_stay, idb->namestring);
	    return(TRUE);
	}
	for (ptr = (ipaddtype *) idb->ip_addressQ.qhead; ptr; ptr = ptr->next){
	    if (ip_address_duplicate(ptr->address, ptr->mask, idb, TRUE)) {
		printf(ip_stay, idb->namestring);
		return(TRUE);
	    }
	}
    }
    return(FALSE);
}

/*
 * ip_slarp_resolve
 * SLARP is the Serial Line Address Resolutio Protocol
 * We use the contents of an HDLC keepalive packet to do IP-specific address
 * resolution on a serial line.  We give the packet back to the caller
 * Called by SERVICE_IP_SLARP
 */

void ip_slarp_resolve (paktype *pak)
{
    paktype *reply;
    struct slarp *ptr;
    idbtype *idb;
    ulong address;
    leveltype level;

    ptr = (struct slarp *)pak->network_start;
    idb = pak->if_input;
    switch (ptr->code) {
      case SLARP_REQUEST:
	if (idb->ip_address) {
	    reply = pak_duplicate(pak);
	    if (reply) {
		hwidbtype *hwidb;

		hwidb = idb->hwptr;
		ptr = (struct slarp *)reply->network_start;
		ptr->code = SLARP_REPLY;
		ptr->t.add.address = idb->ip_address;
		ptr->t.add.mask = idb->ip_nets_mask;
		reply->if_output = idb;
		reply->linktype = LINK_ADDRESS;
		reply->flags |= PAK_PRIORITY;
		if (pak_encap(hwidb, reply, sizeof(struct slarp), 0))
		    datagram_out(reply);
		else
		    retbuffer(reply);
	    }
	}
	break;
      case SLARP_REPLY:
	if (!idb->ip_address) {
	    if (good_address(ptr->t.add.address, ptr->t.add.mask, TRUE)) {
		address = ptr->t.add.address & ~ptr->t.add.mask;
		if ((address < 1) || (address > 2))
		    break;
		address = (address == 1) ? 2 : 1;
		level = raise_interrupt_level(NETS_DISABLE);
		idb->ip_address = 
		    ((ptr->t.add.address & ptr->t.add.mask) | address);
		idb->ip_nets_mask = ptr->t.add.mask;
		idb->ip_resolvehost = ptr->t.add.address;
		idb->resolvemethod = RES_RARP;
		reset_interrupt_level(level);
		errmsg(&msgsym(SLARP, LINK),
		       idb->namestring, idb->ip_address, idb->ip_resolvehost);
		class_resolve(idb);
		if (!nv_valid && !systeminit_flag) {
		    printf("\nSLARP: interface %s resolved.  This ",
			   idb->namestring); 
		    printf("interface has been automatically"); 
		    printf("\nresolved thru the SLARP process.  The ");
		    printf("router will now attempt to retrieve its"); 
		    printf("\nname and configuration file.\n");
		}
	    }
	}
	break;
      default:
	break;
    }
}

/*
 * ip_slarp_sendrequest
 * Send a SLARP address resolution packet
 */

void ip_slarp_sendrequest(idbtype *idb)
{
    paktype *pak;
    struct slarp *ptr;
    hwidbtype *hwidb;

    hwidb = idb->hwptr;
    pak = getbuffer(sizeof(struct slarp));
    if (!pak)
	return;
    ptr = (struct slarp *)pak->network_start;   
    ptr->code = SLARP_REQUEST;
    ptr->t.add.address = 0;
    ptr->t.add.mask = 0;
    pak->if_output = idb;
    pak->linktype = LINK_ADDRESS;
    pak->flags |= PAK_PRIORITY;	/* Put at head of queue */
    if (pak_encap(hwidb, pak, sizeof(struct slarp), 0))
	datagram_out(pak);
    else
	retbuffer(pak);
}

/*
 * ip_ouraddr
 * Get our IP address (this end).
 * Used by BootP and for PPP IPCP negotiations.
 */

ipaddrtype ip_ouraddr (hwidbtype *idb)
{
    idbtype *tempidb;

    /*
     * If we are in a dialer group, use the dialer IDB instead.
     */
     /* Find the ultimate master */
    idb = get_netidb(idb);
    tempidb = idb->firstsw;

    /*
     * We only have an IP address if we have a numbered interface in use.
     */
    if (tempidb->ip_unnumbered)
	return(tempidb->ip_unnumbered->ip_address);
    else if (tempidb->ip_address)
	return(tempidb->ip_address);

    /*
     * Return the IDB addresss of the first interface
     */
    tempidb = SWIDBQ_HEAD;
    return(tempidb->ip_address);
}

/*
 * check_slip_addr()
 *	Figure out if the named address is OK to add as an async address
 */
boolean
check_slip_addr(ipaddrtype target)
{
    register idbtype *idb;
    register arptype *ptr;

    if (martian(target, TRUE))
	 return(FALSE);
    idb = ipaddr_findidb(target);
    if (idb == NULL)
	return(TRUE);
    if (target == idb->ip_address) 	/* Don't let the alias overide */
	return(FALSE);			/*  the interface address */
    /*
     * If not an ethernet or token ring
     * then don't send the ARP.
     *
     * This should be a general encapsulation check.
     */
    if ((idb->hwptr->status & IDB_IEEEMASK) == 0)
	return(TRUE);

    /*
     * Flush the entry if it exists in case we have a user connecting who
     * previously connected to this segment using the same address but to
     * a different destination.
     */
    ip_arp_invalidate(target);

    if (ip_router_running) {
	ptr = ip_arp_table_lookup(idb, target);
	if (ptr == NULL) {
	    process_sleep_for(ONESEC / 2);	/* wait half a second */
	    ptr = ip_arp_table_lookup(idb, target);	/* and retry */
	}
    } else {
	ptr = ip_arp_lookup_and_resolve(idb, target);
    }

    if (ptr != NULL)
	return(FALSE);		/* something else has this address */
    return(TRUE);		/* address is OK */
}

/*
 * autoinstall_process_running
 * This function is used to determine if the autoinstall_over_serial is
 * running.
 */

static boolean autoinstall_process_running (void)
{
    if( autoinstall_pid )
        return TRUE;
    else
        return FALSE; 
}

/* 
 * Define an array of WAN protocols for autoinstall purposes 
 * Only legal one currently is ET_FRAME_RELAY. Aug, 1994.
 * X25 and PPP only have stub files. Also, end the list with 
 * ET_HDLC so that HDLC autoinstall can continue if all else fails.
 */
static const int wan_prot[] = {ET_HDLC, ET_LAPB, ET_X25, ET_PPP, 
                               ET_FRAME_RELAY, ET_SMDS, ET_HDLC, ET_NULL};


/*
 * attempt_autoinstall
 * Start point for attempting autoinstall over WAN links.
 */
void attempt_autoinstall ()
{
    if (!autoinstall_pid) {
	reg_add_autoinstall_terminate(autoinstall_terminate, "autoinstall_terminate");
	reg_add_autoinstall_process_running( autoinstall_process_running, 
					    "autoinstall_process_running" );
	reg_add_autoinstall_reset_serial_encap(autoinstall_reset_serial_encap,
			"autoinstall_reset_serial_encap");
    	autoinstall_pid = cfork((forkproc *)autoinstall_over_serial, 
				0, 0, "Autoinstall", 0);
    }
}
/*
 * 
 * autoinstall_over_serial
 * Support function for autoinstall over WAN links. 
 * Function that attempts to bring up the serial link and if 
 * successful, will send out BOOTP requests and download a 
 * configuration file over that serial link if BOOTP is 
 * successful.
 * 
 * Before we start the process, we need to make sure of two things:
 * That the IDBs are initialized and that the various subsystems are 
 * initialized. look at os/init.c/system_init() and init_process()
 * for details. 
 *
 *
 */
forktype autoinstall_over_serial ()
{
    parseinfo *csb;
    hwidbtype *idb;
    idbtype *swidb;
    int i;

    /* 
     * In order to use existing encapsulation functions 
     * and other utilities, we need the parseinfo structure. 
     */
    csb = malloc(sizeof(parseinfo)); 
    csb_ptr = csb;
    if (!csb) {
	autoinstall_terminate();	/* commit suicide */
    }
    
    /* 
     * Autoinstall is being run on a newly installed unit which is 
     * expected to have an initialized configuration database. 
     */
    FOR_ALL_HWIDBS(idb) {
        i = 0;
	FOR_ALL_SWIDBS_ON_HW(idb, swidb) i++;
    	if (good_address(idb->firstsw->ip_address, idb->firstsw->ip_nets_mask, 
    			(idb->firstsw->ip_nets_mask ? TRUE : FALSE))
	    || ((idb_subif_allowed_here(idb)) && (i > 1))) {
    	    autoinstall_terminate();        /* commit suicide */
    	}
	if (!csb->interface && valid_autoinstall_if(idb)) {
	    csb->interface = idb->firstsw; 
	    csb->interface->hwptr = idb; 
	    idb->enctype_cfg = ET_HDLC;
	    break;
	} 
    }

    if (!csb->interface) {
	autoinstall_terminate();	/* commit suicide */
    }
    idb = csb->interface->hwptr;
    if (idb == NULL) {
	autoinstall_terminate();	/* commit suicide */
    }

    if (good_address(idb->firstsw->ip_address, idb->firstsw->ip_nets_mask, 
			(idb->firstsw->ip_nets_mask ? TRUE : FALSE))) {
	autoinstall_terminate();	/* commit suicide */
    }

    /* 
     * Trial and error to determine the WAN encapsulation type. 
     * For each encapulation type, we must configure the 
     * encapsulation, and "no shut" the interface 
     * before initiating the autoinstall procedure. 
     */
    for (i=0 ; wan_prot[i] != ET_NULL ; i++) {
	csb->sense = TRUE;
	csb->nvgen = FALSE;
	SETOBJ(int,1) = wan_prot[i];
	if (reg_used_media_serial_setup(wan_prot[i])) {
	    encapsulation_command(csb);
	} else {
	    continue;
	}
	csb->sense = FALSE;
	idb->serial_autoinstall = TRUE;
	shutdown_coming_up(idb);

	if (reg_invoke_serial_lineup(wan_prot[i], csb)) {
	    /* 
	     * Since the serial_lineup routine returned success, 
	     * we know that wan_prot[i] is the right encapsulation. 
	     */
	    reg_invoke_serial_autoinstall(wan_prot[i], csb);

	    /* wait for autoinstall to finish */
	    process_wait_on_netsareup(120 * ONESEC);
	    idb->serial_autoinstall = FALSE;

	    /* 
	     * If we did not succeed in resolving an interface, 
	     * lets wait for RARP or SLARP to succeed.
	     */
    	    resolve_interface();
	    /*
	     * If we did succeed with autoinstall, lets kill off the 
	     * setup process that was started by ip_setaddresses(). 
	     *
	     */
	    if (netsareup)
		setup_abort();
	    idb->serial_autoinstall = FALSE;
    	    reg_invoke_autoinstall_complete(idb->enctype, idb);
	    autoinstall_terminate();	/* commit suicide */
	}
	process_suspend();
    }
    /* 
     * If we did not succeed in activating the serial line with 
     * any WAN encapsulation, lets wait for RARP or SLARP to succeed.
     */
    idb->serial_autoinstall = FALSE;
    resolve_interface();
    if (netsareup)
    	setup_abort();
    reg_invoke_autoinstall_complete(idb->enctype, idb);
    autoinstall_terminate();	/* commit suicide */
}
/*
 * resolve_interface
 * try to resolve the address of any of our interfaces
 */
void resolve_interface ()
{
    while (!netsareup) {
	if (address_probe(MAXTRIES, PAUSE_TIME)) /* try probing, very slowly */
	    break;
	/* if never got the file */
	if (netconfig_enable && !hostconfg.responder)
	    if (address_boot())         /* try for it some more */
		break;
    }
}
static void autoinstall_reset_serial_encap (void)
{
    parseinfo *csb;
    if (csb_ptr) {
	csb = csb_ptr;
	csb->sense = TRUE;
	csb->nvgen = FALSE;
	SETOBJ(int,1) = ET_HDLC;
	if (reg_used_media_serial_setup(ET_HDLC))
	    encapsulation_command(csb);
	    if (csb_ptr)
	    	free(csb_ptr);
	    csb_ptr = NULL;
	}
}
static void autoinstall_terminate (void)
{
    if (csb_ptr)
	free(csb_ptr);
    csb_ptr = NULL;
    if (autoinstall_pid) {
	int pid;
	pid = autoinstall_pid;
	autoinstall_pid = 0;
    	process_kill(pid);
    }
}

/*
 * ip_defaultgw
 * Return ip address of the default gateway. Called by other subsystem 
 * through function registration.
 */

ipaddrtype ip_defaultgw ()
{
    return default_gateway;
}


/*
 * ip_extract_addr
 * Extract a source or destination address from the datagram
 */

void ip_extract_addr (paktype *pak, addrtype *dest, enum SRCDST which)
{
    iphdrtype *ip;

    switch (pak->linktype) {
    case LINK_IP:
	ip = iphdrtype_start(pak);
	dest->type = ADDR_IP;
	dest->ip_addr = (which == SOURCE) ? ip->srcadr : ip->dstadr;
	break;
    default:
	dest->type = ADDR_ILLEGAL;
	break;
    }
}


/*
 * ip_match_address
 *
 * Match two IP addresses.
 */
boolean ip_match_address (addrtype *addr1, addrtype *addr2)
{
    if (addr1->type != addr2->type)
        return(FALSE);

    return(addr1->ip_addr == addr2->ip_addr);
}

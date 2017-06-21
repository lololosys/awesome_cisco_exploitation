/* $Id: vines_test.c,v 3.9.4.4 1996/08/07 09:04:08 snyder Exp $
 * $Source: /release/112/cvs/Xsys/vines/vines_test.c,v $
 *------------------------------------------------------------------
 * vines_test.c -- Banyan Vines test and debugging code
 *
 * February 1990, David R. Hamtpon
 *
 * Copyright (c) 1990-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: vines_test.c,v $
 * Revision 3.9.4.4  1996/08/07  09:04:08  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.9.4.3  1996/07/23  13:30:31  thille
 * CSCdi63329: Centralize logging for getbuffer() and pak_duplicate()
 * failures
 * Branch: California_branch
 * Shamelessly rip off Mr. Katz' code from CSCdi36037.  Provide
 * centralized logging for getbuffer() and pak_duplicate() failures and
 * then rip out the 9 bazillion different messages that where in the
 * various callers to report the failure.
 *
 * Revision 3.9.4.2  1996/05/21  10:08:34  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.9.4.1  1996/03/18  22:32:41  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.7.2.4  1996/03/16  07:53:12  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.7.2.3  1996/03/13  02:05:12  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.7.2.2  1996/03/07  11:05:34  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.7.2.1  1996/02/20  21:47:25  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.9  1996/03/09  22:58:20  widmer
 * CSCdi47890:  HTTP server doesnt work
 * Add HTTP server
 *
 * Revision 3.8  1996/02/01  06:11:34  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.7  1996/01/22  07:45:04  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.6  1995/12/17  18:40:49  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.5  1995/12/01  21:39:34  slin
 * CSCdi44836:  Vines doesnt use idb subblocks
 *
 * Revision 3.4  1995/12/01  15:57:18  widmer
 * CSCdi44724:  Remove nv_top argument to parser_add_mode()
 * Clean up "no" handling at beginning of modes
 * Fix appn modes
 * Removed unneded pdecl's
 *
 * Revision 3.3  1995/11/17  17:59:05  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:48:24  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:45:32  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/06/28  09:32:33  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.3  1995/06/16  04:30:18  dkatz
 * CSCdi35882:  Time-of-day code needs cleanup
 *
 * Revision 2.2  1995/06/09  13:25:06  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 23:15:31  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include <string.h>
#include <ctype.h>
#undef   toupper			/* use library function call */
#include <ciscolib.h>
#include "registry.h"
#include "subsys.h"
#include "interface_private.h"
#include "packet.h"
#include "ttysrv.h"
#include "../iprouting/route.h"
#include "address.h"
#include "vines_private.h"
#include "vines_ipc.h"
#include "vines_netrpc.h"
#include "vines_services.h"
#include "vsvc_ss.h"
#include "vsvc_ss.rpch"
#include "vsvc_st.h"
#include "vsvc_st.rpch"
#include "../if/network.h"
#include "ieee.h"
#include "../if/rif.h"
#include "config.h"
#include "parser.h"

#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "parser_defs_vines.h"
#include "../parser/parser_defs_parser.h"

/*
 * VINES subsystem header
 */

#define VINESTEST_MAJVERSION 1
#define VINESTEST_MINVERSION 0
#define VINESTEST_EDITVERSION  1

SUBSYS_HEADER(vinestest, VINESTEST_MAJVERSION, VINESTEST_MINVERSION,
	      VINESTEST_EDITVERSION,
	      vines_test_init, SUBSYS_CLASS_PROTOCOL,
	      "seq: vines", "req: vines");

/*
 * Vines TEST commands
 */
#define ALTERNATE NONE
#include        "exec_test_vines.h"
LINK_POINT      (vines_test_commands, ALTERNATE);
#undef  ALTERNATE

/*
 * Local variables
 */
static paktype *vines_test_last_send = NULL;

static int vtest_data1, vtest_data2, vtest_data3, vtest_data4;
static int vtest_data5, vtest_data6, vtest_data7, vtest_data8;
static int vtest_data9, vtest_data10;

/*
 * Local definitions
 */
#define BUFLEN 100
#define nodetype(x)       ((x == 1) ? VRTP_NODE_ROUTER : VRTP_NODE_CLIENT)
#define controllertype(x) ((x == 1) ? VRTP_CONT_MULTI : VRTP_CONT_SINGLE)
#define machinetype(x)    ((x == 1) ? VRTP_MACH_FAST : VRTP_MACH_SLOW)


/*
 *
 */
static vinesidbtype *vinest_ask_interface (void)
{
    char *p, buff[BUFLEN];
    idbtype *swidb;

    printf("\nInterface: ");
    if (!rdtty(buff,BUFLEN))
	return(NULL);
    p = deblank(buff);
    (void) parse_interface(p, &swidb);
    if (swidb == NULL)
	return(NULL);
    return(vines_getidb(swidb));
}

/*
 *
 */
static int vinest_read_raw_data (uchar *data, int length)
{
    char *p, buff[BUFLEN];

    if (data == NULL)
	return(length);

    while (TRUE) {
	printf("data: ");
	if(!rdtty(buff,BUFLEN))
	    break;
	p = buff;
	if (null(p))
	    break;
	while (!null(p)) {
	    *data++ = parse_hex_long(p) & 0xff;
	    p = findarg(p);
	    length++;
	}
    }
    return(length);
}

/*
 * vinest_buildtables
 *
 * Create four hundres entries in both the vines neighbor table and the
 * vines routing table.  This lets us see the effects of large sets.
 */
static void vinest_buildtables (parseinfo *csb)
{
    int i, j, nets, neighbors, start, current;
    vinesroutetype *route;
    vinesneighbortype *gw;
    vinesnettype *server;
    vinesidbtype *idb;

    nets = GETOBJ(int,1);
    neighbors = GETOBJ(int,3);
    start = current = GETOBJ(int,2);
    idb = vines_getidb(GETOBJ(idb,1));
    if (idb == NULL) {
	printf("\n %%Vines has never been enabled on that interface.");
	return;
    }

    for (i = 0; i < nets; i += neighbors) {
	for (j = 1; (j <= neighbors) && ((i+j) <= nets); j++, current++) {
	    printf(".");
	    if ((i+j) % 80 == 0)
		printf("\n");
	    gw = vines_add_neighbor(start + j, 1, V_N_TEST, idb->vines_metric,
				    VRTP_VERSION, VRTP_NODE_ROUTER);
	    vines_add_path(gw, V_N_TEST, idb, ET_NULL, NULL, idb->vines_metric,
			   VRTP_VERSION, VRTP_NODE_ROUTER);
	    server = vines_add_server(current, V_NET_TEST);
	    route = vines_add_route(server, gw, V_NET_TEST, idb->vines_metric);
	    if (route) {
		route->origin_timestamp = 0;
		route->local_timestamp = vines_sequence + 1;
	    }
	}
	process_may_suspend();
    }
}

/*
 * vinest_cksum
 *
 * Create a canned vines packet and run it through the checksum routine.
 * This allows us to determine if the Vines checksum algorithm is working
 * properly.
 */
void vinest_cksum (void)
{
    paktype *pak;
    vinesiptype *vip;
    vrtp_type *rtp;
    ulong *ptr;

    pak = getbuffer(VINES_MAXSIZE);
    if (!pak) {
	return;
    }
    vip = (vinesiptype *)pak->datagramstart;

    /*
     * test packet 1
     */
    pak->datagramsize = 0x16;
    vip->cksum = 0;
    vip->len = 0x16;
    vip->tc = 0;
    vip->protocol = VINES_RTP;
    PUT2WD(vip->ddstnet, VINES_ALLNETS);
    vip->dsthost = VINES_ALLHOSTS;
    PUT2WD(vip->ssrcnet, 0x0020128BL);
    vip->srchost = 0x820e;
    rtp = (vrtp_type *)vip->data;
    rtp->op = VRTP_UPDATE;
    rtp->node = VRTP_NODE_CLIENT;
    rtp->controller = VRTP_CONT_SINGLE;
    rtp->machine = VRTP_MACH_FAST;
    vines_crc(pak);
    printf("\nVINES: the following packet should have checksum 0x98D6");
    print_vinespak(pak);
    
    /*
     * test packet 2
     */
    pak->datagramsize = 0x35;
    vip->cksum = 0;
    vip->len = 0x35;
    vip->tc = 0x70;
    vip->protocol = VINES_IPC;
    PUT2WD(vip->ddstnet, VINES_ALLNETS);
    vip->dsthost = VINES_ALLHOSTS;
    PUT2WD(vip->ssrcnet, 0xf800ad40L);
    vip->srchost = 0x8000;

    ptr = (ulong *)vip->data;
    *ptr++ = 0x02000006L;
    *ptr++ = 0x00000004L;
    *ptr++ = 0x00039c72L;
    *ptr++ = 0x9c720000L;
    *ptr++ = 0x00009c72L;
    *ptr++ = 0x00009c72L;
    *ptr++ = 0x07444f53L;
    *ptr++ = 0x5c555341L;
    *ptr++ = 0x000000BBL;
    vines_crc(pak);
    printf("\n\nVINES: the following packet should have checksum 0x0f7e");
    print_vinespak(pak);

    datagram_done(pak);
}

static boolean vinest_ask_first_hop (
    paktype *pak,
    vinespathtype **path)
{
    vinesidbtype *idb;
    routingdecision thewaythere;
    char *p, buff[BUFLEN];
    vinesiptype *vip = (vinesiptype *)pak->network_start;
    ulong net = GET2WD(vip->ddstnet);
    ushort host = vip->dsthost;
    boolean ok;

    memset(&thewaythere, 0, sizeof(routingdecision));
    ok = vines_first_hop(net, host, &thewaythere);
    if (ok) {
	printf("\nSend via %#z? ", thewaythere.neighbor->net,
	       thewaythere.neighbor->host);
	(void)rdtty(buff,BUFLEN);
	if (null(buff))
	    return(FALSE);
	p = deblank(buff);
	if (toupper(*p) != 'Y')
	    ok = FALSE;
    }
    if (!ok) {
	printf("\nSend via: ");
	(void)rdtty(buff,BUFLEN);
	if (!null(buff)) {
	    if (!parse_vinesaddress(buff, &net, &host)) {
		printf("Invalid address.");
	    } else {
		vines_unlock_nodes(&thewaythere);
		ok = vines_first_hop(net, host, &thewaythere);
	    }
	}
    }
    if (!ok) {
	idb = vinest_ask_interface();
	if (!idb)
	    return(FALSE);
	pak->if_output = idb->phys_idb;
	vines_unlock_nodes(&thewaythere);
	*path = NULL;
	return(TRUE);
    }

    pak->if_output = thewaythere.path->idb->phys_idb;
    *path = thewaythere.path;
    vneigh_ProtectNeighbor(thewaythere.neighbor);
    vines_unlock_nodes(&thewaythere);
    return(TRUE);
}

/*
 *
 */
static void vinest_generate (parseinfo *csb)
{
    vinesidbtype *idb;

    switch (GETOBJ(int,1)) {
      case VINES_GEN_FULL:
	FOR_ALL_VIDBS(idb) {
	    if (!vines_interface_up(idb))
		continue;
	    vsrtp_send_update(VSRTP_FULL_UPDATE, idb, NULL);
	}
	break;
      case VINES_GEN_CHANGE:
	FOR_ALL_VIDBS(idb) {
	    if (!vines_interface_up(idb))
		continue;
	    vsrtp_send_update(VSRTP_CHANGE_UPDATE, idb, NULL);
	}
	break;
      case VINES_GEN_NULL:
	FOR_ALL_VIDBS(idb) {
	    if (!vines_interface_up(idb))
		continue;
	    vsrtp_send_update(VSRTP_NULL_UPDATE, idb, NULL);
	}
	break;
    }
}

static boolean vinest_send_once (
    paktype *pak,
    idbtype *swidb,
    char *text,
    uchar *macaddr)
{
    uchar savedmac1[IEEEBYTES], savedmac2[IEEEBYTES];
    vinespathtype *path = NULL;
    vinesiptype *vip;
    hwidbtype *hwidb;
    leveltype level;
    addrtype addr;
    boolean send;
    int i;

    /*
     * Find the first hop, or ask for an interface.
     */
    if (swidb == NULL) {
	swidb = pak->if_output;
	if (swidb == NULL) {
	    if (!vinest_ask_first_hop(pak, &path))
		return(FALSE);
	    swidb = path->idb->phys_idb;
	}
    }
    if (swidb == NULL)
	return(FALSE);
    if (!vines_interface_up(vines_getidb(swidb))) {
	printf("\nNot running VINES on %s.", swidb->namestring);
	return(FALSE);
    }
    hwidb = swidb->hwptr;

    vip = (vinesiptype *)pak->network_start;
    vines_crc(pak);
    pak->datagramsize = vip->len;
    pak->if_output = swidb;
    pak->linktype = LINK_VINES;
    pak->acct_proto = ACCT_PROTO_VINES;
    if (is_x25(swidb->hwptr)) {
	addr.type = LINK_VINES;
	addr.length = ADDRLEN_VINES;
	addr.addr.vines_address.net = GETLONG(vip->ddstnet);
	addr.addr.vines_address.host = vip->dsthost;
	send = (*hwidb->vencap)(pak, (long)&addr);
    } else if (macaddr) {
	level = raise_interrupt_level(ALL_DISABLE);
	ieee_copy(hwidb->hardware, savedmac1);
	ieee_copy(hwidb->bit_swapped_hardware, savedmac2);
	ieee_copy(macaddr, hwidb->hardware);
	for (i = 0; i < IEEEBYTES; i++)
	    hwidb->bit_swapped_hardware[i] = bit_swaps[macaddr[i]];
	send = (*hwidb->vencap)(pak, (long)path);
	ieee_copy(savedmac1, hwidb->hardware);
	ieee_copy(savedmac2, hwidb->bit_swapped_hardware);
	reset_interrupt_level(level);
    } else {
	send = (*hwidb->vencap)(pak, (long)path);
    }

    if (send) {
	if (path)
	    vneigh_UnprotectNeighbor(path->neighbor);
	if (vinespkt_debug)
	    vinesbug(pak, "sent");
	pak_lock(pak);
	datagram_out(pak);
	return(TRUE);
    } 
    if (path)
	vneigh_UnprotectNeighbor(path->neighbor);
    printf("\n%s encapsulation failed", text);
    return(FALSE);
}

static boolean vinest_send_many (
    paktype *pak,
    char *text,
    int limit,
    uchar *macaddr)
{
    uchar savedmac1[IEEEBYTES], savedmac2[IEEEBYTES];
    uchar tempmac[IEEEBYTES];
    vinesiptype *vip;
    ulong srcnet;
    leveltype level;
    addrtype addr;
    boolean send;
    hwidbtype *hwidb;
    idbtype *swidb;
    int count, i;

    swidb = pak->if_output;
    if (swidb == NULL)
	return(FALSE);
    hwidb = swidb->hwptr;
    if (hwidb == NULL)
	return(FALSE);
    if (!vines_interface_up(vines_getidb(swidb))) {
	printf("\nNot running VINES on %s.", swidb->namestring);
	return(FALSE);
    }

    vip = (vinesiptype *)pak->network_start;
    pak->datagramsize = vip->len;
    srcnet = GETLONG(vip->ssrcnet);
    for (count = 0; count < limit; count++) {
	PUTLONG(vip->ssrcnet, srcnet + count);
	vines_crc(pak);
	if (is_x25(swidb->hwptr)) {
	    addr.type = LINK_VINES;
	    addr.length = ADDRLEN_VINES;
	    addr.addr.vines_address.net = GETLONG(vip->ddstnet);
	    addr.addr.vines_address.host = vip->dsthost;
	    send = (*hwidb->vencap)(pak, (long)&addr);
	} else if (macaddr) {
	    level = raise_interrupt_level(ALL_DISABLE);
	    ieee_copy(hwidb->hardware, savedmac1);
	    ieee_copy(hwidb->bit_swapped_hardware, savedmac2);
	    ieee_copy(macaddr, tempmac);
	    PUTLONG(&tempmac[2], GETLONG(&tempmac[2]) + count);
	    ieee_copy(tempmac, hwidb->hardware);
	    for (i = 0; i < IEEEBYTES; i++)
		hwidb->bit_swapped_hardware[i] = bit_swaps[tempmac[i]];
	    send = pak_encap(pak->if_output->hwptr, pak, vip->len, 0);
	    ieee_copy(savedmac1, hwidb->hardware);
	    ieee_copy(savedmac2, hwidb->bit_swapped_hardware);
	    reset_interrupt_level(level);
	} else {
	    send = pak_encap(pak->if_output->hwptr, pak, vip->len, 0);
	}
	if (!send) {
	    /*
	     * Encapsulation failed for some reason.
	     */
	    printf("\nEncapsulation failed on hello %d", count);
	    return(FALSE);
	}
	pak_lock(pak);
	datagram_out(pak);
    }
    printf("\n %d %s packets sent", count, text);
    return(TRUE);
}

static inline vinesiptype *vinest_build_ip (
    paktype *pak,
    int length,
    char tc,
    char protocol,
    ulong dstnet,
    ushort dsthost,
    ulong srcnet,
    ulong srchost)
{
    vinesiptype *vip;

    vip = (vinesiptype *)pak->network_start;
    memset(vip, 0, sizeof(vinesiptype) + length);
    vip->cksum = VINES_NOCHECKSUM;
    vip->len = sizeof(vinesiptype) + length;
    vip->tc = tc;
    vip->protocol = protocol;
    PUT2WD(vip->ddstnet, dstnet);
    vip->dsthost = dsthost;
    PUT2WD(vip->ssrcnet, srcnet);
    vip->srchost = srchost;

    pak->linktype = LINK_VINES;
    pak->acct_proto = ACCT_PROTO_VINES;
    pak->datagramsize = vip->len;
    return(vip);
}

/*
 * vinest_send_arp
 */
boolean vinest_send_arp (parseinfo *csb, paktype *pak)
{
    vinesiptype *vip;
    varp_header *arp;
    ulong dstnet, srcnet;
    ushort dsthost, srchost;
    int arpcode = GETOBJ(int,2);

    if (arpcode == VARP_ASS_REQ) {
	dstnet = GETOBJ(paddr,1)->vines_addr.net;
	dsthost = GETOBJ(paddr,1)->vines_addr.host;
    } else {
	dstnet = VINES_ALLNETS;
	dsthost = VINES_ALLHOSTS;
    }
    if ((arpcode == VARP_SVC_REQ) || (arpcode == VARP_ASS_REQ)) {
	srcnet = VINES_UNKNOWNNET;
	srchost = VINES_UNKNOWNHOST;
    } else {
	srcnet = vines_network;
	srchost = vines_host;
    }

    vip = vinest_build_ip(pak, sizeof(varp_type), 0, VINES_ARP, dstnet,
			  dsthost, srcnet, srchost);
    arp = (varp_header *)vip->data;
    arp->op = GETOBJ(int,2);
    if (arpcode == VARP_ASS_RESP) {
	arp->net = GETOBJ(paddr,1)->vines_addr.net;
	arp->host = GETOBJ(paddr,1)->vines_addr.host;
    }

    return(vinest_send_once(pak, GETOBJ(idb,1), "ARP", NULL));
}

/*
 * vinest_send_echo
 */
boolean vinest_send_echo (parseinfo *csb, paktype *pak)
{
    memset(pak->network_start, 0, 50);
    pak->if_output = GETOBJ(idb,1);    
    pak->linktype = LINK_VINES_ECHO;
    pak->acct_proto = ACCT_PROTO_VINES;
    pak->datagramsize = 50;
    if ((*pak->if_output->hwptr->vencap)(pak, 0)) {
	pak_lock(pak);
    	datagram_out(pak);
	printf("\nEcho packet sent");
	return(TRUE);
    }

    /*
     * Encapsulation failed for some reason.
     */
    printf("\nEcho encapsulation failed");
    return(FALSE);
}

/*
 * vinest_send_icp
 */
boolean vinest_send_icp (parseinfo *csb, paktype *pak)
{
    vinesiptype *vip;
    vinesicptype *icp;
    vinesneighbortype *gw;
    vinespathtype *path;
    ulong net = GETOBJ(paddr,1)->vines_addr.net;
    ushort host = GETOBJ(paddr,1)->vines_addr.host;

    gw = vneigh_FindEntry(net, host);
    path = vines_get_path(gw);
    if (!gw || !path) {
	printf("\n%z: No such neighbor or no available path", net,host);
	return(FALSE);
    }

    vip = vinest_build_ip(pak, sizeof(vinesicptype)+40, 0, VINES_IPC, net, host,
			  GETOBJ(paddr,2)->vines_addr.net,
			  GETOBJ(paddr,2)->vines_addr.host);

    icp = (vinesicptype *)vip->data;
    icp->type = GETOBJ(int,2);
    icp->info = GETOBJ(int,3);
    /* Building the old IP packet should go here. */

    return(vinest_send_once(pak, path->idb->phys_idb, "ICP", NULL));
}

/*
 * vinest_send_loop
 */
boolean vinest_send_loop (parseinfo *csb, paktype *pak)
{
    vinesiptype *vip;

    vip = (vinesiptype *)pak->network_start;
    memset(vip, 0, 50);
    vip->cksum = VINES_NOCHECKSUM;

    pak->if_output = GETOBJ(idb,1);    
    pak->linktype = LINK_VINES_LOOP;
    pak->datagramsize = 50;
    if ((*pak->if_output->hwptr->vencap)(pak, 0)) {
    	pak_lock(pak);
	datagram_out(pak);
	printf("\nLoop packet sent");
	return(TRUE);
    }

    /*
     * Encapsulation failed for some reason.
     */
    printf("\nLoop encapsulation failed");
    return(FALSE);
}

/**********************************************************************
 *
 *                     Build RTP messages
 *
 **********************************************************************/

static inline vrtp_type *vinest_build_rtp (
    vinesiptype *vip,
    ushort op)
{
    vrtp_type *rtp = (vrtp_type *)vip->data;

    rtp->op = op;
    rtp->node = nodetype(vip->srchost);
    rtp->controller = controllertype(vip->srchost);
    rtp->machine = machinetype(vip->srchost);
    return(rtp);
}

/*
 * vinest_send_rtp_hello
 */
static boolean vinest_send_rtp_hello (parseinfo *csb, paktype *pak)
{
    vinesiptype *vip;
    vrtp_type *rtp;
    uchar *newmac;

    vip = vinest_build_ip(pak, sizeof(vrtp_type), 0, VINES_RTP, VINES_ALLNETS,
			  VINES_ALLHOSTS, GETOBJ(paddr,1)->vines_addr.net,
			  GETOBJ(paddr,1)->vines_addr.host);
    rtp = vinest_build_rtp(vip, VRTP_UPDATE);

    pak->if_output = GETOBJ(idb,1);
    newmac = GETOBJ(int,2) ? (uchar *)(*GETOBJ(hwaddr,1)).addr : NULL;
    return(vinest_send_many(pak, "RTP Hello", GETOBJ(int,4), newmac));
}

/*
 * vinest_send_rtp_update
 */
static boolean vinest_send_rtp_update (parseinfo *csb, paktype *pak)
{
    vinesiptype *vip;
    vrtp_type *rtp;
    uchar *newmac;
    int length;

    if (GETOBJ(int,4) != 1) {
	return(vinest_send_rtp_hello(csb, pak));
    }

    vip = vinest_build_ip(pak, sizeof(vrtp_type) + 40, 0, VINES_RTP, VINES_ALLNETS,
			  VINES_ALLHOSTS, GETOBJ(paddr,1)->vines_addr.net,
			  GETOBJ(paddr,1)->vines_addr.host);
    rtp = vinest_build_rtp(vip, VRTP_UPDATE);
    length = vinest_read_raw_data(rtp->data, 40);
    vip->len = sizeof(vrtp_type) + length;
	
    pak->if_output = GETOBJ(idb,1);
    newmac = GETOBJ(int,2) ? (uchar *)(*GETOBJ(hwaddr,1)).addr : NULL;
    return(vinest_send_once(pak, pak->if_output, "RTP Hello", newmac));
}

/*
 * vinest_send_rtp_redirect
 */
static boolean vinest_send_rtp_redirect (parseinfo *csb, paktype *pak)
{
    vinesiptype *vip;
    vrtp_type *rtp;
    vrtp_redirecttype *redirect;
    vinesneighbortype *gw;
    vinespathtype *path;
    ulong net = GETOBJ(paddr,1)->vines_addr.net;
    ushort host = GETOBJ(paddr,1)->vines_addr.host;
    int i;

    gw = vneigh_FindEntry(net, host);
    path = vines_get_path(gw);
    if (!gw || !path) {
	printf("\n%z: No such neighbor or no available path", net,host);
	return(FALSE);
    }

    vip = vinest_build_ip(pak, sizeof(vrtp_type) + sizeof(vrtp_redirecttype) +
			  (GETOBJ(int,5) * 2),  0, VINES_RTP,
			  GETOBJ(paddr,1)->vines_addr.net,
			  GETOBJ(paddr,1)->vines_addr.host,
			  GETOBJ(paddr,2)->vines_addr.net,
			  GETOBJ(paddr,2)->vines_addr.host);
    rtp = vinest_build_rtp(vip, VRTP_REDIRECT);

    redirect = (vrtp_redirecttype *)rtp->data;
    redirect->macaddrlen = IEEEBYTES;
    redirect->riflen = GETOBJ(int,5) * 2;
    PUT2WD(&redirect->dstnet, GETOBJ(paddr,3)->vines_addr.net);
    redirect->dsthost = GETOBJ(paddr,3)->vines_addr.host;
    redirect->dstmetric = GETOBJ(int,3);
    redirect->dstnode = nodetype(redirect->dsthost);
    redirect->dstcontroller = controllertype(redirect->dsthost);
    redirect->dstmachine = machinetype(redirect->dsthost);
    PUT2WD(&redirect->nbrnet, GETOBJ(paddr,4)->vines_addr.net);
    redirect->nbrhost = GETOBJ(paddr,4)->vines_addr.host;
    redirect->nbrmetric = GETOBJ(int,4);
    redirect->nbrnode = nodetype(redirect->nbrhost);
    redirect->nbrcontroller = controllertype(redirect->nbrhost);
    redirect->nbrmachine = machinetype(redirect->nbrhost);
    ieee_copy((uchar *)(*GETOBJ(hwaddr,1)).addr, redirect->mac);
    if (GETOBJ(int,5)) {
	for (i = 0; i < GETOBJ(int,5); i++)
	    ((ushort *)redirect->rif)[i] = (csb->uiarray[i] & 0xFFFF);
    }

    return(vinest_send_once(pak, path->idb->phys_idb, "RTP", NULL));
}

static boolean vinest_send_rtp (parseinfo *csb, paktype *pak)
{
    boolean sent = FALSE;
    switch (GETOBJ(int,2)) {
      case VRTP_UPDATE:
	sent = vinest_send_rtp_update(csb, pak);
	break;
      case VRTP_REDIRECT:
	sent = vinest_send_rtp_redirect(csb, pak);
	break;
      default:
	printf("Sending that type of RTP packet is not supported yet.");
	sent = FALSE;
    }
    return(sent);
}

/**********************************************************************
 *
 *                       Build SRTP messages
 *
 **********************************************************************/

static inline vsrtp_header *vinest_build_srtp (
    vinesiptype *vip,
    ushort op)
{
    vsrtp_header *srtp = (vsrtp_header *)vip->data;

    srtp->version = VSRTP_VERSION;
    srtp->operation = op;
    srtp->node_type = nodetype(vip->srchost);
    srtp->flags = VSRTP_FLAG_MISMATCH;
    srtp->reserved = 0;
    return(srtp);
}

/*
 * vinest_build_srtp_request
 */
static void vinest_build_srtp_request (parseinfo *csb, paktype *pak)
{
    vinesiptype *vip;
    vsrtp_header *srtp;
    vsrtp_request *request;

    vip = vinest_build_ip(pak, VSRTP_HEADER_BYTES + VSRTP_REQUEST_BYTES +
			  sizeof(ulong), 0, VINES_RTP,
			  GETOBJ(paddr,1)->vines_addr.net,
			  GETOBJ(paddr,1)->vines_addr.host,
			  vines_network, vines_host);
    srtp = vinest_build_srtp(vip, GETOBJ(int,2));

    request = (vsrtp_request *)srtp->data;
    request->type = GETOBJ(int,3);
    request->reserved = 0;
    request->data[0] = GETOBJ(int,5) ? GETOBJ(int,4) : 0;
}

static void vinest_build_srtp_reinit (parseinfo *csb, paktype *pak)
{
    vinesiptype *vip;
    vsrtp_header *srtp;

    vip = vinest_build_ip(pak, VSRTP_HEADER_BYTES, 0, VINES_RTP,
			  GETOBJ(paddr,1)->vines_addr.net,
			  GETOBJ(paddr,1)->vines_addr.host,
			  vines_network, vines_host);
    srtp = vinest_build_srtp(vip, VSRTP_REINIT);
}

/*
 * vinest_build_srtp_redirect
 */
static boolean vinest_build_srtp_redirect (parseinfo *csb, paktype *pak)
{
    vinesiptype *vip;
    vsrtp_header *srtp;
    vsrtp_redirect *redirect;
    vinesneighbortype *gw;
    vinespathtype *path;
    ulong net = GETOBJ(paddr,1)->vines_addr.net;
    ushort host = GETOBJ(paddr,1)->vines_addr.host;
    int i;

    gw = vneigh_FindEntry(net, host);
    path = vines_get_path(gw);
    if (!gw || !path) {
	printf("\n%z: No such neighbor or no available path", net,host);
	return(FALSE);
    }

    vip = vinest_build_ip(pak, VSRTP_HEADER_BYTES + VSRTP_REDIRECT_BYTES +
			  GETOBJ(int,5) * 2, 0, VINES_RTP,
			  GETOBJ(paddr,1)->vines_addr.net,
			  GETOBJ(paddr,1)->vines_addr.host,
			  vines_network, vines_host);
    srtp = vinest_build_srtp(vip, GETOBJ(int,2));

    redirect = (vsrtp_redirect *)srtp->data;
    redirect->nbr_macaddrlen = IEEEBYTES;
    redirect->nbr_riflen = GETOBJ(int,5) * 2;
    PUT2WD(&redirect->dst_net, GETOBJ(paddr,3)->vines_addr.net);
    redirect->dst_host = GETOBJ(paddr,3)->vines_addr.host;
    redirect->dst_metric = GETOBJ(int,3);
    redirect->dst_node = nodetype(redirect->dst_host);
    redirect->dst_flags = GETOBJ(int,9);
    PUT2WD(&redirect->dst_sequence, GETOBJ(int,10));
    PUT2WD(&redirect->nbr_net, GETOBJ(paddr,4)->vines_addr.net);
    redirect->nbr_host = GETOBJ(paddr,4)->vines_addr.host;
    redirect->nbr_metric = GETOBJ(int,4);
    redirect->nbr_node = nodetype(redirect->nbr_host);
    redirect->nbr_flags = GETOBJ(int,7);
    PUT2WD(redirect->nbr_sequence, GETOBJ(int,8));
    ieee_copy((uchar *)(*GETOBJ(hwaddr,1)).addr, redirect->nbr_mac);
    if (GETOBJ(int,5)) {
	for (i = 0; i < GETOBJ(int,5); i++)
	    ((ushort *)redirect->nbr_rif)[i] = (csb->uiarray[i] & 0xFFFF);
    }
    return(TRUE);
}

static boolean vinest_send_srtp (parseinfo *csb, paktype *pak)
{
    switch (GETOBJ(int,2)) {
      case VSRTP_REQUEST:
	vinest_build_srtp_request(csb, pak);
	break;

      case VSRTP_REINIT:
	vinest_build_srtp_reinit(csb, pak);
	break;

      case VSRTP_REDIRECT:
	if (!vinest_build_srtp_redirect(csb, pak)) {
	    printf("Couldn't build redirect.");
	    return(FALSE);
	}
	break;

      default:
	printf("This SRTP type is not supported yet.");
	return(FALSE);
    }

    return(vinest_send_once(pak, NULL, "SRTP", NULL));
}

/*
 *
 */
void vinest_build_ipc (paktype *pak)
{
    vinesiptype *vip;
    vinesnetrpctype *netrpc;
    vinesipctype *ipc;
    vinesipc2type *ipc2;
    uchar *data;
    int length;

    vip = (vinesiptype *)pak->network_start;
    vip->len = sizeof(vinesiptype) + sizeof(vinesipctype);
    vip->tc = (ushort)get_num_in_range("tc", 16, 0, 0xff, 0x1f);
    
    ipc = (vinesipctype *)vip->data;
    ipc2 = (vinesipc2type *)vip->data;
    printf("\n\nIPC parameters:");
    ipc->srcport   = (ushort)get_num_in_range("   src port", 16, 0, 0xffff, 0x666);
    ipc->dstport   = (ushort)get_num_in_range("   dst port", 16, 0, 0xffff, 0x1);
    ipc->type      =  (uchar)get_num_in_range("       type", 16, 0, 0xff,   0x1);
    ipc->control   =  (uchar)get_num_in_range("    control", 16, 0, 0xff,   0x60);
    if (ipc->type != 0) {
	ipc->local_id  = (ushort)get_num_in_range("src conn id", 16, 0, 0xffff, 0x666);
	ipc->remote_id = (ushort)get_num_in_range("dst conn id", 16, 0, 0xffff, 0x0);
	ipc->sequence  = (ushort)get_num_in_range("   sequence", 16, 0, 0xffff, 0x1);
	ipc->ack       = (ushort)get_num_in_range("        ack", 16, 0, 0xffff, 0x0);
    }
    if (vip->protocol == VINES_SPP)
	ipc->length= (ushort)get_num_in_range("     window", 16, 0, 0xffff, 0x0);
    else if (vip->protocol == VINES_IPC) {
	if (ipc->type != 0) {
	    netrpc = (vinesnetrpctype *)ipc->data;
	} else {
	    netrpc = (vinesnetrpctype *)ipc2->data;
	}
	printf("\n\nnetrpc");
	netrpc->m.call.type = (ushort)get_num_in_range("type", 16, 0, 0xffff, 0x0);
	
	switch (netrpc->m.call.type) {
	  case 0:
	    netrpc->m.call.id = (ushort)get_num_in_range("id", 16, 0, 0xffff, 0);
	    netrpc->m.call.program = get_num_in_range("program", 16, 0, 0xffffffffL, 0);
	    netrpc->m.call.version = (ushort)get_num_in_range("version", 16, 0, 0xffff, 1);
	    netrpc->m.call.procedure = (ushort)get_num_in_range("procedure", 16, 0, 0xffff, 0x12);
	    data = netrpc->m.call.data;
	    length = 12;
	    break;
	  default:
	    netrpc->m.retn.id = (ushort)get_num_in_range("id", 16, 0, 0xffff, 0);
	    data = netrpc->m.retn.data;
	    length = 4;
	    break;
	}
	
	length = vinest_read_raw_data(data, length);
	ipc->length = length;
	vip->len += ipc->length;
    }
}

/*
 * vinest_send_blast
 */
static boolean vinest_send_blast (parseinfo *csb, paktype *pak)
{
    vinesiptype *vip;
    ulong dstnet, srcnet;
    ushort dsthost, srchost;
    uchar *macaddr;
    boolean sent;
    int count;

    dstnet = GETOBJ(paddr,1)->vines_addr.net;
    dsthost = GETOBJ(paddr,1)->vines_addr.host;
    if (GETOBJ(int,2)) {
	srcnet = GETOBJ(paddr,2)->vines_addr.net;
	srchost = GETOBJ(paddr,2)->vines_addr.host;
    } else {
	srcnet = vines_network;
	srchost = vines_host;
    }
    vip = vinest_build_ip(pak, 50, 0, VINES_IPC, dstnet, dsthost, srcnet, srchost);
    vinest_build_ipc(pak);

    macaddr = GETOBJ(int,3) ? (uchar *)(*GETOBJ(hwaddr,1)).addr : NULL;
    sent = vinest_send_many(pak, "IPC Blast", 1, macaddr);
    if (!sent)
	return(FALSE);

    for (count = 0; count < GETOBJ(int,9); count++) {
	PUT2WD(vip->ddstnet, dstnet + count);
	pak_lock(pak);
	datagram_out(pak);
    }
    printf("\n%d IPC packets sent", count);
    return(TRUE);
}

/*
 * vinest_send_ipc
 */
boolean vinest_send_ipc_spp (parseinfo *csb, paktype *pak, int type)
{
    vinesiptype *vip;
    ulong dstnet, srcnet;
    ushort dsthost, srchost;
    uchar *macaddr;

    dstnet = GETOBJ(paddr,1)->vines_addr.net;
    dsthost = GETOBJ(paddr,1)->vines_addr.host;
    if (GETOBJ(int,2)) {
	srcnet = GETOBJ(paddr,2)->vines_addr.net;
	srchost = GETOBJ(paddr,2)->vines_addr.host;
    } else {
	srcnet = vines_network;
	srchost = vines_host;
    }
    vip = vinest_build_ip(pak, 100, 0, type, dstnet, dsthost, srcnet, srchost);
    vinest_build_ipc(pak);

    macaddr = GETOBJ(int,3) ? (uchar *)(*GETOBJ(hwaddr,1)).addr : NULL;
    return(vinest_send_once(pak, NULL, "IPC Packet", macaddr));
}

boolean vinest_send_ipc (parseinfo *csb, paktype *pak)
{
    return(vinest_send_ipc_spp(csb, pak, VINES_IPC));
}

boolean vinest_send_spp (parseinfo *csb, paktype *pak)
{
    return(vinest_send_ipc_spp(csb, pak, VINES_SPP));
}

/*
 * vinest_send_netrpc
 */
static boolean vinest_send_netrpc (parseinfo *csb, paktype *pak)
{
    vinesipcsocket *socket;
    vinesidbtype *idb;
    vinesnetrpctype *rpc;
    ulong dnet = GETOBJ(paddr,1)->vines_addr.net;
    ushort dhost = GETOBJ(paddr,1)->vines_addr.host;
    int length;
    paktype *newpak;
    uchar *data;

    newpak = vrpc_getbuffer(GETOBJ(int,4), 100);
/*
 * BOGOSITY ALERT!!!  The below code mungs with newpak without ever
 * having checked that getbuffer returned a valid packet...
 */
    rpc = (vinesnetrpctype *)newpak->session_start;
    data = NULL;
    switch (GETOBJ(int,4)) {
      case VRPC_REJECT:
      case VRPC_ABORT:
	rpc->m.reject.reason = GETOBJ(int,5);
	break;
      case VRPC_SEARCH:
      case VRPC_SEARCHALL:
	rpc->m.search.min_s = GETOBJ(int,5);
	rpc->m.search.max_s = GETOBJ(int,6);
	rpc->m.search.min_l = GETOBJ(int,5);
	rpc->m.search.max_l = GETOBJ(int,6);
	break;
      case VRPC_RETURNADDRESS:
	rpc->m.retnaddress.service_rev_s = GETOBJ(int,5);
	rpc->m.retnaddress.service_rev_l = GETOBJ(int,5);
	break;
      case VRPC_CALL:
	rpc->m.call.procedure = GETOBJ(int,5);
	data = rpc->m.call.data;
	break;
      case VRPC_RETURN:
	data = rpc->m.retn.data;
	break;
      default:
	datagram_done(newpak);
	return(FALSE);
    }
    length = vinest_read_raw_data(data, 0);
    newpak->datagramsize = vrpc_frame_lengths[GETOBJ(int,4)] + length;
    newpak->tcp_flags = newpak->datagramsize;

    if ((dnet == VINES_ALLNETS) || (dhost == VINES_ALLHOSTS)) {
	idb = vinest_ask_interface();
	if (idb == NULL) {
	    datagram_done(newpak);
	    return(FALSE);
	}
	pak_lock(newpak);
	vipc_send_broadcast_datagram(newpak, idb, NULL, GETOBJ(int,2),
				     GETOBJ(int,3), 0, 0);
    } else {
	socket = vipc_create_conn(dnet, dhost, 0, 0, TRUE);
	if (socket == NULL) {
	    datagram_done(newpak);
	    return(FALSE);
	}
	pak_lock(newpak);
	idb = vipc_send_data(socket, newpak, GETOBJ(int,2), GETOBJ(int,3),
			     VINES_HOPS);
	if (idb == NULL) {
	    printf("\nvipc_send_data failed");
	    datagram_done(newpak);
	    return(FALSE);
	}
    }

    pak_copy(newpak, pak, pak->datagramsize);
    datagram_done(newpak);
    return(TRUE);
}

/*
 * vinest_send_service
 */
static boolean vinest_send_service (parseinfo *csb, paktype *pak)
{
    vinesipcsocket *socket;
    vinesidbtype *idb;
    ulong dnet = GETOBJ(paddr,1)->vines_addr.net;
    ushort dhost = GETOBJ(paddr,1)->vines_addr.host;
    char *data;
    paktype *newpak;

    newpak = vrpc_getbuffer(VRPC_CALL, 100);
    data = newpak->application_start;
    newpak->datagramsize = vinest_read_raw_data(data, 0);
    newpak->tcp_flags = vrpc_frame_lengths[VRPC_CALL]+newpak->datagramsize;

    if (vinestest_debug) {
	buginf("\nVTEST: sending call %d to %#z", GETOBJ(int,4), dnet, dhost);
	vsvc_dump(&vsvc_st_info, "Query", newpak->application_start,
		  newpak->datagramsize);
    }

    if ((dnet == VINES_ALLNETS) || (dhost == VINES_ALLHOSTS)) {
	idb = vinest_ask_interface();
	if (idb == NULL) {
	    datagram_done(newpak);
	    return(FALSE);
	}
	pak_lock(newpak);
	vrpc_send_broadcast_call(newpak, idb, NULL, 0, 1, GETOBJ(int,4),
				  GETOBJ(int,2), GETOBJ(int,3), 0, 0);
    } else {
	socket = vipc_create_conn(dnet, dhost, 0, 0, TRUE);
	if (socket == NULL) {
	    datagram_done(newpak);
	    return(FALSE);
	}
	pak_lock(newpak);
	vrpc_send_long_call(socket, newpak, 0, 1, GETOBJ(int,4),
				  GETOBJ(int,2), GETOBJ(int,3));
    }

    pak_copy(newpak, pak, pak->datagramsize);
    datagram_done(newpak);
    return(TRUE);
}

/*
 *
 */
static const char ip_types[] = "\nType [1-IPC, 2-SPP, 4-ARP, 5-RTP, 6-ICP]";
static const char arp_ops[] = "Operation [0-Srv_Req, 1-Srv_Rep, 2-Add_Req, 3-Add_Rep]";
static const char rtp_ops[] = "Operation [1-Request, 4-Update, 5-Response, 6-Redirect]";
static const char srtp_ops[] = "Operation [1-Request, 2-Update, 3-Redirect, 4-ReInit]";

boolean vinest_buildpak (paktype *pak)
{
    vinesiptype *vip;
    varp_header *arp;
    vrtp_type *rtp;
    vinesicptype *icp;
    char *p, buff[BUFLEN];
    ulong network, i;
    vsrtp_header *srtp;
    vsrtp_request *request;
    vsrtp_update *update;

    memset(pak->data_area, 0, ENCAPBYTES);

    vip = (vinesiptype *)pak->network_start;
    printf("Default: ");
    if(!rdtty(buff,BUFLEN))
	return(FALSE);
    p = deblank(buff);
    if (p && (*p == 'Y' || *p == 'y')) {
	ulong *q;

	q = (ulong *)pak->network_start;
	*q++ = 0xffff0000;
	*q++ = 0x4001ffff;
	*q++ = 0xffffffff;
	*q++ = 0x30004395;
	*q++ = 0x00010217;
	*q++ = 0x000f002c;
	*q++ = 0x00000000;
	*q++ = 0x00000000;
	*q++ = 0x00010023;
	*q++ = 0x00020003;
	*q++ = 0x30004395;
	*q++ = 0x0001000f;
	*q++ = 0x00000000;
	*q++ = 0x00000000;
	*q++ = 0x50d60001;
	*q++ = 0x0000007f;
	*q++ = 0x512f6152;
	*q++ = 0x52306283;
	*q++ = 0x00010007;
	*q++ = 0x53657276;
	*q++ = 0x65727300;
	*q++ = 0x00054675;
	*q++ = 0x62617200;
	*q++ = 0x00020106;
	*q++ = 0x52306283;
	vip->len = (uchar *)q - pak->network_start;
	pak->linktype = LINK_VINES;
	return(TRUE);
    }

    vip->cksum = 0;
    vip->tc = 0;
    pak->linktype = LINK_VINES;

    vip->protocol = get_num_in_range(ip_types, 16, 0, 0xffff, 0x1);

    printf("Destination: ");
    if(!rdtty(buff,BUFLEN))
	return(FALSE);
    p = deblank(buff);
    if (!null(p)) {
	(void)parse_vinesaddress(p, &network, &vip->dsthost);
	PUT2WD(vip->ddstnet, network);
    } else {
	PUT2WD(vip->ddstnet, vines_network);
	vip->dsthost = VINES_SERVER;
    }

    printf("Source: ");
    if(!rdtty(buff,BUFLEN))
	return(FALSE);
    p = deblank(buff);
    if (!null(p)) {
	(void)parse_vinesaddress(p, &network, &vip->srchost);
	PUT2WD(vip->ssrcnet, network);
    } else {
	PUT2WD(vip->ssrcnet, vines_network);
	vip->srchost = VINES_SERVER;
    }

    switch (vip->protocol) {
    case VINES_IPC:
    case VINES_SPP:
	vinest_build_ipc(pak);
	break;
    case VINES_ARP:
	vip->len = sizeof(vinesiptype) + sizeof(varp_header);
	vip->tc = (ushort)get_num_in_range("tc", 16, 0, 0xff, 0x1f);
	arp = (varp_header *)vip->data;
	arp->op = get_num_in_range(arp_ops, 16, 0, 0xffff, 0);
	if (arp->op == VARP_ASS_RESP) {
	    printf("Address: ");
	    if (!rdtty(buff,BUFLEN))
		return(FALSE);
	    (void) parse_vinesaddress(buff, &arp->net, &arp->host);
	} else {
	    arp->net = VINES_UNKNOWNNET;
	    arp->host = VINES_UNKNOWNHOST;
	}
	break;
    case VINES_RTP:
	i = get_num_in_range("0: RTP, 1:SRTP", 10, 0, 1, 1);
	if (i == 0) {
	    vip->len = sizeof(vinesiptype) + sizeof(vrtp_type);
	    vip->tc = (ushort)get_num_in_range("tc", 16, 0, 0xff, 0x1f);
	    rtp = (vrtp_type *)vip->data;
	    rtp->op = get_num_in_range(rtp_ops, 16, 0, 0xffff, 4);
	    rtp->node = VRTP_NODE_ROUTER;
	    rtp->controller = VRTP_CONT_MULTI;
	    rtp->machine = VRTP_MACH_FAST;
	} else {
	    vip->len = sizeof(vinesiptype) + sizeof(vsrtp_header);
	    vip->tc = (ushort)get_num_in_range("tc", 16, 0, 0xff, 0x1f);
	    srtp = (vsrtp_header *)vip->data;
	    srtp->version = VSRTP_VERSION;
	    srtp->operation = get_num_in_range(srtp_ops, 16, 0, 4, 1);
	    srtp->node_type = VSRTP_NODE_ROUTER;
	    srtp->flags = 0;
	    srtp->reserved = 0;
	    
	    if (srtp->operation == VSRTP_REQUEST) {
		vip->len += sizeof(vsrtp_request);
		request = (vsrtp_request *)srtp->data;
		request->type = get_num_in_range("type", 16, 0, 0xff, 1);
		if (request->type <= 2) {
		    vip->len += sizeof(ulong);
		    *((ulong *)request->data) = get_num_in_range("data", 16, 0, 0xffffffff, 1);
		}
	    } else if (srtp->operation == VSRTP_UPDATE) {
		vip->len += sizeof(vsrtp_update);
		update = (vsrtp_update *)srtp->data;
		update->type = get_num_in_range("type", 16, 0, 0xff, 1);
		update->flags = get_num_in_range("flags", 16, 0, 0xff, 3);
		update->packet_id = get_num_in_range("id", 16, 0, 0xffff, 0);
		update->data_offset = get_num_in_range("offset", 16, 0, 0xffff, 0);
		update->sequence = get_num_in_range("sequence", 16, 0, 0xffffffff, 3);
		update->metric = get_num_in_range("metric", 16, 0, 0xffff, 2);
	    }
   	}
	break;
    case VINES_ICP:
	vip->len = sizeof(vinesiptype) + 20;
	vip->tc = (ushort)get_num_in_range("tc", 16, 0, 0xff, 0x1f);
	icp = (vinesicptype *)vip->data;
	break;
    default:
	vip->len = sizeof(vinesiptype) + 20;
	vip->tc = (ushort)get_num_in_range("tc", 16, 0, 0xff, 0x1f);
    }

    print_vinespak(pak);
    return(TRUE);
}

/*
 *
 */
boolean vinest_send_custom (parseinfo *csb, paktype *pak)
{
    if (!vinest_buildpak(pak))
	return(FALSE);
    return(vinest_send_once(pak, NULL, "Custom", NULL));
}

/*
 *
 */
void vinest_send (parseinfo *csb)
{
    paktype *pak;
    boolean sent = FALSE;

    pak = vines_test_last_send;
    vines_test_last_send = NULL;
    if (!pak) {
	pak = getbuffer(VINES_MAXSIZE);
	if (!pak) {
	    printf("Cannot allocate packet buffer.\n");
	    return;
	}
	pak->if_output = NULL;
    }
    if (GETOBJ(int,1) == VINES_SEND_AGAIN) {
	if (pak->if_output) {
	    pak_lock(pak);
	    datagram_out(pak);
	    vines_test_last_send = pak;
	    printf("\nSent.");
	} else {
	    datagram_done(pak);
	    printf("\nNo previous packet to send.");
	}
	return;
    }

    pak->if_output = NULL;
    switch (GETOBJ(int,1)) {
      case VINES_SEND_ARP:
	sent = vinest_send_arp(csb, pak);
	break;

      case VINES_SEND_BLAST:
	sent = vinest_send_blast(csb, pak);
	break;

      case VINES_SEND_CUSTOM:
	sent = vinest_send_custom(csb, pak);
	break;

      case VINES_SEND_ECHO:
	sent = vinest_send_echo(csb, pak);
	break;

      case VINES_SEND_END:
	set_mode_byname(&csb->mode, "vines-test", MODE_VERBOSE);
	sent = TRUE;
	break;

      case VINES_SEND_ICP:
	sent = vinest_send_icp(csb, pak);
	break;

      case VINES_SEND_IPC:
	sent = vinest_send_ipc(csb, pak);
	break;

      case VINES_SEND_LOOP:
	sent = vinest_send_loop(csb, pak);
	break;

      case VINES_SEND_MENU:
	set_mode_byname(&csb->mode, "vines-test-send", MODE_VERBOSE);
	break;

      case VINES_SEND_RTP:
	sent = vinest_send_rtp(csb, pak);
	break;

      case VINES_SEND_SRTP:
	sent = vinest_send_srtp(csb, pak);
	break;

      case VINES_SEND_SPP:
	sent = vinest_send_spp(csb, pak);
	break;

      case VINES_SEND_NETRPC:
	sent = vinest_send_netrpc(csb, pak);
	break;

      case VINES_SEND_SERVICE:
	sent = vinest_send_service(csb, pak);
	break;

      default:
	printf("Should be unreachable.  Send case #%d.\n",
	       GETOBJ(int,1));
	break;
    }

    if (sent) {
	if (GETOBJ(int,1) != VINES_SEND_AGAIN)
	    pak->tcpdriver_flags = GETOBJ(int,1);
	vines_test_last_send = pak;
    } else {
	datagram_done(pak);
    }    
}

/*
 *
 */
static void vinest_set (parseinfo *csb)
{
    vinesneighbortype *neighbor;
    vinespathtype *path;

    switch (GETOBJ(int,1)) {
      case VINES_SET_LOCAL:
	vines_sequence = GETOBJ(int,2);
	break;

      case VINES_SET_ORIGIN:
	neighbor = vneigh_FindEntry(GETOBJ(paddr,1)->vines_addr.net,
				    GETOBJ(paddr,1)->vines_addr.host);
	if (!neighbor) {
	    printf("Cannot find neighbor.");
	    return;
	}
	FOR_ALL_PATHS(neighbor, path)
	    path->origin_timestamp = GETOBJ(int,2);
	break;

      case VINES_SET_RC_TIMER:
	neighbor = vneigh_FindEntry(GETOBJ(paddr,1)->vines_addr.net,
				    GETOBJ(paddr,1)->vines_addr.host);
	if (!neighbor) {
	    printf("Cannot find neighbor.");
	    return;
	}
	path = neighbor->best_path;
	mgd_timer_start(&path->rc_timer, (GETOBJ(int,2) * ONESEC));
	break;

      case VINES_SET_REASS_TIMER:
	neighbor = vneigh_FindEntry(GETOBJ(paddr,1)->vines_addr.net,
				    GETOBJ(paddr,1)->vines_addr.host);
	if (!neighbor) {
	    printf("Cannot find neighbor.");
	    return;
	}
	path = neighbor->best_path;
	mgd_timer_start(&path->reassembly_timer, (GETOBJ(int,2) * ONESEC));
	break;

      case VINES_SET_RETRY_TIMER:
	neighbor = vneigh_FindEntry(GETOBJ(paddr,1)->vines_addr.net,
				    GETOBJ(paddr,1)->vines_addr.host);
	if (!neighbor) {
	    printf("Cannot find neighbor.");
	    return;
	}
	path = neighbor->best_path;
	mgd_timer_start(&path->retry_timer, (GETOBJ(int,2) * ONESEC));
	break;

      case VINES_SET_STATE:
	neighbor = vneigh_FindEntry(GETOBJ(paddr,1)->vines_addr.net,
				    GETOBJ(paddr,1)->vines_addr.host);
	if (!neighbor) {
	    printf("Cannot find neighbor.");
	    return;
	}
	FOR_ALL_PATHS(neighbor, path)
	    path->state = GETOBJ(int,2);
	break;
    }
}
/*
 *
 */
void vines_test_ss_command (parseinfo *csb)
{
    ulong seconds;

    switch (GETOBJ(int,1)) {
      case VINES_SS_BUMP:
	vss_set_time_adjustment(12*ONEHOUR/ONESEC);
	vsvc_ss_info.periodic();
	break;

      case VINES_SS_ENDOFTIME:
	seconds = system_uptime_seconds();
	vss_clear_time_adjustment();
	vss_set_time_adjustment(0xFFFFFFFF - 12*ONEHOUR/ONESEC - seconds);
	vsvc_ss_info.periodic();
	break;

      case VINES_SS_PERIODIC:
	vsvc_ss_info.periodic();
	break;

      case VINES_SS_RESET:
	vss_clear_time_adjustment();
	vsvc_ss_info.periodic();
	break;
    }
}

/*
 * vines_test_st_command
 */
void vines_test_st_command (parseinfo *csb)
{
    serverentry *s;
    ulong serverid;

    switch (GETOBJ(int,1)) {
      case VINES_ST_WELCOME:
	vst_set_gen_dest(&vst_dest_welcome, -1L);
	vst_send_ni_welcome();
	break;

      case VINES_ST_SUMMARY:
	vst_send_ni_summary();
	break;

      case VINES_ST_NEED:
	serverid = GETOBJ(paddr,1)->vines_addr.net;
	if (!(s = vsvc_FindServer(serverid))) {
	    s = vsvc_CreateServer(serverid);
	    if (!s)
		return;
	}
	s->st_port.family = VINES_ADDR_NATIVE;
	PUT2WD(s->st_port.f.vip.nnet, serverid);
	s->st_port.f.vip.host = VINES_SERVER;
	s->st_port.f.vip.port = VIPC_PORT_ST;
	s->st_need_info = TRUE;
	vst_set_gen_dest(&vst_dest_need, -1L);
	vst_send_ni_need();
	break;

      case VINES_ST_DETAIL:
	vst_set_gen_dest(&vst_dest_detail, -1L);
	vst_send_ni_detail();
	break;

      case VINES_ST_COLDDUMP:
	serverid = GETOBJ(paddr,1)->vines_addr.net;
	vst_cold_dump_send_call(serverid, NULL);
	break;

      case VINES_ST_ADD:
	vst_create_local_group(GETOBJ(string,1), GETOBJ(string,2),
			       vines_network, GETOBJ(int,2), TRUE);
	break;

      case VINES_ST_DELETE:
	vst_delete_local_group(GETOBJ(string,1), GETOBJ(string,2),
			       vines_network, TRUE);
	break;
    }
}

/*
 *
 */
void vines_test_command (parseinfo *csb)
{
    vinesnettype *server;
    vinesneighbortype *neighbor;

    switch (csb->which) {
      case VINES_TEST_BUILD:
	vinest_buildtables(csb);
	break;

      case VINES_TEST_CHECKSUM:
	vinest_cksum();
	break;

      case VINES_TEST_DATA:
	vtest_data1  = GETOBJ(int,1);
	vtest_data2  = GETOBJ(int,2);
	vtest_data3  = GETOBJ(int,3);
	vtest_data4  = GETOBJ(int,4);
	vtest_data5  = GETOBJ(int,5);
	vtest_data6  = GETOBJ(int,6);
	vtest_data7  = GETOBJ(int,7);
	vtest_data8  = GETOBJ(int,8);
	vtest_data9  = GETOBJ(int,9);
	vtest_data10 = GETOBJ(int,10);
	break;

      case VINES_TEST_END:
	if (vines_test_last_send) {
	    datagram_done(vines_test_last_send);
	    vines_test_last_send = NULL;
	}    
	csb->mode = exec_mode;
	break;

      case VINES_TEST_FLUSH:
	vines_ager(TRUE);
	neighbor = vines_add_neighbor(vines_network, vines_host, V_N_ME, 0,
				      VSRTP_VERSION, VSRTP_NODE_ROUTER);
	vines_add_path(neighbor, V_N_ME, vinesnullidb, ET_NULL, NULL, 0,
		       VSRTP_VERSION, VSRTP_NODE_ROUTER);
	server = vines_add_server(vines_network, V_NET_ME);
	vines_add_route(server, neighbor, V_NET_ME, 0);
	break;

      case VINES_TEST_GENERATE:
	vinest_generate(csb);
	break;

      case VINES_TEST_SEND:
	vinest_send(csb);
	break;

      case VINES_TEST_SET:
	vinest_set(csb);
	break;

      default:
	printf("Should be unreachable.  Test case #%d.\n", csb->which);
	break;
    }
}

void vines_test (parseinfo *csb)
{
    set_mode_byname(&csb->mode, "vines-test", MODE_VERBOSE);
}


void vines_test_init (subsystype *subsys)
{
    parser_add_commands(PARSE_ADD_TEST_CMD,&pname(vines_test_commands),
			"vines");
    parser_add_mode("vines-test", "vines test", "Vines test mode",
		    FALSE, FALSE, NULL, NULL, NULL,
		    &pname(top_test_vines),
		    NULL);
    parser_add_mode("vines-test-send", "vines test:send",
		    "Vines test send mode",
		    FALSE, FALSE, NULL, NULL, NULL,
		    &pname(vt_send_top),
		    NULL);
}

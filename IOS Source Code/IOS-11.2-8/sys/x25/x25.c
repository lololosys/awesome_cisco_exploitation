/* $Id: x25.c,v 3.16.6.14 1996/09/11 23:24:56 swenson Exp $
 * $Source: /release/112/cvs/Xsys/x25/x25.c,v $
 *------------------------------------------------------------------
 * x25.c -- X25 Packet Level (3)
 *
 * January 1987, Greg Satz
 *
 * Copyright (c) 1987-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: x25.c,v $
 * Revision 3.16.6.14  1996/09/11  23:24:56  swenson
 * CSCdi63831:  x25 facility code not encoded properly
 * Branch: California_branch
 * add facilities length field to switched call connected
 * packets with an address block if we are a DCE
 *
 * Revision 3.16.6.13  1996/08/28  13:23:19  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.16.6.12  1996/07/26  20:09:46  atsao
 * CSCdi55843:  Telnets across X.25 bridging fail
 * Branch: California_branch
 * Broadcast the IP host bridge datagram if its swidb matches with
 * the one in the bridge map entry of X25 mapping table.
 *
 * Revision 3.16.6.11  1996/07/23  13:32:35  thille
 * CSCdi63329: Centralize logging for getbuffer() and pak_duplicate()
 * failures
 * Branch: California_branch
 * Shamelessly rip off Mr. Katz' code from CSCdi36037.  Provide
 * centralized logging for getbuffer() and pak_duplicate() failures and
 * then rip out the 9 bazillion different messages that where in the
 * various callers to report the failure.
 *
 * Revision 3.16.6.10  1996/07/12  23:02:36  swenson
 * CSCdi61933:  pvc-setup can be delayed indefinitely
 * save remaining time on pvc connect timer if interrupted
 * Branch: California_branch
 *
 * Revision 3.16.6.9  1996/06/28  23:32:34  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.16.6.8  1996/06/27  09:47:51  fox
 * CSCdi61238:  linktype is an overloaded symbol
 * Branch: California_branch
 * Change idb->linktype to idb->getlink.
 *
 * Revision 3.16.6.7  1996/05/23  07:38:44  myeung
 * CSCdi58029:  OSPF crash in ospf_interface_up() when unconfiguring OSPF
 * Branch: California_branch
 * - Add boolean parameter to ip_address_change registry to indicate if
 *   the change is for secondary or primary address.
 *
 * Revision 3.16.6.6  1996/05/17  12:17:10  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.15.2.4  1996/05/06  00:10:49  ppearce
 * Sync to IbuMod_Calif_baseline_960504
 *
 * Revision 3.15.2.3  1996/04/06  01:06:10  pmorton
 * CSCdi45577:  QLLC w/PVCs regression with QLLC in CLS.
 * Build a map for managing PVCs, so that they can be claimed either by
 * CLS or by RSRB
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.15.2.2  1996/04/03  22:17:45  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.15.2.1  1996/03/17  18:55:20  ppearce
 * Sync IbuMod_Calif_branch to V111_1_3
 *
 * Revision 3.16.6.5  1996/05/10  21:54:43  swenson
 * CSCdi55124:  x25 pvc connection fails to re-establish
 * Branch: California_branch
 *
 * Revision 3.16.6.4  1996/04/27  06:46:02  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 3.6.10.3  1996/04/26  00:04:36  syiu
 * Branch: Dial1_branch
 * Dial1_branch to IosSec_postmerge_960419
 *
 * Revision 3.6.10.2  1996/04/17  00:04:35  syiu
 * Branch: Dial1_branch
 * Sync Dial1_branch to V111_1_3
 *
 * Revision 3.6.10.1  1996/02/23  21:32:45  lbustini
 * Branch: Dial1_branch
 * Move dialer fields out of the idb.
 *
 * Revision 3.16.6.3  1996/04/15  21:25:21  bcole
 * CSCdi54649:  IP Host Modularity, IP Automatic Stub Routing feature
 * commit
 * Branch: California_branch
 *
 * Revision 3.16.6.2  1996/04/02  17:29:26  swenson
 * CSCdi52548:  x.25 pvc stays in waiting to connect state
 * Branch: California_branch
 *
 * Revision 3.16.6.1  1996/03/18  22:49:55  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.9.2.4  1996/03/16  07:57:28  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.9.2.3  1996/03/13  02:13:22  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.9.2.2  1996/03/07  11:19:14  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.9.2.1  1996/02/20  21:55:38  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.16  1996/03/06  05:02:45  carvalho
 * CSCdi50043:  Appletalk ping fails for datagram size of 128 over x.25
 * Clear M-bit when appending last packet of M-bit sequence to existing
 * buffer.
 *
 * Revision 3.15  1996/02/23  23:21:29  carvalho
 * CSCdi41580:  Calling Address Ext. facility modified while X25 switching
 * Save digit count/authority field for called and calling address
 * extension facilities for re-use when call is forwarded.
 *
 * Revision 3.14  1996/02/15  22:17:27  gglick
 * CSCdi41923:  X25 interface stuck in RNRsent: Input queue wedged at
 *              97/100
 *         Perform some intermediate X.25 data reassembly to reduce the
 *         number of buffers held for reassembly.
 *
 * Revision 3.13  1996/02/15  02:15:06  gglick
 * CSCdi45199:  X.25 PVC command lost upon reload.
 *         Make sure X.25 doesn't come up before system initialization is
 *         complete.
 *
 * Revision 3.12  1996/02/02  17:06:59  atsao
 * CSCdi46647:  Update N1 when MTU, X25 modulo or LAPB compression is
 * changed
 * Incorporate code review comment.
 *
 * Revision 3.11  1996/02/01  06:13:03  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.10  1996/01/25  11:23:50  smackie
 * Registry code gets some much-needed hot-oil massaging, free liposuction
 * and a makeover.  (CSCdi47101)
 *
 *    o Compress registry structures. Saves over 120K for all platforms
 *    o Add registry subsystems and remove feature registry initializion
 *      from registry.c to fix woeful initialization scaling problems
 *    o Remove unused and seemingly unloved registry debugging code
 *    o Add registry memory statistics to shame people into action
 *
 * Revision 3.9  1996/01/22  07:46:38  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.8  1996/01/20  23:49:43  gglick
 * CSCdi46861:  X.25 restart not sent after T10/T20 expiry
 *
 * Revision 3.7  1996/01/15  04:48:06  atsao
 * CSCdi46647:  Update N1 when MTU, X25 modulo or LAPB compression is
 * changed
 * Dynamically update N1 value whenever there is configuration change on
 * hardware MTU, LAPB/X25 modulo, or lapb compression.
 *
 * Revision 3.6  1995/12/06  18:59:02  atsao
 * CSCdi44889:  ip host mode not working over X.25
 * Check if tbridge_on_swidb is set to take care of the corner case where
 * no ip routing is enabled and no bridge-group is configured.
 *
 * Revision 3.5  1995/11/27  08:44:22  atsao
 * CSCdi36544:  bridged IP to router doesnt work over X.25
 * ARP/IP cannot be bridged over X.25 when we are not routing IP and we
 * are the destination IP host bridge of these packets.
 *
 * Revision 3.4  1995/11/20  22:29:15  sdurham
 * CSCdi42520:  IF MIB breaks modularity in oddball images
 *        fixed IFMIB subsystem, and moved linkUp/Down trap code to IFMIB.
 *
 * Revision 3.3  1995/11/17  18:07:05  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:51:53  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:53:15  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.19  1995/11/08  21:37:24  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.16  1995/10/02  22:01:48  carvalho
 * Revision 2.18  1995/10/16  22:22:57  gglick
 * CSCdi40956:  agsb crashes while configuring x25 encap on ser
 *
 * Revision 2.17  1995/10/04  02:04:43  hampton
 * X.25 needs to reset the idb oqueue vector when x25 encapsulation is
 * removed.  Not doing so causes an interaction with autoinstall and the
 * set_default_queuing routine that yields an 85% failure rate on the IGS
 * regression tests.  [CSCdi41491]
 *
 * Revision 2.16  1995/10/02 22:01:48  carvalho
 * CSCdi39783:  CMNS call to ethernet destination fails if LLC2 not open.
 *
 * Revision 2.15  1995/09/23  21:27:46  gglick
 * CSCdi40746:  X.25 and LAPB not working
 *
 * Revision 2.14  1995/09/09  07:59:48  misko
 * CSCdi39602:  when x25 ips/ops not match, no RESET REQUEST seen
 * Add back in line deleted in previous merge.
 *
 * Revision 2.13  1995/09/07  00:21:16  pmorton
 * CSCdi38709:  CMNS generates X25 packets over ethernet 3 bytes too short
 * Register LINK_CMNS for media_x25_macaddr
 *
 * Revision 2.12  1995/09/06  00:15:09  atsao
 * CSCdi38131:  x25 spurious access in 10.3 and 11.0
 * Check if lci is NULL for requeued packets from the driver.
 *
 * Revision 2.11  1995/09/02  08:34:50  pmorton
 * CSCdi38709:  CMNS generates X25 packets over ethernet 3 bytes too short
 *
 * x25_prpack.  Just use a char pointer instead of allocating a string on
 * the stack
 *
 * x25_encaps.  Use payload size for the packet length to pass in to llc2
 * _encapsulate
 *
 * x25_process_data/x25_forward_data.  Pass LINK_CMNS packets to
 * x25_forward_data.  Remove bogus pak->datagramstart/pak->datagramsize
 * computations from x25_forward_data.
 *
 * x25_llc2_clsproc. Remove naughty word from buginf.
 *
 * Revision 2.10  1995/08/30  19:52:10  rbatz
 * CSCdi30085:  SDLLC over RSRB/FST over X.25 crashes router due to stack
 *              overflow. Made 'fakepak' static instead of automatic.
 *
 * Revision 2.9  1995/08/23  20:02:28  dino
 * CSCdi39186:  CMNS disables IP multicast fastswitching.
 *
 * Revision 2.8  1995/08/18  21:43:03  lbustini
 * CSCdi37190:  X.25 encapsulation over BRI interface is failing
 * Fix queueing vectors setup.
 *
 * Revision 2.7  1995/08/17  07:12:14  ccpoe
 * CSCdi38328:  srt registry is not created if tbridge subsys doesn't
 * exist. Check srt registry's presence before do a
 * reg_add_srt_learn_source.
 *
 * Revision 2.6  1995/08/10  15:30:22  gglick
 * CSCdi38546:  LAPB can delay sending an X.25 packet
 *
 * Revision 2.5  1995/07/20  07:38:10  hampton
 * Convert X.25 to fully use the passive timers macros instead of
 * referencing the system clock directly.  [CSCdi37392]
 *
 * Revision 2.4  1995/07/07 03:54:40  atsao
 * CSCdi34427:  x25 pvc can't use 2048 packet size after reload
 * X.25 should never overwrite the hardware MTU value in x25_setup().
 *
 * Revision 2.3  1995/06/20  19:59:17  pmorton
 * CSCdi35117:  X.25 restarts ignored over CMNS connection
 * Correct length input to llc2_send
 *
 * Revision 2.2  1995/06/17  06:27:01  ahh
 * CSCdi35734:  TCP: Long overdue code reorganization
 * Separate tcp.h into public and private header files.  Shuffle functions
 * around into more logical groupings.  Rename functions to meet naming
 * conventions (say goodbye to StudlyCaps).  Delete unneeded externs.
 * Trim off old log messages.
 *
 * Revision 2.1  1995/06/07  23:21:46  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <string.h>
#include <ciscolib.h>
#include "sys_registry.h"
#include "media_registry.h"
#include "subsys.h"
#include "interface_private.h"
#include "packet.h"
#include "address.h"
#include "config.h"
#include "exception.h"
#include "../ui/debug.h"
#include "../if/ether.h"
#include "../ip/ip.h"
#include "../ip/ip_registry.h"
#include "../tcp/tcpcompress.h"
#include "../tcp/tcp_debug.h"
#include "x25.h"
#include "x25_address.h"
#include "x25_proto.h"
#include "x25_bfe.h"
#include "x25_switch.h"
#include "x25_route.h"
#include "x25_debug.h"
#include "x25_tcp.h"
#include "x25_registry.h"
#include "../x25/x25_registry.regc"
#include "lapb.h"
#include "connect.h"
#include "pad.h"
#include "../pt/translate.h"
#include "../if/network.h"
#include "../if/network_debug.h"
#include "logger.h"
#include "../srt/span.h"
#include "../srt/srt_registry.h"
#include "parser_defs_x25.h"
#include "compress.h"
#include "../snmp/snmp_api.h"
#include "../snmp/ifmib_registry.h"
#include "x25_pbp.h"
#include "../wan/dialer.h"
#include "../wan/dialer_registry.h"
#include "../llc2/llc2.h"
#include "x25_event.h"
#include "../if/linkdown_event.h"
#include "../if/msg_datalink.c"		/* Not a typo, see logger.h */
#include "../x25/msg_x25.c"		/* Not a typo, see logger.h */

/*
 *  macro for determining the address block length from its length field
 */
#define X25_ABLOCK_LEN(lengths)	(1 +					\
				 ((lengths & 0x0f) +			\
				  ((lengths >> 4) & 0x0f) + 1) / 2)

int x25_rf;				/* retransmission process' pid */
long x25_regcode;			/* X.25 service registration id */
queuetype x25iQ;			/* Input queue */
queuetype x25aQ;			/* alternate route queue */
queuetype x25tQ;			/* Timer queue */
queuetype x25rpQ;			/* RPOA queue */
queuetype x25LocalAckQ;			/* PBP compression local ack queue */
queuetype x25idbQ;				/* idbs running x25 */

boolean x25_called_from_api;

#ifdef cpluscplus
stateproc *x25_state_table[X25_NSTATES] = {
#else
void (*x25_state_table[X25_NSTATES])(hwidbtype *, paktype *, lcitype *) = {
#endif
	x25_r1, x25_r2_r3, x25_r2_r3, x25_p1, x25_p2_p3,
	x25_p2_p3, x25_p4, x25_p5, x25_p6_p7, x25_p6_p7,
	x25_p4, x25_p4, x25_p4
};


/*
 * X25 subsystem header
 */

#define X25_MAJVERSION 1
#define X25_MINVERSION 99
#define X25_EDITVERSION  99

SUBSYS_HEADER(x25, X25_MAJVERSION, X25_MINVERSION, X25_EDITVERSION,
	      x25_init, SUBSYS_CLASS_PROTOCOL,
	      "seq: tbridge",
	      "req: compress");

static lcitype *x25_place_call(idbtype *idb, addrtype *proto_addr, 
        uchar *subaddr, uchar prec,int protocol, uchar *cud, uint cudlen);
static int x25_reassemble(paktype **pakptr, queuetype *reassmQ);

/*
 * x25_showproto
 * Interface specific portion of "show protocols" commands
 * Called by SERVICE_SHOWPROTO
 */

static void x25_showproto (idbtype *idb)
{
    if (idb == NULL) {
	if (x25_routing)
	    printf("\n  X.25 routing is enabled");
	return;
    }
    if (RUNNING_IF(LINK_X25, idb)) {
	/* no interface specific information printed */
    }
}

/* defaults for case registry services */

/* address_map_enter */

static boolean x25_default_address_map_enter (x25map *map)
{
    return(TRUE);
}

/* lci_busy */

static void x25_default_lci_busy(addrtype *proto_addr,boolean *result)
{
    *result = FALSE;
}


/*
 * x25_sprintf_address
 *
 * Take an addrtype data structure and print it as an X.121 address.
 */
static void x25_sprintf_address (
    char *string,
    addrtype *addr)
{
    sprintf(string, "%s", addr->x121_addr);
}

/*
 * x25_srt_learn_source
 */
boolean x25_srt_learn_source (
    idbtype *idb,
    paktype *pak,
    btetype *mac)
{
    lcitype *lci;

    if (!is_x25(idb->hwptr))
	return(FALSE);
    lci = pak->x25_cb;
    mac->bte_proxy_addr.x25map_ptr = lci->lci_map;
    return(TRUE);
}

static void x25_encaps_nvram_write (parseinfo *csb)
{
    idbtype *swidb = csb->interface;

    nv_write(TRUE, "encapsulation x25");

    nv_add(ISDCE(swidb->hwptr, NULL), " dce");
    nv_add(ISBFE(swidb->hwptr), " bfe");
    nv_add(IS_NON_BFE_DDN(swidb->hwptr), " ddn");
    nv_add(swidb->hwptr->x25_cud_pid_def == X25_CUD_PID_IETF, " ietf");
}
/*
 * x25_print_version
 */
static void x25_print_version (void)
{
    /*
     *      DO NOT CHANGE THE FOLLOWING X.25 VERSION IDENTIFICATION
     *  our certifications are keyed to this identification and any change
     *  will mean large commitments of time and money for recertification
     */
    printf("X.25 software, Version 2.0, NET2, BFE and GOSIP compliant.\n");
}

/*
 *  routine for PVC connection timer--
 *  leftover value if the timer was interrupted,
 *  short timer if the PVC has a reasonable chance of connecting,
 *  long timer if the other end shut the PVC or the PVC timed out,
 *  otherwise no timer (one or both configurations need to be changed)
 */
boolean x25_set_pvc_con_time (lcitype *lci)
{
    if (lci->lci_swtype == X25_ROUTE_IF) {
	TIMER_STOP(lci->lci_timer);
    } else if (lci->lci_pvc_remainder) {
	TIMER_START(lci->lci_timer, lci->lci_pvc_remainder);
    } else if (lci->lci_pvc_status <= X25_PVC_LAST_SHORT_TMR) {
	TIMER_START(lci->lci_timer, 2*ONESEC);
    } else if (lci->lci_pvc_status <= X25_PVC_LAST_LONG_TMR) {
	TIMER_START(lci->lci_timer, 5*ONEMIN);
    } else {
	TIMER_STOP(lci->lci_timer);
    }
    lci->lci_pvc_remainder = 0;
    return(TIMER_RUNNING(lci->lci_timer));
}

/*
 * x25_set_dialer
 * Perform X.25 specific actions when an X.25 interface is configured for DDR.
 */
static void x25_set_dialer (dialerdbtype *ddb, boolean setup)
{
    if (setup) {
	ddb->dialer_max_call = DIALER_MAX_X25_CALLS;
    }
}

/*
 * Initialize X25
 */

void x25_init (subsystype *subsys)
{
    lapb_init();
    x25_debug_init();

    x25_rf = 0;

    queue_init(&x25iQ, 0);
    queue_init(&x25aQ, 0);
    queue_init(&x25tQ, 0);
    queue_init(&x25rpQ, 0);
    queue_init(&x25LocalAckQ, 0);
    queue_init(&x25idbQ, 0);
    x25_lci_hash_init();
    x25_map_hash_init();
    x25_bfe_hash_init();
    x25_route_init();
    x25_service_init();

    /*
     * Register some functions
     */
    reg_add_sprintf_address_link(LINK_X25,
				 x25_sprintf_address, "x25_sprintf_address");
    reg_add_clear_counters(x25_clearcounters, "x25_clearcounters");
    reg_add_send_quench(LINK_X25SERVICE, x25_send_quench, "x25_send_quench");
    reg_add_send_quench(LINK_X25, x25_send_quench, "x25_send_quench");
    reg_add_send_quench(LINK_PAD, x25_send_quench, "x25_send_quench");
    reg_add_swif_erase(x25_map_delete_subint, "x25_map_delete_subint");
    reg_add_showproto(x25_showproto, "x25_showproto");
    reg_add_media_x25_macaddr(LINK_X25, x25_extract_lci, "x25_extract_lci");
    reg_add_media_x25_macaddr(LINK_X25TCP, x25_extract_lci, "x25_extract_lci");
    reg_add_media_x25_macaddr(LINK_CMNS, x25_extract_lci, "x25_extract_lci");
    reg_add_media_llc_show(LINK_CMNS, show_llc2_cons, "show_llc2_cons");
    if (x25_enable) {
	reg_add_media_serial_setup(ET_X25, x25_setup, "x25_setup");

	reg_add_media_x25_encaps(x25_encaps, "x25_encaps");
	reg_add_media_x25_place_call(x25_place_call, "x25_place_call");
	reg_add_media_x25_output(x25_output, "x25_output");
	reg_add_media_x25_sendcause(x25_sendcause, "x25_sendcause");
	reg_add_media_x25_reassemble(x25_reassemble, "x25_reassemble");
	reg_add_default_media_x25_address_map_enter(
		x25_default_address_map_enter,
		"x25_default_address_map_enter");
 	reg_add_default_media_x25_lci_busy(x25_default_lci_busy,
		"x25_default_lci_busy");
	reg_add_print_copyright(x25_print_version,
				"x25_print_version");
	reg_add_default_x25_incoming_call(
	     (service_x25_incoming_call_type)return_true, "return_true");
	reg_add_dialer_setup(ET_X25, x25_set_dialer, "x25_set_dialer");
    }
    reg_add_tcp_listen(X25_SVC_PORT, x25_switch_tcp_listen,
		   "x25_switch_tcp_listen");
    reg_add_tcp_access_check(X25_SVC_PORT, x25_switching, "x25_switching");
    reg_add_ip_address_change(x25_ip_default, "x25_ip_default");
    if (registry_populated(REG_SRT)) {
	reg_add_srt_learn_source(x25_srt_learn_source, "x25_srt_learn_source");
    }
    reg_add_clns_parse_addr_map(ET_X25, x25_clns_parse_addr_map,
				"x25_clns_parse_addr_map");
    reg_add_encaps_nvram_write(ET_X25, x25_encaps_nvram_write,
                               "x25_encaps_nvram_write");
    reg_add_media_copy_encap_fields(ET_X25, copy_x25_fields, "copy_x25_fields");
    reg_add_media_x25_get_next_lci(x25_lci_lookup_next, "x25_lci_lookup_next");
    reg_add_media_x25_get_lci_info(x25_lci_get_info, "x25_lci_get_info");
    reg_add_media_x25_get_lci_stats(x25_lci_get_stats, "x25_lci_get_stats");
    reg_add_media_x25_lookup_lci(x25_lci_lookup_by_x121, "x25_lci_lookup_by_x121");

    /*
     * Initialize x25 parsing routines
     */
    x25_parser_init();

    /*
     * Set up x25 private function registry
     */
    x25_regcode = registry_create(REG_UNKNOWN, SERVICE_X25_MAX, "X.25");
    if (x25_regcode != REG_ILLEGAL)
        create_registry_x25();

    /*
     * Initialize timer support
     */
    reg_add_onemin(x25_pbp_timer, "x25_pbp_timer");
}

/*
 * Check interface type for
 * DTE 
 */
boolean ISDTE (hwidbtype *hwidb, considbtype *considb)
{
    if (ISCMNS_IEEE(hwidb)) {
	if (considb != NULL && validmem(considb))
	    return(IS_DTE(considb, cmns_dce));
	else 
	    return(TRUE); /* DXE(DTE) */
    }
    return(IS_DTE(hwidb, x25_dce));
}

/*
 * Check interface type for
 * DCE 
 */
boolean ISDCE (hwidbtype *hwidb, considbtype *considb)
{
    if (ISCMNS_IEEE(hwidb)) {
	if (considb != NULL && validmem(considb))
	   return(IS_DCE(considb, cmns_dce));
	else 
	    return(FALSE); /* DXE(DTE) */
    }
    return(IS_DCE(hwidb, x25_dce));
}

/*
 * getx25
 * Obtain a packet buffer and set it up for X25
 */

paktype *getx25 (
    hwidbtype *hwidb,
    lcitype *lci,
    int bytes,
    int extended)	/* extended iff DATA, RR or RNR */
{
    register paktype *pak;

    pak = getbuffer(bytes);
    if (pak) {
	pak->linktype = LINK_X25;
	pak->x25_flags = 0;
	pak->if_output = hwidb->firstsw;
	if (extended && (hwidb->x25_modulo == X25_MOD128)) {
	    pak->encsize = X25EHEADERBYTES;
	} else {
	    pak->encsize = X25HEADERBYTES;
	}
	pak->datagramsize = pak->encsize + bytes;
	pak->datagramstart = pak->network_start - pak->encsize;
    }
    return pak;
}

/*
 * x25_sendpak
 * Convenience routine to send a packet 
 * to lapb or llc2
 */
boolean x25_sendpak (
     hwidbtype *hwidb,
     paktype *pak,
     llctype *llc2)
{
    if (ISCMNS_IEEE(hwidb)) {
	if (llc_get_oqueue_status(llc2) == DLC_OQUEUE_FULL) {
	    datagram_done(pak);
	    return(FALSE);
	}
	llc2_send(llc2, pak, pak->datagramsize);
	return(TRUE);
    } else {
	if (lapb_vencapsulate(pak, 0)) {
	    (*hwidb->lapb_oqueue)(hwidb, pak, TAIL);
	    return(TRUE);
	} else {
	    datagram_done(pak);
	    return FALSE; /* lapb exited */
	}
    }
}

/*
 * x25_prstate
 * Return string for given X25 state
 */

char *x25_prstate (
    register hwidbtype *hwidb,
    register lcitype *lci)
{
    int state;

    if (lci)
	state = lci->lci_state;
    else if (hwidb)
	state = hwidb->x25_state;
    else
	state = X25_R1;
    switch (state) {
    case X25_R1:
	return "R1";
    case X25_R2:
	return "R2";
    case X25_R3:
	return "R3";
    case X25_P1:
	return "P1";
    case X25_P2:
	return "P2";
    case X25_P3:
	return "P3";
    case X25_P4:
	return "P4";
    case X25_P5:
	return "P5";
    case X25_P6:
	return "P6";
    case X25_P7:
	return "P7";
    case X25_D1:
	return "D1";
    case X25_D2:
	return "D2";
    case X25_D3:
	return "D3";
    case X25_X1:
	return "X1";
    default:
	return "ILLEGAL";
    }
}

/*
 * x25_prpack
 * Print out an X25 packet
 */

void x25_prpack (
    hwidbtype *hwidb,
    register paktype *pak,
    lcitype *lci,
    char io)
{
    static uchar mod[4] = {0, 8, 128, 0};
    register packettype *p = (packettype *)pak->datagramstart;
    register uchar *c = (uchar *)p + X25HEADERBYTES;
    register packettype *d;
    int len =0;
    char *ns;

    if (x25_vcnum != 0 && x25_vcnum != LCI(p))
	return;

    ns = hwidb ? hwidb->hw_namestring : "XOT";

    switch (p->x25_pti) {
    case X25_PVCSETUP:
	if ((*c != X25_PVCSETUP_VER) ||
	    (pak->datagramsize <
		X25HEADERBYTES + X25_PVCSETUP_BYTES + c[2] + c[5]))
	    buginf("\n%s: X25 %c %s PVC-SETUP ILLEGAL-ENCODING (%d)",
		   ns, io, x25_prstate(hwidb, lci), pak->datagramsize);
	else
	    /* packet description, status (size) <initiator's PVC> ... */
	    /* <responder's PVC> flow control values */
	    buginf("\n%s: X25 %c %s PVC-SETUP, %s (%d) <%*s pvc %d>"
		   "<%*s pvc %d> %d/%d %d/%d",
		   ns, io, x25_prstate(hwidb, lci),
		   x25_pvc_err_str[c[1]],
		   pak->datagramsize,
		   c[2], &c[12],
		   c[3] << 8 | c[4],
		   c[5], &c[12 + c[2]],
		   c[6] << 8 | c[7],
		   c[8], c[9], 1 << c[10], 1 << c[11]);
  	break;
    case X25_CALL_REQUEST:
	buginf("\n%s: X25 %c %s CALL REQUEST (%d)%s%s %d lci %d",
	    ns, io, x25_prstate(hwidb, lci),
	    pak->datagramsize, p->x25_q ? " A" : "",
	    p->x25_d ? " D" : "", mod[p->x25_sn], LCI(p));
	c = x25_prx25_addr(c);
	c = x25_printfacility(c);
	len = (ulong)c - (ulong)p;
	if (len < pak->datagramsize)
	    x25_encaps_print_cud(lci, c, pak->datagramsize - len,
				 X25_CALL_REQUEST);
	break;
    case X25_CALL_CONNECTED:
	buginf("\n%s: X25 %c %s CALL CONNECTED (%d)%s%s %d lci %d",
	    ns, io, x25_prstate(hwidb, lci),
	    pak->datagramsize, p->x25_q ? " A" : "",
	    p->x25_d ? " D" : "", mod[p->x25_sn], LCI(p));
	if (pak->datagramsize > X25HEADERBYTES) {
	    c = x25_prx25_addr(c);
	    c = x25_printfacility(c);
	    len = (ulong)c - (ulong)p;
	    if (len < pak->datagramsize)
		x25_encaps_print_cud(lci, c, pak->datagramsize - len,
				     X25_CALL_CONNECTED);
	}
	break;
    case X25_CLEAR:
	buginf("\n%s: X25 %c %s CLEAR REQUEST (%d)%s%s %d lci %d"
	       " cause %d diag %d",
	    ns, io, x25_prstate(hwidb, lci),
	    pak->datagramsize, p->x25_q ? " A" : "",
	    p->x25_d ? " D" : "", mod[p->x25_sn], LCI(p),
	    c[0], c[1]);
	if (pak->datagramsize > X25HEADERBYTES + X25_CAUSE_DATA) {
	    c = x25_prx25_addr(c + X25_CAUSE_DATA);
	    c = x25_printfacility(c);
	    len = (ulong)c - (ulong)p;
	    if (len < pak->datagramsize)
		x25_encaps_print_cud(lci, c, pak->datagramsize - len,
				     X25_CLEAR);
	}
	break;
    case X25_CLEAR_CONFIRMATION:
	buginf("\n%s: X25 %c %s CLEAR CONFIRMATION (%d)%s%s %d lci %d",
	    ns, io, x25_prstate(hwidb, lci),
	    pak->datagramsize, p->x25_q ? " A" : "",
	    p->x25_d ? " D" : "", mod[p->x25_sn], LCI(p));
	if (pak->datagramsize > X25HEADERBYTES) {
	    c = x25_prx25_addr(c);
	    c = x25_printfacility(c);
	}
	break;
    case X25_DIAGNOSTIC:
	d = (packettype *)(c + 1);
	buginf("\n%s: X25 %c %s DIAGNOSTIC (%d)%s%s %d lci %d"
	       " error %d [%s%s %d lci %d pti %#x]",
	    ns, io, x25_prstate(hwidb, lci),
	    pak->datagramsize, p->x25_q ? " Q" : "",
	    p->x25_d ? " D" : "", mod[p->x25_sn], LCI(p), *c,
	    d->x25_q ? " Q" : "", d->x25_d ? " D" : "", mod[d->x25_sn],
	    LCI(d), pak->datagramsize == (X25HEADERBYTES + X25_DIAG_DATA) ?
	    d->x25_pti : 0);
	break;
    case X25_INTERRUPT:
        buginf("\n%s: X25 %c %s INTR (%d)%s%s %d lci %d",
            ns, io, x25_prstate(hwidb, lci),
            pak->datagramsize, p->x25_q ? " Q" : "",
            p->x25_d ? " D" : "", mod[p->x25_sn], LCI(p));
        if (pak->datagramsize > X25HEADERBYTES) {
            len = (ulong)c - (ulong)p;
            if (len < pak->datagramsize)
                buginf("\n  First byte of Interrupt user data (%d): %#x",
                    ns, pak->datagramsize - len, *c);
        }
	break;
    case X25_INTERRUPT_CONFIRMATION:
	buginf("\n%s: X25 %c %s INTR CONFIRMATION (%d)%s%s %d lci %d",
	    ns, io, x25_prstate(hwidb, lci),
	    pak->datagramsize, p->x25_q ? " Q" : "",
	    p->x25_d ? " D" : "", mod[p->x25_sn], LCI(p));
	break;
    case X25_REGISTRATION:
	buginf("\n%s: X25 %c %s REGISTRATION (%d)%s%s %d lci %d",
	    ns, io, x25_prstate(hwidb, lci),
	    pak->datagramsize, p->x25_q ? " Q" : "",
	    p->x25_d ? " D" : "", mod[p->x25_sn], LCI(p));
	c = x25_prx25_addr(c);
	c = x25_printfacility(c);
	break;
    case X25_REGISTRATION_CONFIRMATION:
	buginf("\n%s: X25 %c %s REGISTRATION CONFIRMATION (%d)%s%s %d lci %d"
	       " cause %d diag %d",
	    ns, io, x25_prstate(hwidb, lci),
	    pak->datagramsize, p->x25_q ? " Q" : "",
	    p->x25_d ? " D" : "", mod[p->x25_sn], LCI(p),
	    c[0], c[1]);
	c = x25_prx25_addr(&c[2]);
	c = x25_printfacility(c);
	break;
    case X25_RESET:
	buginf("\n%s: X25 %c %s RESET REQUEST (%d)%s%s %d lci %"
	       "d cause %d diag %d",
	    ns, io, x25_prstate(hwidb, lci),
	    pak->datagramsize, p->x25_q ? " Q" : "",
	    p->x25_d ? " D" : "", mod[p->x25_sn], LCI(p), c[0], c[1]);
	break;
    case X25_RESET_CONFIRMATION:
	buginf("\n%s: X25 %c %s RESET CONFIRMATION (%d)%s%s %d lci %d",
	    ns, io, x25_prstate(hwidb, lci),
	    pak->datagramsize, p->x25_q ? " Q" : "",
	    p->x25_d ? " D" : "", mod[p->x25_sn], LCI(p));
	break;
    case X25_RESTART:
	buginf("\n%s: X25 %c %s RESTART (%d)%s%s %d lci %d cause %d diag %d",
	    ns, io, x25_prstate(hwidb, lci),
	    pak->datagramsize, p->x25_q ? " Q" : "",
	    p->x25_d ? " D" : "", mod[p->x25_sn], LCI(p),
	    c[0], c[1]);
	break;
    case X25_RESTART_CONFIRMATION:
	buginf("\n%s: X25 %c %s RESTART CONFIRMATION (%d)%s%s %d lci %d",
	    ns, io, x25_prstate(hwidb, lci),
	    pak->datagramsize, p->x25_q ? " Q" : "",
	    p->x25_d ? " D" : "", mod[p->x25_sn], LCI(p));
	break;
    default:
	if ((p->x25_pti & X25_FLOW_MASK) == X25_DTE_REJ)
	    buginf("\n%s: X25 %c %s DTE REJ (%d)%s%s %d lci %d PR %d",
		ns, io, x25_prstate(hwidb, lci),
		pak->datagramsize,
		p->x25_q ? " Q" : "", p->x25_d ? " D" : "",
		mod[p->x25_sn], LCI(p),
		p->x25_sn == X25_MODULO_8 ? (p->x25_pti >> 5) & 0x7 :
		    (c[0] >> 1) & 0x7f);
	else if ((p->x25_pti & X25_FLOW_MASK) == X25_RNR)
	    buginf("\n%s: X25 %c %s RNR (%d)%s%s %d lci %d PR %d",
		ns, io, x25_prstate(hwidb, lci),
		pak->datagramsize,
		p->x25_q ? " Q" : "", p->x25_d ? " D" : "",
		mod[p->x25_sn], LCI(p),
		p->x25_sn == X25_MODULO_8 ? (p->x25_pti >> 5) & 0x7 :
		    (c[0] >> 1) & 0x7f);
	else if ((p->x25_pti & X25_FLOW_MASK) == X25_RR)
	    buginf("\n%s: X25 %c %s RR (%d)%s%s %d lci %d PR %d",
		ns, io, x25_prstate(hwidb, lci),
		pak->datagramsize,
		p->x25_q ? " Q" : "", p->x25_d ? " D" : "",
		mod[p->x25_sn], LCI(p),
		p->x25_sn == X25_MODULO_8 ? (p->x25_pti >> 5) & 0x7 :
		    (c[0] >> 1) & 0x7f);
	else if ((p->x25_pti & X25_DATA_MASK) == X25_DATA) {
	    buginf("\n%s: X25 %c %s DATA (%d)%s%s %d lci %d%s PS %d PR %d",
		ns, io, x25_prstate(hwidb, lci),
		pak->datagramsize,
		p->x25_q ? " Q" : "", p->x25_d ? " D" : "",
		mod[p->x25_sn], LCI(p),
		p->x25_sn == X25_MODULO_8 ? (p->x25_pti & 0x10 ? " M" : "") :
		    (c[0] & 0x1 ? " M" : ""),
		p->x25_sn == X25_MODULO_8 ? (p->x25_pti >> 1) & 0x7 :
		    (p->x25_pti >> 1) & 0x7f,
		p->x25_sn == X25_MODULO_8 ? (p->x25_pti >> 5) & 0x7 :
		    (c[0] >> 1) & 0x7f);
		if (lci) {
		    if (lci->lci_pbp_compress && hwidb != NULL)
			buginf(" (pbp)");
		    else if ((lci->lci_data_pid_method != X25_DATA_PID_NONE) &&
			     ((io == 'I') ?
			     ((lci->lci_data_flags & X25_DATA_M_RX) == 0) :
			      (lci->lci_tx_first_frag))) {
			len = (p->x25_sn == X25_MODULO_8) ?
			      X25HEADERBYTES : X25EHEADERBYTES;
			x25_encaps_print_data_pid(pak->datagramstart + len,
						  pak->datagramsize - len);
		    }
		}
	} else
	    buginf("\n%s: X25 %c %s ILLEGAL %#x (%d)",
		ns, io, x25_prstate(hwidb, lci),
		p->x25_pti, pak->datagramsize);
	break;
    }
}

/*
 * X25_restart
 * restart an X25 interface, which CLEARs all SVCs and breaks all
 * routed PVC connections
 */

void x25_restart (
    register hwidbtype *hwidb,
    uchar ack)
{
    leveltype level;

    hwidb->x25_restart = FALSE;

    /*
     *  flush any VC connections and temporary address maps, then set the
     *  operational parameters from the configured administrative values
     */
    level = raise_interrupt_level(NETS_DISABLE);
    x25_lci_hash_restart(hwidb, NULL);
    x25_map_hash_restart(hwidb, NULL);
    x25_valid_config(hwidb, TRUE);
    reset_interrupt_level(level);

    /*
     *  if a serial link has come up,
     *  tell SNMP if we haven't already sent a X25RESTART trap
     */
    if (!ISIEEEMSK(hwidb) && hwidb->x25_upstate && hwidb->x25_state == X25_R1) {
	if (hwidb->x25_snmp_restart_tx)
	    hwidb->x25_snmp_restart_tx = FALSE;
	else {
	    hwidb->reason = "X.25 up";
	    reg_invoke_ifmib_link_trap(LINK_UP_TRAP, hwidb);
	}
    }

    /*
     *  if indicated, acknowledge an incoming RESTART
     */
    if (ack)
	sendpacket(hwidb, 0, X25_RESTART_CONFIRMATION, 0, NULL);

    /*
     *  cancel the periodic RESTART in R1
     */
    if (hwidb->x25_state == X25_R1) {
	TIMER_STOP(hwidb->x25_timer);
	hwidb->x25_retransmit = 0;
    }

    /*
     * Inform others that the line is up
     */
    if (hwidb->x25_upstate && 
	(hwidb->x25_cause == X25_RESTART_NETWORK_OPERATIONAL))
	reg_invoke_media_protocol_up(hwidb, LINK_ALL);
}

/* x25_place_call
 * Slightly more user-friendly wrapper to make a call out
 * Doesn't help the PAD ...
 */

static lcitype *x25_place_call (
    register idbtype *idb,
    addrtype *proto_addr,
    uchar *subaddr,
    uchar prec,
    int protocol,
    uchar *cud,
    uint cudlen)
{
    register x25map *map;
    hwidbtype *hwidb = idb->hwptr;
    uint map_service;

    /* find the map_service for the protocol which is probably qllc */
    map_service = x25_map_link2service( (uint)protocol);
    /* find the x25 map for this qllc vmac addr */   
    map = x25_map_find(hwidb, proto_addr, X25_MATCH_EXACT, map_service);    
    /* if the map is valid clear the map flags */
/*
    if (map == NULL)
      return(NULL);
    else
*/
    return(x25_call_setup(idb, map, proto_addr, subaddr, protocol, 
			  prec, cud, cudlen, FALSE));
}

/*
 *  x25_call_setup
 *  Perform call setup for an encapsulation VC.
 *  Return pointer to LCI
 */

lcitype *x25_call_setup (
    register idbtype *idb,
    register x25map *map,
    addrtype *proto_addr,
    uchar *subaddr,
    ushort lci_linktype,
    uchar prec,
    uchar *cud,
    uint cudlen,
    boolean reverse)
{
    register lcitype *lci;
    hwidbtype *hwidb = idb->hwptr;

    if (!hwidb->x25_upstate)
	return(NULL);

    if (map == NULL) {
	/*
	 *  only a PAD or QLLC Call to an X.121 address can omit an
	 * address map
	 */
	if (proto_addr->type != ADDR_X121) {
	    return(NULL);
	}

	/*
	 *  a PAD/QLLC map isn't necessary, but check if one does
	 *  exist in order to abide by any configured values
	 *
	 *  note that the search will not skip maps that bar outgoing
	 *  Call; if a map is found that bars outgoing Calls, no Calll
	 *  will be generated
	 */
	if (lci_linktype == LINK_PAD) {
	    map = x25_map_find(hwidb, proto_addr, X25_MATCH_EXACT, X25_MAPS_PAD);
	} else if (lci_linktype == LINK_QLLC) {
	    map = x25_map_find(hwidb, proto_addr, X25_MATCH_EXACT, X25_MAPS_QLLC_NATIVE);
	} else {
	    return(NULL);
	}
    }

    if (map && (map->map_flags & X25_MAPF_BAR_OUT))
	return(NULL);

    /*
     *  create a VC for this connection and initialize it for an outgoing Call
     */
    if ((lci = x25_lci_enter(idb, X25_NO_VC_ASSIGNED, NULL))) {
	lci->lci_linktype = lci_linktype;

	if (map && !x25_map_add_lci(lci, map)) {
	    x25_call_remove(lci, X25_CLEAR_NOT_OBTAINABLE, X25_DIAG_NO_INFO);
	    lci = NULL;
	}
    }

    if (!lci) {
	if (map && (map->map_status != X25_MAP_PERM))
	    x25_map_delete(map, FALSE);
    } else {
	/*
	 *  complete the initialization of the VC
	 */

	/*
	 *  source and destination addresses
	 */
	bcopy(hwidb->x25_address, &lci->lci_src_addr, sizeof(addrtype));

	/*
	 * Put in a subaddress if the caller has one
	 */
	if (subaddr != NULL) {
	    int length;

	    length = strlen(subaddr);
	    if (length + lci->lci_src_addr.length > X121_MAX_DIGITS) {
		errmsg(&msgsym(ADDRESSBAD, X25), 
		       hwidb->hw_namestring, 
		       (*hwidb->x25_address).x121_addr,  subaddr);
		x25_call_remove(lci, X25_CLEAR_NOT_OBTAINABLE, X25_DIAG_NO_INFO);
		if (map && (map->map_status != X25_MAP_PERM))
		    x25_map_delete(map, FALSE);
		lci = NULL;
		return(lci);
	    }
	    bcopy(subaddr, 
		  &lci->lci_src_addr.x121_addr[lci->lci_src_addr.length],
		  length);
	    lci->lci_src_addr.length += length;
	}

	bcopy(map ?
	      &map->map_address[X25_MAP_PRIMARY_ADDR_INX].addr : proto_addr,
	      &lci->lci_dst_addr, sizeof(addrtype));

	lci->lci_prec = prec;
	lci->lci_reverse = reverse;

	/*
	 *  determine the flow control values
	 */
	lci->lci_win = (map && map->map_fac.win) ?
		map->map_fac.win :
		(hwidb->x25_fwin ? hwidb->x25_fwin : hwidb->x25_win);
	lci->lci_wout = (map && map->map_fac.wout) ?
		map->map_fac.wout :
		(hwidb->x25_fwout ? hwidb->x25_fwout : hwidb->x25_wout);
	lci->lci_pin = (map && map->map_fac.pin) ?
		map->map_fac.pin :
		(hwidb->x25_fpin ? hwidb->x25_fpin : hwidb->x25_pin);
	lci->lci_pout = (map && map->map_fac.pout) ?
		map->map_fac.pout :
		(hwidb->x25_fpout ? hwidb->x25_fpout : hwidb->x25_pout);

	/*
	 *  lower any flow control values that are too large
	 */
	(void) x25_fix_flow_fac(lci);

	if (! x25_sendcall(lci, cud, cudlen)) {
	    x25_call_remove(lci, X25_CLEAR_NOT_OBTAINABLE, X25_DIAG_NO_INFO);
	    lci = NULL;
	    hwidb->x25_callssentfailed++;
	}
    }

    return(lci);
}

/*
 * x25_call_remove
 * Remove a given call from data structures
 */

void x25_call_remove (
    register lcitype *lci,
    uchar cause,
    uchar diag)
{
    register paktype *pak;

    if (lci->lci_linktype == LINK_PAD)
	pad_clearcall(lci, cause, diag);
    if (lci->lci_linktype == LINK_X25SERVICE)
	x25_service_remove(lci);
    if (lci->lci_swdata)
	x25_flush_call(lci, cause, diag, FALSE);
    if (TIMER_RUNNING(lci->lci_timer)) {
	p_unqueue(&x25tQ, lci);
	TIMER_STOP(lci->lci_timer);
    }

    /*
     * Send notifications back for packets we could not deliver
     */
    while ((pak = pak_dequeue(&lci->lci_holdQ))) {
	if (pak->if_input)
	    reg_invoke_send_unreach(pak->linktype, pak);
	datagram_done(pak);
    }

    while ((pak = p_dequeue(&lci->lci_reassmQ)))
	datagram_done(pak);
    while ((pak = pak_dequeue(&lci->lci_resetQ)))
	datagram_done(pak);

    /*
     *  a failed encapsulation VC may go into hold-down
     */
    if (lci->lci_idb->hwptr->x25_holdtime &&
	(lci->lci_state == X25_P2 || lci->lci_state == X25_P3) &&
	lci->lci_map && (lci->lci_map->map_services & X25_MAPS_ENCAPS))
	x25_lci_holddown(lci);
    else
	x25_lci_delete(lci);
}

/*
 * x25_call_reset
 * Reset call
 */

void x25_call_reset (
    register lcitype *lci,
    boolean kill)
{
    register paktype *pak;

    /*
     * clear the D state machine variables
     */
    lci->lci_ps = 0;
    lci->lci_pr = 0;
    lci->lci_ack = 0;
    lci->lci_rpr = 0;
    lci->lci_busy = FALSE;
    lci->lci_allow_int = TRUE;
    lci->lci_allow_int_cnf = FALSE;
    lci->lci_tx_frag_cnt = 0;
    lci->lci_tx_first_frag = TRUE;
    lci->lci_reassmbytes = 0;
    lci->lci_data_flags = 0;

    x25_pbp_reset(lci);

    /*
     * flush the packets in the hold queue
     */
    while ((pak = pak_dequeue(&lci->lci_holdQ))) {
	if (pak->if_input)
	    reg_invoke_send_unreach(pak->linktype, pak);
	datagram_done(pak);
    }

    /*
     * flush the packets in the reassembly and reset queues
     */
    while ((pak = p_dequeue(&lci->lci_reassmQ)))
	datagram_done(pak);
    while ((pak = pak_dequeue(&lci->lci_resetQ)))
	datagram_done(pak);

    /*
     * stop a PVC connect timer, an SVC idle timer or, if the link
     * is RESTARTing, a RESET retransmit timer. If it's a PVC connect
     * timer save how much time we have left so we can start where 
     * we left off	
     */
    if (TIMER_RUNNING(lci->lci_timer) &&
	(kill || (lci->lci_state != X25_D2 && lci->lci_state != X25_D3))) {
	if (IS_LCI_PVC(lci->lci_idb->hwptr,lci->lci_lci) &&
            lci->lci_state == X25_D1) {
            lci->lci_pvc_remainder = TIME_LEFT_SLEEPING(lci->lci_timer);
        }
	p_unqueue(&x25tQ, lci);
	TIMER_STOP(lci->lci_timer);
    }

    if (lci->lci_linktype == LINK_PAD)
	pad_reset(lci);
}

/*
 * x25_deencapsulate
 * Perform necessary functions when encapsulation changes 
 */

void x25_deencapsulate (hwidbtype *hwidb)
{
    leveltype level;
    idbtype *swidb;
    int expected_ip_mtu;

    /*
     * In case of BFE and DDN encapsulation, the expected default
     * ip mtu values.
     */
    expected_ip_mtu = 0;
    if (ISBFE(hwidb))
	expected_ip_mtu = X25_BFE_MTU;
    else if (ISDDN(hwidb) && !ISCMNS_IEEE(hwidb))
	expected_ip_mtu = X25_DDN_MTU;

    hwidb->status |= IDB_BCAST;
    if (is_x25(hwidb)){
	/*
	 *  tell SNMP that this link is down
	 */
	hwidb->reason = "X.25 deencapsulated";
	reg_invoke_ifmib_link_trap(LINK_DOWN_TRAP, hwidb);

	level = raise_interrupt_level(NETS_DISABLE);
	hwidb->x25_state = X25_R1;
	hwidb->x25_upstate = FALSE;
	hwidb->x25_cause = X25_RESTART_LOCAL_PROC_ERR;
	hwidb->x25_diag = X25_DIAG_LINK_OUT_OF_ORDER;
	x25_restart(hwidb, FALSE);
	no_compression(hwidb);
	lapb_deencapsulate(hwidb);
	reset_interrupt_level(level);
    }

    /*
     * remove this idb from the x25idbq
     */
    if (hwidb->x25idb) {
        unqueue(&x25idbQ, hwidb->x25idb);
        free(hwidb->x25idb);
        hwidb->x25idb = NULL;
    }

    /*
     *  Remove this interface from bridging processing.  Shut down
     *  all subinterfaces.
     *  If ip mtu has been set by BFE and DDN encapsulation, reset
     *  it to maximum interface mtu.
     */
    FOR_ALL_SWIDBS_ON_HW(hwidb, swidb) {
	if (swidb->tbridge_on_swidb) {
	    hwidb->x25_bridge = FALSE;
	    hwidb->span_bridge_process_force &= ~SPAN_BR_PROC_X25_BRIDGE;
	}
	if (expected_ip_mtu == swidb->ip_mtu)
	    swidb->ip_mtu = hwidb->maxmtu;
	if (is_subinterface(swidb))
	    shutdown_subif(swidb, SUBIF_STATE_DELETED);

	/*
	 * Reenable IP multicast. This is the default setting on other 
	 * encapsulations.
	 */
	swidb->ip_multicast_fs = TRUE;
    }
}

/*
 * x25_extract_lci
 *
 * Convert the opaque element passed to the x25 encapsulation routines into
 * an lci pointer--this procedure is invoked when the encapsulation invoker
 * knows the destination VC a packet will be transmitted on.
 */
boolean x25_extract_lci (
    paktype *pak,
    long opaque,
    addrtype *proto_addr,
    lcitype **lci_ptr,
    boolean *broadcast)
{
    *lci_ptr = (lcitype *)opaque;
    return(TRUE);
}


/*
 * x25_vencapsulate
 *
 * Build an X25 packet from some data, however datagramsize and datagramstart
 * are assumed to be initialized by the caller.
 */
boolean x25_vencapsulate (
    paktype *pak,
    long opaque)
{
    return(x25_encaps(pak, opaque, NULL));
}

/*
 *  x25_encaps
 *
 *  This is the workhorse function for preparing a packet for sending over
 *  an X.25/CONS interface.  It is invoked from a number of places for
 *  all varieties of traffic sent over X.25.
 *
 *  Invokers must assist in determining the outgoing VC to use when given
 *  the packet and passed opaque parameter.  Invokers that are forwarding
 *  datagrams must be able to compose the destination address (which is
 *  used to find first the map, then the VC to use).  X.25 service invokers
 *  (e.g. X.25 switching or PAD) bypass the map lookup procedure by directly
 *  setting the address of the lci.
 *
 *  TRUE is returned if the packet is ready to be handed to x25_output();
 *  FALSE if the packet can't be sent.
 */
boolean x25_encaps (
    paktype *pak,
    long opaque,
    x25map *map)
{
    idbtype *idb;
    lcitype *lci;
    leveltype level;
    addrtype proto_addr;
    boolean broadcast;
    uint service = 0;
    uint linktype;

    idb = pak->if_output;
    linktype = pak->linktype;

    /*
     *  encapsulation fails if
     *  the outgoing interface protocol is down or
     *  the packet protocol type isn't handled by X.25
     */
    if (!idb->hwptr->x25_upstate)
	return(FALSE);
    if ((pak->linktype != LINK_X25) && (pak->linktype != LINK_X25TCP)) { 
	/*
	 * if this is a bridge interface, pass LINK_BRIDGE type 
	 * in order to get a valid service map. 
	 */
	if (BRIDGING_IF(idb, LINK_IP) && (idb->tbridge_on_swidb))
	   linktype = LINK_BRIDGE;
	if ((service = x25_map_link2service(linktype)) == 0)
	    return(FALSE);
    }

    lci = NULL;

    level = raise_interrupt_level(NETS_DISABLE);

    /*
     *  lower layers may prioritize buffers based on the priority bit;
     *  clear it to enforce serialized transmission
     */
    pak->flags &= ~PAK_PRIORITY;

    if ((service == X25_MAPS_COMP_TCP) &&
	(pak->x25_flags & PAK_X25_ENC)) {
	/*
	 *  We're dealing with re-encapsulation of an IP datagram which
	 *  thc_compress_tcp() has processed.  The IP datagram had already
	 *  been encapsulated (and marked as such by x25_output()), so the
	 *  map and outgoing VC are known (indeed, should not be changed).
	 */
	lci = pak->x25_cb;
	map = lci->lci_map;
    } else {
	if (service == X25_MAPS_QLLC) {
	    lci = pak->x25_cb;
	    map = lci->lci_map;
	}

	pak->x25_flags &= ~PAK_X25_ENC;
    }

    if (!map) {
	/*
	 *  This is the original invocation rather than a successive
	 *  invocation that is sending an outgoing broadcast datagram.
	 *
	 *  Any recursive invocation from broadcast datagram handling will
	 *  skip this section because it has already determined the map
	 *  structure to use.
	 */

	memset(&proto_addr, 0, sizeof(addrtype));
	broadcast = FALSE;

	/*
	 *  Invoke the protocol-specific procedure to determine:
	 *    the datagram destination address OR the lci to use,
	 *    if the datagram is a broadcast, and
	 *    the Q and D bit values.
	 *  The protocol-specific procedure determines this information
	 *  from the packet and the opaque value supplied to the
	 *  encapsulation routine.
	 *
	 *  Encapsulated protocols undergo an address -> map lookup and
	 *  a map -> lci lookup to determine the address of the outgoing
	 *  lci structure.
	 *
	 *  Protocols that use the X.25/CONS service directly (e.g.
	 *  LINK_PAD, LINK_X25, etc.) short-circuit the address mapping
	 *  process by simply setting the the address of the outgoing lci
	 *  structure to use.
	 */
	if (!reg_invoke_media_x25_macaddr(pak->linktype, pak, opaque,
					  &proto_addr, &lci, &broadcast)) {
	    reset_interrupt_level(level);
	    if (!reg_used_media_x25_macaddr(pak->linktype))
		errmsg(&msgsym(BADMACREG, LINK), idb->namestring,
		       pak->linktype);
	    return(FALSE);
	}

	/*
	 * Reset the service value because the media_x25_macaddr changes
	 * the link type to LINK_BRIDGE for both IP and ARP packets in case
	 * that we are not routing IP but we are the destination IP host
	 * bridge for these IP/ARP packets.
	 */
	service = x25_map_link2service(pak->linktype);

	if (!lci) {
	    /*
	     *  this is an encapsulation invocation (connection-oriented
	     *  invokers will set the lci address directly); we need to
	     *  find the map to use
	     */

	    /*
	     *  For a broadcast datagram we'll get the adress of the first
	     *  map to use (if any) after the remaining maps have all been
	     *  used to re-invoke this procedure to forward a copy of our
	     *  original datagram.
	     */
	    if (broadcast) {
		map = x25_broadcast(idb, pak);
	    } else {
		/*
		 *  for a IP/TCP packet, check if a compressed TCP map
		 *  applies, otherwise look for the native service map
		 */
		if ((service == X25_MAPS_IP) &&
		    (iphdrtype_start(pak)->prot == TCP_PROT)) {
		    if (idb->subif_link_type == SUBIF_TYPE_P2P)
			map = x25_map_type_check(idb->hwptr, idb,
						LINK_COMPRESSED_TCP);
		    else
			map = x25_map_find(idb->hwptr, &proto_addr,
					X25_MATCH_EXACT, X25_MAPS_COMP_TCP);

		    if (map && ((map->map_flags & X25_MAPF_PASSIVE) == 0)) {
			service = X25_MAPS_COMP_TCP;
			pak->x25_flags |= PAK_X25_ENC;
		    } else
			map = NULL;
		}

		if (map == NULL) {
		    if (idb->subif_link_type == SUBIF_TYPE_P2P)
			map = x25_map_type_check(idb->hwptr, idb,
						pak->linktype);
		    else
			map = x25_map_find(idb->hwptr, &proto_addr,
					X25_MATCH_EXACT, service);
		}

		if ((map == NULL) && (service == X25_MAPS_IP))
		    map = x25_ip_ocall_setup(idb, &proto_addr);
	    }
	}
    }

    if (!lci) {
	/*
	 *  find (or create) an lci using the map
	 */
	if ((map == NULL) ||
	    ((lci = x25_encaps_find_lci(idb, pak, map, service)) == NULL)) {
	    reset_interrupt_level(level);
	    return(FALSE);
	}
    }

    /*
     *  x25_output() (which is invoked after encapsulation is performed)
     *  expects the address of the lci strucutre in the packet
     */
    pak->x25_cb = lci;

    /*
     *  if the VC does multiple protocol encapsulation, we need
     *  to prepend a protocol identification
     */
    if ((lci->lci_data_pid_method != X25_DATA_PID_NONE) &&
	!x25_encaps_form_data_pid(pak, lci->lci_data_pid_method)) {
	reset_interrupt_level(level);
	return(FALSE);
    }

    if (idb->hwptr->x25_modulo == X25_MOD8) {
	pak->datagramsize += X25HEADERBYTES;
	pak->datagramstart -= X25HEADERBYTES;
    } else {
	pak->datagramsize += X25EHEADERBYTES;
	pak->datagramstart -= X25EHEADERBYTES;
    }

    if (ISCMNS_IEEE(idb->hwptr)) {
	register llctype *llc2_cb = lci->lci_llc2;
	boolean rc;
	uchar *savestart;

	if (!validate_llc(llc2_cb)) {
	    if (x25_debug || x25event_debug)
		buginf("\n%s: LLC2 of lci= %d has invalid CB",
		       idb->namestring, lci->lci_lci);
	    reset_interrupt_level(level);
	    return FALSE;
	}
	pak->if_output = llc2_cb->idb;
	pak->linktype = llc2_cb->linktype;
	pak->llc2_cb  = lci->lci_llc2;
	/* Add 4 bytes for I frame */
	pak->datagramstart -= 4;
	savestart = pak->datagramstart;
	/*
	 * Remember the offset to the first SAP.
	 * if encaps didn't fail.
	 */

	rc = llc_encapsulate(pak, LLC_IFRAME, pak->datagramsize, llc2_cb);
	if (rc) {
	    pak->llc2_sapoffset = savestart - pak->datagramstart;
	}
	reset_interrupt_level(level);
	return(rc);
    } else {
	reset_interrupt_level(level);
	return(lapb_vencapsulate(pak, opaque));
    }
}

/*
 * x25_send_quench
 *
 * Registered for VCs that can't drop a packet without a RESET
 */
void x25_send_quench (
    paktype *pak)
{
    paktype *oldpak;
    lcitype *lci;
    
    lci = pak->x25_cb;

    if ((lci->lci_state == X25_P4) || (lci->lci_state == X25_D1)) {
	x25_sendcause(pak->if_output->hwptr, lci, X25_RESET,
		      X25_RESET_NETWORK_CONGESTION, X25_DIAG_NO_INFO, NULL);
    } else {
	while ((oldpak = pak_dequeue(&lci->lci_holdQ)))
	    datagram_done(oldpak);
    }
}

/*
 * x25_output
 * Send an X25 packet on its way
 * OR, if (pak->datagramsize == 0),
 * simply check if any packets on the VC holdQ can be sent
 */

boolean x25_output (
    hwidbtype *hwidb,
    paktype *pak,
    enum HEADTAIL which)
{
    register paktype *newpak;
    register lcitype *lci = pak->x25_cb;
    uint modulo;
    register leveltype level;
    int sent;
    int holdQ_count;
    lcitype *swlci = NULL;

    /*
     * This is a real dangerous thing, but such is life currently.  The
     * MCI output routine was changed to call idb->oqueue instead of
     * holdq_enqueue.  This causes X..25 to barf of course.  So here we
     * check if which == HEAD.  This indicates that the driver is trying
     * to requeue the packet, so we just call holdq_enqueue here like
     * the driver use to do, and put it on the front of the list.
     *
     * In addition, the dialer may give us a raw/out-of-band V25.bis frame to
     * send without invoking the encapsulation function (which will prevent
     * any other use of ET_HDLC)
     */

    if ((which == HEAD) ||
	(pak->datagramsize && (pak->enctype == ET_HDLC) && is_ddr(hwidb))) {
	if (hwidb->lapb_oqueue) {
	    return((*hwidb->lapb_oqueue)(hwidb, pak, which));
	} else {
	    return(holdq_enqueue(hwidb, pak, which));
	}
    }

    level = raise_interrupt_level(NETS_DISABLE);

    modulo = hwidb->x25_modulo;

    /*
     *  check that we have a real packet--
     *  we'll get an empty packet if the caller wants us to try to send
     *  any packets on the VC's holdQ.
     */
    if (pak->datagramsize == 0)
	/*
	 *  Note: DO NOT try to free an empty packet!  They're not "real".
	 */
	pak = NULL;

    /*
     *  don't try to do anything if the pak doesn't have an associated
     *  X.25 circuit
     */
    if (lci) {
	holdQ_count = lci->lci_holdQ.count;
	if (pak) {
	    /*
	     *  we need to do something awkward here--
	     *  invoke TCP header compression if it applies to an outgoing
	     *  IP packet.  The awkward part is that we already determined
	     *  that compression needs to be done and what the outgoing
	     *  VC is, but the compression can't be done in x25_encaps()
	     *  (where it makes more sense) because TCP header compression
	     *  can generate a new packet and the encapsulation process can't
	     *  deal with such an event.  The compression routine is going to
	     *  re-invoke X.25's encapsulation routine, but the packet is
	     *  flagged with the fact that the map and VC have already been
	     *  determined.
	     */
	    if (pak->x25_flags & PAK_X25_ENC) {

#ifdef DEBUG
		paktype *orig_pak;

		if (thc_debug) {
		    orig_pak = pak_duplicate(pak);
		    bugpak("\nX25 packet before THC: ", orig_pak);
		    retbuffer(orig_pak);
		}
#endif DEBUG

		/*
		 *  invoke TCP header compression
		 */
		pak = reg_invoke_compress_tcpip_header(LINK_IP, hwidb, pak,
						FALSE, lci->lci_map->map_thc);
		if (pak && (pak = thc_encapsulation(hwidb, pak))) {
		    pak->x25_cb = lci;

		    /*
		     *  compressed TCP over a VC using's Cisco's proprietary
		     *  method must use the Q bit to distinguish between
		     *  compressed and uncompressed TCP datagrams
		     */
		    if ((lci->lci_cud_pid_method == X25_CUD_PID_CISCO) &&
			(pak->linktype == LINK_COMPRESSED_TCP))
			pak->x25_flags |= PAK_X25_QBIT;
		    else
			pak->x25_flags &= ~PAK_X25_QBIT;
		}

#ifdef DEBUG
		if (thc_debug && pak) {
		    orig_pak = pak_duplicate(pak);
		    bugpak("\nX25 packet After THC : ", orig_pak);
		    retbuffer(orig_pak);
		}
#endif DEBUG
	    }

	    holdQ_count++;
	}

	/*
	 *  if lci is usable, output packets while the window is open
	 */
	if (!lci->lci_busy && !lci->lci_pbp_rnr) {
	    /*
	     *  the VC may be RESET if the output routines encounter
	     *  problems, so test the VC state each time through
	     *
	     *  we can't simply break from the loop after processing the
	     *  new packet because, if fragmented, the remaining data will
	     *  be put on the hold queue.
	     */
	    while (((lci->lci_state == X25_P4) || (lci->lci_state == X25_D1)) &&
		   (((lci->lci_ps - lci->lci_rpr + modulo) % modulo) <
		    lci->lci_wout)) {
		/*
		 *  send the new packet only if the hold queue is empty
		 */
		if (QUEUEEMPTY(&lci->lci_holdQ)) {
		    newpak = pak;
		    pak = NULL;
		} else {
		    newpak = pak_dequeue(&lci->lci_holdQ);
		}

		if ((newpak == NULL) || (!((*lci->x25_dooutput)(newpak))))
		    break;
	    }
	}

	/*
	 *  if we haven't sent our packet and the VC is usable,
	 *  queue the packet to it
	 */
	if (pak) {
	    switch (lci->lci_state) {
	      case X25_P2:
	      case X25_P3:
	      case X25_P5:
	      case X25_P4:
	      case X25_D1:
	      case X25_D2:
	      case X25_D3:
		if (!QUEUEFULL(&lci->lci_holdQ) &&
		    pak_enqueue(&lci->lci_holdQ, pak))
		    pak = NULL;
		break;
	    }
	}

	if (lci->lci_pbp_local_ack) { /*near inner node function*/
	    /*
	     * ack telling far inner node our lci_holdQ status is
	     * the far inner node uses this to compute back pressure for
	     * the far outer node.
	     */
	    if ((sent = holdQ_count - lci->lci_holdQ.count) > 0) {
		lci->lci_pbp_ack = (lci->lci_pbp_ack + sent) % X25_PBP_MODULO;

		if (lci->lci_swtype == X25_ROUTE_IF) { /*local X25 switching*/
		    /* reach over to the local switch lci & update its
		     * ack variables, and send an ack out its interface
		     * if required.
		     */
		    if ((swlci = lci->lci_swdata) == NULL)
			goto PBP_FAILED;

		    swlci->lci_pbp_rpr = lci->lci_pbp_ack;
		    swlci->lci_pbp_holdQ_count = (swlci->lci_pbp_ps - 
						  swlci->lci_pbp_rpr + 
						  X25_PBP_MODULO) %
							    X25_PBP_MODULO;

		    if (x25_pbp_unacked(swlci->lci_idb->hwptr, swlci)) {
			x25_sendflow(swlci->lci_idb->hwptr, swlci, X25_RR);
		    }
		} else { /*remote X25 switching*/
		    if (lci->lci_pbp_ackpak == NULL) {
			lci->lci_pbp_ackpak = x25_pbp_begin_ack(hwidb, lci);
			if (lci->lci_pbp_ackpak == NULL) {
PBP_FAILED:
			    x25_pbp_flush_call(lci);
			    if (pak)
				datagram_done(pak);
			    reset_interrupt_level(level);
			    return(FALSE);
			}
			/* queue for x25_timer() processing.
			 * doing x25_forward_ack() here will crash the router.
			 * we are on ip_input() thread.  doing 
			 * x25_switch_tcp_send() will result in a stack
			 * overflow. 
			 */
			p_enqueue(&x25LocalAckQ, lci->lci_pbp_ackpak);
		    }
		}
	    }
	}
    }

    /*
     *  if we haven't sent our packet, we can't--drop it
     */
    if (pak) {
	if (pak->if_output)
	    pak->if_output->hwptr->counters.output_total_drops++;

	reg_invoke_send_quench(pak->linktype, pak);
	datagram_done(pak);
    }

    reset_interrupt_level(level);
    return(FALSE);
}

/*
 * x25_lapboutput
 * Actually send an X25 DATA packet to LAPB; return FALSE if unable to
 * send because of a buffer failure when fragmenting.
 */

boolean x25_lapboutput (register paktype *pak)
{
    register hwidbtype *hwidb = pak->if_output->hwptr;
    register lcitype *lci = pak->x25_cb;
    register ushort paksize;
    register packettype *p;
    uchar *c, mbit;
    ushort encsize;
    paktype *newpak;

    encsize = (hwidb->x25_modulo == X25_MOD8) ?
		X25HEADERBYTES : X25EHEADERBYTES;
    paksize = (1 << lci->lci_pout);

    if (lci->lci_tx_frag_cnt) {
	lci->lci_tx_first_frag = FALSE;
    } else {
	lci->lci_tx_first_frag = TRUE;

	/* don't re-compress local switched data*/ 
	if (lci->lci_pbp_compress) {
	    /*
	     * since compress size > uncompress size, other than datagrams
	     * can be fragmented.
	     */
	    if (!x25_pbp_compress(hwidb, lci, pak, &newpak)) {
		hwidb->counters.output_total_drops++;
		datagram_done(pak);
		return(TRUE);
	    }
	    if (newpak == NULL) {
		if (!pak_requeue(&lci->lci_holdQ, pak)) {
		    datagram_done(pak);
		    x25_sendcause(hwidb, lci, X25_RESET,
		      X25_RESET_NETWORK_CONGESTION, X25_DIAG_NO_INFO, NULL);
		} else
		    lci->lci_thc_applied = TRUE;
		return(FALSE);
	    }
	    datagram_done(pak);
	    pak = newpak;
	}

	/*
	 *  if fragmentation is needed, generate the needed control data
	 */
	if ((pak->datagramsize - encsize - LAPB_OVERHEAD(hwidb)) > paksize) {
	    pak->datagramsize -= encsize + LAPB_OVERHEAD(hwidb);
	    pak->datagramstart += encsize + LAPB_OVERHEAD(hwidb);
	    lci->lci_tx_frag_cnt =
		(pak->datagramsize + (paksize - 1)) / paksize;
	}
    }

    /*
     *  if the fragment count is non-zero we're sending a complete packet
     *  sequence that defines the datagram--we need to generate the next
     *  fragment
     */
    if (lci->lci_tx_frag_cnt) {
	if (paksize > pak->datagramsize)
	    paksize = pak->datagramsize;
	if (((newpak = getx25(hwidb, lci, paksize, TRUE)) == NULL) ||
	    (!lapb_vencapsulate(newpak, 0))) {
	    /*
	     *  problem--we're out of buffers; requeue the packet to try later
	     */
	    if (newpak)
		datagram_done(newpak);
	    if (!pak_requeue(&lci->lci_holdQ, pak)) {
		datagram_done(pak);
		x25_sendcause(hwidb, lci, X25_RESET,
			X25_RESET_NETWORK_CONGESTION, X25_DIAG_NO_INFO, NULL);
	    }
	    return(FALSE);
	}

	newpak->x25_cb = pak->x25_cb;
	newpak->linktype = pak->linktype;
	newpak->x25_flags = pak->x25_flags & PAK_X25_QBIT;
	newpak->if_output = pak->if_output;
	bcopy(pak->datagramstart, newpak->network_start, paksize);

	/*
	 *  adjust the control data and requeue an incompletely fragmented
	 *  datagram
	 */
	if ((pak->datagramsize -= paksize) == 0) {
	    lci->lci_tx_frag_cnt = 0;
	    datagram_done(pak);
	} else {
	    pak->datagramstart += paksize;
	    lci->lci_tx_frag_cnt--;
	    if (!pak_requeue(&lci->lci_holdQ, pak)) {
		datagram_done(newpak);
		datagram_done(pak);
		x25_sendcause(hwidb, lci, X25_RESET,
			X25_RESET_NETWORK_CONGESTION, X25_DIAG_NO_INFO, NULL);
		return(TRUE);
	    }
	    newpak->flags |= PAK_MBIT;
	}
	pak = newpak;
    }

    /*
     *  we'll set the packet's M bit if it's specified in the buffer flags
     *  and it has the maximum number of data octets (required for GOSIP)
     */
    mbit = FALSE;

    if (pak->flags & PAK_MBIT) {
	if ((pak->x25_flags & PAK_X25_DBIT) ||
	    ((pak->datagramsize - encsize - LAPB_OVERHEAD(hwidb)) ==
	     (1 << lci->lci_pout)))
	    mbit = TRUE;
	else
	    errmsg(&msgsym(BADMBIT, X25), hwidb->hw_namestring, lci->lci_lci);
    }


    /*
     *  form the DATA packet information
     */
    p = (packettype *)(pak->datagramstart + LAPB_OVERHEAD(hwidb));
    p->x25_q = (pak->x25_flags & PAK_X25_QBIT) ? 1 : 0;
    p->x25_d = (pak->x25_flags & PAK_X25_DBIT) ? 1 : 0;
    p->x25_sn = (hwidb->x25_modulo == X25_MOD8) ?
		X25_MODULO_8 : X25_MODULO_128;
    p->x25_lcg = LCG(lci->lci_lci);
    p->x25_lcn = LCN(lci->lci_lci);

    if (lci->lci_pbp_local_ack) {	/*near outer node function*/
	x25_pbp_update_lci_ack(hwidb, lci);
    } else {
	if (lci->lci_linktype != LINK_X25 &&
	   lci->lci_linktype != LINK_X25TCP &&
	   lci->lci_linktype != LINK_PAD &&
	   lci->lci_linktype != LINK_X25SERVICE)
	    lci->lci_ack = lci->lci_pr;
    }

    if (hwidb->x25_modulo == X25_MOD8) {
	p->x25_pti = (lci->lci_ack << 5) | (mbit ? (1 << 4) : 0) |
		     (lci->lci_ps << 1) | X25_DATA;
    } else {
	c = (uchar *)p + X25HEADERBYTES;
	p->x25_pti = (lci->lci_ps << 1) | X25_DATA;
	*c = (lci->lci_ack << 1) | (mbit ? 1 : 0);
    }

    /*
     *  send the DATA packet, adjusting the statistics and management info.
     */
    lci->lci_bytessent += (pak->datagramsize - encsize - LAPB_OVERHEAD(hwidb));

    if (x25_debug) {
	pak->datagramstart += LAPB_OVERHEAD(hwidb);
	pak->datagramsize -= LAPB_OVERHEAD(hwidb);
	x25_prpack(hwidb, pak, lci, 'O');
	pak->datagramstart -= LAPB_OVERHEAD(hwidb);
	pak->datagramsize += LAPB_OVERHEAD(hwidb);
    }

    /*
     *  flush the buffer's flag fields and send the packet
     */
    pak->x25_flags = 0;
    pak->flags &= ~PAK_MBIT;
    (*hwidb->lapb_oqueue)(hwidb, pak, TAIL);

    GET_TIMESTAMP(lci->lci_lastoutput);
    lci->lci_packetssent++;
    hwidb->x25_data_tx++;
    lci->lci_ps = ++lci->lci_ps % hwidb->x25_modulo;

    /*
     *  reset any idle timer
     */
    if (lci->lci_idle) {
	if (TIMER_RUNNING(lci->lci_timer))
	    p_unqueue(&x25tQ, lci);
	TIMER_START(lci->lci_timer, lci->lci_idle);
	x25_addtimer(lci);
    }

    return(TRUE);
}

/*
 * DteProtocolViolation
 * SNA Network Management Hook for 
 * protocol violation.
 */
void DteProtocolViolation(lcitype *lci, 
                          hwidbtype *hwidb, 
                          uchar err)
{
    /* NmEvent Hook 7 */
    X25_EventData eventData;
    eventData.diag = err;
    X25Event(lci, hwidb, &eventData, ProtocolViolation);
}

/*
 *  x25_senddiag
 *  Send a diagnostic packet based upon input packet or timer expiration;
 *  timer diagnostics (X25_DIAG_TE_RESTART_INDICATION,
 *  X25_DIAG_TE_CLEAR_INDICATION and X25_DIAG_TE_RESET_INDICATION) do not
 *  take a pak pointer, although the CLEAR and RESET timer diagnostics
 *  take an lci pointer
 */

void x25_senddiag (
    hwidbtype *hwidb,
    paktype *pak,
    uchar err,
    lcitype *lci)
{
    register paktype *newpak;
    register packettype *np;
    register uchar *c;
    uchar *p = NULL;
    uchar modulo;

    if (pak)
	p = pak->datagramstart;

    if ((!lci && ISCMNS_IEEE(hwidb)) ||
	(newpak = getx25(hwidb, NULL, X25_DIAG_DATA, FALSE)) == NULL)
	return;
    newpak->llc2_cb = (pak) ? pak->llc2_cb : (lci ? lci->lci_llc2 : NULL);

    /*
     *	construct the basic diagnostic packet information
     */

    np = (packettype *)newpak->datagramstart;
    np->x25_q = 0;
    np->x25_d = 0;
    modulo = (hwidb->x25_modulo == X25_MOD8) ? X25_MODULO_8 : X25_MODULO_128;
    np->x25_sn = modulo;
    np->x25_lcg = 0;
    np->x25_lcn = 0;
    np->x25_pti = X25_DIAGNOSTIC;
    c = (uchar *)np + X25HEADERBYTES;
    *c++ = err;

    /*
     *	construct any diagnostic explanation field
     */
    switch (err) {
      case X25_DIAG_TE_RESTART_INDICATION:
	*c++ = modulo << 4;
	*c = 0;
	newpak->datagramsize--;
	break;

      case X25_DIAG_TE_CLEAR_INDICATION:
      case X25_DIAG_TE_RESET_INDICATION:
	/*
	 *  CLEAR and RESET timer diagnostics must have an lci to reference
	 */
	*c++ = (modulo << 4) | LCG(lci->lci_lci);
	*c = LCN(lci->lci_lci);
	newpak->datagramsize--;
	break;

      default:
	/*
	 *  a non-timer diagnostic must have a packet to reference
	 */
	switch (pak->datagramsize) {
	  case 0:
	    newpak->datagramsize -= X25HEADERBYTES;
	    break;
	  case 1:
	    *c = *p;
	    newpak->datagramsize -= X25HEADERBYTES - 1;
	    break;
	  case 2:
	    *c++ = *p++;
	    *c = *p;
	    newpak->datagramsize -= X25HEADERBYTES - 2;
	    break;
	  default:
	    *c++ = *p++;
	    *c++ = *p++;
	    *c = *p;
	    break;
	}
	break;
    }

    /* NmEvent Hook 9 */
    /* diagnostic packet sent has same Alert as received */
     X25EventRx(hwidb, lci, np);

    if (x25_debug || x25event_debug)
	x25_prpack(hwidb, newpak, lci, 'O');
    if (x25_sendpak(hwidb, newpak, newpak->llc2_cb))
	hwidb->x25_diagssent++;
}

/*
 * x25_sendint
 * Send an Interrupt packet
 */

void x25_sendint (
    lcitype *lci,
    paktype *pak,
    uchar *data,
    uint len)
{
    hwidbtype *idb;
    packettype *p;
    uchar *c;

    idb = lci->lci_idb->hwptr;

    if (!pak && (pak = getx25(idb, lci, len, FALSE))) {
	p = (packettype *)pak->datagramstart;
	p->x25_q = 0;
	p->x25_d = 0;
	p->x25_sn = (idb->x25_modulo == X25_MOD8) ?
			X25_MODULO_8 : X25_MODULO_128;
	p->x25_pti = X25_INTERRUPT;	
	c = (uchar *)p + X25HEADERBYTES;
	bcopy(data, c, len);
    }

    if (pak) {
	p = (packettype *)pak->datagramstart;
	p->x25_lcg = LCG(lci->lci_lci);
	p->x25_lcn = LCN(lci->lci_lci);

	if (x25_debug || x25event_debug)
	    x25_prpack(idb, pak, lci, 'O');
	idb->x25_interrupt_tx++;
	lci->lci_intssent++;
	(void) x25_sendpak(idb, pak, lci->lci_llc2);
	lci->lci_allow_int_cnf = TRUE;
    } else {
	x25_sendcause(idb, lci, X25_RESET,
		      X25_RESET_NETWORK_CONGESTION, X25_DIAG_NO_INFO, NULL);
    }
}

/*
 * x25_badgfi
 * Check if input GFI components are legal based on PTI--1988 Table 16/X.25
 */

boolean x25_badgfi (
    uchar qbit,
    uchar dbit,
    uchar sn,
    uchar pti,
    uchar modulo)
{
    if ((modulo == X25_MOD8) && (sn != X25_MODULO_8))
	return TRUE;
    if ((modulo == X25_MOD128) && (sn != X25_MODULO_128))
	return TRUE;

    if ((pti & X25_DATA_MASK) != X25_DATA) {
	switch (pti) {
	  case X25_CALL_REQUEST:
	  case X25_CALL_CONNECTED:
	    /*
	     *	Note: we don't allow A-bit addressing at this time
	     */
	    if (qbit)
		return TRUE;
	    break;
	  default:
	    if (qbit || dbit)
		return TRUE;
	    break;
	}
    }

    return FALSE;
}

/*
 * x25_data
 * Determine if this packet is a legal data or flow control packet.
 * Return broken out information and TRUE otherwise FALSE.
 */

boolean x25_data (
    paktype *pak,
    int *pr,
    int *mbit,
    int *ps,
    int *len)
{
    register packettype *p = (packettype *)pak->datagramstart;
    uchar *c = (uchar *)p + X25HEADERBYTES;

    pak->x25_flags = 0;

    if ((p->x25_pti & X25_FLOW_MASK) != X25_DTE_REJ &&
	(p->x25_pti & X25_FLOW_MASK) != X25_RNR &&
        (p->x25_pti & X25_FLOW_MASK) != X25_RR &&
	(p->x25_pti & X25_DATA_MASK) != X25_DATA)
	return FALSE;

    if ((p->x25_sn & 0x3) == X25_MODULO_8) {
	*len = X25HEADERBYTES;
	*pr = (p->x25_pti >> 5) & 0x7;
	if ((p->x25_pti & X25_DATA_MASK) == X25_DATA) {
	    *ps = (p->x25_pti >> 1) & 0x7;
	    *mbit = (p->x25_pti & 0x10) ? TRUE : FALSE;
	}
    } else {
	*len = X25EHEADERBYTES;
	*pr = (*c >> 1) & 0x7f;
	if ((p->x25_pti & X25_DATA_MASK) == X25_DATA) {
	    *ps = (p->x25_pti >> 1) & 0x7f;
	    *mbit = *c & 0x1 ? TRUE : FALSE;
	}
    }

    if (p->x25_q)
	pak->x25_flags |= PAK_X25_QBIT;
    if (p->x25_d)
	pak->x25_flags |= PAK_X25_DBIT;

    return TRUE;
}

/*
 * x25_ack
 * Update window for given LCI
 */

boolean x25_ack (
    register hwidbtype *hwidb,
    register lcitype *lci,
    int pr)
{
    uchar good;

    if (pr != lci->lci_ps && pr != lci->lci_rpr) {
	if (lci->lci_ps > lci->lci_rpr)
	    good = (pr >= lci->lci_rpr && pr <= lci->lci_ps);
	else if (lci->lci_rpr == lci->lci_ps)
	    good = (pr == lci->lci_rpr);
	else
	    good = (pr >= lci->lci_rpr || pr <= lci->lci_ps);
    } else
	good = TRUE;
    if (!good) {
	x25_sendcause(hwidb, lci, X25_RESET, X25_RESET_LOCAL_PROC_ERR,
	    X25_DIAG_INVALID_PR, NULL);
	return FALSE;
    }
    lci->lci_rpr = pr;
    return TRUE;
}

/*
 * x25_sendcause
 * Send an X25 restart/reset/clear packet with cause
 */

void x25_sendcause (
    hwidbtype *hwidb,
    register lcitype *lci,
    uchar type,
    uchar reason,
    uchar diag,
    paktype *opak)
{
    register paktype *pak;
    register packettype *p;
    register uchar *c;
    uint size;
    register considbtype *considb;
    register leveltype level;
    uchar cause = reason;
    uint cud_len = 0;
    X25_EventData eventData;
    uchar *cud = NULL;

    level = raise_interrupt_level(NETS_DISABLE);

    considb = get_lci_considb(lci);

    size = X25_CAUSE_DATA;

    /*
     *  find Clear User Data
     */
    if (opak) {
	uint osize = X25HEADERBYTES + X25_CAUSE_DATA;

	if (opak->datagramsize > osize) {
	    /*
	     *  skip past the address block and facilities
	     */
	    osize += bcdlen_inline(opak->datagramstart[osize]) + 1;
	    if (opak->datagramsize > osize)
		osize += opak->datagramstart[osize] + 1;
	    if (opak->datagramsize > osize) {
		cud = &opak->datagramstart[osize];
		cud_len = opak->datagramsize - osize;
	    }
	}
    }

    /*
     *  add the size of the address and CUD of an extended Clear packet
     */
    if (lci && (type == X25_CLEAR) && !ISDDN(hwidb)) {
	/*
	 *  only a modified destination address is allowed on a CLEAR
	 *  with a CLAM facility
	 */
	if (lci->lci_lam &&
	    ((lci->lci_state == X25_P2) || (lci->lci_state == X25_P3) ||
	     (lci->lci_state == X25_P5))) {
	    size += X25_CALL_DATA;
	    size += ((lci->lci_dst_addr.length + 1) / 2) + 2;
	    size += cud_len;
	} else if (cud_len)
	    size += X25_CALL_DATA + cud_len;
    }

    /*
     *  for a RESTART, mark the interface for a task-level service reset,
     *  for a CLEAR or RESET, clean up a VC *now* to free any associated
     *  packets, which may avoid some nasty problems that can occur if we
     *  run out of buffers
     */
    if (type == X25_RESTART)
	hwidb->x25_restart = TRUE;
    else
	x25_call_reset(lci, TRUE);	/* also cancels any LCI timer */

    pak = getx25(hwidb, lci, size, FALSE);

    if (!pak) {
	/*
	 *  we can't compose a packet to send!
	 *
	 *  This is a difficult situation; the general strategy for a
	 *  CLEAR or RESET is to plunk the VC in the state it would be
	 *  in if we were able to send a packet and start a short
	 *  retransmit timer to try to get the packet out as soon as
	 *  feasible.  This should cause our handling to be less
	 *  incorrect, but we will do bad things if a CLEAR or RESET
	 *  comes in while we're waiting for the phony retransmit;
	 *  we will violate the protocol by treating the event as
	 *  a collision, but we can't handle these corner cases without
	 *  a lot of special case code.
	 *
	 *  For RESTARTing the interface, we simply mark the interface
	 *  to try again later from the timer task.
	 */

	switch (type) {
	case X25_CLEAR:
	    /*
	     *  we have one of three cases:
	     *    attempting to initiate a CLEAR (state isn't P6 or P7),
	     *    a repeated failure to initiate a CLEAR (retransmit < 0), or
	     *    we can't resend the CLEAR
	     */
	    if (lci->lci_state != X25_P6 && lci->lci_state != X25_P7) {
		x25_flush_call(lci, X25_CLEAR_NETWORK_CONGESTION,
			       X25_DIAG_CALL_ERROR, FALSE);
		lci->lci_state = ISDTE(hwidb, considb) ? X25_P6 : X25_P7;
		lci->lci_retransmit = -1;
	    }

	    /*
	     *  try again later
	     */
	    TIMER_START(lci->lci_timer, ONESEC);
	    x25_addtimer(lci);
	    break;
	case X25_RESET:
	    /*
	     *  we have one of three cases:
	     *    attempting to initiate a RESET (state isn't D2 or D3),
	     *    a repeated failure to initiate a RESET (retransmit < 0), or
	     *    we can't resend the RESET
	     */
	    if (lci->lci_state != X25_D2 && lci->lci_state != X25_D3) {
		lci->lci_state = ISDTE(hwidb, considb) ? X25_D2 : X25_D3;
		lci->lci_retransmit = -1;
	    }

	    /*
	     *  try again later
	     */
	    TIMER_START(lci->lci_timer, ONESEC);
	    x25_addtimer(lci);
	    break;
	case X25_RESTART:
	    if (ISCMNS_IEEE(hwidb)) {
		considb->x25_cause = X25_RESTART_NETWORK_CONGESTION;
		considb->x25_diag = X25_DIAG_NO_INFO;
		TIMER_START(lci->lci_timer, ONESEC);
		x25_addtimer(lci);
	    } else {
		if (hwidb->x25_state != X25_R2 && lci->lci_state != X25_R3) {
		    hwidb->x25_state = ISDTE(hwidb, considb) ? X25_R2 : X25_R3;
		    hwidb->x25_retransmit = -1;
		}
		hwidb->x25_cause = X25_RESTART_NETWORK_CONGESTION;
		hwidb->x25_diag = X25_DIAG_NO_INFO;
		TIMER_START(hwidb->x25_timer, ONESEC);
	    }
	    break;
	}

	reset_interrupt_level(level);
	return;
    }

    switch (type) {
    case X25_CLEAR:
	if (lci->lci_retransmit < 0) {
	    cause = reason = X25_CLEAR_NETWORK_CONGESTION;
	    diag = X25_DIAG_NO_INFO;
	    lci->lci_retransmit = 0;
	}
	if (ISDTE(hwidb, considb))
	    cause = X25_CLEAR_DTE_ORIGINATED;

	if (lci->lci_retransmit < X25_CLEAR_TRANSMIT) {

            /* NmEvent Hook 1 */
            eventData.cause = cause;
            eventData.diag = diag;
            X25Event(lci, hwidb, &eventData, X25Clear_Sent);

	    lci->lci_retransmit++;
	    lci->lci_state = ISDTE(hwidb, considb) ? X25_P6 : X25_P7;
	    TIMER_START(lci->lci_timer, hwidb->x25_t3 * ONESEC);
	    x25_addtimer(lci);
	    if (!x25_called_from_api) {
		reg_invoke_media_x25_notify_event(lci->lci_linktype,
			lci, X25_CLEAR_IND_EVENT, cause, diag, NULL);
	    }
	} else {
	    if (ISDCE(hwidb, considb))
		x25_senddiag(hwidb, NULL, X25_DIAG_TE_CLEAR_INDICATION, lci);
	    x25_call_remove(lci, 0, 0);
	    hwidb->x25_callssentfailed++;
	    hwidb->x25_clears_exhausted++;
	    hwidb->x25_retries_exhausted++;
	    datagram_done(pak);
	    reset_interrupt_level(level);
	    return;
	}
	break;
    case X25_RESET:
	if (lci->lci_retransmit < 0) {
	    cause = reason = X25_RESET_NETWORK_CONGESTION;
	    diag = X25_DIAG_NO_INFO;
	    lci->lci_retransmit = 0;
	}
	if (lci->lci_retransmit < X25_RESET_TRANSMIT) {
	    if (ISDTE(hwidb, considb))
		cause = X25_RESET_DTE_ORIGINATED;
	    lci->lci_resetssent++;
	    lci->lci_retransmit++;

            /* NmEvent Hook 2 */
            /* Reset trap is moved to X25Event */
            eventData.diag = diag;
            eventData.cause = cause;
            X25Event(lci, hwidb, &eventData, X25Reset_Sent); 

	    lci->lci_state = ISDTE(hwidb, considb) ? X25_D2 : X25_D3;
	    TIMER_START(lci->lci_timer, hwidb->x25_t2 * ONESEC);
	    x25_addtimer(lci);
	    if (!x25_called_from_api) {
	        reg_invoke_media_x25_notify_event(lci->lci_linktype,
			lci, X25_RESET_IND_EVENT, cause, diag, NULL);
	    }
	    if (!ISIEEEMSK(hwidb)) {
		reg_invoke_x25Reset_event(lci);
	    }
	} else if (!IS_LCI_PVC(hwidb, lci->lci_lci)) {
	    hwidb->x25_retries_exhausted++;
	    lci->lci_retransmit = 0;
	    datagram_done(pak);
	    x25_sendcause(hwidb, lci, X25_CLEAR,
		X25_CLEAR_LOCAL_PROC_ERR, X25_DIAG_TE_RESET_INDICATION, NULL);
	    reset_interrupt_level(level);
	    return;
	} else {
	    if (ISDCE(hwidb, considb))
		x25_senddiag(hwidb, NULL, X25_DIAG_TE_RESET_INDICATION, lci);
	    hwidb->x25_retries_exhausted++;
	    lci->lci_retransmit = 0;
	    x25_reenter_d1(lci);
	    datagram_done(pak);
	    reset_interrupt_level(level);
	    return;
	}
	break;
    case X25_RESTART:
	/*
	 *  if we're trying to change modulo and the administrative
	 *  configuration is valid, use the new modulo to send the RESTART;
	 *  we can't update all operational values yet, because the SVCs
	 *  must be cleared first (otherwise they may become ersatz PVCs)--
	 *  the SVCs will get cleared and the configuration updated in
	 *  x25_restart()
	 */
	if ((hwidb->x25_modulo != hwidb->x25_admin_modulo) &&
	    x25_valid_config(hwidb, FALSE)) {
	    hwidb->x25_modulo = hwidb->x25_admin_modulo;
	}
	if (ISCMNS_IEEE(hwidb)) {
	   considb->x25_restartssent++;
	   hwidb->x25_restartssent++;
	   considb->x25_cause = cause;
	   considb->x25_diag = diag;

           /* NmEvent Hook 10 */
           eventData.diag = diag;
           eventData.cause = cause;
           X25Event(lci, hwidb, &eventData, CMNS_Restart_Sent);

	   considb->x25_upstate = FALSE;
	   /* make sure we start at DXE until interface resolved */
	   lci->lci_state = ISDCE(hwidb, considb) ? X25_R3 : X25_R2;
	   if (TIMER_RUNNING(lci->lci_timer))
		p_unqueue(&x25tQ, lci);
	   TIMER_START(lci->lci_timer, hwidb->x25_t0 * ONESEC);
	   x25_addtimer(lci);
	} else { /* serial */
	    if (hwidb->x25_retransmit < 0) {
		cause = reason = X25_RESTART_NETWORK_CONGESTION;
		diag = X25_DIAG_NO_INFO;
		hwidb->x25_retransmit = 0;
	    }
	   if ((hwidb->x25_retransmit < X25_DCE_RESTART_TRANSMIT) ||
	       ISDTE(hwidb, considb)) {
		hwidb->x25_retransmit++;
		if (ISDTE(hwidb, considb))
		    cause = X25_RESTART_DTE_ORIGINATED;
		hwidb->x25_restartssent++;
		hwidb->x25_cause = cause;
		hwidb->x25_diag = diag;
		TIMER_START(hwidb->x25_timer, hwidb->x25_t0 * ONESEC);

/* This trap is moved to x25_event.c
 		if (hwidb->x25_upstate && hwidb->x25_state == X25_R1) {
		    reg_invoke_x25Restart_event(hwidb);
  		    hwidb->x25_snmp_restart_tx = TRUE;
 		}
*/
                /* NmEvent Hook 3 */
                eventData.diag = diag;
                eventData.cause = cause;
                X25Event(lci, hwidb, &eventData, X25Restart_Sent);

		hwidb->x25_state = ISDTE(hwidb, considb) ? X25_R2 : X25_R3;
		hwidb->x25_upstate = FALSE;
	    } else {
		if (ISDCE(hwidb, considb))
		    x25_senddiag(hwidb, NULL, X25_DIAG_TE_RESTART_INDICATION,
				 NULL);
		hwidb->x25_retries_exhausted++;
		hwidb->x25_retransmit = 0;
		TIMER_STOP(hwidb->x25_timer);
		hwidb->x25_state = X25_R1;
		hwidb->x25_upstate = TRUE;
		datagram_done(pak);
		reset_interrupt_level(level);
		return;
	    }
	}
	break;
    }

    p = (packettype *)pak->datagramstart;
    p->x25_q = 0;
    p->x25_d = 0;
    p->x25_sn = (hwidb->x25_modulo == X25_MOD8) ? X25_MODULO_8 : X25_MODULO_128;
    if (lci) {
	if (ISCMNS_IEEE(hwidb) &&
	    type == X25_RESTART) {
	   p->x25_lcg = 0;
	   p->x25_lcn = 0;
	} else {
	   p->x25_lcg = LCG(lci->lci_lci);
	   p->x25_lcn = LCN(lci->lci_lci);
	}
    } else {
	p->x25_lcg = 0;
	p->x25_lcn = 0;
    }

    p->x25_pti = type;
    c = (uchar *)p + X25HEADERBYTES;
    *c++ = cause;
    *c++ = diag;

    /*
     *  if needed, encode an extended Clear
     */
    if (lci && (type == X25_CLEAR) && !ISDDN(hwidb)) {
	/*
	 *  only a modified destination address is allowed on a CLEAR
	 *  with a CLAM facility
	 */
	if (lci->lci_lam &&
	    ((lci->lci_state == X25_P2) || (lci->lci_state == X25_P3) ||
	     (lci->lci_state == X25_P5))) {
	    /*
	     *  address block
	     */
	    *c++ = (lci->lci_dst_addr.length & 0xf) << 4;
	    chartobcd(lci->lci_dst_addr.x121_addr, c, lci->lci_dst_addr.length);
	    c += (lci->lci_dst_addr.length + 1) / 2;

	    /*
	     *  facilities
	     */
	    *c++ = 2;
	    *c++ = X25_FACILITY_LINE_ADDR_MODIFY;
	    *c++ = lci->lci_lam;
	} else if (cud_len) {
	    *c++ = 0;	/* address lengths */
	    *c++ = 0;	/* faclility length */
	}

	if (cud_len)
	    bcopy(cud, c, cud_len);
    }

    if (x25_debug || x25event_debug)
	x25_prpack(hwidb, pak, lci, 'O');
    if (lci && lci->lci_swdata && (type != X25_RESTART)) {
	opak = pak_duplicate(pak);
	if (opak) {
	    if (type == X25_CLEAR)
		x25_forward_clear(lci, opak, reason, diag);
	    else
		x25_forward_reset(lci, opak, reason, diag);
	} else {
	    /*
	     *  error--no packets available to forward a CLEAR or RESET
	     */
	    datagram_done(pak);
	    pak = NULL;

	    if (type == X25_CLEAR) {
		x25_flush_call(lci, reason, diag, FALSE);
	    } else {
		reason = IS_LCI_PVC(hwidb, lci->lci_lci) ?
			X25_RESET_NETWORK_OUT_OF_ORDER : 
			X25_CLEAR_NETWORK_CONGESTION;
		x25_flush_call(lci, reason, diag, TRUE);
	    }
	}
    }

    if (pak)
	(void) x25_sendpak(hwidb, pak, lci ? lci->lci_llc2 : NULL);

    reset_interrupt_level(level);
}

/*
 *  Send a Restart Confirm, Call Confirm, Clear Confirm, Interrupt Confirm
 *  or a Reset Confirm packet.
 */
void sendpacket (
    register hwidbtype *hwidb,
    register lcitype *lci,
    uchar type,
    uchar dbit,
    paktype *opak)
{
    register paktype *pak = NULL;	/*make compiler happy*/
    register packettype *p;
    register uchar *c;
    uchar ablock[2 * X25_ADDRESS_LENGTH];
    uchar ablock_len_field;
    uint size, facilitylen;
    uchar *cud = NULL;
    uint cud_len = 0;
    uchar call_conf_pad = 0;

    size = 0;
    ablock_len_field = 0;
    facilitylen = 0;

    if (ISBFE(hwidb) && lci && (type == X25_CALL_CONNECTED)) {
	lci->lci_confirm_addr = FALSE;
	if (lci->lci_force_winsize || lci->lci_force_pksize) {
	    x25_sendcause(hwidb, lci, X25_CLEAR,
			  X25_CLEAR_INCOMPAT_DEST, X25_DIAG_CALL_ERROR, NULL);
	    return;
	}
    }

    if (lci && (type == X25_CALL_CONNECTED)) {
	/*
	 *  form the address block in ASCII and determine its length
	 *
	 *  Note that the suppress flags are not consulted because we're
	 *  sending a Call Confirm, so the addresses in the lci are
	 *  the ones received in the Call packet and so are allowable
	 *  (also note that "calling/source" and "called/destination"
	 *  designations are *sensitive* to which end originated the call!
	 */
	if (lci->lci_confirm_addr) {
	    if ((lci->lci_dst_addr.type == ADDR_X121) &&
		lci->lci_dst_addr.length) {
		bcopy(lci->lci_dst_addr.x121_addr,
		      ablock, lci->lci_dst_addr.length);
		ablock_len_field = lci->lci_dst_addr.length & 0x0f;
	    }
	    if ((lci->lci_src_addr.type == ADDR_X121) &&
		lci->lci_src_addr.length) {
		bcopy(lci->lci_src_addr.x121_addr,
		      &ablock[ablock_len_field], lci->lci_src_addr.length);
		ablock_len_field |= (lci->lci_src_addr.length << 4) & 0xf0;
	    }
	}

	if (lci->lci_force_winsize)
	    facilitylen += 3;
	if (lci->lci_force_pksize)
	    facilitylen += 3;

	if (lci->lci_linktype == LINK_X25 ||
	    lci->lci_linktype == LINK_X25SERVICE) {
	    if (lci->lci_lam != 0)
		facilitylen += 2;
	    if (lci->lci_tput_out || lci->lci_tput_in)
		facilitylen += 2;
	    /* Transit Delay */
	    if (lci->lci_tdelay != 0)
		facilitylen += 3;
	    /*
             * Facility Marker and DDN specific marker
             * is needed for the following facilities.
             */
	    if (lci->lci_prec != X25_UNASSIGNED_PREC && 
		 IS_NON_BFE_DDN(hwidb)) {
		facilitylen += 6;   /* precedence */
	    }	
	    /*
	     * Facility Marker and CCITT specific marker
	     * is needed for the following facilities.
	     */
	    if (CCITT_marker(lci)) {
		facilitylen += 2;

		/* NSAPs */
		if (lci->lci_dst_nsap.length)
		    facilitylen +=  2 + 1 + lci->lci_dst_nsap.length;
		if (lci->lci_src_nsap.length)
		    facilitylen +=  2 + 1 + lci->lci_src_nsap.length;
	    }
	} else {
	    /*
             * Facility Marker and DDN specific marker
             * is needed for the following facilities.
             */
	    if (lci->lci_prec != X25_UNASSIGNED_PREC && 
		 IS_NON_BFE_DDN(hwidb))
	        facilitylen += 6;   /* precedence */
	}

	/*
	 *  find any CUD to include; CUD size validataion has already been
	 *  performed
	 */
	if (opak) {
	    uint osize = X25HEADERBYTES;

	    /*
	     *  skip past a address block and facility block
	     */
	    if (opak->datagramsize > osize) {
		osize += bcdlen_inline(opak->datagramstart[osize]) + 1;
		if (opak->datagramsize > osize)
		    osize += opak->datagramstart[osize] + 1;

		if (opak->datagramsize > osize) {
		    cud = &opak->datagramstart[osize];
		    cud_len = opak->datagramsize - osize;
		}
	    }
	}

	/*
	 *  add in the lengths of any optional fields as needed
	 */
	if (cud_len)
	    size += X25_ABLOCK_LEN(ablock_len_field) +
		(1 + facilitylen) + cud_len;
	else if (facilitylen)
	    size += X25_ABLOCK_LEN(ablock_len_field) + (1 + facilitylen);
	else if (ablock_len_field) {
	    considbtype *considb;
	    considb = get_lci_considb(lci);
	    size += X25_ABLOCK_LEN(ablock_len_field);
	    if (ISDCE(hwidb,considb))
		size += 1;
	}
    }

    if (lci && (type == X25_CALL_CONNECTED) && (size == 0)) {
	call_conf_pad = X25_CALL_DATA;
	size += call_conf_pad;
    }

    pak = getx25(hwidb, ISCMNS_IEEE(hwidb) ? NULL : lci, size, FALSE);

    if (!pak)
	return;

    p = (packettype *)pak->datagramstart;
    p->x25_q = 0;
    p->x25_d = dbit;
    p->x25_sn = (hwidb->x25_modulo == X25_MOD8) ? X25_MODULO_8 : X25_MODULO_128;
    if (lci == NULL) {
	p->x25_lcg = 0;
	p->x25_lcn = 0;
    } else {
	if (ISCMNS_IEEE(hwidb) &&
	 type == X25_RESTART_CONFIRMATION) {
	    p->x25_lcg = 0;
	    p->x25_lcn = 0;
	} else {
	    p->x25_lcg = LCG(lci->lci_lci);
	    p->x25_lcn = LCN(lci->lci_lci);
	}
    }

    p->x25_pti = type;
    c = (uchar *)p + X25HEADERBYTES;

    if (lci && (type == X25_CALL_CONNECTED) &&
	(ablock_len_field || facilitylen || cud_len)) {
	/*
	 *  encode the address block
	 */
	*c = ablock_len_field;
	if (ablock_len_field) {
	    chartobcd(ablock, &c[1],
		((ablock_len_field >> 4) & 0x0f) + (ablock_len_field & 0x0f));
	    c += X25_ABLOCK_LEN(ablock_len_field);
	} else {
	    c++;
	}

	/*
	 *  encode the facilities
	 */

	*c++ = facilitylen;

	if (lci->lci_force_winsize) {
	    *c++ = X25_FACILITY_FLOW_WINDOW;
	    *c++ = lci->lci_wout;		/* from called */
	    *c++ = lci->lci_win;		/* from calling */
	}
	if (lci->lci_force_pksize) {
	    *c++ = X25_FACILITY_FLOW_PACKET;
	    *c++ = lci->lci_pout;		/* from called */
	    *c++ = lci->lci_pin;		/* from calling */
	}

	if (lci->lci_linktype == LINK_X25 ||
	    lci->lci_linktype == LINK_X25SERVICE) {
	    if (lci->lci_lam != 0) {
		*c++ = X25_FACILITY_LINE_ADDR_MODIFY;
		*c++ = lci->lci_lam;
	    }
	    if (lci->lci_tput_out || lci->lci_tput_in) {
		*c++ = X25_FACILITY_THROUGHPUT;
		*c++ = ((lci->lci_tput_out << 4) & 0xf0) |
		       (lci->lci_tput_in & 0x0f);
	    }
	    /* Transit Delay */
	    if (lci->lci_tdelay != 0) {
		*c++ = X25_FACILITY_TDELAY;
		PUTSHORT(c, lci->lci_tdelay);
		c += 2; /* 2 octets */
	    }
	    /*
             * Facility Marker and DDN specific marker
             * is needed for the following facilities.
             */
	    if (lci->lci_prec != X25_UNASSIGNED_PREC && 
		 IS_NON_BFE_DDN(hwidb)) {
		*c++ = X25_FACILITY_MARKER;
		*c++ = X25_FACILITY_MARKER_LOCAL;
		*c++ = X25_FACILITY_DDN_STANDARD;
		*c++ = X25_FACILITY_DDN_STANDARD_ON;
		*c++ = X25_FACILITY_DDN_CALL_PREC;
		*c++ = lci->lci_prec;
	    }
	    /*
	     * Facility Marker and CCITT specific marker
	     * is needed for the following facilities.
	     */
	     if (CCITT_marker(lci)) {
		*c++ = X25_FACILITY_MARKER;
		*c++ = X25_FACILITY_MARKER_CCITT;

		/* NSAP(s) */
		if (lci->lci_dst_nsap.length) {
		    *c++ = X25_FACILITY_CALLED_EXT;
		    /* length + digits + address */
		    *c++ = lci->lci_dst_nsap.length + 1;
		    *c++ = lci->lci_dst_nsap_digits;
		    bcopy(&lci->lci_dst_nsap.cmns_addr[1], c,
			lci->lci_dst_nsap.length);
		    c += lci->lci_dst_nsap.length;
		}
		if (lci->lci_src_nsap.length) {
		    *c++ = X25_FACILITY_CALLING_EXT;
		    /* length + digits + address */
		    *c++ = lci->lci_src_nsap.length + 1;
		    *c++ = lci->lci_src_nsap_digits;
		    bcopy(&lci->lci_src_nsap.cmns_addr[1], c,
			lci->lci_src_nsap.length);
		    c += lci->lci_src_nsap.length;
		}
	    }
	} else {
	    /*
             * Facility Marker and DDN specific marker
             * is needed for the following facilities.
             */
	    if (lci->lci_prec != X25_UNASSIGNED_PREC && 
		 IS_NON_BFE_DDN(hwidb)) {
		*c++ = X25_FACILITY_MARKER;
		*c++ = X25_FACILITY_MARKER_LOCAL;
		*c++ = X25_FACILITY_DDN_STANDARD;
		*c++ = X25_FACILITY_DDN_STANDARD_ON;
		*c++ = X25_FACILITY_DDN_CALL_PREC;
		*c++ = lci->lci_prec;
            }
	}

	/* move user data into packet */
	if (cud_len)
	    bcopy(cud, c, cud_len);
    } else if (call_conf_pad) {
	c[0] = 0;       /* address block */
	c[1] = 0;       /* facility block */
    }


    if (x25_debug || x25event_debug)
	x25_prpack(hwidb, pak, lci, 'O');

    (void) x25_sendpak(hwidb, pak, lci ? lci->lci_llc2 : NULL);
}

/*
 * Send an X25 flow control packet
 */

void x25_sendflow (
    register hwidbtype *hwidb,
    register lcitype *lci,
    uchar type)
{
    register paktype *pak;
    register packettype *p;
    register uchar *c;

    if (lci->lci_pbp_rnr)
	return;
    if (lci->lci_pbp_local_ack) {	/*near outer node function*/
	x25_pbp_update_lci_ack(hwidb, lci);
    }

    pak = getx25(hwidb, lci, 0, TRUE);
    if (!pak)
	return;
    p = (packettype *)pak->datagramstart;
    p->x25_q = 0;
    p->x25_d = 0;
    p->x25_sn = (hwidb->x25_modulo == X25_MOD8) ? X25_MODULO_8 : X25_MODULO_128;
    p->x25_lcg = LCG(lci->lci_lci);
    p->x25_lcn = LCN(lci->lci_lci);
    p->x25_pti = type;
    if (!lci->lci_pbp_local_ack &&
	(lci->lci_linktype != LINK_X25 &&
	 lci->lci_linktype != LINK_X25TCP &&
	 lci->lci_linktype != LINK_PAD &&
	 lci->lci_linktype != LINK_X25SERVICE))
	lci->lci_ack = lci->lci_pr;
    if (hwidb->x25_modulo == X25_MOD8)
	p->x25_pti |= (lci->lci_ack << 5);
    else {
	c = (uchar *)p + X25HEADERBYTES;
	*c = lci->lci_ack << 1;
    }
    if (x25_debug)
	x25_prpack(hwidb, pak, lci, 'O');
    (void) x25_sendpak(hwidb, pak, lci->lci_llc2);
}

/*
 * x25_sendcall
 * Send CALL type packet
 */

boolean x25_sendcall (
    register lcitype *lci,
    uchar *cudptr,
    uint cudlen)
{
    register paktype *pak;
    register x25map *map;
    hwidbtype *hwidb;
    register packettype *p;
    uint bytes, facilitylen;
    uchar *c;
    uchar ablock[2 * X25_ADDRESS_LENGTH];
    uchar ablock_len_field;
    register considbtype *considb;
    uchar pid[X25_MAX_CUD_PID];
    uchar pid_len;

    considb = get_lci_considb(lci);

    hwidb = lci->lci_idb->hwptr;
    map = lci->lci_map;

    /*
     *  encode the addresses in the lci structure
     */

    ablock_len_field = 0;

    /*
     *  if suppressed, clear the destination address, otherwise format it
     */
    if (hwidb->x25_flags & X25_SUPPR_DST_ADDR) {
	memset(&lci->lci_dst_addr, 0, sizeof(lci->lci_dst_addr));
	lci->lci_dst_addr.type = ADDR_X121;
    } else {
	bcopy(lci->lci_dst_addr.x121_addr, ablock, lci->lci_dst_addr.length);
	ablock_len_field = lci->lci_dst_addr.length & 0x0f;
    }

    /*
     *  handle a suppressed or substituted source address and formatting
     */
    if (hwidb->x25_flags & X25_SUPPR_SRC_ADDR) {
	memset(&lci->lci_src_addr, 0, sizeof(lci->lci_src_addr));
	lci->lci_src_addr.type = ADDR_X121;
    } else {
	if (hwidb->x25_flags & X25_SUBST_SOURCE) {
	    bcopy(hwidb->x25_address, &lci->lci_src_addr, sizeof(addrtype));
	}
	bcopy(lci->lci_src_addr.x121_addr,
	      &ablock[ablock_len_field & 0x0f], lci->lci_src_addr.length);
	ablock_len_field |= (lci->lci_src_addr.length << 4) & 0xf0;
    }

    /*
     *  account for the address block and facility length
     */
    bytes = X25_ABLOCK_LEN(ablock_len_field) + 1;

    /*
     *  compute the length of the facilities to encode
     */
    facilitylen = 0;
    if ((lci->lci_linktype == LINK_X25) ||
	(lci->lci_linktype == LINK_X25TCP)) {
	if (lci->lci_win != hwidb->x25_win || lci->lci_wout != hwidb->x25_wout)
	    facilitylen += 3;
	if (lci->lci_reverse || lci->lci_fast)
	    facilitylen += 2;
	if (lci->lci_cug != 0)
	    facilitylen += 2;
	if (lci->lci_tput_out || lci->lci_tput_in)
	    facilitylen += 2;
	if (lci->lci_pin != hwidb->x25_pin || lci->lci_pout != hwidb->x25_pout)
	    facilitylen += 3;
	if (lci->lci_nuid != NULL && lci->lci_nuidlen > 0 &&
	    ISDTE(hwidb, considb))
	    facilitylen += 2 + lci->lci_nuidlen;
	/* Transit Delay */
        if (lci->lci_tdelay != 0)
            facilitylen += 3;
    } else {
	if ((map && map->map_fac.win && map->map_fac.wout) ||
	    (hwidb->x25_fwin && hwidb->x25_fwout) ||
	    (lci->lci_win != hwidb->x25_win) ||
	    (lci->lci_wout != hwidb->x25_wout))
	    facilitylen += 3;
	if ((map && map->map_fac.pin && map->map_fac.pout) ||
	    (hwidb->x25_fpin && hwidb->x25_fpout) ||
	    (lci->lci_pin != hwidb->x25_pin) ||
	    (lci->lci_pout != hwidb->x25_pout))
	    facilitylen += 3;
	if (!((lci->lci_linktype == LINK_PAD) && 
	    (lci->lci_reverse & TRANSF_X25_NOREVERSE)) &&
	    (((map && map->map_fac.rev) || hwidb->x25_freverse) ||
	    (lci->lci_reverse & TRANSF_X25_REVERSE))) {
	        facilitylen += 2;
	}
	if ((map && map->map_fac.cug) || hwidb->x25_fcug)
	    facilitylen += 2;
	if ((map && map->map_fac.tput_in && map->map_fac.tput_out) ||
	    (hwidb->x25_tput_in && hwidb->x25_tput_out))
	    facilitylen += 2;
	/* Transit Delay */
	if ((map && map->map_fac.tdelay) || hwidb->x25_tdelay)
	    facilitylen += 3;
	if (((map && map->map_fac.rpoa != NULL) || hwidb->x25_rpoa != NULL) &&
	    (!hwidb->x25_strict_ccitt_facil || ISDTE(hwidb, considb))) {
	    rpoatype *rpoa = (map && map->map_fac.rpoa) ?
		map->map_fac.rpoa : hwidb->x25_rpoa;
	    if (rpoa->nrpoa > 1)
		facilitylen += 2 + 2*rpoa->nrpoa;
	    else
		facilitylen += 3;
	}
	if (map && map->map_fac.nuid != NULL && map->map_fac.nuid_len > 0 &&
	    ISDTE(hwidb, considb))
	    facilitylen += 2 + map->map_fac.nuid_len;
    }

    /*
     * local facilities
     */
    if ((lci->lci_linktype == LINK_X25) ||
	(lci->lci_linktype == LINK_X25TCP) ||
	(lci->lci_linktype == LINK_IP)) {
	facilitylen += x25_ip_call_size(lci);
    }

    /*
     * CCITT-specific facilities
     */
    if (CCITT_marker(lci)) {
	facilitylen += 2; 
        /* NSAP called facility */
        if (lci->lci_dst_nsap.length)
            facilitylen +=  2 + 1 + lci->lci_dst_nsap.length;
        /* NSAP calling facility */
        if (lci->lci_src_nsap.length)
            facilitylen +=  2 + 1 + lci->lci_src_nsap.length;
    }

    x25_encaps_form_cud_pid(lci, pid, &pid_len);

    /*
     * initialize packet by packet compression
     */
    if (map && (map->map_services & X25_MAPS_ENCAPS) &&
	!x25_pbp_init(lci, TRUE))
	return(FALSE);

    bytes += facilitylen;		/* facility specific stuff */
    bytes += pid_len;
    bytes += cudlen;

    pak = getx25(hwidb, lci, bytes, FALSE);
    if (!pak)
	return FALSE;

    lci->lci_state = ISDTE(hwidb, considb) ? X25_P2 : X25_P3;
    TIMER_START(lci->lci_timer, hwidb->x25_t1 * ONESEC);
    x25_addtimer(lci);
    p = (packettype *)pak->datagramstart;
    p->x25_q = 0;
    p->x25_d = lci->lci_dbit ? 1 : 0;
    p->x25_sn = (hwidb->x25_modulo == X25_MOD8) ? X25_MODULO_8 : X25_MODULO_128;
    p->x25_lcg = LCG(lci->lci_lci);
    p->x25_lcn = LCN(lci->lci_lci);
    p->x25_pti = X25_CALL_REQUEST;
    c = (uchar *)p + X25HEADERBYTES;

    /*
     * Create address block
     */
    *c = ablock_len_field;
    chartobcd(ablock, &c[1],
	      ((ablock_len_field >> 4) & 0x0f) + (ablock_len_field & 0x0f));
    c += X25_ABLOCK_LEN(ablock_len_field);

    /*
     * Establish facility length byte
     */
    *c++ = facilitylen;
    if ((lci->lci_linktype == LINK_X25) ||
	(lci->lci_linktype == LINK_X25TCP)) {
	if (lci->lci_win != hwidb->x25_win ||
	    lci->lci_wout != hwidb->x25_wout) {
	    *c++ = X25_FACILITY_FLOW_WINDOW;
	    *c++ = lci->lci_win;	/* from called */
	    *c++ = lci->lci_wout;	/* from calling */
	}
	if (lci->lci_reverse || lci->lci_fast) {
	    *c++ = X25_FACILITY_FAST_SELECT;
	    *c = lci->lci_fast;
	    if (lci->lci_reverse)
		*c |= X25_FACILITY_REVERSE_CHARGE_ON;
	    c++;
	}
	if (lci->lci_cug != 0) {
	    *c++ = X25_FACILITY_CUG_BASIC;
	    *c++ = lci->lci_cug;
	}
	if (lci->lci_tput_out || lci->lci_tput_in) {
	    *c++ = X25_FACILITY_THROUGHPUT;
	    *c++ = ((lci->lci_tput_in << 4) & 0xf0) |
		   (lci->lci_tput_out & 0x0f);
	}
	if (lci->lci_pin != hwidb->x25_pin ||
	    lci->lci_pout != hwidb->x25_pout) {
	    *c++ = X25_FACILITY_FLOW_PACKET;
	    *c++ = lci->lci_pin;	/* from called */
	    *c++ = lci->lci_pout;	/* from calling */
	}
	if (lci->lci_nuid != NULL && lci->lci_nuidlen > 0 &&
	    ISDTE(hwidb, considb)) {
	    *c++ = X25_FACILITY_NETWORK_USER_ID;
	    *c++ = lci->lci_nuidlen;
	    bcopy(lci->lci_nuid, c, lci->lci_nuidlen);
	    c += lci->lci_nuidlen;
	}
	/* Transit Delay */
	if (lci->lci_tdelay != 0) {
	    *c++ = X25_FACILITY_TDELAY;
	    PUTSHORT(c, lci->lci_tdelay);
	    c += 2; /* 2 octets */
	}
    } else {
	if ((map && map->map_fac.win && map->map_fac.wout) ||
	    (hwidb->x25_fwin && hwidb->x25_fwout) ||
	    (lci->lci_win != hwidb->x25_win) ||
	    (lci->lci_wout != hwidb->x25_wout)) {
	    *c++ = X25_FACILITY_FLOW_WINDOW;
	    *c++ = lci->lci_win;	/* from called */
	    *c++ = lci->lci_wout;	/* from calling */
	}
	if ((map && map->map_fac.pin && map->map_fac.pout) ||
	    (hwidb->x25_fpin && hwidb->x25_fpout) ||
	    (lci->lci_pin != hwidb->x25_pin) ||
	    (lci->lci_pout != hwidb->x25_pout)) {
	    *c++ = X25_FACILITY_FLOW_PACKET;
	    *c++ = lci->lci_pin;	/* from called */
	    *c++ = lci->lci_pout;	/* from calling */
	}
	if (!((lci->lci_linktype == LINK_PAD) && 
	    (lci->lci_reverse & TRANSF_X25_NOREVERSE)) &&
	    (((map && map->map_fac.rev) || hwidb->x25_freverse) ||
	    (lci->lci_reverse & TRANSF_X25_REVERSE))) {
	    *c++ = X25_FACILITY_REVERSE_CHARGING;
	    *c++ = X25_FACILITY_REVERSE_CHARGE_ON;
	}
	if ((map && map->map_fac.cug) || hwidb->x25_fcug) {
	    uint cug = (map && map->map_fac.cug) ?
			map->map_fac.cug : hwidb->x25_fcug;

	    *c++ = X25_FACILITY_CUG_BASIC;
	    *c++ = (((cug / 10) & 0xf) << 4) |
		((cug - ((cug / 10) * 10)) & 0xf);
	}
	if ((map && map->map_fac.tput_in && map->map_fac.tput_out) ||
	    (hwidb->x25_tput_in && hwidb->x25_tput_out)) {
	    uint tput = (map && map->map_fac.tput_in && map->map_fac.tput_out) ?
		((map->map_fac.tput_in << 4) | map->map_fac.tput_out) :
		((hwidb->x25_tput_in << 4) | hwidb->x25_tput_out);
	    *c++ = X25_FACILITY_THROUGHPUT;
	    *c++ = tput;
	}
	/* Transit Delay */
	if ((map && map->map_fac.tdelay) || hwidb->x25_tdelay) {
		ushort td;
		*c++ = X25_FACILITY_TDELAY;
	        td = (map && map->map_fac.tdelay) ?
			map->map_fac.tdelay : hwidb->x25_tdelay;
	    	PUTSHORT(c, td);
	    	c += 2; /* 2 octets */
	}
	if (((map && map->map_fac.rpoa != NULL) || hwidb->x25_rpoa != NULL) &&
	    (!hwidb->x25_strict_ccitt_facil || ISDTE(hwidb, considb))) {
	    uint i;
	    rpoatype *rpoa = (map && map->map_fac.rpoa) ? map->map_fac.rpoa :
		hwidb->x25_rpoa;

	    if (rpoa->nrpoa > 1) {
		*c++ = X25_FACILITY_RPOA_EXTENDED;
		*c++ = rpoa->nrpoa * 2;
		for (i = 0; i < rpoa->nrpoa; i++) {
		    *c++ = (((rpoa->rpoa[i] / 1000) & 0xf) << 4) |
			(((rpoa->rpoa[i] - ((rpoa->rpoa[i] / 1000) * 1000)) /
			100) & 0xf);
		    *c++ = ((((rpoa->rpoa[i] - ((rpoa->rpoa[i] / 100) * 100)) /
		    	10) & 0xf) << 4) |
			((rpoa->rpoa[i] - ((rpoa->rpoa[i] / 10) * 10)) & 0xf);
		}
	    } else {
		*c++ = X25_FACILITY_RPOA_BASIC;
		*c++ = (((rpoa->rpoa[0] / 1000) & 0xf) << 4) |
		    (((rpoa->rpoa[0] - ((rpoa->rpoa[0] / 1000) * 1000)) /
		    100) & 0xf);
		*c++ = ((((rpoa->rpoa[0] - ((rpoa->rpoa[0] / 100) * 100)) /
		    10) & 0xf) << 4) |
		    ((rpoa->rpoa[0] - ((rpoa->rpoa[0] / 10) * 10)) & 0xf);
	    }
	}
	if (map && map->map_fac.nuid != NULL && map->map_fac.nuid_len > 0 &&
	    ISDTE(hwidb, considb)) {
	    *c++ = X25_FACILITY_NETWORK_USER_ID;
	    *c++ = map->map_fac.nuid_len;
	    bcopy(map->map_fac.nuid, c, map->map_fac.nuid_len);
	    c += map->map_fac.nuid_len;
	}
    }

    /*
     * append any local facilities
     */
    if ((lci->lci_linktype == LINK_X25) ||
	(lci->lci_linktype == LINK_X25TCP) ||
	(lci->lci_linktype == LINK_IP)) {
	c += x25_ip_facil(lci, c);
    }

    /*
     * append any CCITT-specific facilities
     */
    if (CCITT_marker(lci)) {
	*c++ = X25_FACILITY_MARKER;
	*c++ = X25_FACILITY_MARKER_CCITT;

	/* NSAP called facility */
	if (lci->lci_dst_nsap.length) {
	    *c++ = X25_FACILITY_CALLED_EXT;
	    /* length + semi-octets + address */
	    c[0] = lci->lci_dst_nsap.length + 1;
	    c[1] = lci->lci_dst_nsap_digits;
	    bcopy(&lci->lci_dst_nsap.cmns_addr[1], &c[2],
		  lci->lci_dst_nsap.length);
	    c += (2 + lci->lci_dst_nsap.length);
	}

	/* NSAP calling facility */
	if (lci->lci_src_nsap.length) {
	    *c++ = X25_FACILITY_CALLING_EXT;
	    /* length + semi-octet + address */
	    c[0] = lci->lci_src_nsap.length + 1;
	    c[1] = lci->lci_src_nsap_digits;
	    bcopy(&lci->lci_src_nsap.cmns_addr[1], &c[2],
		  lci->lci_src_nsap.length);
	    c += (2 + lci->lci_src_nsap.length);
	}
    }

    if (pid_len) {
	bcopy(pid, c, pid_len);		/* append any protocol ID */
	c += pid_len;
    }

    if (cudlen) {			/* append any CUD */
	bcopy(cudptr, c, cudlen);
	c += cudlen;
    }

    if (x25_debug || x25event_debug)
	x25_prpack(hwidb, pak, lci, 'O');

    hwidb->x25_callssent++;
    return(x25_sendpak(hwidb, pak, lci->lci_llc2));
}

/*
 * x25_input
 *
 * Receives all incoming X25 packets, called from device interrupt level.
 * Passes all Data, RR and RNR packets up; processes Call Confirm and
 * Interrupt packets (since data might immediately follow); discards all
 * invalid packets, Registration and Diagnostic packets; all other packet
 * types (Restart, Restart Confirm, Call, Reset, Reset Confirm, Clear,
 * Clear Confirm, Interrupt Confirm) are queued for the x25_timer task
 * to handle.
 */

void x25_input (
    register hwidbtype *hwidb,
    register paktype *pak)
{
    register long lci_num;
    int pr, mbit, ps, len;
    register lcitype *lci;
    register packettype *p = (packettype *)pak->datagramstart;
    llctype *llc2;
    considbtype *considb = NULL;


    llc2 = pak->llc2_cb;
    if (ISCMNS_IEEE(hwidb) && validmem(llc2))
	considb = llc2->considb;
    lci = NULL;
    lci_num = LCI(p);
    pak->linktype = LINK_X25;
    pak->x25_flags = 0;

    /*
     *  if the packet is too short to have a packet type identifier,
     *  set packet type identifier value to X25_DATA--this allows
     *  us to check it with impunity and to reach the appropriate
     *  CCITT Annex C state table to handle the error
     */

    if (pak->datagramsize == X25MINDATA)
	p->x25_pti = X25_DATA;

    if (hwidb->state != IDBS_UP) {
	datagram_done(pak);
    } else if (!ISIEEEMSK(hwidb) && (!hwidb->lapb_upstate)) {
	datagram_done(pak);
    } else if (pak->datagramsize < X25MINDATA) {
	if (x25_debug)
	    x25_prpack(hwidb, pak, NULL, 'I');
	if (ISDCE(hwidb, considb))
	    x25_senddiag(hwidb, pak, X25_DIAG_PACKET_TOO_SHORT, NULL);
        else
            DteProtocolViolation(lci, hwidb, X25_DIAG_PACKET_TOO_SHORT);

	datagram_done(pak);
    } else if (hwidb->x25_admin_modulo != hwidb->x25_modulo &&
	       (p->x25_pti == X25_RESTART ||
		p->x25_pti == X25_RESTART_CONFIRMATION)) {
	/*
	 *  don't test for a bad GFI if we're trying to change modulo
	 */
	p_enqueue(&x25iQ, pak);
    } else if (x25_badgfi(p->x25_q, p->x25_d, p->x25_sn, p->x25_pti,
			  hwidb->x25_modulo)) {
	if (x25_debug)
	    x25_prpack(hwidb, pak, NULL, 'I');
	if (ISDCE(hwidb, considb))
	    x25_senddiag(hwidb, pak, X25_DIAG_INVALID_GFI, NULL);
        else
            DteProtocolViolation(lci, hwidb, X25_DIAG_INVALID_GFI);

	datagram_done(pak);
    } else if (p->x25_pti == X25_DIAGNOSTIC) {
	if (x25_debug || x25event_debug)
	    x25_prpack(hwidb, pak, NULL, 'I');
	/*
	 *  ignore invalid diagnostic packets per ISO 8208 (CCITT is mute)
	 */
	if (lci_num == 0 &&
	    pak->datagramsize > X25HEADERBYTES &&
	    pak->datagramsize <= (X25HEADERBYTES + X25_DIAG_DATA)) {

	    hwidb->x25_diagsrcvd++;
	    hwidb->x25_errors_accused++;

            /* NmEvent Hook 8 */
            X25EventRx(hwidb, lci, (packettype*)pak->datagramstart);

	    if (ISBFE(hwidb))
		x25_bfe_diag(hwidb, pak);
	}
	datagram_done(pak);
    } else if (lci_num &&
	!(lci = x25_lci_lookup(hwidb, lci_num, pak->llc2_cb)) &&
	!(lci_num >= hwidb->x25_lic && lci_num <= hwidb->x25_hic) &&
	!(lci_num >= hwidb->x25_ltc && lci_num <= hwidb->x25_htc) &&
	!(lci_num >= hwidb->x25_loc && lci_num <= hwidb->x25_hoc)) {

	/*
	 *	packet is on an unassigned VC
	 *	(a PVC packet won't get here because the x25_lci_lookup()
	 *	will succeed)
	 */
	if (x25_debug)
	    x25_prpack(hwidb, pak, NULL, 'I');
	if (ISDCE(hwidb, considb))
	    x25_senddiag(hwidb, pak, X25_DIAG_PACKET_UNASS_LC, NULL);
        else
            DteProtocolViolation(lci, hwidb, X25_DIAG_PACKET_UNASS_LC);
         
	datagram_done(pak);
    } else if (lci_num && lci &&
	((lci->lci_state == X25_P4) || (lci->lci_state == X25_D1)) &&
	x25_data(pak, &pr, &mbit, &ps, &len)) {
	if (x25_debug)
	    x25_prpack(hwidb, pak, lci, 'I');
	if (ISBFE(hwidb)) {
	    /*
	     *  BFE does not support the D or Q bit procedures
	     */
	    p->x25_q = 0;
	    p->x25_d = 0;
	}
	if (lci->lci_pbp_rnr)	/*we sent an RNR. defer processing */
	    p_enqueue(&lci->lci_pbp_rnr_deferredQ, pak);
	else
	    x25_process_other(hwidb, lci, pak, pr, mbit, ps, len);
    } else if (lci_num && lci &&
	((lci->lci_state == X25_D2) || (lci->lci_state == X25_D3)) &&
	(x25_data(pak, &pr, &mbit, &ps, &len) ||
	 (p->x25_pti == X25_INTERRUPT) ||
	 (p->x25_pti == X25_INTERRUPT_CONFIRMATION))) {
	if (x25_debug)
	    x25_prpack(hwidb, pak, lci, 'I');
	datagram_done(pak);
    } else if (lci_num && lci &&
	((((lci->lci_state == X25_P2) || (lci->lci_state == X25_P3)) &&
	  (p->x25_pti == X25_CALL_CONNECTED)) ||
	 (((lci->lci_state == X25_D2) || (lci->lci_state == X25_D3)) &&
	  (p->x25_pti == X25_RESET_CONFIRMATION)) ||
	 (((lci->lci_state == X25_P4) || (lci->lci_state == X25_D1)) &&
	  (p->x25_pti == X25_INTERRUPT)))) {
	/*
	 *  Process CALL CONNECTED and RESET CONFIRM packets now, otherwise
	 *  we can't process any DATA or flow control packets that follow,
	 *  and expedite INTERRUPT packet processing
	 */
	(*x25_state_table[hwidb->x25_state])(hwidb, pak, lci);
    } else {
	/*
	 * All other packets are handled at task time
	 */
	p_enqueue(&x25iQ, pak);
    }
}


/*
 * x25_lci_low_water
 *
 * lci hold q is nearly empty, congestion on x.25 link can be 
 * considered relieved.
 */

static boolean x25_lci_low_water(lcitype *lci)
{
    if (lci->lci_holdQ.count <= X25_LCI_LOW_WATER)
	return(TRUE);
    else
	return (FALSE);
}

/*
 * x25_process_other
 * Process RR, etc. packets & begin to process DATA packets
 * (getting the decompression buffer if required.  if the decompression
 * buffer is unavailable, then send an RNR and start the timer to get the
 * buffer. continuation of DATA packet processing is done by
 * x25_process_data().
 */

void x25_process_other (
    hwidbtype *hwidb,
    lcitype *lci,
    paktype *pak,
    int pr,
    int mbit,
    int ps,
    int len)
{
    register paktype *newpak = NULL;
    packettype *p = (packettype *)pak->datagramstart;
    static paktype fakepak;
    int newpaksize;
    boolean dbit;
    x25_pbp_hdr_t *x25_pbp_hdr_p;
    x25paktype *p_old, *p_new;

    /*
     * Update pak->if_input to what's in the LCI table.  The current contents
     * point to the first swIDB;  the LCI table has the appropriate swIDB.
     */
    if (lci->lci_idb)
	pak->if_input = lci->lci_idb;

    GET_TIMESTAMP(lci->lci_lastinput);
    if (lci->lci_state == X25_P4)
	lci->lci_state = X25_D1;
    if (lci->lci_idle) {
	if (TIMER_RUNNING(lci->lci_timer))
	    p_unqueue(&x25tQ, lci);
	TIMER_START(lci->lci_timer, lci->lci_idle);
	x25_addtimer(lci);
    }
    if (pak->datagramsize < len) {
	x25_sendcause(hwidb, lci, X25_RESET,
		X25_RESET_LOCAL_PROC_ERR, X25_DIAG_PACKET_TOO_SHORT, NULL);
	datagram_done(pak);
    } else if (!x25_ack(hwidb, lci, pr)) {
	datagram_done(pak);
    } else {
	if ((p->x25_pti & X25_DATA_MASK) == X25_DATA) {
	    hwidb->x25_data_rx++;
	    lci->lci_packetsrcvd++;
	    /*
	     *	check if the packet has the wrong sequence number,
	     *	is too long, is too short, or
	     *	violates the incoming window
	     */
	    if ((ps != lci->lci_pr) ||
		(pak->datagramsize - len > (1 << lci->lci_pin)) ||
		(ps == (lci->lci_ack + lci->lci_win) % hwidb->x25_modulo)) {
		if (pak->datagramsize - len > (1 << lci->lci_pin))
		    x25_sendcause(hwidb, lci, X25_RESET,
		      X25_RESET_LOCAL_PROC_ERR, X25_DIAG_PACKET_TOO_LONG, NULL);
		else
		    x25_sendcause(hwidb, lci, X25_RESET,
		      X25_RESET_LOCAL_PROC_ERR, X25_DIAG_INVALID_PS, NULL);
		datagram_done(pak);
		return;
	    }

	    /*
	     *  check if the data has an inconsistant Q bit setting
	     */
	    if ((lci->lci_data_flags & X25_DATA_M_RX) &&
		(!p->x25_q != !(lci->lci_data_flags & X25_DATA_Q_RX))) {
		x25_sendcause(hwidb, lci, X25_RESET,
			X25_RESET_LOCAL_PROC_ERR, X25_DIAG_INCONSISTENT_Q_BIT,
			NULL);
		datagram_done(pak);
		return;
	    }

	    /*
	     *  IETF encapsulations disallow Q bit
	     */
	    if ((pak->x25_flags & PAK_X25_QBIT) &&
		lci->lci_map &&
		(lci->lci_map->map_services & X25_MAPS_ENCAPS) &&
		((lci->lci_cud_pid_method == X25_CUD_PID_IETF) ||
		 (lci->lci_cud_pid_method == X25_CUD_PID_SNAP) ||
		 (lci->lci_cud_pid_method == X25_CUD_PID_MULTI))) {
		x25_sendcause(hwidb, lci, X25_CLEAR,
			X25_CLEAR_INCOMPAT_DEST, X25_DIAG_INCONSISTENT_Q_BIT,
			NULL);
		datagram_done(pak);
		return;
	    }

	    /*
	     *  check if the data has an unauthorized D bit set
	     */
	    if (p->x25_d && !lci->lci_dbit) {
		x25_sendcause(hwidb, lci, X25_RESET,
			X25_RESET_LOCAL_PROC_ERR, X25_DIAG_NO_INFO, NULL);
		datagram_done(pak);
		return;
	    }

	    lci->lci_data_flags = (mbit ? X25_DATA_M_RX : 0) |
				  (p->x25_q ? X25_DATA_Q_RX : 0);
	    if (lci->lci_data_flags & X25_DATA_Q_RX)
		pak->x25_flags |= PAK_X25_QBIT;
	    dbit = p->x25_d;
	    if (dbit)
		pak->x25_flags |= PAK_X25_DBIT;

	    if (lci->lci_data_pid_method == X25_DATA_PID_NONE) {
		/*
		 *  x25_forward_() routines require the packet be
		 *  identified by source, so force a packet destined
		 *  for an XOT link to an X25 packet type
		 */
		if (lci->lci_linktype == LINK_X25TCP)
		    pak->linktype = LINK_X25;
		else
		    pak->linktype = lci->lci_linktype;
	    } else
		pak->linktype = LINK_ILLEGAL;

	    /*
	     *  remove the X.25 information from the packet
	     */
	    pak->datagramsize -= len;
	    pak->datagramstart += len;
	    pak->network_start = pak->datagramstart;
	    pak->encsize = 0;

	    lci->lci_bytesrcvd += pak->datagramsize;
	    lci->lci_pr = ++lci->lci_pr % hwidb->x25_modulo;

	    if (mbit && (! dbit) &&
		(pak->datagramsize < (1 << lci->lci_pin))) {
		errmsg(&msgsym(BADMBIT, X25),
			hwidb->hw_namestring, lci->lci_lci);
	    }

	    /*
	     * never pbp compress/decompress lapb or x25 service
	     * (lci_pbp_enabled will never be true for these).		
	     * decompress remote switched data here (no compressed
	     * data on LAN).  locally switched is decompressed if
	     * the other interface is not doing pbp compression.
	     * all encasulations are decompressed here
	     * decompression occurs in x25_process_data()
	     */

	    if (lci->lci_pbp_compress && lci->lci_pbp_depak == NULL) {
		x25_pbp_hdr_p = (x25_pbp_hdr_t *) pak->datagramstart;
		if (x25_pbp_hdr_p->version != X25_PBP_STAC) {
		    errmsg(&msgsym(VERSIONBAD, X25), lci->lci_idb->namestring,
			   lci->lci_lci, X25_PBP_STAC,
			   x25_pbp_hdr_p->version);
		    x25_sendcause(hwidb, lci, X25_RESET,
				  X25_RESET_INCOMPAT_DEST,
				  X25_DIAG_NO_INFO, NULL);
		    datagram_done(pak);
		    return;
  		}
		if (x25_pbp_hdr_p->xmt_paks != lci->lci_pbp_rcv_sync) {
		    errmsg(&msgsym(SYNCBAD, X25), lci->lci_idb->namestring,
			   lci->lci_lci, lci->lci_pbp_rcv_sync,
			   x25_pbp_hdr_p->xmt_paks);
		    x25_sendcause(hwidb, lci, X25_RESET,
				  X25_RESET_REMOTE_PROC_ERR,
				  X25_DIAG_NO_INFO, NULL);
		    datagram_done(pak);
		    return;
		}
		newpaksize = GETSHORT(&x25_pbp_hdr_p->org_size);
		newpak = getbuffer(newpaksize + X25_PBP_STAC_FUDGE);
		if (++lci->lci_pbp_rcv_sync == 0)
		    lci->lci_pbp_rcv_sync = 1;

		if (newpak == NULL) {
		    /*can't decompress, but can't drop packet.  so we send
		     *an RNR and try and get a buffer in x25_timer().  when
		     *we get a buffer we process these deferred pak's.
		     *after we send the RNR we can still get more input DATA.
		     *a bit twisted.
  		     */
		    x25_sendflow(hwidb, lci, X25_RNR);
		    lci->lci_pbp_rnr = TRUE;
		    lci->lci_pbp_rnr_ticks = X25_PBP_MAX_RNR_TICKS;
		    lci->lci_pbp_need_pak_size = newpaksize;
		    pak->datagramsize += len;
		    pak->datagramstart -= len;
		    p_enqueue(&lci->lci_pbp_rnr_deferredQ, pak);

		    /* start timer for getbuffer */
		    if (TIMER_RUNNING(lci->lci_timer))
			p_unqueue(&x25tQ, lci);
		    TIMER_START(lci->lci_timer, ONESEC);
		    x25_addtimer(lci);
		    return;
  		}
  
		lci->lci_pbp_depak = newpak;
		pak_copy(pak, newpak, 0);
		newpak->datagramsize = newpaksize;

		p_old = (x25paktype *) (pak->datagramstart - len);
		p_new = (x25paktype *) (newpak->datagramstart - len);
		p_new->x25GfiLcg = p_old->x25GfiLcg;
		p_new->x25Lcn = p_old->x25Lcn;
		p_new->x25Pti = p_old->x25Pti & ~X25_MBIT_MASK;

		pak->datagramsize -= X25_PBP_HEADERBYTES;
		pak->datagramstart += X25_PBP_HEADERBYTES;
  
	    }
	    x25_process_data(hwidb, lci, pak, pr, mbit, ps, len);
	} else {
	    uint type;

	    /*
	     *	Flow Control Packet (RR, RNR, REJ) handling
	     */

	    /*
	     *	check if the packet is too long
	     */
	    if (pak->datagramsize > len) {
		x25_sendcause(hwidb, lci, X25_RESET,
			X25_RESET_LOCAL_PROC_ERR, X25_DIAG_PACKET_TOO_LONG,
			NULL);
		datagram_done(pak);
	    } else {
		if (len == X25HEADERBYTES)
		    type = p->x25_pti & X25_FLOW_MASK;
		else
		    type = p->x25_pti;
		switch (type) {
		  case X25_RR:
		    lci->lci_busy = FALSE;
		    if (!QUEUEEMPTY(&lci->lci_holdQ)) {
			fakepak.datagramsize = 0;
			fakepak.x25_cb = lci;
			x25_output(hwidb, &fakepak, TAIL);
			if (x25_lci_low_water(lci))
			    /* congestion relieved */
			    reg_invoke_media_x25_notify_event(
				lci->lci_linktype, lci,
				X25_UNQUENCH_EVENT, 0, 0, NULL);
		    }
		    if (lci->lci_swdata && !lci->lci_pbp_local_ack)
			x25_forward_ack(lci, pak, p->x25_sn, pr);
		    else
			datagram_done(pak);
		    break;
		  case X25_RNR:
		    lci->lci_rnrrcvd++;
		    lci->lci_busy = TRUE;
		    if (lci->lci_swdata && !lci->lci_pbp_local_ack)
			x25_forward_ack(lci, pak, p->x25_sn, pr);
		    else
			datagram_done(pak);
		    break;
		  case X25_DTE_REJ:
		    lci->lci_rejrcvd++;
		  default:			/* invalid pti for modulo 128 */
		    x25_sendcause(hwidb, lci, X25_RESET,
			X25_RESET_LOCAL_PROC_ERR, X25_DIAG_UNIDENT_PACKET,
			NULL);
		    datagram_done(pak);
		    break;
		}
	    }
	}
    }
}

/*
 *  x25_ok_to_append_data
 *  Determine if a given number of bytes can be appended to a packet
 */
static boolean x25_ok_to_append_data (paktype *pak, ushort new_data_cnt)
{
    ushort size_left;

    size_left = pak_get_pool_size(pak) -
		((pak->datagramstart - pak->data_area) + pak->datagramsize);
    return(size_left >= new_data_cnt);
}

/*
 * x25_process_data() Continue to process the DATA packet to reassembly if
 * required & decompression.
 */

void x25_process_data (
    hwidbtype *hwidb,
    lcitype *lci,
    paktype *pak,
    int pr,
    int mbit,
    int ps,
    int len)
{
    register paktype *newpak;
    register uchar *cp;
    static paktype fakepak;
    idbtype *swidb;
    /* datagram_done(pak) will occur, so save the qualifiers */
    boolean dbit = pak->x25_flags & PAK_X25_DBIT;
    boolean qbit = pak->x25_flags & PAK_X25_QBIT;
    int sn = (hwidb->x25_modulo == X25_MOD8) ? X25_MODULO_8 : X25_MODULO_128;
    ushort unack_packets;
    uint linktype;
    ulong pbp_check_size, pbp_size;
    
    swidb = pak->if_input;

    if (mbit)
    	pak->flags |= PAK_MBIT;
	
    if (mbit && (lci->lci_pbp_compress || lci->lci_pbp_local_ack ||
		 (lci->lci_map &&
		  lci->lci_map->map_services & X25_MAPS_ENCAPS))) {
	lci->lci_reassmbytes += pak->datagramsize;

	/*
	 *  avoid collecting multiple input buffers by trying to append
	 *  new data to the last fragment received
	 */
	newpak = pak;
	pak = lci->lci_reassmQ.qtail;

	/*
	 *  append the new data to the last buffer received if
	 *  this isn't our first fragment and the last buffer has room
	 */
	if (pak &&
	    x25_ok_to_append_data(pak, newpak->datagramsize)) {
	    bcopy(newpak->datagramstart,
		  &pak->datagramstart[pak->datagramsize], newpak->datagramsize);
	    pak->datagramsize += newpak->datagramsize;
	    datagram_done(newpak);
	} else {
	    /*
	     *  just queue it
	     */
	    p_enqueue(&lci->lci_reassmQ, newpak);
	}

	pak = NULL;
	newpak = NULL;
    } else {
	if (lci->lci_pbp_compress) {	/* PBP decompression */
 	    newpak = lci->lci_pbp_depak;
 	    lci->lci_pbp_depak = NULL;	/* clear 1st time sw */
 	    newpak->if_input = swidb;
 	    newpak->enctype = hwidb->enctype;
 	    newpak->encsize = 0;
 	    newpak->linktype = pak->linktype;
 	    newpak->x25_flags = pak->x25_flags;
 	    if (lci->lci_reassmbytes == 0) {

		if (!x25_pbp_decompress(pak, newpak, newpak->datagramstart, 
					lci, &pbp_check_size)) {
		    errmsg(&msgsym(SIZEBAD, X25),
			   lci->lci_idb->namestring,
			   lci->lci_lci);
		    x25_sendcause(hwidb, lci, X25_RESET,
				  X25_RESET_REMOTE_PROC_ERR,
				  X25_DIAG_NO_INFO, NULL);
		    datagram_done(newpak);
		    return;
		} else {
		    datagram_done(pak);
		    if (pbp_check_size != newpak->datagramsize) {
			errmsg(&msgsym(SIZEBAD, X25),
				lci->lci_idb->namestring,
				lci->lci_lci);
			x25_sendcause(hwidb, lci, X25_RESET,
				      X25_RESET_REMOTE_PROC_ERR,
				      X25_DIAG_NO_INFO, NULL);
			datagram_done(newpak);
			return;
		    }
		}
	    } else {	/* m bit PBP decompression */
		lci->lci_reassmbytes += pak->datagramsize;
		p_enqueue(&lci->lci_reassmQ, pak);

		cp = newpak->datagramstart;
		pbp_check_size = 0;

		while ((pak = p_dequeue(&lci->lci_reassmQ))) {
		    if (!x25_pbp_decompress(pak, newpak, cp, lci, &pbp_size)) {
			errmsg(&msgsym(SIZEBAD, X25),
					lci->lci_idb->namestring,
					lci->lci_lci);
			x25_sendcause(hwidb, lci, X25_RESET,
					X25_RESET_REMOTE_PROC_ERR,
					X25_DIAG_NO_INFO, NULL);
			datagram_done(newpak);
			return;
		    } else {
			pbp_check_size += pbp_size;
			cp += pbp_size;
			datagram_done(pak);
		    }
		}
		if (pbp_check_size != newpak->datagramsize) {
		    errmsg(&msgsym(SIZEBAD, X25),
			   lci->lci_idb->namestring,
			   lci->lci_lci);
		    x25_sendcause(hwidb, lci, X25_RESET,
				  X25_RESET_REMOTE_PROC_ERR,
				  X25_DIAG_NO_INFO, NULL);
		    datagram_done(newpak);
		    return;
		}

		lci->lci_reassmbytes = 0;
	    }
 	    pak = newpak;
	}

	if (lci->lci_map && (lci->lci_map->map_services & X25_MAPS_ENCAPS)) {
	    /*
	     *  This is the last packet of a complete packet sequence
	     *  received on an encapsulation VC.
	     */

	    /*
	     *  reassemble a multi-packet message
	     */
	    if (lci->lci_reassmbytes) {
		/*
		 *  enqueue this last packet and start with the first packet
		 */
		lci->lci_reassmbytes += pak->datagramsize;
		p_enqueue(&lci->lci_reassmQ, pak);
		pak = p_dequeue(&lci->lci_reassmQ);

		newpak = NULL;

		if (x25_ok_to_append_data(pak,
				lci->lci_reassmbytes - pak->datagramsize)) {
		    /*
		     *  append all subsequent data to the first buffer,
		     *  and indicate that this is no longer a partial
		     *  message by clearing the M-bit flag.
		     */
		    newpak = pak;
		    newpak->flags &= ~PAK_MBIT;

		    /*
		     *  prime the append operation with the next packet
		     */
		    pak = p_dequeue(&lci->lci_reassmQ);
		} else {
		    /*
		     *  prepare a new buffer to collect all the data
		     */
		    newpak = getbuffer(lci->lci_reassmbytes);

		    if (newpak) {
			newpak->if_input = swidb;
			newpak->enctype = hwidb->enctype;
			newpak->datagramsize = 0;
			newpak->datagramstart = newpak->network_start;
			newpak->encsize = 0;
			newpak->linktype = pak->linktype;
			newpak->x25_flags = pak->x25_flags;
		    } else {
			/*
			 *  we can't reassemble the message
			 */
			datagram_done(pak);
			while ((pak = p_dequeue(&lci->lci_reassmQ))) {
			    datagram_done(pak);
			}
			pak = NULL;
		    }
		}

		/*
		 *  copy the data
		 */
		if (newpak) {
		    cp = newpak->datagramstart + newpak->datagramsize;

		    for ( ; pak; pak = p_dequeue(&lci->lci_reassmQ)) {
			bcopy(pak->datagramstart, cp, pak->datagramsize);
			cp += pak->datagramsize;
			newpak->datagramsize += pak->datagramsize;
			datagram_done(pak);
		    }

		    pak = newpak;
		}

		lci->lci_reassmbytes = 0;
	    }

	    /*
	     *  determine the packet type
	     */
	    if (pak &&
		(lci->lci_data_pid_method != X25_DATA_PID_NONE) &&
		(! x25_encaps_parse_data_pid(pak, lci))) {
		datagram_done(pak);
		pak = NULL;
	    }
	}

	if (pak) {
	    pak->x25_cb = lci;

	    switch (pak->linktype) {
	      case LINK_PAD:
		pad_enqueue(hwidb, pak, lci, qbit);
		break;
	      case LINK_CMNS:
		x25_forward_data(lci, pak, sn, len, pr);
		break;
	      case LINK_X25:
		if (lci->lci_swdata) {
		    x25_forward_data(lci, pak, sn, len, pr);
		    break;
		} else {
		    if (IS_LCI_PVC(hwidb, lci->lci_lci))
			x25_sendcause(hwidb, lci, X25_RESET,
				X25_RESET_NETWORK_OUT_OF_ORDER,
				X25_DIAG_NO_INFO, NULL);
		    datagram_done(pak);
		    return;
		}
	      case LINK_X25SERVICE:
		x25_service_input(lci, pak);
		break;
	      case LINK_BRIDGE:
		/*
		 * If we've received a packet that is of type bridge,
		 * check to see if it really is a bridged packet, or if
		 * it is a spanning tree update.  If it is a bridge
		 * packet, it must go through the equivilent of
		 * bridge_receive so that it can be added to the
		 * bridging database and send out the proper interface.
		 * If it is a spanning tree update, just queue it for
		 * the spanning tree.
		 * But first adjust the input idb if this interface is
		 * a rotary group member.
		 */
		if (reg_invoke_dialer_change_pak_if(hwidb, pak)) {
		    retbuffer(pak);
		    break;
		}
		linktype = x25_getlink(pak);
		pak->x25_flags = 0;
		if (linktype == LINK_BRIDGE) {
		    if (!swidb->tbridge_on_swidb)
			datagram_done(pak);
		    else {
			/* hybridge_input expect this flag to be set */
			pak->flags |= PAK_TBRIDGE_IN;
			net_bridge_enqueue(pak);
		    }
		} else {
		    /*
		     * Snap the frame to alignment if required
		     */
		    ALIGN_DATAGRAM_CMD(pak_realign(pak, NULL));
		    reg_invoke_raw_enqueue(linktype, pak);
		}
		break;
	      case LINK_COMPRESSED_TCP:
		/*
		 *  Cisco-proprietary compressed TCP encapsulations
		 *  use the Q bit to indicate THC status, whereas
		 *  IETF/RFC-1356 compressed TCP encapsulation requires
		 *  SNAP encoding of the complete packet sequence to
		 *  distingush between the two datagram types
		 */
		if ((lci->lci_cud_pid_method == X25_CUD_PID_CISCO) &&
		    ((pak->x25_flags & PAK_X25_QBIT) == 0))
		    pak->linktype = LINK_UNCOMPRESSED_TCP;

		/* FALL THROUGH */
	      case LINK_UNCOMPRESSED_TCP:
		if (!lci->lci_map->map_thc) {
		    if (thc_debug || pkt_debug)
			buginf("\n%s: X25 compressed TCP/IP packet dropped",
				swidb->namestring);
		    protocol_discard(pak, TRUE);
		    break; /* tossed out */
		}

#ifdef DEBUG
		if (thc_debug) {
		    paktype *debug_pak;

		    debug_pak = pak_duplicate(pak);
		    bugpak("\nx25_process_other before UNC: ", debug_pak);
		    retbuffer(debug_pak);
		}
#endif DEBUG

		pak =
		    reg_invoke_uncompress_tcpip_header(LINK_IP, pak, hwidb,
						       lci->lci_map->map_thc);
#ifdef DEBUG
		if (thc_debug) {
		    paktype *debug_pak;

		    debug_pak = pak_duplicate(pak);
		    bugpak("\nx25_process_other after UNC: ", debug_pak);
		    retbuffer(debug_pak);
		}
#endif DEBUG

		/*
		 *  if pak is NULL it was aborted by thc_uncompress_tcp()
		 */
		if (!pak)
		    break; /* tossed out */

		pak->linktype = LINK_IP;
		/* FALL THROUGH */
	      default:
		/*
		 * If this interface is a rotary group member we need to
		 * adjust its idb to reflect the group leader.
		 */
		if (reg_invoke_dialer_change_pak_if(hwidb, pak)) {
		    retbuffer(pak);
		    break;
		}
		if (pak->linktype == LINK_QLLC) 
		    pak->x25_flags &= PAK_X25_QBIT; /* Retain Q Bit */
		else
		    pak->x25_flags = 0;
		raw_enqueue(hwidb, pak);
		break;
	    }
	}
    }

    /*
     *  this data packet may have acknowledged something;
     *  prod x25_output to send any DATA it can.
     */
    if (!QUEUEEMPTY(&lci->lci_holdQ)) {
	fakepak.datagramsize = 0;
	fakepak.x25_cb = lci;
	x25_output(hwidb, &fakepak, TAIL);
	if (x25_lci_low_water(lci))	/* congestion relieved */
	    reg_invoke_media_x25_notify_event(lci->lci_linktype,
			lci, X25_UNQUENCH_EVENT, 0, 0, NULL);
    }

    if (lci->lci_pbp_local_ack) {	/*near inner node function*/
	if (dbit || x25_pbp_unacked(hwidb, lci))
	    x25_sendflow(hwidb, lci, X25_RR);
    } else {
	/*
	 *  since we didn't necessarily send a DATA packet, we
	 *  need to update our incoming window with an RR if:
	 *	this is an encapsulation VC and
	 *	we have some unacknowledged DATA packets and
	 *	the last packet set the D-bit -or-
	 *	we've reached a delayed ack. threshold -or-
	 *	we've reached the incoming window size
	 */
	if (lci->lci_map &&
	    (lci->lci_map->map_services & (X25_MAPS_ENCAPS | X25_MAPS_ALL_QLLC)) &&
	    (unack_packets =
		((lci->lci_pr + hwidb->x25_modulo) - lci->lci_ack) %
			hwidb->x25_modulo) &&
	    (dbit ||
	     (unack_packets == hwidb->x25_th) ||
	     (unack_packets == lci->lci_win)))
	    x25_sendflow(hwidb, lci, X25_RR);
    }
}

/* 
 *	x25_reassemble
 *
 * This is  a utility to manage the M bit reassembly logic.
 * In particular it provides services for the x25 applications
 * so that they can operate at process level, and use malloc to
 * get larger buffers as needed.
 *
 * Input:
 * 	*pakptr latest x25 data packet as passed to x25 app
 *		with or without M bit
 *	reassmQ	queue of packets with M bit
 *
 * Output:
 * 	*pakptr completed x25 data packet to processed by X.25 application
 *		packet had no m bit, or was the only packet in the chain
 *		Else NULL, if put on reassembly queue, or reasssembly failed
 *
 * Value returned:
 *
 *  X25_PACKET		: *pakptr has a real packet
 *  X25_QUEUED		: input pakcet was put on the reassembly queue
 *  X25_NOPACKET	: reassembly failed, reassembly queue is purged.
 *
 * This routine assumes that it has been called either or at interrupt level
 * for a particular application.  What is not allowed is for one single
 * application to use this routine at both process and interrupt level.
 */

static  int x25_reassemble(paktype **pakptr, queuetype *reassmQ)
{
    int 	length;
    paktype	*inpak;
    paktype 	*outpak;
    paktype	*pak;
    uchar	*cp;
    boolean	first_pak;
    
    
    inpak = *pakptr;
    
    if (inpak->flags & PAK_MBIT) {
	p_enqueue(reassmQ, inpak);
	*pakptr = NULL;
	return(X25_QUEUED);
    }
    
    if (QUEUEEMPTY(reassmQ)) 	/* only packet */
	return(X25_PACKET);
    
    p_enqueue(reassmQ, inpak);
    
    /* compute agggregate packet size */
    length = 0;
    
    for (pak = (paktype *)reassmQ->qhead; pak != NULL; pak = pak->next)
	length += (pak->datagramsize);
    
    /* get output buffer */
    outpak = getbuffer(length);
    *pakptr = outpak;
    if (outpak == NULL) {
	while ((pak = p_dequeue(reassmQ)) != NULL)
	    datagram_done(pak);
	return(X25_NOPACKET);
    }	
    
    /*
     *  copy the data
     */
    outpak->if_input = inpak->if_input;
    outpak->datagramsize = length;
    outpak->datagramstart = outpak->network_start;
    cp = outpak->datagramstart;
    outpak->encsize = 0;
    
    first_pak = TRUE;
    
    while ((pak = p_dequeue(reassmQ)) != NULL) {
    	if (first_pak) {
	    first_pak = FALSE;
	    outpak->linktype = pak->linktype;
	    outpak->x25_flags = pak->x25_flags; 
	    outpak->enctype = pak->enctype;
	}
	bcopy(pak->network_start, cp, pak->datagramsize);
	cp += pak->datagramsize;
	datagram_done(pak);
    }
    return(X25_PACKET);
}

/*
 *	x25BLOCK
 *      This is a standard scheduler test procedure for use by task x25_timer;
 *      returns TRUE if the x25_timer task is blocked,
 *      returns FALSE if the x25_timer task has something to do.
 */

boolean x25BLOCK (void)
{
    register hwidbtype *hwidb;
    register lcitype *lci;
    register leveltype level;
    register x25idbtype *x25idb;

    /*
     * Check for an X25 interface event
     */
    for (x25idb = (x25idbtype *) x25idbQ.qhead; x25idb; x25idb = x25idb->next) {
        hwidb = x25idb->hwidb;
	/*
	 *	only check X.25 and DDN X.25; skips CONS interfaces
	 */
	if (!is_x25(hwidb))
	    continue;

	/*
	 *	check if a deferred restart event has occurred
	 */
	if (hwidb->x25_restart)
	    return FALSE;

	/*
	 *	check if the restart timer has expired
	 */
	if (TIMER_RUNNING_AND_AWAKE(hwidb->x25_timer))
	    return FALSE;
    }

    /*
     *	Check for X25 timer queue activity--
     *	disable interrupts while using the queue
     */
    level = raise_interrupt_level(NETS_DISABLE);
    lci = (lcitype *)x25tQ.qhead;
    if (lci) {
	if (!SLEEPING(lci->lci_timer)) {
	    reset_interrupt_level(level);
	    return FALSE;
	}
    }
    reset_interrupt_level(level);

    /*
     * Check for deferred X25 packet handling
     */
    if (x25iQ.qhead || x25aQ.qhead || x25LocalAckQ.qhead)
	return FALSE;

    return TRUE;
}

/*
 * x25_timer
 * Handle timed events
 */

forktype x25_timer (void)
{
    register hwidbtype *hwidb;
    register lcitype *lci;
    register paktype *pak, *newpak;
    register considbtype *considb;
    register leveltype level;
    int pr, mbit, ps, len;
    x25paktype *p_old, *p_new;
    x25idbtype *x25idb;

    edisms((blockproc *)systeminitBLOCK, 0);

    for (;;) {
	/*
	 *	relinquish the processor until we have something to do
	 */
	edisms((blockproc *)x25BLOCK, 0);

	/*
	 *	look for activity on the X.25 interfaces
	 */
        for (x25idb = (x25idbtype *) x25idbQ.qhead; x25idb; 
	        x25idb = x25idb->next) {
            hwidb = x25idb->hwidb;
	    /*
	     *	only check X.25 and DDN X.25; skips CONS interfaces!
	     */
	    if (!is_x25(hwidb))
		continue;

	    /*
	     *	we need to reset the service
	     */
	    if (hwidb->x25_restart) {
		x25_restart(hwidb, FALSE);
	    }

	    /*
	     *	if the restart timer has run out, restart the service
	     */
	    if (TIMER_RUNNING_AND_AWAKE(hwidb->x25_timer)) {
		hwidb->x25_t0_count++;
		if (!ISIEEEMSK(hwidb) && hwidb->x25_snmp_restart_tx) {
		    /*
		     *  the link is having problems re-establishing
		     */
		    hwidb->reason = "X.25 restart timer expired";
		    reg_invoke_ifmib_link_trap(LINK_DOWN_TRAP, hwidb);
		    hwidb->x25_snmp_restart_tx = FALSE;
		}
		x25_sendcause(hwidb, NULL, X25_RESTART,
			X25_RESTART_LOCAL_PROC_ERR,
			X25_DIAG_TE_RESTART_INDICATION, NULL);
	    }
	}


	/*
	 *	handle deferred input packet processing
	 */
	for (;;) {
	    level = raise_interrupt_level(NETS_DISABLE);
	    pak = p_dequeue(&x25iQ);
	    if (!pak) {
		reset_interrupt_level(level);
		break;
	    }	
	    hwidb = pak->if_input->hwptr;
            
	    (*x25_state_table[hwidb->x25_state])(hwidb, pak, NULL);
	    reset_interrupt_level(level);
	}

	/*
	 *	handle alternate routing input CALL packet processing
	 *
	 *if alternate routing takes longer than the originator's t21/t11
	 *timers, a CLEAR will be received, which will delete the LCI structure
	 *for a packet queued on x25aQ.  A returned NULL lci pointer from 
	 *x25_lci_lookup() will indicate that a CLEAR has been received. 
	 *Or if the LCI starttime is != to the packets inputtime then a
	 *CLEAR, CALL sequence occured.  in either case, all further attemps 
	 *of alternate routing for this packet must be aborted.  
	 */ 
	for (;;) {
	    level = raise_interrupt_level(NETS_DISABLE);
	    pak = p_dequeue(&x25aQ);
	    if (!pak) {
		reset_interrupt_level(level);
		break;
	    }	

	    hwidb = pak->if_input->hwptr;
	    if (((lci = x25_lci_lookup(hwidb,
				LCI((packettype *)pak->datagramstart),
				NULL)) == NULL) || 
		TIMERS_NOT_EQUAL(lci->lci_starttime, pak->inputtime)) {
		datagram_done(pak);
	    } else {
		(*x25_state_table[hwidb->x25_state])(hwidb, pak, lci);
		reset_interrupt_level(level);
		/*do only one alternate routing at a time.        */ 
		/*because tcp/ip failed connects can take 50 sec's*/
		break;
	    }

	    reset_interrupt_level(level);
	}

	/*
	 * PBP Compression local processing -  far inner node 
	 * (remote x25 switching) ack's.  can't return these
	 * ack's from x25_output() because of stack overflow. 
	 */
	for (;;) {
	    level = raise_interrupt_level(NETS_DISABLE);
	    pak = p_dequeue(&x25LocalAckQ);
	    if (!pak) {
		reset_interrupt_level(level);
		break;
	    }	

	    hwidb = pak->if_output->hwptr;
	    if ((lci = x25_lci_lookup(hwidb,
				LCI((packettype *)pak->datagramstart),
				NULL)) == NULL) {
		datagram_done(pak);
	    } else {
		lci->lci_pbp_ackpak = NULL;
		x25_pbp_finish_ack(lci, pak);
		x25_forward_ack(lci, pak, 0, 0);
	    }
	    reset_interrupt_level(level);
	}

	/*
	 *	look for timer queue activity
	 */
	for (;;) {
	    /*
	     *	disable network device interrupts
	     */
	    level = raise_interrupt_level(NETS_DISABLE);

	    if ((lci = (lcitype *)x25tQ.qhead) == NULL ||
		SLEEPING(lci->lci_timer)) {
		/*
		 *	no logical circuit time events available
		 */
		reset_interrupt_level(level);
		break;
	    }

	    /*
	     *	dequeue the LCI and clear its timer
	     */
	    lci = dequeue(&x25tQ);
	    TIMER_STOP(lci->lci_timer);

	    /*
	     * check for PBP waiting for a decompression buffer
	     */
	    if (lci->lci_pbp_rnr) {
		hwidb = lci->lci_idb->hwptr;
		newpak = getbuffer(lci->lci_pbp_need_pak_size +
					       X25_PBP_STAC_FUDGE);
		if (newpak != NULL) {
		    pak = p_dequeue(&lci->lci_pbp_rnr_deferredQ);
		    x25_data(pak, &pr, &mbit, &ps, &len);
		    pak->datagramstart += len;
		    pak->datagramsize -= len;
		    lci->lci_pbp_depak = newpak;

		    pak_copy(pak, newpak, 0);
		    newpak->datagramsize = lci->lci_pbp_need_pak_size;

		    p_old = (x25paktype *) (pak->datagramstart - len);
		    p_new = (x25paktype *) (newpak->datagramstart - len);
		    p_new->x25GfiLcg = p_old->x25GfiLcg;
		    p_new->x25Lcn = p_old->x25Lcn;
		    p_new->x25Pti = p_old->x25Pti & ~X25_MBIT_MASK;

		    pak->datagramsize -= X25_PBP_HEADERBYTES;
		    pak->datagramstart += X25_PBP_HEADERBYTES;
		    /*x25_process_data() kicks off send DATA or RR's.
		     *sent RNR has this blocked. so if nothing queued
		     *open the floodgates send the RR.  but don't
		     *release this thread till done.
		     */
		    if (QUEUEEMPTY(&lci->lci_pbp_rnr_deferredQ)) {
			lci->lci_pbp_rnr = FALSE;
			lci->lci_pbp_rnr_ticks = 0;
			x25_sendflow(hwidb, lci, X25_RR); /*kill RNR*/
		    }
		    x25_process_data(hwidb, lci, pak, pr, mbit, ps, len);

		    while ((pak = p_dequeue(&lci->lci_pbp_rnr_deferredQ))) {
			if (QUEUEEMPTY(&lci->lci_pbp_rnr_deferredQ)) {
			    lci->lci_pbp_rnr = FALSE;
			    lci->lci_pbp_rnr_ticks = 0;
			    x25_sendflow(hwidb, lci, X25_RR); /*kill RNR*/
			}
			/*note: can again get blocked by no decompress buffer
			 *so we test !TIMER_RUNNING(lci->lci_timer) each time.
			 */
			x25_data(pak, &pr, &mbit, &ps, &len);
			x25_process_other(hwidb, lci, pak, pr, mbit, ps, len);
		    }
		} else {	/* re-start timer for getbuffer */
		    if (--lci->lci_pbp_rnr_ticks > 0) {
			TIMER_START(lci->lci_timer, ONESEC);
			x25_addtimer(lci);
		    } else {
			x25_sendcause(hwidb, lci, X25_RESET,
				      X25_RESET_NETWORK_OUT_OF_ORDER,
				      X25_DIAG_NO_INFO,
				      NULL);
		    }
		}
		reset_interrupt_level(level);
		continue;
	    }

	    /*
	     *	handle the LCI's timed event
	     */

	    if ((hwidb = hwidb_or_null(lci->lci_idb)) == NULL) {
		errmsg(&msgsym(LCIBOGUS, X25), lci);
		crashdump(11);
		reset_interrupt_level(level);
		continue;
	    }

	    considb = get_lci_considb(lci);

	    switch (lci->lci_state) {
	    case X25_R2:
	    case X25_R3:
		/* In case of cons */
		if (ISCMNS_IEEE(hwidb)) {
		    x25_sendcause(hwidb, lci, X25_RESTART,
			considb->x25_cause, considb->x25_diag, NULL);
		}
		break;
	    case X25_P2:		/* Call request */
	    case X25_P3:
	    case X25_P5:
		hwidb->x25_t1_count++;
		x25_sendcause(hwidb, lci, X25_CLEAR,
		    X25_CLEAR_LOCAL_PROC_ERR, X25_DIAG_TE_INCOMING_CALL, NULL);
		break;
	    case X25_P4:
	    case X25_D1:		/* Idle timer */
		if (IS_LCI_PVC(hwidb, lci->lci_lci)) {
		    if (lci->lci_swdata == NULL)
			x25_pvc_retry(lci);
		    else
			errmsg(&msgsym(SPURD1, X25),
				hwidb->hw_namestring, lci->lci_lci);
		} else if (lci->lci_idle == 0) {
		    errmsg(&msgsym(SPURD1, X25),
			   hwidb->hw_namestring, lci->lci_lci);
		} else {
		    x25_sendcause(hwidb, lci, X25_CLEAR,
			X25_CLEAR_NETWORK_CONGESTION,
			X25_DIAG_MAINTENANCE_ACTION, NULL);
		}
		break;
	    case X25_D2:		/* Reset */
	    case X25_D3:
		hwidb->x25_t2_count++;
		lci->lci_t2_count++;
		x25_sendcause(hwidb, lci, X25_RESET,
		    X25_RESET_LOCAL_PROC_ERR, X25_DIAG_TE_RESET_INDICATION,
		    NULL);
		break;
	    case X25_P6:		/* Clear */
	    case X25_P7:
		hwidb->x25_t3_count++;
		x25_sendcause(hwidb, lci, X25_CLEAR,
		    X25_CLEAR_LOCAL_PROC_ERR, X25_DIAG_TE_CLEAR_INDICATION,
		    NULL);
		break;
	    case X25_X1:		/* Hold timer expired */
		lci->lci_state = X25_P1;
		x25_call_remove(lci, 0, 0);
		break;
	    default:
		if (x25_debug)
		    buginf("\n%s: X25 lci %d illegal state %d",
			hwidb->hw_namestring, lci->lci_lci, lci->lci_state);
		break;
	    }

	    /*
	     *	restore interrupt mask to previous level
	     */
	    reset_interrupt_level(level);
	}
    }
}

/*
 * x25_addtimer
 * Add LCI to Timer queue
 */

void x25_addtimer (register lcitype *lci)
{
    register lcitype *before, *after;
    leveltype level;

    level = raise_interrupt_level(NETS_DISABLE);

    if (checkqueue(&x25tQ, lci)) {
	errmsg(&msgsym(INTIMEQ, X25), lci->lci_idb->namestring,
		lci->lci_lci, lci->lci_timer);
	reset_interrupt_level(level);
	return;
    }

    before = (lcitype *)x25tQ.qhead;
    after = before;

    /*
     *  find first entry that is larger then the one we are trying to enter.
     */
    while (before && TIMER_EARLIER(before->lci_timer, lci->lci_timer)) {
	after = before;
	before = before->lci_next;
    }

    if (before == NULL)
	enqueue(&x25tQ, lci);
    else if (before == after)
	requeue(&x25tQ, lci);
    else {
	after->lci_next = lci;
	lci->lci_next = before;
    }
    reset_interrupt_level(level);
}

/*
 * x25_getlink
 * Return type of data flowing across this VC
 */

long x25_getlink (paktype *pak)
{
    ether_hdr *ether;

    if (pak->linktype != LINK_BRIDGE)
	return(pak->linktype);

    /*
     * If the receiving lci is used for bridging, then we need to determine
     * if this packet is a bridged packet or if it is spanning information.
     * Spanning information has a fake mac header at the front of it.
     * The type field in this fake header is TYPE_DEC_SPANNING. If it is
     * a spanning tree packet, then we update the datagramstart pointer
     * to skip this fake mac header.  If it is a bridged packet, just
     * pass it on through.  Note that the fake mac type field will only
     * have TYPE_DEC_SPANNING.  This does not indicate that it isn't using
     * IEEE spanning, but is just used as a placeholder.
     */

    ether = (ether_hdr *)pak->datagramstart; 

    switch(type2link(GETSHORT(&ether->type_or_len))) { 
      case LINK_DEC_SPANNING:
	/*
	 * Spanning tree protocol packets does not contain mac header info.,
	 * skip these addresses information and update the size of the
	 * datagram.   
	 */
	pak->network_start += ETHER_ARPA_ENCAPBYTES;
	pak->datagramstart += ETHER_ARPA_ENCAPBYTES;
	pak->datagramsize -= ETHER_ARPA_ENCAPBYTES;
	return(LINK_DEC_SPANNING);
      case LINK_ARP:
	/*
	 * This ARP packet is to resolve my mac address when we are a 
	 * bridge with no ip routing. Update network_start pointer to 
	 * skip this mac header before sending it up to the ARP process.
	 */
	pak->network_start += ETHER_ARPA_ENCAPBYTES;
	pak->enctype = ET_ARPA;
	pak->flags |= PAK_TBRIDGE_IN;
	return(LINK_ARP);
      case LINK_IP:
	/*
	 *  Update network_start pointer for reg_invoke_ip_forus().    
	 *  If this IP packet is for us, send it to IP process, 
	 *  otherwise, set the network_start pointer back to include 
	 *  the mac header in order to bridge this IP packet.
	 */
	pak->network_start += ETHER_ARPA_ENCAPBYTES;
	if (reg_invoke_ip_forus(pak)) {
	    pak->linktype = LINK_IP;
	    pak->enctype = ET_ARPA;
	    pak->flags |= PAK_TBRIDGE_IN;
	    return(LINK_IP);
	} else {
	    pak->network_start -= ETHER_ARPA_ENCAPBYTES;
	    return(LINK_BRIDGE);
	}

      default:
	return(LINK_BRIDGE);
    }
}

/*
 * x25_default_n1
 * Contribute towards the setting up of the
 * LAPB N1 parameter. Called if running an
 * X.25 encapsulation.
 */

int x25_default_n1 (register hwidbtype *hwidb)
{
    int size;

    size = (hwidb->x25_admin_modulo == X25_MOD8) ?
	    X25HEADERBYTES : X25EHEADERBYTES;

    /*
     *  For the DDN and BFE encapsulations we need to add the 'dead space'
     *  between the IP MTU and the maximum packet size to properly set
     *  the LAPB N1, otherwise 1024 byte CALLs will be negotiated downwards
     *  or CLEARed.
     */
    if (ISBFE(hwidb))
	size += X25_DDN_MAXPACKET - X25_BFE_MTU;
    else if (ISDDN(hwidb))
	size += X25_DDN_MAXPACKET - X25_DDN_MTU;

    return(size);
}

/*
 *  x25_payload
 *  Returns the maximum number of data bytes that can be carried in a packet
 */
int x25_payload (hwidbtype *hwidb)
{
    /*
     *	CMNS' llc2 parameters do not have an N1; so we'll just return bignum
     */
    if (ISCMNS_IEEE(hwidb))
	return(X25MAXDATA);

    if (!is_x25(hwidb))
        return(0);

    return(lapb_payload(hwidb) -
	   ((hwidb->x25_modulo == X25_MOD8) ?
		X25HEADERBYTES : X25EHEADERBYTES));
}

/*
 *  x25_fix_flow_fac(idb)
 *  Lowers any unavailable flow control values
 */
boolean x25_fix_flow_fac (lcitype *lci)
{
    boolean result = FALSE;
    int payload;
    hwidbtype *hwidb = lci->lci_idb->hwptr;

    if (lci->lci_win >= hwidb->x25_modulo) {
	lci->lci_win = hwidb->x25_modulo - 1;
	lci->lci_force_winsize = TRUE;
	result = TRUE;
    }
    if (lci->lci_wout >= hwidb->x25_modulo) {
	lci->lci_wout = hwidb->x25_modulo - 1;
	lci->lci_force_winsize = TRUE;
	result = TRUE;
    }

    payload = x25_payload(hwidb);
    while ((1 << lci->lci_pin) > payload) {
	lci->lci_pin--;
	lci->lci_force_pksize = TRUE;
	result = TRUE;
    }
    while ((1 << lci->lci_pout) > payload) {
	lci->lci_pout--;
	lci->lci_force_pksize = TRUE;
	result = TRUE;
    }

    return(result);
}

/*
 * x25_llc2_register 
 * notify routine to keep llc2_timer happy!!!! 
 */
void x25_llc2_register (
    llctype *llc2)
{
}

/*
 *  x25_valid_config
 *  Set the operating parameters from the administrative parameters if the
 *  configuration is valid
 */
boolean x25_valid_config (
    register hwidbtype *hwidb,
    boolean set)
{
    int	payload;
    int	first_svc, old_first_svc;
    lcitype *lci;

    if (set)
	hwidb->x25_modulo = hwidb->x25_admin_modulo;

    /*
     *	disable the incoming-only range if it matches the two-way range
     */
    if (((hwidb->x25_admin_lic) &&
	 (hwidb->x25_admin_lic == hwidb->x25_admin_ltc) &&
	 (hwidb->x25_admin_hic == hwidb->x25_admin_htc)) ||
	((hwidb->x25_admin_lic == 0) && (hwidb->x25_admin_ltc == 1) &&
	 (hwidb->x25_admin_hic == hwidb->x25_admin_htc))) {
	hwidb->x25_admin_lic = 0;
	hwidb->x25_admin_hic = 0;
	errmsg(&msgsym(RANGEUNUSED, X25),
		hwidb->hw_namestring, "incoming-only");
    }

    /*
     *	disable the outgoing-only range if it matches the two-way range
     */
    if (((hwidb->x25_admin_loc) &&
	 (hwidb->x25_admin_loc == hwidb->x25_admin_ltc) &&
	 (hwidb->x25_admin_hoc == hwidb->x25_admin_htc)) ||
	((hwidb->x25_admin_loc == 0) && (hwidb->x25_admin_ltc == 1) &&
	 (hwidb->x25_admin_hoc == hwidb->x25_admin_htc))) {
	hwidb->x25_admin_loc = 0;
	hwidb->x25_admin_hoc = 0;
	errmsg(&msgsym(RANGEUNUSED, X25),
		hwidb->hw_namestring, "outgoing-only");
    }

    for ( ; ; ) {	/* error control loop */
	/*
	 *  check the virtual circuit ranges;
	 *  both elements of a range must either == 0 or != 0,
	 *  the low end must not be > the high end and
	 *  the incoming range < two-way range < outgoing range
	 *
	 *  note that the parse routines will not set a range element
	 *  to a value greater than X25_MAX_LCI
	 */
	if (((hwidb->x25_admin_lic == 0) != (hwidb->x25_admin_hic == 0)) ||
	    (hwidb->x25_admin_lic > hwidb->x25_admin_hic)) {
	    if (set)
		errmsg(&msgsym(BADCONFIG, X25), hwidb->hw_namestring,
			"incoming-only VC range not valid");
	    break;
	}
	if (((hwidb->x25_admin_ltc == 0) != (hwidb->x25_admin_htc == 0)) ||
	    (hwidb->x25_admin_ltc > hwidb->x25_admin_htc)) {
	    if (set)
		errmsg(&msgsym(BADCONFIG, X25), hwidb->hw_namestring,
			"two-way VC range not valid");
	    break;
	}
	if (((hwidb->x25_admin_loc == 0) != (hwidb->x25_admin_hoc == 0)) ||
	    (hwidb->x25_admin_loc > hwidb->x25_admin_hoc)) {
	    if (set)
		errmsg(&msgsym(BADCONFIG, X25), hwidb->hw_namestring,
			"outgoing-only VC range not valid");
	    break;
	}
	if (hwidb->x25_admin_ltc &&
	    (hwidb->x25_admin_ltc <= hwidb->x25_admin_hic)) {
	    if (set)
		errmsg(&msgsym(BADCONFIG, X25), hwidb->hw_namestring,
			"incoming-only/two-way VC ranges overlap");
	    break;
	}
	if (hwidb->x25_admin_loc &&
	    (hwidb->x25_admin_loc <= hwidb->x25_admin_hic)) {
	    if (set)
		errmsg(&msgsym(BADCONFIG, X25), hwidb->hw_namestring,
			"incoming-only/outgoing-only VC ranges overlap");
	    break;
	}
	if (hwidb->x25_admin_loc &&
	    (hwidb->x25_admin_loc <= hwidb->x25_admin_htc)) {
	    if (set)
		errmsg(&msgsym(BADCONFIG, X25), hwidb->hw_namestring,
			"two-way/outgoing-only VC ranges overlap");
	    break;
	}

	/*
	 *  check the window sizes
	 *
	 *  note that the parse routines will not set a window size < 1
	 */
	if ((hwidb->x25_admin_win >= hwidb->x25_admin_modulo) ||
	    (hwidb->x25_admin_wout >= hwidb->x25_admin_modulo)) {
	    if (set)
		errmsg(&msgsym(BADCONFIG, X25), hwidb->hw_namestring,
			"default window sizes not valid");
	    break;
	}

	if (set) {
	    /*
	     *  complain if the "x25 facility packetsize" values or the
	     *  interface's default packet size values are too large
	     */

	    payload = x25_payload(hwidb);

	    if (((1 << hwidb->x25_fpin) > payload) ||
		((1 << hwidb->x25_fpout) > payload)) {
		errmsg(&msgsym(LARGEPKSIZE, X25), hwidb->hw_namestring,
			"facility",
			1 << hwidb->x25_fpin, 1 << hwidb->x25_fpout);
	    }

	    if (((hwidb->x25_fpin == 0) &&
		 ((1 << hwidb->x25_admin_pin) > payload)) ||
		((hwidb->x25_fpout == 0) &&
		 ((1 << hwidb->x25_admin_pout) > payload))) {
		errmsg(&msgsym(LARGEPKSIZE, X25), hwidb->hw_namestring,
			"default",
			1 << hwidb->x25_admin_pin, 1 << hwidb->x25_admin_pout);
	    }

	    /*
	     *  delete any PVCs that are now illegal, e.g. any PVCs
	     *  that are now SVCs because a VC range has been lowered
	     */
	    first_svc = hwidb->x25_admin_lic ? hwidb->x25_admin_lic :
			(hwidb->x25_admin_ltc ? hwidb->x25_admin_ltc :
			 (hwidb->x25_admin_loc ? hwidb->x25_admin_loc :
			  X25_MAX_LCI + 1));
	    old_first_svc = hwidb->x25_lic ? hwidb->x25_lic :
			    (hwidb->x25_ltc ? hwidb->x25_ltc :
			     (hwidb->x25_loc ? hwidb->x25_loc :
			      X25_MAX_LCI + 1));

	    if (old_first_svc > first_svc) {
		for (lci = x25_lci_lookup_next(hwidb,
				first_svc ? first_svc - 1 : 0, NULL);
		     lci;
		     lci = x25_lci_lookup_next(hwidb, lci->lci_lci, NULL)) {
		    if (lci->lci_lci >= old_first_svc)
			break;
		    x25_lci_delete(lci);
		}
	    }

	    hwidb->x25_lic = hwidb->x25_admin_lic;
	    hwidb->x25_hic = hwidb->x25_admin_hic;
	    hwidb->x25_ltc = hwidb->x25_admin_ltc;
	    hwidb->x25_htc = hwidb->x25_admin_htc;
	    hwidb->x25_loc = hwidb->x25_admin_loc;
	    hwidb->x25_hoc = hwidb->x25_admin_hoc;
	    hwidb->x25_pin = hwidb->x25_admin_pin;
	    hwidb->x25_pout = hwidb->x25_admin_pout;
	    hwidb->x25_win = hwidb->x25_admin_win;
	    hwidb->x25_wout = hwidb->x25_admin_wout;
	}

	return(TRUE);
    }

    /*
     *	the configuration is not valid
     */
    return(FALSE);
}

/*
 * x25_linestate
 * Return line protocol status for X.25
 */

static boolean x25_linestate (hwidbtype *hwidb)
{
    if (reg_invoke_dialer_spoofing_prot(hwidb))
	return(TRUE);
    return(hwidb->lapb_upstate && hwidb->x25_upstate);
}

/*
 * x25_extract_addr
 *
 * Given an Ethernet packet, extract the requested MAC level address into
 * a hwaddrtype structure.
 */
boolean x25_extract_addr (
    paktype *pak,
    hwaddrtype *address,
    enum SRCDST which)
{
    lcitype *lci;
    addrtype *x121;

    lci = pak->x25_cb;
    if (!lci || !validmem(lci) || !validmem(lci->lci_map))
	return(FALSE);

    address->type = STATION_X121;
    address->target_type = TARGET_UNICAST;
    memset(address->addr, 0, STATIONLEN_X121);

    switch (which) {
      case SOURCE:
	/*
	 *  address of sending end requested
	 */
	x121 = (lci->lci_outgoing) ? &lci->lci_dst_addr : &lci->lci_src_addr;
	break;
      case DESTINATION:
	/*
	 *  address of receiving end (i.e. local interface) requested
	 */
	x121 = (lci->lci_outgoing) ? &lci->lci_src_addr : &lci->lci_dst_addr;
	break;
      default:
	return(FALSE);
    }

    address->length = x121->length;
    bcopy(x121->x121_addr, address->addr, address->length);
    return(TRUE);
}

/*
 * x25_parse_packet
 *
 *
 * Set up a packet buffer so that the network level protocol fields
 * are initialized.
 *
 * If data is NULL, pak->datagramstart is assumed to point to the
 * start of the frame to parse. No attempt will be made to align
 * the frame to The Line.
 *
 */
static iqueue_t x25_parse_packet (
    hwidbtype *hwidb,
    paktype *pak,
    uchar *data)
{
    int x25;

    /*
     * Grab the encapsulation type, and figure out the encapsulation size.
     */
    pak->enctype = hwidb->enctype;
    x25 = (hwidb->x25_modulo == X25_MOD8) ? X25HEADERBYTES: X25EHEADERBYTES;

    /*
     * If we're building a virgin frame, we have to set datagramstart
     */
    if (data) {
	pak->datagramstart = pak_center(pak) - x25 - LAPB_OVERHEAD(hwidb);
    }
    pak->mac_start     = pak->datagramstart;
    pak->addr_start    = NULL;
    pak->rif_start     = NULL;
    pak->riflen	       = 0;
    pak->info_start    = NULL;
    pak->network_start = pak->datagramstart + x25 + LAPB_OVERHEAD(hwidb);

    /*
     * Record just how big this encapsulation was rather than every protocol
     * have it's own fun and wacky way to work it out.
     */
    pak->encsize = pak->network_start - pak->datagramstart;
    return(lapb_input);
}

/*
 * x25_setup
 * Set up an X25 encapsulation
 */

boolean x25_setup (parseinfo *csb, hwidbtype *hwidb)
{
    idbtype *swidb = hwidb->firstsw;
    int new_ip_mtu;

    if (! ISCMNS_IEEE(hwidb)) {
	hwidb->status &= ~IDB_BCAST;
    }

    /*
     * Set X25 configuration defaults
     */
    hwidb->x25_version = X25_VERSION_DEFAULT;
    hwidb->x25_dce = LAPBX25_DTE;
    hwidb->x25_cud_pid_def = X25_CUD_PID_CISCO;

    if (csb != NULL) {
	if (GETOBJ(int,2) & ENCAPS_X25_BFE)
	    hwidb->x25_version = X25_VERSION_1980_BFE;
	else {
	    if (GETOBJ(int,2) & ENCAPS_X25_DDN)
		hwidb->x25_version = X25_VERSION_1980_DDN;
	    hwidb->x25_dce = (GETOBJ(int,2) & ENCAPS_LAPB_X25_DCE) ?
				LAPBX25_DCE : LAPBX25_DTE;
	}

	if (GETOBJ(int,2) & ENCAPS_X25_IETF)
	    hwidb->x25_cud_pid_def = X25_CUD_PID_IETF;
    }

    hwidb->x25_modulo = X25_DEFAULT_MODULO;

    /*
     *  BFE emergency mode, BFE decision and BFE emergency status
     */
    hwidb->x25_bfe_mode = BFE_NEVER;
    hwidb->x25_bfe_decision = BFE_DECISION_NO;
    hwidb->x25_bfe_status = 0;

    TIMER_STOP(hwidb->x25_timer);
    hwidb->x25_default = LINK_ILLEGAL;

    /*
     *  set the default address
     */
    if (hwidb->x25_address == NULL) {
	if ((hwidb->x25_address = malloc(sizeof(addrtype))) == NULL)
	    return(FALSE);
    }
    memset(hwidb->x25_address, 0, sizeof(addrtype));
    hwidb->x25_address->type = ADDR_X121;
    if (!ISCMNS_IEEE(hwidb))
    	x25_ip_default(swidb, swidb->ip_address, swidb->ip_nets_mask,
		       FALSE, TRUE);

    /*
     * If the ip_mtu is same as the normal default mtu then change it if
     * the encapsulation has a specific default value.
     * If different from the default mtu, it indicates that ip
     * mtu has been explicitly configured (via ip mtu or just mtu 
     * command), so do not change the ip mtu.
     * Note : An assumption is that the default maximum mtu is larger
     * than X25_BFE_MTU & X25_DDN_MTU.
     */

    new_ip_mtu = 0;
    if (ISBFE(hwidb)) {
	new_ip_mtu = X25_BFE_MTU;
	/* BFE emergency mode initialization */
	hwidb->x25_bfe_mode = BFE_NEVER;
	/* BFE decision initialization */
	hwidb->x25_bfe_decision = BFE_DECISION_NO;
	/* Initialize BFE emergency status */
	hwidb->x25_bfe_status = 0;
    } else if (ISDDN(hwidb) && !ISCMNS_IEEE(hwidb)) 
	new_ip_mtu = X25_DDN_MTU;

    if (new_ip_mtu) {
	FOR_ALL_SWIDBS_ON_HW(hwidb, swidb) {
	    if (swidb->ip_mtu == hwidb->maxmtu_def)
		swidb->ip_mtu = new_ip_mtu;
	}
    }

    /*
     * Allocate the x25idb and put it on the queue
     */

    /*
     * First check to see if it is already there.  If so, something is
     * very wrong, but delete it and make a new one...
     */

    if (hwidb->x25idb) {
       /*
        * need an errmsg here probably.  this should never happen, but if
	* it does, we should know about it.
	*/
        unqueue(&x25idbQ, hwidb->x25idb);
        free(hwidb->x25idb);
        hwidb->x25idb = NULL;
    }

    hwidb->x25idb = malloc(sizeof(x25idbtype));
    if (!hwidb->x25idb) {
        return(FALSE);
    }
    hwidb->x25idb->hwidb = hwidb;	/* back pointer to hwidb */
    enqueue(&x25idbQ, hwidb->x25idb);


    hwidb->x25_state = X25_R1;

    /*
     *	initialize the interface control flags
     */
    hwidb->x25_upstate = FALSE;
    hwidb->x25_restart = FALSE;

    hwidb->x25_win = X25_DEFAULT_WIN;		/* input window size */
    hwidb->x25_wout = X25_DEFAULT_WOUT;		/* output window size */
    hwidb->x25_pin = X25_DEFAULT_IPS;		/* input octets (log base 2) */
    hwidb->x25_pout = X25_DEFAULT_OPS;		/* output octets (log base 2) */
    hwidb->x25_retransmit = 0;
    hwidb->x25_cause = 0;
    hwidb->x25_diag = 0;

    /*
     *  set the default timer values (seconds)
     */
    if (ISCMNS_IEEE(hwidb)) {
	hwidb->x25_t0 = CMNS_DEFAULT_DXE_T0;
	hwidb->x25_t1 = CMNS_DEFAULT_DXE_T1;
	hwidb->x25_t2 = CMNS_DEFAULT_DXE_T2;
	hwidb->x25_t3 = CMNS_DEFAULT_DXE_T3;
    } else {
    	if (ISDTE(hwidb, NULL)) {
	   hwidb->x25_t0 = X25_DEFAULT_DTE_T0;
	   hwidb->x25_t1 = X25_DEFAULT_DTE_T1;
	   hwidb->x25_t2 = X25_DEFAULT_DTE_T2;
	   hwidb->x25_t3 = X25_DEFAULT_DTE_T3;
    	} else {
	   hwidb->x25_t0 = X25_DEFAULT_DCE_T0;
	   hwidb->x25_t1 = X25_DEFAULT_DCE_T1;
	   hwidb->x25_t2 = X25_DEFAULT_DCE_T2;
	   hwidb->x25_t3 = X25_DEFAULT_DCE_T3;
    	}
    }

    hwidb->x25_th = X25_DEFAULT_TH;		/* ack. threshold (packets) */
    hwidb->x25_idle = X25_DEFAULT_IDLE;		/* minutes */
    hwidb->x25_nvc = X25_DEFAULT_NVC;		/* max. encapsul. VC count */
    hwidb->x25_flags = X25_LINKRESTART;

    /*
     * set the default VC ranges
     */
    if (ISCMNS_IEEE(hwidb)) {
	hwidb->x25_lic = CMNS_DEFAULT_LIC;
	hwidb->x25_hic = CMNS_DEFAULT_HIC;
	hwidb->x25_ltc = CMNS_DEFAULT_LTC;
	hwidb->x25_htc = CMNS_DEFAULT_HTC;
	hwidb->x25_loc = CMNS_DEFAULT_LOC;
	hwidb->x25_hoc = CMNS_DEFAULT_HOC;
    } else {
	hwidb->x25_lic = X25_DEFAULT_LIC;
	hwidb->x25_hic = X25_DEFAULT_HIC;
	hwidb->x25_ltc = X25_DEFAULT_LTC;
	hwidb->x25_htc = X25_DEFAULT_HTC;
	hwidb->x25_loc = X25_DEFAULT_LOC;
	hwidb->x25_hoc = X25_DEFAULT_HOC;
    }

    hwidb->x25_holdmax = X25_HOLDQ;
    hwidb->x25_holdtime = 0;

    /*
     *  the configured "x25 facility ..." values are not initialized so
     *  that they will be preserved between deencapsulate->encapsulate
     *  events
     */

    hwidb->x25_strict_ccitt_facil = FALSE;
    hwidb->x25_tcp_compress = FALSE;

    /*
     *  initialize the SNMP trap flag
     */
    hwidb->x25_snmp_restart_tx = FALSE;

    /*
     *  initialize the administrative attributes
     */
    hwidb->x25_admin_modulo = hwidb->x25_modulo;
    hwidb->x25_admin_lic = hwidb->x25_lic;
    hwidb->x25_admin_hic = hwidb->x25_hic;
    hwidb->x25_admin_ltc = hwidb->x25_ltc;
    hwidb->x25_admin_htc = hwidb->x25_htc;
    hwidb->x25_admin_loc = hwidb->x25_loc;
    hwidb->x25_admin_hoc = hwidb->x25_hoc;
    hwidb->x25_admin_pin = hwidb->x25_pin;
    hwidb->x25_admin_pout = hwidb->x25_pout;
    hwidb->x25_admin_win = hwidb->x25_win;
    hwidb->x25_admin_wout = hwidb->x25_wout;

    /*
     *	initialize the statistics counters
     */
    hwidb->x25_restartssent = 0;
    hwidb->x25_restartsrcvd = 0;
    hwidb->x25_callssent = 0;
    hwidb->x25_callssentfailed = 0;
    hwidb->x25_callsrcvd = 0;
    hwidb->x25_callsrcvdfailed = 0;
    hwidb->x25_callsfrwd = 0;
    hwidb->x25_callsfrwdfailed = 0;
    hwidb->x25_diagssent = 0;
    hwidb->x25_diagsrcvd = 0;
    hwidb->x25_provider_clears = 0;
    hwidb->x25_dte_resets = 0;
    hwidb->x25_dce_resets = 0;
    hwidb->x25_unexp_restarts_rx = 0;
    hwidb->x25_data_rx = 0;
    hwidb->x25_data_tx = 0;
    hwidb->x25_errors_accused = 0;
    hwidb->x25_interrupt_rx = 0;
    hwidb->x25_interrupt_tx = 0;
    hwidb->x25_t0_count = 0;
    hwidb->x25_t1_count = 0;
    hwidb->x25_t2_count = 0;
    hwidb->x25_t3_count = 0;
    hwidb->x25_retries_exhausted = 0;
    hwidb->x25_clears_exhausted = 0;

    hwidb->si_x25_restartssent = 0; 
    hwidb->si_x25_restartsrcvd = 0;
    hwidb->si_x25_callssent = 0;
    hwidb->si_x25_callssentfailed = 0;
    hwidb->si_x25_callsrcvd = 0; 
    hwidb->si_x25_callsrcvdfailed = 0;
    hwidb->si_x25_callsfrwd = 0;
    hwidb->si_x25_callsfrwdfailed = 0;
    hwidb->si_x25_diagssent = 0;
    hwidb->si_x25_diagsrcvd = 0;
    hwidb->si_x25_provider_clears = 0;
    hwidb->si_x25_dte_resets = 0;
    hwidb->si_x25_dce_resets = 0;
    hwidb->si_x25_unexp_restarts_rx = 0;
    hwidb->si_x25_data_rx = 0;
    hwidb->si_x25_data_tx = 0;
    hwidb->si_x25_errors_accused = 0;
    hwidb->si_x25_interrupt_rx = 0;
    hwidb->si_x25_interrupt_tx = 0;
    hwidb->si_x25_t0_count = 0;
    hwidb->si_x25_t1_count = 0;
    hwidb->si_x25_t2_count = 0;
    hwidb->si_x25_t3_count = 0;
    hwidb->si_x25_retries_exhausted = 0;
    hwidb->si_x25_clears_exhausted = 0;

    /*
     *  lower layer-specific setup
     */
    if (ISCMNS_IEEE(hwidb)) {
	llc2_start();
	llc2_register(SAP_CMNS, LINK_LLC2, x25_llc2_register);
    } else { /* serial */
	/*
	 *  initialize the lower layer
	 */
    	if (! lapb_setup(csb, hwidb)) {
	    unqueue(&x25idbQ, hwidb->x25idb);
	    free(hwidb->x25idb);
	    hwidb->x25idb = NULL;
	    return(FALSE);
	}

	/*
	 * encap lapb followed by encap x25 does not result in a call to
	 * x25_deencapsulate().  so get rid of any compress predictor
	 * malloc's here.
	 */
	no_compression(hwidb);

	/*
     	 *  overwrite the lower layer protocol handling vectors
     	 */
	hwidb->vencap = x25_vencapsulate;
	hwidb->bridge_vencap = (bridge_vencap_t)return_null;
    	hwidb->deencap = x25_deencapsulate;
    	hwidb->getlink = x25_getlink;
    	hwidb->iqueue = lapb_input;		/* not used really */
    	hwidb->parse_packet = x25_parse_packet;
	hwidb->extract_hwaddr = invalidate_hwaddr;
	hwidb->extract_snpa = x25_extract_addr;
    	hwidb->broadcast = lapb_bcast;
	hwidb->show_support = x25_interface;   /* sanjeev, fix CSCdi06440 */
	hwidb->linestate = x25_linestate;
    }

    /*
     *  tell SNMP that we've initialized
     */
    hwidb->reason = "X.25 initialized";
    reg_invoke_ifmib_link_trap(LINK_DOWN_TRAP, hwidb);

    /*
     *	initialize the interface LCI and address tables
     */
    x25_restart(hwidb, FALSE);

    /*
     *	fork off a X.25 timed event task
     */
    if (x25_rf == 0)
	x25_rf = cfork(x25_timer, 0L, 1800, "X25 Timer", 0);

    /*
     *  per-software-IDB setup
     */
    FOR_ALL_SWIDBS_ON_HW(hwidb, swidb) {

	/*
	 * Disable IP multicast fastswitching on interface if X.25 
	 * encapsulation is in effect. 
	 */
	if (!ISCMNS_IEEE(hwidb)) swidb->ip_multicast_fs = FALSE;

	/*
	 * if bridge circuit group was previously enabled on this interface
	 * then remove the circuit group
	 */
	if (swidb->tbridge_on_swidb) {
	    reg_invoke_bridge_circuit_remove(swidb);
	    hwidb->x25_bridge = TRUE;
	    hwidb->span_bridge_process_force |= SPAN_BR_PROC_X25_BRIDGE;
	}
    }

    return(TRUE);
}

/*
 *  x25_cstate
 *  Called by LAPB (CONS uses cons_cstate) when the line protocol goes up/down.
 *  This routine is *not* an interface vector routine.
 */

void x25_cstate (hwidbtype *hwidb)
{
    if (ISCMNS_IEEE(hwidb))
	return; /* no global Restart yet */

    /*
     * if system has not yet completed initialization, just mark this
     * interface for inspection by the background process.
     */
    if (!systeminit_flag) {
	hwidb->x25_restart = TRUE;
	return;
    }
	
    /*
     *	reset the link
     */
    hwidb->x25_state = X25_R1;
    TIMER_STOP(hwidb->x25_timer);
    hwidb->x25_retransmit = 0;

    if (hwidb->lapb_upstate) {
	/*
	 *  service provider is available--
	 *  flush existing connections (which may exist if LAPB hiccuped)
	 */
	hwidb->x25_cause = X25_RESTART_NETWORK_OPERATIONAL;
	hwidb->x25_diag = X25_DIAG_NO_INFO;
	x25_sendcause(hwidb, NULL, X25_RESTART,
		      hwidb->x25_cause, hwidb->x25_diag, NULL);
    } else {
	/*
	 *  service provider is not available
	 */
	if (hwidb->x25_upstate) {
	    hwidb->x25_upstate = FALSE;

            /* NmEvent Hook 4 */
            /* lapb link down trap is moved to LinkDownEvent */
            hwidb->failCause = Lapb_KeepAliveFailure;
            LinkDownEvent(hwidb);     /* call link down event */
	}

	hwidb->x25_cause = X25_RESTART_LOCAL_PROC_ERR;
	hwidb->x25_diag = X25_DIAG_LINK_OUT_OF_ORDER;

	/*
	 *  perform a task-level serivce reset
	 */
	hwidb->x25_restart = TRUE;
    }
}

/*
 * x25_interface
 * Output information for the show interface command
 */

void x25_interface (register hwidbtype *hwidb)
{
    char c;
    
    lapb_interface(hwidb);  /* sanjeev, fix CSCdi06440 */

    c = ISDCE(hwidb, NULL) ? '1' : '2';
    printf(
"\n  X25 %s%s, address %*s, state %s, modulo %d, timer %u"
"\n      Defaults: %s encapsulation, idle %d, nvc %d"
"\n        input/output window sizes %d/%d, packet sizes %d/%d"
"\n      Timers: T%c0 %d, T%c1 %d, T%c2 %d, T%c3 %d, TH %d"
"\n      Channels: ",
	ISDCE(hwidb, NULL) ? "DCE" : "DTE",
	ISBFE(hwidb) ? " BFE" : (IS_NON_BFE_DDN(hwidb) ? " DDN" : ""),
	hwidb->x25_address->length, hwidb->x25_address->x121_addr,
	x25_prstate(hwidb, NULL), hwidb->x25_modulo,
	TIME_LEFT_SLEEPING(hwidb->x25_timer) / ONESEC,
	(hwidb->x25_cud_pid_def == X25_CUD_PID_IETF) ? "IETF" : "cisco",
	hwidb->x25_idle, hwidb->x25_nvc,
	hwidb->x25_win, hwidb->x25_wout,
	1 << hwidb->x25_pin, 1 << hwidb->x25_pout,
	c, hwidb->x25_t0, c, hwidb->x25_t1, c, hwidb->x25_t2, c, hwidb->x25_t3,
	hwidb->x25_th);

    if (hwidb->x25_lic || hwidb->x25_hic)
	printf("Incoming-only %d-%d, ", hwidb->x25_lic, hwidb->x25_hic);
    else
	printf("Incoming-only none, ");
    if (hwidb->x25_ltc || hwidb->x25_htc)
	printf("Two-way %d-%d, ", hwidb->x25_ltc, hwidb->x25_htc);
    else
	printf("Two-way none, ");
    if (hwidb->x25_loc || hwidb->x25_hoc)
	printf("Outgoing-only %d-%d", hwidb->x25_loc, hwidb->x25_hoc);
    else
	printf("Outgoing-only none");

    /*
     *  if needed, generate information on administrative values that
     *  have been configured but are not yet operational
     */
    if (hwidb->x25_modulo != hwidb->x25_admin_modulo ||
	hwidb->x25_lic != hwidb->x25_admin_lic ||
	hwidb->x25_hic != hwidb->x25_admin_hic ||
	hwidb->x25_ltc != hwidb->x25_admin_ltc ||
	hwidb->x25_htc != hwidb->x25_admin_htc ||
	hwidb->x25_loc != hwidb->x25_admin_loc ||
	hwidb->x25_hoc != hwidb->x25_admin_hoc ||
	hwidb->x25_pin != hwidb->x25_admin_pin ||
	hwidb->x25_pout != hwidb->x25_admin_pout ||
	hwidb->x25_win != hwidb->x25_admin_win ||
	hwidb->x25_wout != hwidb->x25_admin_wout) {
	printf("\n      (%s modulo %d, "
	       "\n       Window size: input %d output %d, "
	       "Packet size: input %d, output %d"
	       "\n       Channels: ",
	   x25_valid_config(hwidb, FALSE) ?
		"configuration on RESTART:" : "INVALID configuration:",
	   hwidb->x25_admin_modulo,
	   hwidb->x25_admin_win, hwidb->x25_admin_wout,
	   1 << hwidb->x25_admin_pin, 1 << hwidb->x25_admin_pout);
	if (hwidb->x25_admin_lic || hwidb->x25_admin_hic)
	    printf("Incoming-only %d-%d, ",
		hwidb->x25_admin_lic, hwidb->x25_admin_hic);
	else
	    printf("Incoming-only none, ");
	if (hwidb->x25_admin_ltc || hwidb->x25_admin_htc)
	    printf("Two-way %d-%d, ",
		hwidb->x25_admin_ltc, hwidb->x25_admin_htc);
	else
	    printf("Two-way none, ");
	if (hwidb->x25_admin_loc || hwidb->x25_admin_hoc)
	    printf("Outgoing-only %d-%d)",
		hwidb->x25_admin_loc, hwidb->x25_admin_hoc);
	else
	    printf("Outgoing-only none)");
    }

    printf("\n      RESTARTs %d/%d CALLs %d+%d/%d+%d/%d+%d DIAGs %d/%d",
	(hwidb->x25_restartssent - hwidb->si_x25_restartssent),
	(hwidb->x25_restartsrcvd - hwidb->si_x25_restartsrcvd),
	(hwidb->x25_callssent - hwidb->si_x25_callssent),
	(hwidb->x25_callssentfailed - hwidb->si_x25_callssentfailed),
	(hwidb->x25_callsrcvd - hwidb->si_x25_callsrcvd),
	(hwidb->x25_callsrcvdfailed - hwidb->si_x25_callsrcvdfailed),
	(hwidb->x25_callsfrwd - hwidb->si_x25_callsfrwd),
	(hwidb->x25_callsfrwdfailed - hwidb->si_x25_callsfrwdfailed),
	(hwidb->x25_diagssent - hwidb->si_x25_diagssent),
	(hwidb->x25_diagsrcvd - hwidb->si_x25_diagsrcvd));
}

/*
 * x25_clearcounters
 * Clear X.25 counters when a "clear counters" command is issued.  Clear is done
 * as in if/network.c by copying the data into shadow variables.
 */

void x25_clearcounters(hwidbtype *hwidb)
{
    hwidb->si_x25_restartssent = hwidb->x25_restartssent;
    hwidb->si_x25_restartsrcvd = hwidb->x25_restartsrcvd;
    hwidb->si_x25_callssent = hwidb->x25_callssent;
    hwidb->si_x25_callssentfailed = hwidb->x25_callssentfailed;
    hwidb->si_x25_callsrcvd = hwidb->x25_callsrcvd;
    hwidb->si_x25_callsrcvdfailed = hwidb->x25_callsrcvdfailed;
    hwidb->si_x25_callsfrwd = hwidb->x25_callsfrwd;
    hwidb->si_x25_callsfrwdfailed = hwidb->x25_callsfrwdfailed;
    hwidb->si_x25_diagssent = hwidb->x25_diagssent;
    hwidb->si_x25_diagsrcvd = hwidb->x25_diagsrcvd;
    hwidb->si_x25_provider_clears = hwidb->x25_provider_clears;
    hwidb->si_x25_dte_resets = hwidb->x25_dte_resets;
    hwidb->si_x25_dce_resets = hwidb->x25_dce_resets;
    hwidb->si_x25_unexp_restarts_rx = hwidb->x25_unexp_restarts_rx;
    hwidb->si_x25_data_rx = hwidb->x25_data_rx;
    hwidb->si_x25_data_tx = hwidb->x25_data_tx;
    hwidb->si_x25_errors_accused = hwidb->x25_errors_accused;
    hwidb->si_x25_interrupt_rx = hwidb->x25_interrupt_rx;
    hwidb->si_x25_interrupt_tx = hwidb->x25_interrupt_tx;
    hwidb->si_x25_t0_count = hwidb->x25_t0_count;
    hwidb->si_x25_t1_count = hwidb->x25_t1_count;
    hwidb->si_x25_t2_count = hwidb->x25_t2_count;
    hwidb->si_x25_t3_count = hwidb->x25_t3_count;
    hwidb->si_x25_retries_exhausted = hwidb->x25_retries_exhausted;
    hwidb->si_x25_clears_exhausted = hwidb->x25_clears_exhausted;
}

/*
 * x25_get_idb_x25_address
 *
 * the interface's configured X.121 address
 */

addrtype *x25_get_idb_x25_address(idbtype *idb)
{
    hwidbtype *hwidb;
    hwidb = hwidb_or_null(idb);
    if (hwidb != NULL) {
        return(hwidb->x25_address);
    }
    return (NULL);
}
      

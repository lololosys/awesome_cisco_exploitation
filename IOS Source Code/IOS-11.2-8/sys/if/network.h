/* $Id: network.h,v 3.6.4.12 1996/09/06 01:11:16 snyder Exp $
 * $Source: /release/112/cvs/Xsys/if/network.h,v $
 *------------------------------------------------------------------
 * network.h -- device and protocol independent network interface prototypes
 *
 * February 1991, David Hampton
 *
 * Copyright (c) 1991-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: network.h,v $
 * Revision 3.6.4.12  1996/09/06  01:11:16  snyder
 * CSCdi68277:  make shr_core, shr_ukernel, sub_cxxxx sub_core_platform
 *              things const, save 712 out of data, 208 from image
 * Branch: California_branch
 *
 * Revision 3.6.4.11  1996/08/23  08:09:52  xtang
 * CSCdi60427:  parser error when set mtu size on hssi pa.
 * Branch: California_branch
 *              CSCdi60474 get negative maxdatagram when set max mtu
 *              on hssi pa. is found to be a duplicate of this bug.
 *              CSCdi39825 patch for 11.2 largely solved the
 *              problem for 11.2. Now add a recovery of the
 *              swidb->user_configured_mtu flag.
 *
 * Revision 3.6.4.10  1996/08/20  18:08:44  wmay
 * CSCdi39825:  ISDN PRI, setting d-channel mtu size not reflected on
 * b-channels - set for entire dialer rotary group
 * Branch: California_branch
 *
 * Revision 3.6.4.9  1996/08/13  02:19:05  gstovall
 * CSCdi39484:  Giant packets output on ethernet interface
 * Branch: California_branch
 * Make sure we drop giants, even if they are just a wee bit too big.
 *
 * Revision 3.6.4.8  1996/08/03  23:27:18  fox
 * CSCdi63400:  high priority compatibility queue must die
 * Branch: California_branch
 * Remove high priority compatibility queue and activatehigh flag.
 * Convert high priority cfork'd processes to new-style processes with
 * watched queues, timers, and booleans.  Split Multilink PPP input
 * into separate high priority process.  Split load and busy calculations
 * into separate processes.  Speed up PPP input and scheduler processing.
 * Miscellaneous code changes.
 *
 * Revision 3.6.4.7  1996/05/17  11:23:06  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.5.2.4  1996/05/05  23:09:57  ppearce
 * Sync to IbuMod_Calif_baseline_960504
 *
 * Revision 3.5.2.3  1996/04/26  07:55:21  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.5.2.2  1996/04/03  14:38:28  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.5.2.1  1996/03/17  17:51:25  ppearce
 * Sync IbuMod_Calif_branch to V111_1_3
 *
 * Revision 3.6.4.6  1996/05/09  14:30:18  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.6.4.5  1996/05/04  00:46:47  wilber
 * Branch: California_branch
 *
 * Integrated Routing and Bridging
 * - Restore the input to the Bridge-group Virtual Interface if necessary
 * - Support user settable mtu for the BVI.
 * - Introduce global_irb_enable, SET_BRIDGING_RXTYPE and "show idb irb"
 *
 * Revision 3.6.4.4.4.1  1996/04/27  07:04:01  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.6.4.4  1996/04/20  01:50:38  dino
 * CSCdi54104:  CGMP must support source-only systems in switched Cat5000
 *              network
 * Branch: California_branch
 *
 * Revision 3.6.4.3  1996/04/17  13:40:20  fred
 * CSCdi54910:  commit rsvp, traffic shaping, and RED
 * Branch: California_branch
 *
 * Revision 3.6.4.2.6.1  1996/04/08  01:54:51  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.6.4.2  1996/03/23  01:32:35  slin
 * Branch: California_branch
 *
 * AppleTalk makes its exodus from the idb.
 *
 * Revision 3.6.4.1  1996/03/18  20:14:21  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.6.8.1  1996/03/28  17:25:41  rbadri
 * Branch: LE_Syn_Cal_V111_1_3_branch
 * MTU support for sub-interfaces (used by ATM interfaces)
 *
 * Revision 3.6  1996/03/09  22:55:21  widmer
 * CSCdi47890:  HTTP server doesnt work
 * Add HTTP server
 *
 * Revision 3.5  1996/02/13  08:13:11  dstine
 * CSCdi48797:  Extraneous definitions in interface_private.h
 *
 * Revision 3.4  1995/11/24  19:38:22  rbadri
 * CSCdi36121:  Line protocol is up, while LANE client is non-operational
 * subif_line_state variable will track the swidb's line state. It
 * is not normally altered unless the protocols controlling the
 * sub-interface wish to do so. An interface routine
 * subif_line_statechange() is provided to change swidb's line state.
 *
 * Revision 3.3  1995/11/17  09:30:27  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:38:45  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:51:25  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.6  1995/11/08  21:03:15  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.5  1995/09/09  00:52:51  ronnie
 * CSCdi35837:  ping works, but traceroute fails to go across ATM interface
 * Introduce a new variable type, fuzzy, for hwidb->broadcast to return
 * instead of boolean.  This allows the return of NOT_SURE.  Adjust all
 * hwidb->broadcast routines to return fuzzy instead of boolean.
 *
 * Revision 2.4  1995/08/08  19:28:53  fred
 * CSCdi36704:  IP Fastswitching / WFQ cleanup for high-end
 *         enable WFQ/PQ/CQ of SMDS interfaces.
 *         adjust WFQ discard policy to permit one conversation to use the
 *         entire congestive discard threshold
 *
 * Revision 2.3  1995/07/05  20:49:02  hampton
 * Consolidate common token ring variables into the idb, instead of having
 * them in the private data structure for each type of token ring
 * interface.  Remove overloading of the delay variable by token rings.
 * Eliminate direct references to the system clock.  [CSCdi36721]
 *
 * Revision 2.2  1995/06/16 22:32:50  shankar
 * CSCdi35485:  autoinstall over Frame Relay breaks in 103-3.3
 *
 * Revision 2.1  1995/06/07  20:54:05  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __NETWORK_H__
#define __NETWORK_H__

#define TIMEOUT (3*ONESEC)              /* net output timeout */
#define KEEPALIVE_DEFAULT  (10)         /* 10 secs between keepalive actions*/

/*
 * load interval default.  This is an index into an array where each
 * element corresponds to intervals of 30 seconds (ie: the 0th element
 * is for 30 seconds, the 1st is for 60 seconds...
 * The default is for 5 minutes.
 */
#define DATARATE_INTERVAL_DEF (((5*60)/30)-1)

extern mgd_timer if_cstate_timer;
extern boolean global_crb_enable;
extern boolean global_irb_enable;
extern ushort global_rxtype_flags[];

#define SIZEOF_RXTYPE_FLAGS ((sizeof(ushort)) * MAX_RXTYPES)

typedef enum netinput_events_ {
    INPUT_QUEUE,
    ABORT_QUEUE
} netinput_events;


extern uchar dec_span_address[];
extern uchar ieee_span_address[];
extern uchar esis_8023_all_es_multi[];
extern uchar esis_8023_all_is_multi[];
extern uchar esis_8025_all_es_multi[];
extern uchar esis_8025_all_is_multi[];
extern uchar isis_8025_all_is_multi[];
extern uchar isis_all_l1_is_multi[];
extern uchar isis_all_l2_is_multi[];
extern uchar standby_groups[];
extern uchar transit_bridge_multicast[];
extern const uchar smds_bridge_oui[];
extern const uchar atm_tbridge_oui[];
extern uchar trbaddr1[];
extern uchar bit_swapped_transit_multicast[];
extern uchar default_mac_addr[];
extern const char set_maxdgram_failed[];
extern uchar cgmp_address[];

extern uchar isl_vlan_multicast[];
extern uchar isl_vlan_snap[];

extern idbtype *nullidb;


void encapsulation_command(parseinfo *csb);
void description_command(parseinfo *csb);
extern boolean red_enqueue(hwidbtype *, paktype *, enum HEADTAIL);
extern paktype *red_dequeue(hwidbtype *);
boolean holdq_enqueue(hwidbtype *, paktype *, enum HEADTAIL);
boolean holdq_enqueue_fail(hwidbtype *, paktype *, enum HEADTAIL, int qnum);
boolean holdq_enqueue_fast_reserve(hwidbtype *, paktype *, int reserve);
boolean interface_up(idbtype *);
boolean hw_interface_up(hwidbtype *);
fuzzy physical_broadcast(paktype *);
boolean supports_maxdgram(hwidbtype *);
const char *encstring(uint);
extern uint encaptype(const char *);
char *parse_interface(char *, idbtype **);
char *print_idbflags(hwidbtype *);
char *print_idbstate(idbtype *);
char *print_hwidbstate(hwidbtype *);
char *print_linktype(int);
process net_background(void);
process critical_background(void);
process net_onesecond(void);
process net_periodic(void);
paktype *holdq_dequeue(hwidbtype *);
void backup_adjust(hwidbtype *);
void clear_idb(hwidbtype *);
void coerce_hwaddr(uchar *, hwidbtype *);
void datagram_out(paktype *);
void find_802addr(paktype *, uchar *, boolean);
void holdq_init(hwidbtype *, int);
void holdq_resize(hwidbtype *idb, int qsize);
void set_default_queueing(hwidbtype *);
void holdq_reset(hwidbtype *);
void net_cstate(hwidbtype *, int);
void raw_enqueue(hwidbtype *, paktype *);
void net_bridge_enqueue(paktype *);
void net_bridge_copy(paktype *);
void net_bridge_enqueue_broadcast(paktype *);
void net_bridge_copy_broadcast(paktype *);
void protocol_discard(paktype *, boolean);
void netinput_enqueue(paktype *);
boolean delete_interface(idbtype *);
void route_adjust(hwidbtype *);
void setup_all_bridging(void);
void show_hwidb_chain(hwidbtype *, void (*rtn)(hwidbtype *));
void show_swidb_chain(idbtype *, void (*rtn)(idbtype *), int size);
void show_idb(idbtype *);
void shutdown_interface(idbtype *, boolean);
void reset_if(hwidbtype *);
void shutdown_going_down(hwidbtype *);
void shutdown_coming_up(hwidbtype *);
void transition_adjust(hwidbtype *);
ulong create_unique_number(void);
void new_physical_mtu(hwidbtype *, int, boolean);
ulong idb_inputs(hwidbtype *);
ulong idb_fast_in(hwidbtype *);
ulong idb_auto_in(hwidbtype *);
ulong idb_outputs(hwidbtype *);
ulong idb_fast_out(hwidbtype *);
ulong idb_auto_out(hwidbtype *);
ulong idb_tx_cumbytes(hwidbtype *);
ulong idb_tx_fast_cumbytes(hwidbtype *);
ulong idb_tx_auto_cumbytes(hwidbtype *);
ulong idb_rx_cumbytes(hwidbtype *);
ulong idb_rx_fast_cumbytes(hwidbtype *);
ulong idb_rx_auto_cumbytes(hwidbtype *);
void network_init(void);
void nullidb_init(void);
boolean main_if_or_vc(idbtype *);
boolean main_if_or_vc_or_vlan(idbtype *);
boolean main_if_or_iftype(idbtype *, ulong);
void set_subif_link_type(idbtype *);
boolean can_select_subif_type(idbtype *);
void shutdown_subif(idbtype *, ulong);
void subif_line_statechange(idbtype *, ulong, boolean);
int llc_mtu(hwidbtype *);
boolean routed_protocols_on_idb(idbtype *);
boolean invalidate_snpa(paktype *, hwaddrtype *, enum SRCDST);
boolean invalidate_hwaddr(hwidbtype *hwidb, hwaddrtype *hwaddr);
boolean ieee_extract_hwaddr(hwidbtype *hwidb, hwaddrtype *hwaddr);
boolean usable_interfaces(void);
void sys_set_growbuffers(void);
void set_default_interface_bandwidth(idbtype *idb, ulong visible_bandwidth);
void set_default_interface_delay(idbtype *idb, ulong new_delay);
void set_max_interface_mtu(idbtype *swidb, long sub_delta);
int  if_has_multiple_mtu(hwidbtype *hwidb);
void fair_clear_counters(hwidbtype *);
void red_clear_counters (hwidbtype *idb);
void red_set_precedence_levels(hwidbtype *idb, short managed_to);
void critical_background_wake(void);
void default_encaps_display(idbtype *);
void if_maxdgram_adjust(hwidbtype *hwidb, uint maxdgram);
boolean if_change_mtu(idbtype *swidb, long mtu);

extern void fs_table_fill(fastswitch_type, char *, int);

#endif

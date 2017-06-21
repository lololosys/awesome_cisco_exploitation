/* $Id: ipfast_mci.c,v 3.14.6.18 1996/09/04 17:16:24 ahh Exp $
 * $Source: /release/112/cvs/Xsys/ip/ipfast_mci.c,v $
 *------------------------------------------------------------------
 * ipfast_mci.c -- MCI and cBus IP Fast Switching Support
 *
 * 20-March-1988, Kirk Lougheed
 *
 * Copyright (c) 1988-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * Gurus: tli, pst
 *
 * $Log: ipfast_mci.c,v $
 * Revision 3.14.6.18  1996/09/04  17:16:24  ahh
 * CSCdi67885:  NAT: sub-interfaces not supported properly
 * Allow interface commands to work in sub-interface configuration
 * mode, allow translation to be enabled on some sub-interfaces
 * but not others and still have the fast-path work.
 * Branch: California_branch
 *
 * Revision 3.14.6.17  1996/08/17  03:00:40  hou
 * CSCdi64529:  Fast switching between FEIP and FSIP doesnt work.
 * Branch: California_branch
 * - add ISL vLAN code in ip fastswitching path for 7000.
 *
 * Revision 3.14.6.16  1996/08/14  14:01:38  dblair
 * CSCdi64968:  Implment outbound half-fastswitching for multilink PPP
 * Branch: California_branch
 * enable multilink half-fastswitching on _les but disable on _mci
 * and _pas platforms
 *
 * Revision 3.14.6.15  1996/08/10  20:05:44  sbelair
 * CSCdi62311:  Encrypted tunnels dont work
 * Branch: California_branch
 * Fix previous (blind) checkin which broke the build. For now, if
 * the tunnel interface has crypto vectors (i.e., there is a crypto
 * map assigned to the tunnel interface), then don't fastswitch the packet,
 * since it likely needs to be decrypted).
 *
 * Revision 3.14.6.14  1996/08/09  23:51:09  sbelair
 * CSCdi62311:  Encrypted tunnels dont work
 * Branch: California_branch
 * For now at least, if on the fast tunneling path a packet needs
 * decryption before it can be processed further (decided by
 * crypto_map_check), then pass the packet to process-level.
 *
 * Revision 3.14.6.13  1996/07/30  01:08:16  mbeesley
 * CSCdi55762:  Need to support fast IP multicasting on platforms
 * capable of scatter/gather IE c7200
 * Branch: California_branch
 *
 * Revision 3.14.6.12  1996/07/23  18:46:40  ppearce
 * CSCdi63747:  Move srb_multiring out of SRB subblock
 * Branch: California_branch
 *   (1) srb_multiring doesn't really belong exclusively to SRB
 *         move srb_multiring out of SRB subblock and into swidb
 *         provide srb_multiring accessor routines
 *   (2) determine_rif() should take swidb as argument rather than hwidb
 *
 * Revision 3.14.6.11  1996/07/09  17:24:02  awu
 * CSCdi45135:  IP: bogus entries in sh ip accounting
 * Branch: California_branch
 *
 * Revision 3.14.6.10  1996/07/09  05:59:29  ppearce
 * CSCdi59527:  inlines waste code space
 * Branch: California_branch
 *   Just say "no" to inline abuse - SRB subblock inlines removed
 *
 * Revision 3.14.6.9  1996/07/06  05:53:39  thille
 * CSCdi62136:  seq: iphost and req: iphost duplicated and waste memory
 * Branch: California_branch
 * Put copies of the strings in common_strings.c/h and reference them
 * instead of having multiple copies of the literals.  Save 660 bytes.
 *
 * Revision 3.14.6.8  1996/06/18  01:45:18  hampton
 * Split the monolithic traffic array into smaller per-protocol traffic
 * arrays.  [CSCdi59224]
 * Branch: California_branch
 *
 * Revision 3.14.6.7  1996/06/01  00:24:48  ahh
 * CSCdi59212:  NAT: unsupported switching paths can be disabled more se...
 * Branch: California_branch
 *
 * Revision 3.14.6.6  1996/05/17  11:24:55  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.9.6.6  1996/05/05  23:12:34  ppearce
 * Sync to IbuMod_Calif_baseline_960504
 *
 * Revision 3.9.6.5  1996/05/02  22:05:31  hampton
 * Remove support for old Multibus token ring cards.
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.9.6.4  1996/04/26  15:21:26  ppearce
 * IBU modularity - SRB subblock (part 1)
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.9.6.3  1996/04/03  14:42:21  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.9.6.2  1996/03/28  22:04:32  rogorman
 * CSCdi51077:  Router with CIP fails to fragment when Fastswitching
 * Branch: IbuMod_Calif_branch
 * -Before putting packet from CIP on the holq, the pak->network_start
 *         must be adjusted beyond the CHANNEL_ENCAPBYTES.
 * -Also, fixed problem where input packets from any interface that were
 *         placed on the holq were counted twice (once for process and
 *         once for fast
 *
 * Revision 3.9.6.1  1996/03/17  17:52:41  ppearce
 * Sync IbuMod_Calif_branch to V111_1_3
 *
 * Revision 3.14.6.5  1996/05/04  01:03:50  wilber
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
 * Revision 3.14.6.4  1996/04/27  05:51:47  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 3.14.6.3  1996/04/24  00:06:04  ahh
 * CSCdi55483:  NAT: code review comments, various bug fixes
 * Fast-path fixup, cruft removal.
 * Branch: California_branch
 *
 * Revision 3.4.8.3  1996/04/26  01:23:08  syiu
 * Branch: Dial1_branch
 * Dial1_branch to IosSec_postmerge_960419
 *
 * Revision 3.4.8.2  1996/04/17  02:06:06  syiu
 * Branch: Dial1_branch
 * Sync Dial1_branch to V111_1_3
 *
 * Revision 3.4.8.1  1996/02/23  20:23:42  lbustini
 * Branch: Dial1_branch
 * Move dialer fields out of the idb.
 *
 * Revision 3.14.6.2  1996/04/19  15:41:59  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commit
 *
 * Revision 2.2.6.14  1996/04/11  23:15:02  carrel
 * Branch: IosSec_branch
 * IP code review comments #1
 *
 * Revision 2.2.6.13  1996/04/09  20:37:38  che
 * Branch: IosSec_branch
 * Sync to California_branch (960405)
 *
 * Revision 2.2.6.12  1996/03/30  05:19:12  carrel
 * cvs fixes
 *
 * Revision 2.2.6.11  1996/03/19  08:09:12  che
 * Branch: IosSec_branch
 * Sync to V111_1_3
 *
 * Revision 2.2.6.10  1996/02/27  06:40:09  xliu
 * Branch: IosSec_branch
 * Add crypto_registry subsys.
 *
 * Revision 2.2.6.9  1996/02/21  03:30:02  che
 * Branch: IosSec_branch
 * Sync to 11.1 Mainline (02/20/96)
 *
 * Revision 2.2.6.8  1996/02/13  19:18:04  carrel
 * Branch: IosSec_branch
 * Crypto Map checkin (finally)
 *
 * Revision 2.2.6.7  1996/01/23  05:43:22  che
 * Branch: IosSec_branch
 * Sync with 11.1 mainline. (960122)
 *
 * Revision 2.2.6.6  1996/01/20  17:02:22  xliu
 * Branch: IosSec_branch
 * Solve 7000 problem with ip les fs.
 *
 * Revision 2.2.6.5  1996/01/19  20:27:43  xliu
 * Branch: IosSec_branch
 * Add crypto registry include files.
 *
 * Revision 2.2.6.4  1996/01/04  19:02:55  xliu
 * Branch: IosSec_branch
 * Replaced the direct crypto system call as the crypto subsys registry
 * call.
 *
 * Revision 2.2.6.3  1995/12/19  03:56:26  che
 * Branch: IosSec_branch
 * Re-sync to Arkansas to fix things that broke.
 *
 * Revision 2.2.6.2  1995/11/17  17:01:06  carrel
 * Branch: IosSec_branch
 * Sync to Arkansas
 *
 * Revision 2.2.6.1  1995/09/26  04:13:06  carrel
 * Branch: IosSec_branch
 * Initial commit of encryption ACL code.
 *
 * Revision 3.14.6.1  1996/03/18  20:16:57  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.5.2.3  1996/03/16  06:58:46  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.5.2.2  1996/03/13  01:37:52  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.5.2.1  1996/02/20  00:59:55  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.14  1996/03/06  06:45:06  awu
 * CSCdi50609:  ip fastswitching from AIP onto FDDI does not support RIF
 *   - added code to handle the case.
 *
 * Revision 3.13  1996/02/29  23:17:46  gchristy
 * CSCdi49734:  IP packets fast-switched from ATM to Token Ring with RIF
 * corrupt
 * - Use the cache encap length to deal with RIFs.
 *
 * Revision 3.12  1996/02/29  04:07:07  dino
 * CSCdi49943:  IP ttl may not be decremented with both route-cache and
 *              mroute-cache
 *
 * Revision 3.11  1996/02/27  23:13:57  awu
 * CSCdi44580:  Fastswitching IP traffic from AIP to FIP adds an extra
 *              byte to the end of packets.
 *
 * Revision 3.10  1996/02/27  19:27:46  achopra
 * CSCdi45189:  "show atm traffic" shows different (wrong) packet counts
 *
 * Revision 3.9  1996/02/07  04:04:52  mmcneali
 * CSCdi45328:  VLAN Translator/Routers FAIL to decrement IP TTL.
 *
 * Revision 3.8  1996/02/07  02:34:59  gchristy
 * CSCdi48255:  IP ACL: access-lists fail when using atm_dxi encap
 *  - Another variable size encapsulation. Deal with it correctly.
 *
 * Revision 3.7  1996/02/06  00:48:48  mleelani
 * CSCdi46016:  Mcast fastswitching in GRE tunnel is inop on 7k
 * Wrong sniff offsets for input tunnels on FDDI/Tokenring.
 *
 * Revision 3.6  1996/01/23  08:49:56  mmcneali
 * CSCdi47323:  vLAN encapsulated IP SSE switched.
 *
 * Faster Baby, Faster!
 *
 * Revision 3.5  1996/01/20  06:53:24  lbustini
 * CSCdi39980:  BOD not working on PRI when fastswitching is enabled
 * Use correct MIP vc number when selecting output interface.
 *
 * Revision 3.4  1996/01/02  21:53:00  gchristy
 * CSCdi44250:  GRE tunnel fast-switching broken on 7000
 * - Don't enable fast-switching on unicast tunnels.
 *
 * Revision 3.3  1995/12/17  06:09:49  mmcneali
 * CSCdi43825:  No fastswitching for 802.10 vLAN packets.
 *
 * Revision 3.2  1995/11/17  09:35:02  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:56:57  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.24  1995/11/08  21:04:52  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.23  1995/10/21  19:20:54  fred
 * CSCdi40287:  fsip is dropping large # of packets when fair-queue is on
 *         mci: set up pak->if_output in congestion case (CQ/PQ/RSVP)
 *         rsp: correct use of queuing routines - must use pak->oqueue()
 *              and pak->oqueue_dequeue(), was using
 *              holdq_enqueue_fast_reserve
 *              and pak->oqueue_dequeue()
 *
 * Revision 2.22  1995/09/25  08:32:08  tli
 * CSCdi40960:  ISP jumbo patch 2
 *
 * Revision 2.21  1995/09/15  07:34:20  gchristy
 * CSCdi40409:  FS input access lists on ATM subinterfaces are broken
 * - Set fast_rcvidb to swidb associated with incoming VC.
 *
 * Revision 2.20  1995/09/15  07:28:59  gchristy
 * CSCdi40376:  Access-list ignored by traffic from AIP to EIP
 * - Use correct offset based on incoming VC encapsulation.
 *
 * Revision 2.19  1995/09/07  23:09:57  wmay
 * CSCdi39353:  MIP to XXX fast switching failures - use common inline
 * for congestion path on high end and ip multicast, and make it go a
 * little more efficiently.
 *
 * Revision 2.18  1995/09/07  18:34:15  motto
 * CSCdi39868:  FDDI <-> CIP Fast switching broken
 *
 * Revision 2.17  1995/09/03  02:03:06  gchristy
 * CSCdi38269:  MCI/SCI output counters are incorrect
 * Only increment fast-switching output counts if we successfully
 * fast-switch the packet.
 *
 * Revision 2.16  1995/08/23  05:31:07  gchristy
 * CSCdi28311:  UDP spanning-tree flooding with DLD creates duplicate
 * packets
 *  - Nuke bridge_flood_pak. Create and initialize udp_flood_pak during FS
 *    init.
 *
 *  - Instead of iterating over all interfaces, use the spanning tree
 *    flood table associated with the input interface to get the list of
 *    interfaces participating in the bridge-group.
 *
 *  - If a potential output interface is part of a circuit-group, use a
 *    hash of the source ip address and destination udp port number to
 *    choose one of the members of the group.
 *
 * Revision 2.15  1995/08/19  23:07:39  fred
 * CSCdi39017:  fast switching from serial to fddi broken in 7000.
 * the offset to the start of the data link header in the "on-complex"
 * case was off by one
 *
 * Revision 2.14  1995/08/12  07:37:15  fred
 * CSCdi38525:  correct offset in HDLC->Ethernet MCI-MCI case
 *
 * Revision 2.13  1995/08/11  15:51:39  dino
 * CSCdi38636:  Shave off a few cycles processing tunnels at interrupt
 * level
 *
 * Revision 2.12  1995/08/08  19:29:48  fred
 * CSCdi36704:  IP Fastswitching / WFQ cleanup for high-end
 *
 * Revision 2.11  1995/08/01  04:35:37  gchristy
 * CSCdi37890:  Fast-switched simple ACLs from TR and FDDI use incorrect
 * offset
 * - pass rxrifwordlen unchanged to IP_FAST_FINISH
 *
 * Revision 2.10  1995/07/20  02:33:50  bbenson
 * CSCdi37383:  fastswitch output byte count is not incremented for AIP
 * - Looks like a line which incremented tx_cumbytes_fast[ACCT_PROTO_IP]
 *   for IP packets output through the AIP was dropped.
 *
 * Revision 2.9  1995/07/18  02:14:37  misko
 * CSCdi37068:  serial ppp fast-switching failure handling multiple
 * packets
 * Attempt to reposition in the buffer had the argreg, but forgot the
 * select on the cmdreg.
 *
 * Revision 2.8  1995/07/18  01:58:53  richl
 * CSCdi36765:  ip ping fails, hip (smds) to fip (snap) - can only proc.
 * switch
 *              make corrections to encap offset calculations
 *
 * Revision 2.7  1995/07/10  22:13:12  gchristy
 * CSCdi36697:  FIP to FSIP fast-switching causes ROM monitor reload
 * - Don't set isframerelay flag in call to ip_common_serial_output when
 *   using HDLC or PPP
 *
 * Revision 2.6  1995/07/10  05:20:26  fred
 * CSCdi34574:  correct offset to start of message in *normal* fast
 * switching path.
 *
 * Revision 2.5  1995/07/05  20:38:34  gchristy
 * CSCdi35850:  Eth to FDDI adds one byte to data of IP packets
 *
 * Revision 2.4  1995/06/26  08:30:58  tli
 * CSCdi36086:  MCI-to-MCI fastswitching does not respect policy routing
 *
 * Revision 2.3  1995/06/16  17:20:51  fred
 * CSCdi34574:  ip f/s serial -> fddi is broken
 * Correct error in which the RIF was not included in FDDI received header
 * size calculations. Correct error in which in some cases the IP header
 * was considered to start on the wrong boundary.
 *
 * Also, minor code cleanups in response to code review comments
 *
 * Revision 2.2  1995/06/09  03:29:46  fred
 * Change parameters to ip_pak_mac_rewrite to support IP accounting
 * in the fast path.
 * Add support of FS_DIALER in that routine.
 * Extend ipparse.c registry that displays IP datagram to display
 * UDP/TCP port as well.
 *
 * Revision 2.1  1995/06/07  20:59:13  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "interface_private.h"
#include "subsys.h"
#include "registry.h"
#include "fastswitch_registry.h"
#include "packet.h"
#include "access.h"

#include "logger.h"

#include "../os/pool.h"
#include "../os/particle.h"

#include "../if/arp.h"
#include "../if/ether.h"
#include "../if/tring.h"
#include "../if/fddi.h"
#include "../if/rif.h"
#include "config.h"
#include "../wan/serial.h"
#include "../hes/if_mci.h"
#include "../hes/if_fci.h"
#include "../hes/if_hes_common.h"
#include "../hes/if_hes_ether.h"
#include "../parser/parser_defs_access.h"
#include "../ip/ip.h"
#include "../ipmulticast/igmp.h"
#include "../crypto/crypto_public.h"
#include "../crypto/crypto_registry.h"
#include "../ip/ipaccess.h"
#include "../ip/ipaccess_private.h"
#include "../ip/ipaccess_inline.h"
#include "../ip/ipaccount.h"
#include "../ip/ipfast.h"
#include "../ip/ipfast_hes.h"
#include "../ip/ip_registry.h"
#include "../ip/tunnel.h"
#include "../wan/dialer.h"
#include "../wan/dialer_private.h"
#include "../wan/smds.h"
#include "../wan/frame_relay.h"
#include "../wan/atm_dxi.h"
#include "../wan/ppp.h"
#include "../if/network.h"
#include "../xns/novfast.h"
#include "../if/atm.h"
#include "../hes/if_aip.h"
#include "../if/channel.h"
#include "../lex/lex_fs_mci.h"
#include "../ip/gre.h"
#include "../ip/ipfast_pak.h"

#include "../vlan/vlan.h"

#include "../srt/srb_multiring.h"
#include "../srt/srt_registry.h"
#include "../tbridge/tbridge_sde.h"

/*
 * Externals
 */
extern boolean ip_fast_fragmentation (hwidbtype *inidb, hwidbtype *outidb);

/*
 * ip_pak_mac_rewrite
 *
 * Do MAC rewrite in system buffers.
 */
static void ip_hes_mac_rewrite (paktype *pak, hwidbtype *hwidb,
			 ipcache_rntype *cptr)
{
    pak->if_output = cptr->idb;

    ip_pak_write_mac_inline(pak, hwidb, cptr->m.maclong,
			    cptr->length, cptr->subint_reference_num);
}

/*
 * iphdr:	Only used when smds_unknown_fastswitch() is called.
 *		Only applicable to old MCI/CBUS I cards!
 *
 * The lex code also uses the macros and structure below.  When
 * the fastswitching code is more modular this will need to be
 * addressed.  (So does atm_dxi)
 */

static iphdrtype temp_iphdr;

inline static void write_new_iphdr (mciregtype *destptr)
{
	ulong *ip = (ulong *) &temp_iphdr;

	destptr->write1long = *ip++;
	destptr->write1long = *ip++;
	destptr->write1long = *ip++;
	destptr->write1long = *ip++;
	destptr->write1long = *ip++;
}

/*
 * check for ability to directly enqueue the message.
 *
 * in all honesty, the reason this routine exists the way it does
 * is not modularity, it is to enable the test for the queue to take place
 * without the hardware command, and still slide the statistics instructions
 * under the hardware command.
 */
inline static boolean reserve_space_move (hwidbtype *input, hwidbtype *output,
 				          mciregtype *dstreg, ushort bytes)
{
    if (output->priority_list && output->output_qcount ) {
	/* data waiting in queue, take congestion path */
	input->counters.inputs_fast[ACCT_PROTO_IP]++;
	input->counters.rx_cumbytes_fast[ACCT_PROTO_IP] += bytes;
	ip_traffic.inputs++;
    } else {

	/*
	 * The MCI doesn't care what the argreg value is -- the FCI requires that
	 * it be the buffer pool number i.e. which pool to get buffer from.
	 * Caller sets that up
	 */
	dstreg->argreg = output->mci_index;
	dstreg->cmdreg = MCI_CMD_RX_MOVE;
	input->counters.inputs_fast[ACCT_PROTO_IP]++;
	input->counters.rx_cumbytes_fast[ACCT_PROTO_IP] += bytes;
	ip_traffic.inputs++;
	if (dstreg->cmdreg == MCI_RSP_OKAY) {
	    output->counters.outputs_fast[ACCT_PROTO_IP]++;
	    ip_traffic.forwarded++;
	    return TRUE;
	}
    }
    return FALSE;
}
inline static boolean reserve_space_tx (hwidbtype *input, hwidbtype *output,
				     mciregtype *dstreg, ushort bytes)
{
    if (output->priority_list && output->output_qcount) {
	/* data waiting in queue, take congestion path */
	input->counters.inputs_fast[ACCT_PROTO_IP]++;
	input->counters.rx_cumbytes_fast[ACCT_PROTO_IP] += bytes;
	ip_traffic.inputs++;
    } else {

	/*
	 * The MCI doesn't care what the argreg value is -- the FCI requires that
	 * it be the buffer pool number i.e. which pool to get buffer from.
	 * Caller sets that up
	 */
	dstreg->argreg = output->mci_index;
	dstreg->cmdreg = MCI_CMD_SELECT;

	dstreg->argreg = output->buffer_pool;
	dstreg->cmdreg = MCI_CMD_TX1_RESERVE;
	input->counters.inputs_fast[ACCT_PROTO_IP]++;
	input->counters.rx_cumbytes_fast[ACCT_PROTO_IP] += bytes;
	ip_traffic.inputs++;
	if (dstreg->cmdreg == MCI_RSP_OKAY) {
	    output->counters.outputs_fast[ACCT_PROTO_IP]++;
	    ip_traffic.forwarded++;
	    return TRUE;
	}
    }
    return FALSE;
}

/*
 * SMDS variables: Only used when fastswitching SMDS
 * Global so as to not slow down IP_FAST_STARTUP2 macros
 */
static unsigned smds_infolen;		/* Hold No. of 'information' octets */
static smds_pdb_t *spd;			/* SMDS output struct */
static smdstype *sm;			/* SMDS header info */
static char sm_pad;			/* Padding of 0's at end of data */
static ulong sm_lastlong;		/* Last lword from inbound interface */
static ushort sm_pad_offset;		/* Offset to SMDS pad area */
static ushort sm_head_offset;		/* Offset to SMDS header area */
static ushort sm_modelen;		/* SMDS mode length, DXI or cisco */
static ushort sm_input_enc;		/* Input encap offset for SMDS pad */
static ushort sm_the_line;		/* unknown fast sw - points at line */
static ushort atm_dxi_the_line;		/* unknown fast sw - points at line */

/*
 * This is a damned ugly kludge.  This variable is supposed to only be used
 * in the AIP fastswitching code.  It's a global because it is referenced
 * in the IP_FAST_STARTUP2 macro and otherwise would not exist.	 I'm exposing
 * this kludge for what it is,	but not fixing it at this time (before it was
 * hidden in obscurity as temp1).
 */
static ulong atm_vc_enclen;
#define	ATM_OFF(a)	(btow(atm_vc_enclen - ATM_ENCAP_HDRBYTES))


/*
 * ip_mci2mci
 * Fast switch IP packets between two different MCI controllers.
 * Called from ip_fastswitch().
 */

static void ip_mci2mci (hwidbtype *input, ipcache_rntype *cptr)
{
    hwidbtype *output;
    mciregtype *srcreg, *dstreg;
    ushort bytes, copy_bytes;
    ulong input_interface, output_interface;

    /*
     * Set up some common variables
     */
    output = cptr->idb->hwptr;
    srcreg = input->mci_regptr;
    dstreg = output->mci_regptr;
    output_interface = output->status;
    bytes = input->rxlength;

    /*
     * Reserve a buffer in window 1 of the destination MCI
     */
    if (reserve_space_tx (input, output, dstreg, bytes)) {

	/*
	 * Set up write pointer to base of destination transmit window.
	 * Do some input accounting.  Figure out how to write new MAC header.
	 * Then adjust the read pointer of the receiver window to point after
	 * old MAC header.
	 */
	input_interface = input->status;
	dstreg->argreg = 0;
	dstreg->cmdreg = MCI_CMD_TX1_SELECT;
	copy_bytes = bytes;
	if (output_interface & IDB_ETHER) {
	    /*
	     * Output is Ethernet.
	     */
	    if (input_interface & IDB_ETHER) {
		/*
		 * Output ethernet -- input ethernet
		 */
		srcreg->argreg = MCI_ETHER_OFFSET +
		    btow(ETHER_ARPA_ENCAPBYTES - 2);
		srcreg->cmdreg = MCI_CMD_RX_SELECT;
		ip_write_ether_header(dstreg, cptr);
		dstreg->argreg = bytes;
		copy_bytes -= (ETHER_ARPA_ENCAPBYTES - 2);
	    } else if (input_interface & IDB_FDDI) {
		/*
		 * Output ethernet -- input fddi
		 */
		srcreg->argreg = (FDDI_RECEIVE_OFFSET +
				  (btow(FDDI_SNAP_ENCAPBYTES - 2) +
				   input->rxrifwordlen));
		srcreg->cmdreg = MCI_CMD_RX_SELECT;
		bytes -= ((FDDI_SNAP_ENCAPBYTES +
			   wtob(input->rxrifwordlen) - 2) -
			  (ETHER_ARPA_ENCAPBYTES - 2));
		if (bytes < MINETHERSIZE)
		    bytes = MINETHERSIZE;
		ip_write_ether_header(dstreg, cptr);
		dstreg->argreg = bytes;
		copy_bytes -= (FDDI_SNAP_ENCAPBYTES +
			       wtob(input->rxrifwordlen) - 2);
	    } else if (input_interface & IDB_TR) {
		/*
		 * Output ethernet -- input token ring
		 */
		srcreg->argreg = (CTR_RECEIVE_OFFSET +
				  btow(TRING_SNAP_ENCAPBYTES - 2) +
				  input->rxrifwordlen);
		srcreg->cmdreg = MCI_CMD_RX_SELECT;
		bytes -= ((TRING_SNAP_ENCAPBYTES + wtob(input->rxrifwordlen) - 2) -
			  (ETHER_ARPA_ENCAPBYTES - 2));
		if (bytes < MINETHERSIZE)
		    bytes = MINETHERSIZE;
		ip_write_ether_header(dstreg, cptr);
		dstreg->argreg = bytes;
		copy_bytes -= (TRING_SNAP_ENCAPBYTES +
			       wtob(input->rxrifwordlen) - 2);
	    } else if (input_interface & IDB_SERIAL) {
		/*
		 * Output ethernet -- input serial
		 */
		if (input->enctype == ET_HDLC) {
		    bytes += ((ETHER_ARPA_ENCAPBYTES - 2) -
			      (HDLC_ENCAPBYTES - 2));
		    srcreg->argreg = MCI_SERIAL_OFFSET + 1;
		    srcreg->cmdreg = MCI_CMD_RX_SELECT;
		    ip_write_ether_header(dstreg, cptr);
		    copy_bytes -= (HDLC_ENCAPBYTES - 2);
		    dstreg->argreg = bytes;
		} else if (is_frame_relay(input) || is_ppp(input)) {
		    bytes += ((ETHER_ARPA_ENCAPBYTES - 2) -
			      (HDLC_ENCAPBYTES - 2));
		    srcreg->argreg = MCI_OFFSETSIZE;
		    srcreg->cmdreg = MCI_CMD_RX_SELECT;
		    ip_write_ether_header(dstreg, cptr);
		    dstreg->write1short = TYPE_IP10MB;
		    copy_bytes -= HDLC_ENCAPBYTES;
		    if (srcreg->rxtype == RXTYPE_UNKNOWN) {
			write_new_iphdr(dstreg);
			srcreg->argreg = MCI_OFFSETSIZE + 10;
			srcreg->cmdreg = MCI_CMD_RX_SELECT;
			copy_bytes -= 20;
		    }
		    dstreg->argreg = bytes;
		} else if (is_atm_dxi(input)) {
		    bytes -= input->atm_dxi_stuff->fast_input_encsize;
		    copy_bytes -= input->atm_dxi_stuff->fast_input_encsize;
		    bytes += ETHER_ARPA_ENCAPBYTES;
		    ip_write_ether_header(dstreg, cptr);
		    dstreg->write1short = TYPE_IP10MB;
		    if (input->rxtype == RXTYPE_IPUNC) {
			write_new_iphdr(dstreg);
			srcreg->argreg = atm_dxi_the_line + 10;
			srcreg->cmdreg = MCI_CMD_RX_SELECT;
			copy_bytes -= 20;
		    } else {
			srcreg->argreg = atm_dxi_the_line;
			srcreg->cmdreg = MCI_CMD_RX_SELECT;
		    }
		    dstreg->argreg = bytes;
		} else {	/* TYPE == ET_SMDS */
		    bytes = smds_infolen;
		    bytes += ETHER_ARPA_ENCAPBYTES;
		    srcreg->argreg = MCI_OFFSETSIZE - 1;
		    srcreg->cmdreg = MCI_CMD_RX_SELECT;
		    ip_write_ether_header(dstreg, cptr);
		    copy_bytes -= (SMDS_SNAP_ENCAPBYTES + 2);
		    if (srcreg->rxtype == RXTYPE_UNKNOWN) {
			dstreg->write1short = srcreg->readshort;
			write_new_iphdr(dstreg);
			srcreg->argreg = MCI_OFFSETSIZE + 10;
			srcreg->cmdreg = MCI_CMD_RX_SELECT;
			copy_bytes -= 20;
		    }
		    dstreg->argreg = bytes;
		}
		if (bytes < MINETHERSIZE)
		    dstreg->argreg = MINETHERSIZE;
	    } else if (input_interface & IDB_LEX) {
		/*
		 * Input Lex -- output ethernet.
		 */
		ip_write_ether_header(dstreg, cptr);
		/*
		 * Write the type.
		 */
		dstreg->write1short = TYPE_IP10MB;
		/*
		 * Write the new IP header into the output buffer.
		 */
		write_new_iphdr(dstreg);
		/*
		 * Calculate the number of bytes in the ethernet frame.
		 * If less than 64 pad here.
		 */
		bytes -= (LEX_ARPA_ENCAPBYTES - ETHER_ARPA_ENCAPBYTES);
		if (bytes < MINETHERSIZE)
		    bytes = MINETHERSIZE;
		dstreg->argreg = bytes;
		/*
		 * Move the read pointer to point at the first byte of
		 * IP data (just passed the IP header).
		 */
		srcreg->argreg = (LEX_IP_OFFSET + btow(LEX_IP_HEADER_LEN));
		srcreg->cmdreg = MCI_CMD_RX_SELECT;
		copy_bytes -= (LEX_ARPA_ENCAPBYTES + LEX_IP_HEADER_LEN);
	    }

	} else if (output_interface & IDB_FDDI) {
	    /*
	     * Output is fddi.
	     */
	    if (input_interface & IDB_ETHER) {
		/*
		 * Input ethernet -- output fddi, adjust receive offset (point
		 * at 0800), number of bytes to copy, and size
		 * of output datagram.
		 */
		srcreg->argreg = MCI_ETHER_OFFSET +
		    btow(ETHER_ARPA_ENCAPBYTES - 2);
		srcreg->cmdreg = MCI_CMD_RX_SELECT;
		ip_write_fddi_header(dstreg, cptr);
		bytes += (cptr->length -
  			  (ETHER_ARPA_ENCAPBYTES - 2));
		dstreg->argreg = bytes | MCI_TX_ODDALIGN;
		copy_bytes -= (ETHER_ARPA_ENCAPBYTES - 2);
	    } else if (input_interface & IDB_FDDI) {
		/*
		 * Input fddi -- output fddi
		 */
		srcreg->argreg = (FDDI_RECEIVE_OFFSET +
 				  btow(FDDI_SNAP_ENCAPBYTES - 2) +
 				   input->rxrifwordlen);
		srcreg->cmdreg = MCI_CMD_RX_SELECT;
		ip_write_fddi_header(dstreg, cptr);
 		bytes -= ((FDDI_SNAP_ENCAPBYTES + wtob(input->rxrifwordlen) - 2) -
 			  (2 + cptr->length - 2));
  		bytes |= MCI_TX_ODDALIGN;
		dstreg->argreg = bytes;
 		copy_bytes -= (FDDI_SNAP_ENCAPBYTES +
 			       wtob(input->rxrifwordlen) -
 			       2);
	    } else if (input_interface & IDB_TR) {
		/*
		 * Input token ring -- output fddi, adjust receive offset
		 * (point at 0800) and the number of bytes to copy (temp).
		 */
		srcreg->argreg = (CTR_RECEIVE_OFFSET +
				  btow(TRING_SNAP_ENCAPBYTES - 2) +
				  input->rxrifwordlen);
		srcreg->cmdreg = MCI_CMD_RX_SELECT;
		ip_write_fddi_header(dstreg, cptr);
		bytes -= ((TRING_SNAP_ENCAPBYTES + wtob(input->rxrifwordlen) - 2) -
 			  (2 + cptr->length - 2));
		dstreg->argreg = bytes | MCI_TX_ODDALIGN;
		copy_bytes -= (TRING_SNAP_ENCAPBYTES +
			       wtob(input->rxrifwordlen) - 2);
	    } else if (input_interface & IDB_SERIAL) {
		/*
		 * Input serial -- output fddi
		 */
		if (input->enctype == ET_HDLC) {
		    srcreg->argreg = MCI_SERIAL_OFFSET +
			btow(HDLC_ENCAPBYTES - 2);
		    srcreg->cmdreg = MCI_CMD_RX_SELECT;
		    copy_bytes -= (HDLC_ENCAPBYTES - 2);
		    ip_write_fddi_header(dstreg, cptr);
		    bytes += (cptr->length -
  			      (HDLC_ENCAPBYTES - 2));
		    dstreg->argreg = bytes | MCI_TX_ODDALIGN;
		} else if (is_frame_relay(input) || is_ppp(input)) {
		    srcreg->argreg = MCI_OFFSETSIZE;
		    srcreg->cmdreg = MCI_CMD_RX_SELECT;
		    ip_write_fddi_header(dstreg, cptr);
		    bytes += (cptr->length -
  			      (HDLC_ENCAPBYTES - 2));
		    dstreg->write1short = TYPE_IP10MB;
		    copy_bytes -= HDLC_ENCAPBYTES;;
		    if (srcreg->rxtype == RXTYPE_UNKNOWN) {
			write_new_iphdr(dstreg);
			srcreg->argreg = MCI_OFFSETSIZE + 10;
			srcreg->cmdreg = MCI_CMD_RX_SELECT;
			copy_bytes -= 20;
		    }
		    dstreg->argreg = bytes | MCI_TX_ODDALIGN;
		} else if (is_atm_dxi(input)) {
		    bytes -= input->atm_dxi_stuff->fast_input_encsize;
		    copy_bytes -= input->atm_dxi_stuff->fast_input_encsize;
		    bytes += cptr->length;
		    ip_write_fddi_header(dstreg, cptr);
		    dstreg->write1short = TYPE_IP10MB;
		    if (input->rxtype == RXTYPE_IPUNC) {
			write_new_iphdr(dstreg);
			srcreg->argreg = atm_dxi_the_line + 10;
			srcreg->cmdreg = MCI_CMD_RX_SELECT;
			copy_bytes -= 20;
		    } else {
			srcreg->argreg = atm_dxi_the_line;
			srcreg->argreg = MCI_CMD_RX_SELECT;
		    }
		    dstreg->argreg = bytes | MCI_TX_ODDALIGN;
		} else {	/* TYPE == ET_SMDS */
		    bytes = smds_infolen;
		    bytes += cptr->length + 2;
		    srcreg->argreg = MCI_OFFSETSIZE - 1;
		    srcreg->cmdreg = MCI_CMD_RX_SELECT;
		    ip_write_fddi_header(dstreg, cptr);
		    copy_bytes -= (SMDS_SNAP_ENCAPBYTES + 2);
		    if (srcreg->rxtype == RXTYPE_UNKNOWN) {
			dstreg->write1short = srcreg->readshort;
			write_new_iphdr(dstreg);
			srcreg->argreg = MCI_OFFSETSIZE + 10;
			srcreg->cmdreg = MCI_CMD_RX_SELECT;
			copy_bytes -= 20;
		    }
		    dstreg->argreg = bytes | MCI_TX_ODDALIGN;
		}
	    } else if (input_interface & IDB_LEX) {
		/*
		 * Input Lex -- output fddi.
		 */
		ip_write_fddi_header(dstreg, cptr);
		/*
		 * Write the type.
		 */
		dstreg->write1short = TYPE_IP10MB;
		/*
		 * Write the new IP header into the output buffer.
		 */
		write_new_iphdr(dstreg);
		/*
		 * Calculate the number of bytes in the ethernet frame.
		 * If less than 64 pad here.  Would the ethernet controler
		 * add the padding?
		 */
		bytes -= (LEX_ARPA_ENCAPBYTES - FDDI_SNAP_ENCAPBYTES);
		dstreg->argreg = bytes | MCI_TX_ODDALIGN;
		/*
		 * Move the read pointer to point at the first byte of
		 * IP data (just passed the IP header).
		 */
		srcreg->argreg = (LEX_IP_OFFSET + btow(LEX_IP_HEADER_LEN));
		srcreg->cmdreg = MCI_CMD_RX_SELECT;
		copy_bytes -= (LEX_ARPA_ENCAPBYTES + LEX_IP_HEADER_LEN);
	    }
	} else if (output_interface & IDB_TR) {
	    /*
	     * Output is token ring.
	     */
	    if (input_interface & IDB_ETHER) {
		/*
		 * Input ethernet -- output token ring, adjust receive
		 * offset (point at 0800), number of bytes to copy,
		 * and size (bytes) of output datagram.
		 */
		srcreg->argreg = MCI_ETHER_OFFSET +
		    btow(ETHER_ARPA_ENCAPBYTES - 2);
		srcreg->cmdreg = MCI_CMD_RX_SELECT;
		ip_write_token_header(dstreg, cptr);
		bytes += (cptr->length - (ETHER_ARPA_ENCAPBYTES - 2));
		dstreg->argreg = bytes;
		copy_bytes -= (ETHER_ARPA_ENCAPBYTES - 2);
	    } else if (input_interface & IDB_FDDI) {
		/*
		 * Input fddi -- output token ring
		 */
		srcreg->argreg = FDDI_RECEIVE_OFFSET +
		    (btow(FDDI_SNAP_ENCAPBYTES - 2) +
		     input->rxrifwordlen);
		srcreg->cmdreg = MCI_CMD_RX_SELECT;
		ip_write_token_header(dstreg, cptr);
		bytes -= ((FDDI_SNAP_ENCAPBYTES + wtob(input->rxrifwordlen) - 2) -
			  (2 + cptr->length - 2));
		dstreg->argreg = bytes;
		copy_bytes -= (FDDI_SNAP_ENCAPBYTES - 2);
	    } else if (input_interface & IDB_TR) {
		/*
		 * Input token ring -- output token ring
		 */
		srcreg->argreg = (CTR_RECEIVE_OFFSET +
				  btow(TRING_SNAP_ENCAPBYTES - 2) +
				  input->rxrifwordlen);
		srcreg->cmdreg = MCI_CMD_RX_SELECT;
		ip_write_token_header(dstreg, cptr);
		bytes -= ((TRING_SNAP_ENCAPBYTES + wtob(input->rxrifwordlen) - 2) -
			  (2 + cptr->length - 2));
		dstreg->argreg = bytes;
		copy_bytes -= (TRING_SNAP_ENCAPBYTES + wtob(input->rxrifwordlen) -
			       2);
	    } else if (input_interface & IDB_SERIAL) {
		/*
		 * Input serial -- output token ring
		 */
		if (input->enctype == ET_HDLC ) {
		    srcreg->argreg = MCI_SERIAL_OFFSET +
			btow(HDLC_ENCAPBYTES - 2);
		    srcreg->cmdreg = MCI_CMD_RX_SELECT;
		    ip_write_token_header(dstreg, cptr);
		    bytes += (cptr->length - (HDLC_ENCAPBYTES - 2));
		    dstreg->argreg = bytes;
		    copy_bytes -= (HDLC_ENCAPBYTES - 2);
		} else if (is_frame_relay(input) || is_ppp(input)) {
		    srcreg->argreg = MCI_OFFSETSIZE;
		    srcreg->cmdreg = MCI_CMD_RX_SELECT;
		    ip_write_token_header(dstreg, cptr);
		    bytes += (cptr->length - (HDLC_ENCAPBYTES - 2));
		    dstreg->write1short = TYPE_IP10MB;
		    copy_bytes -= HDLC_ENCAPBYTES;
		    if (srcreg->rxtype == RXTYPE_UNKNOWN) {
			write_new_iphdr(dstreg);
			srcreg->argreg = MCI_OFFSETSIZE + 10;
			srcreg->cmdreg = MCI_CMD_RX_SELECT;
			copy_bytes -= 20;
		    }
		    dstreg->argreg = bytes;
		} else if (is_atm_dxi(input)) {
		    bytes -= input->atm_dxi_stuff->fast_input_encsize;
		    copy_bytes -= input->atm_dxi_stuff->fast_input_encsize;
		    bytes += cptr->length;
		    ip_write_token_header(dstreg, cptr);
		    dstreg->write1short = TYPE_IP10MB;
		    if (input->rxtype == RXTYPE_IPUNC) {
			write_new_iphdr(dstreg);
			srcreg->argreg = atm_dxi_the_line + 10;
			srcreg->cmdreg = MCI_CMD_RX_SELECT;
			copy_bytes -= 20;
		    } else {
			srcreg->argreg = atm_dxi_the_line;
			srcreg->cmdreg = MCI_CMD_RX_SELECT;
		    }
		    dstreg->argreg = bytes;
		} else {	/* TYPE == ET_SMDS */
		    bytes = smds_infolen;
		    bytes += cptr->length + 2;
		    srcreg->argreg = MCI_OFFSETSIZE - 1;
		    srcreg->cmdreg = MCI_CMD_RX_SELECT;
		    ip_write_token_header(dstreg, cptr);
		    copy_bytes -= (SMDS_SNAP_ENCAPBYTES + 2);
		    if (srcreg->rxtype == RXTYPE_UNKNOWN) {
			dstreg->write1short = srcreg->readshort;
			write_new_iphdr(dstreg);
			srcreg->argreg = MCI_OFFSETSIZE + 10;
			srcreg->cmdreg = MCI_CMD_RX_SELECT;
			copy_bytes -= 20;
		    }
		    dstreg->argreg = bytes;
		}
	    } else if (input_interface & IDB_LEX) {
		/*
		 * Input Lex -- output token ring.
		 */
		ip_write_token_header(dstreg, cptr);
		/*
		 * Write the type.
		 */
		dstreg->write1short = TYPE_IP10MB;
		/*
		 * Write the new IP header into the output buffer.
		 */
		write_new_iphdr(dstreg);
		/*
		 * Calculate the number of bytes in the token ring frame.
		 */
		bytes -= (LEX_ARPA_ENCAPBYTES - TRING_SNAP_ENCAPBYTES);
		dstreg->argreg = bytes;
		/*
		 * Move the read pointer to point at the first byte of
		 * IP data (just passed the IP header).
		 */
		srcreg->argreg = (LEX_IP_OFFSET + btow(LEX_IP_HEADER_LEN));
		srcreg->cmdreg = MCI_CMD_RX_SELECT;
		copy_bytes -= (LEX_ARPA_ENCAPBYTES + LEX_IP_HEADER_LEN);
	    }
	} else if (output_interface & IDB_SERIAL) {
	    /*
	     * Output is SERIAL (HDLC || SMDS).
	     */
	    if (!(is_smds(output))) {
		if (input_interface & IDB_ETHER) {
		    /*
		     * Input ethernet -- output serial. Write serial
		     * encapsulation
		     * word from cache, select receive offset, adjust number of
		     * bytes to copy (copy_bytes) and size (bytes) of
		     * output datgram.
		     */
		    ip_write_serial_header(dstreg, cptr);
		    srcreg->argreg = MCI_ETHER_OFFSET +
			btow(ETHER_ARPA_ENCAPBYTES);
		    srcreg->cmdreg = MCI_CMD_RX_SELECT;
		    copy_bytes -= ETHER_ARPA_ENCAPBYTES;
		    bytes -= (ETHER_ARPA_ENCAPBYTES - cptr->length);
		    dstreg->argreg = bytes;
		} else if (input_interface & IDB_FDDI) {
		    /*
		     * Input fddi -- output serial.
		     */
		    ip_write_serial_header(dstreg, cptr);
		    srcreg->argreg = (FDDI_RECEIVE_OFFSET +
				      btow(FDDI_SNAP_ENCAPBYTES) +
				      input->rxrifwordlen);
		    srcreg->cmdreg = MCI_CMD_RX_SELECT;
		    copy_bytes -= (FDDI_SNAP_ENCAPBYTES +
				   wtob(input->rxrifwordlen));
		    bytes -= (FDDI_SNAP_ENCAPBYTES + wtob(input->rxrifwordlen));
		    bytes += cptr->length;
		    dstreg->argreg = bytes;
		} else if (input_interface & IDB_TR) {
		    /*
		     * Input token ring -- output serial.
		     */
		    ip_write_serial_header(dstreg,cptr);
		    srcreg->argreg = (CTR_RECEIVE_OFFSET +
				      btow(TRING_SNAP_ENCAPBYTES) +
				      input->rxrifwordlen);
		    srcreg->cmdreg = MCI_CMD_RX_SELECT;
		    copy_bytes -= (TRING_SNAP_ENCAPBYTES +
				   wtob(input->rxrifwordlen));
		    bytes -= (TRING_SNAP_ENCAPBYTES + wtob(input->rxrifwordlen));
		    bytes += cptr->length;
		    dstreg->argreg = bytes;
		} else if (input_interface & IDB_SERIAL) {
		    /*
		     * Input serial -- output serial, no adjustment needed.
		     */
		    if (input->enctype == ET_HDLC) {
			ip_write_serial_header(dstreg,cptr);
			srcreg->argreg = (MCI_SERIAL_OFFSET +
					  btow(HDLC_ENCAPBYTES));
			srcreg->cmdreg = MCI_CMD_RX_SELECT;
			bytes += cptr->length - HDLC_ENCAPBYTES;
			copy_bytes -= HDLC_ENCAPBYTES;
			dstreg->argreg = bytes;
		    } else if (is_frame_relay(input) || is_ppp(input)) {
			ip_write_serial_header(dstreg, cptr);
			srcreg->argreg = (MCI_SERIAL_OFFSET +
					  btow(HDLC_ENCAPBYTES));
			srcreg->cmdreg = MCI_CMD_RX_SELECT;
			bytes += cptr->length - HDLC_ENCAPBYTES;
			copy_bytes -= HDLC_ENCAPBYTES;
			if (srcreg->rxtype == RXTYPE_UNKNOWN) {
			    write_new_iphdr(dstreg);
			    srcreg->argreg = MCI_OFFSETSIZE + 10;
			    srcreg->cmdreg = MCI_CMD_RX_SELECT;
			    copy_bytes -= 20;
			}
			dstreg->argreg = bytes;
		    } else if (is_atm_dxi(input)) {

			/*
			 * bogon alert:
			 *
			 * the rx_select command is issued in only the
			 * "ipunc" case. This is probably incorrect,
			 * but need verification...
			 */
			bytes -= input->atm_dxi_stuff->fast_input_encsize;
			copy_bytes -= input->atm_dxi_stuff->fast_input_encsize;
			bytes += cptr->length;
			ip_write_serial_header(dstreg, cptr);
			if (input->rxtype == RXTYPE_IPUNC) {
			    write_new_iphdr(dstreg);
			    srcreg->argreg = atm_dxi_the_line + 10;
			    srcreg->cmdreg = MCI_CMD_RX_SELECT;
			    copy_bytes -= 20;
			}
			dstreg->argreg = bytes;
		    } else {	/* TYPE == ET_SMDS */
			ip_write_serial_header(dstreg, cptr);
			bytes = smds_infolen;
			bytes += cptr->length;
			srcreg->argreg = MCI_OFFSETSIZE;
			srcreg->cmdreg = MCI_CMD_RX_SELECT;
			if (srcreg->rxtype == RXTYPE_UNKNOWN) {
			    write_new_iphdr(dstreg);
			    srcreg->argreg = MCI_OFFSETSIZE + 10;
			    srcreg->cmdreg = MCI_CMD_RX_SELECT;
			    copy_bytes -= 20;
			}
			copy_bytes -= (SMDS_SNAP_ENCAPBYTES + 4);
			dstreg->argreg = bytes;
		    }
		} else if (input_interface & IDB_LEX) {
		    /*
		     * Input Lex -- output serial. Write serial encapsulation
		     * word from cache, select receive offset, adjust number of
		     * bytes to copy (temp) and size (bytes) of output datgram.
		     */
		    ip_write_serial_header(dstreg, cptr);
		    /*
		     * Write the new IP header into the output buffer.
		     */
		    write_new_iphdr(dstreg);

		    bytes += cptr->length;
		    bytes -= LEX_ARPA_ENCAPBYTES;
		    dstreg->argreg = bytes;

		    /*
		     * Move the read pointer to point at the first byte of
		     * IP data (just passed the IP header).
		     */
		    srcreg->argreg = LEX_IP_OFFSET + btow(LEX_IP_HEADER_LEN);
		    srcreg->cmdreg = MCI_CMD_RX_SELECT;
		    copy_bytes -= (LEX_ARPA_ENCAPBYTES + LEX_IP_HEADER_LEN);
		}
		if (is_frame_relay(output)) {
		    INC_PVC_FAST_OUTPUT(HWIDB_TO_FR_PVC(output,
						cptr->subint_reference_num),
					bytes);
		} else if (is_atm_dxi(output)) {
		    INC_DXI_PVC_FAST_OUTPUT(HWIDB_TO_DXI_PVC(output,
						cptr->subint_reference_num),
					    bytes);
		}
	    } else {		/* output Serial is ET_SMDS */
		ushort len;

		spd = output->smds_pdb;
		if (spd->dximode) {
		    sm_modelen = sizeof(dxitype);
		} else {
		    sm_modelen = sizeof(ushort);
		}

		if (input_interface & IDB_ETHER) {
		    len = copy_bytes -= ETHER_ARPA_ENCAPBYTES;
		    len += cptr->length;
		} else if (input_interface & IDB_FDDI) {
		    len = copy_bytes -= (FDDI_SNAP_ENCAPBYTES +
					 wtob(input->rxrifwordlen));
		    len += cptr->length;
		} else if (input_interface & IDB_TR) {
		    len = copy_bytes -= (TRING_SNAP_ENCAPBYTES +
					 wtob(input->rxrifwordlen));
		    len += cptr->length;
		} else if (input_interface & IDB_LEX) {
		    len = copy_bytes -= LEX_ARPA_ENCAPBYTES;
		    len += cptr->length;
		} else {	/* input_interface & IDB_SERIAL */
		    if (input->enctype == ET_SMDS) {
			len = copy_bytes = smds_infolen;
		    } else if (is_atm_dxi(input)) {
			len = copy_bytes -=
			    input->atm_dxi_stuff->fast_input_encsize;
		    } else {	/* Else ET_HDLC || ET_FRAME_RELAY  || ET_PPP */
			len = copy_bytes -= input->encsize;
		    }
		    len += cptr->length;
		}
		/*
		 * SMDS pad here
		 */
		sm_pad_offset = ((sm->smds_basize + 4 + sm_modelen) >> 1) -
		    2;
		if (!sm_pad)
		    sm_pad_offset += 2;
		ip_write_smds_trailer(dstreg, (ushort *)&sm->smds_rsvd,
				      sm_pad, sm_lastlong, sm_pad_offset);
		/*
		 * SMDS header here
		 */
		sm_head_offset = 0;
		ip_write_smds_header(dstreg, cptr, sm_head_offset);
		if (input_interface & IDB_TR)
		    srcreg->argreg = (CTR_RECEIVE_OFFSET +
				      btow(TRING_SNAP_ENCAPBYTES) +
				      input->rxrifwordlen);
		else if (input_interface & IDB_FDDI)
		    srcreg->argreg = (FDDI_RECEIVE_OFFSET +
				      btow(FDDI_SNAP_ENCAPBYTES) +
				      input->rxrifwordlen);
		else if (input_interface & IDB_LEX)
		    srcreg->argreg = LEX_IP_OFFSET;
		else
		    srcreg->argreg = MCI_OFFSETSIZE;
		srcreg->cmdreg = MCI_CMD_RX_SELECT;
		if (input->enctype == ET_SMDS &&
		    srcreg->rxtype == RXTYPE_UNKNOWN) {
		    write_new_iphdr(dstreg);
		    srcreg->argreg = MCI_OFFSETSIZE + 10;
		    srcreg->cmdreg = MCI_CMD_RX_SELECT;
		    copy_bytes -= 20;
		}
		len += (sm_pad + 4);
		dstreg->argreg = bytes = len;
		copy_bytes &= ~0x03;
	    }
	} else if (output_interface & IDB_LEX) {
	    /*
	     * Output interface Lex.
	     */
	    if (input_interface & IDB_ETHER) {
		/*
		 * Input ethernet -- output Lex.
		 */
		ip_write_lex_header(dstreg, cptr);
		/*
		 * Calculate the size of the new lex frame.
		 */
		bytes -= (ETHER_ARPA_ENCAPBYTES - cptr->length);
		dstreg->argreg = bytes;

		/*
		 * Calculate the offset and amount of data to copy.
		 */
		srcreg->argreg = MCI_ETHER_OFFSET +
		    btow(ETHER_ARPA_ENCAPBYTES);
		srcreg->cmdreg = MCI_CMD_RX_SELECT;
		copy_bytes -= ETHER_ARPA_ENCAPBYTES;

	    } else if (input_interface & IDB_FDDI) {
		/*
		 * Input fddi -- output Lex.
		 */
		ip_write_lex_header(dstreg, cptr);

		/*
		 * Calculate the size of the new lex frame.
		 */
		bytes -= (FDDI_SNAP_ENCAPBYTES +
			  wtob(input->rxrifwordlen)
			  - cptr->length);
		dstreg->argreg = bytes;

		/*
		 * Calculate the offset and amount of data to copy.
		 */
		srcreg->argreg = (FDDI_RECEIVE_OFFSET +
				  input->rxrifwordlen +
				  (btow(FDDI_SNAP_ENCAPBYTES)));
		srcreg->cmdreg = MCI_CMD_RX_SELECT;
		copy_bytes -= (FDDI_SNAP_ENCAPBYTES +
			       wtob(input->rxrifwordlen));

	    } else if (input_interface & IDB_TR) {

		/*
		 * Input token ring -- output Lex.
		 */
		ip_write_lex_header(dstreg, cptr);

		/*
		 * Calculate the size of the new lex frame.
		 */
		bytes -= (TRING_SNAP_ENCAPBYTES + wtob(input->rxrifwordlen)
			  - cptr->length);
		dstreg->argreg = bytes;

		/*
		 * Calculate the offset and amount of data to copy.
		 */
		srcreg->argreg = (CTR_RECEIVE_OFFSET + input->rxrifwordlen +
				  btow(TRING_SNAP_ENCAPBYTES));

		srcreg->cmdreg = MCI_CMD_RX_SELECT;
		copy_bytes -= (TRING_SNAP_ENCAPBYTES +
			       wtob(input->rxrifwordlen));
	    } else if (input_interface & IDB_SERIAL) {
		/*
		 * Output Lex -- input serial, adjust receive offset,
		 * number of bytes to copy (temp) and size (bytes) of output
		 * datagram.
		 */
		if (input->enctype == ET_HDLC) {
		    bytes += (cptr->length - HDLC_ENCAPBYTES);
		    srcreg->argreg = MCI_SERIAL_OFFSET +
			btow(HDLC_ENCAPBYTES);
		    srcreg->cmdreg = MCI_CMD_RX_SELECT;
		    ip_write_lex_header(dstreg, cptr);
		    dstreg->argreg = bytes;
		} else if (is_frame_relay(input) || is_ppp(input)) {
		    bytes += (cptr->length - HDLC_ENCAPBYTES);
		    srcreg->argreg = MCI_OFFSETSIZE;
		    srcreg->cmdreg = MCI_CMD_RX_SELECT;
		    ip_write_lex_header(dstreg, cptr);
		    copy_bytes -= HDLC_ENCAPBYTES;
		    if (srcreg->rxtype == RXTYPE_UNKNOWN) {
			write_new_iphdr(dstreg);
			srcreg->argreg = MCI_OFFSETSIZE +
			    btow(LEX_IP_HEADER_LEN);
			srcreg->cmdreg = MCI_CMD_RX_SELECT;
			copy_bytes -= LEX_IP_HEADER_LEN;
		    }
		    dstreg->argreg = bytes;
		} else if (is_atm_dxi(input)) {
		    bytes -= input->atm_dxi_stuff->fast_input_encsize;
		    copy_bytes -= input->atm_dxi_stuff->fast_input_encsize;
		    bytes += cptr->length;
		    ip_write_lex_header(dstreg, cptr);
		    if (input->rxtype == RXTYPE_IPUNC) {
			write_new_iphdr(dstreg);
			srcreg->argreg = atm_dxi_the_line + 10;
			srcreg->cmdreg = MCI_CMD_RX_SELECT;
			copy_bytes -= 20;
		    }
		    srcreg->argreg = atm_dxi_the_line;
		    srcreg->cmdreg = MCI_CMD_RX_SELECT;
		    dstreg->argreg = bytes;
	       } else {	/* TYPE == ET_SMDS */
		   bytes = smds_infolen;
		   bytes += cptr->length;
		   srcreg->argreg = MCI_OFFSETSIZE;
		   srcreg->cmdreg = MCI_CMD_RX_SELECT;
		   ip_write_lex_header(dstreg, cptr);
		   copy_bytes -= (SMDS_SNAP_ENCAPBYTES + 2);
		   if (srcreg->rxtype == RXTYPE_UNKNOWN) {
		       write_new_iphdr(dstreg);
		       srcreg->argreg = MCI_OFFSETSIZE +
			   btow(LEX_IP_HEADER_LEN);
		       srcreg->cmdreg = MCI_CMD_RX_SELECT;
		       copy_bytes -= LEX_IP_HEADER_LEN;
		   }
		   dstreg->argreg = bytes;
		}
	    } else if (input_interface & IDB_LEX) {
		/*
		 * Input Lex -- output Lex.
		 */
		ip_write_lex_header(dstreg, cptr);
		/*
		 * Write new IP header.
		 */
		write_new_iphdr(dstreg);

		dstreg->argreg = bytes;

		/*
		 * Calculate the offset and amount of data to copy.
		 */
		srcreg->argreg = LEX_IP_OFFSET + btow(LEX_IP_HEADER_LEN);
		srcreg->cmdreg = MCI_CMD_RX_SELECT;
		copy_bytes -= cptr->length + LEX_IP_HEADER_LEN;
	    }
	}

	/*
	 * Copy bulk of datagram from one MCI to another.
	 * Transmit modified copy and flush original in receive buffer.
	 */
	mci2mci(&srcreg->readlong, &dstreg->write1long, copy_bytes);
	dstreg->cmdreg = MCI_CMD_TX1_START;
	output->counters.tx_cumbytes_fast[ACCT_PROTO_IP] += bytes;
    } else if (0 != output->priority_list) {
	ushort outgoing_mac_length = cptr->length;
	paktype *pak;
	input_interface = input->status;
	copy_bytes = bytes;

	/*
	 * Get the input and output mac lengths.
	 */
	if (input_interface & IDB_ETHER) {
	    /*
	     * Output ethernet -- input ethernet
	     */
	    srcreg->argreg = MCI_ETHER_OFFSET +
		btow(ETHER_ARPA_ENCAPBYTES);
	    srcreg->cmdreg = MCI_CMD_RX_SELECT;
	    copy_bytes -= (ETHER_ARPA_ENCAPBYTES);
	} else if (input_interface & IDB_FDDI) {
	    /*
	     * Output ethernet -- input fddi
	     */
	    srcreg->argreg = (FDDI_RECEIVE_OFFSET +
			      (btow(FDDI_SNAP_ENCAPBYTES) +
			       input->rxrifwordlen));
	    srcreg->cmdreg = MCI_CMD_RX_SELECT;
	    copy_bytes -= (FDDI_SNAP_ENCAPBYTES + wtob(input->rxrifwordlen));
	} else if (input_interface & IDB_TR) {
	    /*
	     * Output ethernet -- input token ring
	     */
	    srcreg->argreg = (CTR_RECEIVE_OFFSET +
			      btow(TRING_SNAP_ENCAPBYTES) +
			      input->rxrifwordlen);
	    srcreg->cmdreg = MCI_CMD_RX_SELECT;
	    copy_bytes -= (TRING_SNAP_ENCAPBYTES + wtob(input->rxrifwordlen));
	} else if (input_interface & IDB_SERIAL) {
	    if (is_atm_dxi(input)) {
		copy_bytes -= input->atm_dxi_stuff->fast_input_encsize;
		bytes += ETHER_ARPA_ENCAPBYTES;
		if (input->rxtype == RXTYPE_IPUNC) {
		    srcreg->argreg = atm_dxi_the_line + 10;
		    srcreg->cmdreg = MCI_CMD_RX_SELECT;
		} else {
		    srcreg->argreg = atm_dxi_the_line;
		    srcreg->cmdreg = MCI_CMD_RX_SELECT;
		}
	    } else {
		/* HDLC, PPP, SMDS, or Frame relay */
		srcreg->argreg = MCI_OFFSETSIZE;
		srcreg->cmdreg = MCI_CMD_RX_SELECT;
		if (srcreg->rxtype == RXTYPE_UNKNOWN) {
		    srcreg->argreg = MCI_OFFSETSIZE + 10;
		    srcreg->cmdreg = MCI_CMD_RX_SELECT;
		}
	    }
	} else if (input_interface & IDB_LEX) {
	    /*
	     * Move the read pointer to point at the first byte of
	     * IP data (just passed the IP header).
	     */
	    srcreg->argreg = (LEX_IP_OFFSET + btow(LEX_IP_HEADER_LEN));
	    srcreg->cmdreg = MCI_CMD_RX_SELECT;
	    copy_bytes -= (LEX_ARPA_ENCAPBYTES + LEX_IP_HEADER_LEN);
	}

	/*
	 * move the IP data into a buffer
	 */
	pak = getbuffer(copy_bytes + outgoing_mac_length);
	if (!pak) {
	    output->counters.output_total_drops++;
	    srcreg->cmdreg = MCI_CMD_RX_FLUSH;
	    return;
	}

	pak->if_output = cptr->idb;
	ip_traffic.forwarded++;

	pak->linktype = LINK_IP;
	pak->network_start = pak->data_area + ENCAPBYTES;
        if (srcreg->rxtype == RXTYPE_UNKNOWN) {			
	    mci2buffer(&srcreg->readlong,			
		pak->network_start + sizeof(iphdrtype), copy_bytes);	
	    bcopy(&temp_iphdr, pak->network_start, sizeof(iphdrtype));	
	} else {						
	    mci2buffer(&srcreg->readlong, pak->network_start, copy_bytes); 
	}							
	srcreg->cmdreg = MCI_CMD_RX_FLUSH;
	pak->datagramsize = GETSHORT(pak->network_start + 2);	/* size taken from message */
	input->counters.inputs++;
	/*
	 * add outbound data link header
	 */
	ip_hes_mac_rewrite(pak, output, cptr);

	/*
	 * save in a queue for later transmission
	 */
	holdq_enqueue_fast(output, pak);
	/* the tx-reserve failed, so we should get an interrupt here */
	return;
    } else {
	output->counters.output_total_drops++;
    }
    /*
     * Flush the datagram.  We come here when we can't do the TX1_RESERVE
     * OR [fall through] after we have successfully copied the datagram
     * over the multibus and issued the start.
     */
    srcreg->cmdreg = MCI_CMD_RX_FLUSH;
}

/*
 * mci_ip_accounting
 * This function is a rip-off of the *accumulate_ip_accounting function --
 * it is MCI specific and does not attempt to malloc additional memory.
 * If additional memory is needed, we will not fast switch the datagram.
 */

static boolean mci_ip_accounting (hwidbtype *input)
{
    mciregtype *srcptr;
    ulong srcadr, dstadr;
    ushort ipbytes, offset;
    shortlong hash;
    ip_acctg_entry_type *ipae;
    ip_acctg_data_type *ipad;
    int indx;
    ulong mask;
    ip_acctg_acc_entry_type *acc_entry;

    /*
     * Read in the IP length and source and destination addresses.
     */
    srcptr = input->mci_regptr;
    if (input->status & (IDB_TR | IDB_FDDI))
	offset = MCI_OFFSETSIZE + ISO2_OVERHEAD + MCI_IP_LENGTH_OFFSET +
	    input->rxrifwordlen;
    else
	if (is_atm(input)) {
            {
                vc_info_t *vc = NULL;
                atm_db_t *aip;
		
                aip = input->atm_db;
                if (aip == NULL)
                    return(FALSE);
                vc = aip->fast_vc_info;
                if (vc == NULL)
                    return(FALSE);
                atm_vc_enclen = vc->encapsize;
            }
            offset = MCI_OFFSETSIZE + MCI_IP_LENGTH_OFFSET +
                ATM_OFF(input);
	} else 
	    offset = MCI_OFFSETSIZE + MCI_IP_LENGTH_OFFSET;
    srcptr->argreg = offset;
    srcptr->cmdreg = MCI_CMD_RX_SELECT;
    ipbytes = srcptr->readshort;
    srcptr->argreg = offset + (MCI_IP_SRCADR_OFFSET - MCI_IP_LENGTH_OFFSET);
    srcptr->cmdreg = MCI_CMD_RX_SELECT;
    srcadr = srcptr->readlong;
    dstadr = srcptr->readlong;

    /*
     * See if filters are present on the accounting capture.
     */
    if (ip_acctg_acc != NULL) {
	for (indx = 0, acc_entry = &ip_acctg_acc->list[0];
	     indx < ip_acctg_acc->count; indx++, acc_entry++) {
	    mask = (ulong)acc_entry->mask;
	    if (((srcadr & mask) == acc_entry->addr) ||
		((dstadr & mask) == acc_entry->addr)) {
		goto hit;
	    }
	}
	if (ip_acctg_acc->count == 0)
	    goto hit;
	if (ip_acctg_acc->transits == 0)
	    return(TRUE);
	ip_acctg_acc->transits--;
    }
hit:

    /*
     * Compute the hash index into the accounting hash.
     */
    hash.d.lword = srcadr;
    hash.d.lword ^= dstadr;
    hash.d.sword[1] ^= (hash.d.sword[0]);
    hash.d.sword[1] ^= (hash.d.sword[1] >> 8);
    for (ipae = ip_acctg_info.current->store[hash.d.lword &= 0xFF];
	 ipae; ipae = ipae->next) {
	if ((ipae->srcadr == srcadr) && (ipae->dstadr == dstadr)) {
	    ipae->packets++;
	    ipae->bytes += ipbytes;
	    return(TRUE);
	}
    }

    if (ip_acctg_info.count < ip_acctg_info.threshold) {
	/*
	 * Entry not found, see if another entry exists on the queue.
	 * If it does, dequeue it and add the entry to the hash.
	 */
	ipae = dequeue(&ipacctQ);
	if (ipae) {
	    ipae->srcadr = srcadr;
	    ipae->dstadr = dstadr;
	    ipae->bytes	 = ipbytes;
	    ipae->packets = 1;
	    ipad = ip_acctg_info.current;
	    ipae->next = ipad->store[hash.d.lword];
	    ipad->store[hash.d.lword] = ipae;
	    ip_acctg_info.count++;
	    return(TRUE);
	}
	/*
	 * We need to get more memory -- do it at process level.
	 */
	return(FALSE);
    }
    /*
     * If reached our threshold level, bump counters and return TRUE.
     */
    ipad = ip_acctg_info.current;
    ipad->packets_tossed++;
    ipad->bytes_tossed += ipbytes;
    return(TRUE);
}

/*
 * mci_ip_fastcheck
 * This MACRO is a rip-off of the *fast_check* function -- it is MCI
 * specific, but the checks are identical to *fast_check*.
 *
 * Mirror images of this function exist, if changes are made to this
 * function the corresponding changes must be made to *fast_check* or
 * *ip_accesscheck_inline*.
 */

#define MCI_IP_FASTCHECK(EXTRA_OFFSET, LABEL_ID, DENY_LABEL)		\
									\
    shortlong srcadr;							\
    fasthash *acc;							\
    fastitem *fitem;							\
    ulong mask;								\
    ushort listhash;							\
    fasttype *list;							\
									\
    list = (fasttype *) acl->list;					\
    if (!list) 								\
	goto fastcheck_ok_ ## LABEL_ID;					\
									\
    /*									\
     * Read in the IP source address.					\
     */									\
    srcptr->argreg = MCI_OFFSETSIZE + EXTRA_OFFSET +			\
		     MCI_IP_SRCADR_OFFSET;				\
    srcptr->cmdreg = MCI_CMD_RX_SELECT;					\
    srcadr.d.lword = srcptr->readlong;					\
									\
    /*									\
     * Check for individual entries with no mask.			\
     */									\
    listhash = srcadr.d.sword[0];					\
    listhash ^= srcadr.d.sword[1];					\
    listhash ^= (listhash >> 8);					\
    acc = list->hashlist[listhash & 0xFF];				\
    while (acc) {							\
	if (srcadr.d.lword == acc->address) {				\
	    if (acc->grant)						\
		goto fastcheck_ok_ ## LABEL_ID;				\
	    else							\
		goto DENY_LABEL;					\
	}								\
	acc = acc->next;						\
    }									\
    /*									\
     * Check out entries with wildcard masks.				\
     */									\
    fitem = list->items;						\
    while (fitem) {							\
	mask = ~fitem->wildmask;					\
	if ((srcadr.d.lword & mask) == fitem->permitmask) {		\
	    if (fitem->grant)						\
		goto fastcheck_ok_ ## LABEL_ID;				\
	    else							\
		goto DENY_LABEL;					\
	}								\
	fitem = fitem->next;						\
    }									\
    goto DENY_LABEL;							\
  fastcheck_ok_ ## LABEL_ID:


/*
 * mci_ip_slowcheck_inline
 * This function is a rip-off of the *ip_accesscheck_inline* function
 * -- it is MCI specific, but the checks are identical to
 * *ip_accesscheck_inline*.  It's really too bad that the packet isn't
 * already in the sniffbuffer, as we could simply grab it from there.
 */

static inline boolean
mci_ip_slowcheck_inline (hwidbtype *input,
			 acl_headertype *acl)
{
    mciregtype *regptr;
    shortlong wrkreg;
    ushort protocol, ackrst, sport, dport;
    ulong source, destination;
    uchar tosbyte;
    
    regptr = input->mci_regptr;

    /*
     * Read the TOS, length, protocol and source address from the IP
     * header.  Unfortunately, this means that we pretty much end up
     * sniffing the entire IP header.
     */
    ackrst = MCI_OFFSETSIZE;
    if (input->status & (IDB_TR|IDB_FDDI))
	ackrst += ISO2_OVERHEAD + input->rxrifwordlen;
    else if (input->status & IDB_ATM)
	ackrst += ATM_OFF(input);
    else if (input->enctype == ET_ATM_DXI)
	ackrst = atm_dxi_the_line;

    regptr->argreg = ackrst;
    regptr->cmdreg = MCI_CMD_RX_SELECT;

    wrkreg.d.lword = regptr->readlong;
    tosbyte = wrkreg.d.sword[0] & 0xff;

    wrkreg.d.lword = regptr->readlong;	/* long 2, frag offset */
    sport = wrkreg.d.sword[1] & IP_FRAGOFFSET_MASK;
    wrkreg.d.lword = regptr->readlong;	/* really the protocol */
    protocol = wrkreg.d.sword[0] & 0xFF;
    source = regptr->readlong;
    destination = regptr->readlong;		/* really unfortunate */
    dport = 0;

    if (acl->tp_header_required) {
	/*
	 * This is a non-initial fragment.  Blindly accept packet if
	 * access list needs to look at transport layer header.
	 */
	if (sport)
	    return (sport > TCP_FLAGS_FRAGMENT) || (protocol != TCP_PROT);

	/*
	 * Read in the source and destination port if tcp or udp protocol.
	 * If TCP, set ackrst to non-zero if the ACK or RST bits are set.
	 * Please note that the IP header is 5 long words long -- we would
	 * not be here if that was not TRUE.
	 *
	 * Note that ackrst currently holds the offset of the start of the IP
	 * header.
	 */
	if (protocol == TCP_PROT) {
	    wrkreg.d.lword = regptr->readlong;	/* get source & dest */
	    sport = wrkreg.d.sword[0];
	    dport = wrkreg.d.sword[1];
	    regptr->argreg = ackrst + btow(MINIPHEADERBYTES) +
		btow(TCP_DOFF_FLAGS_OFFSET);
	    regptr->cmdreg = MCI_CMD_RX_SELECT;
	    ackrst = regptr->readshort;
	    ackrst &= TCP_ACKRST_MASK;
	} else if (protocol == UDP_PROT) {
	    wrkreg.d.lword = regptr->readlong; /* get source & dest */
	    sport = wrkreg.d.sword[0];
	    dport = wrkreg.d.sword[1];
	} else if (protocol == ICMP_PROT) {
	    sport = regptr->readshort;	/* get type & code */
	    dport = sport & 0xff;	/* get code */
	    sport >>= 8;		/* get type */
	} else if (protocol == IGMP_PROT) {
	    sport = regptr->readshort;	/* get type */
	    sport >>= 8;		/* get type */
	}
    }

    return(ip_slow_check_inline(acl, TRUE, input->fast_rcvidb, NULL,
				protocol, source, destination,
				sport, dport, ackrst, tosbyte, 0));
}

/*
 * Macros to handle DDR.
 */
#define DDR_SETNEXT(OUT, DDB)						\
    output_interface = DDB->fs_type;					\
    output_mci_regptr = OUT->mci_regptr;                                \
    if (output->cache_board_encap != NULL) {				\
	/*								\
	 * Set proper MIP channel.					\
	 */								\
	(*output->cache_board_encap)(output, cptr->m.macstring,		\
				     sizeof(ushort));			\
    }

#define DDR_CLASSIFY(OFFSET, RESUME)					\
    acl_headertype *acl;						\
									\
    acl = acl_array[dialerlist];					\
    if (acl) {								\
	/*								\
	 * Access list defined -- determine what type of filtering.	\
	 */								\
	switch (acl->type) {						\
	case ACL_IP_EXTENDED:						\
	case ACL_IP_NAMED_EXTENDED:					\
	    if (!mci_ip_slowcheck_inline(input,	acl))			\
		goto RESUME;						\
            break;							\
	case ACL_IP_SIMPLE:						\
	case ACL_IP_NAMED_SIMPLE:					\
	    {								\
	       MCI_IP_FASTCHECK(OFFSET, DDR, RESUME)			\
	    }								\
            break;							\
	default:							\
	    break;							\
	}								\
    }

/*
 * Macros for the *_ip_fastswitch wrapper routines.
 */



#define IP_FAST_STARTUP(ip_dest)   					\
/*		                                                        \
 * Need ip_dest argument because all interfaces except AIP leave	\
 * read pointer at ip destination.  AIP stashes dest in sniff buffer.   \
 * See CBUS_AIP_RECEIVE in if_ags_c7000.c. 		                \
 */		                                                        \
									\
    mciregtype *srcptr, *output_mci_regptr;				\
    ipcache_rntype *cptr;						\
    hwidbtype *output;							\
    shortlong dstadr;							\
    enum FSTYPE output_interface;					\
    ulong base_len;							\
    ulong len;								\
									\
    len = 0;								\
									\
    /*									\
     * RX_SETUP left read pointer at start of IP destination address,	\
     * so read it in now.  Given check out function and let MCI do its	\
     * thing while we do a cache lookup.  If the cache lookup fails or	\
     * the MCI says thumbs down, punt.					\
     *									\
     * Is it better to use the sniff buffer instead of a local?		\
     */									\
    srcptr = input->mci_regptr;						\
    dstadr.d.lword = (ip_dest);		                                \
    srcptr->cmdreg = MCI_CMD_RX_CHECK;					\
    cptr = ipcache_lookup_fast(dstadr.d.lword);				\
    input->mci_checked = (srcptr->cmdreg == MCI_RSP_OKAY);		\
    if (!input->mci_checked)                                            \
        return(FALSE);						        \
    if (!cptr) {							\
        if (!IPMULTICAST(dstadr.d.lword)) {		                \
            input->counters.cache_miss[ACCT_PROTO_IP]++;                \
            return(FALSE);                                              \
        }								\
                                                                        \
	/*								\
	 * IP multicast packet, call multicast fastswitching.		\
	 */								\
	return(reg_invoke_ipmulticast_hes_fs(dstadr.d.lword, input));	\
    }									\
									\
    output = cptr->idb->hwptr;						\
    /*    								\
     * Though we don't populate the cache when the input		\
     * and output interfaces are the same and				\
     * idb->ip_routecache_same isn't set, without this check,		\
     * there is nothing to stop us from fast-switching out		\
     * the same interface since cache can get populated from		\
     * other flow.							\
     */    								\
    if ((cptr->idb == input->fast_rcvidb) &&				\
        !cptr->idb->ip_routecache_same)   				\
        return (FALSE);  						\
									\
    output_interface = output->fast_switch_type;			\
									\
    if (!output->ip_fast_flags) {					\
	/*		                                                \
	 * If cbus and no access lists or accounting, update the	\
	 * populate the cbus autonomous cache.		                \
	 */		                                                \
	if (output->ip_routecache & IP_CBUS_ROUTECACHE)		        \
	    reg_invoke_auto_sw_ip_cache_update(cptr, NULL, dstadr.d.lword);\
	/*								\
	 * Re entry point, if we pass output access checks and/or	\
	 * perform IP accounting for this output interface.		\
	 */								\
    resume_flow:							\
	base_len = input->rxlength;

#define IP_FAST_STARTUP2						\
									\
	/*								\
	 * FS_SMDS check						\
	 * If outbound interface == FS_SMDS, need to zero pad bytes	\
	 * in RX window before doing the RX_MOVE.			\
	 */								\
	if (output_interface == FS_SMDS) {				\
	    spd = output->smds_pdb;					\
	    sm = (smdstype *)&cptr->m.macstring[spd->fast_output_modelen]; \
									\
	    switch (input->status & (IDB_ETHER|IDB_FDDI|IDB_TR|		\
				     IDB_SERIAL|IDB_ATM|IDB_LEX)) {	\
	    case IDB_ETHER:						\
		sm_input_enc = ETHER_ARPA_ENCAPBYTES;			\
		len = base_len - sm_input_enc;				\
		break;							\
	    case IDB_FDDI:						\
		sm_input_enc = FDDI_SNAP_ENCAPBYTES + 			\
				wtob(input->rxrifwordlen);		\
		len = base_len - sm_input_enc;				\
		sm_input_enc++;                                         \
		break;							\
	    case IDB_TR:						\
		sm_input_enc = TRING_SNAP_ENCAPBYTES +			\
			       wtob(input->rxrifwordlen);		\
		len = base_len - sm_input_enc;				\
		break;							\
	    case IDB_SERIAL:						\
		sm_input_enc = input->encsize;				\
		if (input->enctype != ET_SMDS) {			\
		   if (input->enctype == ET_ATM_DXI)		        \
		      sm_input_enc = input->atm_dxi_stuff->fast_input_encsize;\
		   len = base_len - sm_input_enc;			\
		} /* else len already set up */				\
		break;							\
	    case IDB_ATM:						\
		len = base_len;						\
		sm_input_enc = atm_vc_enclen;				\
		break;							\
	    case IDB_CHANNEL: /* CHANNELTODO: */			\
		sm_input_enc = input->encsize;				\
		base_len -= sm_input_enc;				\
		len = base_len;						\
		break;							\
	    case IDB_LEX:						\
		sm_input_enc = LEX_ARPA_ENCAPBYTES;			\
		len = base_len - sm_input_enc;				\
		break;							\
	    default:							\
		input->mci_checked = TRUE;				\
		return (FALSE);						\
	    }								\
	    sm_head_offset = input->rxoffset +  			\
				((sm_input_enc - cptr->length) >> 1);	\
	    sm_pad_offset = (sm_input_enc + (len & ~0x03)) >> 1;	\
	    sm_pad_offset += input->rxoffset;				\
	    srcptr->argreg = sm_pad_offset;				\
	    srcptr->cmdreg = MCI_CMD_RX_SELECT;				\
	    sm_lastlong = srcptr->readlong;				\
	    len += cptr->length;					\
	    sm_pad = 3 - (((len - spd->fast_output_modelen) +3) % 4);	\
	    sm->smds_basize = len - (4 + spd->fast_output_modelen) + sm_pad; \
	    sm->smds_xhlpi &= ~3;					\
	    sm->smds_xhlpi |= sm_pad;					\
	    sm->smds_betag = output->smds_begin_tag++;			\
	}								\
									\
	/*								\
	 * mci_testptr is always equal to mci_regptr, except when the	\
	 * interface is configured for DDR, in which case it is NULL	\
	 * so that we fall through the following tests directly to 	\
	 * FS_DIALER. 							\
	 */								\
	output_mci_regptr = output->mci_testptr;			\
	/*								\
	 * Re entry point for DDR.  output now points to the real	\
	 * output interface.						\
	 */								\
    resume_dialer:							\
	if (srcptr == output_mci_regptr) {				\
	    if (reserve_space_move (input, output, srcptr, base_len)) {

#define IP_ETHER_OUT \
		switch (output_interface) {                                         \
                                                                        \
      case FS_ETHER:                                                    \
                                                                        \
        /*                                                                     \
	 * If the output software subinterface is doing vLAN Routing then we   \
	 * need to do an extended MAC header write.                            \
	 * The trade off here is in making an extra check to see if this       \
	 * software IDB is doing dot10/ISL encapsulation, as against separate  \
         * code paths for Fast Switching vLAN encapsulated packets which would  \
	 * require microcode classification ...                                \
         * I don't love the double pointer dereference either - M. McNealis.    \
	 *                                                                     \
	 * Also note that the microcode determines on a packet by packet basis \
	 * whether or not to generate a software CRC (ISL requires a CRC for   \
	 * the interior packet, then an appended exterior CRC covering the     \
	 * entire packet - ugly!). So the mechanism to indicate this is to     \
	 * set bit 13 in the "len" field passed down to the SP; note the       \
	 * actual length on Fast Ethernet can be amply encoded in the first    \
	 * 10 bits!!                                                           \
         */                                                              \
                                                                        \
	if (cptr->idb->isl_vlan != NULL) {                              \
                                                                        \
            srcptr->argreg = (MCI_ETHER_OFFSET - ISL_INTERIOR_PACKET);  \
            srcptr->cmdreg = MCI_CMD_TX1_SELECT;                        \
                                                                        \
            ip_write_fast_ethernet_isl_vlan_header(cptr->m.maclong, srcptr); \
                                                                        \
	    /* packet by packet CRC toggle */                              \
                                                                        \
            srcptr->argreg = ((base_len + ISL_HEADER_LENGTH) | ISL_DO_CRC_MASK); \
            srcptr->cmdreg = MCI_CMD_TX1_START;                         \
                                                                        \
	    output->counters.tx_cumbytes_fast[ACCT_PROTO_IP] += base_len;\
	    cptr->idb->isl_vlan->egressing_isl_packets[VLAN_ROUTE_IP]++; \
            return(TRUE);                                                \
                                                                         \
	} else {                                                         \
                                                                         \
	    /*                                                                  \
  	     * Input ethernet -- output ethernet.                               \
	     */                                                           \
                                                                         \
	    srcptr->argreg = MCI_ETHER_OFFSET;                           \
	    srcptr->cmdreg = MCI_CMD_TX1_SELECT;                         \
	    ip_write_ether_header(srcptr, cptr);                         \
	    srcptr->argreg = base_len;                                   \
	    srcptr->cmdreg = MCI_CMD_TX1_START;                          \
	    output->counters.tx_cumbytes_fast[ACCT_PROTO_IP] += base_len;\
	    return(TRUE);                                                \
        }                                                                \
                                                                         \
      case FS_FDDI:                                                      \
	/*                                                                      \
	 * Input ethernet -- output fddi.                                       \
	 */                                                              \
	if (srb_multiring_none_enabled(output->firstsw)) {               \
	    srcptr->argreg = (MCI_ETHER_OFFSET  -                        \
			      btow(FDDI_SNAP_ENCAPBYTES - ETHER_ARPA_ENCAPBYTES));\
	    base_len += (FDDI_SNAP_ENCAPBYTES -  ETHER_ARPA_ENCAPBYTES); \
	    srcptr->cmdreg = MCI_CMD_TX1_SELECT;                         \
	    ip_write_fddi_header_norif(srcptr, cptr);                    \
	} else {                                                         \
	    srcptr->argreg = (MCI_ETHER_OFFSET +                         \
			      btow(ETHER_ARPA_ENCAPBYTES) -              \
			      (cptr->length/2 + 1));                     \
	    base_len += ((cptr->length + 1) - ETHER_ARPA_ENCAPBYTES);    \
	    srcptr->cmdreg = MCI_CMD_TX1_SELECT;                         \
	    ip_write_fddi_header(srcptr, cptr);                          \
	}                                                                \
	output->counters.tx_cumbytes_fast[ACCT_PROTO_IP] += base_len;    \
	base_len |= MCI_TX_ODDALIGN;                                     \
	srcptr->argreg = base_len;                                       \
	srcptr->cmdreg = MCI_CMD_TX1_START;                              \
	return(TRUE);                                                    \
      case FS_TOKEN:                                                     \
	/*                                                                      \
	 * Input ethernet -- output token ring.                                 \
	 */                                                              \
	srcptr->argreg = (MCI_ETHER_OFFSET +                             \
			  btow(ETHER_ARPA_ENCAPBYTES) -                  \
			  (cptr->length/2 + 1));                         \
	srcptr->cmdreg = MCI_CMD_TX1_SELECT;                             \
	ip_write_token_header(srcptr, cptr);                             \
	base_len += ((cptr->length + 2) - ETHER_ARPA_ENCAPBYTES);        \
	srcptr->argreg = base_len;                                       \
	srcptr->cmdreg = FCI_CMD_TX1_START;                              \
	output->counters.tx_cumbytes_fast[ACCT_PROTO_IP] += base_len;    \
	return(TRUE);                                                    \
      case FS_HDLC:                                                      \
      case FS_PPP:                                                       \
	/*                                                                      \
	 * input ethernet -- output hdlc/ppp                                    \
	 */                                                              \
	ip_common_serial_output(output, srcptr, cptr, base_len,          \
				MCI_OFFSETSIZE, ETHER_ARPA_ENCAPBYTES,   \
				FALSE, FALSE);                           \
	return(TRUE);                                                    \
      case FS_FR:                                                        \
	/*                                                                      \
	 * Input ethernet -- output serial.                                     \
	 */                                                              \
	ip_common_serial_output(output, srcptr, cptr, base_len,          \
				MCI_OFFSETSIZE, ETHER_ARPA_ENCAPBYTES,   \
				TRUE, FALSE);                            \
	return(TRUE);                                                    \
      case FS_SMDS:                                                      \
	/*                                                                      \
	 * Input ethernet -- output serial SMDS.                                \
	 */                                                              \
	ip_write_smds_trailer(srcptr, (ushort *)&sm->smds_rsvd,          \
			      sm_pad, sm_lastlong, sm_pad_offset);       \
	ip_write_smds_header(srcptr, cptr, sm_head_offset);              \
	len += (sm_pad + 4);                                             \
	srcptr->argreg = len;                                            \
	srcptr->cmdreg = MCI_CMD_TX1_START;                              \
	output->counters.tx_cumbytes_fast[ACCT_PROTO_IP] += len;         \
	return(TRUE);                                                    \
      case FS_ATM_DXI:                                                   \
	/*                                                                      \
	 * input ethernet -- output serial atm-dxi                              \
	 */                                                              \
	ip_common_serial_output(output, srcptr, cptr, base_len,          \
				MCI_OFFSETSIZE, ETHER_ARPA_ENCAPBYTES,   \
				FALSE, TRUE);                            \
	return(TRUE);                                                    \
      case FS_AIP:                                                       \
	/*                                                                      \
	 * Input ethernet -- output AIP.                                        \
	 */                                                              \
	ATM_VCOUTFAST(output, cptr->m.macshort[0]);                      \
	ATM_OUTFAST(output);                                             \
	base_len -= ETHER_ARPA_ENCAPBYTES;                               \
	ip_write_aip_header(srcptr, cptr,                                \
		MCI_OFFSETSIZE - btow(cptr->length), base_len);          \
	base_len += cptr->length;                                        \
	srcptr->argreg = base_len;                                       \
	srcptr->cmdreg = MCI_CMD_TX1_START;                              \
	output->counters.tx_cumbytes_fast[ACCT_PROTO_IP] += base_len;    \
	return(TRUE);                                                    \
	break;                                                           \
                                                                         \
      case FS_CHANNEL:                                                   \
	  /*                                                                    \
	 * Input ethernet -- output channel.                                    \
	 */                                                              \
	srcptr->argreg = MCI_OFFSETSIZE - btow(cptr->length);            \
	srcptr->cmdreg = MCI_CMD_TX1_SELECT;                             \
	ip_write_channel_header(srcptr, cptr);                           \
	base_len -= ETHER_ARPA_ENCAPBYTES;                               \
	base_len += cptr->length;                                        \
	srcptr->argreg = base_len;                                       \
	srcptr->cmdreg = MCI_CMD_TX1_START;                              \
	output->counters.tx_cumbytes_fast[ACCT_PROTO_IP] += base_len;    \
	return(TRUE);                                                    \
                                                                         \
    case FS_LEX:                                                         \
	/*                                                                      \
	 * Input ethernet -- output Lex.                                        \
	 */                                                             \
	srcptr->argreg = (MCI_ETHER_OFFSET -                             \
			  btow(cptr->length - ETHER_ARPA_ENCAPBYTES));   \
	srcptr->cmdreg = MCI_CMD_TX1_SELECT;                             \
	ip_write_lex_header(srcptr, cptr);                               \
	base_len -= (ETHER_ARPA_ENCAPBYTES - cptr->length);              \
	srcptr->argreg = base_len;                                       \
	srcptr->cmdreg = MCI_CMD_TX1_START;                              \
	output->counters.tx_cumbytes_fast[ACCT_PROTO_IP] += base_len;    \
	return(TRUE);                                                    \
                                                                         \
    default:                                                             \
	break;                                                           \
    }                                          



#define IP_FAST_CONGESTION						\
	    } else if (0 != output->priority_list) {			\
		ushort outgoing_mac_length = cptr->length;		\
		paktype *pak;						\
		len = base_len;

#define IP_FAST_FINISH(EXTRA_OFFSET)					\
		pak = getbuffer(len + outgoing_mac_length);		\
		if (!pak) {						\
		    goto punt;						\
		}							\
                pak->if_output = cptr->idb;	         		\
    		ip_traffic.forwarded++;					\
		pak->linktype = LINK_IP;				\
		pak->network_start = pak->data_area + ENCAPBYTES;	\
        	if (srcptr->rxtype == RXTYPE_UNKNOWN) {			\
		    mci2buffer(&srcptr->readlong,			\
			pak->network_start + sizeof(iphdrtype), len);	\
		    bcopy(&temp_iphdr, pak->network_start,		\
			  sizeof(iphdrtype));				\
		} else {						\
		    mci2buffer(&srcptr->readlong, pak->network_start, len); \
		}							\
		srcptr->cmdreg = MCI_CMD_RX_FLUSH;			\
		pak->datagramsize = GETSHORT(pak->network_start + 2);	/* size taken from message */	\
		/*							\
		 * add outbound data link header			\
		 */							\
		ip_hes_mac_rewrite(pak, output, cptr);			\
		/*							\
		 * save in a queue for later transmission		\
		 */							\
		holdq_enqueue_fast(output, pak);		        \
		/* need to make a tx-reserve to get an interrupt */	\
		if (1 == output->output_qcount)				\
		    (*(output->soutput))(output);			\
		return(TRUE);					     	\
	    } else {							\
									\
		punt:							\
		/*							\
		 * RX_MOVE failed -- drop the datagram.			\
		 */							\
		srcptr->cmdreg = MCI_CMD_RX_FLUSH;			\
		output->counters.output_total_drops++;			\
		return(TRUE);						\
	    }								\
	} else if (output_mci_regptr) {					\
									\
	    /*								\
	     * Output is on another MCI, if TX1 bug, drop the packet.	\
	     */								\
	    if (((mcitype *)output->devctl)->tx1bug == FALSE) {		\
		ip_mci2mci(input, cptr);				\
		return(TRUE);						\
	    }								\
	} else if (output == nullidb->hwptr) {				\
	    /*								\
	     * fast-drop the datagram					\
	     */								\
	    srcptr->cmdreg = MCI_CMD_RX_FLUSH;				\
	    return(TRUE);						\
	} else if (output_interface == FS_DIALER) {			\
	    DDR_FASTSWITCH(input, output, cptr, DDR_SETNEXT,		\
			   ip_dialerlist, DDR_CLASSIFY, EXTRA_OFFSET,	\
			   cptr->dialergroup,				\
			   resume_dialer);				\
	} else {							\
	    /*								\
	     * Oops.  Shouldn't ever get here.				\
	     */								\
	    return(FALSE);						\
	}								\
    deny_access:							\
	input->mci_checked = TRUE;					\
	return(FALSE);							\
    } else {								\
	acl_headertype *acl;						\
									\
	/*								\
	 * Either access lists or accounting is enabled on this		\
	 * interface.  Assume that access list(s) are why we are here.	\
	 */								\
	acl = cptr->idb->ip_output_accesslist;				\
	if (acl) {							\
									\
	    /*								\
	     * Access list(s) defined -- determine what type of		\
	     * filtering.						\
	     */								\
									\
	    switch (acl->type) {					\
		case ACL_IP_EXTENDED:					\
		case ACL_IP_NAMED_EXTENDED:				\
		    if (!mci_ip_slowcheck_inline(input, acl))		\
			goto deny_access;				\
		    break;						\
									\
		case ACL_IP_SIMPLE:					\
		case ACL_IP_NAMED_SIMPLE:				\
		    {							\
			MCI_IP_FASTCHECK(EXTRA_OFFSET, OUT,		\
					 deny_access)			\
		    }							\
		    break;						\
									\
		default:						\
		    break;						\
	    }								\
	}								\
									\
	/*								\
	 * Now check to see if the ANY interface has an input access	\
	 * list on it.							\
	 */								\
	if (output->ip_fast_flags & IP_FAST_INPUTACCESSLISTS) {		\
	    if (is_ddr(input)) {					\
		hwidbtype *netidb;					\
									\
		netidb = get_netidb(input);				\
		acl = netidb->firstsw->ip_input_accesslist;		\
	    } else							\
		acl = input->fast_rcvidb->ip_input_accesslist;		\
	    if (acl) {							\
	       switch (acl->type) {					\
		   case ACL_IP_EXTENDED:				\
		   case ACL_IP_NAMED_EXTENDED:				\
		       if (!mci_ip_slowcheck_inline(input, acl))	\
			   goto deny_access;				\
		       break;						\
									\
		   case ACL_IP_SIMPLE:					\
		   case ACL_IP_NAMED_SIMPLE:				\
		      {							\
			MCI_IP_FASTCHECK(EXTRA_OFFSET, IN,		\
					 deny_access)			\
		      }							\
		      break;						\
									\
		default:						\
		    break;						\
	        }							\
            }								\
	}								\
									\
	if (output->ip_fast_flags & IP_FAST_ACCOUNTING) {		\
									\
	    /*								\
	     * IP accounting selected for this interface. The function	\
	     * only returns FALSE if more memory is needed, done at	\
	     * process level.						\
	     */								\
	    if (!(mci_ip_accounting(input)))				\
		goto deny_access;					\
	}								\
									\
        /*                                                              \
	 * Is IP policy routing enabled on some interface?              \
	 */                                                             \
        if ((output->ip_fast_flags & IP_FAST_POLICY) &&                 \
	    (input->fast_rcvidb->ip_info &&                             \
	     input->fast_rcvidb->ip_info->policy_enabled))              \
	    goto deny_access;                                           \
									\
	/*                                                              \
	 * Is translation possibly needed on the packet?                \
	 */                                                             \
	if ((input->ip_fast_flags & IP_FAST_NAT) &&                     \
	    (output->ip_fast_flags & IP_FAST_NAT))                      \
	    goto deny_access;                                           \
	goto resume_flow;						\
    }

/*
 * ip_read_header_and_checksum is primarily for the ags - it
 * reads in the ip header, calculates the checksum and ttl,
 * and updates the checksum and ttl.
 *
 * Must set the srcptr->argreg to the correct rxoffset before calling this
 * routine.
 */

static inline boolean ip_read_header_and_checksum (hwidbtype *input,
					           mciregtype *srcptr,
					           ulong *dstadr)
{
    ushort sum, *yy;
    ulong crc, *xx;
    register uchar *ttl;

    input->mci_checked = FALSE;
    xx = (ulong *) &temp_iphdr;
    *xx++ = srcptr->readlong;
    *xx++ = srcptr->readlong;
    *xx++ = srcptr->readlong;
    *xx++ = srcptr->readlong;
    *xx++ = srcptr->readlong;

    *dstadr = (ulong) temp_iphdr.dstadr;
    /*
     * Here we MUST check the checksum of the packet we got.  If we've got
     * a bad checksum, drop the packet.
     */
    sum = temp_iphdr.checksum;
    temp_iphdr.checksum = 0;
    yy = (ushort *) &temp_iphdr;
    crc = 0;
    crc += *yy++;
    crc += *yy++;
    crc += *yy++;
    crc += *yy++;
    crc += *yy++;
    crc += *yy++;
    crc += *yy++;
    crc += *yy++;
    crc += *yy++;
    crc += *yy++;
    while (crc >> 16)
	crc = (crc & 0xFFFF) + (crc >> 16);
    crc = (~crc & 0xffff);
    if (sum != crc)
	return(FALSE);
    /*
     * Get the TTL to decrement it.  If it will be <= 0 reject the packet
     */
    ttl = &temp_iphdr.ttl;
    if (*ttl <= 1)
	return(FALSE);
    (*ttl)--;

    /* Get the checksum & adjust it as well */
    crc += 0x100;
    while (crc >> 16)
	crc = (crc & 0xFFFF) + (crc >> 16);

    temp_iphdr.checksum = (ushort)crc;
    /*
     * At this point, consider the packet "MCI" checked.
     */
    input->mci_checked = TRUE;
    return(TRUE);
}

/*
 * ip_common_serial_output
 * Used for hdlc, ppp, atm-dxi and frame-relay output for all interfaces
 * Will do pvc accounting for frame-relay - Won't include the code
 * for non-frame relay, if you have a constant for the isframerelay param
 * (same for atm-dxi and the isatmdxi variable)
 */
static inline void ip_common_serial_output (hwidbtype *output,
					    mciregtype *regptr,
					    ipcache_rntype *cptr,
					    ushort base_len,
					    ushort ip_line,
					    ushort input_encaplen,
					    boolean isframerelay,
					    boolean isatmdxi)
{
    regptr->argreg = ip_line - btow(cptr->length);
    regptr->cmdreg = MCI_CMD_TX1_SELECT;
    ip_write_serial_header(regptr,cptr);
    base_len -= input_encaplen;
    base_len += cptr->length;
    regptr->argreg = base_len;
    regptr->cmdreg = MCI_CMD_TX1_START;
    output->counters.tx_cumbytes_fast[ACCT_PROTO_IP] += base_len;
    if (isframerelay) {
	/* Do pvc accounting - get pvc number from cache */
	INC_PVC_FAST_OUTPUT(HWIDB_TO_FR_PVC(output,cptr->subint_reference_num),
			    base_len);
    }
    if (isatmdxi) {
	INC_DXI_PVC_FAST_OUTPUT(HWIDB_TO_DXI_PVC(output,
						 cptr->subint_reference_num),
				base_len);
    }
}



/* 
 * isl_ether_ip_fastswitch
 * It does almost everything as ether_ip_fastswitch does, but it decrements ttl
 * for ISL packet by  ip_read_header_and_checksum() not the microcode, as the
 * the RXTYPE is set to ISL not IP for ISL ip packet, hence we cann't use 
 * MCI_CMD_RX_CHECK to decrement TTL for ISL packets.
 */
boolean isl_ether_ip_fastswitch (hwidbtype *input)
{

    /*		                                                        
     * Need ip_dest argument because all interfaces except AIP leave	
     * read pointer at ip destination.  AIP stashes dest in sniff buffer.   
     * See CBUS_AIP_RECEIVE in if_ags_c7000.c. 		                
     */	
    mciregtype *srcptr, *output_mci_regptr;				
    ipcache_rntype *cptr;						
    hwidbtype *output;							
    shortlong dstadr;							
    enum FSTYPE output_interface;					
    ulong base_len;							
    ulong len;								
									
    len = 0;								
									
    /*									
     * RX_SETUP left read pointer at start of IP destination address,	
     * so read it in now.  Given check out function and let MCI do its	
     * thing while we do a cache lookup.  If the cache lookup fails or	
     * the MCI says thumbs down, punt.					
     *									
     * Is it better to use the sniff buffer instead of a local?		
     */									
    srcptr = input->mci_regptr;

    /* 
     * Don't follow the IP_FAST_STARTUP model: eg, use MCI_CMD_RX_CHECK and
     * set mci_checked, as the micro code could lie to us: even though 
     * MCI_CMD_RX_CHECK  doesn't recalculate TTL for ISL packet (because the
     * RXTYPE is ISL not IP), the microcode still returns MCI_RSP_OKAY, then we
     * will get wrong TTL/pak->flags. The solution is to recalculate TTL by
     * calling ip_read_header_and_checksum() for fast switched ISL packet or
     * ipttl() for process switched ISL packet later.
     */
    srcptr->argreg = MCI_OFFSETSIZE;
    srcptr->cmdreg = MCI_CMD_RX_SELECT;
    if (!ip_read_header_and_checksum(input, srcptr, &dstadr.d.lword))
	return (FALSE);

    cptr = ipcache_lookup_fast(dstadr.d.lword);				
    if (!cptr) {							
        if (!IPMULTICAST(dstadr.d.lword)) {

	    /* 
	     * Set mci_checked to FALSE so that the packet's TTL can be
	     * decremented in process path.
	     */
	    input->mci_checked = FALSE;		                
            input->counters.cache_miss[ACCT_PROTO_IP]++; 
            return(FALSE);                                              
        }								
                                                                        
	/*								
	 * IP multicast packet, call multicast fastswitching.		
	 */
								
	return(reg_invoke_ipmulticast_hes_fs(dstadr.d.lword, input));	
    }									
									
    output = cptr->idb->hwptr;						
    output_interface = output->fast_switch_type;			
									
    if (!output->ip_fast_flags) {					
	/*		                                                
	 * If cbus and no access lists or accounting, update the	
	 * populate the cbus autonomous cache.		                
	 */		                                                
	if (output->ip_routecache & IP_CBUS_ROUTECACHE)		        
	    reg_invoke_auto_sw_ip_cache_update(cptr, NULL, dstadr.d.lword);
	/*								
	 * Re entry point, if we pass output access checks and/or	
	 * perform IP accounting for this output interface.		
	 */								
    resume_flow:						       
	base_len = input->rxlength;

    if (base_len > cptr->idb->ip_mtu + ETHER_ARPA_ENCAPBYTES) {
	return (FALSE);
    }

    IP_FAST_STARTUP2

    srcptr->argreg = MCI_OFFSETSIZE;
    srcptr->cmdreg = MCI_CMD_TX1_SELECT;
    write_new_iphdr(srcptr);
	
    IP_ETHER_OUT
    
    IP_FAST_CONGESTION;

    /*
     * remove ethernet header and queue or discard message
     */
    srcptr->argreg = MCI_ETHER_OFFSET + btow(ETHER_ARPA_ENCAPBYTES);
    srcptr->cmdreg = MCI_CMD_RX_SELECT;
    len -= (ETHER_ARPA_ENCAPBYTES);

    IP_FAST_FINISH(0);

}

/*
 * *_ip_fastswitch
 * Very high speed switching of Ether, HDLC or FDDI encapsulated IP datagrams.
 * This routine has been very heavily optimized for speed, since it is in
 * the inner loop of the code that handles back-to-back datagrams.  Do not
 * add permanent debugging code, subroutines, or much of anything else.
 * The GNU CC compiler will generate the best code.
 * Speedups *must* be debugged with an oscilloscope.
 */

boolean ether_ip_fastswitch (hwidbtype *input)
{

    IP_FAST_STARTUP(srcptr->readlong)

    /*
     * Don't do the RX_MOVE if the datagram exceeds the
     * maximum output interface datagram size.
     */

    if (base_len > cptr->idb->ip_mtu + ETHER_ARPA_ENCAPBYTES) {
	input->mci_checked = TRUE;
	return (FALSE);
    }

    IP_FAST_STARTUP2

    IP_ETHER_OUT

    IP_FAST_CONGESTION;

    /*
     * remove ethernet header and queue or discard message
     */
    srcptr->argreg = MCI_ETHER_OFFSET + btow(ETHER_ARPA_ENCAPBYTES);
    srcptr->cmdreg = MCI_CMD_RX_SELECT;
    len -= (ETHER_ARPA_ENCAPBYTES);

    IP_FAST_FINISH(0);
}

boolean fddi_ip_fastswitch (hwidbtype *input)
{

    IP_FAST_STARTUP(srcptr->readlong)

    /*
     * Don't do the RX_MOVE if the datagram exceeds the
     * maximum output interface datagram size.
     */

    if (base_len > cptr->idb->ip_mtu + FDDI_SNAP_ENCAPBYTES) {
	input->mci_checked = TRUE;
	return (FALSE);
    }

    IP_FAST_STARTUP2

    switch (output_interface) {
      case FS_ETHER:
	/*
	 * Input fddi -- output ethernet.
	 */

	if (input->rxrifwordlen == 0) {
	    srcptr->argreg = (FDDI_RECEIVE_OFFSET +
			  btow(FDDI_SNAP_ENCAPBYTES - ETHER_ARPA_ENCAPBYTES));
	    base_len -= (FDDI_SNAP_ENCAPBYTES - ETHER_ARPA_ENCAPBYTES);
	} else {
	    srcptr->argreg = (FDDI_RECEIVE_OFFSET + input->rxrifwordlen +
			  btow(FDDI_SNAP_ENCAPBYTES - ETHER_ARPA_ENCAPBYTES));
	    base_len -= (FDDI_SNAP_ENCAPBYTES + wtob(input->rxrifwordlen)
			 - ETHER_ARPA_ENCAPBYTES);
	}

	if (cptr->idb->isl_vlan != NULL) {

	    srcptr->argreg = (FDDI_RECEIVE_OFFSET +
			      input->rxrifwordlen +
			      btow(FDDI_SNAP_ENCAPBYTES -
				   ETHER_ARPA_ENCAPBYTES) -
			      ISL_INTERIOR_PACKET);

	    srcptr->cmdreg = MCI_CMD_TX1_SELECT;

	    ip_write_fast_ethernet_isl_vlan_header(cptr->m.maclong,
						   srcptr);
	    base_len += ISL_HEADER_LENGTH;

	    /* packet by packet CRC toggle */

	    srcptr->argreg = (base_len | ISL_DO_CRC_MASK);
	    srcptr->cmdreg = MCI_CMD_TX1_START;

	    output->counters.tx_cumbytes_fast[ACCT_PROTO_IP] += base_len;
	    cptr->idb->isl_vlan->egressing_isl_packets[VLAN_ROUTE_IP]++;
	    return(TRUE);
	}

	srcptr->cmdreg = MCI_CMD_TX1_SELECT;
	ip_write_ether_header(srcptr, cptr);
	if (base_len < MINETHERSIZE)
	    base_len = MINETHERSIZE;
	srcptr->argreg = base_len;
	srcptr->cmdreg = MCI_CMD_TX1_START;
	output->counters.tx_cumbytes_fast[ACCT_PROTO_IP] += base_len;
	return(TRUE);
      case FS_FDDI:
	/*
	 * Input fddi -- output fddi.
	 */
	if ((srb_multiring_none_enabled(output->firstsw)) && (input->rxrifwordlen == 0)) {
	    srcptr->argreg = FDDI_RECEIVE_OFFSET;
	    srcptr->cmdreg = MCI_CMD_TX1_SELECT;
	    ip_write_fddi_header_norif(srcptr, cptr);
	} else {
	    srcptr->argreg = (FDDI_RECEIVE_OFFSET +
			      (btow(FDDI_SNAP_ENCAPBYTES) -
			       (cptr->length/2 + 1))) +
				   input->rxrifwordlen;
	    srcptr->cmdreg = MCI_CMD_TX1_SELECT;
	    ip_write_fddi_header(srcptr, cptr);
	    base_len += ((1 + cptr->length) - FDDI_SNAP_ENCAPBYTES -
			 wtob(input->rxrifwordlen));
	}
	output->counters.tx_cumbytes_fast[ACCT_PROTO_IP] += base_len;
	base_len |= MCI_TX_ODDALIGN;
	srcptr->argreg = base_len;
	srcptr->cmdreg = MCI_CMD_TX1_START;
	return(TRUE);
      case FS_TOKEN:
	/*
	 * Input fddi -- output token ring.
	 */
	srcptr->argreg = (FDDI_RECEIVE_OFFSET +
  			  (btow(FDDI_SNAP_ENCAPBYTES) -
 			   (cptr->length/2 + 1))) +
 			       input->rxrifwordlen;
	srcptr->cmdreg = MCI_CMD_TX1_SELECT;
	ip_write_token_header(srcptr, cptr);
	base_len -= (FDDI_SNAP_ENCAPBYTES + wtob(input->rxrifwordlen) -
 		     (2 + cptr->length));
	srcptr->argreg = base_len;
	srcptr->cmdreg = MCI_CMD_TX1_START;
	output->counters.tx_cumbytes_fast[ACCT_PROTO_IP] += base_len;
	return(TRUE);
      case FS_HDLC:
      case FS_PPP:
	/*
	 * Input fddi -- output serial.
	 */
	if (input->rxrifwordlen == 0) {
	    ip_common_serial_output(output, srcptr, cptr, base_len,
				    (FDDI_RECEIVE_OFFSET +
				     btow(FDDI_SNAP_ENCAPBYTES)),
				    FDDI_SNAP_ENCAPBYTES, FALSE, FALSE);
	} else {
	    ip_common_serial_output(output, srcptr, cptr, base_len,
				    (FDDI_RECEIVE_OFFSET +
				     btow(FDDI_SNAP_ENCAPBYTES) +
				     input->rxrifwordlen),
				    (FDDI_SNAP_ENCAPBYTES +
				     wtob(input->rxrifwordlen)),
				    FALSE, FALSE);
	}
	return(TRUE);
      case FS_FR:
	/*
	 * Input fddi -- output serial.
	 */
	if (input->rxrifwordlen == 0) {
	    ip_common_serial_output(output, srcptr, cptr, base_len,
				    (FDDI_RECEIVE_OFFSET +
				     btow(FDDI_SNAP_ENCAPBYTES)),
				    FDDI_SNAP_ENCAPBYTES, TRUE, FALSE);
	} else {
	    ip_common_serial_output(output, srcptr, cptr, base_len,
				    (FDDI_RECEIVE_OFFSET +
				     btow(FDDI_SNAP_ENCAPBYTES) +
				     input->rxrifwordlen),
				    (FDDI_SNAP_ENCAPBYTES +
				     wtob(input->rxrifwordlen)),
				    TRUE, FALSE);
	}
	return(TRUE);
      case FS_SMDS:
	/*
	 * Input FDDI SMDS -- output serial SMDS.
	 */
	{
	    ip_write_smds_trailer(srcptr, (ushort *)&sm->smds_rsvd, sm_pad,
				  sm_lastlong, sm_pad_offset);
	    ip_write_smds_header(srcptr, cptr, sm_head_offset);
	    len += (sm_pad + 4);
	    srcptr->argreg = len;
	    srcptr->cmdreg = MCI_CMD_TX1_START;
	    output->counters.tx_cumbytes_fast[ACCT_PROTO_IP] += len;
	    return(TRUE);
	}
      case FS_ATM_DXI:
	/*
	 * Input fddi -- output serial atm-dxi
	 */
	ip_common_serial_output(output, srcptr, cptr, base_len,
  				(FDDI_RECEIVE_OFFSET +
 				 btow(FDDI_SNAP_ENCAPBYTES) +
 				 input->rxrifwordlen),
 				(FDDI_SNAP_ENCAPBYTES +
 				 wtob(input->rxrifwordlen)),
 				FALSE, TRUE);
	return(TRUE);
      case FS_AIP:
	/*
	 * Input FDDI -- output AIP.
	 */
	ATM_VCOUTFAST(output, cptr->m.macshort[0]);
	ATM_OUTFAST(output);
	base_len -= FDDI_SNAP_ENCAPBYTES + wtob(input->rxrifwordlen);
	ip_write_aip_header(srcptr, cptr,
 			    MCI_OFFSETSIZE - btow(cptr->length) + 3 +
 			    input->rxrifwordlen, base_len);
	base_len += cptr->length;
	srcptr->argreg = base_len;
	srcptr->cmdreg = MCI_CMD_TX1_START;
	output->counters.tx_cumbytes_fast[ACCT_PROTO_IP] += base_len;
	return(TRUE);
	break;

      case FS_CHANNEL:
	/*
	 * Input FDDI -- output channel
	 */
	srcptr->argreg = (FDDI_RECEIVE_OFFSET + input->rxrifwordlen +
 			  btow(FDDI_SNAP_ENCAPBYTES) - btow(cptr->length));
	srcptr->cmdreg = MCI_CMD_TX1_SELECT;
	ip_write_channel_header(srcptr, cptr);
	base_len -= (FDDI_SNAP_ENCAPBYTES + wtob(input->rxrifwordlen));
	base_len += cptr->length;
	srcptr->argreg = base_len;
	srcptr->cmdreg = MCI_CMD_TX1_START;
	output->counters.tx_cumbytes_fast[ACCT_PROTO_IP] += base_len;
	return(TRUE);

      case FS_LEX:
	/*
	 * Input fddi -- output Lex.
	 */
	srcptr->argreg = (FDDI_RECEIVE_OFFSET + input->rxrifwordlen +
			  btow(FDDI_SNAP_ENCAPBYTES - cptr->length));
	srcptr->cmdreg = MCI_CMD_TX1_SELECT;
	ip_write_lex_header(srcptr, cptr);
	base_len -= (FDDI_SNAP_ENCAPBYTES + wtob(input->rxrifwordlen)
 		     - cptr->length);
	srcptr->argreg = base_len;
	srcptr->cmdreg = MCI_CMD_TX1_START;
	output->counters.tx_cumbytes_fast[ACCT_PROTO_IP] += base_len;
	return(TRUE);

      default:
	break;
    }

    IP_FAST_CONGESTION;

    /*
     * remove FDDI header and queue or discard message
     */
    srcptr->argreg = FDDI_RECEIVE_OFFSET +
	btow(FDDI_SNAP_ENCAPBYTES) + input->rxrifwordlen;
    srcptr->cmdreg = MCI_CMD_RX_SELECT;
    len -= (FDDI_SNAP_ENCAPBYTES) + wtob(input->rxrifwordlen);
    IP_FAST_FINISH(ISO2_OVERHEAD + input->rxrifwordlen)
}

boolean token_ip_fastswitch (hwidbtype *input)
{
    IP_FAST_STARTUP(srcptr->readlong)

    /*
     * Don't do the RX_MOVE if the datagram exceeds the
     * maximum output interface datagram size.
     */

    if (base_len > cptr->idb->ip_mtu + TRING_SNAP_ENCAPBYTES) {
	input->mci_checked = TRUE;
	return (FALSE);
    }

    IP_FAST_STARTUP2

    switch (output_interface) {
      case FS_ETHER:
	/*
	 * Input token ring -- output ethernet.
	 */

	if (cptr->idb->isl_vlan != NULL) {

	    srcptr->argreg = (CTR_RECEIVE_OFFSET +
			      btow(TRING_SNAP_ENCAPBYTES) +
			      input->rxrifwordlen -
			      btow(ETHER_ARPA_ENCAPBYTES) -
			      ISL_INTERIOR_PACKET);

	    srcptr->cmdreg = MCI_CMD_TX1_SELECT;

	    ip_write_fast_ethernet_isl_vlan_header(cptr->m.maclong,
						   srcptr);
	    base_len -= (TRING_SNAP_ENCAPBYTES +
			 wtob(input->rxrifwordlen) -
			 ETHER_ARPA_ENCAPBYTES - ISL_HEADER_LENGTH);

	    /* packet by packet CRC toggle */

	    srcptr->argreg = (base_len | ISL_DO_CRC_MASK);
	    srcptr->cmdreg = MCI_CMD_TX1_START;

	    output->counters.tx_cumbytes_fast[ACCT_PROTO_IP] += base_len;
	    cptr->idb->isl_vlan->egressing_isl_packets[VLAN_ROUTE_IP]++;
	    return(TRUE);
	}

	srcptr->argreg = (CTR_RECEIVE_OFFSET + btow(TRING_SNAP_ENCAPBYTES) +
			  input->rxrifwordlen - btow(ETHER_ARPA_ENCAPBYTES));
	srcptr->cmdreg = MCI_CMD_TX1_SELECT;
	ip_write_ether_header(srcptr, cptr);
	base_len -= (TRING_SNAP_ENCAPBYTES + wtob(input->rxrifwordlen) -
		     ETHER_ARPA_ENCAPBYTES);
	if (base_len < MINETHERSIZE)
	    base_len = MINETHERSIZE;
	srcptr->argreg = base_len;
	srcptr->cmdreg = MCI_CMD_TX1_START;
	output->counters.tx_cumbytes_fast[ACCT_PROTO_IP] += base_len;
	return(TRUE);
      case FS_FDDI:
	/*
	 * Input token ring -- output fddi.
	 */
	srcptr->argreg = (CTR_RECEIVE_OFFSET + btow(TRING_SNAP_ENCAPBYTES) +
 			  input->rxrifwordlen - (cptr->length/2 + 1));
	srcptr->cmdreg = MCI_CMD_TX1_SELECT;
	ip_write_fddi_header(srcptr, cptr);
	base_len -= (TRING_SNAP_ENCAPBYTES + wtob(input->rxrifwordlen) -
 		     cptr->length - 1);
	output->counters.tx_cumbytes_fast[ACCT_PROTO_IP] += base_len;
	base_len |= MCI_TX_ODDALIGN;
	srcptr->argreg = base_len;
	srcptr->cmdreg = MCI_CMD_TX1_START;
	return(TRUE);
      case FS_TOKEN:
	/*
	 * Input token ring -- output token ring.
	 */
	srcptr->argreg = (CTR_RECEIVE_OFFSET + btow(TRING_SNAP_ENCAPBYTES) +
			  input->rxrifwordlen - ((2 + cptr->length) >> 1));
	srcptr->cmdreg = MCI_CMD_TX1_SELECT;
	ip_write_token_header(srcptr, cptr);
	base_len -= (TRING_SNAP_ENCAPBYTES + wtob(input->rxrifwordlen) -
		     (2 + cptr->length));
	srcptr->argreg = base_len;
	srcptr->cmdreg = MCI_CMD_TX1_START;
	output->counters.tx_cumbytes_fast[ACCT_PROTO_IP] += base_len;
	return(TRUE);
      case FS_HDLC:
      case FS_PPP:
	/*
	 * Input token ring -- output hdlc/ppp
	 */
	ip_common_serial_output(output, srcptr, cptr, base_len,
				(CTR_RECEIVE_OFFSET +
				 btow(TRING_SNAP_ENCAPBYTES) +
				 input->rxrifwordlen),
				(TRING_SNAP_ENCAPBYTES +
				 wtob(input->rxrifwordlen)),
				FALSE, FALSE);
	return(TRUE);
    case FS_FR:
	/*
	 * Input token ring -- output frame-relay.
	 */
	ip_common_serial_output(output, srcptr, cptr, base_len,
				(CTR_RECEIVE_OFFSET +
				 btow(TRING_SNAP_ENCAPBYTES) +
				 input->rxrifwordlen),
				(TRING_SNAP_ENCAPBYTES +
				 wtob(input->rxrifwordlen)),
				TRUE, FALSE);
	return(TRUE);
    case FS_SMDS:
	/*
	 * Input TokenRing -- output serial SMDS.
	 */
	{
	    ip_write_smds_trailer(srcptr, (ushort *)&sm->smds_rsvd, sm_pad,
				  sm_lastlong, sm_pad_offset);
	    ip_write_smds_header(srcptr, cptr, sm_head_offset);
	    len += (sm_pad + 4);
	    srcptr->argreg = len;
	    srcptr->cmdreg = MCI_CMD_TX1_START;
	    output->counters.tx_cumbytes_fast[ACCT_PROTO_IP] += len;
	    return(TRUE);
	}
    case FS_ATM_DXI:
	/*
	 * input TokenRing -- output serial atm-dxi
	 */
	ip_common_serial_output(output, srcptr, cptr, base_len,
				(CTR_RECEIVE_OFFSET +
				 btow(TRING_SNAP_ENCAPBYTES) +
				 input->rxrifwordlen),
				(TRING_SNAP_ENCAPBYTES +
				 wtob(input->rxrifwordlen)),
				FALSE, TRUE);
	return(TRUE);
    case FS_AIP:
	/*
	 * Input TokenRing -- output AIP.
	 */
	ATM_VCOUTFAST(output, cptr->m.macshort[0]);
	ATM_OUTFAST(output);
	base_len -= (TRING_SNAP_ENCAPBYTES + wtob(input->rxrifwordlen));
	ip_write_aip_header(srcptr, cptr,
			    (CTR_RECEIVE_OFFSET+btow(TRING_SNAP_ENCAPBYTES) +
			     input->rxrifwordlen - btow(cptr->length)),
			    base_len);
	base_len += cptr->length;
	srcptr->argreg = base_len;
	srcptr->cmdreg = MCI_CMD_TX1_START;
	output->counters.tx_cumbytes_fast[ACCT_PROTO_IP] += base_len;
	return(TRUE);
	break;

      case FS_CHANNEL:
	/*
	 * Input token ring -- output channel
	 */
	srcptr->argreg = (CTR_RECEIVE_OFFSET + btow(TRING_SNAP_ENCAPBYTES) +
			  input->rxrifwordlen - btow(cptr->length));
	srcptr->cmdreg = MCI_CMD_TX1_SELECT;
	ip_write_channel_header(srcptr, cptr);
	base_len -= (TRING_SNAP_ENCAPBYTES + wtob(input->rxrifwordlen));
	base_len += cptr->length;
	srcptr->argreg = base_len;
	srcptr->cmdreg = MCI_CMD_TX1_START;
	output->counters.tx_cumbytes_fast[ACCT_PROTO_IP] += base_len;
	return(TRUE);

      case FS_LEX:
	/*
	 * Input token ring -- output Lex.
	 */
	srcptr->argreg = (CTR_RECEIVE_OFFSET + input->rxrifwordlen +
			  btow(TRING_SNAP_ENCAPBYTES - cptr->length));
	srcptr->cmdreg = MCI_CMD_TX1_SELECT;
	ip_write_lex_header(srcptr, cptr);
	base_len -= (TRING_SNAP_ENCAPBYTES + wtob(input->rxrifwordlen) -
		     cptr->length);
	srcptr->argreg = base_len;
	srcptr->cmdreg = MCI_CMD_TX1_START;
	output->counters.tx_cumbytes_fast[ACCT_PROTO_IP] += base_len;
	return(TRUE);

      default:
	break;
    }

    IP_FAST_CONGESTION;

    /*
     * remove the Token Thing header and queue or discard message
     */
    srcptr->argreg = (CTR_RECEIVE_OFFSET + btow(TRING_SNAP_ENCAPBYTES) + input->rxrifwordlen);
    srcptr->cmdreg = MCI_CMD_RX_SELECT;
    len -= (TRING_SNAP_ENCAPBYTES + wtob(input->rxrifwordlen));

    IP_FAST_FINISH(ISO2_OVERHEAD + input->rxrifwordlen)
}

boolean hdlc_ip_fastswitch (hwidbtype *input)
{

    IP_FAST_STARTUP(srcptr->readlong)

    /*
     * Don't do the RX_MOVE if the datagram exceeds the
     * maximum output interface datagram size.
     */

    if (base_len > cptr->idb->ip_mtu + cptr->length) {
	input->mci_checked = TRUE;
	return (FALSE);
    }

    IP_FAST_STARTUP2
    /*
     * Force the Ethertype to 0x800 if going from Serial to anything.
     * Solves a problem with PPP & FR where the ethertype would stay at
     * 0x0021 or 0x3cc and get rejected when sent to other interfaces.
     */
    srcptr->argreg = MCI_OFFSETSIZE - 1;
    srcptr->cmdreg = MCI_CMD_TX1_SELECT;
    srcptr->write1short = TYPE_IP10MB;

    switch (output_interface) {
      case FS_ETHER:
	/*
	 * Input serial -- output ethernet.
	 */
	if (cptr->idb->isl_vlan != NULL) {

	    srcptr->argreg = (MCI_ETHER_OFFSET - ISL_INTERIOR_PACKET);
	    srcptr->cmdreg = MCI_CMD_TX1_SELECT;

	    ip_write_fast_ethernet_isl_vlan_header(cptr->m.maclong,
						   srcptr);
	    base_len += ISL_HEADER_LENGTH + ETHER_ARPA_ENCAPBYTES -
	      input->encsize;

	    /* packet by packet CRC toggle */

	    srcptr->argreg = (base_len | ISL_DO_CRC_MASK);
	    srcptr->cmdreg = MCI_CMD_TX1_START;

	    output->counters.tx_cumbytes_fast[ACCT_PROTO_IP] += base_len;
	    cptr->idb->isl_vlan->egressing_isl_packets[VLAN_ROUTE_IP]++;
	    return(TRUE);
	}

	srcptr->argreg = MCI_ETHER_OFFSET;
	srcptr->cmdreg = MCI_CMD_TX1_SELECT;
	ip_write_ether_header(srcptr, cptr);
	base_len += ETHER_ARPA_ENCAPBYTES - input->encsize;
	if (base_len < MINETHERSIZE)
	    base_len = MINETHERSIZE;
	srcptr->argreg = base_len;
	srcptr->cmdreg = MCI_CMD_TX1_START;
	output->counters.tx_cumbytes_fast[ACCT_PROTO_IP] += base_len;
	return(TRUE);
      case FS_FDDI:
	/*
	 * Input serial -- output fddi.
	 */
	if (srb_multiring_none_enabled(output->firstsw)) {
	    srcptr->argreg = (MCI_OFFSETSIZE - btow(FDDI_SNAP_ENCAPBYTES));
	    srcptr->cmdreg = MCI_CMD_TX1_SELECT;
	    ip_write_fddi_header_norif(srcptr, cptr);
	    base_len += (FDDI_SNAP_ENCAPBYTES - input->encsize);
	} else {
	    srcptr->argreg = (MCI_SERIAL_OFFSET +
			      btow(HDLC_ENCAPBYTES) -
			      (cptr->length/2 + 1));
	    srcptr->cmdreg = MCI_CMD_TX1_SELECT;
	    ip_write_fddi_header(srcptr, cptr);
	    base_len += ((1 + cptr->length) - input->encsize);
	}
	output->counters.tx_cumbytes_fast[ACCT_PROTO_IP] += base_len;
	base_len |= MCI_TX_ODDALIGN;
	srcptr->argreg = base_len;
	srcptr->cmdreg = MCI_CMD_TX1_START;
	return(TRUE);
      case FS_TOKEN:
	/*
	 * Input serial -- output token ring.
	 */
	srcptr->argreg = (MCI_SERIAL_OFFSET +
			  btow(HDLC_ENCAPBYTES) -
			  (cptr->length/2 + 1));
	srcptr->cmdreg = MCI_CMD_TX1_SELECT;
	ip_write_token_header(srcptr, cptr);
	base_len += ((2 + cptr->length) - input->encsize);
	srcptr->argreg = base_len;
	srcptr->cmdreg = MCI_CMD_TX1_START;
	output->counters.tx_cumbytes_fast[ACCT_PROTO_IP] += base_len;
	return(TRUE);
      case FS_HDLC:
      case FS_PPP:
	/*
	 * Input serial -- output serial hdlc/ppp.
	 */
	ip_common_serial_output(output, srcptr, cptr, base_len,
				MCI_OFFSETSIZE, input->encsize, FALSE, FALSE);
	return(TRUE);
      case FS_FR:
	/*
	 * Input serial -- output serial.
	 */
	ip_common_serial_output(output, srcptr, cptr, base_len,
				MCI_OFFSETSIZE, input->encsize, TRUE, FALSE);
	return(TRUE);
      case FS_SMDS:
	/*
	 * Input serial -- output serial SMDS.
	 */
	ip_write_smds_trailer(srcptr, (ushort *)&sm->smds_rsvd, sm_pad,
			      sm_lastlong, sm_pad_offset);
	ip_write_smds_header(srcptr, cptr, sm_head_offset);
	len += (sm_pad + 4);
	srcptr->argreg = len;
	srcptr->cmdreg = MCI_CMD_TX1_START;
	output->counters.tx_cumbytes_fast[ACCT_PROTO_IP] += len;
	return(TRUE);
      case FS_ATM_DXI:
	/*
	 * input serial -- output atm-dxi
	 */
	ip_common_serial_output(output, srcptr, cptr, base_len,
				MCI_OFFSETSIZE, input->encsize, FALSE, TRUE);
	return(TRUE);
      case FS_AIP:
	/*
	 * Input Serial(HDLC) -- output AIP.
	 */
	ATM_VCOUTFAST(output, cptr->m.macshort[0]);
	ATM_OUTFAST(output);
	base_len -= input->encsize;
	ip_write_aip_header(srcptr, cptr,
		MCI_OFFSETSIZE - btow(cptr->length), base_len);
	base_len += cptr->length;
	srcptr->argreg = base_len;
	srcptr->cmdreg = MCI_CMD_TX1_START;
	output->counters.tx_cumbytes_fast[ACCT_PROTO_IP] += base_len;
	return(TRUE);
      case FS_CHANNEL:
	/*
	 * Input serial -- output channel
	 */
	srcptr->argreg = MCI_OFFSETSIZE - btow(cptr->length);
	srcptr->cmdreg = MCI_CMD_TX1_SELECT;
	ip_write_channel_header(srcptr, cptr);
	base_len += cptr->length - input->encsize;
	srcptr->argreg = base_len;
	srcptr->cmdreg = MCI_CMD_TX1_START;
	output->counters.tx_cumbytes_fast[ACCT_PROTO_IP] += base_len;
	return(TRUE);
      case FS_LEX:
	/*
	 * Input serial -- output Lex.
	 */
	srcptr->argreg = MCI_OFFSETSIZE - btow(cptr->length);
	srcptr->cmdreg = MCI_CMD_TX1_SELECT;
	ip_write_lex_header(srcptr, cptr);
	base_len += cptr->length - input->encsize;
	srcptr->argreg = base_len;
	srcptr->cmdreg = MCI_CMD_TX1_START;
	output->counters.tx_cumbytes_fast[ACCT_PROTO_IP] += base_len;
	return(TRUE);

      default:
	break;
    }

    IP_FAST_CONGESTION;

    /*
     * select the offset for the ip header - which are the same for
     * HDLC, PPP, or Frame Relay
     */
    srcptr->argreg = MCI_OFFSETSIZE;
    srcptr->cmdreg = MCI_CMD_RX_SELECT;
    base_len -= (HDLC_ENCAPBYTES);

    IP_FAST_FINISH(0)
}

/*
 * smds_ip_fastswitch:	Fast switch incoming SMDS packets to other
 *	outputs.
 */
boolean smds_ip_fastswitch (hwidbtype *input)
{
    smds_pdb_t *smds_pdb;
    IP_FAST_STARTUP(srcptr->readlong)

    len = base_len - (input->encsize + 4);

    /*
     * Subtract out the pad length
     */
    smds_pdb = input->smds_pdb;
    if (smds_pdb->dximode)
	srcptr->argreg = input->rxoffset + 12;
    else
	srcptr->argreg = input->rxoffset + 11;
    srcptr->cmdreg = MCI_CMD_RX_SELECT;

    len -= ((srcptr->readshort >> 8) & SMDS_XHLPI_PL);
    smds_infolen = len;

    /*
     * Don't do the RX_MOVE if the datagram exceeds the
     * maximum output interface datagram size.
     * ACTUALLY:  The test below checks for 2 different cases:
     *	a) the outgoing dgram size > ip_mtu == process switching
     *  b) The outgoing mtu < 0 == DXI frame == process switching
     *  c) A derivative of (b) is that with unsigned vars, (b) will
     *		not be ZERO, but will in fact be a very large #.  In
     *		this case, we punt to process-switching where the DXI
     *		frame is handled properly.
     */
    if ((len <= 0) || (len > cptr->idb->ip_mtu)) {
	input->mci_checked = TRUE;
	return (FALSE);
    }

    /*
     * Note that we are using len as the adjusted number of bytes.
     * base_len will hold the number of input bytes so that
     * the input byte count is more correct
     */
    IP_FAST_STARTUP2

    switch (output_interface) {
      case FS_ETHER:
	/*
	 * Input serial -- output ethernet.
	 */

	if (cptr->idb->isl_vlan != NULL) {

	    srcptr->argreg = (MCI_ETHER_OFFSET - ISL_INTERIOR_PACKET);
	    srcptr->cmdreg = MCI_CMD_TX1_SELECT;

	    ip_write_fast_ethernet_isl_vlan_header(cptr->m.maclong,
						   srcptr);
	    len += ISL_HEADER_LENGTH + ETHER_ARPA_ENCAPBYTES;

	    /* packet by packet CRC toggle */

	    srcptr->argreg = (len | ISL_DO_CRC_MASK);
	    srcptr->cmdreg = MCI_CMD_TX1_START;

	    output->counters.tx_cumbytes_fast[ACCT_PROTO_IP] += len;
	    cptr->idb->isl_vlan->egressing_isl_packets[VLAN_ROUTE_IP]++;
	    return(TRUE);
	}

	srcptr->argreg = MCI_ETHER_OFFSET;
	srcptr->cmdreg = MCI_CMD_TX1_SELECT;
	ip_write_ether_header(srcptr, cptr);
	len += ETHER_ARPA_ENCAPBYTES;
	if (len < MINETHERSIZE)
	    len = MINETHERSIZE;
	srcptr->argreg = len;
	srcptr->cmdreg = MCI_CMD_TX1_START;
	output->counters.tx_cumbytes_fast[ACCT_PROTO_IP] += len;
	return(TRUE);

      case FS_FDDI:
	/*
	 * Input serial -- output fddi.
	 */
	srcptr->argreg = MCI_OFFSETSIZE - (btow(cptr->length) + 1);
	srcptr->cmdreg = MCI_CMD_TX1_SELECT;
	ip_write_fddi_header(srcptr, cptr);
	len += cptr->length + 1;
	output->counters.tx_cumbytes_fast[ACCT_PROTO_IP] += len;
	len |= MCI_TX_ODDALIGN;
	srcptr->argreg = len;
	srcptr->cmdreg = MCI_CMD_TX1_START;
	return(TRUE);

      case FS_HDLC:
      case FS_PPP:
	/*
	 * Input serial SMDS -- output serial HDLC/ppp.
	 */
	ip_common_serial_output(output, srcptr, cptr, len,
				MCI_OFFSETSIZE, 0, FALSE, FALSE);
	return(TRUE);
      case FS_FR:
	/*
	 * Input serial SMDS -- output serial frame relay
	 */
	 ip_common_serial_output(output, srcptr, cptr, len,
				 MCI_OFFSETSIZE, 0, TRUE, FALSE);
	 return(TRUE);
      case FS_SMDS:
	/*
	 * Input serial SMDS -- output serial SMDS.
	 */
	{
	    ip_write_smds_trailer(srcptr, (ushort *)&sm->smds_rsvd, sm_pad,
				  sm_lastlong, sm_pad_offset);
	    ip_write_smds_header(srcptr, cptr, sm_head_offset);
	    len += (sm_pad + 4);
	    srcptr->argreg = len;
	    srcptr->cmdreg = MCI_CMD_TX1_START;
	    output->counters.tx_cumbytes_fast[ACCT_PROTO_IP] += len;
	    return(TRUE);
	}
      case FS_ATM_DXI:
	/*
	 * Input serial smds -- output serial atm-dxi
	 */
	ip_common_serial_output(output, srcptr, cptr, len,
				MCI_OFFSETSIZE, 0, FALSE, TRUE);
	return(TRUE);
      case FS_TOKEN:
	/*
	 * Input SMDS -- output token ring.
	 */
	srcptr->argreg = MCI_OFFSETSIZE - (cptr->length/2 + 1);
	srcptr->cmdreg = MCI_CMD_TX1_SELECT;
	ip_write_token_header(srcptr, cptr);
	len += (cptr->length + 2);
	srcptr->argreg = len;
	srcptr->cmdreg = MCI_CMD_TX1_START;
	output->counters.tx_cumbytes_fast[ACCT_PROTO_IP] += len;
	return(TRUE);

      case FS_AIP:
	/*
	 * Input Serial(SMDS) -- output AIP.
	 */
	ATM_VCOUTFAST(output, cptr->m.macshort[0]);
	ATM_OUTFAST(output);
	ip_write_aip_header(srcptr, cptr,
			    MCI_OFFSETSIZE - btow(cptr->length), len);
	len += cptr->length;
	srcptr->argreg = len;
	srcptr->cmdreg = MCI_CMD_TX1_START;
	output->counters.tx_cumbytes_fast[ACCT_PROTO_IP] += len;
	return(TRUE);
	break;

      case FS_CHANNEL:
	/*
	 * Input SMDS -- output channel
	 */
	srcptr->argreg = MCI_OFFSETSIZE - btow(cptr->length);
	srcptr->cmdreg = MCI_CMD_TX1_SELECT;
	ip_write_channel_header(srcptr, cptr);
	len += cptr->length;  /* Note: this used to be base_len */
	srcptr->argreg = len;
	srcptr->cmdreg = MCI_CMD_TX1_START;
	output->counters.tx_cumbytes_fast[ACCT_PROTO_IP] += len;
	return(TRUE);

    case FS_LEX:
	/*
	 * Input SMDS -- output Lex.
	 */
	srcptr->argreg = MCI_OFFSETSIZE - btow(cptr->length);
	srcptr->cmdreg = MCI_CMD_TX1_SELECT;
	ip_write_lex_header(srcptr, cptr);
	len +=  cptr->length;
	srcptr->argreg = len;
	srcptr->cmdreg = MCI_CMD_TX1_START;
	output->counters.tx_cumbytes_fast[ACCT_PROTO_IP] += len;
	return(TRUE);

      default:
	break;
    }

    IP_FAST_CONGESTION;

    /*
     * HDLC, PPP, SMDS, or Frame relay
     */
    srcptr->argreg = MCI_OFFSETSIZE;
    srcptr->cmdreg = MCI_CMD_RX_SELECT;
    if (srcptr->rxtype == RXTYPE_UNKNOWN) {
        srcptr->argreg = MCI_OFFSETSIZE + 10;
        srcptr->cmdreg = MCI_CMD_RX_SELECT;
    }

    IP_FAST_FINISH(0)
}
static boolean atm_dxi_ip_fastswitch (hwidbtype *input)
{
    mciregtype *srcptr, *output_mci_regptr;
    ipcache_rntype *cptr;
    hwidbtype *output;
    shortlong dstadr;
    enum FSTYPE output_interface;
    ulong base_len;
    ulong len;
    len = 0;

    /*
     * RX_SETUP left read pointer at start of IP destination address,
     * so read it in now.  Given check out function and let MCI do its
     * thing while we do a cache lookup.  If the cache lookup fails or
     * the MCI says thumbs down, try to read it in by hand.
     *
     * If we read it in by hand, and it is correct, set the
     * rxtype to RXTYPE_IPUNC.  This indicates to other code that
     * we have to write the ip header also
     */
    srcptr = input->mci_regptr;
    dstadr.d.lword = srcptr->readlong;
    srcptr->cmdreg = MCI_CMD_RX_CHECK;
    cptr = ipcache_lookup_fast(dstadr.d.lword);

    atm_dxi_the_line = input->rxoffset +
	btow(input->atm_dxi_stuff->fast_input_encsize);
    if (srcptr->cmdreg != MCI_RSP_OKAY) {
	/*
	 * Set the rxtype to RXTYPE_IPUNC.  This indicates to
	 * ip_mci2mci that it has to re-write the ip header
	 */
	srcptr->argreg = atm_dxi_the_line;
	srcptr->cmdreg = MCI_CMD_RX_SELECT;
	if (ip_read_header_and_checksum(input,srcptr,&dstadr.d.lword)) {
	    cptr = ipcache_lookup_fast(dstadr.d.lword);
	} else {
	    return(FALSE);
	}
	input->rxtype = RXTYPE_IPUNC;
    }

    if (!cptr) {
	if (IPMULTICAST(dstadr.d.lword)) {
	    return(reg_invoke_ipmulticast_hes_fs(dstadr.d.lword, input));
	}
	input->counters.cache_miss[ACCT_PROTO_IP]++;
	return(FALSE);
    }

    output = cptr->idb->hwptr;
    output_interface = output->fast_switch_type;

    if (!output->ip_fast_flags) {
	if (output->ip_routecache & IP_CBUS_ROUTECACHE)
	    reg_invoke_auto_sw_ip_cache_update(cptr, NULL, dstadr.d.lword);

	/*
	 * Re entry point, if we pass output access checks and/or
	 * perform IP accounting for this output interface.
	 */
    resume_flow:
	base_len = input->rxlength;

    /*
     * Don't do the RX_MOVE if the datagram exceeds the
     * maximum output interface datagram size.
     */

    if (base_len > cptr->idb->ip_mtu + cptr->length) {
	input->mci_checked = TRUE;
	return (FALSE);
    }

    IP_FAST_STARTUP2

    /*
     * Force the Ethertype to 0x800 if going from Serial to anything.
     * Solves a problem with PPP & FR where the ethertype would stay at
     * 0x0021 or 0x3cc and get rejected when sent to other interfaces.
     *
     * If we have IPUNC type (which means above that we had to check the
     * header by hand, we write the ip header
     */
    srcptr->argreg = atm_dxi_the_line - 1;
    srcptr->cmdreg = MCI_CMD_TX1_SELECT;
    srcptr->write1short = TYPE_IP10MB;
    if (input->rxtype == RXTYPE_IPUNC) {
	write_new_iphdr(srcptr);
    }

    switch (output_interface) {
      case FS_ETHER:
	/*
	 * Input serial -- output ethernet.
	 */

	if (cptr->idb->isl_vlan != NULL) {

	    srcptr->argreg = (atm_dxi_the_line - ETHER_ARPA_ENCAPWORDS
			      - ISL_INTERIOR_PACKET);

	    srcptr->cmdreg = MCI_CMD_TX1_SELECT;

	    ip_write_fast_ethernet_isl_vlan_header(cptr->m.maclong,
						   srcptr);
	    base_len += ISL_HEADER_LENGTH + ETHER_ARPA_ENCAPBYTES -
	      input->atm_dxi_stuff->fast_input_encsize;

	    /* packet by packet CRC toggle */

	    srcptr->argreg = (base_len | ISL_DO_CRC_MASK);
	    srcptr->cmdreg = MCI_CMD_TX1_START;

	    output->counters.tx_cumbytes_fast[ACCT_PROTO_IP] += base_len;
	    cptr->idb->isl_vlan->egressing_isl_packets[VLAN_ROUTE_IP]++;
	    return(TRUE);
	}

	srcptr->argreg = atm_dxi_the_line - ETHER_ARPA_ENCAPWORDS;
	srcptr->cmdreg = MCI_CMD_TX1_SELECT;
	ip_write_ether_header(srcptr, cptr);
	base_len += ETHER_ARPA_ENCAPBYTES -
	    input->atm_dxi_stuff->fast_input_encsize;
	if (base_len < MINETHERSIZE)
	    base_len = MINETHERSIZE;
	srcptr->argreg = base_len;
	srcptr->cmdreg = MCI_CMD_TX1_START;
	output->counters.tx_cumbytes_fast[ACCT_PROTO_IP] += base_len;
	return(TRUE);
      case FS_FDDI:
	/*
	 * Input serial -- output fddi.
	 */
	srcptr->argreg = (atm_dxi_the_line - (cptr->length/2 +1));
	srcptr->cmdreg = MCI_CMD_TX1_SELECT;
	ip_write_fddi_header(srcptr, cptr);
	base_len += ((1 + cptr->length) -
  		     input->atm_dxi_stuff->fast_input_encsize);
	output->counters.tx_cumbytes_fast[ACCT_PROTO_IP] += base_len;
	base_len |= MCI_TX_ODDALIGN;
	srcptr->argreg = base_len;
	srcptr->cmdreg = MCI_CMD_TX1_START;
	return(TRUE);
      case FS_TOKEN:
	/*
	 * Input serial -- output token ring.
	 */
	srcptr->argreg = (atm_dxi_the_line - (cptr->length/2 + 1));
	srcptr->cmdreg = MCI_CMD_TX1_SELECT;
	ip_write_token_header(srcptr, cptr);
	base_len += ((2 + cptr->length) -
		     input->atm_dxi_stuff->fast_input_encsize);
	srcptr->argreg = base_len;
	srcptr->cmdreg = MCI_CMD_TX1_START;
	output->counters.tx_cumbytes_fast[ACCT_PROTO_IP] += base_len;
	return(TRUE);
      case FS_HDLC:
      case FS_PPP:
	/*
	 * Input serial -- output serial hdlc/ppp.
	 */
	ip_common_serial_output(output, srcptr, cptr, base_len,
				atm_dxi_the_line,
				input->atm_dxi_stuff->fast_input_encsize,
				FALSE, FALSE);
	return(TRUE);
      case FS_FR:
	/*
	 * Input serial -- output serial frame relay.
	 */
	ip_common_serial_output(output, srcptr, cptr, base_len,
				atm_dxi_the_line,
				input->atm_dxi_stuff->fast_input_encsize,
				TRUE, FALSE);
	return(TRUE);
      case FS_SMDS:
	/*
	 * Input serial -- output serial SMDS.
	 */
	ip_write_smds_trailer(srcptr, (ushort *)&sm->smds_rsvd, sm_pad,
			      sm_lastlong, sm_pad_offset);
	ip_write_smds_header(srcptr, cptr, sm_head_offset);
	len += (sm_pad + 4);
	srcptr->argreg = len;
	srcptr->cmdreg = MCI_CMD_TX1_START;
	output->counters.tx_cumbytes_fast[ACCT_PROTO_IP] += len;
	return(TRUE);
      case FS_ATM_DXI:
	ip_common_serial_output(output, srcptr, cptr, base_len,
				atm_dxi_the_line,
				input->atm_dxi_stuff->fast_input_encsize,
				FALSE, TRUE);
	return(TRUE);
      case FS_AIP:
	/*
	 * Input Serial -- output AIP.
	 */
	ATM_VCOUTFAST(output, cptr->m.macshort[0]);
	ATM_OUTFAST(output);
	base_len -= input->atm_dxi_stuff->fast_input_encsize;
	ip_write_aip_header(srcptr, cptr,
			    atm_dxi_the_line - btow(cptr->length),
			    base_len);
	base_len += cptr->length;
	srcptr->argreg = base_len;
	srcptr->cmdreg = MCI_CMD_TX1_START;
	output->counters.tx_cumbytes_fast[ACCT_PROTO_IP] += base_len;
	return(TRUE);
      case FS_CHANNEL:
	/*
	 * Input serial -- output channel
	 */
	srcptr->argreg = atm_dxi_the_line - btow(cptr->length);
	srcptr->cmdreg = MCI_CMD_TX1_SELECT;
	ip_write_channel_header(srcptr, cptr);
	base_len += cptr->length -
	    input->atm_dxi_stuff->fast_input_encsize;
	srcptr->argreg = base_len;
	srcptr->cmdreg = MCI_CMD_TX1_START;
	output->counters.tx_cumbytes_fast[ACCT_PROTO_IP] += base_len;
	return(TRUE);
      case FS_LEX:
	/*
	 * Input serial -- output Lex.
	 */
	srcptr->argreg = atm_dxi_the_line - btow(cptr->length);
	srcptr->cmdreg = MCI_CMD_TX1_SELECT;
	ip_write_lex_header(srcptr, cptr);
	base_len += cptr->length - input->atm_dxi_stuff->fast_input_encsize;
	srcptr->argreg = base_len;
	srcptr->cmdreg = MCI_CMD_TX1_START;
	output->counters.tx_cumbytes_fast[ACCT_PROTO_IP] += base_len;
	return(TRUE);

      default:
	break;
    }

    IP_FAST_CONGESTION;

    len -= input->atm_dxi_stuff->fast_input_encsize;
    base_len += ETHER_ARPA_ENCAPBYTES;
    if (input->rxtype == RXTYPE_IPUNC) {
	srcptr->argreg = atm_dxi_the_line + 10;
	srcptr->cmdreg = MCI_CMD_RX_SELECT;
	len -= 20;
    } else {
	srcptr->argreg = atm_dxi_the_line;
	srcptr->cmdreg = MCI_CMD_RX_SELECT;
    }

    IP_FAST_FINISH((atm_dxi_the_line - MCI_OFFSETSIZE))
}

/*
 * hdlc_ipunc_fastswitch:
 *	This routine will fastswitch any hdlc/ppp IP packet that wasn't
 *	classified by the micro code.  This is used in the frame relay
 *	case.
 */
boolean hdlc_ipunc_fastswitch (hwidbtype *input)
{
    mciregtype *srcptr, *output_mci_regptr;
    ipcache_rntype *cptr;
    hwidbtype *output;
    shortlong dstadr;
    ulong output_interface;
    ulong base_len;
    ulong len;

    len = 0;

    /*
     * Ok... Here's were we seperate the men from the mice.  We've
     * gotten into this code 'cuz we couldn't decipher an SMDS packet
     * proper.	This only happens (1) we got a bad packet, (2) we got
     * a packet from a CBUS I or Old MCI card.	In case (2) we cheat
     * here and take a mediocre fast path to yet determine if we can
     * switch this packet.
     *
     * Only IP packets are allowed to be fastswitched here!
     */

    /*
     * First we need to reset mci_checked to FALSE, as it may have been
     * set to TRUE if the last packet fell all the way through this routine
     * and returned TRUE, missing the reset to FALSE in MCI_SERIAL_RECEIVE.
     */
    srcptr = input->mci_regptr;
    srcptr->argreg = MCI_OFFSETSIZE;
    srcptr->cmdreg = MCI_CMD_RX_SELECT;
    if (ip_read_header_and_checksum(input, srcptr, &dstadr.d.lword)) {

	/*
	 * Look up the destination in our IP cache. We've discovered that
	 * we have an IP packet in our hands, now we check to see if we
	 * have a pre-cached destination dude in our cache. Fetch the whole
	 * IP packet onto our stack for interrogation. If the cache lookup
	 * fails and the packet's destination is an IP group address, call
	 * IP multicast fastswitching, otherwise bail to process level.
	 */
	cptr = ipcache_lookup_fast(dstadr.d.lword);
	if (!cptr) {
	    if (IPMULTICAST(dstadr.d.lword)) {
		return(reg_invoke_ipmulticast_hes_fs(dstadr.d.lword, input));
	    }
	    input->counters.cache_miss[ACCT_PROTO_IP]++;
	    return(FALSE);
	}
    } else {
	return(FALSE);
    }

    output = cptr->idb->hwptr;
    output_interface = output->fast_switch_type;

    if (!output->ip_fast_flags) {
	/*
	 * Re entry point, if we pass output access checks and/or
	 * perform IP accounting for this output interface.
	 */
    resume_flow:
	base_len = input->rxlength;

    /*
     * Don't do the RX_MOVE if the datagram exceeds the
     * maximum output interface datagram size.
     */

    if (base_len > cptr->idb->ip_mtu + HDLC_ENCAPBYTES)
	return (FALSE);

    IP_FAST_STARTUP2
    switch (output_interface) {
      case FS_ETHER:
	/*
	 * Input serial -- output ethernet.
	 */
	srcptr->argreg = MCI_OFFSETSIZE;
	srcptr->cmdreg = MCI_CMD_TX1_SELECT;
	write_new_iphdr(srcptr);

	if (cptr->idb->isl_vlan != NULL) {

	    srcptr->argreg = (MCI_ETHER_OFFSET - ISL_INTERIOR_PACKET);
	    srcptr->cmdreg = MCI_CMD_TX1_SELECT;

	    ip_write_fast_ethernet_isl_vlan_header(cptr->m.maclong,
						   srcptr);
	    base_len += ISL_HEADER_LENGTH + ETHER_ARPA_ENCAPBYTES -
	      input->encsize;

	    /* packet by packet CRC toggle */

	    srcptr->argreg = (base_len | ISL_DO_CRC_MASK);
	    srcptr->cmdreg = MCI_CMD_TX1_START;

	    output->counters.tx_cumbytes_fast[ACCT_PROTO_IP] += base_len;
	    cptr->idb->isl_vlan->egressing_isl_packets[VLAN_ROUTE_IP]++;
	    return(TRUE);
	}

	srcptr->argreg = MCI_ETHER_OFFSET;
	srcptr->cmdreg = MCI_CMD_TX1_SELECT;
	ip_write_ether_header(srcptr, cptr);
	srcptr->write1short = TYPE_IP10MB;
	base_len += ETHER_ARPA_ENCAPBYTES - input->encsize;
	if (base_len < MINETHERSIZE)
	    base_len = MINETHERSIZE;
	srcptr->argreg = base_len;
	srcptr->cmdreg = MCI_CMD_TX1_START;
	output->counters.tx_cumbytes_fast[ACCT_PROTO_IP] += base_len;
	return(TRUE);

      case FS_FDDI:
	/*
	 * Input serial -- output fddi.
	 */
	srcptr->argreg = MCI_OFFSETSIZE;
	srcptr->cmdreg = MCI_CMD_TX1_SELECT;
	write_new_iphdr(srcptr);
	if (srb_multiring_none_enabled(output->firstsw)) {
	    srcptr->argreg = MCI_OFFSETSIZE - FDDI_SNAP_ENCAPWORDS;
	    srcptr->cmdreg = MCI_CMD_TX1_SELECT;
	    ip_write_fddi_header_norif(srcptr, cptr);
	    base_len += (FDDI_SNAP_ENCAPBYTES - input->encsize);
	} else {
	    srcptr->argreg = (MCI_OFFSETSIZE - ((cptr->length/2) + 1));
	    srcptr->cmdreg = MCI_CMD_TX1_SELECT;
	    ip_write_fddi_header(srcptr, cptr);
	    base_len += ((1 + cptr->length) - input->encsize);
    	}
	srcptr->write1short = TYPE_IP10MB;
	output->counters.tx_cumbytes_fast[ACCT_PROTO_IP] += base_len;
	base_len |= MCI_TX_ODDALIGN;
	srcptr->argreg = base_len;
	srcptr->cmdreg = MCI_CMD_TX1_START;
	return(TRUE);

      case FS_TOKEN:
	/*
	 * Input serial -- output token ring.
	 */
	srcptr->argreg = MCI_OFFSETSIZE;
	srcptr->cmdreg = MCI_CMD_TX1_SELECT;
	write_new_iphdr(srcptr);
	srcptr->argreg = (MCI_OFFSETSIZE - ((cptr->length/2) + 1));
	srcptr->cmdreg = MCI_CMD_TX1_SELECT;
	ip_write_token_header(srcptr, cptr);
	srcptr->write1short = TYPE_IP10MB;
	base_len += ((2 + cptr->length) - input->encsize);
	srcptr->argreg = base_len;
	srcptr->cmdreg = MCI_CMD_TX1_START;
	output->counters.tx_cumbytes_fast[ACCT_PROTO_IP] += base_len;
	return(TRUE);

      case FS_HDLC:
      case FS_PPP:
	/*
	 * Input serial -- output serial HDLC/ppp.
	 */
	srcptr->argreg = MCI_OFFSETSIZE;
	srcptr->cmdreg = MCI_CMD_TX1_SELECT;
	write_new_iphdr(srcptr);
	ip_common_serial_output(output, srcptr, cptr, base_len,
				MCI_OFFSETSIZE, input->encsize,
				FALSE, FALSE);
	return(TRUE);
      case FS_FR:
	/*
	 * Input serial -- output serial frame relay.
	 */
	srcptr->argreg = MCI_OFFSETSIZE;
	srcptr->cmdreg = MCI_CMD_TX1_SELECT;
	write_new_iphdr(srcptr);
	ip_common_serial_output(output, srcptr, cptr, base_len,
				MCI_OFFSETSIZE, input->encsize,
				TRUE, FALSE);
	return(TRUE);

      case FS_SMDS:
	/*
	 * Input serial SMDS -- output serial SMDS.
	 */
	ip_write_smds_trailer(srcptr, (ushort *)&sm->smds_rsvd, sm_pad,
			      sm_lastlong, sm_pad_offset);
	srcptr->argreg = MCI_OFFSETSIZE;
	srcptr->cmdreg = MCI_CMD_TX1_SELECT;
	write_new_iphdr(srcptr);
	ip_write_smds_header(srcptr, cptr, sm_head_offset);
	len += (sm_pad + 4);
	srcptr->argreg = len;
	srcptr->cmdreg = MCI_CMD_TX1_START;
	output->counters.tx_cumbytes_fast[ACCT_PROTO_IP] += len;
	return(TRUE);

      case FS_ATM_DXI:
	srcptr->argreg = MCI_OFFSETSIZE;
	srcptr->cmdreg = MCI_CMD_TX1_SELECT;
	write_new_iphdr(srcptr);
	ip_common_serial_output(output, srcptr, cptr, base_len,
				MCI_OFFSETSIZE, input->encsize,
				FALSE, TRUE);
	return(TRUE);

      case FS_AIP:
	/*
	 * Input Serial(HDLC) -- output AIP.
	 */
	ATM_VCOUTFAST(output, cptr->m.macshort[0]);
	ATM_OUTFAST(output);
	srcptr->argreg = MCI_OFFSETSIZE;
	srcptr->cmdreg = MCI_CMD_TX1_SELECT;
	write_new_iphdr(srcptr);
	base_len -= input->encsize;
	ip_write_aip_header(srcptr, cptr,
			    MCI_OFFSETSIZE - btow(cptr->length),
			    base_len);
	base_len += cptr->length;
	srcptr->argreg = base_len;
	srcptr->cmdreg = MCI_CMD_TX1_START;
	output->counters.tx_cumbytes_fast[ACCT_PROTO_IP] += base_len;
	return(TRUE);
	break;

      case FS_CHANNEL:
	/*
	 * Input serial -- output channel
	 */
	srcptr->argreg = MCI_OFFSETSIZE;
	srcptr->cmdreg = MCI_CMD_TX1_SELECT;
	write_new_iphdr(srcptr);
	srcptr->argreg = MCI_CHANNEL_OFFSET;
	srcptr->cmdreg = MCI_CMD_TX1_SELECT;
	ip_write_channel_header(srcptr, cptr);
	srcptr->write1short = TYPE_IP10MB;
	base_len += CHANNEL_ENCAPBYTES - input->encsize;
	srcptr->argreg = base_len;
	srcptr->cmdreg = MCI_CMD_TX1_START;
	output->counters.tx_cumbytes_fast[ACCT_PROTO_IP] += base_len;
	return(TRUE);

      case FS_LEX:
	/*
	 * Input serial -- output Lex.
	 */
	srcptr->argreg = MCI_OFFSETSIZE;
	srcptr->cmdreg = MCI_CMD_TX1_SELECT;
	write_new_iphdr(srcptr);
	srcptr->argreg = MCI_OFFSETSIZE - btow(cptr->length);
	srcptr->cmdreg = MCI_CMD_TX1_SELECT;
	ip_write_lex_header(srcptr, cptr);
	base_len += cptr->length - input->encsize;
	srcptr->argreg = base_len;
	srcptr->cmdreg = MCI_CMD_TX1_START;
	output->counters.tx_cumbytes_fast[ACCT_PROTO_IP] += base_len;
	return(TRUE);
    }

    IP_FAST_CONGESTION;

    if (is_atm_dxi(input)) {
        base_len -= input->atm_dxi_stuff->fast_input_encsize;
        if (input->rxtype == RXTYPE_IPUNC) {
            srcptr->argreg = atm_dxi_the_line + 10;
            srcptr->cmdreg = MCI_CMD_RX_SELECT;
            base_len -= 20;
        } else {
            srcptr->argreg = atm_dxi_the_line;
            srcptr->cmdreg = MCI_CMD_RX_SELECT;
        }
    } else {
        /* HDLC, PPP, SMDS, or Frame relay */
        srcptr->argreg = MCI_OFFSETSIZE;
        srcptr->cmdreg = MCI_CMD_RX_SELECT;
        if (srcptr->rxtype == RXTYPE_UNKNOWN) {
            srcptr->argreg = MCI_OFFSETSIZE + 10;
            srcptr->cmdreg = MCI_CMD_RX_SELECT;
        }
    }

    IP_FAST_FINISH(0)
}

/*
 * smds_unknown_fastswitch:	Fast switch incoming SMDS packets to other
 *	outputs.
 *
 * WARNING:	This module is called for ALL RXTYPE_UNKNOWN packets
 *	that arrive over a serial interface with SMDS encapsulation.
 *	This necessary to attempt to fastswitch SMDS packets that are
 *	unclassifyable by the CBUS I & MCI controllers.	 CBUS II should
 *	classify the packet properly and force us to enter the proper
 *	smds_ip_faswswitch() path.
 *	In the meantime, this code was designed to emulate what the
 *	microcode would do to try to classify the incoming IP/SMDS
 *	packets.
 */
boolean smds_unknown_fastswitch (hwidbtype *input)
{
    mciregtype *srcptr, *output_mci_regptr;
    ipcache_rntype *cptr;
    hwidbtype *output;
    shortlong dstadr;
    ulong output_interface;
    ulong base_len;
    smds_pdb_t *smds_pdb;
    ulong len;

    len = 0;

    /*
     * Ok... Here's were we seperate the men from the mice.  We've
     * gotton into this code 'cuz we couldn't decipher an SMDS packet
     * proper.	This only happens (1) we got a bad packet, (2) we got
     * a packet from a CBUS I or Old MCI card.	In case (2) we cheat
     * here and take a mediocre fast path to yet determine if we can
     * switch this packet.
     * Case (3) - we are a MIP-SMDS card, and the line is not in place -
     * there was no way to classify the packet
     *
     * Only IP packets are allowed to be fastswitched here!
     *
     * sm_the_line will point to where the line is on the incoming
     * smds packets.  It won't be at MCI_OFFSETSIZE for the MIP.
     */
    sm_the_line = input->rxoffset + btow(input->encsize);

    /*
     * First we need to reset mci_checked to FALSE, as it may have been
     * set to TRUE if the last packet fell all the way through this routine
     * and returned TRUE, missing the reset to FALSE in MCI_SERIAL_RECEIVE.
     */

    srcptr = input->mci_regptr;
    /* Reject all non-IP packets */
    srcptr->argreg = sm_the_line - 1;
    srcptr->cmdreg = MCI_CMD_RX_SELECT;
    if (srcptr->readshort != TYPE_IP10MB)
	return(FALSE);
    if (ip_read_header_and_checksum(input, srcptr, &dstadr.d.lword)) {
	cptr = ipcache_lookup_fast(dstadr.d.lword);
	if (!cptr) {
	    if (IPMULTICAST(dstadr.d.lword)) {
		return(reg_invoke_ipmulticast_hes_fs(dstadr.d.lword, input));
	    }
	    input->counters.cache_miss[ACCT_PROTO_IP]++;
	    return(FALSE);
	}
    } else {
	return(FALSE);
    }

    output = cptr->idb->hwptr;
    output_interface = output->fast_switch_type;

    if (!output->ip_fast_flags) {
	/*
	 * Re entry point, if we pass output access checks and/or
	 * perform IP accounting for this output interface.
	 */
    resume_flow:
	base_len = input->rxlength;

    /*
     * Don't do the RX_MOVE if the datagram exceeds the
     * maximum output interface datagram size.
     */
    len = base_len - (input->encsize + 4);
    /*
     * Subtract out the pad length
     */
    smds_pdb = input->smds_pdb;
    srcptr->argreg = input->rxoffset + btow(smds_pdb->fast_input_modelen) + 10;
    srcptr->cmdreg = MCI_CMD_RX_SELECT;
    len -= ((srcptr->readshort >> 8) & SMDS_XHLPI_PL);

    /*
     * Don't do the RX_MOVE if the datagram exceeds the
     * maximum output interface datagram size.
     * ACTUALLY:  The test below checks for 2 different cases:
     *	a) the outgoing dgram size > ip_mtu == process switching
     *  b) The outgoing mtu < 0 == DXI frame == process switching
     *  c) A derivative of (b) is that with unsigned vars, (b) will
     *		not be ZERO, but will in fact be a very large #.  In
     *		this case, we punt to process-switching where the DXI
     *		frame is handled properly.
     */
    if ((len <= 0) || (len > cptr->idb->ip_mtu)) {
	return (FALSE);
    }
    smds_infolen = len;

    IP_FAST_STARTUP2

    switch (output_interface) {
      case FS_ETHER:
	/*
	 * Input serial -- output ethernet.
	 */
	srcptr->argreg = sm_the_line;
	srcptr->cmdreg = MCI_CMD_TX1_SELECT;
	write_new_iphdr(srcptr);
	if (cptr->idb->isl_vlan != NULL) {

	    srcptr->argreg = (sm_the_line -
			      btow(ETHER_ARPA_ENCAPBYTES) -
			      ISL_INTERIOR_PACKET);

	    srcptr->cmdreg = MCI_CMD_TX1_SELECT;

	    ip_write_fast_ethernet_isl_vlan_header(cptr->m.maclong,
						   srcptr);
	    len += ISL_HEADER_LENGTH + ETHER_ARPA_ENCAPBYTES;

	    /* packet by packet CRC toggle */

	    srcptr->argreg = (len | ISL_DO_CRC_MASK);
	    srcptr->cmdreg = MCI_CMD_TX1_START;

	    output->counters.tx_cumbytes_fast[ACCT_PROTO_IP] += len;
	    cptr->idb->isl_vlan->egressing_isl_packets[VLAN_ROUTE_IP]++;
	    return(TRUE);
	}
	srcptr->argreg = sm_the_line - btow(ETHER_ARPA_ENCAPBYTES);
	srcptr->cmdreg = MCI_CMD_TX1_SELECT;
	ip_write_ether_header(srcptr, cptr);
	len += ETHER_ARPA_ENCAPBYTES;
	if (len < MINETHERSIZE)
	    len = MINETHERSIZE;
	srcptr->argreg = len;
	srcptr->cmdreg = MCI_CMD_TX1_START;
	output->counters.tx_cumbytes_fast[ACCT_PROTO_IP] += len;
	return(TRUE);

      case FS_FDDI:
	/*
	 * Input serial -- output fddi.
	 */
	srcptr->argreg = sm_the_line;
	srcptr->cmdreg = MCI_CMD_TX1_SELECT;
	write_new_iphdr(srcptr);
	srcptr->argreg = sm_the_line - (cptr->length/2 + 1);
	srcptr->cmdreg = MCI_CMD_TX1_SELECT;
	ip_write_fddi_header(srcptr, cptr);
	len += (cptr->length + 2);
	output->counters.tx_cumbytes_fast[ACCT_PROTO_IP] += len;
	len |= MCI_TX_ODDALIGN;
	srcptr->argreg = len;
	srcptr->cmdreg = MCI_CMD_TX1_START;
	return(TRUE);

      case FS_HDLC:
      case FS_PPP:
	/*
	 * Input serial SMDS -- output serial HDLC/ppp
	 */
	srcptr->argreg = sm_the_line;
	srcptr->argreg = MCI_CMD_TX1_SELECT;
	write_new_iphdr(srcptr);
	ip_common_serial_output(output, srcptr, cptr, len,
				sm_the_line, 0, FALSE, FALSE);
	return(TRUE);
      case FS_LEX:
	/*
	 * Input serial -- output Lex.
	 */
	srcptr->argreg = sm_the_line;
	srcptr->cmdreg = MCI_CMD_TX1_SELECT;
	write_new_iphdr(srcptr);
	srcptr->argreg = sm_the_line - btow(LEX_ARPA_ENCAPBYTES);
	srcptr->cmdreg = MCI_CMD_TX1_SELECT;
	ip_write_lex_header(srcptr, cptr);
	len += LEX_ARPA_ENCAPBYTES - input->encsize;
	srcptr->argreg = len;
	srcptr->cmdreg = MCI_CMD_TX1_START;
	return(TRUE);
      case FS_FR:
	/*
	 * Input serial SMDS -- output serial frame-relay
	 */
	srcptr->argreg = sm_the_line;
	srcptr->argreg = MCI_CMD_TX1_SELECT;
	write_new_iphdr(srcptr);
	ip_common_serial_output(output, srcptr, cptr, len,
				sm_the_line, 0, TRUE, FALSE);
	return(TRUE);

      case FS_SMDS:
	/*
	 * Input serial	 -- output serial SMDS.
	 */
	{
	    ip_write_smds_trailer(srcptr, (ushort *)&sm->smds_rsvd, sm_pad,
				  sm_lastlong, sm_pad_offset);
	    srcptr->argreg = sm_the_line;
	    srcptr->cmdreg = MCI_CMD_TX1_SELECT;
	    write_new_iphdr(srcptr);
	    ip_write_smds_header(srcptr, cptr, sm_head_offset);
	    len += (sm_pad + 4);
	    srcptr->argreg = len;
	    srcptr->cmdreg = MCI_CMD_TX1_START;
	    output->counters.tx_cumbytes_fast[ACCT_PROTO_IP] += len;
	    return(TRUE);
	}
      case FS_ATM_DXI:
	/*
	 * Input serial SMDS -- output serial atm-dxi.
	 */
	srcptr->argreg = sm_the_line;
	srcptr->argreg = MCI_CMD_TX1_SELECT;
	write_new_iphdr(srcptr);
	ip_common_serial_output(output, srcptr, cptr, len,
				sm_the_line, 0, FALSE, TRUE);
	return(TRUE);

      case FS_TOKEN:
	/*
	 * Input SMDS -- output token ring.
	 */
	srcptr->argreg = sm_the_line;
	srcptr->cmdreg = MCI_CMD_TX1_SELECT;
	write_new_iphdr(srcptr);
	srcptr->argreg = sm_the_line - (cptr->length/2 + 1);
	srcptr->cmdreg = MCI_CMD_TX1_SELECT;
	ip_write_token_header(srcptr, cptr);
	len += (cptr->length + 2);
	srcptr->argreg = len;
	srcptr->cmdreg = MCI_CMD_TX1_START;
	output->counters.tx_cumbytes_fast[ACCT_PROTO_IP] += len;
	return(TRUE);

      case FS_AIP:
	/*
	 * Input SMDS -- output AIP.
	 */
	ATM_VCOUTFAST(output, cptr->m.macshort[0]);
	ATM_OUTFAST(output);
	srcptr->argreg = sm_the_line;
	srcptr->cmdreg = MCI_CMD_TX1_SELECT;
	write_new_iphdr(srcptr);
	ip_write_aip_header(srcptr, cptr,
			    sm_the_line - btow(cptr->length), len);
	len += cptr->length;
	srcptr->argreg = len;
	srcptr->cmdreg = MCI_CMD_TX1_START;
	output->counters.tx_cumbytes_fast[ACCT_PROTO_IP] += len;
	return(TRUE);
	break;

      case FS_CHANNEL:
	/*
	 * Input SMDS -- output channel
	 */
	srcptr->argreg = MCI_OFFSETSIZE;
	srcptr->cmdreg = MCI_CMD_TX1_SELECT;
	write_new_iphdr(srcptr);
	srcptr->argreg = MCI_OFFSETSIZE - btow(cptr->length);
	srcptr->cmdreg = MCI_CMD_TX1_SELECT;
	ip_write_channel_header(srcptr, cptr);
	/* note used to be base_len here also */
	len += cptr->length;
	srcptr->argreg = len;
	srcptr->cmdreg = MCI_CMD_TX1_START;
	output->counters.tx_cumbytes_fast[ACCT_PROTO_IP] += len;
	return(TRUE);
    }

    IP_FAST_CONGESTION;

    srcptr->argreg = MCI_OFFSETSIZE;
    srcptr->cmdreg = MCI_CMD_RX_SELECT;
    len -= (SMDS_SNAP_ENCAPBYTES + 2);
    if (srcptr->rxtype == RXTYPE_UNKNOWN) {
	srcptr->argreg = MCI_OFFSETSIZE + 10;
	srcptr->cmdreg = MCI_CMD_RX_SELECT;
	len -= 20;
    }

    IP_FAST_FINISH(0)
}

/*
 * aip_ip_fastswitch(input):	Called to fastswitch all frames received
 *	from an ATM interface.	The packet has already been classified
 *	as an IP frame.	 Here we simply try to find the next hop & go there.
 */

boolean aip_ip_fastswitch (hwidbtype *input)
{

    IP_FAST_STARTUP(input->sniff[SNIFFBASE+1].d.lword)

    {
	vc_info_t *vc;

	/* check for NULL vc done in CBUS_AIP_RECEIVE */

	vc = input->atm_db->fast_vc_info;

 	input->fast_rcvidb = vc->swidb;
	atm_vc_enclen = vc->encapsize;
	/*
	 * Don't do the RX_MOVE if the datagram exceeds the
	 * maximum output interface datagram size.
	 */
	base_len -= atm_vc_enclen;	/* Take out encapsulation stuff */

	if (base_len > cptr->idb->ip_mtu) {
	    input->mci_checked = TRUE;
	    return (FALSE);
	}
	vc->infast++;
    }
    ATM_INFAST(input);

    IP_FAST_STARTUP2

    switch (output_interface) {
      case FS_ETHER:
	/*
	 * -- output ethernet.
	 */
	srcptr->argreg = MCI_OFFSETSIZE - btow(cptr->length) +
	  ATM_OFF(input);
	srcptr->cmdreg = MCI_CMD_TX1_SELECT;

	if (cptr->idb->isl_vlan != NULL) {

	    ip_write_fast_ethernet_isl_vlan_header(cptr->m.maclong,
						   srcptr);
	    base_len += cptr->length;

	    /* packet by packet CRC toggle */

	    srcptr->argreg = (base_len | ISL_DO_CRC_MASK);
	    srcptr->cmdreg = MCI_CMD_TX1_START;

	    output->counters.tx_cumbytes_fast[ACCT_PROTO_IP] += base_len;
	    cptr->idb->isl_vlan->egressing_isl_packets[VLAN_ROUTE_IP]++;
	    return(TRUE);
	}

	ip_write_ether_header(srcptr, cptr);
	srcptr->write1short = TYPE_IP10MB;	/* In case of NLPID */
	base_len += cptr->length;
	if (base_len < MINETHERSIZE)
	    base_len = MINETHERSIZE;
	srcptr->argreg = base_len;
	srcptr->cmdreg = MCI_CMD_TX1_START;
	output->counters.tx_cumbytes_fast[ACCT_PROTO_IP] += base_len;
	return(TRUE);
      case FS_FDDI:
	/*
	 * -- output fddi.
	 */
	if (srb_multiring_none_enabled(output->firstsw)) {
	    srcptr->argreg = MCI_OFFSETSIZE - btow(FDDI_SNAP_ENCAPBYTES) +
		ATM_OFF(input);
	    srcptr->cmdreg = MCI_CMD_TX1_SELECT;
	    ip_write_fddi_header_norif(srcptr, cptr);
	    srcptr->write1short = TYPE_IP10MB;	/* In case of NLPID */
	    /*
	     * the next line of 'base_len += FDDI_SNAP_ENCAPBYTES'
	     * has the same effect as the 'base_len += (cptr->length +1)'.
	     * However, the former produces better optimised code.
	     */
	    base_len += FDDI_SNAP_ENCAPBYTES;
	} else {			/* RIF presence */
	    uchar len_in_bytes;
	    len_in_bytes = cptr->length;
	    srcptr->argreg = MCI_OFFSETSIZE - (btow(len_in_bytes)+1) +
	       ATM_OFF(input);
	    srcptr->cmdreg = MCI_CMD_TX1_SELECT;
	    ip_write_fddi_header(srcptr, cptr);
	    srcptr->write1short = TYPE_IP10MB;	/* In case of NLPID */
	    base_len += (len_in_bytes+1);
	}
	output->counters.tx_cumbytes_fast[ACCT_PROTO_IP] += base_len;
	base_len |= MCI_TX_ODDALIGN;
	srcptr->argreg = base_len;
	srcptr->cmdreg = MCI_CMD_TX1_START;
	return(TRUE);
      case FS_TOKEN:
	/*
	 * -- output token ring.
	 */
	srcptr->argreg = MCI_OFFSETSIZE - (btow(cptr->length) + 1) +
	    ATM_OFF(input);
	srcptr->cmdreg = MCI_CMD_TX1_SELECT;
	ip_write_token_header(srcptr, cptr);
	srcptr->write1short = TYPE_IP10MB;	/* In case of NLPID */
	base_len += ((cptr->length + 2));
	srcptr->argreg = base_len;
	srcptr->cmdreg = FCI_CMD_TX1_START;
	output->counters.tx_cumbytes_fast[ACCT_PROTO_IP] += base_len;
	return(TRUE);
      case FS_HDLC:
      case FS_PPP:
	/*
	 * -- output serial (hdlc/ppp)
	 */
	ip_common_serial_output(output, srcptr, cptr, base_len,
				MCI_OFFSETSIZE + ATM_OFF(input), 0,
				FALSE, FALSE);
	return(TRUE);
      case FS_FR:
	/*
	 * -- output serial (Frame-relay)
	 */
	ip_common_serial_output(output, srcptr, cptr, base_len,
				MCI_OFFSETSIZE + ATM_OFF(input), 0,
				TRUE, FALSE);
	return(TRUE);
      case FS_SMDS:	/* Untested, 3/27/94 P. Schrupp */
	/*
	 * -- output serial SMDS.
	 */
	{
	    ip_write_smds_trailer(srcptr, (ushort *)&sm->smds_rsvd, sm_pad,
				  sm_lastlong, sm_pad_offset);
	    ip_write_smds_header(srcptr, cptr, sm_head_offset);
	    len += (sm_pad + 4);
	    srcptr->argreg = len;
	    srcptr->cmdreg = MCI_CMD_TX1_START;
	    output->counters.tx_cumbytes_fast[ACCT_PROTO_IP] += len;
	    return(TRUE);
	}
      case FS_ATM_DXI:
	/*
	 * -- output atm-dxi
	 */
	ip_common_serial_output(output, srcptr, cptr, base_len,
				MCI_OFFSETSIZE + ATM_OFF(input), 0,
				FALSE, TRUE);
	return(TRUE);

      case FS_AIP:
	/*
	 * Input AIP -- output AIP.
	 */
	ATM_VCOUTFAST(output, cptr->m.macshort[0]);
	ATM_OUTFAST(output);
	ip_write_aip_header(srcptr, cptr, MCI_OFFSETSIZE -
			    btow(cptr->length) + ATM_OFF(input),
			    base_len);
	base_len += cptr->length;
	srcptr->argreg = base_len;
	srcptr->cmdreg = MCI_CMD_TX1_START;
	output->counters.tx_cumbytes_fast[ACCT_PROTO_IP] += base_len;
	return(TRUE);
	break;

      case FS_CHANNEL:
	/*
	 * Input AIP -- output channel
	 */
	srcptr->argreg = MCI_OFFSETSIZE - btow(cptr->length) +
	  ATM_OFF(input);
	srcptr->cmdreg = MCI_CMD_TX1_SELECT;
	ip_write_channel_header(srcptr, cptr);
	base_len += cptr->length;
	srcptr->argreg = base_len;
	srcptr->cmdreg = MCI_CMD_TX1_START;
	output->counters.tx_cumbytes_fast[ACCT_PROTO_IP] += base_len;
	return(TRUE);

    case FS_LEX:
	/*
	 * Input AIP -- output LEX.
	 */
	srcptr->argreg = MCI_OFFSETSIZE - btow(cptr->length) + ATM_OFF(input);
	srcptr->cmdreg = MCI_CMD_TX1_SELECT;
	ip_write_lex_header(srcptr, cptr);
	base_len += cptr->length;
	srcptr->argreg = base_len;
	srcptr->cmdreg = MCI_CMD_TX1_START;
	output->counters.tx_cumbytes_fast[ACCT_PROTO_IP] += base_len;
	return(TRUE);
      default:
	break;
    }

    IP_FAST_CONGESTION;

    srcptr->argreg = input->rxoffset + btow(atm_vc_enclen);
    srcptr->cmdreg = MCI_CMD_RX_SELECT;

    IP_FAST_FINISH(ATM_OFF(input))
}

/*
 * channel_ip_fastswitch
 */
boolean channel_ip_fastswitch (hwidbtype *input)
{
    IP_FAST_STARTUP(srcptr->readlong);

    /*
     * Don't do the RX_MOVE if the datagram exceeds the
     * maximum output interface datagram size.
     */

    if (base_len > cptr->idb->ip_mtu + CHANNEL_ENCAPBYTES) {
	input->mci_checked = TRUE;
	return(FALSE);
    }

    IP_FAST_STARTUP2

    switch (output_interface) {
      case FS_ETHER:
	/*
	 * Input channel - output ethernet.
	 */
	if (cptr->idb->isl_vlan != NULL) {

	    srcptr->argreg = (MCI_ETHER_OFFSET - ISL_INTERIOR_PACKET);
	    srcptr->cmdreg = MCI_CMD_TX1_SELECT;

	    ip_write_fast_ethernet_isl_vlan_header(cptr->m.maclong,
						   srcptr);
	    base_len += ISL_HEADER_LENGTH + ETHER_ARPA_ENCAPBYTES -
	      CHANNEL_ENCAPBYTES;

	    /* packet by packet CRC toggle */

	    srcptr->argreg = (base_len | ISL_DO_CRC_MASK);
	    srcptr->cmdreg = MCI_CMD_TX1_START;

	    output->counters.tx_cumbytes_fast[ACCT_PROTO_IP] += base_len;
	    cptr->idb->isl_vlan->egressing_isl_packets[VLAN_ROUTE_IP]++;
	    return(TRUE);
	}

	srcptr->argreg = MCI_ETHER_OFFSET;
	srcptr->cmdreg = MCI_CMD_TX1_SELECT;
	ip_write_ether_header(srcptr, cptr);
	base_len += ETHER_ARPA_ENCAPBYTES - CHANNEL_ENCAPBYTES;
	if (base_len < MINETHERSIZE)
	    base_len = MINETHERSIZE;
	srcptr->argreg = base_len;
	srcptr->cmdreg = MCI_CMD_TX1_START;
	output->counters.tx_cumbytes_fast[ACCT_PROTO_IP] += base_len;
	return(TRUE);
      case FS_FDDI:
	/*
	 * Input channel -- output fddi.
	 */
	srcptr->argreg = (MCI_CHANNEL_OFFSET +
			  btow(CHANNEL_ENCAPBYTES) -
 			  (cptr->length/2 + 1));
	srcptr->cmdreg = MCI_CMD_TX1_SELECT;
	ip_write_fddi_header(srcptr, cptr);
	base_len += ((cptr->length + 1) - CHANNEL_ENCAPBYTES);
	output->counters.tx_cumbytes_fast[ACCT_PROTO_IP] += base_len;
	base_len |= MCI_TX_ODDALIGN;
	srcptr->argreg = base_len;
	srcptr->cmdreg = MCI_CMD_TX1_START;
	return(TRUE);
      case FS_TOKEN:
	/*
	 * Input channel -- output token ring.
	 */
	srcptr->argreg = (MCI_CHANNEL_OFFSET +
			  btow(CHANNEL_ENCAPBYTES) -
			  (cptr->length/2 + 1));
	srcptr->cmdreg = MCI_CMD_TX1_SELECT;
	ip_write_token_header(srcptr, cptr);
	base_len += ((cptr->length + 2) - CHANNEL_ENCAPBYTES);
	srcptr->argreg = base_len;
	srcptr->cmdreg = FCI_CMD_TX1_START;
	output->counters.tx_cumbytes_fast[ACCT_PROTO_IP] += base_len;
	return(TRUE);
      case FS_HDLC:
      case FS_PPP:
	/*
	 * Input channel -- output serial (hdlc/ppp)
	 */
	ip_common_serial_output(output, srcptr, cptr, base_len,
				MCI_OFFSETSIZE, CHANNEL_ENCAPBYTES,
				FALSE, FALSE);
	return(TRUE);
      case FS_FR:
	/*
	 * Input channel -- output serial (frame-relay)
	 */
	ip_common_serial_output(output, srcptr, cptr, base_len,
				MCI_OFFSETSIZE, CHANNEL_ENCAPBYTES,
				TRUE, FALSE);
	return(TRUE);
      case FS_SMDS:
	/*
	 * Input channel -- output serial SMDS.
	 */
	{
	    ip_write_smds_trailer(srcptr, (ushort *)&sm->smds_rsvd, sm_pad,
				  sm_lastlong, sm_pad_offset);
	    ip_write_smds_header(srcptr, cptr, sm_head_offset);
	    len += sm_pad + 4;
	    srcptr->argreg = len;
	    srcptr->cmdreg = MCI_CMD_TX1_START;
	    output->counters.tx_cumbytes_fast[ACCT_PROTO_IP] += len;
	    return(TRUE);
	}
      case FS_ATM_DXI:
	/*
	 * Input channel -- output atm-dxi
	 */
	ip_common_serial_output(output, srcptr, cptr, base_len,
				MCI_OFFSETSIZE, CHANNEL_ENCAPBYTES,
				FALSE, TRUE);
	return(TRUE);
      case FS_AIP:
	/*
	 * Input channel -- output AIP.
	 */
	ATM_VCOUTFAST(output, cptr->m.macshort[0]);
	ATM_OUTFAST(output);
	base_len -= input->encsize;
	ip_write_aip_header(srcptr, cptr,
			    MCI_OFFSETSIZE - btow(cptr->length),
			    base_len);
	base_len += cptr->length;
	srcptr->argreg = base_len;
	srcptr->cmdreg = MCI_CMD_TX1_START;
	output->counters.tx_cumbytes_fast[ACCT_PROTO_IP] += base_len;
	return(TRUE);
	break;

      case FS_CHANNEL:
	/*
	 * Input channel -- output channel
	 */
	srcptr->argreg = MCI_CHANNEL_OFFSET;
	srcptr->cmdreg = MCI_CMD_TX1_SELECT;
	ip_write_channel_header(srcptr, cptr);
	srcptr->argreg = base_len;
	srcptr->cmdreg = MCI_CMD_TX1_START;
	output->counters.tx_cumbytes_fast[ACCT_PROTO_IP] += base_len;
	return(TRUE);

      case FS_LEX:
	/*
	 * Input channel - output Lex.
	 */
	srcptr->argreg = MCI_OFFSETSIZE - btow(cptr->length);
	srcptr->cmdreg = MCI_CMD_TX1_SELECT;
	ip_write_lex_header(srcptr, cptr);
	srcptr->argreg = base_len + cptr->length - CHANNEL_ENCAPBYTES;
	srcptr->cmdreg = MCI_CMD_TX1_START;
	return(TRUE);

      default:
	break;
    }

    IP_FAST_CONGESTION;

    srcptr->argreg = MCI_CHANNEL_OFFSET + btow(CHANNEL_ENCAPBYTES);
    srcptr->cmdreg = MCI_CMD_RX_SELECT;
    len -= (CHANNEL_ENCAPBYTES);

    IP_FAST_FINISH(0)
}

extern ushort fastiphdrchk(ushort *, ulong);

boolean lex_ip_fastswitch (hwidbtype *input)
{
    mciregtype *srcptr, *output_mci_regptr;
    shortlong dstadr;
    ipcache_rntype *cptr;
    hwidbtype *output;
    enum FSTYPE output_interface;
    ulong *off;
    ulong base_len;
    ulong len;

    len = 0;

    /*
     * Set the RX to point to the IP header.
     */
    srcptr = input->mci_regptr;
    srcptr->argreg = LEX_IP_OFFSET;
    srcptr->cmdreg = MCI_CMD_RX_SELECT;

    /*
     * Read the IP header.
     */
    off = (ulong *) &temp_iphdr;
    *off++ = srcptr->readlong;
    *off++ = srcptr->readlong;
    *off++ = srcptr->readlong;
    *off++ = srcptr->readlong;
    *off++ = srcptr->readlong;

    dstadr.d.lword = temp_iphdr.dstadr;
    cptr = ipcache_lookup_fast(dstadr.d.lword);
    if (!cptr) {
	input->counters.cache_miss[ACCT_PROTO_IP]++;
	return(FALSE);
    }

    if (fastiphdrchk((ushort *)&temp_iphdr, dstadr.d.lword))
	return(FALSE);

    input->mci_checked = TRUE;

    output = cptr->idb->hwptr;
    output_interface = output->fast_switch_type;

    if (!output->ip_fast_flags) {
	if (output->ip_routecache & IP_CBUS_ROUTECACHE)
	    reg_invoke_auto_sw_ip_cache_update(cptr, NULL, dstadr.d.lword);


    resume_flow:

    /*
     * rxlength includes:
     *	MIP Header	2 bytes (optional)
     *	PPP Header	4 bytes.
     *	LEX Control	2 bytes.
     *	Ethernet Frame	60 bytes.
     * Note: the ethernet FCS is not included because it is not
     *	 sent across the link.
     *
     * The length of the pdu is calculated as follows:
     *	rxlength - PPP header - LEX Control - Ethernet Header.
     *
     * Check that the payload comming in will fit in the mtu going
     * out.  This assumes that the mtu is calculated such that the
     * mtu + output headers will not cause a problem.
     */
    base_len = input->rxlength;
    if (base_len - LEX_ARPA_ENCAPBYTES > cptr->idb->ip_mtu)
	return(FALSE);

	IP_FAST_STARTUP2

	/*
	 * Point to the start of the IP header.
	 */
	srcptr->argreg = LEX_IP_OFFSET;
	srcptr->cmdreg = MCI_CMD_TX1_SELECT;
	/*
	 * Write out the new IP header.
	 */
	write_new_iphdr(srcptr);

	switch (output_interface) {
	case FS_ETHER:
	    /*
	     * Input Lex -- output Ethernet.
	     */
	    if (cptr->idb->isl_vlan != NULL) {
	    
	        srcptr->argreg = (LEX_IP_OFFSET -
				  btow(ETHER_ARPA_ENCAPBYTES) -
				  ISL_INTERIOR_PACKET);

		srcptr->cmdreg = MCI_CMD_TX1_SELECT;
		
		ip_write_fast_ethernet_isl_vlan_header(cptr->m.maclong,
						       srcptr);
		base_len -= (LEX_ARPA_ENCAPBYTES -
			     ETHER_ARPA_ENCAPBYTES -
			     ISL_HEADER_LENGTH);

		/* packet by packet CRC toggle */

		srcptr->argreg = (base_len | ISL_DO_CRC_MASK);
		srcptr->cmdreg = MCI_CMD_TX1_START;
	      
		output->counters.tx_cumbytes_fast[ACCT_PROTO_IP] += base_len;
		cptr->idb->isl_vlan->egressing_isl_packets[VLAN_ROUTE_IP]++;
		return(TRUE);
	    }

	    srcptr->argreg = LEX_IP_OFFSET - btow(ETHER_ARPA_ENCAPBYTES);
	    srcptr->cmdreg = MCI_CMD_TX1_SELECT;
	    ip_write_ether_header(srcptr, cptr);
	    /*
	     * Calculate number of bytes to write.
	     */
	    base_len -= (LEX_ARPA_ENCAPBYTES - ETHER_ARPA_ENCAPBYTES);
	    srcptr->argreg = base_len;
	    srcptr->cmdreg = MCI_CMD_TX1_START;
	    output->counters.tx_cumbytes_fast[ACCT_PROTO_IP] += base_len;
	    return(TRUE);

	case FS_TOKEN:
	    /*
	     * Input Lex -- output Token Ring.
	     */
	    srcptr->argreg = LEX_IP_OFFSET - btow(TRING_SNAP_ENCAPBYTES);
	    srcptr->cmdreg = MCI_CMD_TX1_SELECT;
	    ip_write_token_header(srcptr, cptr);
	    /*
	     * Calculate number of bytes to write.
	     */
	    base_len -= (LEX_ARPA_ENCAPBYTES - TRING_SNAP_ENCAPBYTES);
	    srcptr->argreg = base_len;
	    srcptr->cmdreg = MCI_CMD_TX1_START;
	    output->counters.tx_cumbytes_fast[ACCT_PROTO_IP] += base_len;
	    return(TRUE);

	case FS_FDDI:
	    /*
	     * Input Lex -- output Fddi
	     */
	    srcptr->argreg = LEX_IP_OFFSET - btow(FDDI_SNAP_ENCAPBYTES);
	    srcptr->cmdreg = MCI_CMD_TX1_SELECT;
	    ip_write_fddi_header(srcptr, cptr);
	    base_len -= (LEX_ARPA_ENCAPBYTES - FDDI_SNAP_ENCAPBYTES);
	    output->counters.tx_cumbytes_fast[ACCT_PROTO_IP] += base_len;
	    base_len |= MCI_TX_ODDALIGN;
	    srcptr->argreg = base_len;
	    srcptr->cmdreg = MCI_CMD_TX1_START;
	    return(TRUE);

	case FS_HDLC:
	case FS_PPP:
	    /*
	     * Input Lex -- output serial (hdlc/ppp)
	     */
	    ip_common_serial_output(output, srcptr, cptr, base_len,
				    LEX_IP_OFFSET, LEX_ARPA_ENCAPBYTES,
				    FALSE, FALSE);
	    return(TRUE);
	case FS_FR:
	    /*
	     * Input Lex -- output serial (frame-relay)
	     */
	    ip_common_serial_output(output, srcptr, cptr, base_len,
				    LEX_IP_OFFSET, LEX_ARPA_ENCAPBYTES,
				    TRUE, FALSE);
	    return(TRUE);
	case FS_ATM_DXI:
	    /*
	     * Input Lex -- output atm-dxi
	     */
	    ip_common_serial_output(output, srcptr, cptr, base_len,
				    LEX_IP_OFFSET, LEX_ARPA_ENCAPBYTES,
				    FALSE, TRUE);
	    return(TRUE);
	case FS_SMDS:
	    /*
	     * Input Lex -- output serial SMDS.
	     */
	    ip_write_smds_trailer(srcptr, (ushort *)&sm->smds_rsvd,
				  sm_pad, sm_lastlong, sm_pad_offset);
	    ip_write_smds_header(srcptr, cptr, sm_head_offset);
	    len += (sm_pad + 4);
	    srcptr->argreg = len;
	    srcptr->cmdreg = MCI_CMD_TX1_START;
	    output->counters.tx_cumbytes_fast[ACCT_PROTO_IP] += len;
	    return(TRUE);

	case FS_CHANNEL:
	    /*
	     * Input Lex -- output channel.
	     */
	    srcptr->argreg = LEX_IP_OFFSET - btow(cptr->length);
	    srcptr->cmdreg = MCI_CMD_TX1_SELECT;
	    ip_write_channel_header(srcptr, cptr);
	    base_len -= ( LEX_ARPA_ENCAPBYTES - cptr->length);
	    srcptr->argreg = base_len;
	    srcptr->cmdreg = MCI_CMD_TX1_START;
	    output->counters.tx_cumbytes_fast[ACCT_PROTO_IP] += base_len;
	    return(TRUE);

	case FS_LEX:
	    /*
	     * Input Lex -- output Lex.
	     */
	    srcptr->argreg = LEX_IP_OFFSET - btow(cptr->length);
	    srcptr->cmdreg = MCI_CMD_TX1_SELECT;
	    ip_write_lex_header(srcptr, cptr);
	    /*
	     * Calculate number of bytes to write.
	     */
	    base_len -= (LEX_ARPA_ENCAPBYTES - cptr->length);
	    srcptr->argreg = base_len;
	    srcptr->cmdreg = MCI_CMD_TX1_START;
	    output->counters.tx_cumbytes_fast[ACCT_PROTO_IP] += base_len;
	    return(TRUE);

	case FS_AIP:
	    /*
	     * Input Lex -- output AIP.
	     */
	    ATM_VCOUTFAST(output, cptr->m.macshort[0]);
	    ATM_OUTFAST(output);
	    base_len -= LEX_ARPA_ENCAPBYTES;
	    ip_write_aip_header(srcptr, cptr,
				LEX_IP_OFFSET - btow(cptr->length), base_len);
	    /*
	     * Calculate number of bytes to write.
	     */
	    base_len += cptr->length;
	    srcptr->argreg = base_len;
	    srcptr->cmdreg = MCI_CMD_TX1_START;
	    output->counters.tx_cumbytes_fast[ACCT_PROTO_IP] += base_len;
	    return(TRUE);

	default:
	    break;
	}

	IP_FAST_CONGESTION;

	/*
	 * Move the read pointer to point at the first byte of
	 * IP data (just passed the IP header).
	 */
	srcptr->argreg = LEX_IP_OFFSET + btow(LEX_IP_HEADER_LEN);
	srcptr->cmdreg = MCI_CMD_RX_SELECT;
	len -= (LEX_ARPA_ENCAPBYTES + LEX_IP_HEADER_LEN);

	IP_FAST_FINISH(0);
}

/*
 * tunnel_ip_fastswitch
 *
 * Fastswitch IP tunnels with GRE and IPINIP encapsulation. Initially,
 * this checks for IP multicast packets and dispatches to the multicast
 * fastswitching functions.
 */
boolean tunnel_ip_fastswitch (hwidbtype *input)
{
    mciregtype  *regptr;
    iphdrtype   *ip;
    tunnel_info *tinfo;
    boolean     status;
    ulong       junk;
    vc_info_t *vc;

    regptr = input->mci_regptr;

    switch (input->fast_switch_type) {
      case FS_FDDI:
	regptr->argreg = FDDI_RECEIVE_OFFSET + FDDI_SNAP_ENCAPWORDS;
	break;
      case FS_TOKEN:
	regptr->argreg = CTR_RECEIVE_OFFSET + TRING_SNAP_ENCAPWORDS;
	break;
      case FS_ETHER:
      case FS_HDLC:
      case FS_PPP:
      case FS_FR:
	regptr->argreg = MCI_OFFSETSIZE;
	break;
      case FS_AIP:
	regptr->argreg = input->rxoffset;
	regptr->cmdreg = MCI_CMD_RX_SELECT;
	vc = atm_getvc(input->atm_db, regptr->readshort);
	if (!vc) return(FALSE);
	regptr->argreg = input->rxoffset + btow(vc->encapsize);
	break;
      default:
	return(FALSE);
    }

    regptr->cmdreg = MCI_CMD_RX_SELECT;
    /*
     * Read the rest of the IP header.
     */
    ip = (iphdrtype *) &input->sniff[SNIFFBASE+3];
    input->sniff[SNIFFBASE+3].d.lword = regptr->readlong;
    input->sniff[SNIFFBASE+4].d.lword = regptr->readlong;
    input->sniff[SNIFFBASE+5].d.lword = regptr->readlong;
    input->sniff[SNIFFBASE+6].d.lword = regptr->readlong;
    input->sniff[SNIFFBASE+7].d.lword = regptr->readlong;

    /*
     * Check to see if there is a tunnel configured for source.
     */
    FOR_ALL_TUNNELS(tinfo) {
 	if (tinfo->hwidb->state != IDBS_UP) continue;
 	if (ip->prot != tinfo->protocol) continue;
 	if (ip->srcadr != tinfo->destination) continue;
 	if (ip->dstadr == tinfo->source || (tinfo->source_idb && 
	    ip->dstadr == tinfo->source_idb->ip_address)) {
  	    break;
 	}
    }
    if (!tinfo)
	return(FALSE);

    /*
     * Skip passed GRE header.
     */
    if (tinfo->mode == TUN_MODE_GRE_IP) {

	/*
	 * If there is any chance that the incoming packet 
	 * has to be decrypted, then punt this packet to 
	 * process level, since we don't decrypt inline, 
	 * but do so at process level.
	 */
	if (input->crypto_vectors)
	    return (FALSE);
	junk = regptr->readlong;
    }

    /*
     * Get destination IP address from inner header.
     */
    ip = (iphdrtype *) &input->sniff[SNIFFBASE+3];
    input->sniff[SNIFFBASE+3].d.lword = regptr->readlong;
    input->sniff[SNIFFBASE+4].d.lword = regptr->readlong;
    input->sniff[SNIFFBASE+5].d.lword = regptr->readlong;
    input->sniff[SNIFFBASE+6].d.lword = regptr->readlong;
    input->sniff[SNIFFBASE+7].d.lword = regptr->readlong;

    /*
     * RXTYPE_IP_TUNNEL tells the multicast code how much it has to skip
     * over to RX_MOVE into a transmit interface. It will calculate the
     * input encapsulation length and will add to it sizeof(iphdrtype) if
     * input->rxtype is RXTYPE_IP_TUNNEL.
     */
    if (IPMULTICAST(ip->dstadr) && ip->prot != IGMP_PROT) {

	/*
	 * The multicast fastswitch code assumes fixed length header.
	 */
	if (ipheadsize(ip) != MINIPHEADERBYTES)
	    return(FALSE);

	/*
	 * The next two hwidbtype fields are used so the multicast
	 * fastswitching code knows the packet came in on a tunnel.
	 * The real hwidb has to also be passed in so we know what the
	 * real encapsulation is. Note, use of input->tunnel_swidb assumes
	 * no context break.
	 */
	input->rxtype = RXTYPE_IP_TUNNEL;
	input->tunnel_swidb = tinfo->hwidb->firstsw;
	status = reg_invoke_ipmulticast_hes_fs(ip->dstadr, input);
	input->tunnel_swidb = NULL;
	return(status);
    }
    return(FALSE);
}

/*
 * ip_mci_fs_register_drivers
 * Note that the frame relay support handles the installation
 * of IP and other switching drivers
 */

static void ip_mci_fs_register_drivers (ulong flags, hwidbtype* idb)
{
    /*
     * Check to see if we're configuring IP
     */
    if (!(flags & FSF_DODIP))
	return;

    if (idb->fast_switch_flag & FSF_DODIP) {
	/*
	 * No fast-switching if NAT is on.
	 */
	if (idb->ip_fast_flags & IP_FAST_NAT) {
	    idb->ip_routecache &= ~IP_FAST_ROUTECACHE;
	}

	reg_add_ether_fs(RXTYPE_DODIP,
			 (service_ether_fs_type)ether_ip_fastswitch,
			     "ether_ip_fastswitch");
	reg_add_token_fs(RXTYPE_DODIP,
			 (service_token_fs_type)token_ip_fastswitch,
			     "token_ip_fastswitch");
	reg_add_fddi_fs(RXTYPE_DODIP,
			(service_fddi_fs_type)fddi_ip_fastswitch,
			    "fddi_ip_fastswitch");
	reg_add_hdlc_fs(RXTYPE_DODIP,
			(service_hdlc_fs_type)hdlc_ip_fastswitch,
			    "hdlc_ip_fastswitch");
	reg_add_ppp_fs(RXTYPE_IPUNC,
			(service_ppp_fs_type)hdlc_ipunc_fastswitch,
			    "ppp_unknown_fastswitch");
	reg_add_ppp_fs(RXTYPE_DODIP,
		       (service_ppp_fs_type)hdlc_ip_fastswitch,
			   "hdlc_ip_fastswitch");
	reg_add_smds_fs(RXTYPE_DODIP,
			(service_smds_fs_type)smds_ip_fastswitch,
			    "smds_ip_fastswitch");
	reg_add_smds_fs(RXTYPE_UNKNOWN,
			(service_smds_fs_type)smds_unknown_fastswitch,
			    "smds_unknown_fastswitch");
	reg_add_framerelay_fs(RXTYPE_DODIP,
			      (service_framerelay_fs_type)hdlc_ip_fastswitch,
			      "hdlc_ip_fastswitch");
	reg_add_framerelay_fs(RXTYPE_IPUNC,
			      (service_framerelay_fs_type)
			      hdlc_ipunc_fastswitch,
			      "hdlc_ipunc_fastswitch");
	reg_add_atm_dxi_fs(RXTYPE_DODIP,
			   (service_atm_dxi_fs_type)atm_dxi_ip_fastswitch,
			   "atm_dxi_ip_fastswitch");
	reg_add_atm_dxi_fs(RXTYPE_SNAP_DODIP,
			   (service_atm_dxi_fs_type)atm_dxi_ip_fastswitch,
			   "atm_dxi_ip_fastswitch");
	reg_add_aip_fs(RXTYPE_DODIP,
			(service_aip_fs_type)aip_ip_fastswitch,
			    "aip_unknown_fastswitch");
	reg_add_channel_fs(RXTYPE_DODIP,
			   (service_channel_fs_type)channel_ip_fastswitch,
			   "channel_ip_fastswitch");
	reg_add_lex_fs(RXTYPE_DODIP,
		       (service_lex_fs_type)lex_ip_fastswitch,
		       "lex_ip_fastswitch");
	reg_add_fast_fragmentation(RXTYPE_JUMBO_IP,
				   (service_fast_fragmentation_type)
				   ip_fast_fragmentation,
				   "ip_fast_fragmentation");
	reg_add_tunnel_ip_hes_fs(
		    (service_tunnel_ip_hes_fs_type)tunnel_ip_fastswitch, 
		    "tunnel_ip_fastswitch");
    } else {
	fs_table_fill((fastswitch_type)return_false, "return_false",
		      RXTYPE_DODIP);
	fs_table_fill((fastswitch_type)return_false, "return_false",
		      RXTYPE_SNAP_DODIP);
	reg_add_smds_fs(RXTYPE_UNKNOWN,
			(service_smds_fs_type)return_false,
			    "return_false");
	reg_add_fast_fragmentation(RXTYPE_JUMBO_IP,
				   (service_fast_fragmentation_type)
				   return_false, "return_false");
    }
}

static boolean ip_mci_fs_fast_supported (int linktype, hwidbtype *hwidb)
{
    if (!hwidb || linktype != LINK_IP)
	return(FALSE);

    if (hwidb->type == IDBTYPE_VTY_ASYNC || is_tunnel(hwidb) ||
	is_multilink(hwidb))
	return(FALSE);

    return(TRUE);
}

static void ip_mci_fs_init (subsystype* subsys)
{
    reg_add_register_drivers(ip_mci_fs_register_drivers,
			     "ip_mci_fs_register_drivers");
    udp_fast_flood_init();
    reg_add_udp_fast_flood(udp_fast_flood, "udp_fast_flood");

    reg_add_fast_supported(ip_mci_fs_fast_supported,
			   "ip_mci_fs_fast_supported");

}

/*
 * MCI FS subsystem header
 */

#define IPMCIFS_MAJVERSION	1
#define IPMCIFS_MINVERSION	0
#define IPMCIFS_EDITVERSION	1

SUBSYS_HEADER(ip_mci_fs, IPMCIFS_MAJVERSION, IPMCIFS_MINVERSION,
	      IPMCIFS_EDITVERSION, ip_mci_fs_init, SUBSYS_CLASS_PROTOCOL,
	      subsys_seq_iphost,
	      "req: fastswitch, iphost");

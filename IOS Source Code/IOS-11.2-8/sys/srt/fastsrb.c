/* $Id: fastsrb.c,v 3.6.10.10 1996/08/16 19:54:44 dujihara Exp $
 * $Source: /release/112/cvs/Xsys/srt/fastsrb.c,v $
 *------------------------------------------------------------------
 * Fast switching routines for srb and rsrb - platform independent
 *
 * February 1995, Bob O'Grady
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: fastsrb.c,v $
 * Revision 3.6.10.10  1996/08/16  19:54:44  dujihara
 * CSCdi62632:  alignment errors in SRB fastswitching
 * Branch: California_branch
 * Fixed alignment errors and replaced bit-mapped structure
 *
 * Revision 3.6.10.9  1996/07/25  23:54:06  motto
 * CSCdi46834:  SRB explorer counters are incr. on CIP I/F for non CIP
 * destined expl
 * Branch: California_branch
 *
 * Revision 3.6.10.8  1996/07/23  18:48:40  ppearce
 * CSCdi63747:  Move srb_multiring out of SRB subblock
 * Branch: California_branch
 *   (1) srb_multiring doesn't really belong exclusively to SRB
 *         move srb_multiring out of SRB subblock and into swidb
 *         provide srb_multiring accessor routines
 *   (2) determine_rif() should take swidb as argument rather than hwidb
 *
 * Revision 3.6.10.7  1996/07/18  20:29:23  ppearce
 * CSCdi63366:  Move SRB counters from hwidb into SRB subblock
 * Branch: California_branch
 *   - Decreases size of hwidb for non-SRB'd interfaces
 *   - Provides additional IOS-independence for SRB
 *   - Moves SRB a little closer to being completely swidb-based
 *
 * Revision 3.6.10.6  1996/07/09  06:05:12  ppearce
 * CSCdi59527: inlines waste code space
 * Branch: California_branch
 *   Just say "no" to inline abuse - SRB subblock inlines removed
 *
 * Revision 3.6.10.5  1996/06/20  07:04:27  ogrady
 * CSCdi60879:  ALIGN-3-SPURIOUS access in rsrb_fair_queue_flow_id()
 *              Branch: California_branch
 *              Add init for queueing
 *
 * Revision 3.6.10.4  1996/05/22  17:06:44  ppearce
 * CSCdi58099:  SRB fastswitching performance
 * Branch: California_branch
 *   Use direct accesses to SRB subblock fields rather than accessors
 *
 * Revision 3.6.10.3  1996/05/17  12:12:38  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.6.2.5  1996/04/26  15:23:01  ppearce
 * IBU modularity - SRB subblock (part 1)
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.6.2.4  1996/04/03  21:58:33  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.6.2.3  1996/03/20  17:54:50  ppearce
 * IBU modularity - NetBios subblock
 * Branch: IbuMod_Calif_branch
 *   Code review comment - name changes only
 *
 * Revision 3.6.2.2  1996/03/17  18:49:28  ppearce
 * Sync IbuMod_Calif_branch to V111_1_3
 *
 * Revision 3.6.2.1  1996/03/16  12:09:51  ppearce
 * CSCdi50498:  IBU modularity - Implement NetBios subblock
 * Branch: IbuMod_Calif_branch
 *   Move NetBios fields from hwidbtype/idbtype structs to NetBios subblock
 *
 * Revision 3.6.10.2  1996/03/23  22:49:10  ppearce
 * SR/TLB fastswitching feature
 * Branch: California_branch
 *
 * Revision 3.6.10.1  1996/03/18  22:10:19  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.14.4  1996/03/16  07:37:35  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.3.14.3  1996/03/13  02:01:46  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.3.14.2  1996/03/07  10:50:29  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.14.1  1996/02/20  18:46:39  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.6  1996/02/21  02:43:38  hampton
 * Clean up platform specific SRB defines in commonly included header
 * files.  [CSCdi48844]
 *
 * Revision 3.5  1996/02/13  22:24:48  fred
 * CSCdi47956:  WFQ severely degrades RSRB performance
 *         fix editing glitc
 *
 * Revision 3.4  1996/02/13  22:12:18  fred
 * CSCdi47956:  WFQ severely degrades RSRB performance
 *              move fair queue discriminator from srb.c to rsrb.c
 *              make sure pointers are st up for queueing routine
 *
 * Revision 3.3  1995/12/13  04:24:00  ahh
 * CSCdi44230:  Remove dead code
 * Bring out your dead... *clang* (RSRB/TCP fast-switching)
 *
 * Revision 3.2  1995/11/17  18:54:04  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:30:58  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.12  1995/11/08  21:30:07  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.11  1995/10/16  20:54:44  fred
 * CSCdi39023:  RSRB direct over serial with fair queueing doesnt work
 * correctly
 *         partially fixed with CSCdi39816, which corrected queuing of
 *         virtual
 *         interfaces. This makes sure that pak->linktype is set correctl
 *
 * Revision 2.10  1995/09/25  08:35:38  tli
 * CSCdi40960:  ISP jumbo patch 2
 *
 * Revision 2.9  1995/08/07  05:43:33  richl
 * CSCdi38315:  makefile rework
 *              header changes for rif_util.c and tring_inline.h
 *              move dlsw_work to its own file, create libibm_util.a for
 *              utility routines used by dlsw and rsrb
 *
 * Revision 2.8  1995/08/01  22:05:52  motto
 * Changed fast explorer vector routine to keep track of drops instead
 * of the caller.
 * Added support for CIP internal LAN fast explorers.
 * CSCdi37528:  CIP internal LAN SRB fastswitch explorer does not work
 *
 * Revision 2.7  1995/08/01  09:22:30  ogrady
 * CSCdi32342:  update TR_MAXPORTS.
 * Remove constant TR_MAXPORTS.  Determine max number of possible SRB
 * ports dynamically.
 *
 * Revision 2.6  1995/07/18  04:16:23  shaker
 * CSCdi35990:  Need packet pool for fast switched packet structure
 * Use new pak_set_dirty() to only set PAK_BUF_DIRTY iff
 * PAK_BUF_USE_DIRTY is set. Set PAK_BUF_USE_DIRTY for RSP fs_pak
 *
 * Revision 2.5  1995/07/12  23:45:29  hampton
 * Convert TCP to fully use the passive timers macros instead of
 * referencing the system clock directly.  [CSCdi37015]
 *
 * Revision 2.4  1995/07/11 00:52:32  ogrady
 * CSCdi34908:  Process level R/SRB hangs interface (orig AT over SRB
 * fails).  Don't make fastout vector return_false to disable srb fast
 * switching since its called from srb_common() path.  Set AC byte to
 * AC_PRI4 for all srb output paths.  Don't count ignored MAC packets as
 * drops.  Ongoing SRB counter clean up.
 *
 * Revision 2.3  1995/06/26  20:45:11  rbatz
 * CSCdi30326:  Bytes and Packet information INVALID(sho int token ring)
 * When fast srb explorers were transmitted, outputs_fast[ACCT_PROTO_SRB]
 * and tx_cumbytes_fast[ACCT_PROTO_SRB] were not being incremented.  This
 * resulted in incorrect reporting of output stats for "show int tokX".
 *
 * Revision 2.2  1995/06/17  06:26:10  ahh
 * CSCdi35734:  TCP: Long overdue code reorganization
 * Separate tcp.h into public and private header files.  Shuffle functions
 * around into more logical groupings.  Rename functions to meet naming
 * conventions (say goodbye to StudlyCaps).  Delete unneeded externs.
 * Trim off old log messages.
 *
 * Revision 2.1  1995/06/07  23:00:19  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/*
    Fast switching routines for srb and rsrb - hardware independent
    Don't put hardware specific code in this file.
    Changes to this file should also be made to les/fastsrb_les.c
        until the low end migrates to this code.

    SRB input is via idb->tr_srb_fastswitch()
      The srb routines expect for input: 
         pak->datagramstart to point to a token ring style AC header byte.
         pak->datagramsize not to include FCS - like routed packets 
         pak->fcs_type to indicate type of FCS appended to pak if any
         pak->riflen initialized
         caller should set this up.
    Raw srb output is thru hwidb->tr_srb_fastout() which should:
      make media and hw adjustments to the pak (eg fddi).
      decide whether to use the packet fcs as determined by
           pak->fcs_type which should be valid on entry.
      output the packet and return status.
    if the pak is not sent or dropped but the pak struct/data is
    modified (eg by fddi) it should be restored.

    (tr_srb_fastout() params are different from high end)

    Output via hwidb->tr_srb_fastout() and fast rsrb requires
    pak->fcs_type to be valid.  SRB output via hwidb->fastout() does
    not.  Process level srb output should not need to set pak->fcs_type
    since it ends up going out via hwidb->fastout().  Eventually
    process level output should be made to handle pak->fcs_type.

    Routines return FALSE if a packet can't be fastswitched.  In
    general they should NOT attempt to enqueue the packet for process
    level or any later use as the pak struct may point to memory
    accessable to interrupt level only during the interrupt (as on
    RSP).  For routines that must to do this (none now) a platform
    dependent function needs to first copy the pak.

    In general debug messages should be printed for cases where the pak
    is switched or dropped.

    Counters:

      These count srb packets that are fastswitched:
	hwidb->counters.inputs_fast[ACCT_PROTO_SRB]
        hwidb->counters.rx_cumbytes_fast[ACCT_PROTO_SRB]
        hwidb->counters.outputs_fast[ACCT_PROTO_SRB]
        hwidb->counters.tx_cumbytes_fast[ACCT_PROTO_SRB]

      These count srb packets that are dropped:
	hwidb->counters.inputs_fast[ACCT_PROTO_SRB]
        hwidb->counters.rx_cumbytes_fast[ACCT_PROTO_SRB]
	srb_sb->srb_rx_drops or srb_sb->srb_tx_drops

      Packets that are sent to srb_common() or process level should not
      be counted at all here since they will be counted there.


    CPU Caching issues: on RSP packet data is cached writeback in the
    CPU's data cache - packet data is also shared with other processors
    - the IPs.  output routines normally flush only a typical (ip+tcp)
    header's worth of packet from the cache - rif not counted.  If
    packet data beyond that is touched, eg via access checks, the
    output routines need to know to flush the whole packet.  This is
    done with:

        pak_set_dirty(pak);

    For rsrb input this is done up front in platform code.  ie. We play
    it safe and mark all paks as dirty.  For srb and rsrb output this
    is done here only where needed.



                         R/SRB Fastswitching Map

          Starting points are in interface input interrupt handler

                             (SRB INPUT)

                       ihwidb->tr_srb_fastswitch() 
                                  |
                                  |
        ---------------------------------------------
        |  (2 PORT BRIDGE)                          |   (MULTIPORT BRIDGE)
        |                                           |
   fs_srb_to_if()                           fs_srb_to_vring()    
ohwidb->tr_srb_fastout()     ohwidb->tr_srb_fastout()  vrp->vrp_fast_lanout()
   |                                   |                     |
   |                                   | (SRB OUT)           | (RSRB OUT)
   |------------------------------------                     |
   |                                                         |
   |        --------------------------------------------------
   |        |                                                |
   |        |                                                |
   |        |                                                |
   |        |                                                |
   | fs_rsrb_direct_output()                        fs_rsrb_fst_output()
   |        |                                                |
   |        --------------------------------------------------
   |                                 |                        
   |                           rsrb_fastsend()
   |                         ohwidb->fastsend()
   |
   |
   |
   |                               (RSRB INPUT)
   |                                     
   |  reg_invoke_fs_rsrb_direct_input()       reg_invoke_fs_rsrb_fst_input()
   |              |                                       |
   |    fs_rsrb_direct_input()                    fs_rsrb_fst_input()
   |   ohwidb->tr_srb_fastout()                ohwidb->tr_srb_fastout()  
   |              |                                       |                   
   |              |                                       |                   
   |              |                                       |                   
   --------------------------------------------------------
                                   |
                                   |
                        interface/platform specific
                           srb output function
                           ohwidb->fastsend() 

*/

#include "master.h"
#include <ciscolib.h>
#include "interface_private.h"
#include "fastswitch_registry.h"
#include "sys_registry.h"
#include "../ip/ip_registry.h"
#include "subsys.h"
#include "packet.h"
#include "config.h"
#include "../ui/debug.h"
#include "../h/ttysrv.h"
#include "../wan/serial.h"
#include "../if/ether.h"
#include "../if/tring_common.h"
#include "../if/tring.h"
#include "../if/fddi.h"
#include "../if/network.h"
#include "logger.h"
#include "srb.h"
#include "srb_multiring.h"
#include "srb_sb_private.h"
#include "rsrb.h"
#include "srt_debug.h"
#include "../ip/ip.h"
#include "../ip/ipfast.h"
#include "../ibm/netbios.h"
#include "../ibm/netbios_sb.h"
#include "../snmp/sr_old_lchassismib.h"
#include "../if/rif.h"
#include "../if/rif_util.h"
#include "../if/rif_inline.h"
#include "trsrb.h"
#include "../if/atm.h"
#include "../srt/srt_registry.h"
#include "../tbridge/tbridge_sde.h"
#include "../if/tring_inline.h"
#include "../srt/fastsrb.h"


/*
 * srb_incr_vring_counters()
 *   Utility routine to increment SRB counters
 *
 * NOTE: This exact routine exists in three different places
 *           ../srt/fastsrb.c
 *           ../hes/fastsrb_les.c
 *           ../les/fastsrb_cbus.c
 *
 *       Plan on centralizing the routine when time permits --ppearce
 *
 */
static void
srb_incr_vring_counters (hwidbtype *idb, tr_ventrytype *vre, int dgsize)
{
    srb_sb_incr_tx_nb_counters(firstsw_or_null(idb->vidb_link), dgsize);
    srb_sb_incr_rx_counters(firstsw_or_null(vre->virtual_idb), dgsize);
    vre->forwards++;
}
 


/**********************************************************************
 * 
 * FAST RSRB REWRITE ROUTINES
 */ 

/*
 * PACKET FORMAT FOR FST PACKETS
 *
 * Token Ring
 * ----------
 *      ACFC     acfc bits
 * DADA DADA    mac destination addr
 * DADA SASA    mac destination addr / mac source addr
 * SASA SASA    mac source addr
 *  RIF ....    rif information, variable length
 * AAAA LLC1    #define SNAPSNAP 0xAAAA / llc1
 * 0000 0800    zero's / #define TYPE_IP10MB 0x0800 (IP on 10MB net)
 *                      munged ip header (20 bytes worth)
 *  FST  FST    version:4 bits, ip header length:4 bits, type of service: 8 bits
 *                      / total length
 *  FST  FST    identifier / ipreserved: 1 bit, dontfragment:1 bit,
 *                      more fragments:1 bit, frame offset: 13 bits
 *                      (identifier is really sequence no for fst)
 *  FST  FST    time to live:1 uchar, protocol type: 1 uchar / checksum:ushort
 *  FST  FST    source ip address
 *  FST  FST    destination ip address
 *  VER 0010    rsrb version / #define RSRB_OP_FORWARDFST 16
 *   RN  VRN    ring number / virtual ring number
 * 0000 xxxx    offset (always 0) / start of encapsulated packet
 *                                  which BTW has the ac/fc bits from
 *                                  the other ring
 *
 * Serial
 * ------
 * DADA 0800    2 bytes dest addr / #define TYPE_IP10MB 0x0800 (IP on 10MB net)
 *                      munged ip header (20 bytes worth)
 *  FST  FST    version:4 bits, ip header length:4 bits, type of service: 8 bits
 *                      / total length
 *  FST  FST    identifier / ipreserved: 1 bit, dontfragment:1 bit,
 *                      more fragments:1 bit, frame offset: 13 bits
 *                      (identifier is really sequence no for fst)
 *  FST  FST    time to live:1 uchar, protocol type: 1 uchar / checksum:ushort
 *  FST  FST    source ip address
 *  FST  FST    destination ip address
 *  VER 0010    rsrb version / #define RSRB_OP_FORWARDFST 16
 *   RN  VRN    ring number / virtual ring number
 * 0000 xxxx    offset (always 0) / start of encapsulated packet
 *                                  which BTW has the ac/fc bits from
 *                                  the other ring
 *
 * Ethernet
 * --------
 * DADA DADA    mac destination addr
 * DADA SASA    mac destination addr / mac source addr
 * SASA SASA    mac source addr
 *                      munged ip header (20 bytes worth)
 * 0800  FST    #define TYPE_IP10MB 0x0800 (IP on 10MB net), version:4 bits,
 *                      ip header length:4 bits, type of service: 8 bits
 *  FST  FST    total length / identifier
 *                      (identifier is really sequence no for fst)
 *  FST  FST    ipreserved: 1 bit, dontfragment:1 bit, more fragments:1 bit,
 *                      frame offset: 13 bits / time to live:1 uchar,
 *                      protocol type: 1 uchar
 *  FST  FST    checksum / source ip address
 *  FST  FST    source ip address / destination ip address
 *  FST  VER    destination ip address / rsrb version
 * 0010   RN    #define RSRB_OP_FORWARDFST 16 / ring number
 *  VRN 0000    virtual ring number / offset (always 0)
 * xxxx         start of encapsulated packet
 *              which BTW has the ac/fc bits from the other ring
 */

/*
 * write rsrb fst ip header
 */
static inline uchar *write_fst_ip_header (
    uchar *data,
    ulong *fst_hdr)
{
    PUTLONG(((ulong *)data)++, fst_hdr[0]);
    PUTLONG(((ulong *)data)++, fst_hdr[1]);
    PUTLONG(((ulong *)data)++, fst_hdr[2]);
    PUTLONG(((ulong *)data)++, fst_hdr[3]);
    PUTLONG(((ulong *)data)++, fst_hdr[4]);
    return(data);
}


static inline uchar *fst_write_ether_header (uchar *data,
                                             ipcache_rntype *cptr,
                                             ulong *fst_hdr)
{
    PUTLONG(((ulong *)data)++, cptr->m.maclong[0]);
    PUTLONG(((ulong *)data)++, cptr->m.maclong[1]);
    PUTLONG(((ulong *)data)++, cptr->m.maclong[2]);
    PUTSHORT(((ushort *)data)++, TYPE_IP10MB);
    return(write_fst_ip_header(data, fst_hdr));
}


static inline uchar *fst_write_fddi_header (uchar *data,
                                             ipcache_rntype *cptr,
                                             ulong *fst_hdr)
{
    *data++ = FDDI_LLC_FC_BYTE;         /* fc */
    PUTLONG(((ulong *)data)++, cptr->m.maclong[0]);
    PUTLONG(((ulong *)data)++, cptr->m.maclong[1]);
    PUTLONG(((ulong *)data)++, cptr->m.maclong[2]);
    PUTLONG(((ulong *)data)++, cptr->m.maclong[3]); /* SNAP/UI */
    PUTLONG(((ulong *)data)++, TYPE_IP10MB);
    return(write_fst_ip_header(data, fst_hdr));
}


static inline uchar *fst_write_serial_header (
    uchar *data,
    ipcache_rntype *cptr,
    ulong *fst_hdr)
{
    PUTLONG(((ulong *)data)++, ((cptr->m.macshort[0] << 16) | TYPE_IP10MB));
    return(write_fst_ip_header(data, fst_hdr));
}


static inline uchar *fst_write_atm_header (
    uchar *data,
    ipcache_rntype *cptr,
    ulong *fst_hdr)
{

    ushort *src = cptr->m.macshort;
    int    enclen = cptr->length;

    /*
     * atm encapsulation for outgoing vc is already in cache, so just
     * copy it.  how convenient.
     */

    while(enclen > 0) {
        PUTSHORT(((ushort *)data)++, *src++);
        enclen -= sizeof(short);
    }

    /* write ip header */
    return(write_fst_ip_header(data, fst_hdr));
}


static inline uchar *fst_write_token_header (uchar *data,
                                             ipcache_rntype *cptr,
                                             riftype *rifptr,
                                             ulong *fst_hdr)
{
    ushort *rif;
    int i;

    PUTSHORT(((ushort *)data)++, ((AC_PRI4 << 8) | FC_TYPE_LLC));
    PUTSHORT(((ushort *)data)++, cptr->m.macshort[0]);
    PUTSHORT(((ushort *)data)++, cptr->m.macshort[1]);
    PUTSHORT(((ushort *)data)++, cptr->m.macshort[2]);
    if (rifptr && rifptr->length)
        PUTSHORT(((ushort *)data)++, (cptr->m.macshort[3]|(TR_RII<<8)));
    else
        PUTSHORT(((ushort *)data)++, cptr->m.macshort[3]);
    PUTSHORT(((ushort *)data)++, cptr->m.macshort[4]);
    PUTSHORT(((ushort *)data)++, cptr->m.macshort[5]);
    if (rifptr && rifptr->length) {
        for (i=0, rif=(ushort *)rifptr->rif; i < rifptr->length; i+=2)
            PUTSHORT(((ushort *)data)++, *rif++);
    }
    PUTLONG(((ulong *)data)++, ((SNAPSNAP << 16) | ((LLC1_UI<<8) | 0)));
    PUTLONG(((ulong *)data)++, TYPE_IP10MB);
    return(write_fst_ip_header(data, fst_hdr));
}


#define TR_FST_VR_HDR(data, vrp, rn, op)                                \
{                                                                       \
    PUTLONG(((ulong *)data)++, ((vrp->vrp_version << 16)|(op)));        \
    PUTLONG(((ulong *)data)++, ((rn << 16) | vrp->vrg->vrn));           \
    PUTSHORT(((ushort *)data)++, 0);                                    \
}



/*
 * PACKET FORMAT FOR DIRECT ENCAPS RSRB PACKETS
 *
 * Token Ring
 * ----------
 *      ACFC    AC/FC bits
 * DADA DADA    Destination Address
 * DADA SASA    Dest / Source
 * SASA SASA    Source Address
 * RIF INFO     rif info
 * .........    (variable length)
 * SNAP LLC1    SNAP is 0xAAAA / llc1
 * 0000 1996    0 / #define TYPE_RSRB 0x1996   (RSRB raw)
 *  VER 0001    Version / #define RSRB_OP_FORWARD 1
 *   RN  VRN    ring number / virtual ring number
 * 0000  LEN    offset (always 0) / length
 * PARM PARM    parameter (always zero, longword)
 * encapsulated packet which BTW has the ac/fc bits from the other ring
 *
 * Serial
 * ------
 * 0F00 1996    #define HDLC_RSRB 0x0F001996L (RSRB raw serial encapsulation)
 * 0001   RN    #define RSRB_OP_FORWARD 1 / ring number
 * VRN  0000    virtual ring number / offset (always 0)
 *  LEN PARM    length / parameter (always zero, longword)
 * PARM encapsulated packet which BTW has the ac/fc bits from the other ring
 * 
 * Ethernet
 * --------
 * DADA DADA    Destination Address
 * DADA SASA    Dest / Source
 * SASA SASA    Source Address
 *  LEN SNAP    length / SNAP is 0xAAAA
 * LLC1 0000    llc1 / 0
 * 1996  VER    #define TYPE_RSRB 0x1996 (RSRB raw) / version
 * 0001   RN    #define RSRB_OP_FORWARD 1 / ring number
 *  VRN 0000    virtual ring number / offset (always 0)
 *  LEN PARM    length / parameter (always zero, longword)
 * PARM encapsulated packet which BTW has the ac/fc bits from the other ring
 *
 */

#define SRB_WRITE_SERIAL_HEADER(data)           \
    PUTLONG(((ulong *)data)++, HDLC_RSRB);


#define SRB_WRITE_ETHER_HEADER(data, dst, src, length)  \
    ieee_copy(dst, (uchar *)data);                      \
    data += 6;                                          \
    ieee_copy(src, (uchar *)data);                      \
    data += 6;                                          \
    PUTSHORT(((ushort *)data)++, length);               \
    PUTLONG(((ulong *)data)++, ((SNAPSNAP << 16) | ((LLC1_UI << 8) | 0)));\
    PUTLONG(((ulong *)data)++, TYPE_RSRB);



static inline uchar *srb_write_fddi_header (
    hwidbtype *ohwidb,
    uchar *data,
    uchar *dst,
    uchar *src)
{
    *data++  = FDDI_LLC_FC_BYTE;        /* fc byte */

    /*
     * FIP card does hw bit swapping - others don't
     *  xxx should put a flag in the idb like: idb->fddi_hw_bitswap
     */

    if (ohwidb->type == IDBTYPE_FDDIT) {

        *data++ = *dst++;
        *data++ = *dst++;
        *data++ = *dst++;
        *data++ = *dst++;
        *data++ = *dst++;
        *data++ = *dst++;

        *data++ = *src++;
        *data++ = *src++;
        *data++ = *src++;
        *data++ = *src++;
        *data++ = *src++;
        *data++ = *src++;

    } else {
 
        *data++ = bit_swaps[*dst++]; 
        *data++ = bit_swaps[*dst++];
        *data++ = bit_swaps[*dst++];
        *data++ = bit_swaps[*dst++];
        *data++ = bit_swaps[*dst++];
        *data++ = bit_swaps[*dst++];

        *data++ = bit_swaps[*src++];
        *data++ = bit_swaps[*src++];
        *data++ = bit_swaps[*src++];
        *data++ = bit_swaps[*src++];
        *data++ = bit_swaps[*src++];
        *data++ = bit_swaps[*src++];
    }

    PUTLONG(((ulong *)data)++, ((SNAPSNAP << 16) | 0x0300));
    PUTLONG(((ulong *)data)++, TYPE_RSRB);              
    return(data);
}


#define SRB_WRITE_TOKEN_HEADER(data, dst, src, rifptr)                  \
{                                                                       \
    int i;                                                              \
    ushort *rif;                                                        \
    PUTSHORT(((ushort *)data)++, ((AC_PRI4 << 8) | FC_TYPE_LLC));       \
    ieee_copy(dst, (uchar *)data);                                      \
    data += 6;                                                          \
    ieee_copy(src, (uchar *)data);                                      \
    if (rifptr && rifptr->length)                                       \
        *(uchar *)data |= TR_RII;                                       \
    data += 6;                                                          \
    if (rifptr && rifptr->length) {                                     \
        for (i=0, rif=(ushort *)rifptr->rif; i < rifptr->length; i+=2)  \
            PUTSHORT(((ushort *)data)++, *rif++);                       \
    }                                                                   \
    PUTLONG(((ulong *)data)++, ((SNAPSNAP << 16) | ((LLC1_UI<<8) | 0)));\
    PUTLONG(((ulong *)data)++, TYPE_RSRB);                              \
}

#define RSRB_HDR(data, vrp, rn, dgsize, op)                  \
{                                                            \
    PUTLONG(((ulong *)data)++,                               \
            (((vrp->vrp_version) << 16) | (op)));            \
    PUTLONG(((ulong *)data)++, ((rn << 16) | vrp->vrg->vrn));\
    PUTLONG(((ulong *)data)++, dgsize);                      \
    PUTLONG(((ulong *)data)++, 0);              /* param (long)  */  \
}

/*
 * Debug msg support
 */
static char srb_2port_str[] = "2port";
static char srb_local_str[] = "local";
static char rsrb_remote_str[] = "remote";
static char rsrb_direct_str[] = "direct";
static char rsrb_fst_str[] = "fst";

static char rsrb_vrg_err[] = "no vring group";
static char rsrb_vre_vrp_err[] = "no vring entry or peer";
static char srb_odrop_err[] = "output drop";
static char srb_access_err[] = "access drop";
static char srb_down_err[] = "output down";
static char srb_fwd[] = "fast switched";

static void
srb_in_buginf (
    paktype *pak,
    hwidbtype *ihwidb,
    char *status,
    hwidbtype *ohwidb,
    char *switch_type,
    int trn,
    tr_vpeertype *vrp)
{
    char peerid[MAX_PEER_NAME];
    idbtype *iswidb = firstsw_or_null(ihwidb);
    srb_sb_t *srb_sb = srb_get_sb_inline(iswidb);

    buginf("\nSRB");

    if (ihwidb && srb_sb)
        buginf(": in %s rn %d bn %d", ihwidb->hw_short_namestring,
                srb_sb->srb_thisring, srb_sb->srb_bridge_num);
    if (status)
        buginf(": %s", status);
    if (switch_type)
       buginf(": %s", switch_type);
    if (ohwidb)
       buginf(": output %s", ohwidb->hw_short_namestring);
    if (vrp)
       buginf(": to peer %s", vrpeer_id(vrp, peerid));
    if (trn)
       buginf(": trn %d", trn);
}

static void
rsrb_in_buginf (
    paktype *pak,
    hwidbtype *ihwidb,
    char *status,
    hwidbtype *ohwidb,
    char *rsrb_type,
    int trn)
{
    buginf("\nRSRB: fast %s in %s: %s", rsrb_type, ihwidb->hw_short_namestring,
	   status);
        
    if (ohwidb)
       buginf(": output %s", ohwidb->hw_short_namestring);
    if (trn)
       buginf(": trn %d", trn);
}

/*
 * return TRUE if output access checking is NOT permitted on fast
 *  switched packets on an interface
 *
 * xxx this restriction could be a holdover from the hi end
 *  on cbus cards - which have only the frame header in memory -
 *  and netbios checks may go beyond the header.
 * here we have pak in memory and should be able to access check the
 * entire packet - RSP does require PAK_BUF_DIRTY flag
 * set for accesses beyond the header.
 *
 * note: should instead set flag at set up time to indicate this
 *       like in idb->srb_routecache or idb->srb_access_out;
 */
static inline boolean
no_fs_srb_out_acc_chk (hwidbtype *ohwidb)
{
    return(netbios_sb_ibm_b_oacf_is_enabled(ohwidb->firstsw) ||
           (netbios_bytes_accQ.qhead && ohwidb->access_expression_out));
}


static inline void 
srb_rx_counters (hwidbtype *ihwidb, int length)
{
    GET_TIMESTAMP(ihwidb->lastinput);
    ihwidb->counters.inputs_fast[ACCT_PROTO_SRB]++; 
    ihwidb->counters.rx_cumbytes_fast[ACCT_PROTO_SRB] += length;
}


static inline int
rsrb_fastsend (
    paktype *pak,
    hwidbtype *ihwidb,
    hwidbtype *ohwidb,
    char *rsrb_type,
    int trn,
    tr_vpeertype *vrp)
{
    int drops;

    pak->if_output = ohwidb->firstsw;
    drops = ohwidb->counters.output_total_drops;
    (*ohwidb->fastsend)(ohwidb, pak);

        /*
	 * kludge alert:
         * using the fact that ohwidb->outputdrops increments to determine
	 * if fastsend succeeded.  NOT a great way to determine if it
	 * did but better than ASSuming it did -
	 * which the old code did for rsrb accounting.
	 * fastsend() routines should be modified to return a value.
         */

    if (drops == ohwidb->counters.output_total_drops) {
        if (srb_debug)
            srb_in_buginf(pak, ihwidb, srb_fwd, ohwidb, rsrb_type, trn, vrp);
        return (FS_SRB_SENT);
    } else {
        srb_sb_incr_tx_drops(ohwidb->firstsw);
        if (srb_debug || srberr_debug)
            srb_in_buginf(pak, ihwidb, srb_odrop_err, ohwidb, rsrb_type,
	                  trn, vrp);
        return (FS_SRB_DROPPED);
    }
}

/*
 * fastswitch srb packet to an interface (2 port case)
 * called via hwidb->tr_srb_fastswitch().
 * assumes pak riflen, datagramsize, datagramstart are set up
 *
 * return TRUE if pak was switched/dropped
 */
static boolean
fs_srb_to_if (hwidbtype *ihwidb, paktype *pak, hwidbtype *ohwidb)
{
    ulong dgsize = pak->datagramsize + TR_FCS_SIZE;

    srb_rx_counters(ihwidb, dgsize);

    /*
     * Check the access lists.
     */
    if (ihwidb->srb_access_in) {

        /*
         * mark that packet data beyond network hdr is potentially
         * in CPU's cache and needs flushed
         */
	pak_set_dirty(pak);
        if (!srb_in_check(pak->datagramstart, dgsize, ihwidb, pak->riflen)) {
            srb_sb_incr_rx_drops(ihwidb->firstsw);
            datagram_done(pak);
            if (srb_debug || srberr_debug) {
                idbtype *iswidb = firstsw_or_null(ihwidb);
                srb_in_buginf(pak, ihwidb, srb_access_err, ohwidb,
                          srb_2port_str, srb_sb_get_targetring(iswidb), NULL);
            }
            return(TRUE);
      }
    }
    if (ohwidb->srb_access_out) {

        /*
         * for 2 port case we already checked at setup time that we
         *  can do access checks
         *
         * mark that packet data beyond network hdr is potentially
         * in CPU's cache and needs flushed
         */
	pak_set_dirty(pak);
        if (!srb_out_check(pak->datagramstart, dgsize, ohwidb, pak->riflen)) {
            srb_sb_incr_tx_drops(ohwidb->firstsw);
            datagram_done(pak);
            if (srb_debug || srberr_debug) {
                idbtype *iswidb = firstsw_or_null(ihwidb);
                srb_in_buginf(pak, ihwidb, srb_access_err, ohwidb,
                            srb_2port_str, srb_sb_get_targetring(iswidb), NULL);
            }
            return(TRUE);
        }
    }

    if (ohwidb->state != IDBS_UP) {
        ohwidb->counters.output_total_drops++;
        srb_sb_incr_tx_drops(ohwidb->firstsw);
        datagram_done(pak);
        if (srb_debug || srberr_debug) {
             idbtype *iswidb = firstsw_or_null(ihwidb);
             srb_in_buginf(pak, ihwidb, srb_down_err, ohwidb,
                            srb_2port_str, srb_sb_get_targetring(iswidb), NULL);
        }
        return(TRUE);
    }

        /*
         * set acct_proto so fastsend() updates srb output counters
         */
    pak->acct_proto = ACCT_PROTO_SRB;
    pak->linktype = LINK_RSRB;
    pak->if_output  = ohwidb->firstsw;
    pak->mac_start = ((trrif_t *)pak->datagramstart)->daddr;
    ((trrif_t *)pak->datagramstart)->ac = AC_PRI4;
    ((trrif_t *)pak->datagramstart)->fc = FC_TYPE_LLC;

    switch((*ohwidb->if_srb_fastout)(ohwidb, pak)) {

        case FS_SRB_SENT:
            if (srb_debug) { 
                idbtype *iswidb = firstsw_or_null(ihwidb);
                srb_in_buginf(pak, ihwidb, srb_fwd, ohwidb, srb_2port_str, 
			      srb_sb_get_targetring(iswidb), NULL);
            }
            return (TRUE);

        default:
          datagram_done(pak);  /* fall thru to drop - prevents out of order */

        case FS_SRB_DROPPED:
            srb_sb_incr_tx_drops(ohwidb->firstsw);
            if (srb_debug || srberr_debug) {
                idbtype *iswidb = firstsw_or_null(ihwidb);
                srb_in_buginf(pak, ihwidb, srb_odrop_err, ohwidb,
			      srb_2port_str, 
                              srb_sb_get_targetring(iswidb), NULL);
            }
            return (TRUE);
    }
}
 

/*
 * fastswitch srb packet to virtual ring.
 * called via hwidb->tr_srb_fastswitch() vector.
 *
 * on entry pak->riflen should be init
 * on entry pak->rif_match should be init or NULL.
 *
 * return TRUE if pak was switched/dropped
 */
static inline boolean
fs_srb_to_vring_inline (hwidbtype *ihwidb, paktype *pak)
{
    uchar *dgstart;
    ulong dgsize;
    int riflen;
    uchar *rif_start;
    hwidbtype *ohwidb;
    ushort bn;
    ushort rn;
    ushort rd;                  /* ending ring/bridge in the rif */
    tr_vpeertype *vrp;
    tr_ventrytype *vre;
    ushort rc;

    ENTER_RSRB("fs_srb_to_vring");

    pak->acct_proto = ACCT_PROTO_SRB;   /* for fastsend() accting */
    pak->linktype = LINK_RSRB;

    dgstart = pak->datagramstart; 
    rif_start = &((trrif_t*)dgstart)->rc_blth;
    dgsize = pak->datagramsize + TR_FCS_SIZE;
    riflen = pak->riflen; 

    /*
     * les finds rif match for all pkts before calling srb code
     *  (doesn't need it for 2 port case though)
     * so get it here if not found yet (RSP)
     */
    if (!pak->rif_match) {
        idbtype *iswidb = firstsw_or_null(ihwidb);
        srb_sb_t *srb_sb = srb_get_sb_inline(iswidb);
        pak->rif_match = fs_rif_match((srbroute_t *)rif_start, pak->riflen, 
                         srb_sb->rng_br_rng_f, srb_sb->rng_br_rng_r);
        if (!pak->rif_match) 
            return(FALSE);      /* can this happen? */
    }

    /*
     * Gather all the information we will need to forward this frame.
     */
    rc = GETSHORT(rif_start);
    if (rc & RC_D) {
        rd = GETSHORT(rif_start + 2);
        rn = GETSHORT(pak->rif_match - 2) >> 4;
        bn = GETSHORT(pak->rif_match - 2) & 0xf;
    } else {
        rd = GETSHORT(rif_start + riflen - 2);
        bn = GETSHORT(pak->rif_match + 2) & 0xf;
        rn = GETSHORT(pak->rif_match + 4) >> 4;
    }

    /*
     * Bail checks go here.  'Traced' frames must be handed by the system
     * so that the appropriate response may be generated.  Also reject
     * frames that end on the virtual ring because of all the various
     * places in the system code where they could be processed.
     */
    if ((rc & ((RC_BROADBIT | RC_TRACE_BOTH) << 8)))
        return(FALSE);

    /*
     * ------------------------------------------------------------------
     * DLSw Fast Remote Peer Demux.
     * If rif terminates on the virtual ring, packet may be destiined for
     * a DLSw peer (DLSw performs rif termination).
     *
     * Reject non-DLSw frames that end on the virtual ring because of
     * all the various places in the system code they could be processed.
     * LNM is one such place.
     * ------------------------------------------------------------------
     */
    if ((rd >> 4) == srb_sb_get_targetring(ihwidb->firstsw)) {
        if (reg_invoke_dlsw_srb_input(ihwidb, pak))
          return(TRUE);
        else
          return(FALSE);
    }

    /*
     * find the table entry for the destination on the virtual ring.
     */
    vre = vrfind_rn2entry(rn, bn, ihwidb->tr_vring_blk, RSRB_ET_EITHER);

    /*
     * If there is not a forward target, return FALSE now, because this
     * frame needs to be handled by process level to attempt to reopen
     * previous dead remote peers (proxy explorer forces this...)
     */
    if (!vre)
        return(FALSE);

    if (vre->type == RSRB_ET_LOCAL) {
        
        /*
         * simple srb to another interface in this box
         */
        ohwidb = vre->outhole.idb;
        if (!ohwidb->srb_routecache)
            return(FALSE);

        if (ohwidb->state != IDBS_UP) {
            ohwidb->counters.output_total_drops++;
            srb_sb_incr_tx_drops(ohwidb->firstsw);
            datagram_done(pak);
            if (srb_debug || srberr_debug)
		srb_in_buginf(pak, ihwidb, srb_down_err, ohwidb,
			      srb_local_str, rn, NULL);
            return(TRUE);
        }

        if (ihwidb->srb_access_in) {

            /*
             * mark that packet data beyond network hdr is potentially
             * in CPU's cache and needs flushed
             */
	    pak_set_dirty(pak);
            if (!srb_in_check(dgstart, dgsize, ihwidb, riflen)) {
                srb_sb_incr_rx_drops(ihwidb->firstsw);
                datagram_done(pak);
                if (srb_debug || srberr_debug)
		    srb_in_buginf(pak, ihwidb, srb_access_err, ohwidb,
			      srb_local_str, rn, NULL);
                return(TRUE);
            }
        }
        if (ohwidb->srb_access_out) {

            if (no_fs_srb_out_acc_chk(ohwidb))
                return(FALSE);

            /*
             * mark that packet data beyond network hdr is potentially
             * in CPU's cache and needs flushed
             */
	    pak_set_dirty(pak);
            if (!srb_out_check(dgstart, dgsize, ohwidb, riflen)) {
                srb_sb_incr_tx_drops(ohwidb->firstsw);
                datagram_done(pak);
                if (srb_debug || srberr_debug)
		    srb_in_buginf(pak, ihwidb, srb_access_err, ohwidb,
			      srb_local_str, rn, NULL);
                return(TRUE);
            }
        }

        pak->if_output  = ohwidb->firstsw;

        ((trrif_t *)dgstart)->ac = AC_PRI4;
        ((trrif_t *)dgstart)->fc = FC_TYPE_LLC;
	pak->mac_start = ((trrif_t *)dgstart)->daddr;

        switch((*ohwidb->if_srb_fastout)(ohwidb, pak)) {
            case FS_SRB_SENT:
                /*
                 * count vring transmit and receive for LNM
                 */
                srb_incr_vring_counters(ihwidb, vre, dgsize);
                if (srb_debug) {
                    srb_in_buginf(pak, ihwidb, srb_fwd, ohwidb,
			          srb_local_str, rn, NULL);
                }
                return (TRUE);

            default:
	        datagram_done(pak); /* fall thru - drop prevents out of order*/

            case FS_SRB_DROPPED:
                srb_sb_incr_tx_drops(ohwidb->firstsw);
                if (srb_debug || srberr_debug)
                    srb_in_buginf(pak, ihwidb, srb_odrop_err, ohwidb,
			          srb_local_str, rn, NULL);
                return (TRUE);
        }
    }

    if (vre->type == RSRB_ET_LOCAL_VIRTUAL) {
        ohwidb = vre->outhole.idb;
        if (ohwidb->span_bridge_process_force) 
            return(FALSE);

        if (ohwidb->state != IDBS_UP) {
            datagram_done(pak);
            return(TRUE);
        }

        if (ihwidb->srb_access_in) {

            /* mark that packet data beyond network hdr is potentially
             * in CPU's cache and needs flushed */

            pak_set_dirty(pak);
            if (!srb_in_check(dgstart, dgsize, ihwidb, riflen)) {
                srb_sb_incr_rx_drops(ihwidb->firstsw);
                datagram_done(pak);
                if (srb_debug)
		    srb_in_buginf(pak, ihwidb, srb_access_err, ohwidb,
			      srb_local_str, rn, NULL);
                return(TRUE);
            }
        }

        pak->if_input   = ihwidb->firstsw;
	pak->if_output  = ohwidb->firstsw;
	pak->linktype   = LINK_RSRB;
	pak->rxtype     = RXTYPE_UNKNOWN;
        pak->acct_proto = ACCT_PROTO_SRB;

	if ((*ohwidb->vbridge_forward)(pak, ohwidb)) {
            srb_incr_vring_counters(ihwidb, vre, dgsize);
	}
	return(TRUE);
    }

    /*
     * this may not be entirely necessary but play it safe for rsrb
     * signal that packet data beyond network hdr is potentially
     * in CPU's cache and needs flushed (used on RSP).
     */
    pak_set_dirty(pak);

    if (vre->type & (RSRB_ET_REMOTE | RSRB_ET_REMSTATIC)) {
        if (ihwidb->srb_access_in &&
	    !srb_in_check(dgstart, dgsize, ihwidb, riflen)) {
            srb_sb_incr_rx_drops(ihwidb->firstsw);
            datagram_done(pak);
            if (srb_debug || srberr_debug)
                srb_in_buginf(pak, ihwidb, srb_access_err, NULL,
				  rsrb_remote_str, rn, NULL);
            return(TRUE);
        }
        vrp = vre->outhole.vp;
        if ((vrp->type == RSRB_PT_IF) || (vrp->type == RSRB_PT_LAN) ||
	    (vrp->type == RSRB_PT_FST)) {
				 
            if (!(rsrb_out_check(dgstart, dgsize, ihwidb, riflen, vrp))) {
                srb_sb_incr_rx_drops(ihwidb->firstsw);
                datagram_done(pak);
                if (srb_debug || srberr_debug)
                    srb_in_buginf(pak, ihwidb, srb_access_err, NULL,
				    rsrb_remote_str, rn, vrp);
                return(TRUE);
            }

            if (vrp->vrp_fast_lanout) {

                /*
                 * vector to:
                 *   fs_rsrb_direct_output
                 *   fs_rsrb_fst_output
                 */

                switch ((*vrp->vrp_fast_lanout)(ihwidb, vrp, vre, rn, pak)) {
                  
		       /*
		        * debug msgs and counts done in vrp_fast_lanout routine
		        */
                  case FS_SRB_SENT:			/* packet sent */
                      vrp->pkts_tx++;
                      vre->forwards++;
                      return(TRUE);

                  case FS_SRB_DROPPED:
                      return(TRUE);

                  default: 
                  case FS_SRB_SLOWSWITCH:
                      return(FALSE);
                }
            }
        }
    }
    return(FALSE);
}

static boolean
fs_srb_to_vring (hwidbtype *ihwidb, paktype *pak, hwidbtype *dummy)
{
    int dgsize;

        /*
	 * an added layer just to get the accounting right
	 *
	 * should only count as a fast input if it was switched/dropped here.
	 * otherwise it will be passed to srb_common() which will count
	 * it (again) as process input.
	 */

    dgsize = pak->datagramsize;			/* save input value */

    if (fs_srb_to_vring_inline(ihwidb, pak)) {
        srb_rx_counters(ihwidb, dgsize+TR_FCS_SIZE);
        return (TRUE);
    }
    return (FALSE);
}



/*
 * Output SRB packet via direct (snap layer) rsrb encaps
 * for one hop only to dest
 * This routine is called via tr_vpeertype->vrp_fast_lanout() vector
 * (port from les: nim_vring_output() plus tms380_vring_output())
 *
 * Don't change pak struct fields unless packet is actually sent.
 */
static int
fs_rsrb_direct_output (hwidbtype *ihwidb,
                  tr_vpeertype *vrp,
                  tr_ventrytype *vre,
                  ushort rn,
                  paktype *pak)
{
    hwidbtype *ohwidb;
    uchar *data, *dgstart;
    ulong hdrsize;
    ulong srb_pkt_size;
    ulong op;
    ulong ohwidb_status;

    ENTER_RSRB("fs_rsrb_direct_output");

    ohwidb = vrp->outidb;
    ohwidb_status = ohwidb->status;
    
        /*
         * should have an op to forward fddi crc
         */
    if (pak->fcs_type == FCS_TYPE_TR) {
        op = RSRB_OP_FORWARD;
        srb_pkt_size = pak->datagramsize + TR_FCS_SIZE;
    } else {
        op = RSRB_OP_FORWARDCRC;
        srb_pkt_size = pak->datagramsize;
    }

    /*
     * Prepend rsrb MAC header to pkt.
     */

    pak->mac_start = pak->datagramstart + 2; /* point to destination */
    pak->linktype = LINK_RSRB;
    if (ohwidb_status & IDB_FDDI) {
        hdrsize = (FDDI_SNAP_ENCAPBYTES + TR_VHDRTYPE_SIZE);
        dgstart = pak->datagramstart - hdrsize;
        data = srb_write_fddi_header(ohwidb, dgstart, vrp->macaddr,
				     ohwidb->hardware);

    } else if (ohwidb_status & IDB_ETHER) {
        ulong llcsize;
        /*
         * total size is the length of the entire packet.  this
         * length is not the length of the llc.  the size of the llc
         * would be the length of the mac packet minus the size of the
         * destination & source address and the length field.
         * this happens to be 14.  for now fudge this and use
         * ARPA_ENCAPSIZE which happens to be 14.
         */
        hdrsize = (ETHER_SNAP_ENCAPBYTES + TR_VHDRTYPE_SIZE);
        llcsize = srb_pkt_size +
              (ETHER_SNAP_ENCAPBYTES-ETHER_ARPA_ENCAPBYTES+TR_VHDRTYPE_SIZE);
        dgstart = pak->datagramstart - hdrsize;
	data = dgstart;
        SRB_WRITE_ETHER_HEADER(data, vrp->macaddr, ohwidb->hardware, llcsize);
 
    } else if (ohwidb_status & IDB_SERIAL) {
        if (ohwidb->enctype != ET_HDLC)
            return(FS_SRB_SLOWSWITCH);

        hdrsize = (HDLC_HEADER_SIZE + TR_VHDRTYPE_SIZE);
        dgstart = pak->datagramstart - hdrsize;
	data = dgstart;
        SRB_WRITE_SERIAL_HEADER(data);
        
    } else if (ohwidb_status & IDB_TR) {
        riftype *rifptr = NULL;

        hdrsize = (TRING_SNAP_ENCAPBYTES + TR_VHDRTYPE_SIZE);
        if (srb_multiring_any_enabled(ohwidb->firstsw)) {
	    rifptr = get_rif_inline(ohwidb, vrp->macaddr);
            if (!rifptr)
                return(FS_SRB_SLOWSWITCH);
            hdrsize += rifptr->length;
        }
        dgstart = pak->datagramstart - hdrsize;
	data = dgstart;
        SRB_WRITE_TOKEN_HEADER(data, vrp->macaddr, ohwidb->hardware, rifptr);

    } else {
        return(FS_SRB_SLOWSWITCH);
    }
    

    /*
     * Add the RSRB header.
     */
    RSRB_HDR(data, vrp, rn, srb_pkt_size, op);

    /*
     * Adjust datagram size and start - after we are sure the pkt is
     *  being switched.
     */
    pak->datagramsize = (hdrsize + srb_pkt_size);
    pak->datagramstart = dgstart;
    return(rsrb_fastsend(pak, ihwidb, ohwidb, rsrb_direct_str, rn, vrp));
}


/*
 * update fst ip hdr
 */
static inline void 
fs_fst_update_iphdr (tr_vpeertype *vrp, iphdrtype *iphdr)
{
    RSRB_FST_INCREMENT_SEND(vrp, TRUE, iphdr->id);
    iphdr->checksum = ipcrc((ushort *) iphdr, ltob(iphdr->ihl));
}

/*
 * output an SRB packet using fst encaps rsrb. 
 * Called via tr_vpeertype->vrp_fast_lanout() vector
 *
 * Don't change pak struct fields unless packet is actually sent.
 */

static int
fs_rsrb_fst_output (
    hwidbtype *ihwidb,
    tr_vpeertype *vrp,
    tr_ventrytype *vre,
    ushort rn,
    paktype *pak)
{
    ipcache_rntype *cptr;
    hwidbtype *ohwidb;
    ulong    ohwidb_status;
    ulong ip_pkt_size;
    ulong rsrb_op;
    iphdrtype *iphead;
    uchar *hdr_ptr;
    uchar *dgstart;
    ulong dgsize;


    ENTER_RSRB("fs_rsrb_fst_output");

    if (vrp->ip_header == NULL)
        return(FS_SRB_SLOWSWITCH);

    /*
     * get ip cache entry.
     */
    cptr = ipcache_lookup_fast(vrp->ipaddr);
    if (!cptr) {
	ihwidb->counters.cache_miss[ACCT_PROTO_IP]++;
        return(FS_SRB_SLOWSWITCH);
    }
    ohwidb = cptr->idb->hwptr;
    ohwidb_status = ohwidb->status;

    ip_pkt_size = pak->datagramsize + FST_IP_HDR_SIZE + TR_FST_VR_HDR_SIZE;

        /*
         * should have an op to forward fddi crc
         */
    if (pak->fcs_type == FCS_TYPE_TR) {
        rsrb_op = RSRB_OP_FORWARDFST;
        ip_pkt_size += TR_FCS_SIZE;
    } else {
        rsrb_op = RSRB_OP_FORWARDFSTCRC;		   /* no crc */
    }
        

    if (ip_pkt_size >= ohwidb->maxmtu)
        return(FS_SRB_SLOWSWITCH);


    iphead = vrp->ip_header;
    iphead->tl = ip_pkt_size;
    iphead->checksum = 0;                       

    fs_fst_update_iphdr(vrp, iphead);
    
    /*
     * write new mac header and IP header
     */
    if (ohwidb_status & IDB_FDDI) {

	dgsize = ip_pkt_size + FDDI_SNAP_ENCAPBYTES;
        dgstart = pak->datagramstart -
		  (FDDI_SNAP_ENCAPBYTES + FST_IP_HDR_SIZE + TR_FST_VR_HDR_SIZE);
	pak->network_start = dgstart + FDDI_SNAP_ENCAPBYTES;
        hdr_ptr = fst_write_fddi_header(dgstart, cptr, (ulong *)iphead);
        
    } else if (ohwidb_status & IDB_ATM) {

	/*
	 * ATM VC accounting should be done by caller and in fastsend()
	 */
	dgsize = ip_pkt_size + cptr->length;  /* XXX right ??? */
        dgstart = pak->datagramstart - cptr->length -
		  (FST_IP_HDR_SIZE + TR_FST_VR_HDR_SIZE);
	pak->network_start = dgstart + cptr->length;
        hdr_ptr = fst_write_atm_header(dgstart, cptr, (ulong *)iphead);

    } else if (ohwidb_status & IDB_ETHER) {

	dgsize = ip_pkt_size + ETHER_ARPA_ENCAPBYTES;
        dgstart = pak->datagramstart -
		 (ETHER_ARPA_ENCAPBYTES + FST_IP_HDR_SIZE + TR_FST_VR_HDR_SIZE);
	pak->network_start = dgstart + ETHER_ARPA_ENCAPBYTES;
        hdr_ptr = fst_write_ether_header(dgstart, cptr, (ulong *)iphead);

    } else if (ohwidb_status & IDB_SERIAL) {

        if (ohwidb->enctype != ET_HDLC)
            return(FS_SRB_SLOWSWITCH);

	dgsize = ip_pkt_size + HDLC_HEADER_SIZE;
        dgstart = pak->datagramstart -
		 (HDLC_HEADER_SIZE + FST_IP_HDR_SIZE + TR_FST_VR_HDR_SIZE);
	pak->network_start = dgstart + HDLC_HEADER_SIZE;
        hdr_ptr = fst_write_serial_header(dgstart, cptr, (ulong *)iphead);

    } else if (ohwidb_status & IDB_TR) {

        riftype *rifptr = NULL;
	int riflen = 0;

        if (srb_multiring_any_enabled(ohwidb->firstsw)) {
	    rifptr = get_rif_inline(ohwidb, cptr->m.macstring);
            if (!rifptr)
                return(FS_SRB_SLOWSWITCH);
            riflen = rifptr->length;
        }

	dgsize = ip_pkt_size + riflen + TRING_SNAP_ENCAPBYTES;
        dgstart = pak->datagramstart - riflen -
		 (TRING_SNAP_ENCAPBYTES + FST_IP_HDR_SIZE + TR_FST_VR_HDR_SIZE);
	pak->network_start = dgstart + riflen + TRING_SNAP_ENCAPBYTES;
        hdr_ptr = fst_write_token_header(dgstart,cptr,rifptr,(ulong *)iphead);

    } else {
        return(FS_SRB_SLOWSWITCH);               /* return to process level */
    }
    

    TR_FST_VR_HDR(hdr_ptr, vrp, rn, rsrb_op);   /* write vr header for fst */

    /*
     * don't change orig pak fields till we are sure we will send it
     */
    pak->datagramsize = dgsize;
    pak->datagramstart = dgstart;
    pak->linktype = LINK_IP;
    return(rsrb_fastsend(pak, ihwidb, ohwidb, rsrb_fst_str, rn, vrp));
}

/*
 * Do proxy explorer processing on rsrb
 */
static inline void 
process_proxy_explorer (trrif_t *trh, tr_vgrouptype *vrg)
{
    if (global_proxy_explorer_enable) {

        llc1_hdr_t *llc1;
        int riflen;

        riflen = get_tr_riflen((tring_hdr *)trh);
        llc1 = (llc1_hdr_t *)((uchar *)&trh->rc_blth + riflen);
        if (((llc1->control & ~LLC1_P) == LLC1_XID) ||
            ((llc1->control & ~LLC1_P) == LLC1_TEST)) {
            rif_update(NULL, vrg->vrn, trh->saddr, NULL,
                       (srbroute_t *) &trh->rc_blth, RIF_AGED_REM);
        }
    }
}


/*
 * Receive a direct encaps RSRB packet and forward the extracted frame out
 * a (tring) interface.  The RSRB frame must be a 'FORWARD' to be handled by
 * this routine.  Anything not understood is thrown up to the system
 * level code.  Interfaces with Netbios 'Byte' access lists cannot be
 * handled fast.
 *
 * NOTE: caller must set up pak->network_start == snap_hdr->data
 *
 * called via reg_invoke_rsrb_direct_input
 *
 * return TRUE if forwarded or dropped.
 */
static boolean
fs_rsrb_direct_input (hwidbtype *ihwidb, paktype *pak)
{
    tr_vgrouptype *vrg;
    tr_ventrytype *vre;
    int srb_pkt_size;
    hwidbtype *ohwidb;
    tr_vhdrtype *vrh;
    trrif_t *trh;
    tr_vpeertype *vrp;
    int riflen;
    
    ENTER_RSRB("fs_rsrb_direct_input");
    
    vrh = (tr_vhdrtype *)pak->network_start;
    srb_pkt_size = vrh->len;

    /*
     * Ignore the version number.  Handle FORWARD command.
     * xxx why not handle crc ops too?
     */
    if (vrh->op != RSRB_OP_FORWARD)
        return(FALSE);

    /*
     * Is this frame traced?
     * pick up ac/fc from tr header 
     */    
    trh = (trrif_t *)((uchar *)vrh + TR_VHDRTYPE_SIZE);
    if (trh->rc_blth & (RC_BROADBIT | RC_TRACE_BOTH))
      return(FALSE);

    riflen = getRifLenPak(trh->daddr, TR_RII);

    /*
     * If the target ring is zero, or the target ring is the same as the
     * virtual ring, then the packet is for this router.  Otherwise, find
     * the ring group.
     */

    if (!vrh->trn || (vrh->trn == vrh->vrn))
        return(FALSE);

    vrg = vrfind_ringgroup(vrh->vrn);
    
    if (!vrg) {
        srb_sb_incr_rx_drops(ihwidb->firstsw);
        datagram_done(pak);
        if (srb_debug || srberr_debug)
	    rsrb_in_buginf(pak, ihwidb, rsrb_vrg_err, NULL, rsrb_direct_str, 
			   vrh->trn);
        return(TRUE);
    }
 
    vre = vrfind_rn2entry(vrh->trn,
                          RSRB_BN_DONTCARE, 
                          (tr_ventrytype *)&vrg->rings.qhead,
                          RSRB_ET_LOCAL);
    vrp = (tr_vpeertype *) ihwidb->tr_ifpeer;
    if ((!vre) || (!vrp)) {
        srb_sb_incr_rx_drops(ihwidb->firstsw);
        if (srb_debug || srberr_debug)
	    rsrb_in_buginf(pak, ihwidb, rsrb_vre_vrp_err, NULL, rsrb_direct_str,
			   vrh->trn);
        datagram_done(pak);
        return(TRUE);
    }

    if (vre->type == RSRB_ET_LOCAL_VIRTUAL) {
        ohwidb = vre->outhole.idb;
        if (ohwidb->span_bridge_process_force) 
            return(FALSE);

        if (ohwidb->state != IDBS_UP) {
            datagram_done(pak);
            return(TRUE);
        }

        if (ihwidb->srb_access_in) {

            if (!srb_in_check((uchar *)trh, srb_pkt_size, ihwidb, riflen)) {
                srb_sb_incr_rx_drops(ihwidb->firstsw);
                datagram_done(pak);
                if (srberr_debug)
	                rsrb_in_buginf(pak, ihwidb, srb_access_err, ohwidb,
			                        rsrb_direct_str, vrh->trn);
                return(TRUE);
            }
        }

	/* Convert packet to SRB and forward to VBRIDGE translator. */

        pak->datagramstart = (uchar *)trh;
	pak->datagramsize  = srb_pkt_size - TR_FCS_SIZE;
        pak->if_input      = ihwidb->firstsw;
	pak->if_output     = ohwidb->firstsw;
	pak->riflen        = riflen;
	pak->linktype      = LINK_RSRB;
	pak->rxtype        = RXTYPE_UNKNOWN;
        pak->acct_proto    = ACCT_PROTO_SRB;

        pak_set_dirty(pak);

	if ((*ohwidb->vbridge_forward)(pak, ohwidb)) {
            srb_rx_counters(ihwidb, srb_pkt_size);
            srb_sb_incr_rx_counters(vre->virtual_idb->firstsw, srb_pkt_size);
            vre->forwards++;
	}
	return(TRUE);
    }

    if (vre->type != RSRB_ET_LOCAL)
        return(FALSE);

    ohwidb = vre->outhole.idb;

    /*
     * Don't fastswitch the frame if the user doesn't want it done.
     */
    if (ohwidb->state != IDBS_UP)
        return(FALSE);
    if (!ohwidb->srb_routecache)
        return(FALSE);
    
    /*
     * check access lists.  
     */
    if (ohwidb->srb_access_out || global_proxy_explorer_enable ||
        global_netbios_name_cache_enable) {

        if (no_fs_srb_out_acc_chk(ohwidb))
            return(FALSE);

        /*
         * Do proxy explorers, if needed
         */
        process_proxy_explorer(trh, vrg);

        /*
         * Do NetBIOS name caching, if needed
         */
        if (global_netbios_name_cache_enable) {
            netbios_name_cache_update_pak((uchar *) trh, NETBIOS_NAME_AGED_REM,
                                          NULL, vrg->vrn);
        }

        if ((ohwidb->srb_access_out) &&
            (!srb_out_check((uchar *)trh, srb_pkt_size, ohwidb,
			    get_tr_riflen((tring_hdr *)trh)))) {
            srb_sb_incr_tx_drops(ohwidb->firstsw);
            if (srb_debug || srberr_debug)
	        rsrb_in_buginf(pak, ihwidb, srb_access_err, ohwidb,
			       rsrb_direct_str, vrh->trn);
            datagram_done(pak);
            return(TRUE);
        }
    }
    srb_rx_counters(ihwidb, srb_pkt_size);
    vrp->pkts_rx++;

	/*
	 * change size if we ever handle fddi/srb in (RSRB_OP_FORWARDCRC)
	 */
    pak->datagramsize = srb_pkt_size - TR_FCS_SIZE;
    pak->datagramstart = (uchar *)trh;
    pak->fcs_type = FCS_TYPE_TR;        /* should pass this thru rsrb */
    pak->acct_proto = ACCT_PROTO_SRB;   /* for fastsend() accting */
    pak->linktype = LINK_RSRB;
    pak->if_output = ohwidb->firstsw;
    trh->ac = AC_PRI4;
    trh->fc = FC_TYPE_LLC;


    switch((*ohwidb->if_srb_fastout)(ohwidb, pak)) {
        case FS_SRB_SENT:
            vre->forwards++;
            if (srb_debug)
	        rsrb_in_buginf(pak, ihwidb, srb_fwd, ohwidb,
			       rsrb_direct_str, vrh->trn);
            return(TRUE);

        default:
	    datagram_done(pak); /* fall thru to drop - prevents out of order */

        case FS_SRB_DROPPED:
            srb_sb_incr_tx_drops(ohwidb->firstsw);
            if (srb_debug || srberr_debug)
	        rsrb_in_buginf(pak, ihwidb, srb_odrop_err, ohwidb,
			       rsrb_direct_str, vrh->trn);
            return(TRUE);
    }
}


/*
 * Receive an fst encaps RSRB packet and forward the extracted frame out
 * a (tring) interface.  bump up to process level if we can't fast switch it.
 *
 * NOTE: caller must set up pak->network_start == snap_hdr->data
 *
 * called via reg_invoke_rsrb_fst_input()
 *
 * return TRUE if forwarded or dropped.
 */
static boolean
fs_rsrb_fst_input (hwidbtype *ihwidb, paktype *pak)
{
    hwidbtype *ohwidb;
    tr_vgrouptype *vrg;
    tr_ventrytype *vre;
    tr_vpeertype *vrp;
    ushort seq_num;
    ushort srb_pkt_size;
    iphdrtype *ip;
    ipaddrtype ipsrcadr;
    ipaddrtype ipdstadr;
    tr_fst_vhdrtype *vrh;
    boolean fst_result;
    trrif_t *trh;

    ENTER_RSRB("fs_rsrb_fst_input");

    /*
     * Assume that IP fastswitching code did the
     * necessary checks on this packets.
     * 1) IP Checksum is correct.
     * 2) This packet is not an IP fragment.
     * 3) There are no IP options.
     *
     * since this is called after trying to fastswitch it as
     * an ip packet, it fell into here.
     */

        /*
	 * don't use ipheadstart().  No guarantee it will use network_start
	 * which is all thats valid.
	 */
    ip = (iphdrtype *)pak->network_start;
    ipsrcadr = GETLONG(&ip->srcadr);
    ipdstadr = GETLONG(&ip->dstadr);

    /*
     * If it is not for us, don't take it in!
     */
    if (ipdstadr != rsrb_fst_name)
        return(FALSE);
    
    /* only handle FST forward frames */
    vrh = (tr_fst_vhdrtype *)((uchar *)ip + MINIPHEADERBYTES);
    if (vrh->op != RSRB_OP_FORWARDFST)
        return(FALSE);

    /* Is this frame traced? */
    trh = (trrif_t *)((uchar *)vrh + TR_FST_VR_HDR_SIZE);
    if (trh->rc_blth & (RC_BROADBIT | RC_TRACE_BOTH))
        return(FALSE);
    
    /*
     * If the target ring is zero, the packet is for this router.
     * Otherwise, find the ring group.
     */
    if (!vrh->trn || (vrh->trn == vrh->vrn))
        return(FALSE);

    vrg = vrfind_ringgroup(vrh->vrn);

    if (!vrg) {
        srb_sb_incr_rx_drops(ihwidb->firstsw);
        if (srb_debug || srberr_debug)
	    rsrb_in_buginf(pak, ihwidb, rsrb_vrg_err, NULL, rsrb_fst_str,
			   vrh->trn);
        datagram_done(pak);
        return(TRUE);
    }

    vrp = vrfind_peer(vrg, RSRB_PT_FST, (rsrb_cookie)ipsrcadr, 0, 0, NULL);
    vre = vrfind_rn2entry(vrh->trn, 
                          RSRB_BN_DONTCARE,
                          (tr_ventrytype *)&vrg->rings.qhead,
                          RSRB_ET_LOCAL);
    if (!vre || !vrp) {
        srb_sb_incr_rx_drops(ihwidb->firstsw);
        if (srb_debug || srberr_debug)
	    rsrb_in_buginf(pak, ihwidb, rsrb_vre_vrp_err, NULL, rsrb_fst_str,
			   vrh->trn);
        datagram_done(pak);
        return(TRUE);
    }

    if (vre->type == RSRB_ET_LOCAL_VIRTUAL) {
        ohwidb = vre->outhole.idb;
        if (ohwidb->span_bridge_process_force)
        return(FALSE);

        if (ohwidb->state != IDBS_UP) {
            datagram_done(pak);
            return(TRUE);
        }

        srb_pkt_size = ip->tl - (FST_IP_HDR_SIZE + TR_FST_VR_HDR_SIZE);

        /* Packet data past network hdr in CPU cache needs to be flushed */
        pak_set_dirty(pak);

        if (ihwidb->srb_access_in) {

            if (!srb_in_check((uchar *)trh, srb_pkt_size, ihwidb,
                                         getRifLenPak(trh->daddr, TR_RII))) {
                srb_sb_incr_rx_drops(ihwidb->firstsw);
                datagram_done(pak);
                if (srberr_debug)
                        rsrb_in_buginf(pak, ihwidb, srb_access_err, ohwidb,
                                                   rsrb_fst_str, vrh->trn);
                return(TRUE);
            }
        }

        pak->datagramstart = (uchar *)trh;
        pak->datagramsize  = srb_pkt_size - TR_FCS_SIZE;
        pak->if_input      = ihwidb->firstsw;
        pak->if_output     = ohwidb->firstsw;
        pak->riflen        = getRifLenPak(trh->daddr, TR_RII);
        pak->linktype      = LINK_RSRB;
        pak->rxtype        = RXTYPE_UNKNOWN;
        pak->acct_proto    = ACCT_PROTO_SRB;

        if ((*ohwidb->vbridge_forward)(pak, ohwidb)) {
            srb_rx_counters(ihwidb, srb_pkt_size);
            srb_sb_incr_rx_counters(vre->virtual_idb->firstsw, srb_pkt_size);
            vre->forwards++;
        }
        return(TRUE);
    }

    if (vre->type != RSRB_ET_LOCAL)
        return(FALSE);

    ohwidb = vre->outhole.idb;

    srb_pkt_size = ip->tl - (FST_IP_HDR_SIZE + TR_FST_VR_HDR_SIZE);

    /*
     * Don't fastswitch the frame if the user doesn't want it done.
     */
    if (ohwidb->state != IDBS_UP)
        return(FALSE);
    if (!ohwidb->srb_routecache) {
        return(FALSE);
    }

    process_proxy_explorer(trh, vrg);


    /*
     * Do NetBIOS name caching, if needed
     */
    if (global_netbios_name_cache_enable) {
        netbios_name_cache_update_pak((uchar *) trh, NETBIOS_NAME_AGED_REM,
                                      NULL, vrg->vrn);
    }

    /*
     * check access lists
     */
    if (ohwidb->srb_access_out) {

        if (no_fs_srb_out_acc_chk(ohwidb))
             return(FALSE);

        if (!srb_out_check((uchar *)trh, srb_pkt_size, ohwidb,
			  get_tr_riflen((tring_hdr *)trh))) {
            srb_sb_incr_tx_drops(ohwidb->firstsw);
            if (srb_debug || srberr_debug)
	      rsrb_in_buginf(pak, ihwidb, srb_access_err, ohwidb, rsrb_fst_str,
			     vrh->trn);
            datagram_done(pak);
            return(TRUE);
        }
    }

    /*
     * sequence number is ip header id
     */
    seq_num = ip->id;       /* FST uses this for sequence # */

    RSRB_FST_ACCEPTOR(vrp, seq_num, fst_result)

    if (!fst_result) {
        srb_sb_incr_rx_drops(ihwidb->firstsw);
        vrp->drops++;
        if (srb_debug || srberr_debug) {
	    rsrb_in_buginf(pak, ihwidb, "acceptor failed", ohwidb, rsrb_fst_str,
			   vrh->trn);
            buginf("\nRSRB:   fst seq num in %d, int val %d  proc val %d",
		    seq_num, vrp->ip_int_seqnumin, vrp->ip_seqnumin);
	}
        datagram_done(pak);
        return(TRUE);
    }

    srb_rx_counters(ihwidb, srb_pkt_size);
    vrp->pkts_rx++;

	/*
	 * change size if we ever handle fddi/srb in (RSRB_OP_FORWARDCRC)
	 */
    pak->datagramsize = srb_pkt_size - TR_FCS_SIZE;
    pak->datagramstart = (uchar *)trh;
    pak->fcs_type = FCS_TYPE_TR;        /* should pass this thru rsrb */
    pak->acct_proto = ACCT_PROTO_SRB;   /* for fastsend() accting */
    pak->linktype = LINK_RSRB;
    pak->if_output = ohwidb->firstsw;
    trh->ac = AC_PRI4;
    trh->fc = FC_TYPE_LLC;

    switch((*ohwidb->if_srb_fastout)(ohwidb, pak)) {
        case FS_SRB_SENT:
            vre->forwards++;
            if (srb_debug)
	        rsrb_in_buginf(pak, ihwidb, srb_fwd, ohwidb, rsrb_fst_str,
			       vrh->trn);
            return(TRUE);

        default:
	    datagram_done(pak); /* fall thru to drop - prevents out of order */

        case FS_SRB_DROPPED:
            srb_sb_incr_tx_drops(ohwidb->firstsw);
            if (srb_debug || srberr_debug)
	        rsrb_in_buginf(pak, ihwidb, srb_odrop_err, ohwidb,
			       rsrb_fst_str, vrh->trn);
            return(TRUE);
    }
}

/*
 * throw away an srb packet
 *
 * This vector should NOT be used to disable fastswitching.
 *
 * this would only get called if the user is allowed to config an
 * interface for srb that doesn't support srb - like ethernet.
 * it's effectively a bug.
 */
int no_srb_fastout (hwidbtype *ohwidb, paktype *pak)
{
    if (srb_debug || srberr_debug) {
        idbtype   *iswidb = pak->if_input;
        hwidbtype *ihwidb = iswidb->hwptr;
        srb_in_buginf(pak, ihwidb, srb_odrop_err, ohwidb,
          "srb not supported", srb_sb_get_targetring(iswidb), NULL);
    }
    srb_sb_incr_tx_drops(ohwidb->firstsw);
    datagram_done(pak);
    return(FS_SRB_DROPPED);
}


/*
 * Construct fast explorer pak and output to interface.
 */
boolean fs_explorer_out (
    paktype *ipak,
    fs_hdrtype *hcb,
    hwidbtype *ohwidb)
{
    paktype *opak;
    ulong   hdrsize, dgsize;

    hdrsize = TRING_ENCAPBYTES + hcb->newriflen;
    dgsize = hcb->pakdatasize + hdrsize;

    /*
     * NOTE: don't output the data in hcb because FDDI will
     * twist the hcb->newtrh bits.
     *
     * (tbd: create platform specific fs_get_pak()
     *  instead of using getbuffer() routine -
     *   on les just call getbuffer.
     *   on RSP would get pak with memd buffer.)
     */

    opak = getbuffer(dgsize);
    if (!opak) {
	hcb->iidb->fs_expl_input->out_drops++;
        return (FALSE);
    }

    /* 
     * copy explorer AC|FC bytes, TR hdr, rif and data into datagram
     */
    hcb->newtrh->ac = AC_PRI4;
    hcb->newtrh->fc = FC_TYPE_LLC;

    bcopy((char *)hcb->newtrh, opak->datagramstart, hdrsize);
    bcopy((char *)hcb->pakdataptr, opak->datagramstart+hdrsize,
                  hcb->pakdatasize);

    opak->datagramsize = dgsize;
    opak->fcs_type = FCS_TYPE_NONE;   /* don't use input crc */
    opak->if_input = ipak->if_input;  /* not set_if_input() - don't want */
				      /*  input accounting on explorers */

    opak->acct_proto = ACCT_PROTO_SRB;
    opak->linktype = LINK_RSRB;
    opak->if_output = ohwidb->firstsw;

        /*
         * Output the explorer 
         */
    switch (ohwidb->if_srb_fastout(ohwidb, opak)) {

        case FS_SRB_SENT:
            return(TRUE);

        default:
	    datagram_done(opak); /* fall thru to drop - prevents out of order */

        case FS_SRB_DROPPED:
            srb_sb_incr_tx_drops(ohwidb->firstsw);
            return(FALSE);
    }
}
/********************************************************************/
/*
 * subsystem init
 * r/srb init routines
 *
 */

void
fs_srb_setup (void)
{
    hwidbtype *idb, *ohwidb;
    tr_ventrytype *vring;
    tr_vgrouptype *vrg;

    FOR_ALL_HWIDBS(idb) {

        idb->tr_srb_fastswitch = (tr_srb_fastswitch_type) return_false;
           /*
            * routine for platform/media specific srb output conversions
            * this gets called by things like fast explorers even if
            * if idb->srb_routecache is disabled.  So it should be
            * valid all the time.
            *
            * this vector should NOT be used to disable fastswitching.
            */
        idb->if_srb_fastout = get_srb_fastout(idb);
	/* Set the other two variant's for sanity sake */
        idb->if_srb_hes_fastout = (if_srb_hes_fastout_type)no_srb_fastout;
	idb->if_srb_les_fastout = (if_srb_les_fastout_type)no_srb_fastout;

        /*
         * only used by hes and fastsrb_les.c
         */
        idb->rsrb_fst_fastout = (rsrb_fst_fastout_type) return_false;

        if (!is_srb_hw(idb))
          continue;

        if (!idb->srb_routecache)
            continue;

        /*
         * Don't allow srb fs on cards with input side netbios offset
         * lists or acess expressions.  This is because they need to look at
         * random offsets into the frame and the frame is not local to
         * this processor.
         *
         * xxx Above comment sounds like a hes holdover comment
         *  for cbus cards.
         *  this restriction may not apply for this code.
         *  RSP restrictions on touching data inside packet are handled by
         *  PAK_BUF_DIRTY flag;
         */
         if (idb->srb_access_in &&
              (netbios_sb_ibm_b_iacf_is_enabled(idb->firstsw) ||
              (netbios_bytes_accQ.qhead && idb->access_expression_in)))
                  continue;

        /*
         * Target is a virtual ring.
         */
        vring = idb->tr_vring_blk;
        if (vring) {
            idb->tr_srb_fastswitch = (tr_srb_fastswitch_type) fs_srb_to_vring;
            continue;   
        }

        /*
         * Target must be a real ring.  Can check for fast switching enabled
         * on the output interface here rather than in fs routine.
         * Cannot fs if netbios offset
         * lists or acess expressions are enabled on the output
         * interface (xxx cbus only?), so check for those as well.
         */
         ohwidb = idb->tr_bridge_idb;
         if (ohwidb) {
              if ((!ohwidb->srb_routecache) ||
                  (ohwidb->srb_access_out &&  no_fs_srb_out_acc_chk(ohwidb)))
                  continue;

              idb->tr_srb_fastswitch = (tr_srb_fastswitch_type)fs_srb_to_if;
        }
    }

    /* Initialize virtual bridge fast switch vectors. */

    for (vrg = (tr_vgrouptype *)vringQ.qhead; vrg; vrg = vrg->next) {
	if (vrg->virtual_bridge 
	&&  vrg->virtual_bridge->tbridge_media == TBR_VBRIDGE)
	    vrg->virtual_bridge->hwptr->tr_srb_fastswitch = 
			(tr_srb_fastswitch_type)fs_srb_to_vring;
    }
}

void
set_vring_fast_entry (hwidbtype *idb)
{

    tr_ventrytype *vre;

    vre = (tr_ventrytype *) idb->tr_vring_blk;
    for (; vre; vre = vre->next) {
        if (((vre->type == RSRB_ET_LOCAL) ||
             (vre->type == RSRB_ET_LOCAL_VIRTUAL)) &&
            (vre->outhole.idb == idb)) {
            break;
        }
    }

    if (!vre)
        return;

    /*
     * vre->vre_fastout only used by hes and fastsrb_les.c
     *  maybe should we #ifdef out for here
     */
    vre->vre_hes_fastout = (vre_hes_fastout_type)vre_fastout_err;
    vre->vre_les_fastout = (vre_les_fastout_type)vre_fastout_err;
}


void
set_vring_fast_peer (tr_vpeertype *vrp,
                     hwidbtype *idb)
{
    /*
     * BEWARE: the input arg "idb" is invalid for most of the
     *  types
     */

    if (vrp->type == RSRB_PT_IF || vrp->type == RSRB_PT_LAN) {

        vrp->vrp_fast_lanout = fs_rsrb_direct_output;        

    } else if (vrp->type == RSRB_PT_FST) {

        vrp->vrp_fast_lanout = fs_rsrb_fst_output;

    } else
        vrp->vrp_fast_lanout = NULL;

    /* Clear the other two variants for sanity sake. */
    vrp->vrp_hes_fast_lanout = NULL;
    vrp->vrp_les_fast_lanout = NULL;
}

static void fs_srb_init (subsystype* subsys) 
{
    reg_add_fs_rsrb_direct_input(fs_rsrb_direct_input, "fs_rsrb_direct_input");
    reg_add_fs_rsrb_fst_input(fs_rsrb_fst_input, "fs_rsrb_fst_input");

        /*
         * xxx this should use just one registry - but fast explorers init
	 *  code thinks TRIP and TMS380 use different fast srb code.
	 * actually no registry is needed for this unless you want
         *  to run srb w/o fastsrb support.
         */
                              /* platform specific */
    reg_add_mci_expl_vectors(fs_expl_vectors, "fs_expl_vectors");
    reg_add_les_expl_vectors(fs_expl_vectors, "fs_expl_vectors");
}


/*
 * Fast SRB subsystem header
 */
#define FS_SRB_MAJVERSION 1
#define FS_SRB_MINVERSION 0
#define FS_SRB_EDITVERSION 1

SUBSYS_HEADER(fs_srb,
              FS_SRB_MAJVERSION,
              FS_SRB_MINVERSION,
              FS_SRB_EDITVERSION,
              fs_srb_init,
              SUBSYS_CLASS_PROTOCOL,
              "seq: srb",
              "req: fastswitch, srb");


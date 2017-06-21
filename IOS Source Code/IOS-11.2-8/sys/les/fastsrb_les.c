/* $Id: fastsrb_les.c,v 3.11.4.11 1996/08/26 15:10:08 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/les/fastsrb_les.c,v $
 *------------------------------------------------------------------
 * fast srb and fast rsrb for DOSXX
 *
 * Feburary, 1992, Marciano Pitargue
 *
 * Copyright (c) 1991-1996, 97  by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: fastsrb_les.c,v $
 * Revision 3.11.4.11  1996/08/26  15:10:08  ppearce
 * CSCdi66582:  Implement SRB vector table to cut size of SRB_CORE subsys
 * Branch: California_branch
 *
 * Revision 3.11.4.10  1996/08/14  01:38:04  ppearce
 * CSCdi65943:  Input/Output route cache counters incorrect with RSRB
 * Branch: California_branch
 *   counters are incorrect with RSRB/FST as well as RSRB/direct
 *
 * Revision 3.11.4.9  1996/08/14  00:06:00  ppearce
 * CSCdi65943:  Input/Output route cache counters incorrect with RSRB
 * Branch: California_branch
 *
 * Revision 3.11.4.8  1996/08/11  23:34:00  ppearce
 * CSCdi61195:  RSRB performance degraded 40% from 11.1(1)
 * Branch: California_branch
 *   Erroneous RSRB debug processing -- missing check for debug enabled
 *
 * Revision 3.11.4.7  1996/07/23  18:47:23  ppearce
 * CSCdi63747:  Move srb_multiring out of SRB subblock
 * Branch: California_branch
 *   (1) srb_multiring doesn't really belong exclusively to SRB
 *         move srb_multiring out of SRB subblock and into swidb
 *         provide srb_multiring accessor routines
 *   (2) determine_rif() should take swidb as argument rather than hwidb
 *
 * Revision 3.11.4.6  1996/07/18  20:28:53  ppearce
 * CSCdi63366:  Move SRB counters from hwidb into SRB subblock
 * Branch: California_branch
 *   - Decreases size of hwidb for non-SRB'd interfaces
 *   - Provides additional IOS-independence for SRB
 *   - Moves SRB a little closer to being completely swidb-based
 *
 * Revision 3.11.4.5  1996/07/09  06:00:21  ppearce
 * CSCdi59527:  inlines waste code space
 * Branch: California_branch
 *   Just say "no" to inline abuse - SRB subblock inlines removed
 *
 * Revision 3.11.4.4  1996/06/12  00:16:10  fbordona
 * CSCdi60152:  DLSw merge of low-end and RSP fast switch code for
 *              modularity.
 * Branch: California_branch
 *
 * Revision 3.11.4.3  1996/05/17  11:26:05  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.9.2.6  1996/04/26  15:21:59  ppearce
 * IBU modularity - SRB subblock (part 1)
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.9.2.5  1996/04/03  19:57:01  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.9.2.4  1996/03/20  17:54:38  ppearce
 * IBU modularity - NetBios subblock
 * Branch: IbuMod_Calif_branch
 *   Code review comment - name changes only
 *
 * Revision 3.9.2.3  1996/03/19  02:04:56  ppearce
 * IBU modularity - NetBios subblock
 * Branch: IbuMod_Calif_branch
 *   Incorporate NetBios session access lists into NetBios subblock
 *
 * Revision 3.9.2.2  1996/03/17  18:06:35  ppearce
 * Sync IbuMod_Calif_branch to V111_1_3
 *
 * Revision 3.9.2.1  1996/03/16  12:09:44  ppearce
 * CSCdi50498:  IBU modularity - Implement NetBios subblock
 * Branch: IbuMod_Calif_branch
 *   Move NetBios fields from hwidbtype/idbtype structs to NetBios subblock
 *
 * Revision 3.11.4.2  1996/03/23  22:49:00  ppearce
 * SR/TLB fastswitching feature
 * Branch: California_branch
 *
 * Revision 3.11.4.1  1996/03/18  20:40:32  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.6.14.5  1996/03/16  07:11:33  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.6.14.4  1996/03/13  01:48:49  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.6.14.3  1996/03/07  10:05:06  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.6.14.2  1996/02/20  01:07:54  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.11  1996/03/07  06:34:20  kchiu
 * CSCdi40165:  Add netbios session level byte filtering support
 *
 * Revision 3.10  1996/02/29  00:19:27  kchiu
 * CSCdi46631:  lanmgr reporting erroneous Non-Broadcast frame counters
 *
 * Revision 3.9  1996/02/21  02:43:30  hampton
 * Clean up platform specific SRB defines in commonly included header
 * files.  [CSCdi48844]
 *
 * Revision 3.8  1996/02/13  22:23:57  fred
 * CSCdi47956:  WFQ severely degrades RSRB performance
 *         fix editing glitc
 *
 * Revision 3.7  1996/02/13  22:10:45  fred
 * CSCdi47956:  WFQ severely degrades RSRB performance
 *              drivers: increase TQL in short queue mode for link
 *              stability
 *              fastsrb_les.c: make sure pointers are there for queuing
 *              routines
 *
 * Revision 3.6.14.1  1996/01/24  22:22:17  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.6  1995/12/13  04:22:10  ahh
 * CSCdi44230:  Remove dead code
 * Bring out your dead... *clang* (RSRB/TCP fast-switching)
 *
 * Revision 3.5  1995/12/11  10:19:03  ppearce
 * CSCdi41891:  Both FS and Process counters are updated on the same packet
 *   Decrement FS counters when packet booted to process level
 *
 * Revision 3.4  1995/12/03  08:11:52  ppearce
 * CSCdi44430:  Packets not forwarded when RSRB-direct with custom-queueing
 *   Don't fast-switch packet if priority/custom queueing is enabled
 *     on the output interface
 *
 * Revision 3.3  1995/11/17  17:37:15  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:57:00  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:30:07  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.11  1995/09/28  20:24:17  rbatz
 * CSCdi38486:  CSCdi38486:  Fast-switched SRB can pad 4 bytes of data on
 *              frames.  The extra data is the Token Ring FCS.
 *              We pass it while Madge needs to recalculate it.
 *
 * Revision 2.10  1995/09/25  08:37:46  tli
 * CSCdi40960:  ISP jumbo patch 2
 *
 * Revision 2.9  1995/08/07  05:38:50  richl
 * CSCdi38315:  makefile rework
 *              move inlines to tring_inline and utilities to rif_util.c
 *
 * Revision 2.8  1995/08/01  22:03:55  motto
 * Changed fast explorer vector routine to keep track of drops instead
 * of the caller.
 * CSCdi37528:  CIP internal LAN SRB fastswitch explorer does not work
 *
 * Revision 2.7  1995/07/12  23:45:01  hampton
 * Convert TCP to fully use the passive timers macros instead of
 * referencing the system clock directly.  [CSCdi37015]
 *
 * Revision 2.6  1995/07/05 20:49:19  hampton
 * Consolidate common token ring variables into the idb, instead of having
 * them in the private data structure for each type of token ring
 * interface.  Remove overloading of the delay variable by token rings.
 * Eliminate direct references to the system clock.  [CSCdi36721]
 *
 * Revision 2.5  1995/06/26 20:44:27  rbatz
 * CSCdi30326:  Bytes and Packet information INVALID(sho int token ring)
 * When fast srb explorers were transmitted, outputs_fast[ACCT_PROTO_SRB]
 * and tx_cumbytes_fast[ACCT_PROTO_SRB] were not being incremented.  This
 * resulted in incorrect reporting of output stats for "show int tokX".
 *
 * Revision 2.4  1995/06/22  22:46:54  pilee
 * CSCdi27989:  Counters not correct on Sierra when SRB configured
 *
 * Revision 2.3  1995/06/21  03:00:53  gstovall
 * CSCdi35731:  Cleanup unused cruft in idb
 * Nuke some unused vectors and variables from the hwidb.
 *
 * Revision 2.2  1995/06/17  06:25:58  ahh
 * CSCdi35734:  TCP: Long overdue code reorganization
 * Separate tcp.h into public and private header files.  Shuffle functions
 * around into more logical groupings.  Rename functions to meet naming
 * conventions (say goodbye to StudlyCaps).  Delete unneeded externs.
 * Trim off old log messages.
 *
 * Revision 2.1  1995/06/07  21:30:58  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <ciscolib.h>
#include "interface_private.h"
#include "fastswitch_registry.h"
#include "../ip/ip_registry.h"
#include "../h/sys_registry.h"
#include "subsys.h"
#include "packet.h"
#include "../ui/debug.h"
#include "../wan/serial.h"
#include "../if/tring_common.h"
#include "../if/fddi.h"
#include "../if/ether.h"
#include "logger.h"
#include "../srt/srb.h"
#include "../srt/rsrb.h"
#include "../srt/srb_core.h"
#include "../srt/srb_error.h"
#include "../srt/srb_multiring.h"
#include "../ip/ipfast.h"
#include "../ibm/netbios.h"
#include "../ibm/netbios_sb.h"
#include "if_les_tms380.h"
#include "../if/rif.h"
#include "../if/rif_inline.h"
#include "fastsrb_les.h"


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
    srbV.srb_sb_incr_tx_nb_counters(firstsw_or_null(idb->vidb_link), dgsize);
    srbV.srb_sb_incr_rx_counters(firstsw_or_null(vre->virtual_idb), dgsize);
    vre->forwards++;
}


/**********************************************************************
 * 
 *		       FAST DIRECT RSRB OUTPUT
 *
 * This set of routines supports the direct encapsulation of RSRB packets
 * over the various types of interfaces.  This is the output (token ring
 * -> rsrb) path only.  The input path is elsewhere.  These routines do
 * not have to perform any assess checking as the interfaces involved are
 * transit interfaces, no destination interfaces.
 * 
 **********************************************************************/


#define TR_FST_VR_HDR(data, vrp, rn)	      				\
{	             		             		                \
    PUTLONG(((ulong *)data)++, ((vrp->vrp_version << 16) |		\
				RSRB_OP_FORWARDFST));                   \
    PUTLONG(((ulong *)data)++, ((rn << 16) | vrp->vrg->vrn));		\
    PUTSHORT(data++, 0);		             			\
}

#define FST_WRITE_TOKEN2_HEADER(data, cptr, rifptr, fst_hdr,		\
    size, rn, vrp) 							\
{									\
    ushort *rif;							\
    int i;								\
    *data++ = (AC_PRI4 << 8) | FC_TYPE_LLC;				\
    *data++ = cptr->m.macshort[0];					\
    *data++ = cptr->m.macshort[1];					\
    *data++ = cptr->m.macshort[2];					\
    if (rifptr && rifptr->length)					\
	*data++ = cptr->m.macshort[3] | (TR_RII << 8);			\
    else								\
	*data++ = cptr->m.macshort[3];					\
    *data++ = cptr->m.macshort[4];					\
    *data++ = cptr->m.macshort[5];					\
    if (rifptr && rifptr->length) {					\
	for (i=0, rif=(ushort *)rifptr->rif; i < rifptr->length; i+=2)	\
	    *data++ = *rif++;						\
    }									\
    PUTLONG(((ulong *)data)++, ((SNAPSNAP << 16) | ((LLC1_UI << 8) | 0)));\
    PUTLONG(((ulong *)data)++, TYPE_IP10MB);				\
    PUTLONG(((ulong *)data)++, fst_hdr[0]);				\
    PUTLONG(((ulong *)data)++, fst_hdr[1]);				\
    PUTLONG(((ulong *)data)++, fst_hdr[2]);				\
    PUTLONG(((ulong *)data)++, fst_hdr[3]);				\
    PUTLONG(((ulong *)data)++, fst_hdr[4]);				\
    TR_FST_VR_HDR(data, vrp, rn);			                \
}
							

/*
 * PACKET FORMAT FOR RSRB PACKETS
 *
 * Token Ring
 * ----------
 *      ACFC	AC/FC bits
 * DADA DADA	Destination Address
 * DADA SASA	Dest / Source
 * SASA SASA	Source Address
 * RIF INFO     rif info
 * .........	(variable length)
 * SNAP LLC1	SNAP is 0xAAAA / llc1
 * 0000 1996	0 / #define TYPE_RSRB 0x1996   (RSRB raw)
 *  VER 0001	Version / #define RSRB_OP_FORWARD 1
 *   RN  VRN    ring number / virtual ring number
 * 0000  LEN    offset (always 0) / length
 * PARM PARM	parameter (always zero, longword)
 * encapsulated packet which BTW has the ac/fc bits from the other ring
 *
 * Serial
 * ------
 * 0F00 1996	#define HDLC_RSRB 0x0F001996L (RSRB raw serial encapsulation)
 * 0001	  RN	#define RSRB_OP_FORWARD 1 / ring number
 * VRN  0000	virtual ring number / offset (always 0)
 *  LEN PARM	length / parameter (always zero, longword)
 * PARM encapsulated packet which BTW has the ac/fc bits from the other ring
 * 
 * Ethernet
 * --------
 * DADA DADA	Destination Address
 * DADA SASA	Dest / Source
 * SASA SASA	Source Address
 *  LEN SNAP	length / SNAP is 0xAAAA
 * LLC1	0000	llc1 / 0
 * 1996  VER	#define TYPE_RSRB 0x1996 (RSRB raw) / version
 * 0001	  RN	#define RSRB_OP_FORWARD 1 / ring number
 *  VRN 0000	virtual ring number / offset (always 0)
 *  LEN PARM 	length / parameter (always zero, longword)
 * PARM encapsulated packet which BTW has the ac/fc bits from the other ring
 *
 */

#define SRB_WRITE_SERIAL_HEADER(data)		\
    PUTLONG(((ulong *)data)++, HDLC_RSRB);


#define SRB_WRITE_ETHER_HEADER(data, dst, src, length)	\
    ieee_copy(dst, (uchar *)data);			\
    data += 3;						\
    ieee_copy(src, (uchar *)data);			\
    data += 3;						\
    *data++ = length;					\
    PUTLONG(((ulong *)data)++, ((SNAPSNAP << 16) | ((LLC1_UI << 8) | 0)));\
    PUTLONG(((ulong *)data)++, TYPE_RSRB);

inline static ushort *srb_write_fddi_header(ushort *data,
					    uchar *dst,
					    uchar *src)
{
#define cp_byte  *((uchar *)data)++

    cp_byte  = FDDI_ASYNCHRONOUS_FRAME     /* fc */
        | FDDI_48BIT_ADDRESS
	| FDDI_LLC_FRAME
	| FDDI_PRIORITY_ZERO;

    cp_byte = bit_swaps[*dst++];          /* dst */
    cp_byte = bit_swaps[*dst++];
    cp_byte = bit_swaps[*dst++];
    cp_byte = bit_swaps[*dst++];
    cp_byte = bit_swaps[*dst++];
    cp_byte = bit_swaps[*dst++];

    cp_byte = bit_swaps[*src++];          /* src */
    cp_byte = bit_swaps[*src++];
    cp_byte = bit_swaps[*src++];
    cp_byte = bit_swaps[*src++];
    cp_byte = bit_swaps[*src++];
    cp_byte = bit_swaps[*src++];
    PUTLONG(((ulong *)data)++, ((SNAPSNAP << 16) | 0x0300));
    PUTLONG(((ulong *)data)++, TYPE_RSRB);		
    return(data);
}


#define SRB_WRITE_TOKEN_HEADER(data, dst, src, rifptr)			\
{									\
    int i;								\
    ushort *rif;							\
    *data++ = (AC_PRI4 << 8) | FC_TYPE_LLC;				\
    ieee_copy(dst, (uchar *)data);					\
    data += 3;								\
    ieee_copy(src, (uchar *)data);					\
    if (rifptr && rifptr->length)					\
	*(uchar *)data |= TR_RII;				\
    data += 3;								\
    if (rifptr && rifptr->length) {					\
	for (i=0, rif=(ushort *)rifptr->rif; i < rifptr->length; i+=2)	\
            *data++ = *rif++;						\
    }									\
    PUTLONG(((ulong *)data)++, ((SNAPSNAP << 16) | ((LLC1_UI << 8) | 0)));\
    PUTLONG(((ulong *)data)++, TYPE_RSRB);				\
}

#define SRB_WRITE_TOKEN2_HEADER(data, odgsize, vrp, src, rn, rifptr)      \
{									  \
    ushort *rif;							  \
    int i;								  \
    *data++ = (AC_PRI4 << 8) | FC_TYPE_LLC;			  	  \
    ieee_copy(vrp->macaddr, (uchar *)data);				  \
    data += 3;								  \
    ieee_copy(src, (uchar *)data);					  \
    if (rifptr && rifptr->length)					  \
	*(uchar *)data |= TR_RII;  	      	 	  \
    data += 3;								  \
    if (rifptr && rifptr->length) {					  \
	for (i=0, rif=(ushort *)rifptr->rif; i < rifptr->length; i+=2)	  \
	    *data++ = *rif++;						  \
    }									  \
    PUTLONG(((ulong *)data)++, ((SNAPSNAP << 16) | ((LLC1_UI << 8) | 0)));\
    PUTLONG(((ulong *)data)++, TYPE_RSRB);				\
    PUTLONG(((ulong *)data)++, ((vrp->vrp_version << 16) | RSRB_OP_FORWARD)); \
    PUTLONG(((ulong *)data)++, ((rn << 16) | vrp->vrg->vrn));		  \
    PUTLONG(((ulong *)data)++, odgsize);	       			  \
    *data++ = 0;	/* param (long) */             			  \
    *data++ = 0;			             			  \
}

#define RSRB_HDR(data, vrp, rn, dgsize)                      \
{                                                            \
    PUTLONG(((ulong *)data)++,                               \
            (((vrp->vrp_version) << 16) | RSRB_OP_FORWARD)); \
    PUTLONG(((ulong *)data)++, ((rn << 16) | vrp->vrg->vrn));\
    PUTLONG(((ulong *)data)++, dgsize);                      \
    PUTLONG(((ulong *)data)++, 0);	        /* param (long)  */  \
}

/***********************************************************************
 *
 * Common pieces of code written as inline functions so that they can be
 * mixed and matched.  They cannot be written as inline functions because
 * of the number of parameters they generaate that are used later.
 *
 **********************************************************************/

#define TRH_ACL_BYTES (TR_MAXENCSIZE + NETBIOS_HEADER_SIZE)
static uchar trh_acl[TRH_ACL_BYTES];

static inline boolean fs_srb_access_check (hwidbtype *idb,
					   hwidbtype *oidb,
					   uchar *dgstart,
					   int dgsize,
					   int riflen)
{
    /*
     * Check the access lists.
     */
    if (idb->srb_access_in && !srb_in_check(dgstart, dgsize, idb, riflen))
	return(FALSE);
    if (oidb->srb_access_out && !srb_out_check(dgstart, dgsize, oidb, riflen))
	return(FALSE);
    return(TRUE);
}

static inline boolean fs_netbios_input_acl_check (hwidbtype *hwidb)
{
    return(netbios_sb_ibm_b_iacf_is_enabled(hwidb->firstsw) ||
           (netbios_bytes_accQ.qhead && hwidb->access_expression_in));
}
 
static inline boolean fs_netbios_output_acl_check (hwidbtype *hwidb)
{
    return(netbios_sb_ibm_b_oacf_is_enabled(hwidb->firstsw) ||
           (netbios_bytes_accQ.qhead && hwidb->access_expression_out));
}

static inline void srb_rx_counters (hwidbtype *idb, int length)
{
    GET_TIMESTAMP(idb->lastinput);
    idb->counters.inputs_fast[ACCT_PROTO_SRB]++; 
    idb->counters.rx_cumbytes_fast[ACCT_PROTO_SRB] += length;
}

static inline void srb_rx_backout_counters (hwidbtype *idb, int length)
{
    idb->counters.inputs_fast[ACCT_PROTO_SRB]--; 
    idb->counters.rx_cumbytes_fast[ACCT_PROTO_SRB] -= length;
}

static inline void srb_tx_counters (hwidbtype *idb, int dgsize)
{
    GET_TIMESTAMP(idb->lastoutput);
    TIMER_START(idb->xmittimer, TMS380_XMITTIMEOUT);
    idb->counters.outputs_fast[ACCT_PROTO_SRB]++;
    idb->counters.tx_cumbytes_fast[ACCT_PROTO_SRB] += dgsize;
}

static inline void rsrb_tx_counters (hwidbtype *oidb, hwidbtype *vidb, int dgsize)
{
    oidb->counters.outputs_fast[ACCT_PROTO_SRB]++;
    oidb->counters.tx_cumbytes_fast[ACCT_PROTO_SRB] += dgsize;
    if (vidb != NULL) {
        vidb->counters.outputs_fast[ACCT_PROTO_SRB]++;
        vidb->counters.tx_cumbytes_fast[ACCT_PROTO_SRB] += dgsize;
    }
}

static boolean fs_srb_madge_output (hwidbtype *idb,
				    hwidbtype *oidb,
				    paktype *pak,
				    int dgsize)
{
    ENTER_RSRB("fs_srb_madge_output");

    pak->datagramsize -=  TR_FCS_SIZE;
    pak->flags |= PAK_SRB_OUT;            /* nb - pass checksum */
    pak->acct_proto = ACCT_PROTO_SRB ;    /* say so */
    pak->linktype = LINK_SRB;
    pak->mac_start = pak->datagramstart+2;
    pak->if_output = oidb->firstsw;
    (*oidb->fastsend)(oidb, pak);

    if (srb_debug) {
        SrbDebug_FastForward(idb);
	DBG0(DBG_SRB, "\n       from: fs_srb_madge_output");
    }
    return(TRUE);
}

/*
 * Just like above routine, but sends the passed-in packet.
 */
static inline void fs_srb_madge_output_nocopy (hwidbtype *idb,
					       hwidbtype *oidb,
					       paktype *pak)
{
    pak->datagramsize -= TR_FCS_SIZE;
    pak->acct_proto = ACCT_PROTO_SRB ;    /* say so */
    pak->linktype = LINK_SRB;
    pak->mac_start = pak->datagramstart+2;
    pak->if_output = oidb->firstsw;
    (*oidb->fastsend)(oidb, pak);
    if (srb_debug) {
        SrbDebug_FastForward(idb);
    }
}

/**********************************************************************
 *
 * These routines handle all possible two port bridges in a router.
 *
 **********************************************************************/

/*
 * srb_madge2xxx
 *
 * From Madge to another token ring.
 */

static boolean srb_madge2xxx (hwidbtype *idb,
			      fm_receive_t *rx,
			      hwidbtype *oidb)
{
    paktype *pak    = rx->pak;
    uchar *dgstart  = (uchar *)pak->datagramstart; 
    int dgsize      = (int)pak->datagramsize;    /* includes the FCS */ 
    int riflen      = rx->rif_length; 

    ENTER_RSRB("srb_madge2xxx");
    srb_rx_counters(idb, dgsize);

    if (!fs_srb_access_check(idb, oidb, dgstart, dgsize, riflen)) {
	datagram_done(pak);
	return(TRUE);
    }
    if (oidb->state != IDBS_UP) {
	datagram_done(pak);
	return(TRUE);
    }
    if ((*oidb->if_srb_les_fastout)(idb, oidb, pak, pak->datagramsize)) {
        if (srb_debug) {
            SrbDebug_FastForward(idb);
        }
	return(TRUE);
    }
    srb_rx_backout_counters(idb, dgsize);
    return(FALSE);
}


/**********************************************************************
 *
 * These routines handle all token ring to virtual ring bridges.  Thes
 * routines only handle the direction of TR -> VRING.  Another set of
 * routines handles the opposite direction.
 *
 **********************************************************************/

/*
 * fs_to_nim_fst_vring
 * Fast switch SRB packet to virtual
 * ring using IP encapsulation.
 *
 * for fast switching fst stuff out to serial/ethernet/fddi
 *	idb->rsrb_fst_fastout = fs_to_nim_fst_vring;
 */

static int fs_to_nim_fst_vring (tr_vpeertype *vrp,
				tr_ventrytype *vre,
				ushort rn,
				paktype *pak,
				int dgsize,
				ipcache_rntype *cptr,
				hwidbtype *input)
{
    hwidbtype *oidb, *vidb;
    ulong    oidb_status;
    int totalsize, riflength;
    iphdrtype *iphead;
    char peerid[MAX_PEER_NAME];
    ushort *data;
    riftype *rifptr = NULL;

    ENTER_RSRB("fs_to_nim_fst_vring");

    oidb = cptr->idb->hwptr;
    oidb_status = oidb->status;

    DBG1(DBG_FST, "\n fs_to_nim_fst_vring: out to %s", oidb->name);
    
    totalsize = dgsize + FST_IP_HDR_SIZE + TR_FST_VR_HDR_SIZE;

    if (totalsize >= oidb->maxmtu)
	return(FALSE);
    if ((oidb_status & IDB_SERIAL) && (oidb->enctype != ET_HDLC))
	return(FALSE);
    iphead = vrp->ip_header;

    /* 
     * Now do necessary modifications to
     * ip/udp header.
     */
    RSRB_FST_INCREMENT_SEND(vrp, TRUE, iphead->id)

    iphead->tl = dgsize + FST_IP_HDR_SIZE + TR_FST_VR_HDR_SIZE;
    iphead->checksum = 0;			
    iphead->checksum = ipcrc((ushort *) iphead, ltob(iphead->ihl));

    
    /*
     * Set up write pointer to base of destination transmit window.
     * Do some input accounting.  Figure out how to write new MAC header
     * and write new mac header and UDP/IP header and copy the received
     * datagram after that.
     */
    if (oidb_status & IDB_ETHER) {
	totalsize = dgsize + ETHER_ARPA_ENCAPBYTES + FST_IP_HDR_SIZE 
	          + TR_FST_VR_HDR_SIZE;
        (uchar *)data = pak->datagramstart - (ETHER_ARPA_ENCAPBYTES +
					      FST_IP_HDR_SIZE +
					      TR_FST_VR_HDR_SIZE); 
        pak->datagramstart = (uchar *)data;
    	pak->network_start = ETHER_ARPA_ENCAPBYTES + (uchar *)data;
	FST_WRITE_ETHER_HEADER(data, cptr, ((ulong *)iphead));
    } else if (oidb_status & IDB_SERIAL) {
	totalsize = dgsize + HDLC_HEADER_SIZE + FST_IP_HDR_SIZE
	          + TR_FST_VR_HDR_SIZE;
        (uchar *)data = pak->datagramstart - (HDLC_HEADER_SIZE +  
				     FST_IP_HDR_SIZE +
				     TR_FST_VR_HDR_SIZE);
        pak->datagramstart = (uchar *)data;
    	pak->network_start = HDLC_HEADER_SIZE + (uchar *)data;
        FST_WRITE_SERIAL_HEADER(data, cptr, ((ulong *)iphead));
	
    } else if (oidb_status & IDB_FDDI) {
	rifptr = get_rif_inline(oidb, cptr->m.macstring);
	if (rifptr)
	    riflength = rifptr->length;
	else
	    riflength = 0;

	totalsize = dgsize + FDDI_SNAP_ENCAPBYTES + FST_IP_HDR_SIZE
	    + TR_FST_VR_HDR_SIZE + riflength;
        (uchar *)data = pak->datagramstart - (FDDI_SNAP_ENCAPBYTES + 
					      FST_IP_HDR_SIZE + 
					      TR_FST_VR_HDR_SIZE +
					      riflength);
        pak->datagramstart = (uchar *)data;
    	pak->network_start = FDDI_SNAP_ENCAPBYTES + (uchar *)data;
	data = fst_write_fddi_header(data, cptr, (ulong *)iphead);
    } else 
	return(FALSE);               /* return to process level */
    
    TR_FST_VR_HDR(data, vrp, rn);   /* write vr header for fst */
    pak->datagramsize = totalsize;

    pak->linktype = LINK_IP;
    pak->if_output = oidb->firstsw;
    (*oidb->fastsend)(oidb, pak);
    
    vidb = pak->if_input->hwptr->vidb_link;
    pak->acct_proto = ACCT_PROTO_SRB ;    /* say so */
    if (srb_debug) {
        buginf("\nRSRB%d: FAST forward to peer %s",
	       oidb->unit, vrpeer_id(vrp, peerid));
	DBG0(DBG_SRB, "\n       from: fs_to_nim_fst_vring");
    }

    return(TRUE);
} /* end of fs_to_nim_udp_vring */


/*
 * fs_to_madge_fst_vring
 * Fast switch SRB packet to virtual
 * ring using UDP/IP encapsulation.
 *
 * for fast switching fst stuff out to token ring
 */

static int fs_to_madge_fst_vring (tr_vpeertype *vrp,
				  tr_ventrytype *vre,
				  ushort rn,
				  paktype *pak,
				  int dgsize,
				  ipcache_rntype *cptr,
				  hwidbtype *input)
{
    hwidbtype *oidb;
    ushort *odata;
    riftype *rifptr = NULL;
    int hdrsize;
    int totalsize;
    iphdrtype *iphead;
    char peerid[MAX_PEER_NAME];

    ENTER_RSRB("fs_to_madge_fst_vring");

    oidb = cptr->idb->hwptr;
 
    iphead = vrp->ip_header;

    hdrsize = TRING_SNAP_ENCAPBYTES + FST_IP_HDR_SIZE + TR_FST_VR_HDR_SIZE;

    if (srb_multiring_any_enabled(oidb->firstsw)) {
	rifptr = get_rif_inline(oidb, cptr->m.macstring);
	if (!rifptr)
	    return(FALSE);
	hdrsize += rifptr->length;
    }
    totalsize = hdrsize + dgsize;
    if (totalsize >= oidb->maxmtu) {
	return(FALSE);
    }

    /* Now do necessary modifications to ip header */
    
    RSRB_FST_INCREMENT_SEND(vrp, TRUE, iphead->id);
    
    iphead->tl = FST_IP_HDR_SIZE + TR_FST_VR_HDR_SIZE + dgsize;
    iphead->checksum = 0;			
    iphead->checksum = ipcrc((ushort *) iphead, ltob(iphead->ihl));

    /* set up transmit descriptor, copy data in */

    pak->datagramstart -= hdrsize;
    odata = (ushort *)pak->datagramstart;  
    pak->network_start = TRING_SNAP_ENCAPBYTES + (uchar *)odata;
    FST_WRITE_TOKEN2_HEADER(odata, cptr, rifptr, 
			    ((ulong *)iphead),
			    dgsize, rn, vrp);   /* dgsize is not used */

    /* packet and count include FCS */
    pak->datagramsize = totalsize;
    pak->linktype = LINK_IP;
    pak->if_output = oidb->firstsw;
    pak->acct_proto = ACCT_PROTO_SRB ;    /* say so */
    (*oidb->fastsend)(oidb, pak);
   
    if (srb_debug)
	buginf("\nRSRB%d: fst fast forward rn %d to peer %s",
	       oidb->unit, rn, vrpeer_id(vrp, peerid));

    return(TRUE);
}

/*
 * fs_to_fst_vring
 * Fast switch SRB packet to virtual
 * ring using FST encapsulation.
 *
 * fastswitching fst or tcp packets out to a lan by calling
 * fs_to_nim_fst_vring() or fs_to_madge_fst_vring().
 */

static int fs_to_fst_vring (hwidbtype *input,
			    tr_vpeertype *vrp,
			    tr_ventrytype *vre,
			    ushort rn,
			    paktype *pak,
			    int dgsize )
{
    hwidbtype *idb;
    ipcache_rntype *cptr = NULL;

    ENTER_RSRB("fs_to_fst_vring");

    if (vrp->ip_header == NULL)
	return(FALSE);
    
    /*
     * lookup ip cache to see if we have valid
     * entry.
     */
    cptr = ipcache_lookup_fast(vrp->ipaddr);
    if (cptr) {
	idb = cptr->idb->hwptr;
	if (idb->rsrb_fst_fastout)
	    return((*idb->rsrb_fst_fastout)(vrp, vre, rn, pak, 
                    pak->datagramsize, cptr, input));
    } else {
	input->counters.cache_miss[ACCT_PROTO_IP]++;
    }
    return(FALSE);
} 

/*
 * token ring to virtual ring transport function.
 */
static boolean srb_madge_to_vring (hwidbtype *idb,
				   fm_receive_t *rx,
				   hwidbtype *dummy)
{
    paktype *pak    = rx->pak;
    uchar *dgstart  = (uchar *)pak->datagramstart; 
    int dgsize      = (int)pak->datagramsize;    /* includes the FCS */ 
    int riflen      = rx->rif_length;

    hwidbtype *oidb;
    ushort bn;
    ushort rn = 0;
    srbrd_t *rd;
    tr_vpeertype *vrp;
    tr_ventrytype *vre;
    ushort *us, rc;

    ENTER_RSRB("srb_madge_to_vring");

    srb_rx_counters(idb, dgsize);

    /*
     * Gather all the information we will need to forward this frame.
     * Putting all the bail checks at the end means less if statements to
     * slow down the code.
     */
    us = (ushort *)(dgstart + TRING_ENCAPBYTES);
    rc = us[0];
    if (rc & RC_D) {
	rd = (srbrd_t *) us+1;
	rn = us[((rx->rd_offset - TRING_ENCAPBYTES) >> 1) - 1] >> 4;
	bn = us[((rx->rd_offset - TRING_ENCAPBYTES) >> 1) - 1] & 0x0f;
    } else {
	rd = (srbrd_t *) us+((rc >> 9) & (RC_LTHMASK >> 1))-1;
	bn = us[((rx->rd_offset - TRING_ENCAPBYTES) >> 1) + 1] & 0x0f;
	rn = us[((rx->rd_offset - TRING_ENCAPBYTES) >> 1) + 2] >> 4;
    }

    /*
     * Traced' frames must be handed by the system
     * so that the appropriate response may be generated. 
     */
    if (rc & ((RC_BROADBIT | RC_TRACE_BOTH) << 8)) {
        srb_rx_backout_counters(idb, dgsize);
	return(FALSE);
    }

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
    if ((rd->ring_num == srbV.srb_sb_get_targetring(idb->firstsw)) &&
        (((trrif_t *)dgstart)->saddr[0] & TR_RII)) {
        /*
         * DLSw uses common code for low end and RSP fast switching.
         * The RSP interrupt routines provide a pak with the FCS stripped
         * off, whereas, the low end includes it, so compensate here.
         */
        pak->datagramsize -= TR_FCS_SIZE;
        pak->riflen = riflen;
        if (reg_invoke_dlsw_srb_input(idb, pak)) {
            return(TRUE);
        } else {
            pak->datagramsize += TR_FCS_SIZE;
            srb_rx_backout_counters(idb, dgsize);
          return(FALSE);
        } 
    } 

    /*
     * Now find the table entry for the destination cisco on the other
     * side of the virtual ring.
     */
    vre = vrfind_rn2entry(rn, bn, idb->tr_vring_blk, RSRB_ET_EITHER);

    /*
     * If there is not a forward target, return FALSE now, because this
     * frame needs to be handled by the processor to attempt to reopen
     * previous dead remote peers (proxy explorer forces this...)
     */
    if (!vre) {
        srb_rx_backout_counters(idb, dgsize);
	return(FALSE);
    }

    if (vre->type == RSRB_ET_LOCAL) {
	oidb = vre->outhole.idb;
	if (!oidb->srb_routecache) {
            srb_rx_backout_counters(idb, dgsize);
	    return(FALSE);
        }
	if (oidb->state != IDBS_UP) {
            datagram_done(pak);
	    return(TRUE);
        }
	if (!fs_srb_access_check(idb, oidb, dgstart, dgsize, riflen)) {
            datagram_done(pak);
	    return(TRUE);
        }

	if ((*oidb->if_srb_les_fastout)(idb, oidb, pak, pak->datagramsize)) {
            srb_incr_vring_counters(idb, vre, dgsize);
	    if (srb_debug) {
                SrbDebug_VrnFastForward(idb, bn, rn);
		DBG0(DBG_SRB, "\n       from: srb_madge_to_vring");
            }
	}
	return(TRUE);
    }

    if (vre->type == RSRB_ET_LOCAL_VIRTUAL) {
        oidb = vre->outhole.idb;

        if (oidb->span_bridge_process_force)
	    return(FALSE);

	if (oidb->state != IDBS_UP) {
            datagram_done(pak);
	    return(TRUE);
        }
	if (idb->srb_access_in)
	    if (!srb_in_check(dgstart, dgsize, idb, riflen)) {
                datagram_done(pak);
		return(TRUE);
	    }

        pak->datagramsize -= TR_FCS_SIZE;
        pak->if_input      = idb->firstsw;
	pak->if_output     = oidb->firstsw;
	pak->linktype      = LINK_RSRB;
	pak->rxtype        = rx->type;
	pak->riflen        = riflen;

      
	DBG0(DBG_SRB, "\n       from: srb_madge_to_vring");

        if ((*oidb->vbridge_forward)(pak, oidb)) {
            srb_incr_vring_counters(idb, vre, dgsize);
	}
	return(TRUE);
    }

    if (vre->type & (RSRB_ET_REMOTE | RSRB_ET_REMSTATIC)) {
	if (idb->srb_access_in) {
	    if (!srb_in_check(dgstart, dgsize, idb, riflen)) {
	        datagram_done(pak);
	        return(TRUE);
	    }
        }
	vrp = vre->outhole.vp;
	if(!vrp) {
            if(srberr_debug)
                buginf("\nRSRB%d:srb_madge_to_vring: Unhooked vrp to vre ,  srn %d bn %d\n",
                       idb->unit, rn, bn);
	    datagram_done(pak);        
	    return(TRUE);
        }
	if ((vrp->type == RSRB_PT_IF) || (vrp->type == RSRB_PT_LAN) ||
	    (vrp->type == RSRB_PT_FST)) {

	    if (!(rsrb_out_check(dgstart, dgsize, idb, riflen, vrp))) {
		datagram_done(pak);
	        return(TRUE);
            }

	    if (vrp->vrp_les_fast_lanout) {
                /*
                 * low end's version of CSCdi09196.
                 *
                 * problems!  returning false means that it didn't fast
                 * forward it because it
                 * 1)  couldn't get a buffer
                 * 2)  serial encaps not hdlc
                 * 3)  ip route cache not populated if fst
                 * 4)  other reasons
                 * we can't assume #1 and drop the damn thing.
                 * so we cheat.  if 1, then we forwarded,
                 * if 0, dropped for some other reason.
                 * -1 if couldn't get a buffer.
                 * hopefully Dit will clean this up later.  :-)
                 */
                switch ((*vrp->vrp_les_fast_lanout)(idb, vrp, vre, rn, pak, 
                     pak->datagramsize)) {
                  case TRUE:
                    vrp->pkts_tx++;
                    vre->forwards++;
                    GET_TIMESTAMP(idb->lastinput);
                    if (srb_debug) {
                        RsrbDebug_ForwardToPeer(idb, vrp);
                    }
                    return(TRUE);
                  case RSRB_DROP:
                     datagram_done(pak);
                     return(TRUE);
                  case FALSE:
                  default:
                     srb_rx_backout_counters(idb, dgsize);
                     return(FALSE);
                }
            }
	}
    }
    srb_rx_backout_counters(idb, dgsize);
    return(FALSE);
}


/*
 * virtual bridge to virtual ring transport function.
 */

boolean
srb_vbridge_to_vring (hwidbtype *idb,
		    paktype *pak,
		    hwidbtype *dummy)
{
    fm_receive_t vbridge_rx_buffer;
    fm_receive_t *rx = &vbridge_rx_buffer;

    pak->datagramsize += TR_FCS_SIZE;

    rx->pak        = pak;
    rx->rif_length = pak->riflen;
    rx->rd_offset  = (pak->rif_match - pak->datagramstart);

    if (srb_madge_to_vring(idb, rx, dummy))
	return(TRUE);

    pak->datagramsize -= TR_FCS_SIZE;
    return(FALSE);
}


/*
 * nim_vring_output
 *
 * Fast switch an RSRB encapsulated packet out onto an MCI style
 * interface.  This routine is called via a vector from one of the
 * fs_xxx_to_vring() routeins.
 * blast out to virtual ring which is serial/ethernet interface
 *
 * was ported from mci_vring_output()
 */

static int nim_vring_output (hwidbtype *input,
			     tr_vpeertype *vrp,
			     tr_ventrytype *vre,
			     ushort rn,
			     paktype *pak,
			     int dgsize)
{
    hwidbtype *oidb, *vidb;
    idbtype   *oswidb;
    riftype *rifptr = NULL;
    int totalsize = 0;
    ushort *data = NULL;

    ENTER_RSRB("nim_vring_output");

    /*
     * Set up some common variables
     */
    oidb = vrp->outidb;
    oswidb = firstsw_or_null(oidb);
    if (srb_multiring_any_enabled(oswidb)) {
	rifptr = get_rif_inline(oidb, vrp->macaddr);
	if (!rifptr)
	    return(FALSE);
    }

    /*
     * point to destination address in case queuing is invoked
     */
    pak->mac_start = pak->datagramstart + 2;

    /*
     * Set up write pointer to base of destination transmit window.
     * Do some input accounting.  Figure out how to write new MAC header.
     * Then adjust the read pointer of the receiver window to point after
     * old MAC header.  Compute number of bytes to be copied (in temp) and
     * to be sent (in bytes).
     */

    if (oidb->status & IDB_ETHER) {
        int llcsize;
        /*
         * total size is the length of the entire packet.  this
         * length is not the length of the llc.  the size of the llc
         * would be the length of the mac packet minus the size of the
         * destination & source address and the length field.
         * this happens to be 14.  for now fudge this and use
         * ARPA_ENCAPSIZE which happens to be 14.
         */
	totalsize = dgsize + ETHER_SNAP_ENCAPBYTES + TR_VHDRTYPE_SIZE;
        llcsize = totalsize - ETHER_ARPA_ENCAPBYTES;
 
	(uchar *)data = pak->datagramstart - 
               (ETHER_SNAP_ENCAPBYTES + TR_VHDRTYPE_SIZE);
	pak->datagramstart = (uchar *)data;
	SRB_WRITE_ETHER_HEADER(data, vrp->macaddr,
				   oidb->hardware, llcsize);
    } else if (oidb->status & IDB_SERIAL) {
	if (oidb->enctype != ET_HDLC)
	    return(FALSE);
	totalsize = dgsize + HDLC_HEADER_SIZE + TR_VHDRTYPE_SIZE;
        (uchar *)data = pak->datagramstart - (HDLC_HEADER_SIZE + 
               TR_VHDRTYPE_SIZE);
        pak->datagramstart = (uchar *)data;
	SRB_WRITE_SERIAL_HEADER(data);
    } else if (oidb->status & IDB_FDDI) {
	totalsize = dgsize + FDDI_SNAP_ENCAPBYTES + TR_VHDRTYPE_SIZE;
        (uchar *)data = pak->datagramstart - (FDDI_SNAP_ENCAPBYTES + 
               TR_VHDRTYPE_SIZE);
        pak->datagramstart = (uchar *)data;
        data = srb_write_fddi_header(data, vrp->macaddr, oidb->hardware);
    } else if (oidb->status & IDB_TR) {    /* tcp onto tr */
        totalsize = dgsize + TRING_SNAP_ENCAPBYTES + TR_VHDRTYPE_SIZE;
        (uchar *)data = pak->datagramstart - (TRING_SNAP_ENCAPBYTES + 
               TR_VHDRTYPE_SIZE);
        if (rifptr) {
            totalsize += rifptr->length;
            data = (ushort *)((int)data - rifptr->length);
        }
        pak->datagramstart = (uchar *)data;

        SRB_WRITE_TOKEN_HEADER(data, vrp->macaddr, oidb->hardware, rifptr);
    } else
	return(FALSE);
    
    /*
     * Write the RSRB header.
     */
    RSRB_HDR(data, vrp, rn, dgsize);

    pak->datagramsize = totalsize;
    pak->linktype = LINK_RSRB;
    pak->if_output = oswidb;
    pak->acct_proto = ACCT_PROTO_SRB;

    vidb = pak->if_input->hwptr->vidb_link;

    (*oidb->fastsend)(oidb, pak);

    return(TRUE);
}

/*
 * tms380_vring_output
 *
 * Fast switch an RSRB encapsulated packet out onto a CSC-R16 family
 * interface.  This routine is called via a vector from one of the
 * fs_xxx_to_vring() routeins.
 * blast out to virtual ring which is token ring interface
 *
 * was ported from sbe_vring_output()
 */

static int tms380_vring_output (hwidbtype *idb,
				tr_vpeertype *vrp,
				tr_ventrytype *vre,
				ushort rn,
				paktype *pak,
				int dgsize)
{
    hwidbtype *oidb;
    idbtype   *oswidb;
    riftype *rifptr = NULL;
    int hdrsize;
    ushort *data;

    ENTER_RSRB("tms380_vring_output");

    /*
     * Set up some common variables
     */
    oidb = vrp->outidb;
    oswidb = firstsw_or_null(oidb);

    hdrsize = TRING_SNAP_ENCAPBYTES + TR_VHDRTYPE_SIZE;
    
    if (srb_multiring_any_enabled(oswidb)) {
	rifptr = get_rif_inline(oidb, vrp->macaddr);
	if (!rifptr)
	    return(FALSE);
	hdrsize += rifptr->length;
    }

    pak->datagramsize = hdrsize + dgsize;

    /*
     * get a pak buffer (should reference idb, not oidb)
     */
    pak->mac_start = pak->datagramstart+ 2;
    pak->datagramstart -= hdrsize;
    data = (ushort *)pak->datagramstart; 

    SRB_WRITE_TOKEN2_HEADER(data, dgsize, vrp, (oidb->hardware), rn, rifptr);
    pak->if_output = oidb->firstsw;
    pak->linktype = LINK_RSRB;
    (*oidb->fastsend)(oidb, pak);                  /* send the packet  */

    return(TRUE);
}


/************************************************************************
 *
 *                    FAST SWITCH EXPLORERS
 *
 * Functions for fast switching explorer packets to various media.
 * The process level explorer handling function stores the structure
 * pointers of the output interfaces that the explorer packets are
 * forwarded to.  In this way the interrupt routine learns where
 * explorers are headed and attempts to forward them at interrupt.
 * The packet is then sent to the schedualed process where other time
 * consuming operations can be done out of the critical path.
 *
 ************************************************************************/

/*
 * fs_explorer2les - fast switch this explorer out a tms380 interface.
 */
static boolean fs_explorer2les (paktype *pak,
				fs_hdrtype *hcb,
				hwidbtype *oidb)
{
    paktype *opak;
    int hdrsize, dgsize;

    hdrsize = TRING_ENCAPBYTES + hcb->newriflen;
    dgsize  = hcb->pakdatasize + hdrsize;

    opak = input_getbuffer(dgsize, oidb);
    if (!opak) {
	hcb->iidb->fs_expl_input->out_drops++;
        return(FALSE);
    }

    /*
     * NOTE: don't output the data in hcb because FDDI will
     * twist the hcb->newtrh bits.
     *
     * (tbd: create platform specific fs_get_pak()
     *  instead of using input_getbuffer() routine -
     *   on les just call input_getbuffer.
     *   on RSP would get pak with memd buffer.)
     */

    if(oidb->status & IDB_TR) {
        hcb->newtrh->ac = AC_PRI4;
        hcb->newtrh->fc = FC_TYPE_LLC;
    }
    else {
       hcb->newtrh->ac = 0;
       hcb->newtrh->fc = FDDI_LLC_FC_BYTE;
    }

    bcopy((char *)hcb->newtrh, opak->datagramstart, hdrsize);
    bcopy((char *)hcb->pakdataptr, opak->datagramstart+hdrsize,
                  hcb->pakdatasize);

    opak->datagramsize = dgsize;
    opak->acct_proto = ACCT_PROTO_SRB;
    opak->linktype = LINK_SRB;
    opak->if_output = oidb->firstsw;
    opak->mac_start = opak->datagramstart + hdrsize + 2;

    (*oidb->fastsend)(oidb, opak);
    return(TRUE);
 
}

static void fs_les_expl_vectors (hwidbtype *idb)
{
    idb->fs_expl_func = fs_explorer2les;
}



/**********************************************************************
 * 
 *		       FAST DIRECT RSRB INPUT
 *
 * These routines accept a direct RSRB packet from an interface, and
 * send it out onto a locally attached token ring.
 * 
 **********************************************************************/

/*
 * nim_vring_input
 *
 * Receive an RSRB packet from a serial, ethernet or fddi nim, and forward the
 * extracted frame out onto a token ring.  The RSRB frame must be a
 * 'FORWARD' to be handled by this routine.  Anything not understood is
 * thrown up to the system level code.  The 'fastout' will point to a
 * routine that will check access lists (where configured), output the
 * frame, and keep statistics.  Interfaces with Netbios 'Byte' access
 * lists cannot be handled fast.
 *
 * was ported from mci_vring_input()
 */

static boolean nim_vring_input (hwidbtype *idb,
				paktype *pak)
{
    tr_vgrouptype *vrg;
    tr_ventrytype *vre;
    ushort *data = NULL;
    ushort length;
    hwidbtype *outidb;
    tr_vhdrtype *vrh;
    llc1_hdr_t *llc1;
    int riflen;
    trrif_t *trh;
    tr_vpeertype *vrp;
    
    ENTER_RSRB("nim_vring_input");
    
    /*
     * Ignore the version number.  This must be a FORWARD command.  This
     * is the same for all versions.
     */
    if (idb->status & IDB_SERIAL) {
	if (idb->enctype != ET_HDLC)
	    return(FALSE);
	vrh = (tr_vhdrtype *)&(pak->datagramstart[HDLC_ENCAPBYTES]);
    } else if (idb->status & IDB_ETHER )
	vrh = (tr_vhdrtype *)&(pak->datagramstart[ETHER_SNAP_ENCAPBYTES]);
    else if (idb->status & IDB_FDDI )
	vrh = (tr_vhdrtype *)&(pak->datagramstart[FDDI_SNAP_ENCAPBYTES]);
    else
	return(FALSE);

    if ((vrh->op != RSRB_OP_FORWARD) && (vrh->op != RSRB_OP_FORWARDFST))
	return(FALSE);

    /*
     * Is this frame traced?
     * pick up ac/fc from tr header 
     */    
    trh = (trrif_t *)(vrh + 1);
    if (trh->rc_blth & (RC_BROADBIT | RC_TRACE_BOTH))
      return(FALSE);

    /*
     * If the target ring is zero, or the target ring is the same as the
     * virtual ring, then the packet is for this router.  Otherwise, find
     * the ring group.
     */

    if (!vrh->trn || (vrh->trn == vrh->vrn))
	return(FALSE);

    vrg = vrfind_ringgroup(vrh->vrn);
    
    if (!vrg) {
	srbV.srb_sb_incr_rx_drops(idb->firstsw);
	idb->counters.input_drops++;
	datagram_done(pak);
	return(TRUE);
    }
 
    vre = vrfind_rn2entry(vrh->trn,
			  RSRB_BN_DONTCARE, 
			  (tr_ventrytype *)&vrg->rings.qhead,
			  RSRB_ET_LOCAL);
    vrp = idb->tr_ifpeer;
    if ((!vre) || (!vrp)) {
	srbV.srb_sb_incr_rx_drops(idb->firstsw);
	datagram_done(pak);
	return(TRUE);
    }

    outidb = vre->outhole.idb;

    /*
     * Don't fastswitch the frame if the user doesn't want it done.
     */
    if (outidb->state != IDBS_UP)
	return(FALSE);
    if (!outidb->srb_routecache) {
	return(FALSE);
    }
    
    /*
     * Read length and param entries.
     */

    length = vrh->len;  /* keep FCS */

    /*
     * RSRB header stuff checked out ok.  now make data
     * point to the frame w/o the RSRB stuff.
     */

    if (idb->status & IDB_SERIAL) {
	/*
	 * if it's not hdlc encaps, return.  fst only supported
	 * on hdlc encaps.
	 */
	if (idb->enctype != ET_HDLC)
	    return(FALSE);
	data = (ushort *)(&pak->datagramstart[HDLC_ENCAPBYTES
					      + TR_VHDRTYPE_SIZE]);
    } else if (idb->status & IDB_ETHER)
	data = (ushort *)(&pak->datagramstart[ETHER_SNAP_ENCAPBYTES
					      + TR_VHDRTYPE_SIZE]);
    else if (idb->status & IDB_FDDI)
	data = (ushort *)(&pak->datagramstart[FDDI_SNAP_ENCAPBYTES
					      + TR_VHDRTYPE_SIZE]);
    else
	return(FALSE);

    if (vre->type == RSRB_ET_LOCAL_VIRTUAL) {

        if (outidb->span_bridge_process_force)
	    return(FALSE);

	if (outidb->state != IDBS_UP) {
            datagram_done(pak);
	    return(TRUE);
        }

	bcopy(data, trh_acl, TRH_ACL_BYTES);
	trh = (trrif_t *)trh_acl;
	riflen = trh->rc_blth & RC_LTHMASK;

	if (idb->srb_access_in)
	    if (!srb_in_check((uchar *)trh, length, idb, riflen)) {
                datagram_done(pak);
		return(TRUE);
	    }

        pak->datagramstart = (uchar *)data;
        pak->datagramsize  = length - TR_FCS_SIZE;
        pak->if_input      = idb->firstsw;
	pak->if_output     = outidb->firstsw;
	pak->linktype      = LINK_RSRB;
	pak->riflen        = riflen;

        if ((*outidb->vbridge_forward)(pak, outidb)) {
            vrp->pkts_rx++;
            vre->forwards++;
	}
        srb_rx_counters(idb, length);
        return(TRUE);
    }

    /*
     * Time for access lists.  What fun.  Don't forget to go back to the
     * start of the embedded frame once the access list garbage is done.
     */
    if (outidb->srb_access_out || global_proxy_explorer_enable ||
	global_netbios_name_cache_enable) {

	if (fs_netbios_output_acl_check(outidb)) {
	    return(FALSE);
        }
	bcopy(data, trh_acl, TRH_ACL_BYTES);
	trh = (trrif_t *)trh_acl;

	/*
	 * Do proxy explorers, if needed
	 */
	if (global_proxy_explorer_enable) {
	    riflen = trh->rc_blth & RC_LTHMASK;
	    llc1 = (llc1_hdr_t *)((uchar *)&trh->rc_blth + riflen);
	    if (((llc1->control & ~LLC1_P) == LLC1_XID) ||
		((llc1->control & ~LLC1_P) == LLC1_TEST)) {
		rif_update(NULL, vrg->vrn, trh->saddr, NULL,
			   (srbroute_t *) &trh->rc_blth, RIF_AGED_REM);
	    }
	}

	/*
	 * Do NetBIOS name caching, if needed
	 */
	if (global_netbios_name_cache_enable) {
	    netbios_name_cache_update_pak((uchar *) trh, NETBIOS_NAME_AGED_REM,
					  NULL, vrg->vrn);
	}

	if ((outidb->srb_access_out) &&
	    (!srb_out_check(trh_acl, length, outidb,
			   trh->rc_blth & RC_LTHMASK))) {
	    datagram_done(pak);
	    return(TRUE);
	}
    }
    pak->datagramstart = (uchar *)data;
    pak->datagramsize  = length;

    srb_rx_counters(idb, length);
    vrp->pkts_rx++;

    if ((*vre->vre_les_fastout)(idb, outidb, pak, length)) {
	vre->forwards++;
	if (srb_debug) {
	    buginf("\nRSRB%d: fast forward (vrn %d trn %d)",
		    outidb->unit, vrh->vrn, vrh->trn);
	}
    }
    return(TRUE);
}

/*
 * tms380_vring_input
 *
 * Receive an RSRB packet from a TMS380 interface, and forward the
 * extracted frame out onto a token ring.  The RSRB frame must be a
 * 'FORWARD' to be handled by this routine.  Anything not understood is
 * thrown up to the system level code.  The 'fastout' will point to a
 * routine that will check access lists (where configured), output the
 * frame, and keep statistics.  Interfaces with Netbios 'Byte' access
 * lists cannot be handled fast.
 *
 * was ported from sbe_vring_input()
 */
static boolean tms380_vring_input (hwidbtype *idb,
				   fm_receive_t *rx )
{
    paktype *pak = rx->pak;
    uchar *start = (uchar *)rx->data;
    tr_vgrouptype *vrg;
    tr_ventrytype *vre;
    tr_vhdrtype *vrh;
    hwidbtype *outidb;
    int riflen;
    llc1_hdr_t *llc1;
    trrif_t *trh;
    tr_vpeertype *vrp;

    ENTER_RSRB("tms380_vring_input");

    /*
     * Ignore the version number.  This must be a FORWARD command.  This
     * is the same for all versions.
     */
    vrh = (tr_vhdrtype *)start;
    if (vrh->op != RSRB_OP_FORWARD && vrh->op != RSRB_OP_FORWARDFST)
	return(FALSE);

    /*
     * If the target ring is zero, the packet is for this router.
     * Otherwise, find the ring group.
     */
    if (!vrh->trn)
	return(FALSE);
    vrg = vrfind_ringgroup(vrh->vrn);
    if (!vrg) {
	idb->counters.input_drops++;
        datagram_done(pak);
	return(TRUE);
    }
    vrp = idb->tr_ifpeer;  
    vre = vrfind_rn2entry(vrh->trn, 
			  RSRB_BN_DONTCARE, 
			  (tr_ventrytype *)&vrg->rings.qhead,
			  RSRB_ET_LOCAL);
    if ((!vre) || (!vrp)) {
        datagram_done(pak);
	return(TRUE);
    }
    outidb = vre->outhole.idb;

    /*
     * Don't fastswitch the frame if the user doesn't want it done.
     */
    if (!outidb->srb_routecache)
	return(FALSE);

    /*
     * Is this frame traced?  
     */
    trh = (trrif_t *)(start + TR_VHDRTYPE_SIZE);
    riflen = trh->rc_blth & RC_LTHMASK;
    if (trh->rc_blth & (RC_BROADBIT | RC_TRACE_BOTH))
	return(FALSE);

    if (vre->type == RSRB_ET_LOCAL_VIRTUAL) {

        if (outidb->span_bridge_process_force)
	    return(FALSE);

	if (outidb->state != IDBS_UP) {
            datagram_done(pak);
	    return(TRUE);
        }

	if (idb->srb_access_in)
	    if (!srb_in_check((char *)trh, vrh->len, idb, riflen)) {
                datagram_done(pak);
		return(TRUE);
	    }

        pak->datagramstart = (uchar *)trh;
        pak->datagramsize  = vrh->len - TR_FCS_SIZE;
        pak->if_input      = idb->firstsw;
	pak->if_output     = outidb->firstsw;
	pak->linktype      = LINK_RSRB;
	pak->riflen        = riflen;

        if ((*outidb->vbridge_forward)(pak, outidb)) {
            vrp->pkts_rx++;
            vre->forwards++;
	}
        else
           datagram_done(pak);

        srb_rx_counters(idb, vrh->len);
        return(TRUE);
    }

    /*
     * Time for access lists.  What fun.  Don't forget to go back to the
     * start of the embedded frame once the access list garbage is done.
     */
    if (outidb->srb_access_out) {
	if (fs_netbios_output_acl_check(outidb)) {
	    return(FALSE);
        }
	if (!srb_out_check((uchar *)trh, vrh->len, outidb, riflen)) {
            datagram_done(pak);
	    return(TRUE);
	}
    }

    /*
     * Do any proxy explorer processing
     */
    if (global_proxy_explorer_enable) {
	llc1 = (llc1_hdr_t *)((uchar *)&trh->rc_blth + riflen);
	if (((llc1->control & ~LLC1_P) == LLC1_XID) ||
	    ((llc1->control & ~LLC1_P) == LLC1_TEST)) {
	    rif_update(NULL, vrg->vrn, trh->saddr, NULL,
		       (srbroute_t *) &trh->rc_blth, RIF_AGED_REM);
	}
    }
    
    /*
     * Do NetBIOS name caching, if needed
     */
    if (global_netbios_name_cache_enable) {
	netbios_name_cache_update_pak((uchar *) trh, NETBIOS_NAME_AGED_REM,
				      NULL, vrg->vrn);
    }
    /* keep FCS, already included in length */
    srb_rx_counters(idb, vrh->len);
    vrp->pkts_rx++;

    pak->datagramstart = (uchar *)(vrh + 1);
    pak->datagramsize  = vrh->len;

    if ((*vre->vre_les_fastout)(idb, outidb, pak, vrh->len)) {
	vre->forwards++;
	if (srb_debug) {
	    buginf("\nRSRB%d: fast forward (vrn %d trn %d)",
		    idb->unit, vrh->vrn, vrh->trn);
	}
    } else
       datagram_done(pak);
    return(TRUE);
}

/*
 *
 */
void fs_srb_setup (void)
{
    hwidbtype *idb, *oidb;
    tr_ventrytype *vring;
    tr_vgrouptype *vrg;
    list_element *l_elt;

    FOR_ALL_HWIDBS_IN_LIST(HWIDBLIST_TMS380, l_elt, idb) {
	idb->tr_srb_fastswitch = (tr_srb_fastswitch_type) return_false;
	idb->if_srb_les_fastout = (if_srb_les_fastout_type) return_false;
	/* Set the other two variant's for sanity sake */
	idb->if_srb_hes_fastout = (if_srb_hes_fastout_type) return_false;
	idb->if_srb_fastout = (if_srb_fastout_type) return_false;

	/*
	 * Setting up the output side if_srb_les_fastout is simpler.  Set it
	 * up first.
	 */
	if (idb->srb_routecache && !fs_netbios_output_acl_check(idb)) { 
	    idb->if_srb_les_fastout = fs_srb_madge_output;
        }

	/*
	 * Don't touch cards with input side netbios offset lists or
	 * acess expressions.  This is because they need to look at
	 * random offsets into the frame and the frame is not local to
	 * this processor.
	 */
	if (idb->srb_access_in && fs_netbios_input_acl_check(idb)) {
	    continue;
        }

	/*
	 * Target is a virtual ring.
	 */
	vring = idb->tr_vring_blk;
	if (vring) {
	    if (idb->srb_routecache) {
	        idb->tr_srb_fastswitch 
		    = (tr_srb_fastswitch_type) srb_madge_to_vring;
	    }
	    continue;	
	}

	/*
	 * Target must be a real ring.  Check for fast switching enabled
	 * on the output interface.  Cannot function if netbios offset
	 * lists or acess expressions are enabled on the output
	 * interface, so check for those as well.
	 */
	oidb = idb->tr_bridge_idb;
	if (oidb) {
	    if ((!oidb->srb_routecache) ||
		(oidb->srb_access_out && fs_netbios_output_acl_check(oidb))) {
	            continue;
            }
	    idb->tr_srb_fastswitch = (tr_srb_fastswitch_type) srb_madge2xxx;
	}
    }

    /* Initialize virtual bridge fast switch vector. */

    for (vrg = (tr_vgrouptype *)vringQ.qhead; vrg; vrg = vrg->next) {
	if (vrg->virtual_bridge)
	    if (vrg->virtual_bridge->tbridge_media == TBR_VBRIDGE)
	        vrg->virtual_bridge->hwptr->tr_srb_fastswitch = 
			        (tr_srb_fastswitch_type)srb_vbridge_to_vring;
    }
}

void set_vring_fast_entry (hwidbtype *idb)
{
    tr_ventrytype *vre;

    vre = idb->tr_vring_blk;
    for (; vre; vre = vre->next) {
	if (((vre->type == RSRB_ET_LOCAL) ||
	     (vre->type == RSRB_ET_LOCAL_VIRTUAL)) &&
	    (vre->outhole.idb == idb)) {
	    break;
	}
    }

    if (!vre)
	return;
    /* For sanity sake, set the other variant. */
    vre->vre_hes_fastout = (vre_hes_fastout_type)vre_fastout_err;

    if (idb->type == IDBTYPE_TMS380) {
	vre->vre_les_fastout = fs_srb_madge_output;
	return;
    }
    
    vre->vre_les_fastout = (vre_les_fastout_type)vre_fastout_err;
}

void set_vring_fast_peer (tr_vpeertype *vrp,
			  hwidbtype *idb)
{
    switch (vrp->type) {
    case RSRB_PT_IF:
	vrp->vrp_les_fast_lanout = nim_vring_output;
	break;

    case RSRB_PT_LAN:
	if (idb->type == IDBTYPE_TMS380)
	    vrp->vrp_les_fast_lanout = tms380_vring_output;	
	else
	    vrp->vrp_les_fast_lanout = nim_vring_output;	
	break;

    case RSRB_PT_FST:
	vrp->vrp_les_fast_lanout = fs_to_fst_vring;
	FOR_ALL_HWIDBS(idb) {
	    if (idb->type == IDBTYPE_TMS380)
		idb->rsrb_fst_fastout = 
                      (rsrb_fst_fastout_type)fs_to_madge_fst_vring;
	    else
		idb->rsrb_fst_fastout = 
                       (rsrb_fst_fastout_type)fs_to_nim_fst_vring;
	}
	break;

    default:
	vrp->vrp_les_fast_lanout = NULL;
	break;
    }

    /* Clear the other two variants for sanity sake. */
    vrp->vrp_fast_lanout = NULL;
    vrp->vrp_hes_fast_lanout = NULL;
}



static void les_fsrb_init (subsystype* subsys) 
{
    reg_add_nim_vring_input(nim_vring_input, "nim_vring_input");
    reg_add_tms380_vring_input(
        (service_tms380_vring_input_type)tms380_vring_input, 
        "tms380_vring_input");
    reg_add_les_expl_vectors(fs_les_expl_vectors, "fs_les_expl_vectors");
}


/*
 * Fast SRB subsystem header
 */

#define FSRB_LES_MAJVERSION 1
#define FSRB_LES_MINVERSION 0
#define FSRB_LES_EDITVERSION 1

SUBSYS_HEADER(les_fsrb, FSRB_LES_MAJVERSION, FSRB_LES_MINVERSION,
	      FSRB_LES_EDITVERSION,
	      les_fsrb_init, SUBSYS_CLASS_PROTOCOL,
	      "seq: srb",
	      "req: fastswitch, srb");


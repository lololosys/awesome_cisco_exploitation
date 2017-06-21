/* $Id: at_fast_mci.c,v 3.9.8.7 1996/08/26 20:34:56 kleung Exp $
 * $Source: /release/112/cvs/Xsys/atalk/at_fast_mci.c,v $
 *------------------------------------------------------------------
 * MCI and cBus AppleTalk fast switching support
 *
 * August 1990, David Edwards
 *
 * Copyright (c) 1990-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: at_fast_mci.c,v $
 * Revision 3.9.8.7  1996/08/26  20:34:56  kleung
 * CSCdi66720:  AppleTalk route-cache disabled for subinterfaces.
 * Branch: California_branch
 *
 * Revision 3.9.8.6  1996/08/02  18:20:16  kleung
 * CSCdi64905:  spurious access in 11.2(0.19) on boot
 * Branch: California_branch
 *
 * Revision 3.9.8.5  1996/05/17  10:39:41  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.8.2.4  1996/05/05  22:46:45  ppearce
 * Sync to IbuMod_Calif_baseline_960504
 *
 * Revision 3.8.2.3  1996/05/02  22:00:55  hampton
 * Remove support for old Multibus token ring cards.
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.8.2.2  1996/04/03  13:43:20  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.9.8.4  1996/05/02  08:39:43  dwong
 * Branch: California_branch
 * Commit SMRP-7kFastSwitch, Load Balancing, and AURP Modularity
 *
 * Revision 3.9.8.3  1996/04/27  05:44:49  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 3.7.8.3  1996/04/25  23:55:49  syiu
 * Branch: Dial1_branch
 * Dial1_branch to IosSec_postmerge_960419
 *
 * Revision 3.7.8.2  1996/04/17  00:30:02  syiu
 * Branch: Dial1_branch
 * Sync Dial1_branch to V111_1_3
 *
 * Revision 3.7.8.1  1996/02/23  20:11:26  lbustini
 * Branch: Dial1_branch
 * Move dialer fields out of the idb.
 *
 * Revision 3.9.8.2  1996/03/23  01:26:24  slin
 * Branch: California_branch
 *
 * AppleTalk makes its exodus from the idb.
 *
 * Revision 3.9.8.1  1996/03/18  18:52:08  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.7.12.3  1996/03/16  06:26:40  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.7.12.2  1996/03/13  01:06:34  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.7.12.1  1996/02/20  00:17:10  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.9  1996/02/27  19:26:40  achopra
 * CSCdi45189:  "show atm traffic" shows different (wrong) packet counts
 *
 * Revision 3.8  1996/02/13  08:10:49  dstine
 * CSCdi48797:  Extraneous definitions in interface_private.h
 *
 * Revision 3.7  1995/12/18  18:35:27  kleung
 * CSCdi45792:  Performance degraded due to fix CSCdi45378.
 *
 * Revision 3.6  1995/12/14  22:10:24  kleung
 * CSCdi45378:  Incorrect determination of DDP Type when fast switching.
 *
 * Revision 3.5  1995/11/23  18:05:49  bbenson
 * CSCdi43579:  Appletalk fastswitching to LANE on 7000 screws up packet
 * length
 * Add atalk_write_lane_header which will write a length word for snap
 * encaps over LANE
 *
 * Revision 3.4  1995/11/20  22:55:44  wmay
 * CSCdi43158:  AppleTalk fast-switching fails over hssi with smds
 * encapsulation - deal with odd byte starts.
 *
 * Revision 3.3  1995/11/20  21:38:27  kleung
 * CSCdi43508:  AppleTalk fast switching cleanup for at_fast_mci.c.
 *
 * Revision 3.2  1995/11/17  08:42:28  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:56:45  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.9  1995/10/24  21:37:44  kleung
 * CSCdi40486:  Appletalk switching fails from aip to serial and eip to
 * serial
 *
 * Actually, AIP -> Serial (SMDS) was broken due to outdated ATM input
 * handler.
 *
 * Revision 2.8  1995/09/19  23:20:53  kleung
 * CSCdi40683:  High end f/s code need to handle WFQ correctly.
 *
 * Revision 2.7  1995/07/10  05:31:52  fred
 * CSCdi36850:  commit fast path queuing for appletalk, decnet, etc.
 *
 * Revision 2.6  1995/07/05  22:24:09  kleung
 * CSCdi35438:  AURP update event did not send through tunnel properly
 *
 * Revision 2.5  1995/06/28  09:21:39  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.4  1995/06/22  22:46:29  wmay
 * CSCdi36246:  Appletalk doesnt fast-switch smds
 *
 * Revision 2.3  1995/06/21  02:50:43  gstovall
 * CSCdi35731:  Cleanup unused cruft in idb
 * Nuke some unused vectors and variables from the hwidb.
 *
 * Revision 2.2  1995/06/13  22:35:04  awu
 * CSCdi32913:  lex to atm appletalk phase 1 fast switching broken.
 *
 * Revision 2.1  1995/06/07  20:08:09  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifdef GLOBAL_DEBUG
#ifdef AT_FAST_DEBUG
#define FAST_DEBUG		/* Provides msgs as things fast switch */
#define FAST1_DEBUG		/* Provides detailed analysis of fields */
#endif
#endif

#include "master.h"
#include "interface_private.h"
#include "registry.h"
#include "fastswitch_registry.h"
#include "subsys.h"
#include "packet.h"
#include "../if/ether.h"
#include "../if/tring.h"
#include "config.h"
#include "../wan/serial.h"
#include "../wan/ppp.h"
#include "../if/fddi.h"
#include "../hes/if_mci.h"
#include "../hes/if_fci.h"
#include "../wan/frame_relay.h"
#include "../wan/atm_dxi.h"
#include "../wan/smds.h"
#include "../atalk/atalk_private.h"
#include "../atalk/at_fast.h"
#include "../lex/lex_fs_mci.h"
#include "../atalk/at_domain.h"
#include "../if/atm.h"
#include "../lane/lane.h"
#include "../atalk/at_registry.h"
#include "../atalk/at_fast_mci.h"
#include "../wan/dialer.h"


/*
 * local storage
 */
boolean (*atfs_samemci[AT_FS_MAX])(hwidbtype *, atcachetype *, ushort, ushort);
boolean (*atfs_mci2mci[AT_FS_MAX])(hwidbtype *, atcachetype *, ushort, ushort);

/*
 * A stub routine to boost a packet up to process level.
 */
static boolean
atalk_boost_to_process (hwidbtype *unused1, atcachetype *unused2,
			ushort unused3, ushort unused4) 
{return (FALSE);}

/*
 * A note about using this macro:
 * the idb->rxrifwordlen field is setup for incoming packets only.
 * If you try to use idb->rxrifwordlen for computing the output
 * RIF size, you're going to possibly push a very confused packet
 * out on the wire.
 */
#define	AT_TRING_OFFSET	CTR_RECEIVE_OFFSET + (TRING_SNAP_ENCAPBYTES >> 1) \
                        + input->rxrifwordlen



#ifdef	AT_FAST_DEBUG
#define PRINT_INFO(_name_) \
    if ((long)(input->atalk_compat_flags) < 0) {\
        buginf("\nATp2" _name_": %s: sniff = %08x %08x %08x", 		\
	   input->hw_namestring,					\
	   input->sniff[SNIFFBASE+0].d.lword,				\
	   input->sniff[SNIFFBASE+1].d.lword,				\
	   input->sniff[SNIFFBASE+2].d.lword);				\
        buginf("\nATp2" _name_": hops = %d, hopword = %04x", 		\
	   hopword.d.byte[0]>>2,					\
	   hopword.d.sword);						\
        buginf("\nATp2" _name_ ": net = %d, node = %d",temp,		\
	   input->sniff[SNIFFBASE+2].d.byte[0]);                        \
    }

#define PRINT_ERROR_RETURN(_name_) \
    if ((long)(input->atalk_compat_flags) < 0) {                        \
    buginf("\nATp2" _name_": Did not find a cache entry for %d.%d",	\
	   input->sniff[SNIFFBASE+1].d.sword[0],			\
	   input->sniff[SNIFFBASE+2].d.byte[0]);                        \
    }

#else
#define	PRINT_INFO(_name_)	;
#define	PRINT_ERROR_RETURN(_name_) ;
#endif


/**********************************************************************
 *
 *		FAST SWITCHING ENCAPSULATION WRITE FUNCTIONS
 *
 **********************************************************************/


/*
 * atalk_write_remapped_header
 *
 * Write the remapped network addresses.
 */
static inline void
write_remapped_header (mciregtype *regptr, ushort dst, ushort src)
{
    regptr->write1short = 0;	/* zero out ddp checksum */
    regptr->write1short = dst;
    regptr->write1short = src;
}

/*
 * at_write_etalk_header
 *
 * Write the 12 bytes of MAC-address,  the ethertype, 3 bytes of LLAP,
 * and the hopword onto a Phase-I ethernet frame.
 */
static inline void 
atalk_write_etalk_header (mciregtype* regptr, atcachetype* cptr, ushort len)
{
    regptr->write1long = cptr->m.maclong[0];	/* dd, dd, dd, dd */
    regptr->write1long = cptr->m.maclong[1];	/* dd, dd, ss, ss */
    regptr->write1long = cptr->m.maclong[2];	/* ss, ss, ss, ss */
    regptr->write1short = len;
    regptr->write1long = cptr->m.maclong[3];	/* tt, tt, dn, sn */
}


/*
 * at_write_token_header
 *
 * Write a full Phase-II Token-Ring header, consisting of
 * - frame control bytes (AC & FC)
 * - 12 bytes of MAC-address information (DA & SA)
 * - up to 18 bytes of RIF information.
 * - SNAP-SNAP ISO2 header (8 bytes)
 * - the hop-count word in the DDP frame. (2 bytes)
 */
static inline void
atalk_write_token_header (mciregtype* regptr, atcachetype* cptr)
{
    ushort	i;

    regptr->write1short = (AC_PRI0 << 8) | FC_TYPE_LLC;	/* ac fc */
    regptr->write1long = cptr->m.maclong[0];	/* dd, dd, dd, dd */
    regptr->write1long = cptr->m.maclong[1];	/* dd, dd, ss, ss */
    regptr->write1long = cptr->m.maclong[2];	/* ss, ss, ss, ss */
    
    if (cptr->w_riflen > 0)
	for (i = 6; i < 6 + cptr->w_riflen; ++i)
	    regptr->write1short = cptr->m.macshort[i];

    regptr->write1long = CONST_AA_AA_03_08;	/* aa, aa, 03, 08 */
    regptr->write1long = CONST_00_07_80_9B; 	/* 00, 07, 80, 9b */
}

/*
 * atalk_write_serial_header
 *
 * Write a general serial header for HDLC.
 * This will write a header on either a nominal serial controller or
 * the new MIP serial controller, both aligned and unaligned.
 *
 * Routine assumes that cptr->byte_len is even!
 */
inline void
atalk_write_serial_header (mciregtype* regptr, atcachetype* cptr,
			   ushort hw, ushort unaligned)
{
    ushort	num_long, words_to_write;
    tinybool    write_a_short;
    uchar	*macptr;

    if (unaligned) {
	/* If the input is unaligned then we'll have to write out the
	 * first and last byte separately.  Note that the last byte
	 * will get written out with the hopbyte.  Also, adjust the
	 * number of words to write down by 1. 
	 */
	regptr->write1short = cptr->m.macstring[0]; /* Write out 1st byte */
	macptr = &cptr->s.macstring[0];
	words_to_write = cptr->word_len - 1;
    } else {
	macptr = &cptr->m.macstring[0];
	words_to_write = cptr->word_len;
    }
        
    num_long = words_to_write >> 1;
    write_a_short = words_to_write & 0x1;
    
    switch (num_long) {
      case 12:	regptr->write1long = *(ulong *)macptr; macptr += sizeof(ulong);
      case 11:	regptr->write1long = *(ulong *)macptr; macptr += sizeof(ulong);
      case 10:	regptr->write1long = *(ulong *)macptr; macptr += sizeof(ulong);
      case 9:	regptr->write1long = *(ulong *)macptr; macptr += sizeof(ulong);
      case 8:	regptr->write1long = *(ulong *)macptr; macptr += sizeof(ulong);
      case 7:	regptr->write1long = *(ulong *)macptr; macptr += sizeof(ulong);
      case 6:	regptr->write1long = *(ulong *)macptr; macptr += sizeof(ulong);
      case 5:	regptr->write1long = *(ulong *)macptr; macptr += sizeof(ulong);
      case 4:	regptr->write1long = *(ulong *)macptr; macptr += sizeof(ulong);
      case 3:	regptr->write1long = *(ulong *)macptr; macptr += sizeof(ulong);
      case 2:	regptr->write1long = *(ulong *)macptr; macptr += sizeof(ulong);
      case 1:	regptr->write1long = *(ulong *)macptr; macptr += sizeof(ulong);

      case 0:	;			/* null statement */
      default:
    }

    if (write_a_short) { /* should we write a short? */
	regptr->write1short = *(ushort *)macptr; macptr += sizeof(ushort);
    }

    /*
     * If the input was unaligned, this means that we're going
     * to have to fudge the writing of the hop-word. This
     * happens when the input packet came from ARPA Ethernet,
     * running the EtherTalk Phase-I encapsulation.
     */
    if (unaligned) {
	/*
	 * Now we're left with the last byte to be written and the hop-byte.
	 * We pair these two together to get a short, which crosses the
	 * "line" at the start of the DDP frame.
	 */
	regptr->write1short = (*macptr << 8) | ((hw & 0xff00) >> 8);
    } else {
	regptr->write1short = hw;
    }
}


/**********************************************************************
 *
 *		FAST SWITCHING BETWEEN CBUS/MCI CARDS
 *
 **********************************************************************/

/*
 * ATALK2_M2M_START
 *
 * Fast switch AppleTalk packets between two different MCI controllers
 * or between an MCI controller and a cBus interface/controller.
 * Called from atalk_mci2mci_xx().
 */
#define ATALK2_M2M_START                                             	\
    output = cptr->hwif;				                \
    srcreg = (mciregtype*) input->mci_regptr;				\
    dstreg = (mciregtype*) output->mci_regptr;				\
    bytes = input->ddp_length;                                          \
    if (output->output_qcount && output->priority_list) {               \
        /* if sorting traffic and interface is congested, process switch */\
	return(FALSE);                                                  \
    }

#define ATALK2_M2M_START2						\
    /* 									\
     * Reserve a buffer in window 1 of the destination MCI 		\
     */									\
    dstreg->argreg = output->mci_index;					\
    dstreg->cmdreg = MCI_CMD_SELECT;					\
    \
    dstreg->argreg = output->buffer_pool;				\
    dstreg->cmdreg = MCI_CMD_TX1_RESERVE;				\
    if (dstreg->cmdreg != MCI_RSP_OKAY) {				\
        if (output->priority_list) {                                    \
            /* if sorting traffic and interface is congested, process switch */\
	    return(FALSE);                                              \
        } else {                                                        \
	    output->atalk_outputerrs++;					\
	    output->counters.output_total_drops++;		       	\
	    srcreg->cmdreg = MCI_CMD_RX_FLUSH;				\
	    return (TRUE);					       	\
        }							       	\
    }									\
    output->counters.outputs_fast[ACCT_PROTO_APPLE]++;			\
    atalk_stat[ATALK_XFASTFORWARD]++;					\
    xfercnt = bytes;

#define ATALK2_M2M_START3						\
    dstreg->argreg = 0;							\
    dstreg->cmdreg = MCI_CMD_TX1_SELECT;


/*
 * ATALK2_M2M_FINISH
 *
 * Finish up this packet and send it.  This routine first copies out the
 * parts of the AppleTalk DDP packet that have already been sucked into local
 * memory, and then it copies the bulk of datagram from one MCI to
 * another.  Last, it transmits modified copy and flush the original from
 * the receive buffer.
 */
#define ATALK2_M2M_FINISH						\
    dstreg->argreg = bytes;						\
    mci2mci(&srcreg->readlong, &dstreg->write1long, xfercnt);		\
    dstreg->cmdreg = MCI_CMD_TX1_START;					\
    srcreg->cmdreg = MCI_CMD_RX_FLUSH;					\
    output->counters.tx_cumbytes_fast[ACCT_PROTO_APPLE]			\
        += bytes & ~MCI_TX_ODDALIGN;




/**********************************************************************
 *
 *		FAST SWITCHING WITHIN AN CBUS/MCI CARD
 *
 **********************************************************************/

/*
 * ATALK_SAME_START
 *
 * Move the buffer to the output interface.  If it can't move the buffer,
 * the packet is dropped if queueing is not enabled; otherwise packet is
 * returned to process level.
 *
 * Called from atalk_samemci_xx().
 */
#define ATALK_SAME_START(input, regptr, output)                         \
    /*                                                                  \
     * RX buffer from receive window to transmit window 1. This may     \
     * fail if interface output queue limit has been exceeded.          \
     */                                                                 \
    if (input->output_qcount && input->priority_list) {                 \
	/*                                                              \
	 * if sorting traffic and interface is congested, process       \
         * switch                                                       \
	 */                                                             \
	return(FALSE);                                                  \
    }                                                                   \
    regptr->argreg = (ushort)output->mci_index;                         \
    regptr->cmdreg = MCI_CMD_RX_MOVE;                                   \
    if (regptr->cmdreg != MCI_RSP_OKAY) {                               \
	/*                                                              \ 
	 * if sorting traffic and interface is congested, process       \
	 * switch                                                       \
	 */                                                             \
	if (output->priority_list) {                                    \
	    return(FALSE);                                              \
	} else {                                                        \
	    output->counters.output_total_drops++;                      \
	    output->atalk_outputerrs++;                                 \
	    regptr->cmdreg = MCI_CMD_RX_FLUSH;                          \
	    return(TRUE);                                               \
	}                                                               \
    }

/*
 * atalk_same_finish
 *
 * Finish up this packet and send it.  This routine first copies out the
 * parts of the AppleTalk packet that have already been sucked into local
 * memory, and then kicks the cbus/mci to start it transmitting the
 * frame.
 */
inline void 
atalk_same_finish (hwidbtype *input,  mciregtype *dstreg,
		   hwidbtype *output, ushort size_to_xmit)
{
    dstreg->argreg = size_to_xmit;
    dstreg->cmdreg = MCI_CMD_TX1_START;
    output->counters.outputs_fast[ACCT_PROTO_APPLE]++;
    output->counters.tx_cumbytes_fast[ACCT_PROTO_APPLE]
	+= size_to_xmit & ~MCI_TX_ODDALIGN;
    atalk_stat[ATALK_FASTFORWARD]++;
}

/*
 * atalk_hes_fastswitch()
 *
 * This will be the common bottleneck for the NEW AppleTalk high-end
 * fast-switching code.
 *
 * The new method will use the encapsulation-specific classification
 * down in the driver(s) to remove some of the complexity up in the AppleTalk
 * code.
 *
 * This function is the entry point for all extended (ie, PhII)
 * incoming packets. Non-extended packets will be switched into a different
 * function.
 */
/*
static inline boolean 
atalk_hes_fastswitch (hwidbtype *input, mciregtype *mci_regptr)
*/
static boolean 
atalk_hes_fastswitch (hwidbtype *input, mciregtype *mci_regptr)
{
    atcachetype	*cptr, *primary;
    hwidbtype	*output;
    at_Domain   *domain;
    ushort       remap_src = 0;
    ushort       remap_dst = 0;
    boolean      incrementhop = TRUE;
    ushort       ddptype;    

    PRINT_INFO("atalk_hes_fastswitching");

    /*
     * ddptype will be either the source socket number or DDP type
     * for unaligned or aligned packets, respectively.  Since
     * socket number and DDP type correspond, value can be used
     * for DDP type checking.  This saves extra execution to
     * differentiate between aligned and unaligned packets for DDP
     * type checking purpose.
     */
    ddptype = mci_regptr->readshort;

    if (atalk_NBPFilters) {
	if (ddptype >> 8 == DDP_TYPE_NBP)
	    return (FALSE);
    }

    /*
     * If SMRP packet, let SMRP fast switching routine handle it.
     * Note that only long DDP format is processed.
     */
    if (atalk_SMRP_running && (ddptype >> 8 == DDP_TYPE_SMRP)) {

	input->ddp_length = input->hop_len_word & DDPLENGTHMASK;

	/* Do some pkt accounting */
	atalk_stat[ATALK_INPUT]++;
	    
	if (reg_invoke_atalk_smrp_mci_fastswitch(input, mci_regptr,
						 input->trans_hdr_size,
						 input->input_unaligned)) {
	    input->counters.inputs_fast[ACCT_PROTO_APPLE]++;
	    input->counters.rx_cumbytes_fast[ACCT_PROTO_APPLE] += input->rxlength;
	    return(TRUE);
	}
	return(FALSE);
    }

    if (atalk_DomainRouter_running) {
        at_Domain *domain_in;
        atalkidbtype *atalkidb;

	if (ddptype >> 8 == DDP_TYPE_NBP)
	    return (FALSE);

	atalkidb = atalk_getidb(input->fast_rcvidb);
	if (!atalkidb)
	  return (FALSE);
	
	ATFAST_REMAPNETS(atalkidb, input->dst_net, input->dst_node,
			 input->src_net);
	
	domain_in = atalkidb->atalk_dminfo;
	ATFAST_ADJUSTHOPCOUNT(domain_in, domain, input->hopbyte);

    } else {

	cptr = at_lookup_cache_ptr(input->dst_net, input->dst_node);
	if (cptr == NULL)	{
	    PRINT_ERROR_RETURN(_name_);
	    return (FALSE);
	}
	
	if (cptr->delaycount) {
	     /* 
	      * First parallel path # of packets will be
	      * handled at the process switch level.
	      */
             return (FALSE);
        }
        else {
             /*
              * Switch to the next parallel cache entry
	      */
	     primary = cptr;
	     cptr = primary->next_equalcache;
	     primary->next_equalcache = cptr->equalcache;
	}
    }

    if (incrementhop)
	if ((input->hopbyte += 0x04) > (DDP_MAX_HOPCOUNT << 2)) {
	    if (at_debug(atalkpkt_debug, ATALK_NULL_MASK))
		return (FALSE);	/* Allow processor to report failure */
	    atalk_stat[ATALK_INPUT]++;
	    atalk_stat[ATALK_HOPCOUNT]++;
	    input->atalk_inputerrs++;
	    mci_regptr->cmdreg = MCI_CMD_RX_FLUSH;
	    return (TRUE);
	}


    output = cptr->hwif;
    input->ddp_length = input->hop_len_word & DDPLENGTHMASK;
    atalk_stat[ATALK_INPUT]++;
    input->counters.inputs_fast[ACCT_PROTO_APPLE]++;
    input->counters.rx_cumbytes_fast[ACCT_PROTO_APPLE] += input->rxlength;
    if (mci_regptr == output->mci_regptr) {
	/*
	 * Output interface is on the same MCI.
	 */

 	/* We can't remap if ddp header starts on odd boundary. */
 	if (atalk_DomainRouter_running && input->input_unaligned)
 	    return (FALSE);

	return((*atfs_samemci[cptr->encaptype])(input, cptr, remap_dst,
						remap_src));
    } else if (output->mci_regptr) {
	/*
	 * Output is another MCI, this means a card-to-card copy.
	 * Before we call the specific routine, reset the
	 * source interface window to point just after the
	 * DDP hop/length word; we will manually write the MAC header
	 * and hop/length word and then do an mci2mci() to copy
	 * everything after the hop/length word.
	 */
  	if (atalk_DomainRouter_running) {
 	    if (input->input_unaligned)
 		return (FALSE);
 	    else
 		mci_regptr->argreg = input->ddp_start + 4;
  	} else
  	    mci_regptr->argreg = input->ddp_start + 1;

	mci_regptr->cmdreg = MCI_CMD_RX_SELECT;
	return((*atfs_mci2mci[cptr->encaptype])(input, cptr, remap_dst,
						remap_src));
    }
    return (FALSE);
}

/*
 * Here start the fast-switch functions called from the driver's
 * interrupt levels.
 */

/*******************************************************************
 *                      Ethernet ARPA V2 (Phase 1) switching       *
 *******************************************************************/
/*
 * atalk_ether_fastswitch
 *
 * Very high speed switching of an ARPA encapsulated Ethernet AppleTalk
 * (Phase I) datagram. This function also much account for the 3 LLAP bytes
 * whih immediately follow the Ethernet length field.
 * 
 * This routine has been heavily optimized for speed, since it
 * is in the inner loop of the code that handles back-to-back datagrams.
 */
static boolean 
atalk_ether_fastswitch (hwidbtype *input)
{
    mciregtype	*srcreg;
    charlong	sniff1;
    charlong	sniff2;

    /*
     * Setup DDP data read pointer to check the hop count.
     */
    srcreg = input->mci_regptr;
    srcreg->argreg = AT_ETALK_OFFSET;
    srcreg->cmdreg = MCI_CMD_RX_SELECT;

    /*
     * Here's what the DDP frame looks like from the AT_ETALK_OFFSET
     * on...
     *
     * +-------+-------+-------+-------+-------+-------+-------+-------+
     * | LLAP  |       |       |       |       |       |       |       |
     * | Pkt   | hop ct. & len |   DDP cksum   |   dest net.   |  src net
     * | Type  |       |       |       |       |       |       |       |
     * +-------+-------+-------+-------+-------+-------+-------+-------+
     * {        first long sniff      }{     second long sniff        }
     *
     * The LLAP packet type must be $02, because anything else
     * implies a non-DDP long packet. We don't do that here; non DDP long
     * packets get bucked down to process switching.
     *
     * We're going to fill in only the following information:
     * - hop count & DDP length
     * - destination network
     * - source network
     * - destination node
     * - source node
     */
    sniff1.d.lword = srcreg->readlong;
    if (sniff1.d.byte[0] != ALAP_DDP_LONG)
	return (FALSE);			/* we f/s only DDP longs */

    input->hop_len_word = (sniff1.d.byte[1] << 8) | sniff1.d.byte[2];
    sniff2.d.lword  = srcreg->readlong;
    input->dst_net  = (sniff2.d.byte[1] << 8) | sniff2.d.byte[2];
    sniff1.d.lword  = srcreg->readlong;
    input->src_net  = (sniff2.d.byte[3] << 8) | sniff1.d.byte[0];
    input->dst_node = sniff1.d.byte[1];
    input->src_node = sniff1.d.byte[2];
    input->dst_sock = sniff1.d.byte[3];
    input->ddp_start       = AT_ETALK_OFFSET;
    input->input_unaligned = TRUE;
    input->trans_hdr_size   = AT_ETALK_OFFSET + (DDP_OVERHEADBYTES >> 1) + 1;
    return (atalk_hes_fastswitch(input, srcreg));
}

/*
 * atalk_write_ether_header
 *
 * Write the 12 bytes of MAC-address,  the ethertype, 3 bytes of LLAP,
 * and the hopword onto a Phase-I ethernet frame. Align as necessary.
 *
 * The important thing to observe here is that when we are switching from
 * a Phase-II (extended) encapsulation, the start of the DDP header
 * will be "on the line" and evenly aligned. The size of the non-extended
 * ethernet encapsulation is 17 bytes. This means that MCI_TX_ODDALIGN
 * must be set on the output length.
 *
 */
inline void 
atalk_write_ether_header (mciregtype* regptr, atcachetype* cptr,
			  ushort hopw, ushort unaligned)
{
    /*
     * The "unaligned" sense here is reversed because the
     * ARPA Ethernet encapsulation is odd in length.
     *
     * If the input started out unaligned, then adding an odd
     * length encap to an odd start will get us an even start.
     */
    if (unaligned) {
	regptr->write1long = cptr->m.maclong[0]; /* dd, dd, dd, dd */
	regptr->write1long = cptr->m.maclong[1]; /* dd, dd, ss, ss */
	regptr->write1long = cptr->m.maclong[2]; /* ss, ss, ss, ss */
	regptr->write1long = cptr->m.maclong[3]; /* tt, tt, dn, sn */
	regptr->write1short = (cptr->m.macstring[16] << 8) | ((hopw & 0xff00) >> 8);
    } else {
	regptr->write1short = cptr->m.macstring[0]; /* dd */
	regptr->write1long = cptr->s.maclong[0]; /* dd, dd, dd, dd */
	regptr->write1long = cptr->s.maclong[1]; /* dd, ss, ss, ss */
	regptr->write1long = cptr->s.maclong[2]; /* ss, ss, ss, tt */
	regptr->write1long = cptr->s.maclong[3]; /* tt, dn, sn, pt */
	regptr->write1short = hopw;		 /* hc, ln */
    }
}

/*
 * atalk_samemci_ether_arpa
 *
 * Write a ethernet ARPA header onto the front of the supplied packet,
 * and then hand the packet off to the transmit code.  This routine
 * works within a cbus/mci card.
 */
static boolean 
atalk_samemci_ether_arpa (hwidbtype *input, atcachetype *cptr,
			  ushort remap_dst, ushort remap_src)
{
    mciregtype	*regptr = input->mci_regptr;
    hwidbtype	*output = cptr->hwif;
    ushort size_to_xmit = input->ddp_length + cptr->byte_len;

    ATALK_SAME_START(input, regptr, output);
    
    /* Move pointer so we can write the new header.  If the input
     * packet came in aligned (i.e. input_aligned == FALSE) then we'll
     * have to back up a little bit more to account for the odd number
     * of bytes we're about to write.
     */
    regptr->argreg = input->ddp_start - ETALK_OVERHEAD_SIZE -
	(input->input_unaligned == FALSE ? UNALIGNED_OVERHEAD : 0);

    regptr->cmdreg = MCI_CMD_TX1_SELECT;
    atalk_write_ether_header(regptr, cptr,
			     input->hop_len_word, input->input_unaligned);

    if (atalk_DomainRouter_running)
	write_remapped_header(regptr, remap_dst, remap_src);
    
    if (size_to_xmit < MINETHERSIZE)
	size_to_xmit = MINETHERSIZE;
    /*
     * Since the size of an ethertalk header is odd, if the
     * DDP frame started on an even boundary, then the output
     * will start on an odd byte.
     */
    if (input->input_unaligned == FALSE)
	size_to_xmit |= MCI_TX_ODDALIGN;
    atalk_same_finish(input, output->mci_regptr,
		      output, size_to_xmit);
    return (TRUE);
}

/*
 * atalk_mci2mci_ether_arpa
 *
 * Write a ethernet ARPA header and LLAP bytes onto the front of the
 * supplied packet, and then hand the packet off to the transmit code.
 * This routine works between cbus/mci cards.
 */
static boolean
atalk_mci2mci_ether_arpa (hwidbtype *input, atcachetype *cptr,
			  ushort remap_dst, ushort remap_src)
{
    hwidbtype*	output;
    mciregtype* srcreg;
    mciregtype* dstreg;
    ushort bytes, xfercnt;
    output = cptr->hwif;
    srcreg = input->mci_regptr;
    dstreg = output->mci_regptr;
    bytes = input->ddp_length;

    /*
     * Reserve a buffer in window 1 of the destination MCI
     */
    dstreg->argreg = output->mci_index;
    dstreg->cmdreg = MCI_CMD_SELECT;

    dstreg->argreg = output->buffer_pool;
    dstreg->cmdreg = MCI_CMD_TX1_RESERVE;
    if (dstreg->cmdreg != MCI_RSP_OKAY) { /* TX buffer not reserved */
	if (output->priority_list) {
	    /* if sorting traffic and interface is congested, process switch */
	    return(FALSE);
	} else {
	    output->counters.output_total_drops++;
	    output->atalk_outputerrs++;
	    srcreg->cmdreg = MCI_CMD_RX_FLUSH;
	    return (TRUE);
	}
    }
    output->counters.outputs_fast[ACCT_PROTO_APPLE]++;
    output->counters.tx_cumbytes_fast[ACCT_PROTO_APPLE] += input->rxlength;
    atalk_stat[ATALK_XFASTFORWARD]++;
    dstreg->argreg = 0;
    dstreg->cmdreg = MCI_CMD_TX1_SELECT;
    xfercnt = bytes;

    atalk_write_ether_header(dstreg, cptr,
			     input->hop_len_word, input->input_unaligned);
    bytes += ETALK_ENCAPBYTES;
    if (bytes < MINETHERSIZE)
	bytes = MINETHERSIZE;
    if (input->input_unaligned == FALSE)
	bytes |= MCI_TX_ODDALIGN;

    if (atalk_DomainRouter_running) {
	xfercnt -= sizeof(ushort) * 4;
	write_remapped_header(dstreg, remap_dst, remap_src);
    } else
        xfercnt -= (input->input_unaligned ? sizeof(uchar) : sizeof(ushort));
    /* 
     * copy bulk of datagram from one MCI to another. 
     */
    dstreg->argreg = bytes;
    mci2mci(&srcreg->readlong, &dstreg->write1long, xfercnt);
    dstreg->cmdreg = MCI_CMD_TX1_START;
    srcreg->cmdreg = MCI_CMD_RX_FLUSH;
    return (TRUE);
}


/**********************************************************************
 *                   802.3 Ethernet (SNAP Encapsulation)              *
 **********************************************************************/

/*
 * atalk_ether_snap_fastswitch
 *
 * Very high speed switching of SNAP (Phase II) encapsulated Ethernet 
 * AppleTalk datagram. 
 * This routine has bee heavily optimized for speed, since it
 * is in the inner loop of the code that handles back-to-back datagrams.
 */
static boolean 
atalk_ether_snap_fastswitch (hwidbtype *input)
{
    mciregtype *srcreg;
 
    /*
     * Setup DDP data read pointer to check the hop count.
     */
    srcreg = input->mci_regptr;
    srcreg->argreg = AT_ISO2_OFFSET;
    srcreg->cmdreg = MCI_CMD_RX_SELECT;
    input->hop_and_cksum   = srcreg->readlong;
    input->src_dst_net     = srcreg->readlong;
    input->nodes_and_socks = srcreg->readlong;
    input->ddp_start       = AT_ISO2_OFFSET;
    input->input_unaligned = FALSE;
    input->trans_hdr_size  = AT_ISO2_OFFSET + (DDP_OVERHEADBYTES >> 1);
    return (atalk_hes_fastswitch(input, srcreg));
}

/*
 * atalk_write_ether_snap_header
 *
 * Write an Phase-II style SNAP-SNAP header on a frame.
 *
 * We assume that the length passed in 
 */
inline void
atalk_write_ether_snap_header (mciregtype* regptr, atcachetype* cptr,
			       ushort len, ushort hopw, ushort unaligned)
{
    longbyte lenlong;
    shortbyte lenw;
    lenw.d.sword = len - ETHER_ARPA_ENCAPBYTES;	/* we want ddp + 8 bytes */
    if (unaligned) {
	regptr->write1short = cptr->m.macstring[0];	/* dd */
	regptr->write1long = cptr->s.maclong[0];	/* dd, dd, dd, dd */
	regptr->write1long = cptr->s.maclong[1];	/* dd, ss, ss, ss */
	regptr->write1short = cptr->s.macshort[4];	/* ss, ss */
	lenlong.d.byte[0] = cptr->s.macstring[10];	/* ss */
	lenlong.d.byte[1] = lenw.d.byte[0];		/* ll */
	lenlong.d.byte[2] = lenw.d.byte[1];		/* ll */
	lenlong.d.byte[3] = CONST_AA;			/* aa */
	regptr->write1long = lenlong.d.lword;
	regptr->write1long = CONST_AA_03_08_00;		/* aa, 03, oi, oi */
	regptr->write1long = CONST_07_80_9B_00
	                     | ((hopw & 0xff00) >> 8);	/* oi, tt, tt, hb */
    } else {
	regptr->write1long = cptr->m.maclong[0];	/* dd, dd, dd, dd */
	regptr->write1long = cptr->m.maclong[1];	/* dd, dd, ss, ss */
	regptr->write1long = cptr->m.maclong[2];	/* ss, ss, ss, ss */
	regptr->write1short = lenw.d.sword;		/* ll, ll */
	regptr->write1long = CONST_AA_AA_03_08;		/* aa, aa, 03, 08 */
	regptr->write1long = CONST_00_07_80_9B; 	/* 00, 07, 80, 9b */
	regptr->write1short = hopw;
    }
}

/*
 * atalk_mci2mci_ether_snap
 *
 * Write a SNAP ethernet header onto the front of the supplied packet,
 * and then hand the packet off to the transmit code.  This routine
 * works between cbus/mci cards.
 */
static boolean 
atalk_mci2mci_ether_snap (hwidbtype *input, atcachetype *cptr,
			  ushort remap_dst, ushort remap_src)
{
    hwidbtype* output = cptr->hwif;
    mciregtype* srcreg = input->mci_regptr; 
    mciregtype* dstreg = output->mci_regptr;
    ushort bytes = input->ddp_length;
    ushort xfercnt;

    ATALK2_M2M_START;
    ATALK2_M2M_START2;
    ATALK2_M2M_START3;
    /*
     * The size of the frame must be adjusted with the output encap
     * size before the header is written in this case so that
     * the 802.3 header length calculation works correctly in
     * atalk_write_ether_snap_header.
     */
    bytes += ETHER_SNAP_ENCAPBYTES;
    atalk_write_ether_snap_header(dstreg, cptr, bytes,
				  input->hop_len_word, input->input_unaligned);
    if (bytes < MINETHERSIZE)
	bytes = MINETHERSIZE;

    if (atalk_DomainRouter_running) {
	xfercnt -= sizeof(ushort) * 4;
	write_remapped_header(dstreg, remap_dst, remap_src);
    } else
        xfercnt -= (input->input_unaligned ? sizeof(uchar) : sizeof(ushort));
    if (input->input_unaligned)
	bytes |= MCI_TX_ODDALIGN;
    ATALK2_M2M_FINISH;
    return (TRUE);
}

/*
 * atalk_samemci_ether_snap
 *
 * Write a SNAP ethernet header onto the front of the supplied packet,
 * and then hand the packet off to the transmit code.  This routine
 * works within a cbus/mci card.
 */
static boolean 
atalk_samemci_ether_snap (hwidbtype *input, atcachetype *cptr,
			  ushort remap_dst, ushort remap_src)
{
    mciregtype *regptr = input->mci_regptr;
    hwidbtype *output  = cptr->hwif;
    ushort size_to_xmit = input->ddp_length + ETHER_SNAP_ENCAPBYTES;

    ATALK_SAME_START(input, regptr, output);
    
    regptr->argreg = input->ddp_start - ISO2_OVERHEAD_SIZE;
    regptr->cmdreg = MCI_CMD_TX1_SELECT;
    atalk_write_ether_snap_header(output->mci_regptr, cptr, size_to_xmit,
				  input->hop_len_word, input->input_unaligned);

    if (atalk_DomainRouter_running)
	write_remapped_header(regptr, remap_dst, remap_src);
    
    if (size_to_xmit < MINETHERSIZE)
	size_to_xmit = MINETHERSIZE;
    if (input->input_unaligned)
	size_to_xmit |= MCI_TX_ODDALIGN;
    atalk_same_finish(input, regptr, output, size_to_xmit);
    return (TRUE);
}


/**********************************************************************
 *                      Token Ring (SNAP Encapsulation)               *
 **********************************************************************/

/*
 * atalk_token_snap_fastswitch
 *
 * Very high speed switching of SNAP encapsulated Broken Ring AppleTalk
 * datagram. This routine has been optimized for speed, since it is in
 * the inner loop of the code that handles back-to-back datagrams.
 */
static boolean 
atalk_token_snap_fastswitch (hwidbtype *input)
{
    mciregtype *srcreg;

    /*
     * Setup DDP data read- pointer to check the hop count.
     */
    srcreg = input->mci_regptr;
    srcreg->argreg = AT_TRING_OFFSET;
    srcreg->cmdreg = MCI_CMD_RX_SELECT;
    input->hop_and_cksum   = srcreg->readlong;
    input->src_dst_net     = srcreg->readlong;
    input->nodes_and_socks = srcreg->readlong;
    input->ddp_start       = AT_TRING_OFFSET;
    input->input_unaligned = FALSE;
    return (atalk_hes_fastswitch(input, srcreg));
}

/*
 * at_write_token_header
 *
 * Write a full Phase-II Token-Ring header, consisting of
 * - frame control bytes (AC & FC)
 * - 12 bytes of MAC-address information (DA & SA)
 * - up to 18 bytes of RIF information.
 * - SNAP-SNAP ISO2 header (8 bytes)
 * - the hop-count word in the DDP frame. (2 bytes)
 */
static inline void
atalk_write_token_snap_header (mciregtype* regptr, atcachetype* cptr,
			       ushort hopw, ushort unaligned)
{
    ushort	i;			/* init'ed for no-rif case */

    if (unaligned) {
	regptr->write1short = AC_PRI0;			/* ac */
	regptr->write1short =
	    (FC_TYPE_LLC << 8)|cptr->m.macstring[0];	/* fc, dd */
	regptr->write1long  = cptr->s.maclong[0];	/* dd, dd, dd, dd */
	regptr->write1long  = cptr->s.maclong[1];	/* dd, ss, ss, ss */
	regptr->write1short = cptr->s.macshort[4];	/* ss, ss */
	
	i = 5;
	if (cptr->w_riflen > 0) {
	    for (; i < 5 + cptr->w_riflen; ++i)
		regptr->write1short = cptr->s.macshort[i];
	}
	regptr->write1short =
	    (cptr->s.macstring[i] << 8) | CONST_AA;	/* dd, aa */
	regptr->write1long = CONST_AA_03_08_00;		/* aa, 03, oi, oi */
	regptr->write1long = CONST_07_80_9B_00
	                     | ((hopw & 0xff00) >> 8);	/* oi, tt, tt, hb */
    } else {
	regptr->write1short = (AC_PRI0 << 8) | FC_TYPE_LLC;	/* ac fc */
	regptr->write1long = cptr->m.maclong[0];	/* dd, dd, dd, dd */
	regptr->write1long = cptr->m.maclong[1];	/* dd, dd, ss, ss */
	regptr->write1long = cptr->m.maclong[2];	/* ss, ss, ss, ss */

	if (cptr->w_riflen > 0)
	    for (i = 6; i < 6 + cptr->w_riflen; ++i)
		regptr->write1short = cptr->m.macshort[i];
	
	regptr->write1long = CONST_AA_AA_03_08;		/* aa, aa, 03, 08 */
	regptr->write1long = CONST_00_07_80_9B; 	/* 00, 07, 80, 9b */
	regptr->write1short = hopw;			/* hp, ln */
    }
}

/*
 * atalk_mci2mci_token_snap
 *
 * Write a Token Ring SNAP header onto the front of the supplied packet,
 * and then hand the packet off to the transmit code.  This routine
 * works between cbus/mci cards.
 */
static boolean 
atalk_mci2mci_token_snap (hwidbtype *input, atcachetype *cptr,
			  ushort remap_dst, ushort remap_src)
{
    hwidbtype* output = cptr->hwif;
    mciregtype* srcreg = input->mci_regptr; 
    mciregtype* dstreg = output->mci_regptr;
    ushort bytes = input->ddp_length;
    ushort xfercnt;

    ATALK2_M2M_START;
    ATALK2_M2M_START2;
    ATALK2_M2M_START3;
    atalk_write_token_snap_header(dstreg, cptr,
				  input->hop_len_word, input->input_unaligned);
    if (input->input_unaligned)
	bytes |= MCI_TX_ODDALIGN;
    bytes += TRING_SNAP_ENCAPBYTES + cptr->b_riflen;
  
    if (atalk_DomainRouter_running) {
	xfercnt -= sizeof(ushort) * 4;
	write_remapped_header(dstreg, remap_dst, remap_src);
    } else
	xfercnt -= (input->input_unaligned ? sizeof(uchar) : sizeof(ushort));
    ATALK2_M2M_FINISH;
    return (TRUE);
}

/*
 * atalk_samemci_token_snap
 *
 * Write a Token Ring SNAP header onto the front of the supplied packet,
 * and then hand the packet off to the transmit code.  This routine works
 * within a cbus/mci card.
 */
static boolean 
atalk_samemci_token_snap (hwidbtype *input, atcachetype *cptr,
			  ushort remap_dst, ushort remap_src)
{
    mciregtype *regptr = input->mci_regptr;
    hwidbtype *output = cptr->hwif;
    ushort size_to_xmit = input->ddp_length
	+ TRING_SNAP_ENCAPBYTES + cptr->b_riflen;

    ATALK_SAME_START(input, regptr, output);
    
    regptr->argreg = input->ddp_start
	             - (TRING_SNAP_ENCAPWORDS + cptr->w_riflen);
    regptr->cmdreg = MCI_CMD_TX1_SELECT;
    atalk_write_token_snap_header(regptr, cptr,
				  input->hop_len_word, input->input_unaligned);

    if (atalk_DomainRouter_running)
	write_remapped_header(regptr, remap_dst, remap_src);
    
    if (input->input_unaligned)
	size_to_xmit |= MCI_TX_ODDALIGN;
    atalk_same_finish(input, regptr, output, size_to_xmit);
    return (TRUE);
}

/*******************************************************************
 *                     FDDI w/ SNAP-SNAP encapsulation
 *******************************************************************/

/*
 * atalk_fddi_fastswitch
 *
 * Very high speed switching of SNAP encapsulated FDDI AppleTalk datagram.
 * This routine has been optimized for speed, since it is in the inner
 * loop of the code that handles back-to-back datagrams.
 */
static boolean 
atalk_fddi_fastswitch (hwidbtype *input)
{
    mciregtype	*srcreg;

    /*
     * Setup DDP data read pointer to check the hop count.
     */
    srcreg = input->mci_regptr;
    srcreg->argreg = AT_FDDI_OFFSET;
    srcreg->cmdreg = MCI_CMD_RX_SELECT;
    input->hop_and_cksum   = srcreg->readlong;
    input->src_dst_net     = srcreg->readlong;
    input->nodes_and_socks = srcreg->readlong;
    input->ddp_start       = AT_FDDI_OFFSET;
    input->input_unaligned = FALSE;
    input->trans_hdr_size  = AT_FDDI_OFFSET + (DDP_OVERHEADBYTES >> 1);
    return (atalk_hes_fastswitch(input, srcreg));
}

/*
 * at_write_fddi_header
 *
 * Write a Phase-II FDDI header; the FDDI frame control byte,
 * 12 bytes of MAC-address information and the SNAP header, plus the
 * DDP hop word.
 */
inline void 
atalk_write_fddi_header (mciregtype* regptr, atcachetype* cptr,
			 ushort hopw, ushort unaligned)
{
    if (unaligned) {
	regptr->write1short = (ushort) (FDDI_LLC_FC_BYTE << 8) | 
	    cptr->m.macstring[0];			/* fc, ss */
	regptr->write1long  = cptr->s.maclong[0];	/* dd, dd, dd, dd */
	regptr->write1long  = cptr->s.maclong[1];	/* dd, ss, ss, ss */
	regptr->write1short = cptr->s.macshort[4];	/* ss, ss */
	regptr->write1short =
	    (cptr->s.macstring[10] << 8) | CONST_AA;	/* dd, aa */
	regptr->write1long = CONST_AA_03_08_00;		/* aa, 03, oi, oi */
	regptr->write1long = CONST_07_80_9B_00
	                     | ((hopw & 0xff00) >> 8);	/* oi, tt, tt, hb */
    } else {
	regptr->write1short = FDDI_LLC_FC_BYTE;
	regptr->write1long = cptr->m.maclong[0];	/* dd, dd, dd, dd */
	regptr->write1long = cptr->m.maclong[1];	/* dd, dd, ss, ss */
	regptr->write1long = cptr->m.maclong[2];	/* ss, ss, ss, ss */
	regptr->write1long = CONST_AA_AA_03_08;		/* aa, aa, 03, 08 */
	regptr->write1long = CONST_00_07_80_9B; 	/* 00, 07, 80, 9b */
	regptr->write1short = hopw;			/* hp, ln */
    }
}

/*
 * atalk_mci2mci_fddi
 *
 * Write a FDDI SNAP header onto the front of the supplied packet,
 * and then hand the packet off to the transmit code.  This routine
 * works between cbus/mci cards.
 */
static boolean 
atalk_mci2mci_fddi (hwidbtype *input, atcachetype *cptr,
		    ushort remap_dst, ushort remap_src)
{
    hwidbtype* output = cptr->hwif;
    mciregtype* srcreg = input->mci_regptr; 
    mciregtype* dstreg = output->mci_regptr;
    ushort bytes = input->ddp_length;
    ushort xfercnt;

    ATALK2_M2M_START;
    ATALK2_M2M_START2;
    ATALK2_M2M_START3;
    atalk_write_fddi_header(dstreg, cptr,
			    input->hop_len_word, input->input_unaligned);
    bytes += FDDI_SNAP_ENCAPBYTES;

    if (atalk_DomainRouter_running) {
	xfercnt -= sizeof(ushort) * 4;
	write_remapped_header(dstreg, remap_dst, remap_src);
    } else
        xfercnt -= (input->input_unaligned ? sizeof(uchar) : sizeof(ushort));
    if (input->input_unaligned == FALSE)
	bytes |= MCI_TX_ODDALIGN;
    ATALK2_M2M_FINISH;
    return (TRUE);
}

/*
 * atalk_samemci_fddi
 *
 * Write a FDDI SNAP header onto the front of the supplied packet,
 * and then hand the packet off to the transmit code.  This routine
 * works within a cbus/mci card.
 */
static boolean 
atalk_samemci_fddi (hwidbtype *input, atcachetype *cptr,
		    ushort remap_dst, ushort remap_src)
{
    mciregtype *regptr = input->mci_regptr;
    hwidbtype *output = cptr->hwif;
    ushort size_to_xmit = input->ddp_length + FDDI_SNAP_ENCAPBYTES;

    ATALK_SAME_START(input, regptr, output);
    
    /* Move pointer so we can write the new header.  If the input
     * packet came in unaligned (i.e. input_aligned == TRUE) then
     * we'll to back up a little bit less.
     */
    regptr->argreg = input->ddp_start - FDDI_OVERHEAD_SIZE +
	(input->input_unaligned == FALSE ? 0 : UNALIGNED_OVERHEAD);

    regptr->cmdreg = MCI_CMD_TX1_SELECT;
    atalk_write_fddi_header(regptr, cptr, 
			    input->hop_len_word, input->input_unaligned);

    if (atalk_DomainRouter_running)
	write_remapped_header(regptr, remap_dst, remap_src);
	
    if (input->input_unaligned == FALSE)
	size_to_xmit |= MCI_TX_ODDALIGN; /* because FDDI encap len is odd */
    atalk_same_finish(input, regptr, output, size_to_xmit);
    return (TRUE);
}

/*******************************************************************
 *                   HDLC, extended (no-LLAP)
 *******************************************************************/

/*
 * atalk_hdlc_fastswitch
 *
 * Very high speed switching of HDLC encapsulated AppleTalk datagram. This
 * routine has been optimized for speed, since it is in the inner loop of
 * the code that handles back-to-back datagrams.
 *
 * This routine doubles as the PPP input routine.
 */
static boolean atalk_hdlc_fastswitch (hwidbtype *input)
{
    mciregtype *srcreg;

    /*
     * Setup DDP data read pointer to check the hop count.
     * Use encsize rather than the equate value so we subtract
     * the correct number of bytes for the MIP.
     */
    srcreg = input->mci_regptr;
    srcreg->argreg = AT_INPUT_SERIAL_OFFSET;
    srcreg->cmdreg = MCI_CMD_RX_SELECT;
    input->hop_and_cksum   = srcreg->readlong;
    input->src_dst_net     = srcreg->readlong;
    input->nodes_and_socks = srcreg->readlong;
    input->ddp_start       = AT_INPUT_SERIAL_OFFSET;
    input->input_unaligned = FALSE;
    input->trans_hdr_size  = AT_INPUT_SERIAL_OFFSET + (DDP_OVERHEADBYTES >> 1);
    return (atalk_hes_fastswitch(input, srcreg));
}

/*
 * atalk_mci2mci_hdlc
 *
 * Write a HDLC header onto the front of the supplied packet, and then
 * hand the packet off to the transmit code.  This routine works between
 * cbus/mci cards.
 */
static boolean 
atalk_mci2mci_hdlc (hwidbtype *input, atcachetype *cptr,
		    ushort remap_dst, ushort remap_src)
{
    hwidbtype* output = cptr->hwif;
    mciregtype* srcreg = input->mci_regptr; 
    mciregtype* dstreg = output->mci_regptr;
    ushort bytes = input->ddp_length;
    ushort xfercnt;

    /* If input is unaligned and output interface is not cBus, then we
     * cannot support odd byte starts.  Send the packet to process
     * level.
     */
    if (input->input_unaligned && !(output->status & IDB_CBUS))
	return (FALSE);
    
    ATALK2_M2M_START;
    ATALK2_M2M_START2;
    ATALK2_M2M_START3;
    atalk_write_serial_header(dstreg, cptr, 
			      input->hop_len_word, input->input_unaligned);
    bytes += HDLC_ENCAPBYTES;

    if (atalk_DomainRouter_running) {
	xfercnt -= sizeof(ushort) * 4;
	write_remapped_header(dstreg, remap_dst, remap_src);
    } else
        xfercnt -= (input->input_unaligned ? sizeof(uchar) : sizeof(ushort));
    if (input->input_unaligned)
	bytes |= MCI_TX_ODDALIGN;
    ATALK2_M2M_FINISH;
    return (TRUE);
}

/*
 * atalk_samemci_hdlc
 *
 * Write a HDLC header onto the front of the supplied packet, and then
 * hand the packet off to the transmit code.  This routine works within a
 * cbus/mci card. 
 */
static boolean 
atalk_samemci_hdlc (hwidbtype *input, atcachetype *cptr,
		    ushort remap_dst, ushort remap_src)
{
    mciregtype *regptr = input->mci_regptr;
    hwidbtype *output = cptr->hwif;
    ushort size_to_xmit = input->ddp_length + cptr->byte_len;

    ATALK_SAME_START(input, regptr, output);
    
    regptr->argreg = input->ddp_start - cptr->word_len;
    regptr->cmdreg = MCI_CMD_TX1_SELECT;
    atalk_write_serial_header(regptr, cptr,
			      input->hop_len_word, input->input_unaligned);

    if (atalk_DomainRouter_running)
	write_remapped_header(regptr, remap_dst, remap_src);
    
    if (input->input_unaligned)
	size_to_xmit |= MCI_TX_ODDALIGN;
    atalk_same_finish(input, regptr, output, size_to_xmit);
    return (TRUE);
}


/*******************************************************************
 *                    Frame Relay, cisco mode, extended
 *******************************************************************/

/*
 * atalk_frame_fastswitch
 *
 * Very high speed switching of Frame Relay encapsulated DDP datagrams.
 */
static boolean 
atalk_frame_fastswitch (hwidbtype *input)
{
    mciregtype *srcreg;

    /*
     * Setup DDP data read pointer to check the hop count.
     */
    srcreg = input->mci_regptr;
    srcreg->argreg = AT_INPUT_SERIAL_OFFSET;
    srcreg->cmdreg = MCI_CMD_RX_SELECT;
    input->hop_and_cksum   = srcreg->readlong;
    input->src_dst_net     = srcreg->readlong;
    input->nodes_and_socks = srcreg->readlong;
    input->ddp_start       = AT_INPUT_SERIAL_OFFSET;
    input->input_unaligned = FALSE;
    input->trans_hdr_size  = AT_INPUT_SERIAL_OFFSET + (DDP_OVERHEADBYTES >> 1);
    return (atalk_hes_fastswitch(input, srcreg));
}

/*
 * atalk_mci2mci_frame
 *
 * Write a Non-IETF Frame Relay header onto the front of the supplied
 * packet, and then hand the packet off to the transmit code.  This
 * routine works between cbus/mci cards.
 */
static boolean 
atalk_mci2mci_frame (hwidbtype *input, atcachetype *cptr,
		     ushort remap_dst, ushort remap_src)
{
    hwidbtype* output = cptr->hwif;
    mciregtype* srcreg = input->mci_regptr; 
    mciregtype* dstreg = output->mci_regptr;
    ushort bytes = input->ddp_length;
    ushort xfercnt;

    /* If input is unaligned and output interface is not cBus, then we
     * cannot support odd byte starts.  Send the packet to process
     * level.
     */
    if (input->input_unaligned && !(output->status & IDB_CBUS))
	return (FALSE);
    
    ATALK2_M2M_START;
    ATALK2_M2M_START2;
    ATALK2_M2M_START3;
    atalk_write_serial_header(dstreg, cptr,
			      input->hop_len_word, input->input_unaligned);
    bytes += FRAME_RELAY_ENCAPBYTES;

    if (atalk_DomainRouter_running) {
	xfercnt -= sizeof(ushort) * 4;
	write_remapped_header(dstreg, remap_dst, remap_src);
    } else
        xfercnt -= (input->input_unaligned ? sizeof(uchar) : sizeof(ushort));
    INC_PVC_FAST_OUTPUT(HWIDB_TO_FR_PVC(output, cptr->subint_reference_num),
			bytes);
    if (input->input_unaligned)
	bytes |= MCI_TX_ODDALIGN;
    ATALK2_M2M_FINISH;
    return (TRUE);
}

/*
 * atalk_samemci_frame
 *
 * Write a Non-IETF header onto the front of the supplied packet, and
 * then hand the packet off to the transmit code.  This routine works
 * within a cbus/mci card.
 */
static boolean 
atalk_samemci_frame (hwidbtype *input, atcachetype *cptr,
		     ushort remap_dst, ushort remap_src)
{
    mciregtype *regptr = input->mci_regptr;
    hwidbtype *output = cptr->hwif;
    ushort size_to_xmit = input->ddp_length + cptr->byte_len;

    ATALK_SAME_START(input, regptr, output);
    
    regptr->argreg = input->ddp_start - cptr->word_len;
    regptr->cmdreg = MCI_CMD_TX1_SELECT;
    atalk_write_serial_header(regptr, cptr, 
			      input->hop_len_word, input->input_unaligned);

    if (atalk_DomainRouter_running)
	write_remapped_header(regptr, remap_dst, remap_src);

    INC_PVC_FAST_OUTPUT(HWIDB_TO_FR_PVC(output, cptr->subint_reference_num),
			size_to_xmit);
    if (input->input_unaligned)
	size_to_xmit |= MCI_TX_ODDALIGN;
    atalk_same_finish(input, regptr, output, size_to_xmit);
    return (TRUE);
}



/**********************************************************************
 *                              Frame Relay, IETF                     *
 **********************************************************************/

/*
 * atalk_frame_ietf_fastswitch
 *
 * Very high speed switching of Frame Relay encapsulated AppleTalk datagram.
 */
static boolean 
atalk_frame_ietf_fastswitch (hwidbtype *input)
{
    mciregtype *srcreg;

    /*
     * Setup ddp data read pointer to check the hop count.
     */
    srcreg = input->mci_regptr;
    srcreg->argreg   = AT_FR_IETF_OFFSET;
    srcreg->cmdreg   = MCI_CMD_RX_SELECT;
    input->hop_and_cksum   = srcreg->readlong;
    input->src_dst_net     = srcreg->readlong;
    input->nodes_and_socks = srcreg->readlong;
    input->ddp_start       = AT_FR_IETF_OFFSET;
    input->input_unaligned = FALSE;
    input->trans_hdr_size  = AT_FR_IETF_OFFSET + (DDP_OVERHEADBYTES >> 1);
    return (atalk_hes_fastswitch(input, srcreg));
}

/*
 * atalk_mci2mci_frame_ietf
 *
 * Write a IETF Frame Relay header onto the front of the supplied
 * packet, and then hand the packet off to the transmit code.  This
 * routine works between cbus/mci cards.
 */
static boolean 
atalk_mci2mci_frame_ietf (hwidbtype *input, atcachetype *cptr,
			  ushort remap_dst, ushort remap_src)
{
    hwidbtype* output = cptr->hwif;
    mciregtype* srcreg = input->mci_regptr; 
    mciregtype* dstreg = output->mci_regptr;
    ushort bytes = input->ddp_length;
    ushort xfercnt;

    /* If input is unaligned and output interface is not cBus, then we
     * cannot support odd byte starts.  Send the packet to process
     * level.
     */
    if (input->input_unaligned && !(output->status & IDB_CBUS))
	return (FALSE);
    
    ATALK2_M2M_START;
    ATALK2_M2M_START2;
    ATALK2_M2M_START3;
    atalk_write_serial_header(dstreg, cptr,
			      input->hop_len_word, input->input_unaligned);
    bytes += cptr->byte_len;

    if (atalk_DomainRouter_running) {
	xfercnt -= sizeof(ushort);
	write_remapped_header(dstreg, remap_dst, remap_src);
    } else
        xfercnt -= (input->input_unaligned ? sizeof(uchar) : sizeof(ushort));
    INC_PVC_FAST_OUTPUT(HWIDB_TO_FR_PVC(output, cptr->subint_reference_num),
			bytes);
    if (input->input_unaligned)
	bytes |= MCI_TX_ODDALIGN;
    ATALK2_M2M_FINISH;
    return (TRUE);
}

/*
 * atalk_samemci_frame_ietf
 *
 * Write a IETF header onto the front of the supplied packet, and
 * then hand the packet off to the transmit code.  This routine works
 * within a cbus/mci card.
 */
static boolean 
atalk_samemci_frame_ietf (hwidbtype *input, atcachetype* cptr,
			  ushort remap_dst, ushort remap_src)
{
    mciregtype *regptr  = input->mci_regptr;
    hwidbtype *output   = cptr->hwif;
    ushort size_to_xmit = input->ddp_length + cptr->byte_len;

    ATALK_SAME_START(input, regptr, output);
    
    regptr->argreg = input->ddp_start - cptr->word_len;
    regptr->cmdreg = MCI_CMD_TX1_SELECT;
    atalk_write_serial_header(regptr, cptr,
			      input->hop_len_word, input->input_unaligned);

    if (atalk_DomainRouter_running)
	write_remapped_header(regptr, remap_dst, remap_src);

    INC_PVC_FAST_OUTPUT(HWIDB_TO_FR_PVC(output, cptr->subint_reference_num),
			size_to_xmit);
    
    if (input->input_unaligned)
	size_to_xmit |= MCI_TX_ODDALIGN;
    atalk_same_finish(input, regptr, output, size_to_xmit);
    return (TRUE);
}

/*******************************************************************
 *                   PPP, extended (no-LLAP)
 *******************************************************************/

/*
 * atalk_ppp_fastswitch
 *
 * Very high speed switching of HDLC encapsulated AppleTalk datagram. This
 * routine has been optimized for speed, since it is in the inner loop of
 * the code that handles back-to-back datagrams.
 *
 * This routine doubles as the PPP input routine.
 */
static boolean atalk_ppp_fastswitch (hwidbtype *input)
{
    mciregtype *srcreg;

    /*
     * Setup DDP data read pointer to check the hop count.
     * Use encsize rather than the equate value so we subtract
     * the correct number of bytes for the MIP.
     */
    srcreg = input->mci_regptr;
    srcreg->argreg = AT_INPUT_SERIAL_OFFSET;
    srcreg->cmdreg = MCI_CMD_RX_SELECT;
    input->hop_and_cksum   = srcreg->readlong;
    input->src_dst_net     = srcreg->readlong;
    input->nodes_and_socks = srcreg->readlong;
    input->ddp_start       = AT_INPUT_SERIAL_OFFSET;
    input->input_unaligned = FALSE;
    input->trans_hdr_size  = AT_INPUT_SERIAL_OFFSET + (DDP_OVERHEADBYTES >> 1);
    return (atalk_hes_fastswitch(input, srcreg));
}

/*
 * atalk_mci2mci_ppp
 *
 * Write a PPP header onto the front of the supplied packet, and then
 * hand the packet off to the transmit code.  This routine works between
 * cbus/mci cards.
 */
static boolean 
atalk_mci2mci_ppp (hwidbtype *input, atcachetype *cptr,
		   ushort remap_dst, ushort remap_src)
{
    hwidbtype* output = cptr->hwif;
    mciregtype* srcreg = input->mci_regptr; 
    mciregtype* dstreg = output->mci_regptr;
    ushort bytes = input->ddp_length;
    ushort xfercnt;

    /* If input is unaligned and output interface is not cBus, then we
     * cannot support odd byte starts.  Send the packet to process
     * level.
     */
    if (input->input_unaligned && !(output->status & IDB_CBUS))
	return (FALSE);
    
    ATALK2_M2M_START;
    ATALK2_M2M_START2;
    ATALK2_M2M_START3;
    atalk_write_serial_header(dstreg, cptr,
			      input->hop_len_word, input->input_unaligned);
    bytes += cptr->byte_len;

    if (atalk_DomainRouter_running) {
	xfercnt -= sizeof(ushort) * 4;
	write_remapped_header(dstreg, remap_dst, remap_src);
    } else
        xfercnt -= (input->input_unaligned ? sizeof(uchar) : sizeof(ushort));
    if (input->input_unaligned)
	bytes |= MCI_TX_ODDALIGN;
    ATALK2_M2M_FINISH;
    return (TRUE);
}

/*
 * atalk_samemci_ppp
 *
 * Write a PPP header onto the front of the supplied packet, and then
 * hand the packet off to the transmit code.  This routine works within a
 * cbus/mci card.
 */
static boolean 
atalk_samemci_ppp (hwidbtype *input, atcachetype *cptr,
		   ushort remap_dst, ushort remap_src)
{
    mciregtype *regptr = input->mci_regptr;
    hwidbtype *output = cptr->hwif;
    ushort size_to_xmit = input->ddp_length + cptr->byte_len;

    ATALK_SAME_START(input, regptr, output);
    
    regptr->argreg = input->ddp_start - cptr->word_len;
    regptr->cmdreg = MCI_CMD_TX1_SELECT;
    atalk_write_serial_header(regptr, cptr,
			      input->hop_len_word, input->input_unaligned);

    if (atalk_DomainRouter_running)
    	write_remapped_header(regptr, remap_dst, remap_src);

    if (input->input_unaligned)
	size_to_xmit |= MCI_TX_ODDALIGN;
    atalk_same_finish(input, regptr, output, size_to_xmit);
    return (TRUE);
}

/**********************************************************************
 *                 ATM-DXI					      *
 **********************************************************************/

/*
 * atalk_atm_dxi_fastswitch
 *
 * Very high speed switching of atm-dxi encapsulated AppleTalk datagram.
 */
static boolean 
atalk_atm_dxi_fastswitch (hwidbtype *input)
{
    mciregtype *srcreg;
    ushort offset;

    offset = input->rxoffset +
	btow(input->atm_dxi_stuff->fast_input_encsize);
    /*
     * Setup ddp data read pointer to check the hop count.
     */
    srcreg = input->mci_regptr;
    srcreg->argreg   = offset;
    srcreg->cmdreg   = MCI_CMD_RX_SELECT;
    input->hop_and_cksum   = srcreg->readlong;
    input->src_dst_net     = srcreg->readlong;
    input->nodes_and_socks = srcreg->readlong;
    input->ddp_start       = offset;
    input->input_unaligned = FALSE;
    input->trans_hdr_size  = offset + (DDP_OVERHEADBYTES >> 1);
    return (atalk_hes_fastswitch(input, srcreg));
}

/*
 * atalk_mci2mci_atm_dxi
 *
 * Write an atm-dxi header onto the front of the supplied
 * packet, and then hand the packet off to the transmit code.  This
 * routine works between cbus/mci cards.
 */
static boolean 
atalk_mci2mci_atm_dxi (hwidbtype *input, atcachetype *cptr,
			ushort remap_dst, ushort remap_src)
{
    hwidbtype* output = cptr->hwif;
    mciregtype* srcreg = input->mci_regptr; 
    mciregtype* dstreg = output->mci_regptr;
    ushort bytes = input->ddp_length;
    ushort xfercnt;

    ATALK2_M2M_START;
    ATALK2_M2M_START2;
    ATALK2_M2M_START3;
    atalk_write_serial_header(dstreg, cptr,
			      input->hop_len_word, input->input_unaligned);
    bytes += cptr->byte_len;
    if (atalk_DomainRouter_running) {
	xfercnt -= sizeof(ushort) * 4;
	write_remapped_header(dstreg, remap_dst, remap_src);
    } else 
	xfercnt -= (input->input_unaligned ? sizeof(uchar) : sizeof(ushort));
    INC_DXI_PVC_FAST_OUTPUT(HWIDB_TO_DXI_PVC(output,
					     cptr->subint_reference_num),
			    bytes);
    if (input->input_unaligned)
	bytes |= MCI_TX_ODDALIGN;
    ATALK2_M2M_FINISH;
    return (TRUE);
}
/*
 * atalk_samemci_atm_dxi
 *
 * Write an atm-dxi header onto the front/back of the supplied packet, and
 * then hand the packet off to the transmit code.  This routine works
 * within a cbus/mci card.
 */
static boolean 
atalk_samemci_atm_dxi (hwidbtype *input, atcachetype* cptr,
			ushort remap_dst, ushort remap_src)
{
    mciregtype *regptr;
    hwidbtype *output;
    ushort size_to_xmit;

    regptr  = input->mci_regptr;
    output   = cptr->hwif;
    size_to_xmit = input->ddp_length + cptr->byte_len;

    ATALK_SAME_START(input, regptr, output);
    
    regptr->argreg = input->ddp_start - cptr->word_len;
    regptr->cmdreg = MCI_CMD_TX1_SELECT;
    atalk_write_serial_header(regptr, cptr,
			      input->hop_len_word, input->input_unaligned);
    if (atalk_DomainRouter_running)
	write_remapped_header(regptr, remap_dst, remap_src);
    
    INC_DXI_PVC_FAST_OUTPUT(HWIDB_TO_DXI_PVC(output,
					     cptr->subint_reference_num),
			    size_to_xmit);
    if (input->input_unaligned)
	size_to_xmit |= MCI_TX_ODDALIGN;
    atalk_same_finish(input, regptr, output, size_to_xmit);
    return (TRUE);
}
/************************************************************************
 *				SMDS (ugh)				*
 ************************************************************************/
/*
 * atalk_smds_fastswitch
 *
 * Very high speed switching of smds encapsulated AppleTalk datagram.
 */
static boolean 
atalk_smds_fastswitch (hwidbtype *input)
{
    mciregtype *srcreg;
    ulong temp;

    temp = input->rxoffset + btow(input->encsize);
    /*
     * Setup ddp data read pointer to check the hop count.
     */
    srcreg = (mciregtype *)input->mci_regptr;
    srcreg->argreg   = temp;
    srcreg->cmdreg   = MCI_CMD_RX_SELECT;
    input->hop_and_cksum   = srcreg->readlong;
    input->src_dst_net     = srcreg->readlong;
    input->nodes_and_socks = srcreg->readlong;
    input->ddp_start       = temp;
    input->input_unaligned = FALSE;
    input->trans_hdr_size  = temp + (DDP_OVERHEADBYTES >> 1);
    return (atalk_hes_fastswitch(input, srcreg));
}
static uchar sm_lastbuffer[12];
static ushort sm_pad_offset;
ushort atalk_smds_pad;

/*
 * atalk_write_smds_trailer
 * trailer is already set up - write it to the end
 */
inline void atalk_write_smds_trailer (mciregtype *regptr,
					     hwidbtype *input)
{
    ulong *tptr;
    
    regptr->argreg = sm_pad_offset;
    regptr->cmdreg = MCI_CMD_TX1_SELECT;
    tptr = (ulong *)sm_lastbuffer;
    if (input->input_unaligned == FALSE) {
	/* aligned trailer - write last long, then more if we have a pad */
	regptr->write1long = *tptr++;
	if (atalk_smds_pad) {
	    regptr->write1long = *tptr++;
	}
    } else {
	/* unaligned trailer - write at least 5 bytes, then more if we
	 * have a pad
	 */
	regptr->write1long = *tptr++;
	if (atalk_smds_pad) {
	    regptr->write1long = *tptr++;
	    regptr->write1short = GETSHORT(tptr);
	} else {
	    regptr->write1short = GETSHORT(tptr);
	}
    }
}

/*
 * atalk_smds_setup_inline
 * read the trailer bytes, change the cache for the per packet
 * info and create the entire trailer
 */
inline void atalk_smds_setup_inline (hwidbtype *input,
					    hwidbtype *output,
					    mciregtype *srcreg,
					    atcachetype *cptr,
					    ushort size_to_xmit)
{
    smds_pdb_t *spd = output->smds_pdb;
    smdstype *sm;

    sm_pad_offset = input->ddp_start + ((size_to_xmit & ~0x3) >> 1);
    spd = output->smds_pdb;
    size_to_xmit += cptr->byte_len - spd->fast_output_modelen;
    atalk_smds_pad = 3 - ((size_to_xmit + 3) % 4);
    
    if (input->input_unaligned == FALSE) {
	/* Aligned datagram - no odd offsets here */
	sm = (smdstype *)&cptr->m.macstring[spd->fast_output_modelen];
	sm->smds_xhlpi = (sm->smds_xhlpi & ~3) | atalk_smds_pad;
	sm->smds_basize = size_to_xmit - 4 + atalk_smds_pad;
	sm->smds_betag = output->smds_begin_tag++;
	if (atalk_smds_pad) {
	    srcreg->argreg = sm_pad_offset;
	    srcreg->cmdreg = MCI_CMD_RX_SELECT;
	    PUTLONG(sm_lastbuffer, srcreg->readlong);
	    switch (atalk_smds_pad) {
	    case 3:
		PUTLONG(&sm_lastbuffer[1], 0);
		PUTLONG(&sm_lastbuffer[4], GETLONG(&sm->smds_rsvd));
		break;
	    case 2:
		PUTSHORT(&sm_lastbuffer[2], 0);
		PUTLONG(&sm_lastbuffer[4], GETLONG(&sm->smds_rsvd));
		break;
	    case 1:
		sm_lastbuffer[3] = 0;
		PUTLONG(&sm_lastbuffer[4], GETLONG(&sm->smds_rsvd));
		break;
	    default:		/* Not needed */
		PUTLONG(sm_lastbuffer, GETLONG(&sm->smds_rsvd));
		break;
	    }
	} else {
	    /* No pad - just transfer the smds trailer */
	    PUTLONG(sm_lastbuffer, GETLONG(&sm->smds_rsvd));
	}
    } else {
	/*
	 * Unaligned data here.  We always have 1 byte of data in
	 * the sm_lastbuffer.  It also uses a different cache, so the
	 * sm pointer needs to point into that cache.
	 */
	sm = (smdstype *)&cptr->s.macstring[spd->fast_output_modelen - 1];
	sm->smds_xhlpi = (sm->smds_xhlpi & ~3) | atalk_smds_pad;
	sm->smds_basize = size_to_xmit - 4 + atalk_smds_pad;
	sm->smds_betag = output->smds_begin_tag++;
	srcreg->argreg = sm_pad_offset;
	srcreg->cmdreg = MCI_CMD_RX_SELECT;
	PUTLONG(sm_lastbuffer, srcreg->readlong);
	switch (atalk_smds_pad) {
	case 3:
	    PUTLONG(&sm_lastbuffer[2], 0);
	    PUTLONG(&sm_lastbuffer[5], GETLONG(&sm->smds_rsvd));
	    break;
	case 2:
	    PUTSHORT(&sm_lastbuffer[3], 0);
	    PUTLONG(&sm_lastbuffer[5], GETLONG(&sm->smds_rsvd));
	    break;
	case 1:
	    sm_lastbuffer[4] = 0;
	    PUTLONG(&sm_lastbuffer[5], GETLONG(&sm->smds_rsvd));
	    break;
	default:
	    PUTLONG(&sm_lastbuffer[1], GETLONG(&sm->smds_rsvd));
	}
    }
}
/*
 * atalk_mci2mci_smds
 *
 * Write a smds header onto the front/rear of the supplied
 * packet, and then hand the packet off to the transmit code.  This
 * routine works between cbus/mci cards.
 */

static boolean 
atalk_mci2mci_smds (hwidbtype *input, atcachetype *cptr,
			ushort remap_dst, ushort remap_src)
{
    hwidbtype* output = cptr->hwif;
    mciregtype* srcreg = (mciregtype *) input->mci_regptr; 
    mciregtype* dstreg = (mciregtype *) output->mci_regptr;
    ushort bytes = input->ddp_length;
    ushort xfercnt;

    /* If input is unaligned and output interface is not cBus, then we
     * cannot support odd byte starts.  Send the packet to process
     * level.
     */
    if (input->input_unaligned && !(output->status & IDB_CBUS))
	return (FALSE);

    ATALK2_M2M_START;
    /*
     * set up the smds header and trailer
     */
    atalk_smds_setup_inline(input, output, srcreg, cptr, bytes);

    ATALK2_M2M_START2;

    /*
     * calculate the pad offset (destination), then write the trailer
     */
    bytes += cptr->byte_len;
    bytes += atalk_smds_pad;
    sm_pad_offset = (bytes >> 1); 
    if (atalk_smds_pad != 0)
	sm_pad_offset -= btow(sizeof(ulong));
    atalk_write_smds_trailer(dstreg, input);
    /*
     * write the smds header
     */
    ATALK2_M2M_START3;

    atalk_write_serial_header(dstreg, cptr,
			      input->hop_len_word, input->input_unaligned);
    bytes += sizeof(ulong); /* add size of trailer */
    xfercnt &= ~0x03; /* remove for the lastlong */
    /*
     * Reset the srcreg address to the next word of the pdu to write,
     * then do the DomainRouter stuff
     */
    if (atalk_DomainRouter_running) {
	srcreg->argreg = input->ddp_start + 4;
	xfercnt -= sizeof(ushort) * 4;
	write_remapped_header(dstreg,remap_dst,remap_src);
    } else {
	srcreg->argreg = input->ddp_start + 1;
	xfercnt -= sizeof(ushort);
    }
    srcreg->cmdreg = MCI_CMD_RX_SELECT;
    if (xfercnt & 0x2) {
	/*
	 * We've already written the first 2 bytes, in write_serial_header.
	 * mci2mci will write longs, only.  Therefore, if we have
	 * an extra word left off, write it here, and transfer 1
	 * less word
	 */
	/* Might be able to get this from input->hop_and_cksum */
	dstreg->write1short = srcreg->readshort;
	xfercnt -= sizeof(ushort);
    }
    if (input->input_unaligned)
	bytes |= MCI_TX_ODDALIGN;
    ATALK2_M2M_FINISH;
    return (TRUE);
}

/*
 * atalk_samemci_smds
 *
 * Write a smds header/trailer onto the front/rear of the supplied packet, and
 * then hand the packet off to the transmit code.  This routine works
 * within a cbus/mci card.
 */
static boolean 
atalk_samemci_smds (hwidbtype *input, atcachetype* cptr,
			ushort remap_dst, ushort remap_src)
{
    mciregtype *regptr;
    hwidbtype *output;
    ushort size_to_xmit;

    regptr  = (mciregtype *)input->mci_regptr;
    output   = cptr->hwif;
    size_to_xmit = input->ddp_length;

    /* Set up the smds header and trailer */
    atalk_smds_setup_inline (input, output, regptr, cptr, size_to_xmit);
    
    ATALK_SAME_START(input, regptr, output);
    
    /* write the trailer */
    atalk_write_smds_trailer(regptr, input);

    /* write the header */
    regptr->argreg = input->ddp_start - cptr->word_len;
    regptr->cmdreg = MCI_CMD_TX1_SELECT;
    atalk_write_serial_header(regptr, cptr,
			      input->hop_len_word, input->input_unaligned);
    /* include the encapsize and the trailer */
    size_to_xmit += cptr->byte_len + atalk_smds_pad + sizeof(ulong);
    if (atalk_DomainRouter_running)
	write_remapped_header(regptr, remap_dst, remap_src);
    if (input->input_unaligned)
	size_to_xmit |= MCI_TX_ODDALIGN;
    atalk_same_finish(input, regptr, output, size_to_xmit);
    return (TRUE);
}

/**********************************************************************
 *                      ATM (SNAP Encapsulation)                      *
 **********************************************************************/
#define	ATM_HDR_WORDS_IN	btow(vc->encapsize)

/**********************************************************************
 *                 ATM						      *
 **********************************************************************/
 
/*
 * atalk_atm_fastswitch
 *
 * Very high speed switching of ATM encapsulated DDP datagram. This
 * routine has been optimized for speed, since it is in the inner loop of
 * the code that handles back-to-back datagrams.
 */
static boolean
atalk_atm_fastswitch (hwidbtype *input)
{
    mciregtype *srcreg;
    ushort offset=0;
    vc_info_t *vc;
 
    srcreg = input->mci_regptr;
    vc = input->atm_db->fast_vc_info;
    switch (vc->flags & VC_FLAG_AAL_MASK) {
    case ATM_VC_AAL5NLPID:
	offset = btow(ATM_NLPID_SNAP_HDRBYTES);
	break;
    case ATM_VC_AAL5LANE:
        offset = btow(ATM_SNAP_HDRBYTES);
	break;
    default:
	break;
    }

    vc->infast++;
    ATM_INFAST(input);
    input->ddp_start = input->rxoffset + ATM_HDR_WORDS_IN + offset;
    srcreg->argreg = input->ddp_start;
    srcreg->cmdreg = MCI_CMD_RX_SELECT;
    input->hop_and_cksum   = srcreg->readlong;
    input->src_dst_net     = srcreg->readlong;
    input->nodes_and_socks = srcreg->readlong;
    input->input_unaligned = FALSE;
    return(atalk_hes_fastswitch(input, srcreg));
}

/*
 * atalk_write_lane_snap_header
 *
 * Write the Phase-II style SNAP-SNAP header on a frame.
 * Stolen from the ether_snap_header equivalent...can't use the same
 * function for LANE because of the VCD/ENCTYPE for ATM.
 */

inline void
atalk_write_lane_snap_header (mciregtype* regptr, atcachetype* cptr,
			      ushort len, ushort hopw, ushort unaligned)

{
    longbyte lenlong;
    shortbyte lenw;
    lenw.d.sword = len - ATM_LANE_DATA_ENCAPBYTES;	/* we want ddp + 8 bytes */
    if (unaligned) {
	regptr->write1short = cptr->m.macstring[0];	/* vcd */
	regptr->write1long = cptr->s.maclong[0];	/* vcd, en, en, id */
	regptr->write1long = cptr->s.maclong[1];	/* id, dd, dd, dd */
	regptr->write1long = cptr->s.maclong[2];	/* dd, dd, dd, ss */
	regptr->write1long = cptr->s.maclong[3];	/* ss, ss, ss, ss */
	lenlong.d.byte[0] = cptr->s.macstring[16];	/* ss */
	lenlong.d.byte[1] = lenw.d.byte[0];		/* ll */
	lenlong.d.byte[2] = lenw.d.byte[1];		/* ll */
	lenlong.d.byte[3] = CONST_AA;			/* aa */
	regptr->write1long = lenlong.d.lword;
	regptr->write1long = CONST_AA_03_08_00;		/* aa, 03, oi, oi */
	regptr->write1long = CONST_07_80_9B_00
	                     | ((hopw & 0xff00) >> 8);	/* oi, tt, tt, hb */
    } else {
	regptr->write1long = cptr->m.maclong[0];	/* vc, vc, enc, enc  */
	regptr->write1long = cptr->m.maclong[1];	/* id, id, dd, dd */
	regptr->write1long = cptr->m.maclong[2];	/* dd, dd, dd, dd */
	regptr->write1long = cptr->m.maclong[3];	/* ss, ss, ss, ss */
	regptr->write1short = cptr->m.macshort[8];	/* ss, ss */ 
	regptr->write1short = lenw.d.sword;		/* ll, ll */
	regptr->write1long = CONST_AA_AA_03_08;		/* aa, aa, 03, 08 */
	regptr->write1long = CONST_00_07_80_9B; 	/* 00, 07, 80, 9b */
	regptr->write1short = hopw;
    }
}

/*
 * atalk_samemci_atm
 *
 * Write an atm header onto the front of the supplied packet, and
 * then hand the packet off to the transmit code.  This routine works
 * within a cbus/mci card.
 */
static boolean 
atalk_samemci_atm (hwidbtype *input, atcachetype* cptr,
			ushort remap_dst, ushort remap_src)
{
    mciregtype *regptr;
    hwidbtype *output;
    ushort size_to_xmit;

    regptr = input->mci_regptr;
    output = cptr->hwif;
    size_to_xmit = input->ddp_length + cptr->byte_len;

    ATALK_SAME_START(input, regptr, output);
    
    regptr->argreg = input->ddp_start - cptr->word_len;
    regptr->cmdreg = MCI_CMD_TX1_SELECT;
    if (lec_enabled(cptr->idb)) {
	atalk_write_lane_snap_header(regptr, cptr, size_to_xmit,
				     input->hop_len_word, input->input_unaligned);
    } else {
	atalk_write_serial_header(regptr, cptr,
				  input->hop_len_word, input->input_unaligned);
    }

    if (atalk_DomainRouter_running)
	write_remapped_header(regptr, remap_dst, remap_src);

    if (input->input_unaligned)
	size_to_xmit |= MCI_TX_ODDALIGN;
    atalk_same_finish(input, regptr, output, size_to_xmit);
    ATM_OUTFAST(output);
    ATM_VCOUTFAST(output, cptr->m.macshort[0]);
    return (TRUE);
}

/**********************************************************************
 *                   802.3 Ethernet in LEX (SNAP Encapsulation)       *
 **********************************************************************/

/*
 * atalk_lex_snap_fastswitch
 *
 * Very high speed switching of SNAP (Phase II) encapsulated Ethernet 
 * AppleTalk datagram. 
 * This routine has bee heavily optimized for speed, since it
 * is in the inner loop of the code that handles back-to-back datagrams.
 */
static boolean 
atalk_lex_snap_fastswitch (hwidbtype *input)
{
    mciregtype *srcreg;
 
    /*
     * Setup DDP data read pointer to check the hop count.
     */
    srcreg = input->mci_regptr;
    srcreg->argreg = LEX_AT_ISO2_OFFSET;
    srcreg->cmdreg = MCI_CMD_RX_SELECT;
    input->hop_and_cksum   = srcreg->readlong;
    input->src_dst_net     = srcreg->readlong;
    input->nodes_and_socks = srcreg->readlong;
    input->ddp_start       = LEX_AT_ISO2_OFFSET;
    input->input_unaligned = FALSE;
    return (atalk_hes_fastswitch(input, srcreg));
}

/*
 * atalk_write_lex_snap_header
 *
 * This function is used to write both even aligned and odd-aligned
 * Phase-II Ethernet-in-LEX/PPP headers.
 *
 * the "len" passed in is the DDP frame length -- to get the 802.3 frame
 * length, we add 8 bytes (SNAP_HDRBYTES) to the DDP size and jam that into
 * the packet.
 */
inline void
atalk_write_lex_snap_header (mciregtype* regptr, atcachetype* cptr,
			     ushort len, ushort hw, ushort unaligned)
{
    uint off = 0;
    uint length = (2 * IEEEBYTES);
    ushort sw;
    
    if (unaligned) {
	length += cptr->byte_len - ETHER_SNAP_ENCAPBYTES - 2;

	regptr->write1short = cptr->m.macstring[0];
	while (length > 0) {
	    regptr->write1short = cptr->s.macshort[off++];
	    length -= sizeof(short);
	}
	len += SNAP_HDRBYTES;
	sw = (cptr->s.macshort[off++] & 0xFF00);
	regptr->write1short = (sw | ((len & 0xFF00) >> 8));
	regptr->write1short = (CONST_AA | ((len & 0x00FF) << 8));
	regptr->write1short = CONST_AA_03;
	regptr->write1short = CONST_08_00;
	regptr->write1short = (CONST_07_00 | ((TYPE_ETHERTALK & 0xFF00) >> 8));
	regptr->write1short = (((TYPE_ETHERTALK & 0x00FF) << 8) | hw);
    } else {
	length += cptr->byte_len - ETHER_SNAP_ENCAPBYTES;
	while (length > 0) {
	    regptr->write1short = cptr->m.macshort[off++];
	    length -= sizeof(short);
	}
	regptr->write1short = len + SNAP_HDRBYTES;
	regptr->write1short = CONST_AA_AA;
	regptr->write1short = CONST_03_08;
	regptr->write1short = CONST_00_07;
	regptr->write1short = TYPE_ETHERTALK;
	regptr->write1short = hw;
    }
}

/*
 * atalk_mci2mci_lex_snap
 *
 * Write a SNAP ethernet header onto the front of the supplied packet,
 * and then hand the packet off to the transmit code.  This routine
 * works between cbus/mci cards.
 */
static boolean 
atalk_mci2mci_lex_snap (hwidbtype *input, atcachetype *cptr,
			ushort remap_dst, ushort remap_src)
{
    hwidbtype* output = cptr->hwif;
    mciregtype* srcreg = input->mci_regptr; 
    mciregtype* dstreg = output->mci_regptr;
    ushort bytes = input->ddp_length;
    ushort xfercnt;

    ATALK2_M2M_START;
    ATALK2_M2M_START2;
    ATALK2_M2M_START3;
    atalk_write_lex_snap_header(dstreg, cptr, bytes,
				input->hop_len_word, input->input_unaligned);
    bytes += cptr->byte_len;

    if (atalk_DomainRouter_running) {
	xfercnt -= sizeof(ushort) * 4;
	write_remapped_header(dstreg, remap_dst, remap_src);
    } else
	xfercnt -= (input->input_unaligned ? sizeof(uchar) : sizeof(ushort));

    if (input->input_unaligned)
	bytes |= MCI_TX_ODDALIGN;
    ATALK2_M2M_FINISH;
    return (TRUE);
}

/*
 * atalk_samemci_lex_snap
 *
 * Write a SNAP ethernet header onto the front of the supplied packet,
 * and then hand the packet off to the transmit code.  This routine
 * works within a cbus/mci card.
 */
static boolean 
atalk_samemci_lex_snap (hwidbtype *input, atcachetype *cptr,
			ushort remap_dst, ushort remap_src)
{
    mciregtype *regptr = input->mci_regptr;
    hwidbtype *output  = cptr->hwif;
    ushort size_to_xmit = input->ddp_length + cptr->byte_len;

    ATALK_SAME_START(input, regptr, output);
    
    regptr->argreg = input->ddp_start - cptr->word_len;
    regptr->cmdreg = MCI_CMD_TX1_SELECT;
    atalk_write_lex_snap_header(output->mci_regptr, cptr, input->ddp_length,
				  input->hop_len_word, input->input_unaligned);

    if (atalk_DomainRouter_running)
	write_remapped_header(regptr, remap_dst, remap_src);
    
    if (input->input_unaligned)
	size_to_xmit |= MCI_TX_ODDALIGN;
    atalk_same_finish(input, regptr, output, size_to_xmit);
    return (TRUE);
}

/**********************************************************************
 *                   ARPA Ethernet in LEX                             *
 **********************************************************************/

/*
 * atalk_lex_arpa_fastswitch
 *
 * Very high speed switching of ARPA encapsulated Ethernet 
 * AppleTalk datagram. (Phase I)
 * This routine has bee heavily optimized for speed, since it
 * is in the inner loop of the code that handles back-to-back datagrams.
 *
 * Except for where the datagram starts, this routine is very, very much
 * like the atalk_ether_fastswitch routine.
 */
static boolean 
atalk_lex_arpa_fastswitch (hwidbtype *input)
{
    mciregtype *srcreg;
    charlong	sniff1;
    charlong	sniff2;

    /*
     * Setup DDP data read pointer to check the hop count.
     */
    srcreg = input->mci_regptr;
    srcreg->argreg = LEX_AT_ETALK_OFFSET;
    srcreg->cmdreg = MCI_CMD_RX_SELECT;

    sniff1.d.lword = srcreg->readlong;
    if (sniff1.d.byte[0] != ALAP_DDP_LONG)
	return (FALSE);			/* we f/s only DDP longs */

    input->hop_len_word = (sniff1.d.byte[1] << 8) | sniff1.d.byte[2];
    sniff2.d.lword  = srcreg->readlong;
    input->dst_net  = (sniff2.d.byte[1] << 8) | sniff2.d.byte[2];
    sniff1.d.lword  = srcreg->readlong;
    input->src_net  = (sniff2.d.byte[3] << 8) | sniff1.d.byte[0];
    input->dst_node = sniff1.d.byte[1];
    input->src_node = sniff1.d.byte[2];
    input->dst_sock = sniff1.d.byte[3];
    input->ddp_start       = LEX_AT_ETALK_OFFSET+1;
    input->input_unaligned = TRUE;
    return (atalk_hes_fastswitch(input, srcreg));
}

inline void
atalk_write_lex_arpa_header (mciregtype *regptr, atcachetype *cptr,
			     ushort hw, ushort unaligned)
{
    ushort	off = 0;
    ushort	num_longs;
    ushort	num_shorts;

    /*
     * The "unaligned" sense here is reversed because the
     * ARPA Ethernet + LEX/PPP encapsulation is odd in length. (23 bytes)
     *
     * If the input started out unaligned, then adding an odd
     * length encap to an odd start will get us an even start.
     */
    if (unaligned) {
	/*
	 * We start out writing on an even boundary and finish
	 * on the odd hop-word.
	 */
	num_shorts = cptr->word_len - 1;
	num_longs  = num_shorts >> 1;
	while (num_longs > 0) {
	    regptr->write1long = cptr->m.maclong[off++];
	    --num_longs;
	}
	off <<= 1;			/* make it a word offset */
	if (num_shorts & 0x0001)
	    regptr->write1short = cptr->m.macshort[off++];

	regptr->write1short = (cptr->m.macshort[off] | ((hw & 0xff00)>>8));
    } else {
	/*
	 * We start out writing on an odd boundary and finish
	 * "on the line".
	 */
	regptr->write1short = cptr->m.macstring[0];
	num_shorts = btow(cptr->byte_len - 1);
	num_longs  = num_shorts >> 1;
	while (num_longs > 0) {
	    regptr->write1long = cptr->s.maclong[off++];
	    --num_longs;
	}
	if (num_shorts & 0x0001) {
	    off <<= 1;			/* make it a word offset */
	    regptr->write1short = cptr->s.macshort[off++];
	}
	regptr->write1short = hw;
    }
}
/*
 * atalk_samemci_lex_arpa
 *
 * Write a ethernet header and LLAP onto the front of the supplied packet,
 * and then hand the packet off to the transmit code.  This routine
 * works within a cbus/mci card.
 */
static boolean 
atalk_samemci_lex_arpa (hwidbtype *input, atcachetype *cptr,
			ushort remap_dst, ushort remap_src)
{
    mciregtype	*regptr = input->mci_regptr;
    hwidbtype	*output = cptr->hwif;
    ushort size_to_xmit = input->ddp_length + cptr->byte_len;

    ATALK_SAME_START(input, regptr, output);

    regptr->argreg = input->ddp_start - cptr->word_len;
    regptr->cmdreg = MCI_CMD_TX1_SELECT;
    atalk_write_lex_arpa_header(regptr, cptr,
				input->hop_len_word, input->input_unaligned);

    if (atalk_DomainRouter_running)
	write_remapped_header(regptr, remap_dst, remap_src);
    
    /*
     * Since the size of an ethertalk header is odd, if the
     * DDP frame started on an even boundary, then the output
     * will start on an odd byte.
     */
    if (input->input_unaligned == FALSE)
	size_to_xmit |= MCI_TX_ODDALIGN;
    atalk_same_finish(input, output->mci_regptr,
		      output, size_to_xmit);
    return (TRUE);
}

/*
 * atalk_mci2mci_lex_arpa
 *
 * Write a ethernet header and LLAP onto the front of the supplied packet,
 * and then hand the packet off to the transmit code.  This routine
 * works between cbus/mci cards.
 */
static boolean 
atalk_mci2mci_lex_arpa (hwidbtype *input, atcachetype *cptr,
			ushort remap_dst, ushort remap_src)
{
    hwidbtype*	output;
    mciregtype* srcreg;
    mciregtype* dstreg;
    ushort bytes, xfercnt;
    output = cptr->hwif;
    srcreg = input->mci_regptr;
    dstreg = output->mci_regptr;
    bytes = input->ddp_length;

    /*
     * Reserve a buffer in window 1 of the destination MCI
     */
    dstreg->argreg = output->mci_index;
    dstreg->cmdreg = MCI_CMD_SELECT;

    dstreg->argreg = output->buffer_pool;
    dstreg->cmdreg = MCI_CMD_TX1_RESERVE;
    if (dstreg->cmdreg != MCI_RSP_OKAY) { /* TX buffer not reserved */
	if (output->priority_list) {
	    /* if sorting traffic and interface is congested, process switch */
	    return(FALSE);
	} else {
	    output->counters.output_total_drops++;
	    output->atalk_outputerrs++;
	    srcreg->cmdreg = MCI_CMD_RX_FLUSH;
	    return (TRUE);
	}
    }
    output->counters.outputs_fast[ACCT_PROTO_APPLE]++;
    atalk_stat[ATALK_XFASTFORWARD]++;
    dstreg->argreg = 0;
    dstreg->cmdreg = MCI_CMD_TX1_SELECT;
    xfercnt = bytes;
    output->counters.tx_cumbytes_fast[ACCT_PROTO_APPLE] += bytes;

    atalk_write_lex_arpa_header(dstreg, cptr,
				input->hop_len_word, input->input_unaligned);
    bytes += ETALK_ENCAPBYTES;
    if (input->input_unaligned == FALSE)
	bytes |= MCI_TX_ODDALIGN;

    if (atalk_DomainRouter_running) {
	xfercnt -= sizeof(ushort) * 4;
	write_remapped_header(dstreg, remap_dst, remap_src);
    } else
	xfercnt -= (input->input_unaligned ? sizeof(uchar) : sizeof(ushort));
    /* 
     * copy bulk of datagram from one MCI to another. 
     */
    dstreg->argreg = bytes;
    mci2mci(&srcreg->readlong, &dstreg->write1long, xfercnt);
    dstreg->cmdreg = MCI_CMD_TX1_START;
    srcreg->cmdreg = MCI_CMD_RX_FLUSH;
    return (TRUE);
}


/*
 * atfast_CntrlSetupIF
 * Setup the interface fields for MCI/MEC fast switching.  Here we perform
 * all necessary validation of the hardware but we do not generate any
 * messages.  Messages are generated only after explicit initialization.
 */
static int
atfast_CntrlSetupIF (hwidbtype* idb)
{
    mcitype* mci;
    dcitype* dci;
    dialerdbtype *ddb = get_dialerdb(idb);
    idbtype *swidb;
    atalkidbtype *atalkidb = NULL;

    FOR_ALL_SWIDBS_ON_HW(idb, swidb) {
        atalkidb = atalk_getidb(swidb);
        if (atalkidb)
            break;
    }

    if (!atalkidb)
	return (fsNotSupported);

    /* Don't go any further if the user has disabled route caching */
    if (idb->atalk_routecache == fsUserDisabled)
	return(fsUserDisabled);

    idb->atalk_fastoddoutadj = 1;

    if ((idb->status & (IDB_MCI | IDB_CBUS | IDB_TR)) == 0)
	return (fsNotSupported);

    mci = idb->devctl;

    /* Do some hardware checks for imcompatible versions */
    if (idb->status & IDB_MCI) {
	if (idb->status & IDB_ETHER) {
	    /*
	     * MCI ethernet controllers do not properly recognize AT
	     * packets until uCode version 1.7 so we must check it.
	     * Beginning with uCode version 1.9, the off by one bug is
	     * fixed.
	     */
	    if (mci->hw_version == 0x100) {
		if (mci->sw_version < 0x107)
		    return (fsOldHardware);
		else if (mci->sw_version >= 0x109)
		    idb->atalk_fastoddoutadj = 0;
	    }
	} else if (idb->status & IDB_SERIAL) {
	    /*
	     * MCI controllers prior to 1.4 have the TX1 bug which we
	     * do not check for in the fast switching handlers so we
	     * simply disable FS for eariler versions.
	     */
	    if (mci->hw_version == 0x100)
		if (mci->sw_version < 0x104)
		    return (fsOldHardware);
	}
    } else if (idb->status & IDB_CBUS) {
	/*
	 * MEC controllers prior to 1.07/2.02, have a bug in some
	 * flavors of odd byte alignments so we work around this by
	 * increasing all MEC odd starts by one byte.  This causes us
	 * to occasionally squirt an extra byte and upsets Digital's
	 * PathWorks.  	 
	 */
	if (idb->status & IDB_ETHER) {
	    if (mci->hw_version == (CBUS_HARDWARE << 8)) {
		dci = mci->dci[idb->cbus_ctrlr_index];
		if (dci->hw_version == 0x500)
		    if (dci->sw_version > 0x106)
			idb->atalk_fastoddoutadj = 0;
		if (dci->hw_version == 0x501)
		    if (dci->sw_version > 0x201)
			idb->atalk_fastoddoutadj = 0;
	    } else
		idb->atalk_fastoddoutadj = 0;
	} else
	    idb->atalk_fastoddoutadj = 0;
    }

    if (idb->status & IDB_SERIAL) {
	if (idb->enctype == ET_PPP && (ddb && (ddb->dialer != DIALER_NONE)))
	    return (fsNotSupported);

	/* No fastswitching on nonextended AppleTalk interfaces */
	if (!atalkif_net_extended(atalkidb))
	    return (fsNotSupported);

	switch (idb->enctype) {
	  case ET_HDLC:
	  case ET_FRAME_RELAY:
	  case ET_PPP:
	  case ET_ATM_DXI:
	  case ET_FR_IETF:
	  case ET_SMDS:
	  case ET_SMDS_DXI:
	    break;

	default:
	    return (fsIncompLinkProtocol);
	}
	idb->fast_switch_flag |= FSF_SNAP_APPLE;
	return (fsNotRouting);
    }

    if (idb->status & IDB_FDDI) {
	idb->fast_switch_flag |= FSF_APPLE;
	return (fsNotRouting);
    }

    /*
     * For the high-end platforms, we support only the CTR (CTR and
     * TRIP) cards.
     */
    if (idb->status & IDB_TR) {
	if (idb->type == IDBTYPE_CTR) {
	    idb->fast_switch_flag |= FSF_APPLE;
	    return (fsNotRouting);
	} else
	    return (fsNotSupported);
    }

    if (idb->status & IDB_ETHER || idb->status & IDB_LEX) {
	if (idb->status & IDB_MCI)
	    idb->atalk_fastoddoutadj = 1;
	if (atalkidb->atalk_enctype == ET_ETHERTALK) {
	    if (idb->enctype != ET_ARPA) 
		return (fsIncompLinkProtocol);
	    idb->fast_switch_flag |= FSF_APPLE;
	} else {
	    idb->fast_switch_flag |= FSF_SNAP_APPLE;
	}
	return (fsNotRouting);
    }

    /*
     * Info. for AIP FS setup.
     */
    if (is_atm(idb)) {
	idb->fast_switch_flag |= FSF_SNAP_APPLE;
	return (fsNotRouting);
    }

    return (fsNotSupported);
}

/*
 * Register the AppleTalk fast-switching entry points with the modularity
 * points in the rest of the system.
 */
static void 
atalk_mci_fs_register_drivers (ulong flags, hwidbtype* hwidb)
{
    /*
     * Check to see if we're configuring Appletalk
     */
    if (!(flags & (FSF_APPLE | FSF_SNAP_APPLE)))
	return;

    hwidb->atalk_routecache = atfast_CntrlSetupIF(hwidb);
    if (hwidb->fast_switch_flag & (FSF_APPLE | FSF_SNAP_APPLE)) {

	fs_table_fill((fastswitch_type)return_false, "return_false",
		      RXTYPE_APPLE);
	fs_table_fill((fastswitch_type)return_false, "return_false",
		      RXTYPE_SNAP_APPLE);

	reg_add_ether_fs(RXTYPE_APPLE,
			 (service_ether_fs_type)atalk_ether_fastswitch,
			 "atalk_ether_fastswitch");

	reg_add_ether_fs(RXTYPE_SNAP_APPLE,
			 (service_ether_fs_type)atalk_ether_snap_fastswitch,
			 "atalk_ether_snap_fastswitch");

	reg_add_fddi_fs(RXTYPE_APPLE,
			(service_fddi_fs_type)atalk_fddi_fastswitch,
			"atalk_fddi_fastswitch");

	reg_add_token_fs(RXTYPE_APPLE,
			 (service_token_fs_type)atalk_token_snap_fastswitch,
			 "atalk_token_snap_fastswitch");

	reg_add_hdlc_fs(RXTYPE_APPLE,
			(service_hdlc_fs_type)atalk_hdlc_fastswitch,
			"atalk_hdlc_fastswitch");
	
	reg_add_ppp_fs(RXTYPE_APPLE,
		       (service_ppp_fs_type)atalk_ppp_fastswitch,
		       "atalk_ppp_fastswitch");
	
	reg_add_framerelay_fs(RXTYPE_APPLE,
			      (service_framerelay_fs_type)atalk_frame_fastswitch,
			      "atalk_frame_fastswitch");

	reg_add_framerelay_fs(RXTYPE_SNAP_APPLE,
			      (service_framerelay_fs_type)atalk_frame_ietf_fastswitch,
			      "atalk_frame_ietf_fastswitch");

	reg_add_atm_dxi_fs(RXTYPE_APPLE,
			   (service_atm_dxi_fs_type)atalk_atm_dxi_fastswitch,
			   "atalk_atm_dxi_fastswitch");

	reg_add_atm_dxi_fs(RXTYPE_SNAP_APPLE,
			   (service_atm_dxi_fs_type)atalk_atm_dxi_fastswitch,
			   "atalk_atm_dxi_fastswitch");

	reg_add_aip_fs(RXTYPE_APPLE,
			      (service_aip_fs_type)atalk_atm_fastswitch,
			      "atalk_atm_fastswitch");

	reg_add_aip_fs(RXTYPE_SNAP_APPLE,
			      (service_aip_fs_type)atalk_atm_fastswitch,
			      "atalk_atm_fastswitch");

	reg_add_smds_fs(RXTYPE_APPLE,
			(service_smds_fs_type)atalk_smds_fastswitch,
			"atalk_smds_fastswitch");

	reg_add_smds_fs(RXTYPE_SNAP_APPLE,
			(service_smds_fs_type)atalk_smds_fastswitch,
			"atalk_smds_fastswitch");

	reg_add_lex_fs(RXTYPE_APPLE, (service_lex_fs_type)atalk_lex_arpa_fastswitch,
			"atalk_lex_arpa_fastswitch");

	reg_add_lex_fs(RXTYPE_SNAP_APPLE, (service_lex_fs_type)atalk_lex_snap_fastswitch,
			"atalk_lex_snap_fastswitch");
	
    }
}

/*
 * atalk_mci_fs_init
 *
 * Register the AppleTalk high-end fastswitching code into the
 * rest of the router system.
 */
static void 
atalk_mci_fs_init (subsystype* subsys)
{
    atfs_mci2mci[AT_FS_ETHER_ARPA]	= atalk_mci2mci_ether_arpa;
    atfs_mci2mci[AT_FS_ETHER_SNAP]	= atalk_mci2mci_ether_snap;
    atfs_mci2mci[AT_FS_TOKEN_SNAP]	= atalk_mci2mci_token_snap;
    atfs_mci2mci[AT_FS_FDDI]		= atalk_mci2mci_fddi;
    atfs_mci2mci[AT_FS_HDLC]		= atalk_mci2mci_hdlc;
    atfs_mci2mci[AT_FS_FRAME]		= atalk_mci2mci_frame;
    atfs_mci2mci[AT_FS_FRAME_IETF]	= atalk_mci2mci_frame_ietf;
    atfs_mci2mci[AT_FS_PPP]		= atalk_mci2mci_ppp;
    atfs_mci2mci[AT_FS_LEX_ARPA]	= atalk_mci2mci_lex_arpa;
    atfs_mci2mci[AT_FS_LEX_SNAP]	= atalk_mci2mci_lex_snap;
    atfs_mci2mci[AT_FS_ATM_DXI]		= atalk_mci2mci_atm_dxi;

    /*
     * all these vectors return false to boost the
     * packet to the slow-switch path because the encapsulation
     * isn't defined yet and the code isn't written.
     */
    atfs_mci2mci[AT_FS_ATM]		= atalk_boost_to_process;
    atfs_mci2mci[AT_FS_SMDS]		= atalk_mci2mci_smds;

    atfs_samemci[AT_FS_ETHER_ARPA]	= atalk_samemci_ether_arpa;
    atfs_samemci[AT_FS_ETHER_SNAP]	= atalk_samemci_ether_snap;
    atfs_samemci[AT_FS_TOKEN_SNAP]	= atalk_samemci_token_snap;
    atfs_samemci[AT_FS_FDDI]		= atalk_samemci_fddi;
    atfs_samemci[AT_FS_HDLC]		= atalk_samemci_hdlc;
    atfs_samemci[AT_FS_FRAME]		= atalk_samemci_frame;
    atfs_samemci[AT_FS_FRAME_IETF]	= atalk_samemci_frame_ietf;
    atfs_samemci[AT_FS_PPP]		= atalk_samemci_ppp;
    atfs_samemci[AT_FS_LEX_ARPA]	= atalk_samemci_lex_arpa;
    atfs_samemci[AT_FS_LEX_SNAP]	= atalk_samemci_lex_snap;
    atfs_samemci[AT_FS_ATM_DXI]		= atalk_samemci_atm_dxi;
    atfs_samemci[AT_FS_ATM]		= atalk_samemci_atm;
    atfs_samemci[AT_FS_SMDS]		= atalk_samemci_smds;

    reg_add_register_drivers(atalk_mci_fs_register_drivers,
			     "atalk_mci_fs_register_drivers");
}

/*
 * Appletalk MCI FS subsystem header.
 *
 * This needs to come after the base fastswitching code in the system,
 * and it requires both the AppleTalk code and the high-end fastswitch
 * support code.
 */

#define ATMCIFS_MAJVERSION	1
#define ATMCIFS_MINVERSION	0
#define ATMCIFS_EDITVERSION	1

SUBSYS_HEADER(atalk_mci_fs, ATMCIFS_MAJVERSION, ATMCIFS_MINVERSION,
	      ATMCIFS_EDITVERSION,
	      atalk_mci_fs_init, SUBSYS_CLASS_PROTOCOL,
	      "req: fastswitch, atalk", NULL);

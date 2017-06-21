/* $Id: at_smrpfast_mci.c,v 3.1.2.1 1996/05/02 08:44:38 dwong Exp $
 * $Source: /release/112/cvs/Xsys/smrp/at_smrpfast_mci.c,v $
 *------------------------------------------------------------------
 * cBus AppleTalk SMRP fast switching support
 *
 * March 1996, Kent Leung
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: at_smrpfast_mci.c,v $
 * Revision 3.1.2.1  1996/05/02  08:44:38  dwong
 * Branch: California_branch
 * Commit SMRP-7kFastSwitch, Load Balancing, and AURP Modularity
 *
 * Revision 3.1  1996/04/16  23:03:44  kleung
 * Placeholder for CA commit.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "interface_private.h"
#include "registry.h"
#include "fastswitch_registry.h"
#include "subsys.h"
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
#include "../if/atm.h"
#include "../lane/lane.h"
#include "../atalk/at_registry.h"
#include "../atalk/at_fast_mci.h"
#include "smrp_private.h"
#include "smrp_registry.h"
#include "smrp_fast.h"
#include "at_smrpfast.h"
#include "../atalk/at_fast_mci.h"
#include "../hes/cbus_bcopy_inline.h"

#ifdef SMRP_FAST_DEBUG
#include "ttysrv.h"
#endif

boolean (*atsmrpfs_mci[AT_FS_MAX])(hwidbtype *, atcachetype *, ushort, ushort, tinybool);

#define DDPLENGTHMASK_LONG 0x03ff0000       /* length field for long */

/*
 * Forward declarations.
 */
static void atsmrp_mci_fs_init(subsystype*);
static boolean atsmrp_mci_fastswitch(hwidbtype*, SMRPCache*, tinybool);
static void atsmrp_RegisterPorts(idbtype*);
static boolean atsmrp_mci_ether_arpa(hwidbtype*, atcachetype*, ushort, ushort, tinybool);
static boolean atsmrp_mci_ether_snap(hwidbtype*, atcachetype*, ushort, ushort, tinybool);
static boolean atsmrp_mci_fddi(hwidbtype*, atcachetype*, ushort, ushort, tinybool);
static boolean atsmrp_mci_hdlc(hwidbtype*, atcachetype*, ushort, ushort, tinybool);
static boolean atsmrp_mci_frame(hwidbtype*, atcachetype*, ushort, ushort, tinybool);
static boolean atsmrp_mci_frame_ietf(hwidbtype*, atcachetype*, ushort, ushort, tinybool);
static boolean atsmrp_mci_ppp(hwidbtype*, atcachetype*, ushort, ushort, tinybool);
static boolean atsmrp_mci_atm_dxi(hwidbtype*, atcachetype*, ushort, ushort, tinybool);
static boolean atsmrp_mci_atm(hwidbtype*, atcachetype*, ushort, ushort, tinybool);
static boolean atsmrp_mci_lex_snap(hwidbtype*, atcachetype*, ushort, ushort, tinybool);
static boolean atsmrp_mci_lex_arpa(hwidbtype*, atcachetype*, ushort, ushort, tinybool);
static boolean atsmrp_mci_smds(hwidbtype*, atcachetype*, ushort, ushort, tinybool);

    
/*
 * SMRP Appletalk HES FS subsystem header
 */

#define ATSMRPMCIFS_MAJVERSION 1
#define ATSMRPMCIFS_MINVERSION 0
#define ATSMRPMCIFS_EDITVERSION  1

SUBSYS_HEADER(atsmrp_mci_fs, ATSMRPMCIFS_MAJVERSION, ATSMRPMCIFS_MINVERSION, 
	      ATSMRPMCIFS_EDITVERSION,
	      atsmrp_mci_fs_init, SUBSYS_CLASS_PROTOCOL,
	      "seq: smrp",
	      "req: fastswitch, atalk, smrp");

/*
 *------------------------------------------------------------------
 * atsmrp_mci_fs_init			(Module Internal Function)
 *
 * Initialization for ATSMRP 7000 fast switching.
 *------------------------------------------------------------------
 */
static void 
atsmrp_mci_fs_init (subsystype* subsys)
{
    atsmrpfs_mci[AT_FS_ETHER_ARPA]	= atsmrp_mci_ether_arpa;
    atsmrpfs_mci[AT_FS_ETHER_SNAP]	= atsmrp_mci_ether_snap;
    atsmrpfs_mci[AT_FS_FDDI]		= atsmrp_mci_fddi;
    atsmrpfs_mci[AT_FS_HDLC]		= atsmrp_mci_hdlc;
    atsmrpfs_mci[AT_FS_FRAME]		= atsmrp_mci_frame;
    atsmrpfs_mci[AT_FS_FRAME_IETF]	= atsmrp_mci_frame_ietf;
    atsmrpfs_mci[AT_FS_PPP]		= atsmrp_mci_ppp;
    atsmrpfs_mci[AT_FS_LEX_ARPA]	= atsmrp_mci_lex_arpa;
    atsmrpfs_mci[AT_FS_LEX_SNAP]	= atsmrp_mci_lex_snap;
    atsmrpfs_mci[AT_FS_ATM_DXI]		= atsmrp_mci_atm_dxi;
    atsmrpfs_mci[AT_FS_ATM]		= atsmrp_mci_atm;
    atsmrpfs_mci[AT_FS_SMDS]		= atsmrp_mci_smds;

    reg_add_atalk_smrp_fs_update_mcache(atsmrp_UpdateMCache,
					"atsmrp_UpdateMCache");
    reg_add_smrp_fs_register_ports(LINK_APPLETALK, atsmrp_RegisterPorts,
				   "atsmrp_RegisterPorts");
    reg_add_atalk_smrp_mci_fastswitch(smrpfast_mci_fastswitch, 
				      "smrpfast_mci_fastswitch");
}

/*
 *------------------------------------------------------------------
 * smrp_RegisterPorts			(Module Internal Function)
 *
 * Register the port for fast switching.
 *------------------------------------------------------------------
 */
static void
atsmrp_RegisterPorts (idbtype* idb)
{
    SMRPIdbsb* idbsb;
    hwidbtype *hwidb = hwidb_or_null(idb);

    if (hwidb == NULL)
	return;
    
    idbsb = smrp_GetIdbsb(idb);
    if (idbsb == NULL)
	return;

    /* We are now up if fsreason == fsEnabled, therefore can now reset */
    if (hwidb->atalk_routecache == fsEnabled) {
	/* 
	 * This is actually the function called when packet is going out
	 * this port.  All incoming packets are generically processed.
	 */
	idbsb->atsmrp_port->MCIFastswitch = atsmrp_mci_fastswitch;
	idbsb->atsmrp_port->Fastswitch_name = "SMRP AppleTalk";
    }
    
    idbsb->atsmrp_port->RouteCache = hwidb->atalk_routecache;
	
}

/*
 *------------------------------------------------------------------
 * This routine overwrites the DDP header.
 *------------------------------------------------------------------
 */
static inline void 
atsmrp_mci_write_ddp_header (mciregtype* regptr, ddplong *ddp,
			     ushort hopw, ushort unaligned)
{
    /* 
     * Recalculate length if inbound link type is different.
     * For now, it's an no-op.
     */

    /* 
     * Don't overwrite datagram length, but zero out hop count
     * and checksum.  The src net and node is replaced with local
     * addr.  The other fields don't change.  Skip DDP type field.  
     */
    if (unaligned) {
	regptr->write1long = (GETLONG(&ddp->length) >> 8) | (ALAP_DDP_LONG << 24);
	regptr->write1long = (GETLONG(&ddp->dnet) << 8) & ddp->snet[0];
	regptr->write1short = (GETSHORT(&ddp->snet) << 8) & ddp->dnode;
    } else {
	regptr->write1short = hopw;
	regptr->write1short = 0;
	regptr->write1long = GETLONG(&ddp->dnet);
	regptr->write1long = GETLONG(&ddp->dnode);
    }    
}

/*
 *------------------------------------------------------------------
 * This routine is mostly a regurgitation of at_fastswitch().
 * The DDP header is overwritten from the SMRP cache.  Then only
 * the supported media/encaps are f/s.
 *------------------------------------------------------------------
 */
static boolean
atsmrp_mci_fastswitch (hwidbtype *input, SMRPCache* cache, tinybool notdone)
{
    atcachetype	*cptr;
    hwidbtype	*output;
    mciregtype *regptr;
    
    if (cache == NULL)
	return (FALSE);
    
    cptr = &cache->mac.at;
    if (cptr == NULL)
	return (FALSE);

    output = cptr->idb->hwptr;
    regptr = input->mci_regptr;

    if (notdone) {
	
	regptr->argreg = output->mci_index;
	regptr->cmdreg = MCI_CMD_SELECT;
	
	regptr->argreg = output->buffer_pool;
	regptr->cmdreg = MCI_CMD_TX1_RESERVE;

	if (regptr->cmdreg != MCI_RSP_OKAY) {
	    output->counters.output_total_drops++;
	    return (FALSE);
	}

	regptr->argreg = input->ddp_start;
	regptr->cmdreg = MCI_CMD_TX1_SELECT;

	regptr->rx_select = input->ddp_start;
	cbus_bcopy(regptr, input->ddp_length);
    }

    /* 
     * Move pointer so we can write the new header.  If the input
     * packet came in aligned (i.e. input_aligned == FALSE) then we'll
     * have to back up a little bit more to account for the odd number
     * of bytes we're about to write.
     */
    regptr->argreg = input->ddp_start;
    regptr->cmdreg = MCI_CMD_TX1_SELECT;

    atsmrp_mci_write_ddp_header(regptr, (ddplong*)&cache->net.at, 
				input->hop_len_word, input->input_unaligned);
  
    return((*atsmrpfs_mci[cptr->encaptype])(input, cptr, input->dst_net,
					    input->src_net, notdone));
}

/*
 * ATSMRP_SAME_START
 *
 * Move the buffer to the output interface.  If it can't move the buffer,
 * the packet is dropped if queueing is not enabled; otherwise packet is
 * returned to process level.
 *
 * Called from atsmrp_samemci_xx().
 */
#define ATSMRP_SAME_START(input, regptr, output)                         \
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
    if (!notdone) {                                                     \
        regptr->argreg = (ushort)output->mci_index;                     \
        regptr->cmdreg = MCI_CMD_RX_MOVE;                               \
        if (regptr->cmdreg != MCI_RSP_OKAY) {                           \
   	    /*                                                          \ 
	     * if sorting traffic and interface is congested, process   \
	     * switch                                                   \
	     */                                                         \
	    if (output->priority_list) {                                \
	        return(FALSE);                                          \
	    } else {                                                    \
	        output->counters.output_total_drops++;                                  \
    	        output->atalk_outputerrs++;                             \
	        regptr->cmdreg = MCI_CMD_RX_FLUSH;                      \
	        return(TRUE);                                           \
	    }                                                           \
        }                                                               \
    }

/*
 * atsmrp_mci_ether_arpa
 *
 * Write a ethernet ARPA header onto the front of the supplied packet,
 * and then hand the packet off to the transmit code.  This routine
 * works within a cbus/mci card.
 */
static boolean 
atsmrp_mci_ether_arpa (hwidbtype *input, atcachetype *cptr,
		       ushort remap_dst, ushort remap_src, tinybool notdone)
{
    mciregtype	*regptr = input->mci_regptr;
    hwidbtype	*output = cptr->idb->hwptr;
    ushort size_to_xmit = input->ddp_length + cptr->byte_len;

    ATSMRP_SAME_START(input, regptr, output); 

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
 * atsmrp_mci_ether_snap
 *
 * Write a SNAP ethernet header onto the front of the supplied packet,
 * and then hand the packet off to the transmit code.  This routine
 * works within a cbus/mci card.
 */
static boolean 
atsmrp_mci_ether_snap (hwidbtype *input, atcachetype *cptr,
		       ushort remap_dst, ushort remap_src, tinybool notdone)
{
    mciregtype *regptr = input->mci_regptr;
    hwidbtype *output  = cptr->idb->hwptr;
    ushort size_to_xmit = input->ddp_length + ETHER_SNAP_ENCAPBYTES;

    ATSMRP_SAME_START(input, regptr, output);
    
    regptr->argreg = input->ddp_start - ISO2_OVERHEAD_SIZE;
    regptr->cmdreg = MCI_CMD_TX1_SELECT;
    atalk_write_ether_snap_header(output->mci_regptr, cptr, size_to_xmit,
				  input->hop_len_word, input->input_unaligned);

    if (size_to_xmit < MINETHERSIZE)
	size_to_xmit = MINETHERSIZE;
    if (input->input_unaligned)
	size_to_xmit |= MCI_TX_ODDALIGN;
    atalk_same_finish(input, regptr, output, size_to_xmit);
    return (TRUE);
}

/*
 * atsmrp_mci_fddi
 *
 * Write a FDDI SNAP header onto the front of the supplied packet,
 * and then hand the packet off to the transmit code.  This routine
 * works within a cbus/mci card.
 */
static boolean 
atsmrp_mci_fddi (hwidbtype *input, atcachetype *cptr,
		 ushort remap_dst, ushort remap_src, tinybool notdone)
{
    mciregtype *regptr = input->mci_regptr;
    hwidbtype *output = cptr->idb->hwptr;
    ushort size_to_xmit = input->ddp_length + FDDI_SNAP_ENCAPBYTES;

    ATSMRP_SAME_START(input, regptr, output);
    
    /* Move pointer so we can write the new header.  If the input
     * packet came in unaligned (i.e. input_aligned == TRUE) then
     * we'll to back up a little bit less.
     */
    regptr->argreg = input->ddp_start - FDDI_OVERHEAD_SIZE +
	(input->input_unaligned == FALSE ? 0 : UNALIGNED_OVERHEAD);

    regptr->cmdreg = MCI_CMD_TX1_SELECT;
    atalk_write_fddi_header(regptr, cptr, 
			    input->hop_len_word, input->input_unaligned);

    if (input->input_unaligned == FALSE)
	size_to_xmit |= MCI_TX_ODDALIGN; /* because FDDI encap len is odd */
    atalk_same_finish(input, regptr, output, size_to_xmit);
    return (TRUE);
}

/*
 * atsmrp_mci_hdlc
 *
 * Write a HDLC header onto the front of the supplied packet, and then
 * hand the packet off to the transmit code.  This routine works within a
 * cbus/mci card. 
 */
static boolean 
atsmrp_mci_hdlc (hwidbtype *input, atcachetype *cptr,
		 ushort remap_dst, ushort remap_src, tinybool notdone)
{
    mciregtype *regptr = input->mci_regptr;
    hwidbtype *output = cptr->idb->hwptr;
    ushort size_to_xmit = input->ddp_length + cptr->byte_len;

    ATSMRP_SAME_START(input, regptr, output);
    
    regptr->argreg = input->ddp_start - cptr->word_len;
    regptr->cmdreg = MCI_CMD_TX1_SELECT;
    atalk_write_serial_header(regptr, cptr,
			      input->hop_len_word, input->input_unaligned);

    if (input->input_unaligned)
	size_to_xmit |= MCI_TX_ODDALIGN;
    atalk_same_finish(input, regptr, output, size_to_xmit);
    return (TRUE);
}

/*
 * atsmrp_mci_frame
 *
 * Write a Non-IETF header onto the front of the supplied packet, and
 * then hand the packet off to the transmit code.  This routine works
 * within a cbus/mci card.
 */
static boolean 
atsmrp_mci_frame (hwidbtype *input, atcachetype *cptr,
		  ushort remap_dst, ushort remap_src, tinybool notdone)
{
    mciregtype *regptr = input->mci_regptr;
    hwidbtype *output = cptr->idb->hwptr;
    ushort size_to_xmit = input->ddp_length + cptr->byte_len;

    ATSMRP_SAME_START(input, regptr, output);
    
    regptr->argreg = input->ddp_start - cptr->word_len;
    regptr->cmdreg = MCI_CMD_TX1_SELECT;
    atalk_write_serial_header(regptr, cptr, 
			      input->hop_len_word, input->input_unaligned);

    INC_PVC_FAST_OUTPUT(HWIDB_TO_FR_PVC(output, cptr->subint_reference_num),
			size_to_xmit);
    if (input->input_unaligned)
	size_to_xmit |= MCI_TX_ODDALIGN;
    atalk_same_finish(input, regptr, output, size_to_xmit);
    return (TRUE);
}

/*
 * atsmrp_mci_frame_ietf
 *
 * Write a IETF header onto the front of the supplied packet, and
 * then hand the packet off to the transmit code.  This routine works
 * within a cbus/mci card.
 */
static boolean 
atsmrp_mci_frame_ietf (hwidbtype *input, atcachetype* cptr,
		       ushort remap_dst, ushort remap_src, tinybool notdone)
{
    mciregtype *regptr  = input->mci_regptr;
    hwidbtype *output   = cptr->idb->hwptr;
    ushort size_to_xmit = input->ddp_length + cptr->byte_len;

    ATSMRP_SAME_START(input, regptr, output);
    
    regptr->argreg = input->ddp_start - cptr->word_len;
    regptr->cmdreg = MCI_CMD_TX1_SELECT;
    atalk_write_serial_header(regptr, cptr,
			      input->hop_len_word, input->input_unaligned);

    INC_PVC_FAST_OUTPUT(HWIDB_TO_FR_PVC(output, cptr->subint_reference_num),
			size_to_xmit);
    
    if (input->input_unaligned)
	size_to_xmit |= MCI_TX_ODDALIGN;
    atalk_same_finish(input, regptr, output, size_to_xmit);
    return (TRUE);
}

/*
 * atsmrp_mci_ppp
 *
 * Write a PPP header onto the front of the supplied packet, and then
 * hand the packet off to the transmit code.  This routine works within a
 * cbus/mci card.
 */
static boolean 
atsmrp_mci_ppp (hwidbtype *input, atcachetype *cptr,
		ushort remap_dst, ushort remap_src, tinybool notdone)
{
    mciregtype *regptr = input->mci_regptr;
    hwidbtype *output = cptr->idb->hwptr;
    ushort size_to_xmit = input->ddp_length + cptr->byte_len;

    ATSMRP_SAME_START(input, regptr, output);
    
    regptr->argreg = input->ddp_start - cptr->word_len;
    regptr->cmdreg = MCI_CMD_TX1_SELECT;
    atalk_write_serial_header(regptr, cptr,
			      input->hop_len_word, input->input_unaligned);

    if (input->input_unaligned)
	size_to_xmit |= MCI_TX_ODDALIGN;
    atalk_same_finish(input, regptr, output, size_to_xmit);
    return (TRUE);
}

/*
 * atsmrp_mci_atm_dxi
 *
 * Write an atm-dxi header onto the front/back of the supplied packet, and
 * then hand the packet off to the transmit code.  This routine works
 * within a cbus/mci card.
 */
static boolean 
atsmrp_mci_atm_dxi (hwidbtype *input, atcachetype* cptr,
		    ushort remap_dst, ushort remap_src, tinybool notdone)
{
    mciregtype *regptr;
    hwidbtype *output;
    ushort size_to_xmit;

    regptr  = input->mci_regptr;
    output   = cptr->idb->hwptr;
    size_to_xmit = input->ddp_length + cptr->byte_len;

    ATSMRP_SAME_START(input, regptr, output);
    
    regptr->argreg = input->ddp_start - cptr->word_len;
    regptr->cmdreg = MCI_CMD_TX1_SELECT;
    atalk_write_serial_header(regptr, cptr,
			      input->hop_len_word, input->input_unaligned);
    INC_DXI_PVC_FAST_OUTPUT(HWIDB_TO_DXI_PVC(output,
					     cptr->subint_reference_num),
			    size_to_xmit);
    if (input->input_unaligned)
	size_to_xmit |= MCI_TX_ODDALIGN;
    atalk_same_finish(input, regptr, output, size_to_xmit);
    return (TRUE);
}

/*
 * atsmrp_mci_atm
 *
 * Write an atm header onto the front of the supplied packet, and
 * then hand the packet off to the transmit code.  This routine works
 * within a cbus/mci card.
 */
static boolean 
atsmrp_mci_atm (hwidbtype *input, atcachetype* cptr,
		ushort remap_dst, ushort remap_src, tinybool notdone)
{
    mciregtype *regptr;
    hwidbtype *output;
    ushort size_to_xmit;

    regptr = input->mci_regptr;
    output = cptr->idb->hwptr;
    size_to_xmit = input->ddp_length + cptr->byte_len;

    ATSMRP_SAME_START(input, regptr, output);
    
    regptr->argreg = input->ddp_start - cptr->word_len;
    regptr->cmdreg = MCI_CMD_TX1_SELECT;
    if (lec_enabled(cptr->idb)) {
	atalk_write_lane_snap_header(regptr, cptr, size_to_xmit,
				     input->hop_len_word, input->input_unaligned);
    } else {
	atalk_write_serial_header(regptr, cptr,
				  input->hop_len_word, input->input_unaligned);
    }

    if (input->input_unaligned)
	size_to_xmit |= MCI_TX_ODDALIGN;
    atalk_same_finish(input, regptr, output, size_to_xmit);
    ATM_OUTFAST(output);
    ATM_VCOUTFAST(output, cptr->m.macshort[0]);
    return (TRUE);
}


/*
 * atsmrp_mci_smds
 *
 * Write a smds header/trailer onto the front/rear of the supplied packet, and
 * then hand the packet off to the transmit code.  This routine works
 * within a cbus/mci card.
 */
boolean 
atsmrp_mci_smds (hwidbtype *input, atcachetype* cptr,
		 ushort remap_dst, ushort remap_src, tinybool notdone)
{
    mciregtype *regptr;
    hwidbtype *output;
    ushort size_to_xmit;

    regptr  = (mciregtype *)input->mci_regptr;
    output   = cptr->idb->hwptr;
    size_to_xmit = input->ddp_length;

    /* Set up the smds header and trailer */
    atalk_smds_setup_inline (input, output, regptr, cptr, size_to_xmit);
    
    ATSMRP_SAME_START (input, regptr, output);
    
    /* write the trailer */
    atalk_write_smds_trailer(regptr, input);

    /* write the header */
    regptr->argreg = input->ddp_start - cptr->word_len;
    regptr->cmdreg = MCI_CMD_TX1_SELECT;
    atalk_write_serial_header(regptr, cptr,
			      input->hop_len_word, input->input_unaligned);
    /* include the encapsize and the trailer */
    size_to_xmit += cptr->byte_len + atalk_smds_pad + sizeof(ulong);
    if (input->input_unaligned)
	size_to_xmit |= MCI_TX_ODDALIGN;
    atalk_same_finish(input, regptr, output, size_to_xmit);
    return (TRUE);
}

/*
 * atsmrp_mci_lex_snap
 *
 * Write a SNAP ethernet header onto the front of the supplied packet,
 * and then hand the packet off to the transmit code.  This routine
 * works within a cbus/mci card.
 */
static boolean 
atsmrp_mci_lex_snap (hwidbtype *input, atcachetype *cptr,
		     ushort remap_dst, ushort remap_src, tinybool notdone)
{
    mciregtype *regptr = input->mci_regptr;
    hwidbtype *output  = cptr->idb->hwptr;
    ushort size_to_xmit = input->ddp_length + cptr->byte_len;

    ATSMRP_SAME_START(input, regptr, output);
    
    regptr->argreg = input->ddp_start - cptr->word_len;
    regptr->cmdreg = MCI_CMD_TX1_SELECT;
    atalk_write_lex_snap_header(output->mci_regptr, cptr, input->ddp_length,
				  input->hop_len_word, input->input_unaligned);

    if (input->input_unaligned)
	size_to_xmit |= MCI_TX_ODDALIGN;
    atalk_same_finish(input, regptr, output, size_to_xmit);
    return (TRUE);
}

/*
 * atsmrp_mci_lex_arpa
 *
 * Write a ethernet header and LLAP onto the front of the supplied packet,
 * and then hand the packet off to the transmit code.  This routine
 * works within a cbus/mci card.
 */
static boolean 
atsmrp_mci_lex_arpa (hwidbtype *input, atcachetype *cptr,
		     ushort remap_dst, ushort remap_src, tinybool notdone)
{
    mciregtype	*regptr = input->mci_regptr;
    hwidbtype	*output = cptr->idb->hwptr;
    ushort size_to_xmit = input->ddp_length + cptr->byte_len;

    ATSMRP_SAME_START(input, regptr, output);

    regptr->argreg = input->ddp_start - cptr->word_len;
    regptr->cmdreg = MCI_CMD_TX1_SELECT;
    atalk_write_lex_arpa_header(regptr, cptr,
				input->hop_len_word, input->input_unaligned);

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

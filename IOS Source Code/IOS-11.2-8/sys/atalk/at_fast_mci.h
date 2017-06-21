/* $Id: at_fast_mci.h,v 3.1.2.1 1996/05/02 08:39:53 dwong Exp $
 * $Source: /release/112/cvs/Xsys/atalk/at_fast_mci.h,v $
 *------------------------------------------------------------------
 * AppleTalk cBus/MCI fast switching definitions
 *
 * March 1996, Kent Leung 
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: at_fast_mci.h,v $
 * Revision 3.1.2.1  1996/05/02  08:39:53  dwong
 * Branch: California_branch
 * Commit SMRP-7kFastSwitch, Load Balancing, and AURP Modularity
 *
 * Revision 3.1  1996/04/16  22:59:02  kleung
 * Placeholder for CA commit
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __AT_FAST_MCI_H__
#define __AT_FAST_MCI_H__


/*
 * The following "sizes" are the length of encapsulations and such measured 
 * in words (16-bit words). Where an odd number of bytes needs to be
 * represented, the byte-size is rounded up by one byte and divided by two.
 */
#define ISO2_ETHER_LENSIZE 1	/* Word size of ether len field (not in FDDI) */
#define UNALIGNED_OVERHEAD 1	/* For fudging the LLAP encaps field */

#define ISO2_OVERHEAD_SIZE 11   /* Size of the SNAP-SNAP ISO2 header */
#define FDDI_OVERHEAD_SIZE (ISO2_OVERHEAD_SIZE + UNALIGNED_OVERHEAD - ISO2_ETHER_LENSIZE)
#define ETALK_OVERHEAD_SIZE 8

/*
 * A note about this serial encapsulation overhead size:
 * This works fine for HDLC and Frame Relay, but SMDS and PPP will need
 * a bit more work in the determination of fast-switching offsets.
 */
#define SERIAL_OVERHEAD_SIZE 2

#define AT_ISO2_OFFSET		(MCI_ETHER_OFFSET + ISO2_OVERHEAD_SIZE)
#define AT_INPUT_SERIAL_OFFSET	(MCI_SERIAL_OFFSET + SERIAL_OVERHEAD_SIZE)
#define AT_FDDI_OFFSET		(FDDI_RECEIVE_OFFSET + FDDI_OVERHEAD_SIZE)
#define AT_ETALK_OFFSET		(MCI_ETHER_OFFSET + ETALK_OVERHEAD_SIZE)
#define AT_FR_IETF_OFFSET	(MCI_SERIAL_OFFSET + FRAME_RELAY_SNAPENCAPWORDS)
#define LEX_AT_ISO2_OFFSET	(LEX_RECEIVE_OFFSET + ISO2_OVERHEAD_SIZE)
#define LEX_AT_ETALK_OFFSET	(LEX_RECEIVE_OFFSET + ETALK_OVERHEAD_SIZE)


/*
 * Definitions for how we use the hwidb->sniff[] vector.
 *
 * The sniff vector should be thought of as merely someplace which is 
 * convenient and handy pre-allocated scratch memory for us to use
 * to hold the values we snort up from the MCI/cBus to discover the
 * hop-word and destination network/node address.
 *
 * The first three sniff longwords are used to hold information we
 * sniff out of the DDP frame, starting at the hopword.
 *
 * The fourth longword is used to hold the offset from the beginning
 * of the 100-byte encapsulation vector where the DDP frame actually
 * begins. We need to remember this for later use when we cross
 * controllers and need to compute where the new encapsulation will begin.
 */
#define	hop_len_word	sniff[SNIFFBASE+0].d.sword[0]
#define hopbyte		sniff[SNIFFBASE+0].d.byte[0]
#define checksum	sniff[SNIFFBASE+0].d.sword[1]
#define hop_and_cksum	sniff[SNIFFBASE+0].d.lword

#define	dst_net		sniff[SNIFFBASE+1].d.sword[0]
#define	src_net		sniff[SNIFFBASE+1].d.sword[1]
#define src_dst_net	sniff[SNIFFBASE+1].d.lword

#define dst_node	sniff[SNIFFBASE+2].d.byte[0]
#define src_node	sniff[SNIFFBASE+2].d.byte[1]
#define	dst_sock	sniff[SNIFFBASE+2].d.byte[2]
#define	src_sock	sniff[SNIFFBASE+2].d.byte[3]
#define nodes           sniff[SNIFFBASE+2].d.sword[0]
#define dst_src_sock    sniff[SNIFFBASE+2].d.sword[1]
#define nodes_and_socks sniff[SNIFFBASE+2].d.lword

/*
 * This is a private stash variable -- we don't fill this with data
 * from the incoming frame.
 */
#define ddp_start	sniff[SNIFFBASE+3].d.lword
#define	ddp_length	sniff[SNIFFBASE+4].d.sword[0]
#define input_unaligned	sniff[SNIFFBASE+4].d.byte[2]
#define trans_hdr_size	sniff[SNIFFBASE+4].d.byte[3]

extern ushort atalk_smds_pad;


/*
 * Forward declarations.
 */
extern inline void atalk_same_finish(hwidbtype*,  mciregtype*, hwidbtype*,
				     ushort);
extern inline void atalk_write_ether_header(mciregtype*, atcachetype*,
					    ushort, ushort);
extern inline void atalk_write_ether_snap_header(mciregtype* , atcachetype*,
						 ushort, ushort, ushort);
extern inline void atalk_write_fddi_header(mciregtype*, atcachetype*,
					   ushort, ushort );
extern inline void atalk_write_smds_trailer(mciregtype*, hwidbtype*);
extern inline void atalk_write_lane_snap_header(mciregtype* , atcachetype*,
						ushort , ushort , ushort);
extern inline void atalk_write_lex_snap_header(mciregtype* , atcachetype* ,
					       ushort , ushort , ushort);
extern inline void atalk_write_lex_arpa_header(mciregtype*, atcachetype*,
					       ushort, ushort);
extern inline void atalk_write_serial_header(mciregtype*, atcachetype*,
					     ushort, ushort);
extern inline void atalk_smds_setup_inline(hwidbtype*, hwidbtype*, mciregtype*,
					   atcachetype*, ushort);

#endif __AT_FAST_MCI_H__

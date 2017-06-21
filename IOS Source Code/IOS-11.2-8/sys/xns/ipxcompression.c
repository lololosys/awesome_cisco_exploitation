/* $Id: ipxcompression.c,v 3.4.18.3 1996/07/03 22:25:28 irfan Exp $
 * $Source: /release/112/cvs/Xsys/xns/ipxcompression.c,v $
 *------------------------------------------------------------------
 * IPX compression code for Telebit compression RFC1553
 *
 * Sept 1995,  Syed Irfan, (Joel Bion)
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ipxcompression.c,v $
 * Revision 3.4.18.3  1996/07/03  22:25:28  irfan
 * CSCdi58638:  ipx compression cipx not accepted under interface
 * group-async1
 * Branch: California_branch
 * IPX interfaces with "ppp-client" type configs don't have
 * primary_nidb but they do have owner_nidb. Use that.
 *
 * Revision 3.4.18.2  1996/06/03  20:23:42  john
 * CSCdi58748:  ipxwan and nlsp command nvgen order wrong and subsys issues
 * Branch: California_branch
 *
 * Revision 3.4.18.1  1996/03/18  22:52:43  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.12.3  1996/03/13  02:14:19  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.3.12.2  1996/03/07  11:21:35  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.12.1  1996/02/20  21:59:42  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.4  1996/02/08  20:15:43  thille
 * CSCdi48347: format string in inlines, code size impact
 * Pass 1 - Clean up some literal strings in inlines, duplicate literals,
 * etc.  11,356 bytes saved on gs7-j-m, 9336 bytes saved on igs-j-l
 *
 * Revision 3.3  1995/12/31  12:15:35  irfan
 * CSCdi46265:  crash when configuring ipx compression cipx
 * Clear ptrs for free'd memory blocks to avoid reuse.
 *
 * Revision 3.2  1995/11/17  19:22:01  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:58:17  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:38:35  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/09/21  08:18:22  irfan
 * First breath in the IOS world.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
/*
 * IPX COMPRESSION DATA/CODE OVERVIEW.
 *
 * All of this assumes you have fully familiarized yourself with RFC1553, and
 * understand its contents.
 *
 * Some interesting details of our implementation:
 *	1) Slot compression is NEVER enabled, because as a sender or
 *         receiver, we CANNOT account for all erroneous or discarded
 *	   packets. Slot compression can only be enabled if we CAN. 
 * 
 * IPX COMPRESSION DATA STRUCTURES
 *
 * Per NIDB -
 *  The essential idea to note is that we have to know:
 *	(a) what we will negotiate for compression when a link is brought up
 *	(b) what we finally negotiate with the other side; these are the
 *	    parameters we will actually use to maintain the session.
 *      (c) the state of current compression sessions. Each compression session
 *	    of packets we are sending to the remote router has its state
 *	    maintained in an 'outbound compression slot.' Similarly, each
 *          compression session of packets we are receiving from the remote
 *          router has its state maintained in an 'inbound compression slot.'
 *          For outbound sessions, we keep an ordered list of slots based on
 *          most recently used, with the head and tail pointers of that list
 *          maintained as entries in this table. To make the code a bit
 *          faster, we reference slots by offsets into a packed array, as
 *          opposed to keeping pointers to them.
 *          
 *	compression_enabled		- TRUE if compression enabled for this
 *					  nidb via user configuration.
 *      compression_negotiated_enabled  - Did we negotiate to have compression
 *					  enabled on this link?
 *      compression_slots		- # of compression slots we will offer
 *					  on connection startup.
 *      compression_negotiated_slots    - # of negotiated compression slots
 *      compression_negotiated_type     - Compression type negotiated. So far,
 *					  we offer only RFC1553.
 *      compression_negotiated_options  - Compression options negotiated. So
 *					  far, we offer none.
 *	inbound_compression_slots	- Points to an array of inbound
 *					  compression slots. (Packets from
 *					  them to us.)
 *	outbound_compression_slots	- Points to an array of outbound
 *					  compression slots. (Packet from us to
 *					  them.)
 *	compression_most_recently_used_slot
 *					- Of all of the compression slots,
 *					  this is the one most recently used.
 *	compression_least_recently_used_slot
 *					- Of all of the compression slots,
 *					  this is the one least recently used.
 *      compression_rejects_sent	- How many 'reject' frames have been
 *                                        sent out this interface?
 *      compression_uncompressed_bytes_out
 *                                      - Total # of bytes that would be sent
 *                                        if compression were disabled.
 *      compression_bytes_saved         - Total # of bytes avoided being sent
 *                                        because compression is enabled.
 *
 * STRUCTURE OF EACH COMPRESSION SLOT
 *	buffer_misses                   - # of 'getbuffers' that failed
 *      buffer_swaps                    - # of times a 'getbuffer' needed to
 *                                        be performed because a decompress
 *                                        needed more bytes than the current
 *                                        packet buffer of the compressed
 *                                        packet has available.
 *	slot				- The number of this slot.
 *	id				- The unique ID identifying this
 *					  instance of the slot. This is
 *					  incremented each time a new session
 *					  uses a slot.
 *	more_recently_used_slot		- Points to the slot number of the
 *					  next most recently used slot in the
 *					  list.
 *	less_recently_used_slot		- Points to the slot number of the
 *					  next less recently used slot in the
 *					  list.
 *	current_header			- A block containing a 'full' version
 *					  of the IPX{/NCP} header being used
 *					  for this session.
 *	flags				- Various flags of interest:
 *		IPX_COMPRESSION_FLAG_INUSE 		- Slot is in active use
 * 		IPX_COMPRESSION_FLAG_CONFIRMED		- For outgoing slots,
 *							  indicates that remote
 *							  side has confirmed
 *							  our Initial packet,
 *							  so we can send
 *							  compressed headers.
 *		IPX_COMPRESSION_FLAG_GIVES_NCP_COMPRESSION -
 *							  Indicates this slot
 *							  is performing not
 *							  only IPX header
 *							  compression, but NCP
 *							  header compression
 *							  as well.
 *
 * DATA FLOW OUTSIDE OF THIS MODULE:
 *	INBOUND PACKETS:
 *		in nov_input, call the IPX registry function:
 *		   pak = reg_invoke_ipx_compression_decompress_header(pak);
 *		which will return a decompressed packet (if a new one needed
 *		to be allocated to to fit the decompressed packet, it will
 *		be returned, and the current one will be discarded. If the
 *		old packet buffer fits the new decompressed packet, it will
 *		simply be overwritten in place, and header fields adjusted
 *		appropriately.
 *	OUTBOUND PACKETS:
 *		In holdq_dequeue, priority_dequeue, and custom_dequque,
 *		compress header with call to generic function to do the work
 *		for any linktype:
 *			pak = reg_invoke_compress_header(pak->linktype, pak);
 *
 *  CONTROL FLOW WITHIN THIS MODULE:
 *	INBOUND PACKETS:
 *		ipx_compression_decompress_header will examine the first
 *		byte of the packet, and if not entirely unprocessed (0xffff),
 *		will process it based on the first byte of the packet giving
 *		the flags that indicate what 'type' the compressed packet is.
 *		(COMPRESSED, REGULAR, CONFIRMED_INITIAL, CONFIRM, REJECT,
 *		UNCONFIRMED_INITIAL.) If the packet is not understood for any
 *		reason, a REJECT is sent.
 *	OUTBOUND PACKETS:
 *		ipx_compression_compress_header will first determine if any
 *		compression should be done on this packet. Compression should
 *		not be done if IPX is not enabled on the interface, or if
 *		compression has not been fully negotiated, or if the packet
 *		is an IPXWAN packet. If none of these are true, then the packet
 *		should be compressed, so the code will look up the current
 *		active slot for this packet, if any, and create one if one does
 *		not exist. Using this slot, it will compress this packet, and
 *		give it either a CONFIRMED_INITIAL, UNCONFIRMED_INITIAL, or a
 *		COMPRESSED type. A current slot may only be used if all of
 *		the source/dest net, host, and socket, and protocol types of
 *		the packet and the slot are the same. Furthermore, for NCP
 *		sessions, the NCP type and low and high connection numbers must
 *		be the same. If no active slots match this packet, any unused
 *		slot (or the least recently used active slot) is taken over and
 *		used for this packet. If a slot is 'new' in this manner, 
 *		CONFIRMED_INITIAL packets are sent until a CONFIRM is received
 *		from the remote side. If the NCP sequeunce number is not
 *		incrementing by one from the previous one sent, then it means
 *		a frame was lost somewhere, and a retransmission is occuring.
 *		If that is the case, send an UNCONFIRMED_INITIAL to reset to
 *		the new NCP sequeunce number.
 *
 * HOW A PROTOCOL LEARNS ABOUT COMPRESSION OPTIONS ON AN INTERFACE:
 *	When a protocol (such as IPXWAN or PPP) wants to negotiate
 *	IPX compression parameters, it should call:
 *		ipx_compression_options_provide
 *	to receive the options it should negotiate with the remote device.
 *
 *	When those options are negotiated, it should call:
 *		ipx_compression_options_receive
 *	to tell the compression code how the compression code should actually
 *	operate.
 *
 * 	If the link is being reset for any reason by a protocol, it should
 *	call:
 *		ipx_compression_reset
 *	to have IPX compression flush all of its current thoughts about the
 *	status of compression slots on the link.
 *
 *	A final function exists for anyone wanting it:
 *		ipx_compression_exists
 *	returns TRUE simply if the compression code is linked into the system.
 */
#include "master.h"
#include <ciscolib.h>
#include "sys_registry.h"
#include "interface_private.h"
#include "../if/network.h"
#include "xns.h"
#include "novell.h"
#include "config.h"
#include "../clns/clns_route.h"
#include "../clns/isis.h"
#include "../clns/isis_ipx.h"
#include "../xns/ipx_registry.h"
#include "subsys.h"
#include "../wan/serial.h"
#include "../xns/ipxcompression_chain.h"
#include "../ui/common_strings.h"

/* Debugging macros */

#define DEBUG_IPX_COMPRESSION_ANY \
  (novellcompression_debug || novellcompression_event_debug)

#define DEBUG_IPX_COMPRESSION_ACTIVITY (novellcompression_debug)

#define DEBUG_IPX_COMPRESSION_EVENT DEBUG_IPX_COMPRESSION_ANY

/*
 * ipx_compression_shipit
 * Take a 'new' packet built from another IPX compression routine, and ship
 * it out the NIDB specified. Return results of ultimate 'send' operation.
 *
 * INPUT ASSUMPTIONS:
 *	a) Packet goes to the broadcast address
 *	b) Packet's datagramsize = IPX portion of packet (no encapsulation)
 *	   so pak->datagramsize can be passed into nov_send1.
 */
static boolean ipx_compression_shipit (paktype *pak)
{
    return(nov_send1(pak, baddr, pak->datagramsize));
}


/*
 * ipx_compression_gimme_a_packet
 *
 * Create a packet to be sent out the interface specified for IPX compression
 * needs.
 */
static paktype *ipx_compression_gimme_a_packet (nidbtype *nidb, int len)
{
    paktype *pak;

    pak = getbuffer(len);
    if (pak == NULL)
      return(NULL);

    /* Any packet sent by this code is to be considered compressed! */
    pak->flags |= PAK_IPXCOMPRESS;
    pak->nif_output = nidb;
    pak->if_output = nidb->idb;
    pak->linktype = LINK_NOVELL;
    pak->datagramsize = len;
    return(pak);
}

/*
 * ipx_compression_send_reject
 *
 * Send a reject packet out of the NIDB specified, for the packet given.
 * TOSSES THE PACKET THAT IS PASSED IN!
 *
 * The first byte of the input_pak must be already cleared of flags that
 * are understood, so we build the reject frame correctly.
 *
 * This function returns NULL always to remind callers that the packet input
 * is always tossed by this routine.
 */
static paktype  *ipx_compression_send_reject (nidbtype *nidb, paktype *input_pak,
				       ipx_compressed_header *input_ich)
{
    paktype *pak;
    ipx_compressed_header *ich;

    if (DEBUG_IPX_COMPRESSION_ANY) {
	buginf("\nIPXCPRS: Sending reject out net %x, data: %d[%x], %d[%x]",
	       nidb->novellnet,
	       input_ich->data[IPX_COMPRESSION_REJECT_SLOT_NUMBER_OFFSET],
	       input_ich->data[IPX_COMPRESSION_REJECT_SLOT_NUMBER_OFFSET],
	       input_ich->data[IPX_COMPRESSION_REJECT_REJECTED_FLAGS_OFFSET],
	       input_ich->data[IPX_COMPRESSION_REJECT_REJECTED_FLAGS_OFFSET]);
    }
    nidb->compression_sent_cipx_reject++;
    pak = ipx_compression_gimme_a_packet(nidb, IPX_COMPRESSION_REJECT_SIZE);
    if (pak != NULL) {
	ich = (ipx_compressed_header *) ipxcompressionheadstart(pak);
	if (ich != NULL) {
	    ich->flags = IPX_COMPRESSION_PACKET_TYPE_REJECT;
	    if (input_ich != NULL) {
		ich->data[IPX_COMPRESSION_REJECT_SLOT_NUMBER_OFFSET] =
		  input_ich->data[IPX_COMPRESSION_REJECT_SLOT_NUMBER_OFFSET];
		ich->data[IPX_COMPRESSION_REJECT_REJECTED_FLAGS_OFFSET] =
		  input_ich->flags;
	    }
	}
    }
    datagram_done(input_pak);
    ipx_compression_shipit(pak);
    return(NULL);
}

/*
 * ipx_compression_send_confirm
 * Send a confirm out the NIDB specified, for the slot and ID given.
 */
static void ipx_compression_send_confirm (nidbtype *nidb,
				   ipx_ncp_compression_slot *slot)
{
    paktype *pak;
    ipx_compressed_header *ich;

    if (DEBUG_IPX_COMPRESSION_ANY) {
	buginf("\nIPXCPRS: Sending confirm out net %x, slot %d[%x], id %d[%x]",
	       nidb->novellnet, slot->slot, slot->slot, slot->id, slot->id);
    }
    pak = ipx_compression_gimme_a_packet(nidb, IPX_COMPRESSION_CONFIRM_SIZE);
    if (pak != NULL) {
	ich = (ipx_compressed_header *) ipxcompressionheadstart(pak);
	if (ich != NULL) {
	    ich->flags = IPX_COMPRESSION_PACKET_TYPE_CONFIRM;
	    if (slot != NULL) {
		nidb->compression_sent_cipx_confirm++;
		ich->data[IPX_COMPRESSION_CONFIRM_SLOT_NUMBER_OFFSET] =
		  slot->slot;
		ich->data[IPX_COMPRESSION_CONFIRM_ID_OFFSET] = slot->id;
	    }
	}
    }
    ipx_compression_shipit(pak);
}

/*
 * ipx_compression_cut_n_paste
 *
 * Given an existing packet, rip out 'bytes_to_cut' bytes starting at
 * 'cut_point'. If 'bytes_to_cut' is zero, cut nothing. Paste in
 * 'bytes_to_paste' bytes from 'paste_point' into location 'cut_point', if
 * 'bytes_to_paste' is non-zero, and then copy the rest of the original
 * packet (after cut_location), which consists of
 * 'bytes_after_cut_area_to_copy' bytes to copy.
 *
 * This routine is general enough to provide the necessary work for both
 * compression and decompression cases (I.e., bytes_to_cut is > bytes_to_paste,
 * which means compression, or bytes_to_cut < bytes_to_paste, which means
 * decompression.) Returns the new packet to use. The old will be discarded
 * if it is not the same.
 *
 * 'slot' will be passed in as NULL only when we want to get rid of the
 * annoying single byte header for 'uncompressed' packets.
 *
 * THIS ROUTINE IS THE FIRST ONE TO OPTIMIZE TO IMPROVE PERFORMANCE.
 * 
 */
static paktype *ipx_compression_cut_n_paste (paktype *pak,
				      ipx_ncp_compression_slot *slot,
				      uint bytes_to_cut, uchar *cut_point,
				      uint bytes_to_paste, uchar *paste_point)
{
    paktype *newpak;
    uint new_packet_length;
    int bytes_after_cut_area_to_copy;
    int cut_offset = cut_point - pak->datagramstart;
    uchar *src, *dst;
    
    new_packet_length =  pak->datagramsize - bytes_to_cut + bytes_to_paste;
    if  (pak->nif_output != NULL) {
	if (bytes_to_cut > bytes_to_paste) {
	    pak->nif_output->compression_bytes_saved += (bytes_to_cut -
							 bytes_to_paste);
	}
	pak->nif_output->compression_uncompressed_bytes_out +=
	    pak->datagramsize;
    }
    
    /* This is how many bytes are left in the frame after the cut point */
    bytes_after_cut_area_to_copy = pak->datagramsize - (cut_offset +
							bytes_to_cut);
    
    /* Check to see if a new buffer is needed, and get it if it is there */
    newpak = pak_grow(pak, pak->datagramsize, new_packet_length);
    if (newpak != pak) {
	datagram_done(pak);
	if (slot != NULL)
	  slot->buffer_swaps++;
	cut_point = newpak->datagramstart + cut_offset;
	if (newpak == NULL) {
	    if (slot != NULL)
		slot->buffer_misses++;
	    return(NULL);
	}
	/* Yup. It is to be considered compressed already. */
	newpak->flags |= PAK_IPXCOMPRESS;
	pak = newpak;
    }

    /*
     * Now we have a big enough packet to do whatever we want to with it,
     * and keep all of our data. The first task is to copy the data starting
     * at (cut_point + bytes_to_cut) to be at a location (bytes_to_paste)
     * after (cut_point). Then, copy in the paste area.
     */
    if (bytes_after_cut_area_to_copy > 0) {
	if (bytes_to_paste > bytes_to_cut) {
	    /*
	     * Will the result exist in memory following the existing location?
	     * If so, copy from the end backwards to the front.
	     */
	    for (src =
		 (cut_point + bytes_to_cut +
		  (bytes_after_cut_area_to_copy - 1)),
		 dst =
		 (cut_point + bytes_to_paste +
		  (bytes_after_cut_area_to_copy - 1));
		 (bytes_after_cut_area_to_copy > 0);
		 bytes_after_cut_area_to_copy--)
		*dst-- = *src--;
	} else {
	    /*
	     * Will the result exist in memory before the existing location?
	     * If so, copy from the front forwards to the back.
	     */
	    for (src = cut_point + bytes_to_cut, dst = cut_point + bytes_to_paste;
		 (bytes_after_cut_area_to_copy > 0);
		 bytes_after_cut_area_to_copy--)
		*dst++ = *src++;
	}
    }

    /*
     * This bcopy is safe because the paste point is never anywhere near
     * the cut point.
     */
    if (bytes_to_paste > 0) {
	bcopy(paste_point, cut_point, bytes_to_paste);
    }
    pak->datagramsize = new_packet_length;
    return(pak);
}

/*
 * ipx_compression_clear_slot
 * Clear fields in the slot given.
 */
static inline void ipx_compression_clear_slot (ipx_ncp_compression_slot *slot)
{
    slot->id = 0;
    slot->buffer_misses = 0;
    slot->buffer_swaps = 0;
    slot->flags = 0;
}

/*
 * ipx_compression_process_unconfirmed_intial
 * Process an 'unconfirmed inital' type packet. Returns the packet, if any,
 * to give back to the "novell_input" routine.
 */
static paktype *ipx_compression_process_unconfirmed_initial (nidbtype *nidb,
						      paktype *pak,
						      ipx_compressed_header
						      *ich)
{
    int slot_value;
    ipx_ncp_compression_slot *slot;
    ipxhdrtype *ipx;
    
    if (DEBUG_IPX_COMPRESSION_ANY) {
	buginf("\nIPXCPRS: Rcv UI from net %x, slot %d[%x], flags %d[%x]",
	       nidb->novellnet,
	       ich->data
	       [IPX_COMPRESSION_UNCONFIRMED_INITIAL_SLOT_NUMBER_OFFSET],
	       ich->data
	       [IPX_COMPRESSION_UNCONFIRMED_INITIAL_SLOT_NUMBER_OFFSET],
	       ich->flags, ich->flags);
    }
    nidb->compression_rx_cipx_uinitial++;
    if ((ich->flags & IPX_COMPRESSION_FLAGS_TYPE_SPECIFIC_FLAGS_MASK) !=
	IPX_COMPRESSION_FLAGS_UNCONFIRMED_INITIAL_MUST_BE_THIS) {
	pak = ipx_compression_send_reject(nidb, pak, ich);
    } else {
	slot_value = ich->data
	  [IPX_COMPRESSION_UNCONFIRMED_INITIAL_SLOT_NUMBER_OFFSET];
	if ((slot_value < nidb->compression_negotiated_slots) &&
	    (nidb->inbound_compression_slots != NULL)) {
	    slot = (ipx_ncp_compression_slot *)
	      &(nidb->inbound_compression_slots[slot_value]);
	    ipx_compression_clear_slot(slot);
	    pak = ipx_compression_cut_n_paste
	      (pak, NULL, IPX_COMPRESSION_UNCONFIRMED_INITIAL_SIZE,
	       (uchar *) ich, 0, NULL);
	    ipx = (ipxhdrtype *) ipxheadstart(pak);
	    bcopy(ipx, &(slot->current_header),
		  4);
	    if ((slot->current_header.pt == NOVELL_TYPE_NCP) &&
		((slot->current_header.ncp_type ==
		  IPX_NCP_TYPE_REQUEST_FROM_WORKSTATION) ||
		 (slot->current_header.ncp_type ==
		  IPX_NCP_TYPE_REPLY_FROM_SERVER))) {
		slot->flags |= IPX_COMPRESSION_FLAG_GIVES_NCP_COMPRESSION;
	    }
	} else {
	    pak = ipx_compression_send_reject(nidb, pak, ich);
	}
    }
    return(pak);
}


/*
 * ipx_compression_process_confirmed_initial
 * Process a 'confirmed initial' type packet. Returns the packet, if any,
 * to give back to the "novell_input" routine.
 */
static paktype *ipx_compression_process_confirmed_initial (nidbtype *nidb,
						    paktype *pak,
						    ipx_compressed_header *ich)
{
    int slot_value;
    int id_value;
    ipx_ncp_compression_slot *slot;
    ipxhdrtype *ipx;
    
    if (DEBUG_IPX_COMPRESSION_ANY) {
	buginf("\nIPXCPRS: Rcv CI from net %x, slot %d[%x], flags %d[%x]",
	       nidb->novellnet,
	       ich->data
	       [IPX_COMPRESSION_CONFIRMED_INITIAL_SLOT_NUMBER_OFFSET],
	       ich->data
	       [IPX_COMPRESSION_CONFIRMED_INITIAL_SLOT_NUMBER_OFFSET],
	       ich->flags, ich->flags);
    }
    nidb->compression_rx_cipx_cinitial++;
    if ((ich->flags & IPX_COMPRESSION_FLAGS_TYPE_SPECIFIC_FLAGS_MASK) !=
	IPX_COMPRESSION_FLAGS_CONFIRMED_INITIAL_MUST_BE_THIS) {
	pak = ipx_compression_send_reject(nidb, pak, ich);
    } else {
	slot_value = ich->data
	  [IPX_COMPRESSION_CONFIRMED_INITIAL_SLOT_NUMBER_OFFSET];
	id_value = ich->data[IPX_COMPRESSION_CONFIRMED_INITIAL_ID_OFFSET];
	if ((slot_value < nidb->compression_negotiated_slots) &&
	    (nidb->inbound_compression_slots != NULL)) {
	    slot = (ipx_ncp_compression_slot *)
	      &(nidb->inbound_compression_slots[slot_value]);
	    ipx_compression_clear_slot(slot);
	    slot->id = id_value;
	    slot->flags |= IPX_COMPRESSION_FLAG_INUSE;
	    pak = ipx_compression_cut_n_paste
	      (pak, NULL, IPX_COMPRESSION_CONFIRMED_INITIAL_SIZE,
	       (uchar *) ich, 0, NULL);
	    ipx = (ipxhdrtype *) ipxheadstart(pak);
	    bcopy(ipx, &(slot->current_header),
		  IPX_COMPRESSION_HEADER_STORAGE_SIZE);
	    if ((slot->current_header.pt == NOVELL_TYPE_NCP) &&
		((slot->current_header.ncp_type ==
		  IPX_NCP_TYPE_REQUEST_FROM_WORKSTATION) ||
		 (slot->current_header.ncp_type ==
		  IPX_NCP_TYPE_REPLY_FROM_SERVER))) {
		slot->flags |= IPX_COMPRESSION_FLAG_GIVES_NCP_COMPRESSION;
	    }
	    ipx_compression_send_confirm(nidb, slot); 
	} else {
	    pak = ipx_compression_send_reject(nidb, pak, ich);
	}
    }
    return(pak);
}

/*
 * ipx_compression_process_reject
 * Process a 'reject' type packet. Returns the packet, if any, to give back
 * to the "novell_input" routine.
 */
static paktype *ipx_compression_process_reject (nidbtype *nidb, paktype *pak,
					 ipx_compressed_header *ich)
{
    int slot_value;
    ipx_ncp_compression_slot *slot;
    
    if (DEBUG_IPX_COMPRESSION_ANY) {
	buginf("\nIPXCPRS: Rcv reject in net %x, data: %d[%x], %d[%x]",
	       nidb->novellnet,
	       ich->data[IPX_COMPRESSION_REJECT_SLOT_NUMBER_OFFSET],
	       ich->data[IPX_COMPRESSION_REJECT_SLOT_NUMBER_OFFSET],
	       ich->data[IPX_COMPRESSION_REJECT_REJECTED_FLAGS_OFFSET],
	       ich->data[IPX_COMPRESSION_REJECT_REJECTED_FLAGS_OFFSET]);
    }
    nidb->compression_rx_cipx_reject++;
    slot_value = ich->data[IPX_COMPRESSION_REJECT_SLOT_NUMBER_OFFSET];
    if ((slot_value < nidb->compression_negotiated_slots) &&
	(nidb->outbound_compression_slots != NULL)) {
	slot = (ipx_ncp_compression_slot *)
	  &(nidb->outbound_compression_slots[slot_value]);
	slot->flags &= ~(IPX_COMPRESSION_FLAG_CONFIRMED);
    }
    datagram_done(pak);
    return(NULL);
}

/*
 * ipx_compression_process_confirm
 * Process a 'confirm' type packet. Returns the packet, if any, to give back
 * to the "novell_input" routine.
 */
static paktype *ipx_compression_process_confirm (nidbtype *nidb, paktype *pak,
					  ipx_compressed_header *ich)
{
    int slot_value, id_value;
    ipx_ncp_compression_slot *slot;

    if (DEBUG_IPX_COMPRESSION_ANY) {
	buginf("\nIPXCPRS: Rcv Confirm in net %x, slot: %d[%x]",
	       nidb->novellnet,
	       ich->data[IPX_COMPRESSION_CONFIRM_SLOT_NUMBER_OFFSET],
	       ich->data[IPX_COMPRESSION_CONFIRM_SLOT_NUMBER_OFFSET]);
    }
    nidb->compression_rx_cipx_confirm++;
    if ((ich->flags & IPX_COMPRESSION_FLAGS_TYPE_SPECIFIC_FLAGS_MASK) !=
	IPX_COMPRESSION_FLAGS_CONFIRM_MUST_BE_THIS) {
	pak = ipx_compression_send_reject(nidb, pak, ich);
    } else {
	slot_value = ich->data[IPX_COMPRESSION_CONFIRM_SLOT_NUMBER_OFFSET];
	id_value = ich->data[IPX_COMPRESSION_CONFIRM_ID_OFFSET];
	if ((slot_value < nidb->compression_negotiated_slots) &&
	    (nidb->outbound_compression_slots != NULL)) {
	    slot = (ipx_ncp_compression_slot *)
	      &(nidb->outbound_compression_slots[slot_value]);
	    if ((slot->flags & IPX_COMPRESSION_FLAG_INUSE) &&
		(id_value == slot->id)) {
		slot->flags |= IPX_COMPRESSION_FLAG_CONFIRMED;
	    }
	}
    }
    if (pak != NULL)
      datagram_done(pak);
    return(NULL);
}

/*
 * ipx_compression_process_regular
 * Process a 'regular' type packet coming from the nidb given.
 * Returns the packet, if any, to give back to the "novell_input" routine.
 */
static paktype *ipx_compression_process_regular (nidbtype *nidb, paktype *pak,
					  ipx_compressed_header *ich)
{

    nidb->compression_rx_cipx_regular++;
    if ((ich->flags & IPX_COMPRESSION_FLAGS_TYPE_SPECIFIC_FLAGS_MASK) !=
	IPX_COMPRESSION_FLAGS_REGULAR_MUST_BE_THIS) {
	pak = ipx_compression_send_reject(nidb, pak, ich);
    } else {
	pak = ipx_compression_cut_n_paste(pak, NULL,
					  IPX_COMPRESSION_REGULAR_SIZE,
					  (uchar *) ich, 0, NULL);
    }
    return(pak);
}

/*
 * ipx_compression_process_compressed
 * Process a 'compressed' type packet coming from the nidb given.
 * Returns the packet, if any, to give back to the "novell_input" routine. 
 */
static paktype *ipx_compression_process_compressed (nidbtype *nidb, paktype *pak,
					     ipx_compressed_header *ich)
{
    int slot_value;
    ipx_ncp_compression_slot *slot;
    int data_offset;
    int paste_size;
    ipx_ncp_header *ipxncp;

    nidb->compression_rx_cipx_compressed++;
    slot = NULL;
    if ((nidb->inbound_compression_slots != NULL) &&
	(ich->flags & IPX_COMPRESSION_FLAGS_COMPRESSED_SLOT_INCLUDED)) {
	slot_value = ich->data[IPX_COMPRESSION_COMPRESSED_SLOT_NUMBER_OFFSET];
	if (slot_value < nidb->compression_negotiated_slots) {
	    slot = (ipx_ncp_compression_slot *)
	      &(nidb->inbound_compression_slots[slot_value]);
	}
    }

    ipxncp = (ipx_ncp_header *) ipxheadstart(pak);
    if ((ich->flags & ~IPX_COMPRESSION_FLAGS_UNDERSTOOD) || (ipxncp == NULL) ||
	(slot == NULL) || (!(slot->flags & IPX_COMPRESSION_FLAG_INUSE))) {
	ich->flags &= ~IPX_COMPRESSION_FLAGS_UNDERSTOOD;
	pak = ipx_compression_send_reject(nidb, pak, ich);
	if (slot != NULL) {
	    ipx_compression_clear_slot(slot);
	}
    } else {
	/* One past the slot, which is always included */
	data_offset = 1;
	/* Try checksum... */
	if (ich->flags & IPX_COMPRESSION_FLAGS_COMPRESSED_CHECKSUM_INCLUDED) {
	    slot->current_header.cksum =
	      ((ich->data[data_offset] << 8) || (ich->data[data_offset + 1]));
	    data_offset += 2;
	} else {
	    slot->current_header.cksum = NOVELL_NO_CHECKSUM;
	}
	/* Try length... */
	if (ich->flags & IPX_COMPRESSION_FLAGS_COMPRESSED_LENGTH_INCLUDED) {
	    if (ich->data[data_offset] < 128) {
		slot->current_header.len = ich->data[data_offset++];
	    } else if (ich->data[data_offset] < 192) {
		slot->current_header.len = ((ich->data[data_offset++] &
					     0x7f) << 8);
		slot->current_header.len += ich->data[data_offset++];
	    } else {
		data_offset++;
		slot->current_header.len = (ich->data[data_offset++] << 8);
		slot->current_header.len += ich->data[data_offset++];
	    }
	} else {
	    slot->current_header.len =
	      pak->datagramsize -
		(data_offset +
		 (ich->flags &
		  IPX_COMPRESSION_FLAGS_COMPRESSED_TASK_INCLUDED) ? 1 : 0);
	}
	/* Try task... */
	if (ich->flags & IPX_COMPRESSION_FLAGS_COMPRESSED_TASK_INCLUDED) {
	    slot->current_header.task_number = ich->data[data_offset++];
	}
	paste_size = XNSOVERHEADBYTES;
	if ((slot->current_header.pt == NOVELL_TYPE_NCP) &&
	    ((slot->current_header.ncp_type ==
	      IPX_NCP_TYPE_REQUEST_FROM_WORKSTATION) ||
	     (slot->current_header.ncp_type ==
	      IPX_NCP_TYPE_REPLY_FROM_SERVER))) {
	    paste_size += IPX_NCP_OVERHEADBYTES;
	    slot->current_header.sequence_number =
	      (slot->current_header.sequence_number + 1) % 256;
	}
	pak = ipx_compression_cut_n_paste(pak, slot, data_offset + 1,
					  (uchar *) ipxncp, paste_size,
					  (uchar *) &(slot->current_header));
    }
    return(pak);
}

/*
 * ipx_compression_decompress_header
 * Decompress an IPX header
 */
paktype *ipx_compression_decompress_header (paktype *pak)
{
    ipx_compressed_header *ich;
    nidbtype *nidb = pak->nif_input;
    vhdlctype *hdlc;

    /* Need to know about NIDB, and have a packet header! */
    ich = (ipx_compressed_header *) ipxcompressionheadstart(pak);
    if ((nidb == NULL) || (ich == NULL)) {
	datagram_done(pak);
    }

    /* Don't do anything if there is nothing to do... */
    if (!nidb->compression_negotiated_enabled)
      return(pak);

    /* Cope with HDLC 'make things even sizes' monstrosity */
    if (pak->enctype == ET_HDLC) {
	hdlc = (vhdlctype *) pak->mac_start;
	if (hdlc->var_hdlcflags & HDLC_PADDED) {
	    pak->datagramsize--;
	    /* Clear this bit, so noone else decrements like this! */
	    hdlc->var_hdlcflags &= ~(HDLC_PADDED);
	}
    }
    /* A compressed packet type is special */
    if (!(ich->flags & 0x1)) {
	pak = ipx_compression_process_compressed(nidb, pak, ich);
    } else {
	switch (ich->flags & IPX_COMPRESSION_FLAGS_PACKET_TYPE_MASK) {
	  case IPX_COMPRESSION_PACKET_TYPE_REGULAR:
	    pak = ipx_compression_process_regular(nidb, pak, ich);
	    break;
	  case IPX_COMPRESSION_PACKET_TYPE_CONFIRMED_INITIAL:
	    pak = ipx_compression_process_confirmed_initial(nidb, pak, ich);
	    break;
	  case IPX_COMPRESSION_PACKET_TYPE_CONFIRM:
	    pak = ipx_compression_process_confirm(nidb, pak, ich);
	    break;
	  case IPX_COMPRESSION_PACKET_TYPE_UNCONFIRMED_INITIAL:
	    pak = ipx_compression_process_unconfirmed_initial(nidb, pak, ich);
	    break;
	  case IPX_COMPRESSION_PACKET_TYPE_REJECT:
	    pak = ipx_compression_process_reject(nidb, pak, ich);
	    break;
	  default:
	    if (ich->flags != IPX_COMPRESSION_INDICATES_UNPROCESSED) {
		pak = ipx_compression_send_reject(nidb, pak, ich);
	    }
	    break;
	}
    }
    return(pak);
}

/*
 * ipx_compression_increment_slot
 * Increment the slot ID count on this slot, setup for new compression use
 */
static void ipx_compression_increment_slot (nidbtype *nidb,
				     ipx_ncp_compression_slot *slot,
				     boolean pak_needs_ncp_compression)
{
    slot->id = (slot->id + 1) % 256;
    slot->flags = IPX_COMPRESSION_FLAG_INUSE;
    if (pak_needs_ncp_compression) {
	slot->flags |= IPX_COMPRESSION_FLAG_GIVES_NCP_COMPRESSION;
    }
    slot->buffer_misses = slot->buffer_swaps = 0;
    if (DEBUG_IPX_COMPRESSION_ACTIVITY) {
	buginf("\nIPXCPRS: New outbound slot usage on net %x: slot %d, id %d, %s",
	       nidb->novellnet, slot->slot, slot->id,
	       (pak_needs_ncp_compression ? "IPX/NCP" : "IPX"));
    }
}

/*
 * ipx_compression_lookup_outbound_slot
 * Lookup the outbound slot corresponding to the source/dest pair given.
 * If no slot exists, take over the least recently used slot, causing it
 * to be renegotiated. Return the slot pointer. Will always return a slot
 * pointer. The data in the slot pointer will indicate if it needs to be
 * 'confirmed' or not. Returns NULL only on an error.
 */
static ipx_ncp_compression_slot *ipx_compression_lookup_outbound_slot (paktype *pak,
								nidbtype *nidb)
{
    ipx_ncp_header *ipxncp;
    ipx_ncp_compression_slot *slot;
    ipx_ncp_compression_slot *next_more_recent_slot;
    ipx_ncp_compression_slot *next_less_recent_slot;
    int current_slot, new_lru, new_mru;
    ulong pak_snet, pak_dnet;
    boolean pak_needs_ncp_compression;

    /* Do we have any outbound slots to examine? */
    if (nidb->outbound_compression_slots == NULL)
        return(NULL);
    ipxncp = (ipx_ncp_header *) ipxheadstart(pak);
    if (ipxncp == NULL)
        return(NULL);
    pak_snet = GET2WD(ipxncp->ssnet);
    pak_dnet = GET2WD(ipxncp->ddnet);

    pak_needs_ncp_compression =
      ((ipxncp->pt == NOVELL_TYPE_NCP) &&
       ((ipxncp->ncp_type == IPX_NCP_TYPE_REQUEST_FROM_WORKSTATION) ||
	(ipxncp->ncp_type == IPX_NCP_TYPE_REPLY_FROM_SERVER)));

    /* Find our slot... */
    slot = NULL;
    for (current_slot = nidb->compression_most_recently_used_slot;
	 (current_slot != IPX_COMPRESSION_SLOT_END_OF_CHAIN);
	 current_slot = slot->less_recently_used_slot) {
	slot = (ipx_ncp_compression_slot *)
	    &(nidb->outbound_compression_slots[current_slot]);
	/* If unused slot, ignore */
	if (!(slot->flags & IPX_COMPRESSION_FLAG_INUSE)) {
	    continue;
	}
	if ((pak_snet == GET2WD(slot->current_header.ssnet)) &&
	    (pak_dnet == GET2WD(slot->current_header.ddnet)) &&
	    (ipxncp->pt == slot->current_header.pt) &&
	    (ipxncp->ssock == slot->current_header.ssock) &&
	    (ipxncp->dsock == slot->current_header.dsock) &&
	    ieee_equal(ipxncp->shost, slot->current_header.shost) &&
	    ieee_equal(ipxncp->dhost, slot->current_header.dhost)) {
	    /* If we can ignore NCP stuff, great! */
	    if (!pak_needs_ncp_compression &&
		!(slot->flags & IPX_COMPRESSION_FLAG_GIVES_NCP_COMPRESSION))
	      break;
	    /* If either don't need NCP compression, we have no match... */
	    if ((!pak_needs_ncp_compression) ||
		(!(slot->flags &
		   IPX_COMPRESSION_FLAG_GIVES_NCP_COMPRESSION)))
	      continue;
	    /* We must pay attention to NCP connection and type numbers.. */
	    if ((ipxncp->connection_number_low_octet ==
		 slot->current_header.connection_number_low_octet) &&
		(ipxncp->connection_number_high_octet ==
		 slot->current_header.connection_number_high_octet) &&
		(ipxncp->ncp_type == slot->current_header.ncp_type)) {
		break;
	    }
	}
    }
    /* Use least recently used if none found. Setup new info in it */
    if (current_slot == IPX_COMPRESSION_SLOT_END_OF_CHAIN) {
	current_slot = nidb->compression_least_recently_used_slot;
	slot = (ipx_ncp_compression_slot *)
	  &(nidb->outbound_compression_slots[current_slot]);
	/* Make sure this is set to the current value... */
	if (slot == NULL) {
	    return(NULL);
	}
	ipx_compression_increment_slot(nidb, slot, pak_needs_ncp_compression);
    }
    /* Set this slot to most recently used if not already there... */
    if (nidb->compression_most_recently_used_slot != current_slot) {
	/* This is now the most recently used slot. Mark as such */
	new_mru = slot->slot;
	new_lru = nidb->compression_least_recently_used_slot;
	/*
	 * We have an old slot at 'slot' removed from the chain. Put
	 * it up at the front. This means noting where it is now,
	 * and setting up its current 'less recently used' slot, and
	 * its current 'more recently used slot'. Then, have these point
	 * at each other instead of though it. Then, put this slot back
	 * in the list at the front by having the current most recently
	 * used slot accept it as 'even more recently used' and have
	 * this current slot point to the current head as the next
	 * most recently used slot.
	 */
	if (slot->more_recently_used_slot !=
	    IPX_COMPRESSION_SLOT_END_OF_CHAIN) {
	    next_more_recent_slot = (ipx_ncp_compression_slot *)
	      &(nidb->outbound_compression_slots
		[slot->more_recently_used_slot]);
	    next_more_recent_slot->less_recently_used_slot =
	      slot->less_recently_used_slot;
	}
	if (slot->less_recently_used_slot !=
	    IPX_COMPRESSION_SLOT_END_OF_CHAIN) {
	    next_less_recent_slot = (ipx_ncp_compression_slot *)
	      &(nidb->outbound_compression_slots
		[slot->less_recently_used_slot]);
	    next_less_recent_slot->more_recently_used_slot =
	      slot->more_recently_used_slot;
	} else {
	    /* This was the LRU. Make next LRU slot the new LRU. */
	    new_lru = slot->more_recently_used_slot;
	}
	slot->more_recently_used_slot = IPX_COMPRESSION_SLOT_END_OF_CHAIN;
	slot->less_recently_used_slot =
	  nidb->compression_most_recently_used_slot;
	next_less_recent_slot = (ipx_ncp_compression_slot *)
	  &(nidb->outbound_compression_slots
	    [nidb->compression_most_recently_used_slot]);
	next_less_recent_slot->more_recently_used_slot = current_slot;
	nidb->compression_most_recently_used_slot = new_mru;
	nidb->compression_least_recently_used_slot = new_lru;
    }
    return(slot);
}


/*
 * ipx_compression_compress_header
 * Compress an IPX header
 */
static paktype *ipx_compression_compress_header (paktype *pak)
{
    paktype *newpak;
    ipx_compressed_header ich;
    ipx_ncp_header *ipxncp;
    ipx_ncp_compression_slot *slot;
    int data_offset;
    int paste_size;
    int cut_size;
    boolean send_full_header;
    
    /* Compressed already, or compression not enabled? */
    if ((pak->flags & PAK_IPXCOMPRESS) ||
	(pak->nif_output == NULL) ||
	(pak->nif_output->compression_negotiated_enabled == FALSE) ||
	NOVELL_NIDB_DO_NOT_COMPRESS_NOW(pak->nif_output)) {
	return(pak);
    }

    ipxncp = (ipx_ncp_header *) ipxheadstart(pak);
    if (ipxncp == NULL) {
	datagram_done(pak);
	return(NULL);
    }
    /* As a 'super sanity check', do not compress IPXWAN packets */
    if (ipxncp->dsock == NOVELL_SOCK_IPXWAN) {
	return(pak);
    }
    pak->flags |= PAK_IPXCOMPRESS;

    /*
     * If the packet is shared, then do not destroy the use by other people!
     * Instead, make a copy first.
     */
    if (pak->refcount > 1) {
	newpak = pak_duplicate(pak);
	if (newpak == NULL) {
	    datagram_done(pak);
	    return(NULL);
	}
/* JPBJPB: POssible cause of buffer in list problem... */
	datagram_done(pak);
	pak = newpak;
	/* Reset pointers */
	pak->flags |= PAK_IPXCOMPRESS;
	ipxncp = (ipx_ncp_header *) ipxheadstart(pak);
    }

    slot = ipx_compression_lookup_outbound_slot(pak, pak->nif_output);
    /* Mark the frame as having gone through compression */
    if (slot == NULL) {
	/* Revert to sending a 'regular' format frame */ 
	ich.flags = IPX_COMPRESSION_PACKET_TYPE_REGULAR;
	pak->nif_output->compression_sent_cipx_regular++;
	return(ipx_compression_cut_n_paste(pak, NULL, 0, (uchar *) ipxncp,
					   IPX_COMPRESSION_REGULAR_SIZE,
					   &(ich.flags)));
    }
    send_full_header = FALSE;

    /* We have a slot. If it is unconfirmed, send a confirm_initial */
    if (!(slot->flags & IPX_COMPRESSION_FLAG_CONFIRMED)) {
	pak->nif_output->compression_sent_cipx_cinitial++;
	ich.flags = IPX_COMPRESSION_PACKET_TYPE_CONFIRMED_INITIAL;
	ich.data[IPX_COMPRESSION_CONFIRMED_INITIAL_SLOT_NUMBER_OFFSET] =
	  slot->slot;
	ich.data[IPX_COMPRESSION_CONFIRMED_INITIAL_ID_OFFSET] = slot->id;
	paste_size = IPX_COMPRESSION_CONFIRMED_INITIAL_SIZE;
	send_full_header = TRUE;
	cut_size = 0;
    } else {
	cut_size = XNSOVERHEADBYTES;
	/* Include the flags word at least */
	paste_size = 1;
	/*
	 * Handle the discovery of lost NCP frames. Convert to 'unconfirmed'
	 * and a new ID number if that is the case. A lost frame is discovered
	 * when the sequence number is not strictly incrementing by one...
	 */
	if (slot->flags & IPX_COMPRESSION_FLAG_GIVES_NCP_COMPRESSION) {
	    cut_size += IPX_NCP_OVERHEADBYTES;
	    slot->current_header.sequence_number =
	      (slot->current_header.sequence_number + 1) % 256;
	    if (ipxncp->sequence_number !=
		slot->current_header.sequence_number) {
		send_full_header = TRUE;
		pak->nif_output->compression_sent_cipx_uinitial++;
		ich.flags = IPX_COMPRESSION_PACKET_TYPE_UNCONFIRMED_INITIAL;
		ich.data
		  [IPX_COMPRESSION_UNCONFIRMED_INITIAL_SLOT_NUMBER_OFFSET] =
		    slot->slot;
		paste_size = IPX_COMPRESSION_UNCONFIRMED_INITIAL_SIZE;
	    }
	}
    }
    if (send_full_header) {
	bcopy(ipxncp, &(slot->current_header),
	      IPX_COMPRESSION_HEADER_STORAGE_SIZE);
	return(ipx_compression_cut_n_paste
	       (pak, NULL, cut_size, (uchar *) ipxncp, paste_size,
		&(ich.flags)));
    }

    /* We have a slot, and it is confirmed. Send a compressed frame! */
    pak->nif_output->compression_sent_cipx_compressed++;
    ich.flags = IPX_COMPRESSION_PACKET_TYPE_COMPRESSED;
    data_offset = 0;

    /* Put in the slot number */
    ich.data[data_offset] = slot->slot;
    data_offset++;
    paste_size++;
    ich.flags |= IPX_COMPRESSION_FLAGS_COMPRESSED_SLOT_INCLUDED;

    /* Put in the checksum if needed */
    if (ipxncp->cksum != NOVELL_NO_CHECKSUM) {
	ich.data[data_offset++] = (ipxncp->cksum & 0xFF00) >> 8;
	ich.data[data_offset++] = (ipxncp->cksum & 0xFF);
	paste_size += 2;
	ich.flags |= IPX_COMPRESSION_FLAGS_COMPRESSED_CHECKSUM_INCLUDED;
    }

    /*
     * In OUR implementation, we don't trust the length to be from our MAC
     * header right now... our system constantly adds bytes to HDLC frames
     * to make them even aligned, etc. It is really ugly...
     */
    ich.flags |= IPX_COMPRESSION_FLAGS_COMPRESSED_LENGTH_INCLUDED;
    if (ipxncp->len <= IPX_COMPRESSION_ONE_BYTE_LENGTH_MAX) {
	ich.data[data_offset++] = (ipxncp->len & 0xFF);
	paste_size++;
    } else if (ipxncp->len <= IPX_COMPRESSION_TWO_BYTE_LENGTH_MAX) {
	ich.data[data_offset++] = (128 | ((ipxncp->len & 0xFF00) >> 8));
	ich.data[data_offset++] = (ipxncp->len & 0xFF);
	paste_size += 2;
    } else { /* Three bytes needed */
	ich.data[data_offset++] = 192;
	ich.data[data_offset++] = ((ipxncp->len & 0xFF00) >> 8);
	ich.data[data_offset++] = (ipxncp->len & 0xFF);
	paste_size += 3; 
    }

    /*
     * If the task number has changed, include that...
     */
    if (ipxncp->task_number != slot->current_header.task_number) {
	ich.data[data_offset++] = ipxncp->task_number;
	paste_size++;
	ich.flags |= IPX_COMPRESSION_FLAGS_COMPRESSED_TASK_INCLUDED;
    }
    
    /*
     * Copy in the new header. Who cares if we take extra stuff. The
     * flag 'IPX_COMPRESSION_FLAG_GIVES_NCP_COMPRESSION' lets us know
     * what is significant when we compare stuff on future compression
     * needs
     */
    bcopy(ipxncp, &(slot->current_header),
	  IPX_COMPRESSION_HEADER_STORAGE_SIZE);
    return(ipx_compression_cut_n_paste(pak, slot, cut_size, (uchar *) ipxncp,
				       paste_size, &(ich.flags)));
}

/*
 * ipx_compression_reset
 * Reset compression on the NIDB specified
 */
void ipx_compression_reset (nidbtype *nidb)
{
    ipx_ncp_compression_slot *slot;
    int i;
    
    if ((nidb == NULL) || (nidb->compression_enabled == FALSE) ||
	(!nidb->inbound_compression_slots) ||
	(!nidb->outbound_compression_slots))
	return;
    
    nidb->compression_negotiated_enabled = FALSE;
    nidb->compression_bytes_saved =
	nidb->compression_uncompressed_bytes_out =
	nidb->compression_sent_cipx_regular =
	nidb->compression_sent_cipx_compressed =
	nidb->compression_sent_cipx_cinitial =
	nidb->compression_sent_cipx_uinitial =
	nidb->compression_sent_cipx_confirm =
	nidb->compression_sent_cipx_reject =
	nidb->compression_rx_cipx_regular =
	nidb->compression_rx_cipx_compressed =
	nidb->compression_rx_cipx_cinitial =
	nidb->compression_rx_cipx_uinitial =
	nidb->compression_rx_cipx_confirm =
	nidb->compression_rx_cipx_reject = 0;


    for (i = 0; i < nidb->compression_negotiated_slots; i++) {
	/* Inbounds are under the control of the other end */
	slot = (ipx_ncp_compression_slot *)
	  &(nidb->inbound_compression_slots[i]);
	slot->slot = i;
	ipx_compression_clear_slot(slot);

	/* Outbounds have LRU chains  */
	slot = (ipx_ncp_compression_slot *)
	  &(nidb->outbound_compression_slots[i]);
	slot->slot = i;
	ipx_compression_clear_slot(slot);
	slot->more_recently_used_slot = i + 1;
	slot->less_recently_used_slot = i - 1;
    }
    /*
     * Fix up first and last pointers
     */
    slot = (ipx_ncp_compression_slot *) &(nidb->outbound_compression_slots[0]);
    slot->less_recently_used_slot = IPX_COMPRESSION_SLOT_END_OF_CHAIN;
    slot = (ipx_ncp_compression_slot *)
      &(nidb->outbound_compression_slots
	[nidb->compression_negotiated_slots - 1]);
    slot->more_recently_used_slot = IPX_COMPRESSION_SLOT_END_OF_CHAIN;
    nidb->compression_least_recently_used_slot = 0;
    nidb->compression_most_recently_used_slot =
      (nidb->compression_negotiated_slots - 1);
}

/*
 * ipx_compression_exists
 * Return TRUE, because we do exist
 */
boolean ipx_compression_exists (void)
{
    return(TRUE);
}

/*
 * ipx_compression_options_provide
 * Provide what we would negotiate for IPX compression options on the NIDB
 */
void ipx_compression_options_provide (nidbtype *nidb,
				      boolean *compression_enabled,
				      uchar *compression_type,
				      uchar *compression_options,
				      uchar *compression_slots)
{
    if ((compression_enabled == NULL) || (compression_slots == NULL) ||
	(compression_type == NULL) || (compression_options == NULL) ||
	(nidb == NULL)) {
	if (compression_enabled != NULL) {
	    *compression_enabled = FALSE;
	}
	return;
    }

    *compression_enabled = nidb->compression_enabled;
    *compression_slots = nidb->compression_slots;
    *compression_type = IPX_COMPRESSION_TYPE_TELEBIT;
    *compression_options = IPX_COMPRESSION_SUPPORTED_OPTIONS;
}

/*
 * ipx_compression_options_receive
 * Receive from the other side what was negotiated for IPX compression
 * options on the NIDB.
 */
void ipx_compression_options_receive (nidbtype *nidb,
				      boolean compression_enabled,
				      uchar compression_type,
				      uchar compression_options,
				      uchar compression_slots)
{
    if (nidb == NULL)
      return;

    nidb->compression_negotiated_slots = compression_slots;
    nidb->compression_negotiated_options = compression_options;
    nidb->compression_negotiated_type = compression_type;
    ipx_compression_reset(nidb);
    /* Because ipx_compression_reset clears this field... */
    nidb->compression_negotiated_enabled = compression_enabled;
}

/*
 * ipx_compression_if_command
 * Parse interface-specific IPX compression commands.
 */
void ipx_compression_if_command (parseinfo *csb)
{
    int slots = GETOBJ(int,1);
    nidbtype *nidb;
    hwidbtype *hwidb;
    idbtype *idb;

    if (!novell_running || novell_shutting_down) {
	if (!csb->nvgen) {
	    printf("\n%%Must give \"ipx routing\" command first");
	}
	return;
    }
    if (csb->nvgen) {
	nidb = IPX_CONFIG_NIDB(csb->interface);
	if (nidb == NULL)
	  return;
	nv_write(nidb->compression_enabled, "%s %d", csb->nv_command,
		 nidb->compression_slots);
	return;
    }
    idb = csb->interface;
    hwidb = hwidb_or_null(idb);
    if ((csb->interface == NULL) || (hwidb == NULL) ||
	((nidb = IPX_CONFIG_NIDB(csb->interface)) == NULL)) {
	if (csb->interface != NULL) {
	    buginf("\nIPX: No IPX network configured on interface %s",
		   csb->interface->namestring);
	}
	return;
    }
    if (csb->sense) {
	if ((ET_PPP != hwidb->enctype) && !nov_ipxwan_on_nidb(nidb)) {
	    buginf("\nIPX: IPXWAN or PPP must be configured on %s to"
		   " use compression", nidb->idb->namestring);
	    return;
	}
	if (hwidb->novell_routecache & NOVELL_RCACHE_CBUS) {
	    printf("\n%%IPX autonomous switching must be disabled\n       "
		   "Please configure NO IPX ROUTE-CACHE on %s",
		   hwidb->hw_namestring);
	    return;
	} else if (hwidb->novell_routecache & NOVELL_RCACHE) {
	    printf("\n%%IPX fast switching must be disabled\n       "
		   "Please configure NO IPX ROUTE-CACHE on %s",
		   hwidb->hw_namestring);
	    return;
	}
    }
    /* No change? */ 
    if ((csb->sense == nidb->compression_enabled) &&
	(slots == nidb->compression_slots)) {
	return;
    }
    novell_nidb_newstate(nidb, NOVELL_NIDB_DOWN);
    if (nidb->inbound_compression_slots != NULL)
        free(nidb->inbound_compression_slots);
    if (nidb->outbound_compression_slots != NULL)
        free(nidb->outbound_compression_slots);
    nidb->inbound_compression_slots = NULL;    /* Mark ptrs as invalid */
    nidb->outbound_compression_slots = NULL;   /* Mark ptrs as invalid */
    nidb->compression_enabled = FALSE;
    nidb->compression_negotiated_enabled = FALSE;
    if (csb->sense) {
	nidb->inbound_compression_slots =
	  malloc_named(sizeof(ipx_ncp_compression_slot) * slots,
		       "CIPX IN CMPX");
	if (nidb->inbound_compression_slots == NULL) {
	    printf("\n%%IPX: No memory to set up compression; try again later");
	    return;
	}
	nidb->outbound_compression_slots =
	  malloc_named(sizeof(ipx_ncp_compression_slot) * slots,
		       "CIPX OUT CMPX");
	if (nidb->outbound_compression_slots == NULL) {
	    printf("\n%%IPX: No memory to set up compression; try again later");
	    free(nidb->inbound_compression_slots);
	    return;
	}
	nidb->compression_enabled = TRUE;
	nidb->compression_slots = slots;
	/* Some value to start out with... */
	nidb->compression_negotiated_slots = slots;
	ipx_compression_reset(nidb);
    }
    novell_nidb_newstate(nidb, NOVELL_NIDB_UP);
}

/*
 * ipx_compression_show_idb
 * ipx_compression_show_idb_detail
 * Show compression information on an interface
 */
static void ipx_compression_show_idb (idbtype *idb)
{
    nidbtype *nidb;

    nidb = IPX_CONFIG_NIDB(idb);
    if ((nidb == NULL) || (nidb->compression_enabled == FALSE))
      return;

    printf("\n  IPX Compression information (Configured/Negotiated):");
    printf("\n   Enabled: %s/%s,  Slots: %d/%d, Type: %d/%d, Options: %d/%d",
	   nidb->compression_enabled ? "yes" : "no",
	   nidb->compression_negotiated_enabled ? "yes" : "no",
	   nidb->compression_slots, nidb->compression_negotiated_slots,
	   IPX_COMPRESSION_TYPE_TELEBIT, nidb->compression_negotiated_type,
	   IPX_COMPRESSION_SUPPORTED_OPTIONS,
	   nidb->compression_negotiated_options);
    if (nidb->compression_negotiated_enabled) {
	printf("\n   %d bytes sent as %d compressed bytes",
	       nidb->compression_uncompressed_bytes_out,
	       (nidb->compression_uncompressed_bytes_out -
		nidb->compression_bytes_saved));
	       
	printf("\n   CIPX sent cmprsd: %d,  regular: %d,  cinit: %d,"
	       "  uinit: %d,  confirm: %d,  reject: %d",
	       nidb->compression_sent_cipx_compressed,
	       nidb->compression_sent_cipx_regular,
	       nidb->compression_sent_cipx_cinitial,
	       nidb->compression_sent_cipx_uinitial,
	       nidb->compression_sent_cipx_confirm,
	       nidb->compression_sent_cipx_reject);

	printf("\n   CIPX rcvd cmprsd: %d,  regular: %d,  cinit: %d,"
	       "  uinit: %d,  confirm: %d,  reject: %d",
	       nidb->compression_rx_cipx_compressed,
	       nidb->compression_rx_cipx_regular,
	       nidb->compression_rx_cipx_cinitial,
	       nidb->compression_rx_cipx_uinitial,
	       nidb->compression_rx_cipx_confirm,
	       nidb->compression_rx_cipx_reject);

	printf("\n   Most recently used slot: %d  Least recently used slot: %d",
	       nidb->compression_most_recently_used_slot,
	       nidb->compression_least_recently_used_slot);
    }
}

static void ipx_compression_show_idb_header (idbtype *idb)
{
    printf(if_is_line_proto_is_msg, idb->namestring,
          print_idbstate(idb), interface_up(idb) ? "up" : "down");
}

static void ipx_compression_print_slot_details (nidbtype *nidb,
					 ipx_ncp_compression_slot *slot)
{
    printf("\n    Slot number %d, ID %d", slot->slot, slot->id);
    if (slot->flags & IPX_COMPRESSION_FLAG_CONFIRMED)
      printf(", confirmed");
    if (slot->flags & IPX_COMPRESSION_FLAG_INUSE)
      printf(", inuse");
    if (slot->flags & IPX_COMPRESSION_FLAG_GIVES_NCP_COMPRESSION)
      printf(", ncp");
    printf("\n     Source: %x.%e(%d[%x])\n     Dest: %x.%e(%d[%x]), pt=%d[%x]",
	   GET2WD(slot->current_header.ssnet), slot->current_header.shost,
	   slot->current_header.ssock, slot->current_header.ssock,
	   GET2WD(slot->current_header.ddnet), slot->current_header.dhost,
	   slot->current_header.dsock, slot->current_header.dsock,
	   slot->current_header.pt, slot->current_header.pt);
    if (slot->flags & IPX_COMPRESSION_FLAG_GIVES_NCP_COMPRESSION) {
	printf("\n     NCP: seq %d[%x], task %d[%x] connection %d[%x]",
	       slot->current_header.sequence_number,
	       slot->current_header.sequence_number,
	       slot->current_header.task_number,
	       slot->current_header.task_number,
	       IPX_COMPRESSION_NCP_CONNECTION_NUMBER_FROM_HEADER((ipx_ncp_header *) (&(slot->current_header))),
	       IPX_COMPRESSION_NCP_CONNECTION_NUMBER_FROM_HEADER((ipx_ncp_header *) (&(slot->current_header))));
    }
    printf("\n     Buffer information: misses %d, swaps %d", slot->buffer_misses,
	   slot->buffer_swaps);
}

static void ipx_compression_show_idb_always_summary (idbtype *idb)
{
    ipx_compression_show_idb_header(idb);
    ipx_compression_show_idb(idb);
}

static void ipx_copmression_show_idb_cond_summary (idbtype *idb)
{
    nidbtype *nidb;

    nidb = IPX_CONFIG_NIDB(idb);
    if ((nidb == NULL) || (nidb->compression_enabled == FALSE))
      return;
    ipx_compression_show_idb_always_summary(idb);
}

static void ipx_compression_show_idb_always_detail (idbtype *idb)
{
    int i;
    nidbtype *nidb;
    ipx_ncp_compression_slot *slot;
    boolean did_header;

    ipx_compression_show_idb_always_summary(idb);
    nidb = IPX_CONFIG_NIDB(idb);
    if (nidb == NULL)
      return;
    for (i = 0, did_header = FALSE;
	 i < nidb->compression_negotiated_slots;
	 i++) {
	slot = (ipx_ncp_compression_slot *)
	  &(nidb->inbound_compression_slots[i]);
	if (slot->flags & IPX_COMPRESSION_FLAG_INUSE) {
	    if (did_header == FALSE) {
		did_header = TRUE;
		printf("\n  Active inbound compression slots:");
	    }
	    ipx_compression_print_slot_details(nidb, slot);
	}
    }
    for (i = 0, did_header = FALSE;
	 i < nidb->compression_negotiated_slots;
	 i++) {
	slot = (ipx_ncp_compression_slot *)
	  &(nidb->outbound_compression_slots[i]);
	if (slot->flags & IPX_COMPRESSION_FLAG_INUSE) {
	    if (did_header == FALSE) {
		did_header = TRUE;
		printf("\n  Active outbound compression slots:");
	    }
	    ipx_compression_print_slot_details(nidb, slot);
	}
    }
}

static void ipx_copmression_show_idb_cond_detail (idbtype *idb)
{
    nidbtype *nidb;

    nidb = IPX_CONFIG_NIDB(idb);
    if ((nidb == NULL) || (nidb->compression_enabled == FALSE))
      return;
    ipx_compression_show_idb_always_detail(idb);
}

/*
 * ipx_compression_show_command
 * Parse IPX compression show commands.
 */
void ipx_compression_show_command (parseinfo *csb)
{
    automore_enable(NULL);
    show_swidb_chain(GETOBJ(idb,1),
		     GETOBJ(int, 1) ?
		     (GETOBJ(idb, 1) ?
		      ipx_compression_show_idb_always_detail :
		      ipx_copmression_show_idb_cond_detail) :
		     (GETOBJ(idb, 1) ?
		      ipx_compression_show_idb_always_summary :
		      ipx_copmression_show_idb_cond_summary), 0);
    automore_disable();
}

/*
 * ipx_compression_show_interface
 * Show IPX compression specific information for 'show ipx interface'
 */
static void ipx_compression_show_interface (idbtype *idb)
{
    ipx_compression_show_idb(idb);
}

/*
 * Initialize IPX compression
 */
static void ipx_compression_init (subsystype *subsys)
{
    ipx_compression_parser_init();
    reg_add_compress_header(LINK_NOVELL, ipx_compression_compress_header,
			    "ipx_compression_compress_header");
    reg_add_ipx_compression_decompress_header
      (ipx_compression_decompress_header, "ipx_compression_decompress_header");
    reg_add_ipx_compression_options_provide(ipx_compression_options_provide,
					    "ipx_compression_options_provide");
    reg_add_ipx_compression_options_receive(ipx_compression_options_receive,
					    "ipx_compression_options_receive");
    reg_add_ipx_compression_reset(ipx_compression_reset,
				  "ipx_compression_reset");
    reg_add_ipx_compression_exists(ipx_compression_exists,
				   "ipx_compression_exists");
    reg_add_ipx_show_interface(ipx_compression_show_interface,
			       "ipx_compression_show_interface");
}

/*
 * The IPX_COMPRESSION subsystem header.
 */
#define IPX_COMPRESSION_MAJVERSION 1
#define IPX_COMPRESSION_MINVERSION 0
#define IPX_COMPRESSION_EDITVERSION  1

SUBSYS_HEADER(ipxcompression,
	      IPX_COMPRESSION_MAJVERSION,
	      IPX_COMPRESSION_MINVERSION,
	      IPX_COMPRESSION_EDITVERSION,
	      ipx_compression_init, SUBSYS_CLASS_PROTOCOL,
 	      "seq: ip, ipx, ipxwan",
	      "req: ipx");

/* $Id: vines_srtp.h,v 3.5 1996/03/05 22:09:05 ehk Exp $
 * $Source: /release/111/cvs/Xsys/vines/vines_srtp.h,v $
 *------------------------------------------------------------------
 * vines_srtp.h -- Vines Sequenced RTP Support
 *
 * June 1993, David R. Hampton
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: vines_srtp.h,v $
 * Revision 3.5  1996/03/05  22:09:05  ehk
 * CSCdi50536:  Corrupted Vines SRTP redirects and invalid packet length
 * field
 * corrected SRTP redirect packet size and set the destination host id.
 *
 * Revision 3.4  1996/02/08  20:14:18  thille
 * CSCdi48347: format string in inlines, code size impact
 * Pass 1 - Clean up some literal strings in inlines, duplicate literals,
 * etc.  11,356 bytes saved on gs7-j-m, 9336 bytes saved on igs-j-l
 *
 * Revision 3.3  1995/11/28  23:40:26  slin
 * CSCdi44517:  Vines:Incomplete Answer to a Vines Client SRTP Request
 * - Off by one error in calculating number of entries in request
 *
 * Revision 3.2  1995/11/17  17:58:49  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:45:21  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:15:16  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __VINES_SRTP_H__
#define __VINES_SRTP_H__


/**********************************************************************
 *
 *			     VINES 5.50
 *			 PACKET DATA FORMATS
 *
 **********************************************************************/

/*
 * Definitions for the common SRTP header
 */
struct vsrtp_header_ {
    ushort version;
    uchar  operation;
    uchar  node_type;
    uchar  flags;
    uchar  reserved;
    uchar  data[0];
};

#define VSRTP_HEADER_BYTES 	6	/* Damn compilers!!! */

#define VRTP_VERSION		0x0000
#define VSRTP_VERSION		0x0001
#define VSRTP_VERSION_THRESH	0x00FF

#define VSRTP_REQUEST		0x01
#define VSRTP_UPDATE		0x02
#define VSRTP_REDIRECT		0x03
#define VSRTP_REINIT		0x04
#define VSRTP_LAST		0x05

/*
 * The following two definitions are the same as the definitions for
 * the VINES RTP protocol.  The code utiliizes this knowledge to
 * perform as single test on the neighbor node type, instead of a
 * specific test for each version.  If these definitions change, the
 * code must be searched for all of these tests.
 */
#define VSRTP_NODE_CLIENT	0x01
#define VSRTP_NODE_ROUTER	0x02

#define VSRTP_FLAG_MISMATCH	0x01
#define VSRTP_FLAG_OLD_PRESENT	0x02
#define VSRTP_FLAG_AUTOMATIC	0x04

/*
 * Definitions for a SRTP `request' message
 */
struct vsrtp_request_ {
    uchar  type;
    uchar  reserved;
    ulong  PACKED(data[1]);
};

#define VSRTP_REQUEST_BYTES 	6	/* Damn compilers!!! */
#define VSRTP_REQUEST_HEADER_BYTES 2

typedef enum vrtp5_request_infotype_ {
    VSRTP_REQ_SPECIFIC = 0x01,
    VSRTP_REQ_CHANGES  = 0x02,
    VSRTP_REQ_FULL     = 0x03,
    VSRTP_REQ_RESPONSE = 0x04,
    VSRTP_REQ_LAST     = VSRTP_REQ_RESPONSE,
    VSRTP_REQ_INVALID  = 0x05
} vrtp5_request_infotype;

/*
 * Definitions for a SRTP `update' message
 */
struct vsrtp_update_ {
    uchar  type;
    uchar  flags;
    ushort packet_id;
    ushort data_offset;			/* in tuples */
    ulong  PACKED(sequence);
    ushort metric;
    uchar  data[0];
};

#define VSRTP_UPDATE_BYTES 	12	/* Damn compilers!!! */

enum vsrtp_update_infotype {
    VSRTP_UPD_UPDATE = 0,
    VSRTP_UPD_RESPONSE = 1,
    VSRTP_UPD_LAST = VSRTP_UPD_RESPONSE
};

#define VSRTP_UPD_FLAG_NONE	0x00
#define VSRTP_UPD_FLAG_NULL	0x00
#define VSRTP_UPD_FLAG_BOM	0x01
#define VSRTP_UPD_FLAG_EOM	0x02
#define VSRTP_UPD_FLAG_BOMEOM	0x03
#define VSRTP_UPD_FLAG_CHANGE	0x04
#define VSRTP_UPD_FLAG_FULL	0x08
#define VSRTP_UPD_FLAG_SYNC	0x10

#define VSRTP_SYNC_INTERVAL	(12 * ONEHOUR)

enum vsrtp_update_type {
    VSRTP_NULL_UPDATE    = 0,
    VSRTP_CHANGE_UPDATE  = 1,
    VSRTP_FULL_UPDATE    = 2,
    VSRTP_SYNC_UPDATE    = 3,
    VSRTP_INVALID_UPDATE = 4
};

typedef struct vsrtp_entry_ {
    ulong  net;
    ushort metric;
    ulong  PACKED(sequence);
    uchar  flags;
    uchar  reserved;
} vsrtp_entry;

#define VSRTP_ENTRY_BYTES 	12	/* Damn compilers!!! */

#define VSRTP_ENTRY_NONE	0x00
#define VSRTP_ENTRY_LAN		0x01
#define VSRTP_ENTRY_WAN		0x02
#define VSRTP_ENTRY_NON_VINES	0x04
#define VSRTP_ENTRY_NO_SEQ	0x08
#define VSRTP_ENTRY_INVALID	0x80

#define VSRTP_UPD_FRAGENTRIES	120
#define VSRTP_UPD_FRAGBYTES	(VSRTP_ENTRY_BYTES * VSRTP_UPD_FRAGENTRIES)
#define VSRTP_UPD_TOTALBYTES	(VSRTP_ENTRY_BYTES * vines_network_tree->activeNodes)

/*
 * Definitions for a SRTP `redirect' message
 */
typedef struct vsrtp_redirect_ {
    uchar  nbr_macaddrlen;
    uchar  nbr_riflen;
    uchar  dst_net[V_NETBYTES];
    ushort dst_host;
    ushort dst_metric;
    uchar  dst_node;
    uchar  dst_flags;
    uchar  dst_sequence[4];
    uchar  nbr_net[V_NETBYTES];
    ushort nbr_host;
    ushort nbr_metric;
    uchar  nbr_node;
    uchar  nbr_flags;
    uchar  nbr_sequence[4];
    uchar  nbr_mac[IEEEBYTES];
    uchar  nbr_rif[0];
} vsrtp_redirect;
#define VSRTP_REDIRECT_BYTES 	36	/* Damn compilers!!! */

/*
 * General constants used by the SRTP code.
 */
#define VSRTP_REQUEST_COUNT 	4
#define VSRTP_REQUEST_TIME  	(30 * ONESEC)
#define VSRTP_REQUEST_SLEEP  	(30 * ONESEC)

#define VSRTP_REINIT_CHECK	( 3 * ONEMIN)	/* When to check for isolation */
/* This constant is used for the initial check after enabling routing
   or SRTP support.  Twice the constant is used for any subsequent
   sequence number checking */
#define VSRTP_REINIT_PAUSE	(180* ONESEC)	/* How long to pause updates */
/* Since the next update is just about to happen, any time interval of
   [1-90] will skip one update [91-180] will skip two updates, etc. */

#define VSRTP_REASSEMBLY_TIME   ( 10 * ONESEC)

enum vsrtp_seq_comp {
    LESS = 0,
    EQUAL = 1,
    ONEMORE = 2,
    GREATER = 3
};

typedef enum vsrtp_reass_type_ {
    VSRTP_REASS_FAILED,
    VSRTP_REASS_IGNORED,
    VSRTP_REASS_DUPLICATE,
    VSRTP_REASS_ENQUEUED,
    VSRTP_REASS_DONE
} vsrtp_reass_type;



/**********************************************************************
 *
 *		     VINES SRTP Metric Manipulation
 *
 **********************************************************************/

/*
 * vmetric_to_srtp
 *
 * Convert a metric from its internal form to SRTP form.
 */
static inline ushort vmetric_to_srtp (ushort metric)
{
    return(((metric & 0xFFF0) >> 4) | ((metric & 0x000F) << 12));
}

/*
 * vmetric_from_srtp
 *
 * Convert a metric from SRTP form to internal form.
 */
static inline ushort vmetric_from_srtp (ushort metric)
{
    return(((metric & 0x0FFF) << 4) | ((metric & 0xF000) >> 12));
}


/**********************************************************************
 *
 *			       INLINES
 *
 **********************************************************************/

/*
 * vrtp_compare_seqno
 *
 * Compare two vines sequence numbers.
 */
static inline enum vsrtp_seq_comp vsrtp_compare_seqno (ulong current, ulong new)
{
    long temp;

    if (current == 0)
	return(GREATER);

    temp = (long)new - (long)current;
    if (temp == 0)
	return(EQUAL);
    if (temp == 1)
	return(ONEMORE);
    if (temp > 1)
	return(GREATER);
    return(LESS);
}

/*
 * vrtp_compare_seqno
 *
 * Compare three vines sequence numbers.
 */
static inline enum vsrtp_seq_comp vsrtp_compare_3seqno (
    ulong low,
    ulong high,
    ulong new)
{
    long temp;

    if ((low == 0) || (high == 0))
	return(EQUAL);

    temp = (long)new - (long)low;
    if (temp < 0)
	return(LESS);
    
    temp = (long)new - (long)high;
    if (temp > 0)
	return(GREATER);
    return(EQUAL);
}

/*
 * vsrtp_getbuffer
 *
 * Allocate a packet buffer and set it up for use by the SRTP code.
 */
static inline paktype *vsrtp_getbuffer (char *name, int length)
{
    paktype *pak;

    pak = vip_getbuffer(VSRTP_HEADER_BYTES + length);
    if (pak) {
	pak->session_start = pak->transport_start + VSRTP_HEADER_BYTES;
	pak->datagramsize = length;
    } else {
	if (do_vines_rtp_debug(vines_network, vines_host))
	    buginf(vsrtp_no_packet_msg, name);
	vroute_no_buffer_out++;
    }
    return(pak);
}



/**********************************************************************
 *
 *			      EXTERNS
 *
 **********************************************************************/
extern ulong vsrtp_rx_response_count;
extern ulong vsrtp_rx_request_lookup_count;
extern ulong vsrtp_rx_request_slr_count;
extern ulong vsrtp_rx_resync_count;
extern ulong vsrtp_rx_frag_rcvd;
extern ulong vsrtp_rx_frag_drop_dup;
extern ulong vsrtp_rx_frag_drop_oos;
extern ulong vsrtp_rx_frag_drop_old;
extern ulong vsrtp_reass_complete;
extern ulong vsrtp_reass_failed;
extern ulong vsrtp_rx_outdated;
extern ulong vsrtp_rx_oldnetinfo;

extern ulong vsrtp_tx_response_count;
extern ulong vsrtp_tx_request_lookup_count;
extern ulong vsrtp_tx_request_slr_count;
extern ulong vsrtp_tx_resync_count;
extern ulong vsrtp_tx_frag_sent;
extern ulong vsrtp_tx_frag_drop;
extern ulong vsrtp_frag_complete;
extern ulong vsrtp_frag_failed;


/**********************************************************************
 *
 *			      PROTOTYPES
 *
 **********************************************************************/

/* vines_srtp.c */
extern void vsrtp_init(void);
extern void vsrtp_start(void);
extern void vsrtp_schedule_start(void);
extern void vsrtp_stop(void);
extern boolean vsrtp_send_broadcast(paktype *, enum vsrtp_update_infotype, vinesidbtype *);
extern vinesidbtype *vsrtp_send_specific(paktype *, int, vinespathtype *);
extern void vsrtp_retry_timer_expired(vinespathtype *);
extern void vsrtp_reassembly_timer_expired(vinespathtype *);
extern boolean vsrtp_fragment_update(queuetype *, paktype *, enum vsrtp_update_infotype, ushort, uchar);
extern void vsrtp_send_fragments(queuetype *, vinespathtype *, vinesidbtype *, boolean);
extern paktype *vsrtp_reassemble1(vinespathtype *, paktype *, vsrtp_update *, vsrtp_reass_type *);
extern paktype *vsrtp_reassemble(vinespathtype *, paktype *, vsrtp_header *, vsrtp_reass_type *);
extern paktype *vsrtp_generate_update(enum vsrtp_update_type, char *, int, int, vinesidbtype *);
extern void vsrtp_display_global(void);
extern void vsrtp_display_interface(vinesidbtype *);
extern void vsrtp_display_neighbor(vinesneighbortype *);
extern void vsrtp_display_timer(void);
extern uchar *vsrtp_decode_packet(uchar *, int *, void (*fn )(const char *, ...));
extern boolean vsrtp_update_route(ulong, ushort, ulong, uchar, vinespathtype *);
extern void vsrtp_send_full_request(vinesidbtype *, vinespathtype *);
extern void vsrtp_send_update(enum vsrtp_update_type, vinesidbtype *, vinespathtype *);
extern void vsrtp_send_redirect(paktype *, routingdecision *);
extern void vsrtp_send_reinit(vinesidbtype *);
extern void vsrtp_send_periodic(vinesidbtype *);
extern void vsrtp_process_message(paktype *);
extern boolean vines_vsrtp_enabled(vinesidbtype *);

#endif /* __VINES_SRTP_H__ */

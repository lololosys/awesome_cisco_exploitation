/* $Id: rsvp_database.h,v 1.1.4.8 1996/08/24 01:16:41 fred Exp $
 * $Source: /release/112/cvs/Xsys/rsvp/rsvp_database.h,v $
 *------------------------------------------------------------------
 * Resource Reservation Protocol
 *
 * April 1996, Fred Baker
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rsvp_database.h,v $
 * Revision 1.1.4.8  1996/08/24  01:16:41  fred
 * CSCdi67062:  Resolution of interoperability testing with ISI 4.0a6 code
 * Branch: California_branch
 *
 * Revision 1.1.4.7  1996/08/20  10:24:24  fred
 * CSCdi66538:  implement integrated services objects (RSVP)
 * Branch: California_branch
 *
 * Revision 1.1.4.6  1996/08/09  00:18:03  asastry
 * CSCdi65504:  Allow <debug ip rsvp detail> command to be RESV-only or
 * PATH-only
 * Branch: California_branch
 *
 * Revision 1.1.4.5  1996/06/21  19:05:23  fred
 * CSCdi57507:  RSVP gets installed with Conversation number 0 on ethernet.
 *         Install registry callback from WFQ setup to RSVP to make sure
 *         that some reservable queues are allocated. Also, do not display
 *         weights and conversations unless WFQ invoked on interface
 * Branch: California_branch
 *
 * Revision 1.1.4.4  1996/06/18  11:05:43  fred
 * CSCdi53943:  box crashes when removing RSVP from an interface
 *              Add defensive code to avoid null dereferences, and
 *              improve handling of interface and topology changes
 * Branch: California_branch
 *
 * Revision 1.1.4.3  1996/06/18  08:37:12  fred
 * CSCdi60599:  RSVP interaction with PIM Sparse Mode has problems.
 *              Implement special code required for sparse mode route
 *              creation
 * Branch: California_branch
 *
 * Revision 1.1.4.2  1996/06/05  21:11:43  ronnie
 * CSCdi39527:  wavl_walk calls avl_walk with wrong parameter
 * Branch: California_branch
 *
 * Revision 1.1.4.1  1996/04/17  13:57:17  fred
 * CSCdi54910:  commit rsvp, traffic shaping, and RED
 * Branch: California_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __RSVP_DATABASE_H__
#define __RSVP_DATABASE_H__

#include "mgd_timers.h"
#include "../util/wavl.h"

/***********************************************************************

 * Storage Data Structures
 *
 ***********************************************************************/

enum rsvp_timer_types {
    TIMER_TYPE_REFRESH_PATH,
    TIMER_TYPE_REFRESH_RESERVATION,
    TIMER_TYPE_STATIC_PATH,
    TIMER_TYPE_STATIC_RESV,
    TIMER_TYPE_CLEANUP,
    TIMER_TYPE_PATH_POLL
};

enum rsvp_search_result {
    RSVP_FOUND, RSVP_NOT_FOUND, RSVP_NO_MEMORY
};

STRUCTURE_REF(rsvp_neighbor_type);
struct rsvp_neighbor_type_ {
    rsvp_neighbor_type *next;		/* chain pointer */
    ipaddrtype neighbor;		/* ip address of neighbor */
    uchar ip_protocol;			/* IP Protocol he's using or zero */
};

/*
 * pointed to by hwidbtype
 */
STRUCTURE_REF(rsvp_flow_ipv4);
struct rsvp_flow_ipv4_ {
    rsvp_flow_ipv4 *next;		/* chain pointer */
    enum qos_service_type qos_service;	/* type of QOS service */
    sys_timestamp last_event;		/* time of last token bucket */
    long bytes_this_interval;		/* bytes this interval */
    long bytes_per_interval;		/* bytes per interval */
    ulonglong weight_calculation;	/* temporary variable */
    ipaddrtype destination;		/* session/destination address */
    ipaddrtype source;			/* sender ip address */
    ulong bandwidth;			/* bandwidth in bits per second */
    ulong weight;			/* weight assigned */
    ushort udp_dest_port;		/* destination UDP port */
    ushort udp_source_port;		/* source UDP port */
    ushort conversation;		/* conversation queue number */
    ushort interval;			/* token bucket interval */
    uchar service_level;		/* priority level */
    uchar ip_protocol;			/* IP Protocol */
};

/*
 * pointed to by idbtype
 */
#define RSVP_FLOW_HASH 256
struct rsvp_info_type_ {
    rsvp_flow_ipv4 *flows[RSVP_FLOW_HASH];/* list of conversations */
    rsvp_neighbor_type *rsvp_nbr;	/* list of neighbors */
    ipaddrtype udp_group;		/* special udp multicast address */
    ulong allocated_bitrate;		/* bits per second now allocated */
    ulong maximum_flow_bandwidth;	/* max bits allowed per flow */
    ulong maximum_bandwidth;		/* max bits per second allowed */
    ushort udp_neighbors;		/* number of UDP speaking neighbors */
    ushort ip_neighbors;		/* number of IP speaking neighbors */
    ushort duplex_neighbors;		/* number of bilingual neighbors */
    ushort nbr_acl;			/* access list specifying neighbors */
    tinybool udp_required;		/* configuration variable */
};

#define SESSION_PORT_THREAD 0
#define SESSION_WAVL_THREADS 1
STRUCTURE_REF(rsvp_session_head);
struct rsvp_session_head_ {
    wavl_node_type threads[SESSION_WAVL_THREADS]; /* stored in WAVL trees */

    wavl_handle static_psb_database;	/* statically configured PSBs */
    wavl_handle static_rsb_database;	/* statically configured RSBs */
    wavl_handle psb_database;		/* PSBs received from neighbors */
    wavl_handle rsb_incoming_database;	/* RSBs received from neighbors */
    wavl_handle rsb_fixed_database;	/* RSBs outbound, explicit */
    wavl_handle rsb_shared_database;	/* RSBs outbound, wildcard */

    mgd_timer master;			/* master timer for session */
    mgd_timer cleanup;			/* cleanup trigger timer */

    ulong static_psb_count;		/* statically configured PSBs */
    ulong static_rsb_count;		/* statically configured RSBs */
    ulong psb_count;			/* PSBs received from neighbors */
    ulong rsb_incoming_count;		/* RSBs received from neighbors */
    ulong rsb_outbound_count;		/* RSBs outbound */

    rsvp_session_ipv4 session;		/* session object */
};

/*
 * PSB -- Path State Block
 *
 * Each PSB holds path state for a particular(session, sender)
 * pair, which are defined by SESSION and SENDER_TEMPLATE objects,
 * respectively. PSB contents include a PHOP object and possibly
 * SENDER_TSPEC, POLICY_DATA, and/or ADSPEC objects from PATH
 * messages.
 */
#define PSB_SESSION_SENDER_THREAD 0
#define PSB_WAVL_THREADS          1
#define PSB_NO_RA_RETRIES         3    /* Final retry sends quick RA packet */
#define NO_RA_QUICK_INTERVAL      300  /* Quick intvl for PATH msgs w/o RA */
#define RA_QUICK_INTERVAL         3000 /* Quick intvl for PATH msgs with RA */
STRUCTURE_REF(psbtype);
struct psbtype_ {
    wavl_node_type threads[PSB_WAVL_THREADS]; /* stored in WAVL trees */
    rsvp_session_head *head;		/* session data structure */
    mgd_timer refresh_timer;		/* timer to trigger transmissions */
    sys_timestamp cleanup_timer;	/* timer to trigger cleanup */
    queuetype if_output;		/* list of idbs for output */
    idbtype *if_input;			/* idb received on */
    int ttl;				/* IP TTL to send using */
    rsvp_hop_ipv4 hop;			/* next hop object */
    rsvp_refresh time_values;		/* time values object */
    rsvp_template_ipv4 template;	/* sender template object */
    rsvp_tspec traffic_specification;	/* traffic specification object */
    rsvp_policy policy;			/* policy object */
    rsvp_adspec advertisement;		/* advertisement object */
    ulong no_ra_retry_count;            /* for sending no router alert */
};

/*
 * RSB -- Reservation State Block
 *
 * Each RSB holds reservation state for a particular 4-tuple:
 * (session, next hop, style, filterspec), which are defined in
 * SESSION, NHOP, STYLE, and FILTER_SPEC objects, respectively.
 * RSB contents also include a FLOWSPEC object and may include a
 * POLICY_DATA object. We assume that RSB contents include the
 * outgoing interface OI that is implied by NHOP.
 */

#define RSB_SESSION_RECEIVER_THREAD 0
#define RSB_WAVL_THREADS 1

STRUCTURE_REF(rsbtype);
struct rsbtype_ {
    wavl_node_type threads[RSB_WAVL_THREADS]; /* stored in WAVL trees */
    rsvp_session_head *head;		/* session data structure */
    mgd_timer refresh_timer;		/* timer to trigger transmissions */
    idbtype *if_input;			/* idb rsb received on */
    sys_timestamp cleanup_timer;	/* timer for cleanup */

    rsvp_confirm_ipv4 confirm;		/* confirmation address */
    rsvp_hop_ipv4 hop;			/* next hop object */
    rsvp_refresh time_values;		/* time values object */
    rsvp_policy policy;			/* policy object */
    rsvp_scope_ipv4 *scope;		/* scope object */
    rsvp_style style;			/* style object */
    rsvp_flowspec flowspec;		/* flow specification */
    rsvp_filter_ipv4 filter;		/* filter specification */
};
/***********************************************************************

 * Externs and Prototypes
 *
 ***********************************************************************/

extern wavl_handle rsvp_session_database;
extern int rsvp_running;
extern mgd_timer rsvp_root_timer;
extern pid_t rsvp_pid;
extern boolean rsvp_clr_ip_resv_signal;
extern boolean rsvp_clr_ip_sndr_signal;
extern watched_boolean *rsvp_clr_cmd_over;

extern ushort rsvp_ip_conversation(paktype * pak);
extern void rsvp_clean_interface(idbtype *idb);
extern void rsvp_refresh_path(psbtype * psb);
extern void rsvp_refresh_reservation(rsbtype * rsb);
extern void rsvp_modify_interface_reservations(hwidbtype *);
extern boolean rsvp_psb_expiry(wavl_node_type * node, va_list argptr);
extern boolean rsvp_rsb_expiry(wavl_node_type * node, va_list argptr);
extern void rsvp_input_message(paktype * pak, rsvp_header * rsvp,
			       ushort length);
extern void rsvp_clear_reservations(void);
extern void rsvp_shutdown_interface(idbtype * idb);
extern void rsvp_parser_init(void);
extern void rsvp_debug_init(void);
extern void rsvp_process(void);
extern void rsvp_neighbor(paktype * pak);
extern void rsvp_dump_object(rsvp_header *header, short length);
extern void rsvp_fixup_wfq(hwidbtype *hwidb);
extern void rsvp_calculate_wfq_weights(idbtype * idb);
extern void rsvp_session_done(rsvp_session_head *head);
extern void rsvp_session_head_init(rsvp_session_head *head,
				   ipaddrtype destination,
				   uchar protocol,
				   ushort udp_dest_port);
extern boolean rsvp_print_detail(rsvp_message_types, boolean);
extern void rsvp_show_techsupport_commands(parseinfo *);
extern boolean rsvp_clr_ip_sndr(wavl_node_type * node, va_list argptr);
extern boolean rsvp_clr_ip_resv(wavl_node_type * node, va_list argptr);
extern void rsvp_clean_session_heads(void);

enum rsvp_search_result
rsvp_session_head_search(rsvp_session_head **result,
		         ipaddrtype destination,
			 uchar protocol,
		         ushort udp_dest_port);

enum rsvp_search_result
rsvp_session_path_search(wavl_handle * handle,
			 psbtype ** result,
			 ipaddrtype source,
			 ushort udp_source_port);

enum rsvp_search_result
rsvp_session_reservation_search(wavl_handle * handle,
				rsbtype ** result,
				idbtype * if_input,
				ipaddrtype neighbor,
				ulong style,
				ipaddrtype source,
				ushort udp_source_port);

enum rsvp_search_result
rsvp_session_request_search(wavl_handle * handle,
			    rsbtype ** result,
			    idbtype * if_input,
			    ipaddrtype neighbor,
			    ulong style,
			    ipaddrtype source,
			    ushort udp_source_port);

boolean
rsvp_upstream_lookup(ipaddrtype source, ipaddrtype dest,
		     idbtype ** idb, ipaddrtype * next_hop);
boolean
rsvp_downstream_lookup(ipaddrtype source, ipaddrtype dest,
		       queuetype ** outputQ, idbtype ** idb,
		       ipaddrtype * next_hop);
boolean
rsvp_get_downstream_interface(queuetype * outputQ, idbtype * last_idb,
			      idbtype ** idb, ipaddrtype * next_hop,
			      ulong ttl);
boolean 
rsvp_needs_sm_attention(psbtype *psb);
void
rsvp_update_mdb_timers(ipaddrtype group, ipaddrtype source);
void rsvp_send_fixed_message(int message_type, idbtype * idb,
			     ipaddrtype next_hop, int number_of_objects,...);
void rsvp_send_fixed_multicast(int message_type, boolean use_ra,
			       ipaddrtype sender, idbtype *output, uchar ttl,
			       int number_of_objects,...);
/***********************************************************
 *
 * Session Database Routines
 *
 ***********************************************************/

static inline wavl_node_type *
rsvp_session_to_wavl (rsvp_session_head * session)
{
    return session->threads;
}
static inline rsvp_session_head *
rsvp_wavl_to_session (wavl_node_type * wavl_node)
{
    return(rsvp_session_head *) wavl_node;
}
/*
 * psb_get_first
 *
 * Return the first node in the specified tree, or NULL if it is empty
 */
static inline rsvp_session_head *
rsvp_first_session (wavl_handle * handle)
{
    return rsvp_wavl_to_session(wavl_get_first(handle,
					       PSB_SESSION_SENDER_THREAD));
}
/*
 * psb_get_next
 *
 * get the next one...
 */
static inline rsvp_session_head *
rsvp_next_session (wavl_handle * handle, rsvp_session_head * node)
{
    return rsvp_wavl_to_session(wavl_get_next(handle,
					      rsvp_session_to_wavl(node),
					      PSB_SESSION_SENDER_THREAD));
}
/***********************************************************
 * *
 * Path State Database Routines *
 * *
 ***********************************************************/
static inline wavl_node_type *
rsvp_psbtype_to_wavl (psbtype * psb)
{
    return psb->threads;
}
static inline psbtype *
rsvp_wavl_to_psbtype (wavl_node_type * wavl_node)
{
    return(psbtype *) wavl_node;
}
static inline boolean
rsvp_session_database_init (wavl_handle * handle, avl_compare_type cmp1)
{
    return wavl_init(handle, SESSION_WAVL_THREADS, NULL, cmp1);
}
static inline boolean
rsvp_psb_init (wavl_handle * handle, avl_compare_type cmp1)
{
    return wavl_init(handle, PSB_WAVL_THREADS, NULL, cmp1);
}
static inline void
rsvp_psb_finish (wavl_handle * handle)
{
    wavl_finish(handle);
}
/*
 * psb_get_first
 *
 * Return the first node in the specified tree, or NULL if it is empty
 */
static inline psbtype *
rsvp_session_first_path (wavl_handle * handle)
{
    return rsvp_wavl_to_psbtype(wavl_get_first(handle,
					       PSB_SESSION_SENDER_THREAD));
}
/*
 * psb_get_next
 *
 * get the next one...
 */
static inline psbtype *
rsvp_session_next_path (wavl_handle * handle, psbtype * node)
{
    return rsvp_wavl_to_psbtype(wavl_get_next(handle,
					      rsvp_psbtype_to_wavl(node),
					      PSB_SESSION_SENDER_THREAD));
}
/*
 * rsvp_psb_insert
 *
 * Insert an item on the path state database. Return a pointer to the
 * node or NULL if there is a failure.
 *
 * inserts on all threads
 */
static inline psbtype *
rsvp_psb_insert (wavl_handle * handle, psbtype * node)
{
    return rsvp_wavl_to_psbtype(wavl_insert(handle,
					    rsvp_psbtype_to_wavl(node)));
}
/*
 * wavl_delete
 *
 * Remove us from all the trees. Return a pointer to the node if at
 * least one of the avl_delete's was successful, NULL if *all* the
 * deletes fail.
 */
static inline psbtype *
rsvp_psb_delete (wavl_handle * handle, psbtype * psb)
{
    mgd_timer_stop(&psb->refresh_timer);
    return rsvp_wavl_to_psbtype(wavl_delete(handle,
					    rsvp_psbtype_to_wavl(psb)));
}
/***********************************************************
 * *
 * Reservation State Database Routines *
 * *
 ***********************************************************/

enum avl_compare rsvp_psb_compare(avl_node_type * psb1,
				  avl_node_type * psb2);
enum avl_compare rsvp_rsb_compare(avl_node_type * rsb1,
				  avl_node_type * rsb2);
enum avl_compare rsvp_rsb_shared_compare(avl_node_type * rsb1,
					 avl_node_type * rsb2);
enum avl_compare rsvp_session_compare(avl_node_type * head1,
				      avl_node_type * head2);

static inline wavl_node_type *
rsvp_rsbtype_to_wavl (rsbtype * rsb)
{
    return rsb->threads;
}
static inline rsbtype *
rsvp_wavl_to_rsbtype (wavl_node_type * wavl_node)
{
    return(rsbtype *) wavl_node;
}
static inline boolean
rsvp_rsb_init (wavl_handle * handle, avl_compare_type cmp1)
{
    return wavl_init(handle, RSB_WAVL_THREADS, NULL, cmp1);
}
static inline void
rsvp_rsb_finish (wavl_handle * handle)
{
    wavl_finish(handle);
}
/*
 * rsb_get_first
 *
 * Return the first node in the specified tree, or NULL if it is empty
 */ static inline rsbtype *
rsvp_session_first_request (wavl_handle * handle)
{
    return
	rsvp_wavl_to_rsbtype(wavl_get_first(handle,
					    RSB_SESSION_RECEIVER_THREAD));
}
/*
 * rsb_get_next
 *
 * get the next one...
 */
static inline rsbtype *
rsvp_session_next_request (wavl_handle * handle, rsbtype * node)
{
    return rsvp_wavl_to_rsbtype(wavl_get_next(handle,
					      rsvp_rsbtype_to_wavl(node),
					      RSB_SESSION_RECEIVER_THREAD));
}
/*
 * rsb_get_first
 *
 * Return the first node in the specified tree, or NULL if it is empty
 */
static inline rsbtype *
rsvp_session_first_reservation (wavl_handle * handle)
{
    return
	rsvp_wavl_to_rsbtype(wavl_get_first(handle,
					    RSB_SESSION_RECEIVER_THREAD));
}
/*
 * rsb_get_next
 *
 * get the next one...
 */
static inline rsbtype *
rsvp_session_next_reservation (wavl_handle * handle, rsbtype * node)
{
    return rsvp_wavl_to_rsbtype(wavl_get_next(handle,
					      rsvp_rsbtype_to_wavl(node),
					      RSB_SESSION_RECEIVER_THREAD));
}
/*
 * rsvp_rsb_insert
 *
 * Insert an item on the path state database. Return a pointer to the
 * node or NULL if there is a failure.
 *
 * inserts on all threads
 */
static inline rsbtype *
rsvp_rsb_insert (wavl_handle * handle, rsbtype * node)
{
    return rsvp_wavl_to_rsbtype(wavl_insert(handle,
					    rsvp_rsbtype_to_wavl(node)));
}
/*
 * wavl_delete
 *
 * Remove us from all the trees. Return a pointer to the node if at
 * least one of the avl_delete's was successful, NULL if *all* the
 * deletes fail.
 */
static inline rsbtype *
rsvp_rsb_delete (wavl_handle * handle, rsbtype * node)
{
    return rsvp_wavl_to_rsbtype(wavl_delete(handle,
					    rsvp_rsbtype_to_wavl(node)));
}
#endif /* __RSVP_DATABASE_H__ */

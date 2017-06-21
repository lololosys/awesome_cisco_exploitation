/* $Id: rsvp_inline.h,v 1.1.4.9 1996/08/20 10:24:26 fred Exp $
 * $Source: /release/112/cvs/Xsys/rsvp/rsvp_inline.h,v $
 *------------------------------------------------------------------
 * Resource Reservation Protocol
 *
 * April 1996, Fred Baker
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rsvp_inline.h,v $
 * Revision 1.1.4.9  1996/08/20  10:24:26  fred
 * CSCdi66538:  implement integrated services objects (RSVP)
 * Branch: California_branch
 *
 * Revision 1.1.4.8  1996/07/17  20:43:02  snyder
 * CSCdi61528:  RSVP inlines have format strings wasting ~6k
 *              Code review change
 * Branch: California_branch
 *
 * Revision 1.1.4.7  1996/07/15  20:45:06  fred
 * CSCdi61528:  RSVP inlines have format strings wasting ~6k
 *              Move some of the access routines from inline to hard code
 * Branch: California_branch
 *
 * Revision 1.1.4.6  1996/06/18  08:37:13  fred
 * CSCdi60599:  RSVP interaction with PIM Sparse Mode has problems.
 *              Implement special code required for sparse mode route
 *              creation
 * Branch: California_branch
 *
 * Revision 1.1.4.5  1996/06/11  16:23:14  fred
 * CSCdi60109:  replace rsvp_ip_address_is_ours with ip_ouraddress
 * Branch: California_branch
 *
 * Revision 1.1.4.4  1996/06/11  16:06:55  fred
 * CSCdi60109:  replace rsvp_ip_address_is_ours with ip_ouraddress
 * Branch: California_branch
 *
 * Revision 1.1.4.3  1996/05/13  16:45:50  fred
 * CSCdi57500:  update rsvp from draft 8 to draft 11 objects
 *         This is done with an ifdef at this point as some EFT sites
 *         need draft 8 code; these will be removed later
 * Branch: California_branch
 *
 * Revision 1.1.4.2  1996/05/13  16:42:28  fred
 * CSCdi57498:  rsvp data structures trashed - reverse order of destination
 *         and source when changing bcopy to memcpy calls
 * Branch: California_branch
 *
 * Revision 1.1.4.1  1996/04/17  13:57:21  fred
 * CSCdi54910:  commit rsvp, traffic shaping, and RED
 * Branch: California_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __RSVP_OBJECTS_H__
#define __RSVP_OBJECTS_H__
#include "../lib/cisco/include/ciscolib.h"
#include "string.h"

rsvp_object *rsvp_next_nonnull_object(rsvp_object *object, ushort *remaining);
boolean rsvp_extract_adspec(ushort *length, rsvp_object **object,
			    rsvp_adspec **result);
boolean rsvp_extract_confirm_ipv4(ushort *length, rsvp_object **object,
				  rsvp_confirm_ipv4 **result);
boolean rsvp_valid_error_ipv4(rsvp_error_ipv4 *error_spec_object);
boolean rsvp_extract_error_ipv4(ushort *length, rsvp_object **object,
				rsvp_error_ipv4 **result);
boolean rsvp_valid_filter_ipv4(rsvp_filter_ipv4 *filter_spec_object);
boolean rsvp_extract_filter_ipv4(ushort *length, rsvp_object **object,
				 rsvp_filter_ipv4 **result);
boolean rsvp_valid_flowspec(rsvp_flowspec *flowspec_object);
boolean rsvp_extract_flowspec(ushort *length, rsvp_object **object,
			      rsvp_flowspec **result);
boolean rsvp_valid_integrity(rsvp_integrity *integrity_object);
boolean rsvp_extract_integrity(ushort *length, rsvp_object **object,
			       rsvp_integrity **result);
boolean rsvp_extract_null(ushort *length, rsvp_object **object,
			  rsvp_null **result);
boolean rsvp_extract_policy(ushort *length, rsvp_object **object,
			    rsvp_policy **result);
boolean rsvp_valid_hop_ipv4(rsvp_hop_ipv4 *rsvp_hop_object,
			    idbtype *incoming_idb);
boolean rsvp_extract_hop_ipv4(ushort *length, rsvp_object **object,
			      rsvp_hop_ipv4 **result, idbtype *incoming_idb);
boolean rsvp_valid_scope_ipv4(rsvp_scope_ipv4 *scope_object);
boolean rsvp_extract_scope(ushort *length, rsvp_object **object,
			   rsvp_scope_ipv4 **result);
boolean rsvp_valid_template_ipv4(rsvp_template_ipv4 *sender_template,
				 rsvp_session_ipv4 *session);
boolean rsvp_extract_template_ipv4(ushort *length, rsvp_object **object,
				   rsvp_template_ipv4 **result,
				   rsvp_session_ipv4 *session);
boolean rsvp_extract_tspec_ipv4(ushort *length, rsvp_object **object,
				rsvp_tspec **result);
boolean rsvp_valid_session_ipv4(rsvp_session_ipv4 *session_object);
boolean rsvp_extract_session_ipv4(ushort *length, rsvp_object **object,
				  rsvp_session_ipv4 **result);
void rsvp_build_style(void *object, uchar session_style);
boolean rsvp_valid_style(rsvp_style *style_object);
boolean rsvp_extract_style(ushort *length, rsvp_object **object,
			   rsvp_style **result);
boolean rsvp_valid_refresh(rsvp_refresh *refresh);
boolean rsvp_extract_refresh(ushort *length, rsvp_object **object,
			     rsvp_refresh **result);
void rsvp_send_resv(idbtype *idb,
		    rsvp_hop_ipv4 *hop,
		    rsvp_session_ipv4 *session,
		    rsvp_refresh *time_values,
		    rsvp_policy *policy,
		    rsvp_confirm_ipv4 *confirm,
		    rsvp_scope_ipv4 *scope,
		    rsvp_style *style,
		    rsvp_flowspec *flowspec,
		    rsvp_filter_ipv4 *filter);
void rsvp_send_resv_error(idbtype *idb,
			  ipaddrtype neighbor,
			  rsvp_hop_ipv4 *hop,
			  rsvp_session_ipv4 *session,
			  rsvp_error_ipv4 *error_spec,
			  rsvp_scope_ipv4 *scope,
			  rsvp_policy *policy,
			  rsvp_style *style,
			  rsvp_flowspec *flowspec,
			  rsvp_filter_ipv4 *filter);
void rsvp_send_resv_confirm(idbtype *idb,
			    ipaddrtype neighbor,
			    rsvp_session_ipv4 *session,
			    rsvp_error_ipv4 *error_spec,
			    rsvp_confirm_ipv4 *confirm,
			    rsvp_style *style,
			    rsvp_flowspec *flowspec,
			    rsvp_filter_ipv4 *filter);
void rsvp_send_resv_teardown(idbtype *idb,
			     rsvp_hop_ipv4 *hop,
			     rsvp_session_ipv4 *session,
			     rsvp_scope_ipv4 *scope,
			     rsvp_style *style,
			     rsvp_flowspec *flowspec,
			     rsvp_filter_ipv4 *filter);
void rsvp_send_path(idbtype *output,
		    uchar ttl,
		    rsvp_session_ipv4 *session,
		    rsvp_refresh *time_values,
		    rsvp_template_ipv4 *template,
		    rsvp_tspec *tspec,
		    rsvp_policy * policy,
		    rsvp_adspec *advertisement,
		    boolean use_ra);
void rsvp_send_path_error(idbtype *idb,
			  ipaddrtype neighbor,
			  rsvp_session_ipv4 *session,
			  rsvp_error_ipv4 *error_spec,
			  rsvp_template_ipv4 *template,
			  rsvp_tspec *tspec,
			  rsvp_policy * policy,
			  rsvp_adspec *advertisement);
void rsvp_send_path_teardown(idbtype *output,
			     uchar ttl,
			     rsvp_session_ipv4 *session,
			     rsvp_template_ipv4 *template,
			     rsvp_tspec *tspec,
			     rsvp_adspec *advertisement);
void rsvp_build_flowspec(void *object,
			 enum qos_service_type qos_service,
			 ulong maximum_burst, ulong average_bit_rate,
			 ulong maximum_e2e_delay, ulong service_level,
			 ulong min_unit, ulong max_unit, ulong peak,
			 ulong requested_rate, ulong slack,
			 ushort composed_mtu);
boolean rsvp_valid_flowspec(rsvp_flowspec *flowspec);
void rsvp_build_adspec(void *object);
boolean rsvp_valid_adspec(rsvp_adspec *adspec);
void rsvp_build_tspec(void *object, ulong rate, ulong depth, 
		      ulong peak, ulong min_unit, ulong max_unit);
boolean rsvp_valid_tspec(rsvp_tspec *tspec);
ulong rsvp_get_average_bit_rate(rsvp_tspec *tspec);
ulong rsvp_get_maximum_burst(rsvp_tspec *tspec);
ulong rsvp_get_peak (rsvp_tspec *tspec);
ulong rsvp_get_flow_average_bit_rate(rsvp_flowspec *flowspec);
ulong rsvp_get_flow_maximum_burst(rsvp_flowspec *flowspec);
ulong rsvp_get_flow_maximum_e2e_delay(rsvp_flowspec *flowspec);
ulong rsvp_get_flow_service_level(rsvp_flowspec *flowspec);
ushort rsvp_get_flow_min_unit(rsvp_flowspec *flowspec);
ushort rsvp_get_flow_max_unit(rsvp_flowspec *flowspec);
ulong rsvp_get_flow_peak(rsvp_flowspec *flowspec);
ulong rsvp_get_flow_requested_rate(rsvp_flowspec *flowspec);
ulong rsvp_get_flow_slack(rsvp_flowspec *flowspec);
ushort rsvp_get_flow_composed_mtu(rsvp_flowspec *flowspec);
enum qos_service_type rsvp_get_flow_qos_service(rsvp_flowspec *flowspec);

/*
 *	Copy an object from here to there
 */
static inline void
rsvp_object_copy (void *tov, void *fromv)
{
    rsvp_object *to = tov;
    rsvp_object *from = fromv;
    if (from->length)
	bcopy(from, to, from->length);
}
/*
 * whose address is this
 */
static inline ipaddrtype
rsvp_ip_address (idbtype *idb)
{
    if (idb->ip_address)
	return idb->ip_address;
    else if (idb->ip_unnumbered)
	return idb->ip_unnumbered->ip_address;
    else
	return 0;
}

/*
 * build a basic object
 */
static inline void
rsvp_build_object (void *address, uchar class,
		   uchar type, ushort length)
{
    rsvp_object *object = address;

    object->length = length;
    object->class = class;
    object->type = type;
}
/*
 * check for new object
 */
static inline boolean
rsvp_unrecognized_object (rsvp_null * null_object)
{
    return(null_object->base.class > RSVP_CLASS_MAX);
}
/*
 * valid NULL object, returning TRUE
 * if valid and FALSE otherwise
 */
static inline boolean
rsvp_valid_null_object (rsvp_null * null_object)
{
    if(null_object->base.class != RSVP_CLASS_NULL)
	return FALSE;
    return TRUE;
}
/*
 * step to next object
 */
static inline rsvp_object *
rsvp_next_object (rsvp_object * object)
{
    return(rsvp_object *)(object->length +(char *) object);
}
/*
 * valid RSVP_HOP object, returning TRUE if valid and FALSE otherwise
 */
static inline rsvp_object *
rsvp_first_object (rsvp_header * header, ushort * remaining)
{
    rsvp_object *object = header->rsvp_first_object;

    *remaining = header->rsvp_length - sizeof(rsvp_header);
    while(rsvp_valid_null_object((rsvp_null *) object))
	object = rsvp_next_nonnull_object(object, remaining);
    return object;
}
/*
 * build a COMMON_HEADER object
 */
#ifdef DRAFT10
static ulong rsvp_message_id = 0;
#endif
static inline rsvp_object *
rsvp_build_header (void *object, uchar message_type, uchar sending_ttl)
{
    rsvp_header *common_header = object;

    bzero(common_header, sizeof(rsvp_header));
    common_header->version = RSVP_VERSION;
    common_header->message_type = message_type;
    common_header->rsvp_length = sizeof(rsvp_header);
#ifdef DRAFT10
    common_header->message_id = rsvp_message_id++;
#endif
    common_header->sending_ttl = sending_ttl;
    return common_header->rsvp_first_object;
}
/*
 * build a CONFIRM object
 */
static inline void
rsvp_build_confirm_ipv4 (void *object, ipaddrtype receiver)
{
    rsvp_confirm_ipv4 *confirm = object;

    rsvp_build_object(&confirm->base, RSVP_CLASS_RESV_CONFIRM, RSVP_TYPE_IPV4,
		      sizeof(rsvp_confirm_ipv4));
    confirm->receiver = receiver;
}
/*
 * valid CONFIRM object, returning TRUE if valid and FALSE otherwise
 */
static inline boolean
rsvp_valid_confirm_ipv4 (rsvp_confirm_ipv4 * confirm_object)
{
    if(confirm_object->base.class != RSVP_CLASS_RESV_CONFIRM)
	return FALSE;
    if(confirm_object->base.length != sizeof(rsvp_confirm_ipv4))
	return FALSE;
    return TRUE;
}
/*
 * build a ERROR_SPEC_IPV4 object
 */
static inline void
rsvp_build_error_ipv4 (void *object, ipaddrtype error_node,
		       uchar flags, uchar error_code, ushort error_value)
{
    rsvp_error_ipv4 *error_spec = object;

    rsvp_build_object(&error_spec->base, RSVP_CLASS_ERROR_SPEC,
		      RSVP_TYPE_IPV4, sizeof(rsvp_error_ipv4));
    error_spec->error_node = error_node;
    error_spec->flags = flags;
    error_spec->error_code = error_code;
    error_spec->error_value = error_value;
}
/*
 * build a FILTER_SPEC_IPV4 object
 */
static inline void
rsvp_build_filter_ipv4 (void *object,
			ipaddrtype source, /* source sending data */
			ushort udp_source_port)
{
    rsvp_filter_ipv4 *filter_spec = object;

    rsvp_build_object(&filter_spec->base, RSVP_CLASS_FILTER_SPEC,
		      RSVP_TYPE_IPV4, sizeof(rsvp_filter_ipv4));
    filter_spec->source = source;
    filter_spec->unused = 0;
    filter_spec->udp_source_port = udp_source_port;
}
/*
 * build a INTEGRITY object
 */
static inline void
rsvp_build_integrity (void *object, uchar key_number, unsigned sequence,
		      uchar md5_digest[16])
{
    rsvp_integrity *integrity = object;

    rsvp_build_object(&integrity->base, RSVP_CLASS_INTEGRITY,
		      RSVP_TYPE_IPV4, sizeof(rsvp_integrity));
    integrity->key_number = key_number;
    integrity->sequence = sequence;
    memcpy(integrity->md5_digest, md5_digest, sizeof(md5_digest));
}
/*
 * build a NULL object
 */
static inline void
rsvp_build_null (void *object)
{
    rsvp_null *null = object;

    rsvp_build_object(null, RSVP_CLASS_NULL, RSVP_TYPE_IPV4,
		      sizeof(rsvp_null));
}
/*
 * build a POLICY_DATA object
 */
static inline void
rsvp_build_policy (void *object)
{
    rsvp_policy *policy_data = object;

    rsvp_build_object(policy_data, RSVP_CLASS_POLICY_DATA, RSVP_TYPE_IPV4,
		      sizeof(rsvp_policy));
}
/*
 * valid POLICY_DATA object, returning TRUE if valid and FALSE otherwise
 */
static inline boolean
rsvp_valid_policy (rsvp_policy * policy_data_object)
{
    if(policy_data_object->base.class != RSVP_CLASS_POLICY_DATA)
	return FALSE;
    return TRUE;
}
/*
 * build a RSVP_HOP_IPV4 object
 */
static inline void
rsvp_build_hop_ipv4 (void *object,
		     ipaddrtype neighbor, /* next RSVP system */
		     void *lif_handle)
{
    rsvp_hop_ipv4 *rsvp_hop = object;

    rsvp_build_object(&rsvp_hop->base, RSVP_CLASS_HOP,
		      RSVP_TYPE_IPV4, sizeof(rsvp_hop_ipv4));
    rsvp_hop->neighbor = neighbor;
    rsvp_hop->lif_handle = lif_handle;
}
/*
 * build a SCOPE_IPV4 object
 */
static inline void
rsvp_build_scope_ipv4 (void *object)
{
    rsvp_scope_ipv4 *scope= object;

    rsvp_build_object(&scope->base, RSVP_CLASS_SCOPE,
		      RSVP_TYPE_IPV4, sizeof(rsvp_object));
}
static inline void
rsvp_add_scope_ipv4 (void *object, ipaddrtype source)
{
    rsvp_scope_ipv4 *scope = object;
    ipaddrtype *list;
    int count;

    /*
     * search existing list; NO DUPLICATES!
     */
    count = scope->base.length - sizeof(scope->base);
    count /= sizeof(ipaddrtype);
    list = scope->source;
    while(--count >= 0)
	if(*list++ == source)
	    return;

    /*
     * not found in list - add it
     */
    scope->base.length += sizeof(ipaddrtype);
    *list = source;
}
/*
 * build a SENDER_TEMPLATE_IPV4 object
 */
static inline void
rsvp_build_template_ipv4 (void *object,
			  ipaddrtype source, /* source sending data */
			  ushort udp_source_port)
{
    rsvp_template_ipv4 *sender_template= object;

    rsvp_build_object(&sender_template->base, RSVP_CLASS_SENDER_TEMPLATE,
		      RSVP_TYPE_IPV4, sizeof(rsvp_template_ipv4));
    sender_template->source = source;
    sender_template->unused = 0;
    sender_template->udp_source_port = udp_source_port;
}
/*
 * build a SESSION_IPV4 object
 */
static inline void
rsvp_build_session_ipv4 (void *object, ipaddrtype destination,
			 uchar protocol, uchar flags, ushort udp_dest_port)
{
    rsvp_session_ipv4 *session= object;

    rsvp_build_object(&session->base, RSVP_CLASS_SESSION,
		      RSVP_TYPE_IPV4, sizeof(rsvp_session_ipv4));
    session->destination = destination;
    session->protocol = protocol;
    session->flags = flags;
    session->udp_dest_port = udp_dest_port;
}
/*
 * unless it describes the default value, build a TIME_VALUES object
 */
static inline void
rsvp_build_refresh (void *object, ulong refresh_ms)
{
    rsvp_refresh *time_values = object;

    if (refresh_ms < 10000)
	refresh_ms = 10000;

    rsvp_build_object(&time_values->base, RSVP_CLASS_TIME_VALUES,
		      1, sizeof(rsvp_refresh));
    time_values->refresh_ms = refresh_ms;
}
/*
 * copy an object
 */
static inline
void *rsvp_object_copy_new (rsvp_object * object) {
    rsvp_object *object_copy;
    object_copy = malloc(object->length);
    if (object_copy)
	memcpy(object_copy, object, object->length);
    return object_copy;
}
/****************************************************************
 *								*
 * Compatibility Wrappers and Defensive Code Routines *
 *								*
 ***************************************************************/

/*
 * build routing alert option
 */
extern ipoptiontype rsvp_alert;
static inline void
rsvp_routing_alert_init (void)
{
    rsvp_alert.length = IPOPT_ALERTSIZE;
    rsvp_alert.paddedlength = IPOPT_ALERTSIZE;
    bzero(rsvp_alert.ipopts, MAXIPOPTIONBYTES);
    rsvp_alert.ipopts[0] = IPOPT_ALERT;
    rsvp_alert.ipopts[1] = IPOPT_ALERTSIZE;
}
static inline boolean
rsvp_same_path (rsvp_session_ipv4 * session,
		rsvp_template_ipv4 * template, psbtype * psb)
{
    if(session != &psb->head->session)
	return(FALSE);
    if(template->source != psb->template.source)
	return(FALSE);
    if(template->udp_source_port != psb->template.udp_source_port)
	return(FALSE);
    return TRUE;
}
/*
 * routines to trigger or schedule refresh
 */
#define RSVP_JITTER_PERCENT 10
static inline void
rsvp_schedule_path_refresh (psbtype * psb)
{
    mgd_timer_start_jittered(&psb->refresh_timer,
			     psb->time_values.refresh_ms, RSVP_JITTER_PERCENT);
}
static inline void
rsvp_schedule_resv_refresh (rsbtype * rsb)
{
    mgd_timer_start_jittered(&rsb->refresh_timer,
			     rsb->time_values.refresh_ms, RSVP_JITTER_PERCENT);
}
static inline void
rsvp_trigger_path_refresh (psbtype * psb)
{
    mgd_timer_start(&psb->refresh_timer, 1);
}
static inline void
rsvp_trigger_resv_refresh (rsbtype * rsb)
{
    mgd_timer_start(&rsb->refresh_timer, 1);
}
static inline void
rsvp_stop_path_refresh (psbtype * psb)
{
    mgd_timer_stop(&psb->refresh_timer);
}
static inline void
rsvp_stop_resv_refresh (rsbtype * rsb)
{
    mgd_timer_stop(&rsb->refresh_timer);
}
/*
 * return request database address
 */
static inline wavl_handle *
rsvp_request_database (rsbtype * rsb)
{
    if(rsb->style.reservation_type & RSVP_SCOPE_WILD)
	return(&rsb->head->rsb_shared_database);
    else
	return(&rsb->head->rsb_fixed_database);
}
#endif __RSVP_OBJECTS_H__

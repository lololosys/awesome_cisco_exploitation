/* $Id: rsvp_objects.c,v 1.1.2.6 1996/08/24 01:16:46 fred Exp $
 * $Source: /release/112/cvs/Xsys/rsvp/rsvp_objects.c,v $
 *------------------------------------------------------------------
 * Resource Reservation Protocol
 *
 * July 1996, Fred Baker
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rsvp_objects.c,v $
 * Revision 1.1.2.6  1996/08/24  01:16:46  fred
 * CSCdi67062:  Resolution of interoperability testing with ISI 4.0a6 code
 * Branch: California_branch
 *
 * Revision 1.1.2.5  1996/08/20  20:18:05  fred
 * CSCdi66538:  implement integrated services objects
 *         integrate code review comments
 * Branch: California_branch
 *
 * Revision 1.1.2.4  1996/08/20  10:24:35  fred
 * CSCdi66538:  implement integrated services objects (RSVP)
 * Branch: California_branch
 *
 * Revision 1.1.2.3  1996/08/09  00:18:20  asastry
 * CSCdi65504:  Allow <debug ip rsvp detail> command to be RESV-only or
 * PATH-only
 * Branch: California_branch
 *
 * Revision 1.1.2.2  1996/07/19  06:09:30  asastry
 * CSCdi63472:  Add page breaks to RSVP files
 * Branch: California_branch
 *
 * Revision 1.1.2.1  1996/07/15  20:45:04  fred
 * CSCdi61528:  RSVP inlines have format strings wasting ~6k
 *              Move some of the access routines from inline to hard code
 * Branch: California_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#include "master.h"
#include "packet.h"
#include "interface_private.h"
#include "config.h"
#include "parser.h"
#include "access.h"
#include "logger.h"
#include "subsys.h"
#include "../os/signal.h"
#include "../ip/ip.h"
#include "../ip/ipoptions.h"
#include "../ip/ip_registry.h"
#include "../if/priority_private.h"
#include "rsvp_objects.h"
#include "rsvp_database.h"
#include "rsvp_inline.h"

/*
 * step to next object, skipping NULL objects
 */
rsvp_object *
rsvp_next_nonnull_object (rsvp_object *object, ushort *remaining)
{
    ushort length = *remaining;

    if(length) {
	do {
	    length -= object->length;
	    object = rsvp_next_object(object);
	}
	while(length &&
	      (rsvp_valid_null_object((rsvp_null *) object) ||
	       rsvp_unrecognized_object((rsvp_null *) object)));
	*remaining = length;
    }
    return object;
}

/*
 * extract rsvp_adspec object
 */
boolean
rsvp_extract_adspec (ushort *length, rsvp_object ** object,
		     rsvp_adspec ** result)
{
    if(*length && RSVP_CLASS_ADSPEC == (*object)->class) {
	*result =(rsvp_adspec *)(*object);
	if(!rsvp_valid_adspec(*result)) {
	    return FALSE;
	}
	*object = rsvp_next_nonnull_object(*object, length);
    }
    return TRUE;
}

/*
 * extract rsvp_confirm object
 */
boolean
rsvp_extract_confirm_ipv4 (ushort *length, rsvp_object ** object,
			   rsvp_confirm_ipv4 ** result)
{
    if(RSVP_CLASS_RESV_CONFIRM == (*object)->class) {
	*result =(rsvp_confirm_ipv4 *)(*object);
	if(!rsvp_valid_confirm_ipv4(*result)) {
	    return FALSE;
	}
	*object = rsvp_next_nonnull_object(*object, length);
    }
    return TRUE;
}

/*
 * valid ERROR_SPEC object, returning TRUE if valid and FALSE otherwise
 */
boolean
rsvp_valid_error_ipv4 (rsvp_error_ipv4 *error_spec_object)
{
    if(error_spec_object->base.class != RSVP_CLASS_ERROR_SPEC)
	return FALSE;
    if(error_spec_object->base.type != RSVP_TYPE_IPV4) {
	RSVP_BUGINF(0, ("\nRSVP: error specification object type != 1"));
	return FALSE;
    }
    if(error_spec_object->base.length != sizeof(rsvp_error_ipv4)) {
	RSVP_BUGINF(0, ("\nRSVP: error specification size != %d",
			sizeof(rsvp_error_ipv4)));
	return FALSE;
    }
    return TRUE;
}

/*
 * extract rsvp_error_ipv4 object
 */
boolean
rsvp_extract_error_ipv4 (ushort *length, rsvp_object ** object,
			 rsvp_error_ipv4 ** result)
{
    if(*length && RSVP_CLASS_ERROR_SPEC == (*object)->class) {
	*result =(rsvp_error_ipv4 *)(*object);
	if(!rsvp_valid_error_ipv4(*result)) {
	    return FALSE;
	}
	*object = rsvp_next_nonnull_object(*object, length);
    }
    return TRUE;
}

/*
 * valid FILTER_SPEC object, returning TRUE if valid and FALSE otherwise
 */
boolean
rsvp_valid_filter_ipv4 (rsvp_filter_ipv4 *filter_spec_object)
{
    if(filter_spec_object->base.class != RSVP_CLASS_FILTER_SPEC)
	return FALSE;
    if(filter_spec_object->base.type != RSVP_TYPE_IPV4) {
	RSVP_BUGINF(0, ("\nRSVP: filter specification object type != 1"));
	return FALSE;
    }
    if(filter_spec_object->base.length != sizeof(rsvp_filter_ipv4)) {
	RSVP_BUGINF(0, ("\nRSVP: filter specification size != %d",
			sizeof(rsvp_filter_ipv4)));
	return FALSE;
    }
    return TRUE;
}

/*
 * extract rsvp_filter_ipv4 object
 */
boolean
rsvp_extract_filter_ipv4 (ushort *length, rsvp_object ** object,
			  rsvp_filter_ipv4 ** result)
{
    if(*length && RSVP_CLASS_FILTER_SPEC == (*object)->class) {
	*result =(rsvp_filter_ipv4 *)(*object);
	if(!rsvp_valid_filter_ipv4(*result)) {
	    return FALSE;
	}
	*object = rsvp_next_nonnull_object(*object, length);
    }
    return TRUE;
}

/*
 * extract rsvp_flowspec object
 */
boolean
rsvp_extract_flowspec (ushort *length, rsvp_object ** object,
		       rsvp_flowspec ** result)
{
    if(*length && RSVP_CLASS_FLOWSPEC == (*object)->class) {
	*result =(rsvp_flowspec *)(*object);
	if(!rsvp_valid_flowspec(*result)) {
	    return FALSE;
	}
	*object = rsvp_next_nonnull_object(*object, length);
    }
    return TRUE;
}

/*
 * valid INTEGRITY object, returning TRUE if valid and FALSE otherwise
 */
boolean
rsvp_valid_integrity (rsvp_integrity *integrity_object)
{
    if(integrity_object->base.class != RSVP_CLASS_INTEGRITY)
	return FALSE;
    if(integrity_object->base.type != RSVP_TYPE_CRYPTO_INTEGRITY) {
	RSVP_BUGINF(0, ("\nRSVP: integrity object type != 1"));
	return FALSE;
    }
    if(integrity_object->base.length != sizeof(rsvp_integrity)) {
	RSVP_BUGINF(0, ("\nRSVP: integrity object size != %d",
			sizeof(rsvp_integrity)));
	return FALSE;
    }

    /*
     * note that we very carefully didn't calculate it - after the demo :^)
     */

    return TRUE;
}

/*
 * extract rsvp_integrity object
 */
boolean
rsvp_extract_integrity (ushort *length, rsvp_object ** object,
			rsvp_integrity ** result)
{
    if(*length && RSVP_CLASS_INTEGRITY == (*object)->class) {
	*result =(rsvp_integrity *)(*object);
	if(!rsvp_valid_integrity(*result)) {
	    return FALSE;
	}
	*object = rsvp_next_nonnull_object(*object, length);
    }
    return TRUE;
}

/*
 * extract rsvp_null object
 */
boolean
rsvp_extract_null (ushort *length, rsvp_object ** object,
		   rsvp_null ** result)
{
    if(*length && RSVP_CLASS_NULL == (*object)->class) {
	*result =(rsvp_null *)(*object);
	if(!rsvp_valid_null_object(*result)) {
	    return FALSE;
	}
	*object = rsvp_next_nonnull_object(*object, length);
    }
    return TRUE;
}

/*
 * extract rsvp_policy object
 */
boolean
rsvp_extract_policy (ushort *length, rsvp_object ** object,
		     rsvp_policy ** result)
{
    if(*length && RSVP_CLASS_POLICY_DATA == (*object)->class) {
	*result =(rsvp_policy *)(*object);
	if(!rsvp_valid_policy(*result)) {
	    return FALSE;
	}
	*object = rsvp_next_nonnull_object(*object, length);
    }
    return TRUE;
}

/*
 * valid RSVP_HOP object, returning TRUE if valid and FALSE otherwise
 */
boolean
rsvp_valid_hop_ipv4 (rsvp_hop_ipv4 *rsvp_hop_object, idbtype *incoming_idb)
{
    if(rsvp_hop_object->base.class != RSVP_CLASS_HOP)
	return FALSE;
    if(rsvp_hop_object->base.type != RSVP_TYPE_IPV4) {
	RSVP_BUGINF(0, ("\nRSVP: next hop object type != 1"));
	return FALSE;
    }
    if(rsvp_hop_object->base.length != sizeof(rsvp_hop_ipv4)) {
	RSVP_BUGINF(0, ("\nRSVP: next hop size != %d",
			sizeof(rsvp_hop_ipv4)));
	return FALSE;
    }
    if(rsvp_hop_object->neighbor == 0) {
	RSVP_BUGINF(0, ("\nRSVP: next hop ip address is zero"));
	return FALSE;
    }
    return TRUE;
}

/*
 * extract rsvp_hop_ipv4 object
 */
boolean
rsvp_extract_hop_ipv4 (ushort *length, rsvp_object ** object,
		       rsvp_hop_ipv4 ** result, idbtype *incoming_idb)
{
    if(*length && RSVP_CLASS_HOP == (*object)->class) {
	*result =(rsvp_hop_ipv4 *)(*object);
	if(!rsvp_valid_hop_ipv4(*result, incoming_idb)) {
	    return FALSE;
	}
	*object = rsvp_next_nonnull_object(*object, length);
    }
    return TRUE;
}

/*
 * valid SCOPE object, returning TRUE if valid and FALSE otherwise
 */
boolean
rsvp_valid_scope_ipv4 (rsvp_scope_ipv4 *scope_object)
{
    if(scope_object->base.class != RSVP_CLASS_SCOPE)
	return FALSE;
    if(scope_object->base.type != RSVP_TYPE_IPV4) {
	RSVP_BUGINF(0, ("\nRSVP: scope object type != 1"));
	return FALSE;
    }
    /*
     * there better be at least one address there
     */
    if(scope_object->base.length == sizeof(rsvp_object)) {
	RSVP_BUGINF(0, ("\nRSVP: scope contains no addresses"));
	return FALSE;
    }
    return TRUE;
}

/*
 * extract rsvp_scope object
 */
boolean
rsvp_extract_scope (ushort *length, rsvp_object ** object,
		    rsvp_scope_ipv4 ** result)
{
    if(*length && RSVP_CLASS_SCOPE == (*object)->class) {
	*result =(rsvp_scope_ipv4 *)(*object);
	if(!rsvp_valid_scope_ipv4(*result)) {
	    return FALSE;
	}
	*object = rsvp_next_nonnull_object(*object, length);
    }
    return TRUE;
}

/*
 * valid SENDER_TEMPLATE object, returning TRUE if valid and FALSE otherwise
 */
boolean
rsvp_valid_template_ipv4 (rsvp_template_ipv4 *sender_template,
			  rsvp_session_ipv4 *session)
{
    if(sender_template->base.class != RSVP_CLASS_SENDER_TEMPLATE)
	return FALSE;
    if(sender_template->base.type != RSVP_TYPE_IPV4) {
	RSVP_BUGINF(0, ("\nRSVP: template object type != 1"));
	return FALSE;
    }
    if(sender_template->base.length != sizeof(rsvp_template_ipv4)) {
	RSVP_BUGINF(0, ("\nRSVP: template size != %d", 
			sizeof(rsvp_error_ipv4)));
	return FALSE;
    }
    if (sender_template->udp_source_port)
	if (session->protocol != UDP_PROT && session->protocol != TCP_PROT) {
	    RSVP_BUGINF(0, ("\nRSVP: template specifies source port but "
			    "neither UDP nor TCP"));
	    return FALSE;
	}

    return TRUE;
}

/*
 * extract rsvp_template_ipv4 object
 */
boolean
rsvp_extract_template_ipv4 (ushort *length, rsvp_object ** object,
			    rsvp_template_ipv4 ** result,
			    rsvp_session_ipv4 *session)
{
    if(*length && RSVP_CLASS_SENDER_TEMPLATE == (*object)->class) {
	*result =(rsvp_template_ipv4 *)(*object);
	if(!rsvp_valid_template_ipv4(*result, session)) {
	    return FALSE;
	}
	*object = rsvp_next_nonnull_object(*object, length);
    }
    return TRUE;
}

/*
 * extract rsvp_tspec object
 */
boolean
rsvp_extract_tspec_ipv4 (ushort *length, rsvp_object ** object,
			 rsvp_tspec ** result)
{
    if(*length && RSVP_CLASS_SENDER_TSPEC == (*object)->class) {
	*result =(rsvp_tspec *)(*object);
	if(!rsvp_valid_tspec(*result)) {
	    return FALSE;
	}
	*object = rsvp_next_nonnull_object(*object, length);
    }
    return TRUE;
}

/*
 * valid SESSION object, returning TRUE if valid and FALSE otherwise
 */
boolean
rsvp_valid_session_ipv4 (rsvp_session_ipv4 *session_object)
{
    if(session_object->base.class != RSVP_CLASS_SESSION)
	return FALSE;
    if(session_object->base.type != RSVP_TYPE_IPV4) {
	RSVP_BUGINF(0, ("\nRSVP: session object type != 1"));
	return FALSE;
    }
    if(session_object->base.length != sizeof(rsvp_session_ipv4)) {
	RSVP_BUGINF(0, ("\nRSVP: session size != %d", 
			sizeof(rsvp_session_ipv4)));
	return FALSE;
    }
    switch(session_object->protocol) {
    case 0:
	RSVP_BUGINF(0, ("session has non-zero port and zero ip protocol; "
			"forcing ip protocol to UDP"));
	break;
    default:
	if(session_object->udp_dest_port) {
	    RSVP_BUGINF(0, ("session has non-zero port and "
			    "ip protocol is neither UDP nor TCP"));
	}
	break;
    case UDP_PROT:			/* UDP */
    case TCP_PROT:			/* TCP */
	break;
    }
    return TRUE;
}

/*
 * extract rsvp_session_ipv4 object
 */
boolean
rsvp_extract_session_ipv4 (ushort *length, rsvp_object ** object,
			   rsvp_session_ipv4 ** result)
{
    if(*length && RSVP_CLASS_SESSION == (*object)->class) {
	*result =(rsvp_session_ipv4 *)(*object);
	if(!rsvp_valid_session_ipv4(*result)) {
	    return FALSE;
	}
	*object = rsvp_next_nonnull_object(*object, length);
    }
    return TRUE;
}

/*
 * build a STYLE object
 */
void
rsvp_build_style (void *object, uchar session_style)
{
    rsvp_style *style = object;

    rsvp_build_object(style, RSVP_CLASS_STYLE,
		      RSVP_TYPE_IPV4, sizeof(rsvp_style));

    switch(session_style) {
    default:
	buginf("\nRSVP: invalid style generated");
	break;

    case RSVP_WF_OPTION:
    case RSVP_SE_OPTION:
    case RSVP_FF_OPTION:
	style->reservation_type = session_style;
	break;
    }
}

/*
 * valid STYLE object, returning TRUE if valid and FALSE otherwise
 */
boolean
rsvp_valid_style (rsvp_style *style_object)
{
    if(style_object->base.class != RSVP_CLASS_STYLE)
	return FALSE;
    if(style_object->base.type != RSVP_TYPE_IPV4) {
	RSVP_BUGINF(0, ("\nRSVP: style object type != 1"));
	return FALSE;
    }
    if(style_object->base.length != sizeof(rsvp_style)) {
	RSVP_BUGINF(0, ("\nRSVP: style size != %d", sizeof(rsvp_style)));
	return FALSE;
    }
    if(style_object->reservation_type & ~(RSVP_SHARING_MASK|RSVP_SCOPE_MASK)) {
	RSVP_BUGINF(0, ("\nRSVP: style reserved field != 0"));
	return FALSE;
    }

    style_object->reservation_type &= 0x00FFFFFF;
    switch(style_object->reservation_type) {
    case RSVP_WF_OPTION:
    case RSVP_FF_OPTION:
    case RSVP_SE_OPTION:
	return TRUE;
    default:
	RSVP_BUGINF(0, ("\nRSVP: style object has unknown style"));
	return FALSE;
    }
}

/*
 * extract rsvp_style object
 */
boolean
rsvp_extract_style (ushort *length, rsvp_object ** object,
		    rsvp_style ** result)
{
    if(*length && RSVP_CLASS_STYLE == (*object)->class) {
	*result =(rsvp_style *)(*object);
	if(!rsvp_valid_style(*result)) {
	    return FALSE;
	}
	*object = rsvp_next_nonnull_object(*object, length);
    }
    return TRUE;
}

/*
 * valid TIME_VALUES object, returning TRUE if valid and FALSE otherwise
 */
boolean
rsvp_valid_refresh (rsvp_refresh *refresh)
{
    if(refresh->base.class != RSVP_CLASS_TIME_VALUES)
	return FALSE;
    if(refresh->base.type != 1) {
	RSVP_BUGINF(0, ("\nRSVP: timer values object type != 1"));
	return FALSE;
    }
    if(refresh->base.length != sizeof(rsvp_refresh)) {
	if(refresh->base.length == 4 + sizeof(rsvp_refresh)) {
	    /* old code; ignore dead field */
	    refresh->base.length = sizeof(rsvp_refresh);
	} else {
	    RSVP_BUGINF(0, ("\nRSVP: timer value size != %d", 
			    sizeof(rsvp_error_ipv4)));
	    return FALSE;
	}
    }
    if(refresh->refresh_ms < 1000) {
	refresh->refresh_ms = 1000;
    }
    return TRUE;
}

/*
 * extract rsvp_refresh object
 */
boolean
rsvp_extract_refresh (ushort *length, rsvp_object ** object,
		      rsvp_refresh ** result)
{
    rsvp_object *next_object;
    if(*length && RSVP_CLASS_TIME_VALUES == (*object)->class) {
	*result =(rsvp_refresh *)(*object);
	next_object = rsvp_next_nonnull_object(*object, length);
	if(!rsvp_valid_refresh(*result)) {
	    return FALSE;
	}
	*object = next_object;
    }
    return TRUE;
}

/*
 * routines to assure we send messages with the correct list of objects in
 * the correct order
 */
void
rsvp_send_resv (idbtype *idb,
		rsvp_hop_ipv4 *hop, rsvp_session_ipv4 *session,
		rsvp_refresh *time_values, rsvp_policy *policy,
		rsvp_confirm_ipv4 *confirm,
		rsvp_scope_ipv4 *scope, rsvp_style *style,
		rsvp_flowspec *flowspec, rsvp_filter_ipv4 *filter)
{
    rsvp_hop_ipv4 next_hop;

    if(!ip_ouraddress(hop->neighbor)) {
	RSVP_RESV_BUGINF(session->destination,
			 ("\nRSVP: send reservation to %i about %i",
			  hop->neighbor, session->destination));
	rsvp_build_hop_ipv4(&next_hop, rsvp_ip_address(idb), hop->lif_handle);
	if(style->reservation_type == RSVP_WF_OPTION)
	    rsvp_send_fixed_message(RSVP_MSG_RESV, idb, hop->neighbor, 8,
				    session, &next_hop, time_values,
				    confirm, scope, policy, style, flowspec);

	else
	    rsvp_send_fixed_message(RSVP_MSG_RESV, idb, hop->neighbor, 9,
				    session, &next_hop, time_values, confirm,
				    scope, policy, style, flowspec, filter);
    }
}

void
rsvp_send_resv_error (idbtype *idb, 
		      ipaddrtype neighbor,
		      rsvp_hop_ipv4 *hop,
		      rsvp_session_ipv4 *session,
		      rsvp_error_ipv4 *error_spec,
		      rsvp_scope_ipv4 *scope,
		      rsvp_policy *policy,
		      rsvp_style *style,
		      rsvp_flowspec *flowspec,
		      rsvp_filter_ipv4 *filter)
{
    rsvp_hop_ipv4 next_hop;

    if(!ip_ouraddress(neighbor)) {
	RSVP_BUGINF(session->destination,
		   ("\nRSVP: send reservation error to %i about %i",
		     neighbor, session->destination));
	rsvp_build_hop_ipv4(&next_hop, rsvp_ip_address(idb), hop->lif_handle);
	if(style->reservation_type == RSVP_WF_OPTION)
	    rsvp_send_fixed_message(RSVP_MSG_RERR, idb, neighbor, 7,
				    session, &next_hop, error_spec,
				    scope, policy, style, flowspec);
	else
	    rsvp_send_fixed_message(RSVP_MSG_RERR, idb, neighbor, 8,
				    session, &next_hop, error_spec,
				    scope, policy, style, flowspec, filter);
    }
}

void
rsvp_send_resv_confirm (idbtype *idb, ipaddrtype neighbor,
			rsvp_session_ipv4 *session,
			rsvp_error_ipv4 *error_spec,
			rsvp_confirm_ipv4 *confirm,
			rsvp_style *style,
			rsvp_flowspec *flowspec,
			rsvp_filter_ipv4 *filter)
{
    if (!ip_ouraddress(neighbor)) {
	RSVP_RESV_BUGINF(session->destination,
			 ("\nRSVP: send reservation confirm to %i about %i",
			  neighbor, session->destination));
	if (style->reservation_type == RSVP_WF_OPTION)
	    rsvp_send_fixed_message(RSVP_MSG_CONFIRM, idb, neighbor, 5,
				    session, error_spec, confirm,
				    style, flowspec);
	else
	    rsvp_send_fixed_message(RSVP_MSG_CONFIRM, idb, neighbor, 6,
				    session, error_spec, confirm,
				    style, flowspec, filter);
    }
}

void
rsvp_send_resv_teardown (idbtype *idb,
			 rsvp_hop_ipv4 *hop,
			 rsvp_session_ipv4 *session,
			 rsvp_scope_ipv4 *scope,
			 rsvp_style *style,
			 rsvp_flowspec *flowspec,
			 rsvp_filter_ipv4 *filter)
{
    rsvp_hop_ipv4 next_hop;

    if (!ip_ouraddress(hop->neighbor)) {
	RSVP_RESV_BUGINF(session->destination,
			 ("\nRSVP: send reservation teardown to %i about %i",
			  hop->neighbor, session->destination));
	rsvp_build_hop_ipv4(&next_hop, rsvp_ip_address(idb), hop->lif_handle);
	if(style->reservation_type == RSVP_WF_OPTION)
	    rsvp_send_fixed_message(RSVP_MSG_RTEAR, idb, hop->neighbor, 5,
				    session, &next_hop, scope, style,
				    flowspec);
	else
	    rsvp_send_fixed_message(RSVP_MSG_RTEAR, idb, hop->neighbor, 6,
				    session, &next_hop, scope, style,
				    flowspec, filter);
    }
}

/*
 * up
 */
static void
rsvp_update_adspec_params(void *params, ushort words,
			  hwidbtype *hwidb, idbtype *output,
			  ulong interface_rate, ulong mtu)
{
    intsrv_integer_object *integer;
    intsrv_real_object *real;

    integer = params;
    for (; words > 1; words -= 2) {
	real = params;

	switch (integer->parameter_id) {
	case INTSRV_NON_IS_HOPS:
	    break;
	case INTSRV_COMPOSED_NON_IS_HOPS:
	    break;
	case INTSRV_IS_HOPS:
	    integer->value = 1;
	    break;
	case INTSRV_COMPOSED_IS_HOPS:
	    integer->value++;
	    break;
	case INTSRV_PATH_BANDWIDTH:
	    real->value = long_to_ieee(interface_rate/8);
	    break;
	case INTSRV_MIN_PATH_BANDWIDTH:
	    if (interface_rate < ieee_to_long(real->value)/8)
		real->value = long_to_ieee(interface_rate/8);
	    break;
	case INTSRV_IF_LATENCY:
	    break;
	case INTSRV_PATH_LATENCY:
	    break;
	case INTSRV_MTU:
	    integer->value = mtu;
	    break;
	case INTSRV_COMPOSED_MTU:
	    integer->value = min(integer->value, mtu);
	    break;
	case INTSRV_E2E_DELAY:
	    integer->value += mtu*hwidb->mci_txcount;
	    if (integer->value > PLUS_INFINITY)
		integer->value = PLUS_INFINITY;
	    break;
	case INTSRV_E2E_MAX_JITTER:
	    integer->value += mtu*8000/output->visible_bandwidth;
	    if (integer->value > PLUS_INFINITY)
		integer->value = PLUS_INFINITY;
	    break;
	case INTSRV_SHP_DELAY:
	    integer->value += mtu*hwidb->mci_txcount;
	    if (integer->value > PLUS_INFINITY)
		integer->value = PLUS_INFINITY;
	    break;
	case INTSRV_SHP_MAX_JITTER:
	    integer->value += mtu*8000/output->visible_bandwidth;
	    if (integer->value > PLUS_INFINITY)
		integer->value = PLUS_INFINITY;
	    break;
	}

	integer++;
	params = integer;
    }
}

/*
 *	rsvp_send_path
 *
 *	in sending a path message to an interface, several components
 *	must be doctored - the tspec and adspec are updated to reflect
 *	the capacity of the interface, and the hop object is given the
 *	address of the interface itself.
 *
 *	note that the routine may autonomously decide not to send the
 *	message at all, if it would be inapropriate for anyone to make
 *	a reservation on the basis of the path message. Information
 *	about this shows up in debug statements.
 */
void
rsvp_send_path (idbtype *output, uchar ttl,
		rsvp_session_ipv4 *session, rsvp_refresh *time_values,
		rsvp_template_ipv4 *template, rsvp_tspec *in_tspec,
		rsvp_policy *policy, rsvp_adspec *advertisement,
		boolean use_ra)
{
    hwidbtype *hwidb;
    rsvp_hop_ipv4 hop;
    rsvp_tspec    tspec;
    union {
	char space[128];
	rsvp_adspec   adspec;
    } p;
    ulong	  mean_rate;
    ulong	  depth;
    ulong	  peak_rate;
    ulong	  interface_rate;
    ulong	  mtu;
    ulong	  queue_limit;
    intsrv_gtd_adspec   *gtd;
    intsrv_cload_adspec *cload;
    void         *goo;
    void         *max_adspec;

    if (ip_ouraddress(session->destination)) 
	return;

    mean_rate = rsvp_get_average_bit_rate(in_tspec);
    depth = rsvp_get_maximum_burst(in_tspec);
    peak_rate = rsvp_get_peak(in_tspec);
    interface_rate = 1000*output->visible_bandwidth;
    mtu = max_proto_mtu(output, LINK_IP);
    hwidb = hwidb_or_null(output);
    queue_limit = maximum_interface_queue_depth(hwidb);

    if (mean_rate > interface_rate) {
	RSVP_PATH_BUGINF(session->destination,
			 ("\nRSVP: PATH not sent on %s:"
			  " advertised mean rate exceeds interface rate",
			  output->namestring));
	return;
    }

    /*
     * We need to skip the next test for loopback interfaces since its
     * output queue depth is "0", and would always fail the following 
     * check.  
     * The loopback interface is often used to create static senders.
     */

    if (hwidb && hwidb->type != IDBTYPE_LB) {
      if (depth > 8*mtu*queue_limit) {
	RSVP_PATH_BUGINF(session->destination,
			 ("\nRSVP: PATH not sent on %s:"
			  " advertised burst size (%i)exceeds queue"
			  " capacity (%i)", output->namestring, depth,
			  8*mtu*queue_limit));
	return;
      }
    } 

    if (!advertisement->base.length)
	p.adspec.base.length = 0;
    else {
	rsvp_object_copy(&p.adspec, advertisement);
	max_adspec = rsvp_next_object(&p.adspec.base);

	rsvp_update_adspec_params(p.adspec.ad.gtd.general.first_object,
				  p.adspec.ad.gtd.general.service_length,
				  hwidb, output, interface_rate, mtu);

	gtd = &p.adspec.ad.gtd.gtd;
	if (gtd->service_id != INTSRV_GTD)
	    cload = goo = gtd;
	else {
	    goo = gtd;
	    goo = 4*(1 + gtd->service_length) + (char *) goo;
	    cload = goo;
	    if (hwidb) {
		if (!gtd->service_length) {

		    /*
		     * if null guaranteed spec, the specification says
		     * we must fill in these objects. What a pain!
		     * If there is CLOAD info following the GS, we need
		     * to move the CLOAD info down by 8 *words*, to make
		     * way for the additional GS info that we're going
		     * to add. Once we do that, we reset cload to point
		     * to the right spot.
		     */
		    if (goo < max_adspec &&
			cload->service_id == INTSRV_CLOAD) {
			memcpy((char *)cload + (4 * RSVP_MIN_GTD_SPEC_LENGTH),
			       cload, 
			       4 * (1 + cload->service_length));
			cload = (intsrv_cload_adspec *)
			     ((char *)cload + (4 * RSVP_MIN_GTD_SPEC_LENGTH));
		    }
		    gtd->break_bit = RSVP_ADSPEC_BREAK;
		    gtd->service_length = RSVP_MIN_GTD_SPEC_LENGTH;
		    rsvp_build_integer_object(&gtd->ctot,
					      INTSRV_E2E_DELAY,
					      0);
		    rsvp_build_integer_object(&gtd->dtot,
					      INTSRV_E2E_MAX_JITTER,
					      0);
		    rsvp_build_integer_object(&gtd->csum,
					      INTSRV_SHP_DELAY,
					      0);
		    rsvp_build_integer_object(&gtd->dsum,
					      INTSRV_SHP_MAX_JITTER,
					      0);
		    p.adspec.base.length += (gtd->service_length * 4);
		    p.adspec.ad.gtd.length_in_words += gtd->service_length;
		}
		rsvp_update_adspec_params(&gtd->first_object,
					  gtd->service_length,
					  hwidb, output, interface_rate, mtu);
	    }
	}

	if (hwidb && cload->service_length) {
	    rsvp_update_adspec_params(cload->first_object,
				      cload->service_length,
				      hwidb, output, interface_rate, mtu);
	}
    }

    RSVP_PATH_BUGINF(session->destination,
		     ("\nRSVP: send path multicast about %i on %s",
		      session->destination, 
		      (use_ra) ? output->namestring : "null"));

    rsvp_build_hop_ipv4(&hop, rsvp_ip_address(output), NULL);
    rsvp_build_tspec(&tspec, mean_rate, depth,
		     min(interface_rate, peak_rate),
		     in_tspec->t.gtd.min_unit,
		     in_tspec->t.gtd.max_unit);

    rsvp_send_fixed_multicast(RSVP_MSG_PATH, use_ra, template->source,
			      output, ttl, 7, session, &hop,
			      time_values, policy, template, &tspec,
			      &p.adspec);
}

void
rsvp_send_path_error (idbtype * idb, ipaddrtype neighbor,
		      rsvp_session_ipv4 * session,
		      rsvp_error_ipv4 * error_spec,
		      rsvp_template_ipv4 * template,
		      rsvp_tspec * tspec,
		      rsvp_policy * policy,
		      rsvp_adspec * advertisement)
{
    if (!ip_ouraddress(neighbor)) {
	RSVP_PATH_BUGINF(session->destination,
			 ("\nRSVP: send path error to %i about %i",
			  neighbor, session->destination));
	rsvp_send_fixed_message(RSVP_MSG_PERR, idb, neighbor, 6,
				session, error_spec, policy, template,
				tspec, advertisement);
    }
}

void
rsvp_send_path_teardown (idbtype *output, uchar ttl,
			 rsvp_session_ipv4 *session,
			 rsvp_template_ipv4 *template,
			 rsvp_tspec *tspec,
			 rsvp_adspec *advertisement)
{
    rsvp_hop_ipv4 hop;

    if (!ip_ouraddress(session->destination)) {
	RSVP_PATH_BUGINF(session->destination,
			 ("\nRSVP: send path teardown multicast about "
			  "%i on %s",
			  session->destination, output->namestring));
	rsvp_build_hop_ipv4(&hop, rsvp_ip_address(output), NULL);
	rsvp_send_fixed_multicast(RSVP_MSG_PTEAR, TRUE, template->source,
				  output, ttl, 5, session, &hop, template, 
				  tspec, advertisement);
    }
}

/*
 *Build a Guaranteed Service FLOWSPEC Object
 */
static inline void
build_intsrv_gtd_flowspec (rsvp_flowspec *flowspec,
			   ulong rate,
			   ulong depth,
			   ulong peak,
			   ulong min_unit,
			   ulong max_unit,
			   ulong requested_rate,
			   ulong slack,
			   ushort composed_mtu)
{
    rsvp_build_object(&flowspec->base, RSVP_CLASS_FLOWSPEC, INTSRV,
		      sizeof(rsvp_object) + sizeof(intsrv_gtd_flowspec));
    flowspec->flow.gtd.version = 0;
    flowspec->flow.gtd.length_in_words = sizeof(intsrv_gtd_flowspec)/4 - 1;
    flowspec->flow.gtd.service_id = INTSRV_GTD;
    flowspec->flow.gtd.fill = 0;
    flowspec->flow.gtd.service_length = sizeof(intsrv_gtd_flowspec)/4 - 2;

    flowspec->flow.gtd.parameter_id_tspec = INTSRV_TOKEN_BUCKET_TSPEC;
    flowspec->flow.gtd.flags_tspec = 0;
    flowspec->flow.gtd.param1_length = 5;
    flowspec->flow.gtd.rate = long_to_ieee(rate);
    flowspec->flow.gtd.depth = long_to_ieee(depth);
    flowspec->flow.gtd.peak = long_to_ieee(peak);
    flowspec->flow.gtd.min_unit = min_unit;
    flowspec->flow.gtd.max_unit = max_unit;

    flowspec->flow.gtd.parameter_id_rspec = INTSRV_GTD_RSPEC;
    flowspec->flow.gtd.flags_rspec = 0;
    flowspec->flow.gtd.param2_length = 2;
    flowspec->flow.gtd.requested_rate = long_to_ieee(requested_rate);
    flowspec->flow.gtd.slack = slack;
}

/*
 * Build a Controlled Load FLOWSPEC Object
 */
static inline void
build_intsrv_cload_flowspec (rsvp_flowspec *flowspec,
			     ulong rate,
			     ulong depth,
			     ulong min_unit,
			     ulong max_unit)
{
    rsvp_build_object(&flowspec->base, RSVP_CLASS_FLOWSPEC, INTSRV,
		      sizeof(rsvp_object) + sizeof(intsrv_cload_flowspec));
    flowspec->flow.cload.version = 0;
    flowspec->flow.cload.length_in_words = sizeof(intsrv_cload_flowspec)/4 - 1;
    flowspec->flow.cload.service_id = INTSRV_CLOAD;
    flowspec->flow.cload.fill = 0;
    flowspec->flow.cload.service_length = sizeof(intsrv_cload_flowspec)/4 - 2;
    flowspec->flow.cload.parameter_id_tspec = INTSRV_TOKEN_BUCKET_TSPEC;
    flowspec->flow.cload.flags_tspec = 0;
    flowspec->flow.cload.param_length = sizeof(intsrv_cload_flowspec)/4 - 3;
    flowspec->flow.cload.rate = long_to_ieee(rate);
    flowspec->flow.cload.depth = long_to_ieee(depth);
    flowspec->flow.cload.peak = IEEE_INFINITY;
    flowspec->flow.cload.min_unit = min_unit;
    flowspec->flow.cload.max_unit = max_unit;
}

/*
 * build a FLOWSPEC object
 */
void
rsvp_build_flowspec (void *object,
		     enum qos_service_type qos_service,
		     ulong maximum_burst,
		     ulong average_bit_rate,
		     ulong maximum_e2e_delay,
		     ulong service_level,
		     ulong min_unit,
		     ulong max_unit,
		     ulong peak,
		     ulong requested_rate,
		     ulong slack,
		     ushort composed_mtu)
{
    rsvp_flowspec *flowspec = object;

    switch(qos_service) {
    default:
	/*
	 * don't build one if you don't know how 
	 */
	flowspec->base.length = 0;
	break;
    case QOS_GUARANTEED:
	build_intsrv_gtd_flowspec(flowspec, average_bit_rate/8,
    				  maximum_burst/8, peak/8, min_unit,
    				  max_unit, requested_rate/8, slack,
    				  composed_mtu);
	break;
    case QOS_CONTROLLED_LOAD:
	build_intsrv_cload_flowspec(flowspec, average_bit_rate/8,
    				    maximum_burst/8, min_unit,
    				    max_unit);
	break;
    }
}


/*
 * Validate a Guaranteed Service FLOWSPEC Object
 */
static boolean
valid_intsrv_gtd_flowspec (rsvp_flowspec *flowspec)
{
    if(flowspec->base.length !=
       sizeof(rsvp_object) + sizeof(intsrv_gtd_flowspec)) {
	RSVP_BUGINF(0, ("\nRSVP: IS Guaranteed Flow Spec wrong length"));
	return FALSE;
    }
    if(flowspec->flow.gtd.version != 0 ||
       flowspec->flow.gtd.length_in_words != sizeof(intsrv_gtd_flowspec)/4 - 1 ||
       flowspec->flow.gtd.service_id != INTSRV_GTD ||
       flowspec->flow.gtd.fill != 0 ||
       flowspec->flow.gtd.service_length != sizeof(intsrv_gtd_flowspec)/4 - 2 ||
       flowspec->flow.gtd.parameter_id_tspec != INTSRV_TOKEN_BUCKET_TSPEC ||
       flowspec->flow.gtd.flags_tspec != 0 ||
       flowspec->flow.gtd.param1_length != 5 ||
       flowspec->flow.gtd.parameter_id_rspec != INTSRV_GTD_RSPEC ||
       flowspec->flow.gtd.flags_rspec != 0 ||
       flowspec->flow.gtd.param2_length != 2) {
	RSVP_BUGINF(0, ("\nRSVP: IS Guaranteed Flow Spec wrong format"));
	return FALSE;
    }
    if(IEEE_INFINITY == flowspec->flow.gtd.rate ||
	ieee_to_long(flowspec->flow.gtd.rate) < 0) {
	RSVP_BUGINF(0, ("\nRSVP: IS Controlled Load Flow Spec bad average rate"));
	return FALSE;
    }
    if(IEEE_INFINITY == flowspec->flow.gtd.depth ||
	ieee_to_long(flowspec->flow.gtd.depth) < 0) {
	RSVP_BUGINF(0, ("\nRSVP: IS Controlled Load Flow Spec bad burst size"));
	return FALSE;
    }
    if(ieee_to_long(flowspec->flow.gtd.peak) < 0) {
	RSVP_BUGINF(0, ("\nRSVP: IS Controlled Load Flow Spec bad peak rate"));
	return FALSE;
    }
    if(IEEE_INFINITY == flowspec->flow.gtd.requested_rate ||
	ieee_to_long(flowspec->flow.gtd.requested_rate) < 0) {
	RSVP_BUGINF(0, ("\nRSVP: IS Controlled Load Flow Spec bad requested rate"));
	return FALSE;
    }
    return TRUE;
}

/*
 * Validate a Controlled Load FLOWSPEC Object
 */
static boolean
valid_intsrv_cload_flowspec (rsvp_flowspec *flowspec)
{
    if(flowspec->base.length !=
       sizeof(rsvp_object) + sizeof(intsrv_cload_flowspec)) {
	RSVP_BUGINF(0, ("\nRSVP: IS Controlled Load Flow Spec wrong length"));
	return FALSE;
    }
    if(flowspec->flow.cload.version != 0 ||
       flowspec->flow.cload.length_in_words != sizeof(intsrv_cload_flowspec)/4 - 1 ||
       flowspec->flow.cload.service_id != INTSRV_CLOAD ||
       flowspec->flow.cload.fill != 0 ||
       flowspec->flow.cload.service_length != sizeof(intsrv_cload_flowspec)/4 - 2 ||
       flowspec->flow.cload.parameter_id_tspec != INTSRV_TOKEN_BUCKET_TSPEC ||
       flowspec->flow.cload.flags_tspec != 0 ||
       flowspec->flow.cload.param_length != sizeof(intsrv_cload_flowspec)/4 - 3) {
	RSVP_BUGINF(0, ("\nRSVP: IS Controlled Load Flow Spec incorrect format"));
	return FALSE;
    }
    if(IEEE_INFINITY == flowspec->flow.cload.rate ||
	ieee_to_long(flowspec->flow.cload.rate) < 0) {
	RSVP_BUGINF(0, ("\nRSVP: IS Controlled Load Flow Spec bad rate"));
	return FALSE;
    }
    if(IEEE_INFINITY == flowspec->flow.cload.depth ||
	ieee_to_long(flowspec->flow.cload.depth) < 0) {
	RSVP_BUGINF(0, ("\nRSVP: IS Controlled Load Flow Spec bad burst size"));
	return FALSE;
    }
    if(ieee_to_long(flowspec->flow.cload.peak) < 0) {
	RSVP_BUGINF(0, ("\nRSVP: IS Controlled Load Sender T Spec bad peak rate"));
	return FALSE;
    }
    return TRUE;
}

/*
 * Validate a FLOWSPEC object
 */
boolean
rsvp_valid_flowspec (rsvp_flowspec *flowspec)
{
    if(flowspec->base.class != RSVP_CLASS_FLOWSPEC)
	return FALSE;
    switch(flowspec->base.type) {
    default:
	RSVP_BUGINF(0, ("\nRSVP: Unrecognized QOS Service"));
	return FALSE;

    case INTSRV:
	switch(flowspec->flow.gtd.service_id) {
	default:
	    RSVP_BUGINF(0, ("\nRSVP FLOW SPEC:"
			    " Unrecognized Integrated Services QOS Service"));
	    return FALSE;
	case INTSRV_GTD:
	    return valid_intsrv_gtd_flowspec(flowspec);
	case INTSRV_CLOAD:
	    return valid_intsrv_cload_flowspec(flowspec);
	}
    }
}

/*
 * build an ADSPEC object
 */
void
rsvp_build_adspec (void *object)
{
    rsvp_adspec *adspec = object;
    intsrv_template_adspec *template = &adspec->ad.gtd;

    rsvp_build_object(&adspec->base, RSVP_CLASS_ADSPEC, INTSRV,
		      sizeof(rsvp_object) + sizeof(intsrv_template_adspec));
    template->version = 0;
    template->length_in_words = sizeof(intsrv_template_adspec)/4-1;

    template->general.service_id = INTSRV_GENERAL;
    template->general.break_bit = 0;
    template->general.service_length = sizeof(template->general)/4-1;
    rsvp_build_integer_object(&template->general.number_is_hops,
			      INTSRV_COMPOSED_IS_HOPS, 0);
    rsvp_build_real_object(&template->general.bandwidth,
			   INTSRV_MIN_PATH_BANDWIDTH, IEEE_INFINITY);
    rsvp_build_real_object(&template->general.latency,
			   INTSRV_PATH_LATENCY, 0);
    rsvp_build_integer_object(&template->general.path_mtu,
			      INTSRV_COMPOSED_MTU, ~0);

    template->gtd.service_id = INTSRV_GTD;
    template->gtd.break_bit = 0;
    template->gtd.service_length = sizeof(template->gtd)/4-1;
    rsvp_build_integer_object(&template->gtd.ctot, INTSRV_E2E_DELAY, 0);
    rsvp_build_integer_object(&template->gtd.dtot, INTSRV_E2E_MAX_JITTER, 0);
    rsvp_build_integer_object(&template->gtd.csum, INTSRV_SHP_DELAY, 0);
    rsvp_build_integer_object(&template->gtd.dsum, INTSRV_SHP_MAX_JITTER, 0);

    template->cload.service_id = INTSRV_CLOAD;
    template->cload.break_bit = 0;
    template->cload.service_length = sizeof(template->cload)/4-1;
}

/*
 * Validate a ADSPEC object
 */
boolean
rsvp_valid_adspec (rsvp_adspec *adspec)
{
    if(adspec->base.class != RSVP_CLASS_ADSPEC){
        RSVP_BUGINF(0,("\nInvalid class (%d)", adspec->base.class));
        return FALSE;
    } 

    if (INTSRV != adspec->base.type) {
	RSVP_BUGINF(0, ("\nRSVP: Unrecognized QOS Service"));
	return FALSE;
    }

    if (adspec->ad.gtd.version != 0) {
        RSVP_BUGINF(0,("\nInvalid version (%d)",adspec->ad.gtd.version));
	return FALSE;
    }

    if (adspec->ad.gtd.general.service_id != INTSRV_GENERAL){
        RSVP_BUGINF(0,("\nInvalid general service ID (%d)", 
		       adspec->ad.gtd.general.service_id));
	return FALSE;
    }

    if (adspec->ad.gtd.general.service_length
	!= sizeof(adspec->ad.gtd.general)/4-1) {
        RSVP_BUGINF(0,("\nInvalid length %d", 
		       adspec->ad.gtd.general.service_length));
	return FALSE;
    }

    /*
     * If no GS fragment present, make sure it's a CL fragment.
     */
    if (adspec->ad.gtd.gtd.service_id != INTSRV_GTD) {
	if (adspec->ad.gtd.gtd.service_id != INTSRV_CLOAD) {
	    RSVP_BUGINF(0,("\nInvalid service ID (%d) - "
			   "expecting CL info (no GS info)", 
			   adspec->ad.gtd.gtd.service_id));
	    return FALSE;
	}
    }
    else {
        /*
	 * GS fragment is present; check if there is a fragment 
	 * following the GS fragment, and if there is, check if it's
	 * a CL fragment.
	 */
        ushort gen_and_gs_len;
	void *temp;
	intsrv_cload_adspec *cload;

	gen_and_gs_len = adspec->ad.gtd.general.service_length + 1 +
	                 adspec->ad.gtd.gtd.service_length + 1;

        if (adspec->ad.gtd.length_in_words > gen_and_gs_len) {
	    temp = 4 * (adspec->ad.gtd.gtd.service_length + 1) +
	           (char *)(&adspec->ad.gtd.gtd);
	    cload = temp;
            if (cload->service_id != INTSRV_CLOAD) {
	        RSVP_BUGINF(0,("\nInvalid service ID (%d) - "
			       "expecting CL info ", 
			       cload->service_id));
		return FALSE;
	    }
	}
    }
        
    return TRUE;
}

/*
 * Build an Integrated Services SENDER_TSPEC Object
 */
void
rsvp_build_tspec (void *object,
		ulong rate,
		ulong depth,
		ulong peak,
		ulong min_unit,
		ulong max_unit)
{
    rsvp_tspec *tspec = object;

    rsvp_build_object(&tspec->base, RSVP_CLASS_SENDER_TSPEC, INTSRV,
		      sizeof(rsvp_object) + sizeof(intsrv_tspec));
    tspec->t.gtd.version = 0;
    tspec->t.gtd.length_in_words = sizeof(intsrv_tspec)/4 - 1;

    tspec->t.gtd.service_id = INTSRV_GENERAL;
    tspec->t.gtd.fill = 0;
    tspec->t.gtd.service_length = sizeof(intsrv_tspec)/4 - 2;

    tspec->t.gtd.parameter_id_tspec = INTSRV_TOKEN_BUCKET_TSPEC;
    tspec->t.gtd.flags_tspec = 0;
    tspec->t.gtd.parameter_length = sizeof(intsrv_tspec)/4-3;

    tspec->t.gtd.rate = long_to_ieee(rate/8);
    tspec->t.gtd.depth = long_to_ieee(depth/8);
    tspec->t.gtd.peak = long_to_ieee(peak/8);
    tspec->t.gtd.min_unit = min_unit;
    tspec->t.gtd.max_unit = max_unit;
}

/*
 * Validate a Integrated Services SENDER_TSPEC Object
 */
static boolean
valid_intsrv_tspec (rsvp_tspec *tspec)
{
    if(tspec->base.length !=
       sizeof(rsvp_object) + sizeof(intsrv_tspec)) {
	RSVP_BUGINF(0, ("\nRSVP: IS Controlled Load Sender T Spec wrong length"));
	return FALSE;
    }
    if (tspec->t.gtd.version != 0 ||
	tspec->t.gtd.length_in_words != sizeof(intsrv_tspec)/4 - 1 ||
	tspec->t.gtd.service_id != INTSRV_GENERAL ||
	tspec->t.gtd.fill != 0 ||
	tspec->t.gtd.service_length != sizeof(intsrv_tspec)/4 - 2 ||
	tspec->t.gtd.parameter_id_tspec != INTSRV_TOKEN_BUCKET_TSPEC ||
	tspec->t.gtd.flags_tspec != 0 ||
	tspec->t.gtd.parameter_length != sizeof(intsrv_tspec)/4-3) {
	RSVP_BUGINF(0, ("\nRSVP: IS Controlled Load Sender T Spec incorrect format"));
	return FALSE;
    }
    if(IEEE_INFINITY == tspec->t.cload.rate ||
       ieee_to_long(tspec->t.cload.rate) < 0) {
	RSVP_BUGINF(0, ("\nRSVP: IS Controlled Load Sender T Spec bad rate"));
	return FALSE;
    }
    if(IEEE_INFINITY == tspec->t.cload.depth ||
       ieee_to_long(tspec->t.cload.depth) < 0) {
	RSVP_BUGINF(0, ("\nRSVP: IS Controlled Load Sender T Spec bad burst size"));
	return FALSE;
    }
    if(ieee_to_long(tspec->t.cload.peak) < 0) {
	RSVP_BUGINF(0, ("\nRSVP: IS Controlled Load Sender T Spec bad peak rate"));
	return FALSE;
    }
    return TRUE;
}

/*
 * Validate a SENDER_TSPEC object
 */
boolean
rsvp_valid_tspec (rsvp_tspec *tspec)
{
    if(tspec->base.class != RSVP_CLASS_SENDER_TSPEC)
	return FALSE;
    switch(tspec->base.type) {
    default:
	RSVP_BUGINF(0, ("\nRSVP: Unrecognized QOS Service"));
	return FALSE;

    case INTSRV:
	switch(tspec->t.gtd.service_id) {
	default:
	    RSVP_BUGINF(0, ("\nRSVP TSPEC : Unrecognized Integrated Services QOS Service"));
	    return FALSE;
	case INTSRV_GENERAL:
	    return valid_intsrv_tspec(tspec);
	}
    }
}

/*
 * Service Routines to get various parameters
 */
ulong
rsvp_get_average_bit_rate (rsvp_tspec *tspec)
{
    switch(tspec->base.type) {
    default:
	return 0;
    case INTSRV:
	switch(tspec->t.gtd.service_id) {
	default:
	    return 0;
	case INTSRV_GENERAL:
	    return 8 * ieee_to_long(tspec->t.gtd.rate);
	}
    }
}

ulong
rsvp_get_maximum_burst (rsvp_tspec *tspec)
{
	switch(tspec->t.gtd.service_id) {
	default:
	    return 0;
	case INTSRV_GENERAL:
	    return 8 * ieee_to_long(tspec->t.gtd.depth);
	}
}
ulong
rsvp_get_peak (rsvp_tspec *tspec)
{
    switch(tspec->t.gtd.service_id) {
    default:
	return 0;
    case INTSRV_GTD:
    case INTSRV_GENERAL:
    case INTSRV_CLOAD:
	return 8 * ieee_to_long(tspec->t.cload.peak);
    }
}

ulong
rsvp_get_flow_average_bit_rate (rsvp_flowspec *flowspec)
{
	switch(flowspec->flow.gtd.service_id) {
	default:
	    return 0;
	case INTSRV_GTD:
    case INTSRV_GENERAL:
	case INTSRV_CLOAD:
	    return 8 * ieee_to_long(flowspec->flow.cload.rate);
	}
}

ulong
rsvp_get_flow_maximum_burst (rsvp_flowspec *flowspec)
{
	switch(flowspec->flow.gtd.service_id) {
	default:
	    return 0;
	case INTSRV_GTD:
    case INTSRV_GENERAL:
	case INTSRV_CLOAD:
	    return 8 * ieee_to_long(flowspec->flow.cload.depth);
	}
}

ulong
rsvp_get_flow_maximum_e2e_delay (rsvp_flowspec *flowspec)
{
    return PLUS_INFINITY;
}

ulong
rsvp_get_flow_service_level (rsvp_flowspec *flowspec)
{
    return 1;
}

ushort
rsvp_get_flow_min_unit (rsvp_flowspec *flowspec)
{
	switch(flowspec->flow.gtd.service_id) {
	default:
	    return 0;
	case INTSRV_GTD:
	    return flowspec->flow.gtd.min_unit;
	case INTSRV_CLOAD:
	    return flowspec->flow.cload.min_unit;
	}
}

ushort
rsvp_get_flow_max_unit (rsvp_flowspec *flowspec)
{
	switch(flowspec->flow.gtd.service_id) {
	default:
	    return ~0;
    case INTSRV_GENERAL:
	case INTSRV_GTD:
	case INTSRV_CLOAD:
	    return flowspec->flow.cload.max_unit;
	}
}

ulong
rsvp_get_flow_peak (rsvp_flowspec *flowspec)
{
    if (INTSRV_GTD == flowspec->flow.gtd.service_id ||
	INTSRV_GENERAL == flowspec->flow.gtd.service_id ||
	INTSRV_CLOAD == flowspec->flow.gtd.service_id) {
	return 8 * ieee_to_long(flowspec->flow.cload.peak);
    } else {
	return 0;
    }
}

ulong
rsvp_get_flow_requested_rate (rsvp_flowspec *flowspec)
{
	switch(flowspec->flow.gtd.service_id) {
	default:
	    return 0;
	case INTSRV_GTD:
	    return 8 * ieee_to_long(flowspec->flow.gtd.requested_rate);
    case INTSRV_GENERAL:
	case INTSRV_CLOAD:
	    return 8 * ieee_to_long(flowspec->flow.cload.rate);
	}
}

ulong
rsvp_get_flow_slack (rsvp_flowspec *flowspec)
{
	switch(flowspec->flow.gtd.service_id) {
	default:
	    return 0;
	case INTSRV_GTD:
	    return flowspec->flow.gtd.slack;
	case INTSRV_CLOAD:
	    return 0;
	}
}

ushort
rsvp_get_flow_composed_mtu (rsvp_flowspec *flowspec)
{
	switch(flowspec->flow.gtd.service_id) {
	default:
	    return 0;
	case INTSRV_GTD:
	    return ~0;
	case INTSRV_CLOAD:
	    return ~0;
	}
}

enum qos_service_type
rsvp_get_flow_qos_service (rsvp_flowspec *flowspec)
{
	switch(flowspec->flow.gtd.service_id) {
	default:
	    return QOS_TSPEC;
	case INTSRV_GTD:
	    return QOS_GUARANTEED;
	case INTSRV_CLOAD:
	    return QOS_CONTROLLED_LOAD;
	}
}

void rsvp_display_flowspec(void *object)
{
    rsvp_flowspec *flowspec = object;

    buginf("\n  version = %d"
	   "    length in words = %d"
	   "\n  service id = %d"
	   "    service length = %d"
	   "\n  tspec parameter id = %d"
	   "    tspec flags = %d"
	   "    tspec length = %d"
	   "\n  average rate = %d bytes/sec"
	   "    burst depth = %d bytes"
	   "    peak rate = %d bytes/sec"
	   "\n  min unit = %d bytes"
	   "    max unit = %d bytes",

	   flowspec->flow.gtd.version>>14,
	   flowspec->flow.gtd.length_in_words,
	   flowspec->flow.gtd.service_id,
	   flowspec->flow.gtd.service_length,
	   flowspec->flow.gtd.parameter_id_tspec,
	   flowspec->flow.gtd.flags_tspec,
	   flowspec->flow.gtd.param1_length,
	   ieee_to_long(flowspec->flow.gtd.rate),
	   ieee_to_long(flowspec->flow.gtd.depth),
	   ieee_to_long(flowspec->flow.gtd.peak),
	   flowspec->flow.gtd.min_unit,
	   flowspec->flow.gtd.max_unit);

    if (flowspec->flow.gtd.length_in_words == sizeof(intsrv_gtd_flowspec)/4-1)
	buginf("\nrspec parameter id=%d"
	       "  rspec flags=%d"
	       "  rspec length=%d"
	       "\nrequested rate=%d"
	       "  slack=%d",

	       flowspec->flow.gtd.parameter_id_rspec,
	       flowspec->flow.gtd.flags_rspec,
	       flowspec->flow.gtd.param2_length,
	       ieee_to_long(flowspec->flow.gtd.requested_rate),
	       flowspec->flow.gtd.slack);
}

void rsvp_display_tspec(void *object)
{
    rsvp_tspec *tspec = object;

    buginf("\nversion=%d"
	   "  length in words=%d"
	   "\nservice id=%d"
	   "  service length=%d"
	   "\nparameter id=%d"
	   "  flags=%d"
	   "  parameter length=%d"
	   "\naverage rate=%d bytes/sec"
	   "  burst depth=%d bytes"
	   "  peak rate=%d bytes/sec"
	   "\nmin unit=%d bytes"
	   "  max unit=%d bytes",

	   tspec->t.gtd.version>>14,
	   tspec->t.gtd.length_in_words,
	   tspec->t.gtd.service_id,
	   tspec->t.gtd.service_length,
	   tspec->t.gtd.parameter_id_tspec,
	   tspec->t.gtd.flags_tspec,
	   tspec->t.gtd.parameter_length,
	   ieee_to_long(tspec->t.gtd.rate),
	   ieee_to_long(tspec->t.gtd.depth),
	   ieee_to_long(tspec->t.gtd.peak),
	   tspec->t.gtd.min_unit,
	   tspec->t.gtd.max_unit);
}

/*
 * up
 */
static void *
rsvp_display_adspec_part (intsrv_adspec_general *part, char *name)
{
    ushort words;
    void *params;
    intsrv_integer_object *integer;
    intsrv_real_object *real;
    ulong value = 0;

    buginf("\n %s"
	   "  break bit=%d"
	   "  service length=%d",
	   name,
	   part->break_bit>>7,
	   part->service_length);

    params = part->first_object;
    words = part->service_length;
    integer = params;
    for (; words > 1; words -= 2) {
	real = params;
	switch (integer->parameter_id) {
	default:
	    name = "unknown";
	    value = integer->value;
	    break;
	case INTSRV_NON_IS_HOPS:
	    name = "Non IS Hops";
	    value = integer->value;
	    break;
	case INTSRV_COMPOSED_NON_IS_HOPS:
	    name = "Composed Non IS Hops";
	    value = integer->value;
	    break;
	case INTSRV_IS_HOPS:
	    name = "IS Hops";
	    value = integer->value;
	    break;
	case INTSRV_COMPOSED_IS_HOPS:
	    name = "IS Hops";
	    value = integer->value;
	    break;
	case INTSRV_PATH_BANDWIDTH:
	    name = "Path Bandwidth (bytes/sec)";
	    value = ieee_to_long(real->value);
	    break;
	case INTSRV_MIN_PATH_BANDWIDTH:
	    name = "Minimum Path Bandwidth (bytes/sec)";
	    value = ieee_to_long(real->value);
	    break;
	case INTSRV_IF_LATENCY:
	    name = "Latency (microseconds)";
	    value = ieee_to_long(real->value);
	    break;
	case INTSRV_PATH_LATENCY:
	    name = "Path Latency (microseconds)";
	    value = ieee_to_long(real->value);
	    break;
	case INTSRV_MTU:
	    name = "MTU";
	    value = integer->value;
	    break;
	case INTSRV_COMPOSED_MTU:
	    name = "Path MTU";
	    value = integer->value;
	    break;
	case INTSRV_E2E_DELAY:
	    name = "Path Delay (microseconds)";
	    value = integer->value;
	    break;
	case INTSRV_E2E_MAX_JITTER:
	    name = "Path Jitter (microseconds)";
	    value = integer->value;
	    break;
	case INTSRV_SHP_DELAY:
	    name = "Path delay since shaping (microseconds)";
	    value = integer->value;
	    break;
	case INTSRV_SHP_MAX_JITTER:
	    name = "Path Jitter since shaping (microseconds)";
	    value = integer->value;
	    break;
	}
	buginf("\n\t%-45s:%d", name, value);
	integer++;
	params = integer;
    }
    return params;
}

void rsvp_display_adspec(void *object)
{
    rsvp_adspec *adspec = object;
    ushort length = adspec->ad.gtd.length_in_words;
    intsrv_adspec_general *part;

    buginf("\n version=%d  length in words=%d",
	   adspec->ad.gtd.version>>14, length);
    part = &adspec->ad.gtd.general;
    length -= 1 + part->service_length;
    part = rsvp_display_adspec_part(part, "General Parameters");
    if (length <= 0)
    	return;

    if (part->service_id == INTSRV_GTD) {
	length -= 1 + part->service_length;
	part = rsvp_display_adspec_part(part, "Guaranteed Service");
	if (length <= 0)
	    return;
    }

    if (part->service_id == INTSRV_CLOAD) {
	length -= 1 + part->service_length;
	part = rsvp_display_adspec_part(part, "Controlled Load Service");
    }
}

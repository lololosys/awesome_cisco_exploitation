/* $Id: rsvp_ip_api.c,v 1.1.4.13 1996/08/24 01:16:42 fred Exp $
 * $Source: /release/112/cvs/Xsys/rsvp/rsvp_ip_api.c,v $
 *------------------------------------------------------------------
 * Resource Reservation Protocol
 *
 * April 1996, Fred Baker
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rsvp_ip_api.c,v $
 * Revision 1.1.4.13  1996/08/24  01:16:42  fred
 * CSCdi67062:  Resolution of interoperability testing with ISI 4.0a6 code
 * Branch: California_branch
 *
 * Revision 1.1.4.12  1996/08/20  10:24:29  fred
 * CSCdi66538:  implement integrated services objects (RSVP)
 * Branch: California_branch
 *
 * Revision 1.1.4.11  1996/08/16  17:21:02  asastry
 * CSCdi65504:  Make 'debug IP RSVP detail' command dump packet contents
 * in the event of an error condition.
 *
 * Branch: California_branch
 *
 * Revision 1.1.4.10  1996/08/09  00:18:10  asastry
 * CSCdi65504:  Allow <debug ip rsvp detail> command to be RESV-only or
 * PATH-only
 * Branch: California_branch
 *
 * Revision 1.1.4.9  1996/08/07  09:02:13  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 1.1.4.8  1996/07/31  00:33:13  asastry
 * CSCdi53951:  RSVP fails to forward unicast PATH messages.
 * Branch: California_branch
 *
 * Revision 1.1.4.7  1996/07/19  06:09:27  asastry
 * CSCdi63472:  Add page breaks to RSVP files
 * Branch: California_branch
 *
 * Revision 1.1.4.6  1996/07/05  16:51:19  fred
 * CSCdi61916:  RSVP doesnt check the return value of pak_duplicate()
 * Branch: California_branch
 *
 * Revision 1.1.4.5  1996/06/18  08:37:14  fred
 * CSCdi60599:  RSVP interaction with PIM Sparse Mode has problems.
 *              Implement special code required for sparse mode route
 *              creation
 * Branch: California_branch
 *
 * Revision 1.1.4.4  1996/06/11  20:30:52  fred
 * CSCdi60143:  add interface to buginfs
 * Branch: California_branch
 *
 * Revision 1.1.4.3  1996/05/13  16:45:53  fred
 * CSCdi57500:  update rsvp from draft 8 to draft 11 objects
 *         This is done with an ifdef at this point as some EFT sites
 *         need draft 8 code; these will be removed later
 * Branch: California_branch
 *
 * Revision 1.1.4.2  1996/05/13  16:42:30  fred
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

#include "master.h"
#include "interface_private.h"
#include "logger.h"
#include "stdarg.h"
#include "../ip/ip.h"
#include "../ip/ipoptions.h"
#include "../ip/ip_registry.h"
#include "../iprouting/route.h"
#include "../iprouting/route_inlines.h"
#include "../ipmulticast/mroute.h"
#include "rsvp_objects.h"
#include "rsvp_database.h"
#include "rsvp_inline.h"

static ipparamtype rsvp_mc_params = {0};
ipoptiontype rsvp_alert;
void bugtrace(void);


/*
 * add to list of neighbors
 */
void
rsvp_neighbor (paktype *pak)
{
    iphdrtype *ip;
    idbtype *idb;
    rsvp_info_type *rsvp_info;
    rsvp_neighbor_type *rsvp_nbr;
    uchar ip_protocol;
    ipaddrtype neighbor;

    ip =(iphdrtype *) ipheadstart(pak);
    ip_protocol = ip->prot;
    neighbor = ip->srcadr;
    idb = pak->if_input;

    if(!ip_onifsubnet(idb, neighbor))
	return;

    rsvp_info = idb->rsvp_info;
    if (!rsvp_info)
	return;
    for(rsvp_nbr = rsvp_info->rsvp_nbr; rsvp_nbr; rsvp_nbr = rsvp_nbr->next)
	if(neighbor == rsvp_nbr->neighbor) {
	    if(rsvp_nbr->ip_protocol && ip->prot != rsvp_nbr->ip_protocol) {
		if (UDP_PROT == rsvp_nbr->ip_protocol)
		    rsvp_info->udp_neighbors--;
		else
		    rsvp_info->ip_neighbors--;
		rsvp_nbr->ip_protocol = 0;
		rsvp_info->duplex_neighbors++;
	    }
	    return;
	}
    rsvp_nbr = malloc(sizeof(rsvp_neighbor_type));
    if(rsvp_nbr) {
	rsvp_nbr->neighbor = neighbor;
	rsvp_nbr->ip_protocol = ip->prot;
	rsvp_nbr->next = rsvp_info->rsvp_nbr;
	rsvp_info->rsvp_nbr = rsvp_nbr;
	if (UDP_PROT == rsvp_nbr->ip_protocol)
	    rsvp_info->udp_neighbors++;
	else
	    rsvp_info->ip_neighbors++;
    }
}

/*
 * do I need to send udp?
 */
static boolean
rsvp_nbr_udp (idbtype *idb, ipaddrtype neighbor)
{
    rsvp_info_type *rsvp_info;
    rsvp_neighbor_type *rsvp_nbr;

    rsvp_info = idb->rsvp_info;
    if(!rsvp_info)
	return FALSE;
    for(rsvp_nbr = rsvp_info->rsvp_nbr; rsvp_nbr; rsvp_nbr = rsvp_nbr->next) {
	if(rsvp_nbr->neighbor == neighbor) {
	    if(rsvp_nbr->ip_protocol == UDP_PROT)
		return(TRUE);
	    else
		return(FALSE);
	}
    }
    return(FALSE);
}

/*
 * do I need to send udp?
 */
static boolean
rsvp_sends_udp (idbtype *idb)
{
    rsvp_info_type *rsvp_info;

    rsvp_info = idb->rsvp_info;
    if(!rsvp_info)
	return FALSE;
    if (rsvp_info->udp_required || rsvp_info->udp_neighbors)
	return TRUE;
    return FALSE;
}

/*
 * rsvp_upstream_lookup
 *
 * Lookup in the unicast/multicast routing table the upstream neighbor to
 * reach the source. If the destination is unicast, the unicast routing
 * table will be used. If the destination is multicast, the mutlicast routing
 * table will be used. Returns FALSE when route lookup failed.
 *
 * This function is intended to be used when sending RESV messages.
 */
boolean
rsvp_upstream_lookup (ipaddrtype source, ipaddrtype dest,
		      idbtype **idb, ipaddrtype *next_hop)
{
    mdbtype *mdb;
    idbtype *upstream_idb;
    ipaddrtype upstream_nbr;

    /*
     * Do multicast routing table lookup if destination is a group address.
     */
    if (IPMULTICAST(dest)) {
	mdb = ip_create_mroute_state(source, dest);
	if (!mdb) return(FALSE);
	upstream_idb = mdb->if_input;
	upstream_nbr = mdb->rpf_nbr;
    }
    else {
	upstream_nbr = source;
	upstream_idb =
	    reg_invoke_ip_first_hop(&upstream_nbr, source,
			            (IP_PATH_ROUNDROBIN | IP_PATH_FAIL_FORUS));
    }
    /*
     * Return values.
     */
    if(idb)
	*idb = upstream_idb;
    if(next_hop)
	*next_hop = upstream_nbr;
    return((NULL != upstream_idb));
}

/*
 * rsvp_downstream_lookup
 *
 * Get outgoing interface(s) to send PATH messages. If outputQ is Null, there
 * may be only one interface which is described in idb(the destination is
 * a unicast route). Returns FALSE when the route lookup failed.
 */
boolean rsvp_downstream_lookup (ipaddrtype source, ipaddrtype dest,
				queuetype **outputQ, idbtype **idb,
				ipaddrtype * next_hop)
{
    mdbtype *mdb;

    /*
     * Unicast lookup is the same as in the upstream case.
     */
    if (!IPMULTICAST(dest)) {
	if (outputQ) *outputQ = NULL;
	return(rsvp_upstream_lookup(dest, dest, idb, next_hop));
    }

    /*
     * Do multicast lookup and return outgoing interface list. Create state
     * if it doesn't exist.
     */
    mdb = ip_create_mroute_state(source, dest);
    if (!mdb) return(FALSE);
    
    /*
     * Send a Prune back if this mdb has a null-oif
     */
    if (MDB_OUTPUTQ_NULL(mdb)) {
	ipmulticast_send_prune(mdb);
	return(FALSE);
    }

    /*
     * Return output parameter values.
     */
    if (idb) *idb = NULL;
    if (next_hop) *next_hop = 0;
    if (outputQ) *outputQ = &mdb->if_outputQ;
    return(TRUE);
}

/*
 * rsvp_get_dowstream_interface
 *
 * Used in a loop to get each interface in a outgoing interface list. last_idb
 * is set to Null to get the first entry. Each subsequent call should pass in
 * the last idb processed by the caller. Returns FALSE when the traversal is
 * complete.
 */
boolean
rsvp_get_downstream_interface (queuetype *outputQ, idbtype *last_idb,
			       idbtype **idb, ipaddrtype *next_hop,
			       ulong ttl)
{

    midbtype *midb;

    /*
     * Go to correct spot in list. Assume that an insertion could have happened
     * between calls.
     */

    if(!last_idb) {
	midb = outputQ->qhead;
    }
    else {
	midb = ip_get_midb(outputQ, last_idb, 0, FALSE);
	if(midb)
	    midb = midb->next;
    }
    /*
     * Get next interface that is not pruned and meets TTL threshold
     * constraints.
     */
    for(; midb; midb = midb->next) {
	if(midb->state == MIDB_PRUNE_STATE)
	    continue;
	if(ttl < midb->idb->ip_multicast_threshold)
	    continue;
	if(idb)
	    *idb = midb->idb;
	if(next_hop)
	    *next_hop = midb->next_hop;
	return(TRUE);
    }
    return(FALSE);
}

/*
 * rsvp_needs_sm_attention	
 *
 * Determine if a PIM register message needs to be sent for this sparse-mode
 * group. If so, have the packet looped back through the IP multicast
 * forwarding path.
 */
boolean rsvp_needs_sm_attention (psbtype *psb)
{
    gdbtype *gdb;
    mdbtype *mdb;
    
    gdb = ip_get_gdb(psb->head->session.destination);
    if (!gdb) return(FALSE);
    
    mdb = ip_get_best_mdb(gdb, psb->template.source);
    if (!mdb || !mdb->register_flag) return(FALSE);
    return(TRUE);
}

/*
 * rsvp_update_mdb_timers
 *
 * Updates multicast routing table entry inactivity timers when PATH message
 * is sent.
 */
void rsvp_update_mdb_timers (ipaddrtype group, ipaddrtype source)
{
    gdbtype *gdb;
    mdbtype *mdb;

    gdb = ip_get_gdb(group);
    if (!gdb) return;

    mdb = ip_get_best_mdb(gdb, source);

    /*
     * Can't update timer if there is no mdb. If there is one, don't update
     * it if there are no olist entries. We want the case in the first-hop
     * router to PIM reregister when the (S,G) entry times out.
     */
    if (!mdb || MDB_OUTPUTQ_NULL(mdb)) return;

    TIMER_START_JITTERED(mdb->expires, mdb->expiration_time, MDB_JITTER_PCT);
}

/***********************************************************
 *
 * API to deal with IP and UDP 
 *
 ***********************************************************/
/*
 * display messages sent
 */
static char *const rsvp_msg_type[] = {
    "", "PATH", "RESV", "PERR", "RERR", "PTEAR", "RTEAR", "CONFIRM"};
static char *const rsvp_obj_type[] = {
    "NULL", "SESSION", "SESSION_GROUP", "HOP", "INTEGRITY",
    "TIME_VALUES", "ERROR_SPEC", "SCOPE", "STYLE", "FLOWSPEC",
    "FILTER_SPEC", "SENDER_TEMPLATE", "SENDER_TSPEC", "ADSPEC",
    "POLICY_DATA", "CONFIRM"
    };
void
rsvp_dump_object (rsvp_header *header, short length)
{
    uchar *x;
    rsvp_object *object;
    short object_length;
    short i;

    if (!rsvp_detail_flag)
	return;

    buginf("\nRSVP:\tversion:%d", header->version);
    buginf(" flags:%d%d%d%d", header->flag0, header->flag1,
	   header->flag2, header->flag3);
    buginf(" type:%s", rsvp_msg_type[header->message_type]);
    buginf(" cksum:%04x", header->cksum);
    buginf(" ttl:%d", header->sending_ttl);
    buginf(" reserved:%d", header->reserved_byte);
    buginf(" length:%d", header->rsvp_length);
#ifdef DRAFT10
    buginf("\n\tmsg id:%d", header->message_id);
    buginf(" MF:%d", header->fragment & RSVP_MORE_FRAGS? 1:0);
    buginf(" offset:%d", header->fragment & RSVP_FRAGMENT_OFFSET);
#endif
    length -= sizeof(*header);
    object = header->rsvp_first_object;
    while (length > 0) {
	object_length = object->length;
	if (0 == object_length) {
	    buginf("\n zero length object");
	    bugtrace();
	    break;
	}
	if (0 != (object_length%4)) {
	    buginf("\n invalid length object (%d)", object_length);
	    bugtrace();
	    break;
	}
	length -= object_length;
	switch (object->class) {
	case RSVP_CLASS_FLOWSPEC:
	    buginf("\n %18s type %d length %2d:",
		   rsvp_obj_type[object->class], object->type, object->length);
	    rsvp_display_flowspec(object);
	    break;
	case RSVP_CLASS_SENDER_TSPEC:
	    buginf("\n %18s type %d length %2d:",
		   rsvp_obj_type[object->class], object->type, object->length);
	    rsvp_display_tspec(object);
	    break;
	case RSVP_CLASS_ADSPEC:
	    buginf("\n %18s type %d length %2d:",
		   rsvp_obj_type[object->class], object->type, object->length);
	    rsvp_display_adspec(object);
	    break;
	default:
	if (object->class <= RSVP_CLASS_MAX) {
	    buginf("\n %18s type %d length %2d:",
		   rsvp_obj_type[object->class], object->type, object->length);
	} else {
	    char class_string[32];
	    sprintf(class_string, "UNKNOWN CLASS %d", object->class);
	    buginf("\n %18s type %d length %2d:",
		   class_string, object->type, object->length);
	}
	x = object->data;
	for (i = sizeof(rsvp_object); i < object_length; i++) {
	    if (i && (i % 8 == 0))
	        buginf("\n\t%18s      %s      %2s: ", " ", " ", " ");
	    else if (i && (i % 4 == 0))
		buginf(" ");
	    buginf("%02x", *x++);
	}
	break;
	}
	object = rsvp_next_object(object);
    }
    buginf("\n");
}

/*
 * calculate length of the message that we are going to build
 */
static ushort
rsvp_message_length (int message_type, int number_of_objects, va_list argptr)
{
    ushort length;
    int index;
    rsvp_object *object;

    length = sizeof(rsvp_header);
    for (index = number_of_objects; --index >= 0;) {
	object = va_arg(argptr, rsvp_object *);
	if (object)
	    length += object->length;
    }
    return length;
}

/*
 * Build an RSVP message to send.
 */
static paktype *
rsvp_build_message (int message_type, int number_of_objects,
		    va_list argptr, ushort length, ushort offset, uchar ttl)
{
    paktype *pak;
    int index;
    rsvp_header *header;
    rsvp_object *object;
    rsvp_object *new;

    pak = getbuffer(length + offset);
    if (pak) {
	pak->datagramsize = length + offset;
	header = (rsvp_header *) (ipheadstart(pak) + offset);
	new = rsvp_build_header(header, message_type, ttl);
	for (index = number_of_objects; --index >= 0;) {
	    object = va_arg(argptr, rsvp_object *);
	    if (!object)
		continue;
	    if (!object->length)
		continue;
	    rsvp_object_copy(new, object);
	    new = rsvp_next_object(new);
	}

	header->rsvp_length = length;
	header->cksum = 0;
	header->cksum = ipcrc((ushort *) header, length);
    }
    return pak;
}

/*
 * send an IP RSVP message to a next hop router or end station
 */
static void
rsvp_send_fixed_message_ip (int message_type, idbtype *idb,
			    ipaddrtype destination, ipaddrtype next_hop,
			    int number_of_objects, va_list argptr)
{
    paktype *pak;
    ushort length;
    rsvp_header *header;

    length = rsvp_message_length(message_type, number_of_objects, argptr);
    pak = rsvp_build_message(message_type, number_of_objects, argptr,
			     length, IPHEADERBYTES(NOPT), TTL_DEFAULT);
    if (pak) {
	if (rsvp_acl_debug(rsvp_debug, destination)) {
	    header = (rsvp_header *) (ipheadstart(pak) + IPHEADERBYTES(NOPT));
	    buginf("\nRSVP: IP to %i length=%d checksum=%04x (%s)",
		   next_hop, header->rsvp_length, header->cksum,
		   (pak->if_output) ? pak->if_output->namestring : "null");
	    RSVP_DUMP_OBJECT(header, rsvp_detail_flag, length);
	}
	ipwrite(pak, length + IPHEADERBYTES(NOPT), RSVP_PROT,
		rsvp_ip_address(idb), next_hop, idb, NULL, NULL);
    }
}

/*
 * send a UDP/IP RSVP message to a next hop router or end station
 */
static void
rsvp_send_fixed_message_udp (int message_type, idbtype *idb,
			     ipaddrtype destination, ipaddrtype next_hop,
			     int number_of_objects, va_list argptr)
{
    paktype *pak;
    ushort length;
    rsvp_header *header;
    udptype *udp;

    length = rsvp_message_length(message_type, number_of_objects, argptr);
    pak = rsvp_build_message(message_type, number_of_objects,
			     argptr, length,
			     IPHEADERBYTES(NOPT) + UDPHEADERBYTES,
			     TTL_DEFAULT);
    if (pak) {
	udp = (udptype *) (ipheadstart(pak) + IPHEADERBYTES(NOPT));

	udp->checksum = 0;
	udp->length = length + UDPHEADERBYTES;
	udp->dport = RSVP_PORT_HOST;
	udp->sport = RSVP_PORT_ROUTER;
	udp->checksum = CheckSum((uchar *) udp, udp->length,
				 rsvp_ip_address(idb), next_hop, UDP_PROT);
	if (rsvp_acl_debug(rsvp_debug, destination)) {
	    header = (rsvp_header *) (udp + 1);
	    buginf("\nRSVP: UDP to %i(%d:%d) length=%d checksum=%04x (%s)",
		   next_hop, RSVP_PORT_HOST, RSVP_PORT_ROUTER,
		   udp->length, udp->checksum, pak->if_output->namestring);
	    RSVP_DUMP_OBJECT(header, rsvp_detail_flag, length);
	}
	ipwrite(pak, length + UDPHEADERBYTES + IPHEADERBYTES(NOPT),
		UDP_PROT, rsvp_ip_address(idb), next_hop, idb, NULL, NULL);
    }
}

/*
 * send an RSVP message to the next hop router or end station
 */
void
rsvp_send_fixed_message (int message_type, idbtype *idb, ipaddrtype next_hop,
			 int number_of_objects, ...)
{
    va_list argptr;
    rsvp_session_ipv4 *session;

    va_start(argptr, number_of_objects);
    session = va_arg(argptr, rsvp_session_ipv4 *);
    va_end(argptr);

    va_start(argptr, number_of_objects);
    if (rsvp_nbr_udp(idb, next_hop)) {
	rsvp_send_fixed_message_udp(message_type, idb, session->destination,
				    next_hop, number_of_objects, argptr);
    } else {
	rsvp_send_fixed_message_ip(message_type, idb, session->destination,
				   next_hop, number_of_objects, argptr);
    }
    va_end(argptr);
}

/*
 * send an IP RSVP message to a multicast group on one interface
 */
static void
rsvp_send_fixed_multicast_ip (int message_type, boolean use_ra,
			      ipaddrtype sender, ipaddrtype destination, 
			      idbtype *output, uchar ttl,
			      int number_of_objects, va_list argptr)
{
    paktype *pak;
    ushort length, offset;
    rsvp_header *header;

    /*
     * We might send a PATH message without the Router-Alert option to
     * build the multicast distribution tree when there are no data
     * packets being sent to do so (i.e. the PATH message is boostraping
     * the distribution tree state).
     */
    if (use_ra) {
	length = rsvp_message_length(message_type, number_of_objects, argptr);
	pak = rsvp_build_message(message_type, number_of_objects, argptr, 
				 length, IPHEADERBYTES(&rsvp_alert), ttl);
    } else {
	length = sizeof(rsvp_header);
	pak = rsvp_build_message(message_type, 0, NULL, length, 
				 IPHEADERBYTES(NOPT), ttl);
    }
    if (!pak) return;

    offset = use_ra ? IPHEADERBYTES(&rsvp_alert) : IPHEADERBYTES(NOPT);
    if (rsvp_acl_debug(rsvp_debug, destination)) {
	header = (rsvp_header *) (ipheadstart(pak) + offset);
	buginf("\nRSVP:\t IP to %i length=%d checksum=%04x (%s)",
	       destination, header->rsvp_length, header->cksum,
	       output ? output->namestring : "null");
	RSVP_DUMP_OBJECT(header, rsvp_detail_flag, length);
    }
    
    if (use_ra) {
	length += IPHEADERBYTES(&rsvp_alert);
    } else {
	length += IPHEADERBYTES(NOPT);
    }
    ipwrite(pak, length, RSVP_PROT, sender, destination, output, 
	    (use_ra) ? &rsvp_alert : NULL, &rsvp_mc_params);
}

/*
 * send a UDP/IP RSVP message to a multicast group on one interface
 */
static void
rsvp_send_fixed_multicast_udp (int message_type, ipaddrtype sender,
			       ipaddrtype destination, idbtype *output, 
			       uchar ttl, int number_of_objects, 
			       va_list argptr)
{
    paktype *pak;
    ushort length;
    udptype *udp;
    rsvp_header *header;

    length = rsvp_message_length(message_type, number_of_objects, argptr);
    pak = rsvp_build_message(message_type, number_of_objects, argptr, length,
			     UDPHEADERBYTES + IPHEADERBYTES(NOPT),
			     ttl);
    if (pak) {
	udp = (udptype *) (ipheadstart(pak) + IPHEADERBYTES(NOPT));
	udp->length = UDPHEADERBYTES + length;
	udp->dport = RSVP_PORT_HOST;
	udp->sport = RSVP_PORT_ROUTER;
	udp->checksum = 0;

	if (rsvp_acl_debug(rsvp_debug, destination)) {
	    header = (rsvp_header *) (udp + 1);
	    buginf("\nRSVP:\tUDP to %i(%d:%d) length=%d checksum=%04x (%s)",
		   destination, RSVP_PORT_HOST, RSVP_PORT_ROUTER,
		   udp->length, udp->checksum, output->namestring);
	    RSVP_DUMP_OBJECT(header, rsvp_detail_flag, length);
	}

	if ((IPMULTICAST(destination)) ||
	    (destination == output->ip_broadcast)) {
	    udp->checksum = CheckSum((uchar *) udp, udp->length,
				     rsvp_ip_address(output),
				     output->rsvp_info->udp_group,
				     UDP_PROT);
	    ipwrite(pak, length + UDPHEADERBYTES + IPHEADERBYTES(NOPT),
		    UDP_PROT, rsvp_ip_address(output),
		    output->rsvp_info->udp_group,
		    output, NULL, &rsvp_mc_params);
	}
	else {
	    udp->checksum = CheckSum((uchar *) udp, udp->length,
				     rsvp_ip_address(output), destination,
				     UDP_PROT);
	    ipwrite(pak, length + UDPHEADERBYTES + IPHEADERBYTES(NOPT),
		    UDP_PROT, rsvp_ip_address(output),
		    destination, output, NULL, &rsvp_mc_params);
	}
    }
}

/*
 * send an RSVP message to a multicast group on one interface
 */
void
rsvp_send_fixed_multicast (int message_type, boolean use_ra, ipaddrtype sender,
			   idbtype *output, uchar ttl, int number_of_objects, ...)
{
    va_list argptr;
    rsvp_session_ipv4 *session;

    if (ttl <= 1)
	return;

    ttl--;
    rsvp_mc_params.ttl = ttl;
    rsvp_mc_params.switching = TRUE;

    va_start(argptr, number_of_objects);
    session = va_arg(argptr, rsvp_session_ipv4 *);
    va_end(argptr);

    va_start(argptr, number_of_objects);
    if (output && rsvp_sends_udp(output)) {
	rsvp_send_fixed_multicast_udp(message_type, sender,
				      session->destination, output, ttl, 
				      number_of_objects, argptr);
    }
    rsvp_send_fixed_multicast_ip(message_type, use_ra, sender,
				 session->destination, output, ttl, 
				 number_of_objects, argptr);
    va_end(argptr);

    /*
     * Update activity timers for multicast routing table entry.
     */
    rsvp_update_mdb_timers(session->destination, sender);
}

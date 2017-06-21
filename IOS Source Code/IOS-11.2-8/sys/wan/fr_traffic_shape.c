/* $Id: fr_traffic_shape.c,v 3.1.2.8 1996/08/15 19:32:49 fowler Exp $
 * $Source: /release/112/cvs/Xsys/wan/fr_traffic_shape.c,v $
 *------------------------------------------------------------------
 * fr_traffic_shape.c
 *
 * April 1996, Greg Fowler
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: fr_traffic_shape.c,v $
 * Revision 3.1.2.8  1996/08/15  19:32:49  fowler
 * CSCdi60278:  Removing frame-relay traffic shaping from an int causes
 *              traceback
 *              nvgen fixes and timer fix for de-encapsulation
 * Branch: California_branch
 *
 * Revision 3.1.2.7  1996/07/17  06:12:20  fowler
 * CSCdi57463:  Frame relay broadcast queue should use new IDB list
 * functions
 *              Remove several instances of FOR_ALL_HWIDBS and replace
 *              with walks of the FR idb list
 * Branch: California_branch
 *
 * Revision 3.1.2.6  1996/06/20  16:14:16  fowler
 * CSCdi60477:  crash in fr_enqueue
 * Branch: California_branch
 *
 * Revision 3.1.2.5  1996/06/12  14:33:43  fowler
 * CSCdi60166:  Crash at fr_enque and fr_oqueue
 *              Make sure idb initialization complete before setting
 *              up queuing
 * Branch: California_branch
 *
 * Revision 3.1.2.4  1996/06/11  18:14:57  fowler
 * CSCdi59356:  All frame relay subinterface tests are failing - could be
 *              a broadcast
 * Branch: California_branch
 *
 * Revision 3.1.2.3  1996/05/31  18:19:35  vrawat
 * CSCdi58846:  frame-relay debugging enhancements
 * Branch: California_branch
 *
 * Revision 3.1.2.2  1996/05/29  06:35:00  fowler
 * CSCdi58766:  Link errors in 11.2 build
 * Branch: California_branch
 *
 * Revision 3.1.2.1  1996/05/15  05:54:45  fowler
 * Frame relay VC queuing and rate enforcement
 * Branch: California_branch
 *
 * Revision 3.1  1996/05/02  20:55:11  fowler
 * Placeholders.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "sys_registry.h"
#include "media_registry.h"
#include "subsys.h"
#include "interface_private.h"
#include "packet.h"
#include "config.h"
#include "../ui/debug.h"
#include "serial.h"
#include "../if/network.h"
#include "../wan/dialer.h"
#include "address.h"
#include "mgd_timers.h"
#include "../wan/frame_relay.h"
#include "fr_traffic_shape.h"
#include "../wan/fr_registry.h"
#include "../wan/fr_vcq.h"
#include "../if/traffic_shape.h"
#include "parser_defs_frame.h"
#include "logger.h"
#include "msg_frame_relay.c"
#include "../wan/dialer_registry.h"

mgd_timer fr_idb_maint;


/*
 * fr_bq_int_display
 * Show interface stats for broadcast queue
*/
static void fr_bq_int_display (fr_idb_struct_type *fr_idb,
			dialerdbtype *ddb)
{
    if (!(ddb && HUNTGROUP_LEADER(ddb))) {
	printf("\n  Broadcast queue %d/%d, broadcasts sent/dropped %d/%d,"
	       " interface broadcasts %d",
	       fr_idb->fr_br_packetQ.count,fr_idb->fr_br_packetQ.maximum,
	       fr_idb->num_broad_frbq,fr_idb->num_drop,
	       fr_idb->num_broad_total);
    }
}

/*
 * fr_vc_shape_display
 * Display the traffic shaping parameters for a VC
 */
static void fr_vc_shape_display (hwidbtype *hwidb,
				  ushort dlci)
{
    fr_idb_struct_type *fr_idb = hwidb->frame_relay_stuff;
    fr_pvc_info_type *pvc;
    traffic_shape_t *shape;
    idbtype *idb;

    if (fr_tr_on_idb(hwidb)) {
	if ((pvc = fr_idb->fr_dlci_tab[dlci])) {
	    if (!(shape = pvc->shape))
		return;
	    idb = pvc->idb;
	    printf("\n  %3s %9u %2s %9u %2s %9u %5s %6u %8s %3u "
		   "\n  %6s %9u %13s %5u %13s %3s",
		   "cir",
		   shape->shape_cir,
		   "bc",
		   shape->shape_sustained_bits,
		   "be",
		   shape->shape_excess_bits,
		   "limit",
		   shape->shape_byte_limit,
		   "interval",
		   shape->shape_interval,
		   "mincir",
		   shape->shape_cir_lower,
		   "byte increment",
		   shape->shape_increment,
		   "BECN response",
		   shape->shape_to_becn?"yes":"no");
	    printf("\n  %4s %9u %5s %9u %12s %9u %13s %9u"
		   "\n  %20s %10s","pkts ",
		   (shape->stat.shape_packets - shape->si.shape_packets) +
		   (shape->stat.shape_packets_dly - shape->si.shape_packets_dly),
		   "bytes ",
		   (shape->stat.shape_octets - shape->si.shape_octets) + 
		   (shape->stat.shape_octets_dly - shape->si.shape_octets_dly),
		   "pkts delayed",
		   (shape->stat.shape_packets_dly - shape->si.shape_packets_dly),
		   "bytes delayed",
		   (shape->stat.shape_octets_dly - shape->si.shape_octets_dly),
		   mgd_timer_running(&shape->shape_restart)?"shaping active":"shaping inactive");
	}
    }
}

/*
 * fr_oqueue
 * frame relay output queuing vector
 */
static boolean fr_oqueue (
    hwidbtype *output,
    paktype *pak,
    enum HEADTAIL which)
{
    fr_idb_struct_type *fr_idb;

    fr_idb = output->frame_relay_stuff;
    if (!(pak->fr_flags & PAK_FR_BROADCAST)) {
	return((*fr_idb->save_oqueue)(output,pak,which));
    }
    else
	if (!QUEUEFULL(&fr_idb->fr_br_packetQ)) {
	    p_enqueue(&fr_idb->fr_br_packetQ,pak);
	    fr_idb->num_broad_frbq++;
	    return(TRUE);
	} else {
	    fr_idb->num_drop++;
	    datagram_done(pak);
	    return(FALSE);
	}
}

/*
 * fr_get_vc_class_param
 * Return the requested parameter from the map class
 */
static uint fr_get_vc_class_param (mapclass_type *classp,
				enum FR_PARAMS_TYPE param_type)
{
    fr_params_type *fr_params;
	
    if (!classp || classp->encap != ET_FRAME_RELAY) {
	return(56000);
    }
    fr_params = classp->params;
    if (!fr_params)
	return(FR_DEFAULT_CIR);
    if (!fr_params->vcq_params)
	return(FR_DEFAULT_CIR);
    switch (param_type) {
    case VC_CIR_OUT:
	if (fr_params->vcq_params->fr_ave_rate)
	    return(fr_params->vcq_params->fr_ave_rate);
	if (fr_params->vcq_params->cir)
	    return(fr_params->vcq_params->cir);
	return(FR_DEFAULT_CIR);
    case VC_BC_OUT:
	if (fr_params->vcq_params->fr_ave_rate)
	    return(fr_params->vcq_params->fr_ave_rate);
	if (fr_params->vcq_params->bc)
	    return(fr_params->vcq_params->bc);
	if (fr_params->vcq_params->cir)
	    return(fr_params->vcq_params->cir);
	return(FR_DEFAULT_BC);
    case VC_BE_OUT:
	if (fr_params->vcq_params->fr_peak_rate)
	    return(fr_params->vcq_params->fr_peak_rate
		   -fr_params->vcq_params->fr_ave_rate);
	else
	    return(fr_params->vcq_params->be);
    case VC_CIR_LOWER:
	return(fr_params->vcq_params->cir/2);
    case VC_BECN:
	return(fr_params->vcq_params->fr_becn_response);
    default:
	return(56000);
    }

}

/*
 * fr_set_shape_params
 * Initialize or update shape parameters
 */
void fr_set_shape_params (traffic_shape_t *shape,
			  fr_pvc_info_type *fr_pvc)
{

    ulong shape_interval;		/* duration of interval */
    ulong shape_cir;			/* target traffic rate - upper bound */
    ulong shape_sustained_bits;		/* Bc - sustained bits */
    ulong shape_excess_bits;		/* Be - excess bits first interval */
    ulong shape_byte_limit;		/* max number of bytes for interval */
    ulong shape_increment;		/* number bytes added per interval */
    ulong shape_increment_limit;	/* upper bound of FECN/BECN range */
    ulong param_cir;  /* Target Rate */
    ulong param_Bc; /* Burst Size - sustained */
    ulong param_Be;  /* Burst Size - excess */
    ulong param_lcir;
    boolean param_becn;
    mapclass_type *classp;

    reg_invoke_fr_find_mapclass(fr_pvc, &classp);
    param_cir = fr_get_vc_class_param(classp, VC_CIR_OUT); 
    param_Bc = fr_get_vc_class_param(classp,VC_BC_OUT); 
    param_Be = fr_get_vc_class_param(classp,VC_BE_OUT);
    param_lcir = fr_get_vc_class_param(classp, VC_CIR_LOWER);
    param_becn = fr_get_vc_class_param(classp, VC_BECN);
    /*
     * apply default of DEFAULT_SHAPE_INTERVAL milliseconds
     */
    shape_cir = param_cir;	/* BPS */
    if (0 == param_Bc)
	shape_sustained_bits = (shape_cir*DEFAULT_SHAPE_INTERVAL)/1000;
    else
	shape_sustained_bits = param_Bc; /* bits */

    if (0 == param_Be)
	shape_excess_bits = shape_sustained_bits;
    else
	shape_excess_bits = param_Be; /* bits */

    /*
     * determine length of interval
     *
     * note that command line is in bits per second
     * and we ultimately want the interval in milliseconds and
     * the limits in bytes.
     */
    shape_byte_limit = (shape_sustained_bits + shape_excess_bits)/8;
    if (shape_sustained_bits) {
	shape_increment = shape_sustained_bits/8;
	shape_interval = shape_sustained_bits*1000/shape_cir;
    } else {
	shape_increment = shape_excess_bits/8;
	shape_interval = shape_excess_bits*1000/shape_cir;
    }
    /*
     * intervals that are too large will be painful.
     * scale down if necessary
     */
#define SHAPE_MAX_BYTES		10000 /* bigger than this is too deep */
    while (shape_interval >= DEFAULT_SHAPE_INTERVAL*2 ||
	   shape_increment > SHAPE_MAX_BYTES) {
	if (shape_interval < 2*SHAPE_MIN_INTERVAL)
	    break;
	shape_interval = (shape_interval + 1)/2;
	shape_increment = (shape_increment + 1)/2;
	shape_byte_limit -= shape_increment;
    }
    shape_increment_limit = shape_increment;

    shape->shape_interval = shape_interval;
    shape->shape_cir = shape_cir;
    shape->shape_sustained_bits = shape_sustained_bits;
    shape->shape_excess_bits = shape_excess_bits;
    shape->shape_cir_lower = param_lcir;
    shape->shape_byte_limit = shape_byte_limit;
    shape->shape_increment = shape_increment;
    shape->shape_increment_limit = shape_increment_limit;
    shape->shape_to_becn = param_becn;
    shape->shape_acl = 0;
    shape->shape_byte_credit = 0;
    shape->shape_byte_credit_int = 0;
    traffic_shape_apply_credit(shape); /* initialize NULL credit */
    shape_adaptive_configure(shape); /* adjust adaptive parameters */
}

/*
 * fr_shape_setup
 * Init the per VC shape structure
 */
void fr_shape_setup (idbtype *idb,
		     fr_pvc_info_type *fr_pvc,
		     enum FR_SHAPE_CONF sense)
{
    hwidbtype *hwidb = hwidb_or_null(idb);
    traffic_shape_t *shape = idb->traffic_shape;
    boolean new_shape_entry;		/* need to put on list? */
    leveltype status;
    traffic_shape_t **shapep = &idb->traffic_shape;
    paktype *pak;
    ushort dlci_num = fr_pvc->this_dlci;

    /*
     * Determine if the shaping structure already exists
     */
    new_shape_entry = FALSE;
    for (; (shape = *shapep); shapep = &shape->shape_param_next)
	if (dlci_num == shape->shape_dlci_num)
	    break;

    switch (sense) {
    case SHAPE_UPDATE:
	if (!shape)
	    return;
	fr_set_shape_params(shape, fr_pvc);
	fr_vcq_queue_add(fr_pvc, TRUE);
	break;
    case SHAPE_CREATE:
	/*
	 * install active configuration
	 */
	if (!shape) {
	    new_shape_entry = TRUE;
	    shape = malloc_named(sizeof(traffic_shape_t), "shaping queue");
	    if (!shape) {
		buginf("\nconfiguration failed");
		return;
	    }
	    shape->shape_fair = NULL;        
	    shape->shape_oqueue = fr_enqueue;
	    shape->shape_dequeue = fr_vc_dequeue;
	    shape->shape_dlci_num = dlci_num;
	    shape->shape_idb = idb;
	    shape->shape_dynamic = TRUE;
	    fr_pvc->shape = shape;
	    traffic_shape_add_entry(idb, shape);
	}
	fr_set_shape_params(shape, fr_pvc);
	if (new_shape_entry) {
	    shape->shape_param_next = idb->traffic_shape;
	    idb->traffic_shape = shape;
	}
	reg_invoke_shape_cache(hwidb);
	fr_vcq_queue_add(fr_pvc, TRUE);
	break;
    case SHAPE_DELETE:
	/*
	 * remove active configuration
	 */
	*shapep = shape->shape_param_next; /* remove entry from list */
	status = raise_interrupt_level(NETS_DISABLE);
	mgd_timer_stop(&shape->shape_restart);
	while ((pak = shape->shape_dequeue(shape)))
	    datagram_done(pak);
	free(shape);
	reset_interrupt_level(status);
	fr_pvc->shape = NULL;
	traffic_shape_delete_entry();
	fr_vcq_queue_add(fr_pvc, FALSE);
	break;
    }
}


/*
 * fr_idb_config_shape
 * Enable/initialize shaping structures on an idb
*/
void fr_idb_config_shape (hwidbtype *hwidb,
			  enum FR_SHAPE_CONF sense)
{
    fr_pvc_info_type *fr_pvc;
    fr_idb_struct_type *fr_idb = hwidb->frame_relay_stuff;
    leveltype status;
    
    if (!fr_idb)
	return;
    fr_pvc = (fr_pvc_info_type *)fr_idb->fr_pvcQ.qhead;
    status = raise_interrupt_level(NETS_DISABLE);
    while (fr_pvc) {
	fr_shape_setup(fr_pvc->idb, fr_pvc, sense);
	fr_pvc = fr_pvc->next;
    }
    reset_interrupt_level(status);
}


void fr_traffic_setup (hwidbtype *hwidb,
		       parseinfo *csb)
{
    fr_idb_struct_type *fr_idb;
    
    fr_idb = hwidb->frame_relay_stuff; 
    fr_idb_config_shape(hwidb,csb->sense ? SHAPE_CREATE : SHAPE_DELETE);
}

/*
 * fr_bq_proc
 * Send out FR broadcasts that have been queued
 */
static forktype fr_bq_proc (void)
{
    paktype *pak;
    idbtype *swidb;
    hwidbtype *idb;
    frame_relay_hdr *hdr; 
    fr_idb_struct_type *fr_idb;
    fr_pvc_info_type *fr_pvc;
    ushort num_dlci;
    ulong pak_count, byte_count;
    list_element *element;
    
    while (TRUE) {
	process_sleep_for(ONESEC/FR_BQ_INT);
	FOR_ALL_HWIDBS_IN_LIST(HWIDBLIST_FR, element, idb) {
	    fr_idb = idb->frame_relay_stuff;
	    if (!fr_idb)
		continue;
	    pak_count = fr_idb->bq_pkts/FR_BQ_INT;
	    if (!pak_count)
		pak_count = 1; 
	    byte_count = fr_idb->bq_bytes/FR_BQ_INT;

	    /*
	     * This loop controls broadcast rate dequeuing.
	     * Each second is divided into FR_BQ_INT intervals.
	     * The byte count checks are done after the packet is sent so
	     * the actual count may go slightly above the defined limit.
	     */
	    
	    while ((pak_count > 0) && (byte_count > 0)  &&
		   (fr_idb->bq_pkts_cur > 0) && (fr_idb->bq_bytes_cur > 0)) {
		pak = p_dequeue(&fr_idb->fr_br_packetQ);
		if (!pak)
		    break;
		swidb = pak->if_output;
		hdr = (frame_relay_hdr *)pak->mac_start;
		num_dlci = DLCI_TO_NUM(GETSHORT(&hdr->dlci));
		fr_pvc = fr_idb->fr_dlci_tab[num_dlci];
		/*
		 * check if the pvc is active or not
		 */
		if (!fr_pvc) {
		    datagram_done(pak);
		    continue;
		}
		if (!idb->nokeepalive && !DLCI_ACTIVE(fr_pvc->pvc_status)) {
		    if (frame_relay_packet_debug(swidb, num_dlci))
			buginf("\nDLCI %d broadcast drop", num_dlci);
		    datagram_done(pak);
		    continue;
		}
		if (frpkt_debug) {
		    buginf("\nbroadcast dequeue");
		    switch (GETSHORT((uchar *)pak->datagramstart+ADDRLEN_FR)) {
		    case FR_IETF_IP:
		    case FR_ISIS:
		    case FR_ESIS:
		    case FR_CLNP:
                        if (frame_relay_packet_debug(swidb,
						     DLCI_TO_NUM(GETSHORT(&hdr->dlci))))
			    buginf("\n%s(o):Pkt sent on dlci %d(0x%x), NLPID 0x%2x(%s), "
			       "datagramsize %d", swidb->namestring,
			       DLCI_TO_NUM(GETSHORT(&hdr->dlci)), 
			       GETSHORT(&hdr->dlci), GETSHORT(&hdr->type),
			       link2name(pak->linktype), pak->datagramsize);
			break;
		    case FR_SNAP0PAD:
		    case FR_SNAP1PAD:
                        if (frame_relay_packet_debug(swidb, 
                              DLCI_TO_NUM(GETSHORT(pak->datagramstart))))
			    buginf("\n%s(o):Pkt sent on dlci %d(0x%x), "
			       "pkt encaps 0x%4x 0x%4x 0x%4x"
			       " 0x%4x(%s), datagramsize %d", swidb->namestring,
			       DLCI_TO_NUM(GETSHORT(pak->datagramstart)),
			       GETSHORT(pak->datagramstart),
			       GETSHORT((ushort *)(pak->datagramstart+ADDRLEN_FR)),
			       GETSHORT((ushort *)(pak->datagramstart+4)),
			       GETSHORT((ushort *)(pak->datagramstart+6)),
			       GETSHORT((ushort *)(pak->datagramstart+8)),
			       link2name(pak->linktype),
			       pak->datagramsize);
			break;
		    default:
                        if (frame_relay_packet_debug(swidb,
						     DLCI_TO_NUM(GETSHORT(&hdr->dlci))))
			    buginf("\n%s(o):Pkt sent on dlci %d(0x%x), pkt type 0x%2x(%s), "
			       "datagramsize %d", swidb->namestring,
			       DLCI_TO_NUM(GETSHORT(&hdr->dlci)), 
			       GETSHORT(&hdr->dlci), GETSHORT(&hdr->type),
			       link2name(pak->linktype), pak->datagramsize);
			break;
		    }
		}
		/*
		 * Since we always dequeue the packet, we have already checked the
		 * board encap
		 */
		byte_count -= pak->datagramsize;
		pak_count--;
		fr_idb->bq_pkts_cur--;
		fr_idb->bq_bytes_cur -= pak->datagramsize;
		pak->fr_flags &= ~PAK_FR_BROADCAST;  

		if ((!pak->if_output) ||
		    (!pak->if_output->hwptr)) { 
		    errmsg(&msgsym(NULL_INT, FR));
		    datagram_done(pak);
		    continue;
		}		

		/* ensure the pak is for this dlci */
		if (pak->if_output != fr_pvc->idb) { 
		    if (if_is_ddr(pak->if_output->hwptr) &&
			(pak->if_output->hwptr ==
			reg_invoke_dialer_get_conn_member(fr_pvc->idb->hwptr))) {
                    } else {
			errmsg(&msgsym(INCORRECT_INT, FR));
			datagram_done(pak);
			continue;
		    }
		}

		fr_pvc->pvc_stats.num_pkts_broad++;
		fr_pvc->pvc_stats.num_bytess_broad += pak->datagramsize;
		datagram_out(pak);
	    }
	    fr_idb->bq_cur_int--;
	    if (!fr_idb->bq_cur_int) {
		/*
		 * Reset the values for the full interval
		 */
		fr_idb->bq_cur_int = FR_BQ_INT;
		fr_idb->bq_pkts_cur = fr_idb->bq_pkts;
		fr_idb->bq_bytes_cur = fr_idb->bq_bytes;
	    }
	}
    }
}

/*
 * fr_check_vector
 * Check for idb vector changes
 */
static void fr_chk_vector (hwidbtype *idb)
{
    fr_idb_struct_type *fr_idb;
    register leveltype level;

    if (!idb || !is_frame_relay(idb))
	return;
    if (idb->oqueue == fr_oqueue)
	return;
    level = raise_interrupt_level(NETS_DISABLE);
    fr_idb = idb->frame_relay_stuff;
    fr_idb->save_oqueue = idb->oqueue;
    idb->oqueue = fr_oqueue;
    reset_interrupt_level(level);
}

/*
 * fr_traffic_proc_start
 * Create processes when frame relay enabled
*/
static void fr_traffic_proc_start (void)
{
    reg_add_interface_cfg_modified(fr_chk_vector,"frame relay vector check"); 
    process_create(fr_bq_proc,"FR Broadcast Output", NORMAL_STACK,
		   PRIO_HIGH);
    mgd_timer_init_parent(&fr_idb_maint,NULL);
}

/*
 * fr_traffic_mapclass_parse
 * Handle parameters shared with SVCs
 */
static void fr_traffic_mapclass_parse (parseinfo *csb)
{
    mapclass_type *class_ptr = csb->mapclass;
    fr_params_type *fr_params = class_ptr->params;
    list_element *element;
    hwidbtype *hwidb;

    if (csb->sense) {
	/*
	 * if no parameters configured yet, initialize parameter list
	 */
	if (!fr_params) {
	    fr_vcq_mapparams(class_ptr,NULL);
	    if (!(fr_params = class_ptr->params))
		return;
	}
	switch (csb->which) {
	case FR_MAP_CLASS_CBS:
	  fr_params->vcq_params->bc = GETOBJ(int, 1);
	  break;
	case FR_MAP_CLASS_EBS:
	  fr_params->vcq_params->be = GETOBJ(int, 1);
	  break;
	case FR_MAP_CLASS_IN_CBS:
	case FR_MAP_CLASS_OUT_CBS:
	case FR_MAP_CLASS_IN_EBS:
	case FR_MAP_CLASS_OUT_EBS:
	  break;
  	case FR_MAP_CLASS_CIR:
	  fr_params->vcq_params->cir = GETOBJ(int, 1);
	  break;
	case FR_TRAFFIC_BECN:
	  fr_params->vcq_params->fr_becn_response = TRUE;
	  break;
	case FR_TRAFFIC_RATE:
	  fr_params->vcq_params->fr_ave_rate = GETOBJ(int,1);
	  fr_params->vcq_params->fr_peak_rate = GETOBJ(int,2);
	  break;
	case FR_MAP_CLASS_IN_CIR:
	case FR_MAP_CLASS_OUT_CIR:
	}
    } else {
	if (!fr_params)
	    return;
	if (!fr_params->vcq_params)
	    return;
	switch (csb->which) {
	case FR_TRAFFIC_BECN:
	  fr_params->vcq_params->fr_becn_response = FALSE;
	  break;
	case FR_TRAFFIC_RATE:
	  fr_params->vcq_params->fr_ave_rate = 0;
	  fr_params->vcq_params->fr_peak_rate = 0;
	  break;
	}
    }
    FOR_ALL_HWIDBS_IN_LIST(HWIDBLIST_FR, element, hwidb)
	if (fr_tr_on_idb(hwidb))
	    fr_idb_config_shape(hwidb, SHAPE_UPDATE);
}

/*
 * fr_traffic_mapclass_nvgen
 * Generate mapclass parameters related to traffic shaping
*/
static void fr_traffic_mapclass_nvgen (mapclass_type *classp)
{
    fr_params_type *fr_params = classp->params;
	
    if (classp->encap != ET_FRAME_RELAY) {
	return;
    }
    if (!fr_params)
	return;
    if (!fr_params->vcq_params)
	return;
    nv_write(fr_params->vcq_params->fr_ave_rate,
	     "frame-relay traffic-rate %d",fr_params->vcq_params->fr_ave_rate);
    nv_add(fr_params->vcq_params->fr_peak_rate," %d",
	   fr_params->vcq_params->fr_peak_rate);
    nv_write(!fr_params->vcq_params->fr_becn_response,
	     "no frame-relay becn-response");
    nv_write(fr_params->vcq_params->cir,"frame-relay cir %d",
	     fr_params->vcq_params->cir);
    nv_write(fr_params->vcq_params->bc,"frame-relay bc %d",
	     fr_params->vcq_params->bc);
    nv_write(fr_params->vcq_params->be,"frame-relay be %d",
	     fr_params->vcq_params->be);
}

/*
 * Start traffic shaping and broadcast queue
*/
static void fr_traffic_start (hwidbtype *hwidb)
{
}

/*
 * fr_broadcast_setup
 * SEt up broadcast and parameters
*/
static void fr_broadcast_setup (hwidbtype *hwidb)

{
  fr_idb_struct_type *fr_idb = NULL;

  fr_idb = hwidb->frame_relay_stuff;
  if (!fr_idb)
    return;
  

    queue_init(&fr_idb->fr_br_packetQ,DEF_BQSIZE);
    fr_idb->fr_traffic_shape = FALSE;
    queue_init(&fr_idb->fr_pvcQ,0l);
    reg_add_fr_interface_display(fr_bq_int_display,
				 "Display broadcast queue info");
    fr_idb->bq_bytes = DEF_BQBYTES;
    fr_idb->bq_bytes_cur = DEF_BQBYTES;
    fr_idb->bq_pkts = DEF_BQPKTS;
    fr_idb->bq_pkts_cur = DEF_BQPKTS;
    fr_idb->bq_cur_int = FR_BQ_INT;
    fr_idb->save_oqueue = hwidb->oqueue;
    hwidb->oqueue = fr_oqueue;
}

/*
 * fr_traffic_stop
 * De-configure traffic shaping on an idb
*/
static void fr_traffic_stop (hwidbtype *hwidb)
{
    fr_idb_struct_type *fr_idb = NULL;

    fr_idb =  hwidb->frame_relay_stuff;
    if (!fr_idb)
        return;
    if (idb_vcq && idb_vcq->fr_vcq_enable) {
    }
}

/*
 * fr_broadcast_stop
 * Clear interface broadcast queue and vectors
*/
static void fr_broadcast_stop (hwidbtype *hwidb)
{
    fr_idb_struct_type *fr_idb = NULL;

    fr_idb =  hwidb->frame_relay_stuff;
    if (!fr_idb)
        return;
    hwidb->oqueue = fr_idb->save_oqueue;
    fr_idb->save_oqueue = NULL;
}

/*
 * fr_traffic_init
 * initialize traffic shaping in the subsystem
*/
void fr_traffic_init ()
{
    reg_add_fr_proc_create(fr_traffic_proc_start,
			   "Create and start traffic shaping");
    reg_add_fr_idb_init(fr_traffic_start,"Config interface traffic shaping");
    reg_add_fr_idb_uninit(fr_traffic_stop,"Deconfig int. traffic shaping");
    reg_add_fr_dlci_create(fr_shape_setup,"Add shaping struct for VC");
    reg_add_fr_dlci_destroy(fr_shape_setup,"Remove shaping struct for VC");
    reg_add_fr_idb_init(fr_broadcast_setup, "Broadcast queue setup");
    reg_add_fr_mapclass_parse(fr_traffic_mapclass_parse,
			      "FR mapclass parameter parsing");
    reg_add_fr_mapclass_nvgen(fr_traffic_mapclass_nvgen,
			      "fr_traffic_mapclass_nvgen");
    reg_add_fr_show_pvc(fr_vc_shape_display, "fr_vc_shape_display");
    reg_add_fr_idb_uninit(fr_broadcast_stop, "Broadcast queue cleanup");
}



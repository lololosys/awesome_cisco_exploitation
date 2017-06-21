/* $Id: fr_vcq.c,v 3.1.2.18 1996/09/11 16:49:42 liqin Exp $
 * $Source: /release/112/cvs/Xsys/wan/fr_vcq.c,v $
 *------------------------------------------------------------------
 * Frame Relay VC Queueing
 *
 * March 1996, Charles Poe
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: fr_vcq.c,v $
 * Revision 3.1.2.18  1996/09/11  16:49:42  liqin
 * CSCdi68280:  replace malloc with malloc_named in frame relay code
 * Branch: California_branch
 *
 * Revision 3.1.2.17  1996/08/24  05:15:07  ccpoe
 * CSCdi66212:  Turning off traffic-shaping causes crash in FR VC Queueing
 * Branch: California_branch
 *
 * Revision 3.1.2.16  1996/08/15  19:32:47  fowler
 * CSCdi60278:  Removing frame-relay traffic shaping from an int causes
 *              traceback
 *              nvgen fixes and timer fix for de-encapsulation
 * Branch: California_branch
 *
 * Revision 3.1.2.15  1996/08/09  20:37:54  ccpoe
 * CSCdi59994:  Traceback messages in configuring vc queueing. Default
 * queueing can be invoked for changing or new VC queueing. For new VC,
 * queueing data structure need to be there before accessing.
 * Branch: California_branch
 *
 * Revision 3.1.2.14  1996/08/06  19:06:20  achopra
 * CSCdi63640:  crash in _fr_enqueue
 * Branch: California_branch
 *
 * Revision 3.1.2.13  1996/07/31  22:11:10  achopra
 * CSCdi64288:  VC queueing commands cannot be removed from under
 * map-class command
 * Branch: California_branch
 *
 * Revision 3.1.2.12  1996/07/30  01:15:06  ccartee
 * CSCdi60470:  Custom-queue-list command cant be applied to channelized
 * T1 interfac
 * Branch: California_branch
 * Change name of encaps_queueing to encaps_queuing_allowed and change
 * type to RETVAL.  Insure that default behavior is to allow queuing
 * commands on interface.  Cauterize legacy references encaps_queuing to
 * remove references to passed in (ptr to) return value.
 *
 * Revision 3.1.2.11  1996/07/19  02:55:09  ccpoe
 * CSCdi53793:  Warning needed in map-class configuration for FR VC
 * Queueing.
 * Branch: California_branch
 *
 * Revision 3.1.2.10  1996/07/18  05:23:29  ccpoe
 * CSCdi53787:  Interface queueing shouldnt be allowed with VC Queueing
 * Branch: California_branch
 *
 * Revision 3.1.2.9  1996/06/28  23:32:07  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.1.2.8  1996/06/18  23:51:47  ccpoe
 * CSCdi60277:  priority-group command cannot be deleted from map-class
 * Branch: California_branch
 *
 * Revision 3.1.2.7  1996/06/10  20:21:15  ccpoe
 * CSCdi60001:  Router crashes in fr_traffic_command. NV gen needs the idb
 * validation as well.
 * Branch: California_branch
 *
 * Revision 3.1.2.6  1996/06/10  18:47:05  ccpoe
 * CSCdi60001:  Router crashes in fr_traffic_command. Some frame relay
 * traffic shaping commands are not interface specific. idb validation is
 * required.
 * Branch: California_branch
 *
 * Revision 3.1.2.5  1996/05/29  06:35:01  fowler
 * CSCdi58766:  Link errors in 11.2 build
 * Branch: California_branch
 *
 * Revision 3.1.2.4  1996/05/24  21:21:37  ccpoe
 * CSCdi58460:  map-class frame-relay traffic-shaping features dont work
 * on FR SVC
 * Branch: California_branch
 *
 * Revision 3.1.2.3  1996/05/17  21:03:31  ccpoe
 * CSCdi57874:  IDB clear counters will lead to crash.
 * Branch: California_branch
 *
 * Revision 3.1.2.2  1996/05/17  12:16:19  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.1.2.1  1996/05/15  05:54:48  fowler
 * Frame relay VC queuing and rate enforcement
 * Branch: California_branch
 *
 * Revision 3.1  1996/04/26  05:48:03  ccpoe
 * placeholder
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
#include "address.h"
#include "../if/priority_private.h"
#include "../if/static_map.h"
#include "../ip/ip.h"
#include "../tcp/tcp.h"
#include "frame_relay.h"
#include "fr_vcq.h"
#include "fr_traffic_shape.h"
#include "fr_debug.h"
#include "fr_class.h"
#include "../if/network_debug.h"
#include "../if/network.h"
#include "parser.h"
#include "parser_defs_frame.h"
#include "../parser/parser_defs_priority.h"
#include "../xns/novell.h"
#include "../wan/fr_registry.h"


#define FR_VCQ_MAJVERSION 1
#define FR_VCQ_MINVERSION 0
#define FR_VCQ_EDITVERSION 1


SUBSYS_HEADER(fr_vcq, FR_VCQ_MAJVERSION, FR_VCQ_MINVERSION,
	      FR_VCQ_EDITVERSION, fr_vcq_init, SUBSYS_CLASS_PROTOCOL,
	      "seq: frame_relay",
	      "req: frame_relay");


#define QUEUING_NOT_ALLOWED (QUEUE_CUSTOM|QUEUE_PRIORITY)
/*
 * fr_vcq_queue2name
 *
 * Given an vc and a queue number, return queue name.
 */

static char *fr_vcq_queue2name (
    fr_pvc_info_type *vc,
    int queue)
{
    static char namenum[8];

    if (vc->fr_vcq->custom_queuenum) {
	sprintf(namenum, "%d", queue);
	return(namenum);
    }
    return(priority2name(queue));
}

/*
 * Check if VC Queueing and interface queueing are mutual exclusive
 * Returns: TRUE  - fancy queueing is allowed
 *          FALSE - fancy queueing is NOT allowed
 */
static boolean fr_vcq_intq_chk (hwidbtype *hwidb, ulong queue_type)
{
    fr_idb_struct_type *fr_idb;
    boolean queuing_allowed = TRUE;

    if (!hwidb->frame_relay_stuff)
	return(queuing_allowed);	

    fr_idb = hwidb->frame_relay_stuff; 
    if (queue_type & QUEUING_NOT_ALLOWED) {
	if (hwidb->queue_algorithm == QUEUE_PRIORITY) {
	    printf("Must remove interface priority queueing\n");
	    queuing_allowed = FALSE;
	}
    }
    
    if (queue_type == QUEUE_PRIORITY || queue_type == QUEUE_CUSTOM) {
	if (idb_vcq->fr_vcq_enable == TRUE) {
	    printf("Must remove or disable Frame Relay traffic shaping\n");
	    queuing_allowed = FALSE;
	}
    }
    return(queuing_allowed);
}
    

/*
 * fr_vcq_prilist_chk
 * 	check if priority-list, queue-list is valid for VC queueing
 */
static boolean fr_vcq_prilist_chk (int list_no, enum fr_vcq_type vcq_type)
{
    pritype *p;
    plisttype *plist;
    
    switch(vcq_type) {
    case FR_VCQ_CUSTOM:
	p = cust_parray[list_no];
	break;
    case FR_VCQ_PRIORITY:
	p = parray[list_no];
	break;
    case FR_VCQ_FCFS:
	return TRUE;
    default:
	return FALSE;
    }

    if (p == NULL) {
	return TRUE;
    }
    for (plist = p->plist; plist; plist = plist->next) {
	if (plist->interface) {
	    return FALSE;
	}
    }
    return TRUE;
}


static inline struct VCQ *vcq_create (void)
{
    struct VCQ *vcq;
    int i;

    vcq = malloc_named(sizeof(struct VCQ), "FR VC Queueing");
    vcq->queue_algorithm = FR_VCQ_FCFS;
    vcq->oqueue = fr_vcq_fcfs_enqueue;
    vcq->oqueue_dequeue = fr_vcq_fcfs_dequeue;
    vcq->priority_list = 0;

    for (i = 0; i < PRIORITY_MAXTYPE; i++) {
	queue_init(&vcq->outputq[i], FR_VCQ_HOLDQ);
	vcq->output_defqcount[i] = FR_VCQ_HOLDQ;
    }

    return vcq;
}


    
/*
 * fr_vcq_write
 *	Associate a VCQ structure with a map-class.
 *      Caller is responsible to disable the traffic.
 *	1. If vcq is NULL, no class is associated with this vc.
 *	2. change existing vc queueing.
 *	3. config vc queueing from the existing class configured
 *	   (turn on vc queueing enable after vc queueing is configured.
 *      
 */
static void fr_vcq_write (fr_pvc_info_type *pvc, struct VCQ *vcq)
{
    struct VCQ *vcq_old;
    struct VCQ *vcq_new;

    if (vcq == NULL) {
	if (!pvc->fr_vcq) {
	    vcq = vcq_create();
	    pvc->fr_vcq = vcq;
	    vcq->vcq_vc = pvc;
	} else {
	    pvc->fr_vcq->queue_algorithm = FR_VCQ_FCFS;
	    pvc->fr_vcq->oqueue = fr_vcq_fcfs_enqueue;
	    pvc->fr_vcq->oqueue_dequeue = fr_vcq_fcfs_dequeue;
	    pvc->fr_vcq->priority_list = 0;
	}
	return;
    }

    vcq_old = pvc->fr_vcq;
    if (vcq_old) {
	*vcq_old = *vcq; /* struct copy */
	vcq_old->vcq_vc = pvc;
    } else {
	vcq_new = vcq_create();
	*vcq_new = *vcq; /* struct copy */
	pvc->fr_vcq = vcq_new;
	pvc->fr_vcq->vcq_vc = pvc;
    }
}

/*
 * fr_vcq_priority_qsize
 * Return desired size of an interface's output queue
 */

static int fr_vcq_priority_qsize (fr_pvc_info_type *pvc, int index)
{
    pritype *p;

    if (pvc->fr_vcq->priority_list) {
	p = VCQ_PRIORITY_ARRAY(pvc);
	if (p)
	    return(p->qsize[index]);
	if (pvc->fr_vcq->custom_queuenum)
	    return(CUSTOM_QUEUE_SIZE);
	else
	    return(PRIORITY_QLIMIT*(index+1));
    }
    return(pvc->fr_vcq->output_defqcount[index]);
}


/*
 * fr_vcq_custom_qbytecnt
 * Return desired packet byte count for an vc's custom output
 * queue.
 */

static ulong fr_vcq_custom_qbytecnt (fr_pvc_info_type *vc, int index)
{
    pritype *p;

    p = VCQ_PRIORITY_ARRAY(vc);
    return(p ? p->bytecount[index] : CUSTOM_QUEUE_PACKET_SIZE);
}


/*
 * fr_vcq_reset
 *	reset the vc queue by clearing the vc output queue and reset the
 *	hw interface if 1. first fancy vc queueing is configured on the
 *			   hwidb.
 *			2. last fancy vcq queueing is removed from the
 *			   hwidb.
 *	Note: raise_interrupt_levle is necessary since this function is
 *	      not only invoked by fr_vcq_queue_setup() and also be called
 *	      by prilist_command when there is a priority-list change.
 */
static void fr_vcq_reset (fr_pvc_info_type *pvc)
{

    struct VCQ *vcq;
    paktype *pak;
    int status, i, qsize;


    vcq = pvc->fr_vcq;

    /*
     * flush the existing interface queue if no existing pvc queue
     */
    status = raise_interrupt_level(NETS_DISABLE);
    for (i = 0; i < PRIORITY_MAXTYPE; i++) {
	/*
	 *  flush queue contents
	 */
	while (vcq->outputq[i].qhead) {
	    pak = pak_dequeue(&vcq->outputq[i]);
	    pak->flags &= ~PAK_DLQ;
	    datagram_done(pak);
	}
	qsize = 0;
	/*
	 *  adjust queue limit
	 */
	if (vcq->priority_list) {
	    qsize = fr_vcq_priority_qsize(pvc,i);
	} else {
	    qsize = vcq->outputq[VCQ_DEFAULT_QUEUE(pvc)].maximum;
	}
	queue_init(&(vcq->outputq[i]), qsize);
    }
    /*
     *  clear the count of packets waiting for transmit
     */
    vcq->output_qcount = 0;
    reset_interrupt_level(status);
}

/*
 * fr_vcq_mapparams
 *	allocate frame relay vc queueing map class params
 */
void fr_vcq_mapparams (mapclass_type *class_ptr, struct VCQ *vcq)
{
    fr_params_type *params;

    if ((params = (fr_params_type *) class_ptr->params)) {
	if (params->vcq_params) {
	    return;
	} else {
	    params->vcq_params = malloc_named(sizeof(struct fr_vcq_params),
					      "FR VCQ Mapclass Param");
	}
    } else {
	params = malloc_named(sizeof(fr_params_type),
			      "FR Mapclass Param");
	params->svc_params = NULL;
	params->vcq_params = malloc_named(sizeof(struct fr_vcq_params),
					  "FR VCQ Mapclass Param");
    }

    params->vcq_params->map_vcq_cnt = 0;
    params->vcq_params->fr_becn_response = TRUE;
    params->vcq_params->fr_ave_rate = 0;
    params->vcq_params->fr_peak_rate = 0;
    params->vcq_params->fr_becn_response = TRUE;
    if (vcq == NULL) {
	/*
	 * When fr class is issued, and no map-class is yet configured.
	 */
	vcq = vcq_create();
    }
    params->vcq_params->map_vcq = vcq;
    class_ptr->params = params;
}


/*
 *  fr_vcq_map_assign
 *      return TRUE if a mapclass is already associated with a VC
 *      return FALSE otherwise.
 */
boolean fr_vcq_map_assign (mapclass_type *classp)
{
    struct fr_vcq_params *params;

    params = get_vcq_params(classp);
    if (params) {
	if (params->map_vcq_cnt) {
	    return TRUE;
	}
    }
    return FALSE;
}
    
    
void fr_vcq_mapclass_nvgen (mapclass_type *classp)
{
    struct fr_vcq_params *params;
    struct VCQ *vcq;
    
    params = get_vcq_params(classp);
    if (params == NULL) {
	return;
    }
    if (!(vcq = params->map_vcq)) {
	return;
    }
    
    switch (vcq->queue_algorithm) {
    case FR_VCQ_CUSTOM:
	nv_write(TRUE,"%s %d", "frame-relay custom-queue-list",
		 vcq->priority_list);
	break;
    case FR_VCQ_PRIORITY:
	nv_write(TRUE,"%s %d", "frame-relay priority-group",
		 vcq->priority_list);
	break;
    default:
	    
    }
}


/*
 * fr_vcq_unmap_vc
 *	remove the link between mapclass and a vc
 */
static void fr_vcq_unmap_vc (fr_pvc_info_type *pvc)
{
    fr_idb_struct_type *fr_idb;
    struct fr_vcq_params *params;
    hwidbtype *hwidb;

    hwidb = pvc->idb->hwptr;
    GET_FR_IDB(hwidb, fr_idb);
    params = get_vcq_params(pvc->pvc_class);
    if (params) {
	params->map_vcq_cnt--;
	fr_vcq_default_queueing(pvc);
    }
}
    

/*
 * fr_vcq_map_vc
 *	associate mapclass with a vc. all the null pointer checking are
 *      caller's responsibility
 */
static void fr_vcq_map_vc (fr_pvc_info_type *pvc,
		    mapclass_type *classp)
{
    fr_idb_struct_type *fr_idb;
    struct fr_vcq_params *params;
    hwidbtype *hwidb;

    hwidb = pvc->idb->hwptr;
    GET_FR_IDB(hwidb, fr_idb);

    params = get_vcq_params(classp);
    if (params) {
	params->map_vcq_cnt++;
	pvc->pvc_class = classp;
	fr_vcq_queue_setup(pvc, params->map_vcq);
    }
}

static void fr_vcq_mapclass_remove (mapclass_type *class_ptr)
{
    fr_params_type *params;
    
    if ((params = class_ptr->params)) {
	if (params->vcq_params) {
	    free(params->vcq_params);
	}
	params->vcq_params = NULL;
    }
}

/*
 * fr_vcq_mapclass
 *	associate a queueu-list or priority-list with a map-class.
 */
void fr_vcq_mapclass (parseinfo *csb)
{
    struct fr_vcq_params *params;
    mapclass_type *classp;    
    struct VCQ *vcq, *exist_vcq = NULL;
    hwidbtype *hwidb;
    fr_pvc_info_type *pvc = NULL;
    int i;
    list_element *element;
    

    classp = csb->mapclass;
    params = get_vcq_params(classp);
    if (params == NULL) {
	if (!csb->sense) {
	    return;
	}
	vcq = vcq_create();
    } else {
	vcq = params->map_vcq;
	exist_vcq = vcq;
    }
    
    /*
     * print out the warning if changing existing VC queueing
     */
    if (csb->sense && exist_vcq) {
	if (exist_vcq->queue_algorithm != csb->which) {
	    if (exist_vcq->queue_algorithm == FR_VCQ_CUSTOM) {
		printf("Must remove frame-relay vc custom queueing\n");
		return;
	    }
	    if (exist_vcq->queue_algorithm == FR_VCQ_PRIORITY) {
		printf("Must remove frame-relay vc priority queueing\n");
		return;
	    }
	}
    }
    vcq->priority_list = GETOBJ(int, 1);
    vcq->queue_algorithm = csb->which;
    if (!csb->sense) {
	/*
	 * Reset all the vc associate with this class
	 */
	FOR_ALL_HWIDBS_IN_LIST(HWIDBLIST_FR, element, hwidb) {
	    if (hwidb->frame_relay_stuff) {
		for (i = MIN_DLCI; i <= MAX_DLCI; i++) {
		    if ((pvc = hwidb->frame_relay_stuff->fr_dlci_tab[i])) {
			if (pvc->fr_vcq && pvc->pvc_class == classp) {
			    fr_vcq_unmap_vc(pvc);
			    if (fr_tr_on_idb(hwidb)) 
				fr_shape_setup(pvc->idb, pvc, SHAPE_UPDATE);
			}
		    }
		}
	    }
	}
	fr_vcq_mapclass_remove(classp);
	return;
    }
    
    if (!fr_vcq_prilist_chk(GETOBJ(int, 1), vcq->queue_algorithm)) {
	vcq_delete(vcq);
	return;
    }
    fr_vcq_mapparams(classp, vcq);
    FOR_ALL_HWIDBS_IN_LIST(HWIDBLIST_FR, element, hwidb) {
	if (hwidb->frame_relay_stuff &&
	    hwidb->frame_relay_stuff->fr_vcq->fr_vcq_enable) {
	    for (i = MIN_DLCI; i <= MAX_DLCI; i++) {
		if ((pvc = hwidb->frame_relay_stuff->fr_dlci_tab[i])) {
		    if (pvc->fr_vcq && pvc->pvc_class == classp) {
			fr_vcq_queue_setup(pvc, vcq);
			if (fr_tr_on_idb(hwidb)) 
			    fr_shape_setup(pvc->idb, pvc, SHAPE_UPDATE);
		    }
		}
	    }
	}
    }
}


void fr_vcq_intf_queueing (hwidbtype *hwidb)
{
    fr_idb_struct_type *fr_idb;
    fr_pvc_info_type *pvc;
    int i;
    
    GET_FR_IDB(hwidb, fr_idb);
    /*
     * flush all the vcq queues
     */
    for (i = MIN_DLCI; i < MAX_DLCI; i++)  {
	pvc = fr_idb->fr_dlci_tab[i];
	if (pvc) {
	    fr_vcq_queue_add(pvc, FALSE);
	}
    }
}


/*
 * fr_vcq_queue_add
 *	set up a vc queueing for a vc
 */
void fr_vcq_queue_add (fr_pvc_info_type *pvc, boolean ADD_VCQ)
{
    struct fr_vcq_params *params = NULL;
    fr_idb_struct_type *fr_idb;
    mapclass_type *classp;
    struct VCQ *vcq = NULL;
    hwidbtype *hwidb;
    paktype *pak;
    int j;

    hwidb = pvc->idb->hwptr;
    GET_FR_IDB(hwidb, fr_idb);

    if (!fr_idb->fr_vcq->fr_vcq_enable) {
	return;
    }
    
    if (pvc->pvc_type == SVC_DYNAMIC) {
	fr_vcq_frclass(pvc->pvc_class, pvc);
	return;
    }

    classp = pvc->pvc_class;
    params = get_vcq_params(classp);
    if (params && ADD_VCQ) {
	vcq = params->map_vcq;
    }

    if (ADD_VCQ) {
	fr_vcq_queue_setup(pvc, vcq);
    } else {
	vcq = pvc->fr_vcq;
	for (j = 0; j < PRIORITY_MAXTYPE; j++) {
	    /*
	     *  flush queue contents
	     */
	    while (vcq->outputq[j].qhead) {
		pak = pak_dequeue(&vcq->outputq[j]);
		pak->flags &= ~PAK_DLQ;
		datagram_done(pak);
	    }		
	}
	if ((params = get_vcq_params(pvc->pvc_class))) {
	    params->map_vcq_cnt--;
	}
	vcq_delete(pvc->fr_vcq);
	pvc->fr_vcq = NULL;
    }
}


/*
 * fr_vcq_onoff
 *	Turn on/off fr vc queueing.
 *	Needs to reset the hwidb TQL (transmit queue length).
 */
static void fr_vcq_onoff (hwidbtype *hwidb, boolean ON)
{
    fr_idb_struct_type *fr_idb;
    fr_pvc_info_type *pvc;
    int status, i;

    GET_FR_IDB(hwidb, fr_idb);

    status = raise_interrupt_level(NETS_DISABLE);
    /* Turn ON VC queueing */
    if (ON == TRUE) {
	if (idb_vcq->fr_vcq_enable == FALSE) {
	    idb_vcq->fr_vcq_enable = TRUE;
	    for (i = MIN_DLCI; i <= MAX_DLCI; i++) {
		if ((pvc = fr_idb->fr_dlci_tab[i])) {
		    fr_vcq_queue_add(pvc, TRUE);
		}
	    }
	}
    }

    /* Turn OFF VC queueing */
    if (ON == FALSE) {
	if (idb_vcq->fr_vcq_enable == TRUE) {
	    fr_vcq_intf_queueing(hwidb);
	    idb_vcq->fr_vcq_enable = FALSE;
	}
    }
    reset_interrupt_level(status);
}



/*
 * fr_vcq_frclass
 *	Assumption: frame-relay class will be passed valid
 *	            classp and pvc
 *	         
 */ 
void fr_vcq_frclass (mapclass_type *classp, fr_pvc_info_type *pvc)
{
    struct fr_vcq_params *params;
    struct VCQ *vcq = NULL;
    
    if (pvc == NULL) {
	return;
    }

    if (classp == NULL) {
	fr_vcq_default_queueing(pvc);
	return;
    }
	
    params = get_vcq_params(classp);
    if (!params) {
	fr_vcq_mapparams(classp, vcq);
    }
    
    if (pvc->fr_vcq) {
	fr_vcq_unmap_vc(pvc);
    }
    fr_vcq_map_vc(pvc, classp);
}


void fr_traffic_command (parseinfo *csb)
{
    hwidbtype *hwidb = NULL;
    fr_idb_struct_type *fr_idb = NULL;
    list_element *element;

    if (csb->interface) {
	hwidb = csb->interface->hwptr;
    }
    if (hwidb) {
	fr_idb = hwidb->frame_relay_stuff;
    }
    
    if (csb->nvgen) {
	switch(csb->which) {
	case FR_TRAFFIC_SWITCH:
	    if (!fr_idb) {
		return;
	    }
	    if (idb_vcq && fr_idb->fr_traffic_shape) {
		nv_write(TRUE, "%s", csb->nv_command);
	    }
	break;
	case FR_BQ:
	    if (!fr_idb) {
		return;
	    }
	    if ((fr_idb->fr_br_packetQ.maximum != DEF_BQSIZE) ||
		(fr_idb->bq_bytes != DEF_BQBYTES) ||
		(fr_idb->bq_pkts != DEF_BQPKTS)) {
		nv_write(TRUE,"%s %d ", csb->nv_command,
			 fr_idb->fr_br_packetQ.maximum);
		nv_add(TRUE,"%d ",fr_idb->bq_bytes);
		nv_add(TRUE,"%d ",fr_idb->bq_pkts);
	    }
	    break;
	default:
	}
	return;
    }
	
    switch(csb->which) {
    case FR_TRAFFIC_SWITCH:
	if (fr_vcq_intq_chk(hwidb, QUEUING_NOT_ALLOWED) && hwidb) {
	    fr_idb->fr_traffic_shape = csb->sense;
	    fr_traffic_setup(hwidb, csb);
	    fr_vcq_onoff(hwidb, csb->sense);
	}
	break;

    case FR_VCQ_CUSTOM:
    case FR_VCQ_PRIORITY:
	fr_vcq_mapclass(csb);
	FOR_ALL_HWIDBS_IN_LIST(HWIDBLIST_FR, element, hwidb) 
	    if (fr_tr_on_idb(hwidb))
		fr_idb_config_shape(hwidb, SHAPE_UPDATE);
	break;
	
    case FR_BQ:
	if (!fr_idb) {
	    return;
	}
	if (!csb->sense) {
	    fr_idb->fr_br_packetQ.maximum = DEF_BQSIZE;
	    fr_idb->bq_bytes = DEF_BQBYTES;
	    fr_idb->bq_bytes_cur = DEF_BQBYTES;
	    fr_idb->bq_pkts = DEF_BQPKTS;
	    fr_idb->bq_pkts_cur = DEF_BQPKTS;
	} else {
	    fr_idb->fr_br_packetQ.maximum = GETOBJ(int,1);
	    fr_idb->bq_bytes = GETOBJ(int,2);
	    fr_idb->bq_pkts = GETOBJ(int,3);
	    fr_idb->bq_pkts_cur = fr_idb->bq_pkts;
	    fr_idb->bq_bytes_cur = fr_idb->bq_bytes;
	}
	break;
    case FR_TRAFFIC_RATE:
    case FR_TRAFFIC_BECN:
    case FR_MAP_CLASS_CBS:
    case FR_MAP_CLASS_EBS:
    case FR_MAP_CLASS_IN_CBS:
    case FR_MAP_CLASS_OUT_CBS:
    case FR_MAP_CLASS_IN_EBS:
    case FR_MAP_CLASS_OUT_EBS:
    case FR_MAP_CLASS_CIR:
    case FR_MAP_CLASS_IN_CIR:
    case FR_MAP_CLASS_OUT_CIR:
	reg_invoke_fr_mapclass_parse(csb);
	FOR_ALL_HWIDBS_IN_LIST(HWIDBLIST_FR, element, hwidb) 
	    if (fr_tr_on_idb(hwidb))
		fr_idb_config_shape(hwidb, SHAPE_UPDATE);
    default:
    }
}


static void fr_vcq_idb_init (hwidbtype *hwidb)
{
    fr_idb_struct_type *fr_idb;
    struct fr_idb_vcq *vcqp;

    GET_FR_IDB(hwidb, fr_idb);

    vcqp = malloc_named(sizeof(struct fr_idb_vcq), "FR IDB VCQ");
    vcqp->fr_vcq_enable = FALSE;
    vcqp->fr_vcq_index = 0;
    fr_idb->fr_vcq = vcqp;
}
    
static void fr_vcq_idb_uninit(hwidbtype *hwidb)
{
    fr_idb_struct_type *fr_idb;

    GET_FR_IDB(hwidb, fr_idb);
    fr_vcq_onoff(hwidb, FALSE);

    free(idb_vcq);
}


/*
 * fr_vcq_priority_classify
 * Return priority level for a given datagram and output interface.
 */

static int fr_vcq_priority_classify (fr_pvc_info_type *output_vc, paktype *pak)
{
    ushort typecode, sport, dport;
    plisttype *plist;
    char source[ADDRLEN_MAX*3], destination[ADDRLEN_MAX*3];
    hwidbtype *output;
    int queue;
    iphdrtype *ippak;

    output = output_vc->idb->hwptr;
    if (VCQ_PRIORITY_ARRAY(output_vc) == NULL) {
	queue = VCQ_DEFAULT_QUEUE(output_vc);
	if (priority_debug) {
	    buginf("\nPQ: %s dlci %d : %s -> %s",
		   output_vc->idb->namestring,
		   output_vc->this_dlci,
		   print_linktype(pak->linktype),
		   fr_vcq_queue2name(output_vc, queue));
	}
	return(queue);
    }

    for (plist = VCQ_PRIORITY_ARRAY(output_vc)->plist;
	 plist;
	 plist = plist->next) {

	if (plist->linkcode != pak->linktype) {
	    continue;
	}

	if (plist->stun_group) {
	    if (reg_invoke_stun_prioritization(plist, pak, output)) {
		return(plist->priority);
	    }
	    continue;
	}

	if (plist->bstun_group) {
	    if (reg_invoke_bstun_prioritization(plist, pak, output)) {
		return(plist->priority);
	    }
	    continue;
	}

	if (plist->gt_size) {
	    if (pak->datagramsize <= plist->gt_size) {
		continue;
	    }
	    if (priority_debug) {
		buginf("\nPQ: %s dlci: %s (%d bytes) -> %s",
		       output_vc->idb->namestring,
		       output_vc->this_dlci,
		       print_linktype(pak->linktype), pak->datagramsize,
		       fr_vcq_queue2name(output_vc, plist->priority));
	    }
	    return(plist->priority);
	}
	if (plist->lt_size) {
	    if (pak->datagramsize >= plist->lt_size) {
		continue;
	    }
	    if (priority_debug) {
		buginf("\nPQ: %s dlci %d : %s (%d bytes) -> %s",
		       output_vc->idb->namestring,
		       output_vc->this_dlci,
		       print_linktype(pak->linktype), pak->datagramsize,
		       fr_vcq_queue2name(output_vc, plist->priority));
	    }
	    return(plist->priority);
	}
	if (plist->tcp_port) {
	    ippak = (iphdrtype *)ipheadstart(pak);
	    if (ippak->prot != TCP_PROT)
		continue;
	    /*
	     * Fragmented IP packets do not have higher level header available
	     */
	    if (ippak->fo)
		continue;
	    sport = TcpPkt(pak)->sourceport;
	    dport = TcpPkt(pak)->destinationport;
	    if ((sport != plist->tcp_port) && (dport != plist->tcp_port)) {
		continue;
	    }
	    if (priority_debug) {
		buginf("\nPQ: %s dlci %d: %s (tcp %d) -> %s",
		       output_vc->idb->namestring,
		       output_vc->this_dlci,
		       print_linktype(pak->linktype), plist->tcp_port,
		       fr_vcq_queue2name(output_vc, plist->priority));
	    }
	    return(plist->priority);
	}
	if (plist->udp_port) {
	    ippak = (iphdrtype *)ipheadstart(pak);
	    if (ippak->prot != UDP_PROT)
		continue;
	    /*
	     * Fragmented IP packets do not have higher level header available
	     */
	    if (ippak->fo)
		continue;
	    sport = ((udptype *)ipdatastart(pak))->sport;
	    dport = ((udptype *)ipdatastart(pak))->dport;
	    if ((sport != plist->udp_port) && (dport != plist->udp_port)) {
		continue;
	    }
	    if (priority_debug) {
		buginf("\nPQ: %s dlci %d : %s (udp %d) -> %s",
		       output_vc->idb->namestring,
		       output_vc->this_dlci,
		       print_linktype(pak->linktype), plist->udp_port,
		       fr_vcq_queue2name(output_vc, plist->priority));
	    }
	    return(plist->priority);
	}
	if (plist->ip_fragments) {
	    ippak = (iphdrtype *)ipheadstart(pak);
	    if (!ippak->fo)
		continue;
	    if (priority_debug) {
		buginf("\nPQ: %s dlci %d: %s fragment -> %s",
		       output_vc->idb->namestring,
		       output_vc->this_dlci,
		       print_linktype(pak->linktype),
		       fr_vcq_queue2name(output_vc, plist->priority));
	    }
	    return(plist->priority);
	}
	if (plist->testlist) {
	    if (!reg_invoke_access_check(plist->linkcode,
					 pak, (int) plist->testlist)) {
		continue;
	    }
	    if (priority_debug) {
		if (plist->linkcode == LINK_BRIDGE) {
		    typecode = reg_invoke_extract_typecode(pak);
		    buginf("\nPQ: %s dlci %d : %s (0x%-04x) -> %s",
			   output_vc->idb->namestring,
			   output_vc->this_dlci,
			   print_linktype(pak->linktype), typecode,
			   fr_vcq_queue2name(output_vc, plist->priority));
		} else {
		    reg_invoke_sprintf_src_dst(
				  plist->linkcode, source, pak, SOURCE);
		    reg_invoke_sprintf_src_dst(
				  plist->linkcode, destination, pak,
				  DESTINATION);
		    buginf("\nPQ: %s dlci %d: %s (s=%s, d=%s) -> %s",
			   output_vc->idb->namestring,
			   output_vc->this_dlci,
			   print_linktype(pak->linktype), source, destination,
			   fr_vcq_queue2name(output_vc, plist->priority));
		}
	    }
	    return(plist->priority);
	}
	if (priority_debug) {
	    buginf("\nPQ: %s dlci %d : %s -> %s",
		   output_vc->idb->namestring,
		   output_vc->this_dlci,
		   print_linktype(pak->linktype),
		   fr_vcq_queue2name(output_vc, plist->priority));
	}
	return(plist->priority);
    }
    if (priority_debug) {
	buginf("\nPQ: %s dlci %d : %s (defaulting) -> %s",
	       output_vc->idb->namestring,
	       output_vc->this_dlci,
	       print_linktype(pak->linktype),
	       fr_vcq_queue2name(output_vc,
				 VCQ_PRIORITY_ARRAY(output_vc)->pdefault));
    }
    return(VCQ_PRIORITY_ARRAY(output_vc)->pdefault);
}


/*
 * fr_vcq_priority_enqueue
 *	
 * Enqueue a packet for output vc according to priority.
 * We are assuming an output interface is set in pak->if_output
 */
static boolean fr_vcq_priority_enqueue (
    fr_pvc_info_type *vc,
    paktype *pak,
    enum HEADTAIL which)
{
    int value;

    if (pak->flags & PAK_PRIORITIZED) {
	/*
	 * Packet was classified prior to arriving here, so just take the queue
	 * number out of the packet.
	 */
	value = pak->oqnumber;
	if (priority_debug) {
	    buginf("\nPQ: PAK_PRIORITIZED %s/dlci %d : %s -> %s",
		   pak->if_output->hwptr->hw_namestring,
		   vc->primary_dlci,
		   print_linktype(pak->linktype),
		   fr_vcq_queue2name(vc, pak->oqnumber));
	}
    } else if (pak->flags & PAK_PRIORITY) {
	value = PRIORITY_HIGH;
	which = HEAD;
    } else {
	value = fr_vcq_priority_classify(vc, pak);
    }

    /*
     * Hang datagram on appropriate output queue
     */
    if (which == TAIL) {
	if (pak_enqueue(&(vc->fr_vcq->outputq[value]), pak)) {
	    pak->flags |= PAK_DLQ;
	    vc->fr_vcq->output_qcount++;
	    return(TRUE);
	}
    } else {
	if (pak_requeue(&(vc->fr_vcq->outputq[value]), pak)) {
	    pak->flags |= PAK_DLQ;
	    vc->fr_vcq->output_qcount++;
	    return(TRUE);
	}
    }
    if (priority_debug) {
	buginf(" -- congestion drop");
    }

    return FALSE;
}



/*
 * fr_vcq_priority_dequeue
 *
 * Dequeue the next packet from one of the hold queues
 */
static paktype *fr_vcq_priority_dequeue (fr_pvc_info_type *fr_vc)
{
    paktype *pak = NULL;
    struct VCQ *vcq;
    hwidbtype *idb;
    int i;
    ushort datagramsize_old;

    /*
     * Handle priority queueing.
     * Get next packet in highest queue.
     */
    idb = fr_vc->idb->hwptr;
    vcq = fr_vc->fr_vcq;
    for (i = 0; i < PRIORITY_MAXTYPE; i++) {
	if (vcq->outputq[i].qhead) {
	    pak = pak_dequeue(&(vcq->outputq[i]));
	    if (pak) {
		pak->flags &= ~PAK_DLQ;
		vcq->output_qcount--;
		vcq->output_dequeued[i]++;
		pak->oqnumber = i;
		break;
	    }
	}
    }

    if (!pak)
	return(NULL);

    /*
     * Do any compression on output
     * Store pre-compression size for potential credit
     * The test below should be more generic.
     */
    datagramsize_old = pak->datagramsize;
    if (idb->tcp_compress ||
 		((pak->nif_output != NULL) &&
 		 (pak->nif_output->compression_negotiated_enabled))) {
	pak = reg_invoke_compress_header(pak->linktype, pak);
    }
    if (idb->comp_db) {
	pak = reg_invoke_encaps_compress(idb->enctype, pak);
    }
    /*
     * Credit traffic shaping statistics for overcharging which
     * results from last-minute knowledge of compression savings.
     * Put check beyond all compression routines to insure all
     * bytes savings are accounted for...
     */
    traffic_shape_credit(pak, datagramsize_old);

    return(pak);
}


static paktype *fr_vcq_custom_dequeue(fr_pvc_info_type *vc)
{
    hwidbtype *idb;
    pritype *p;
    paktype *pak = NULL;
    struct VCQ *vcq;
    int i;
    ushort datagramsize_old;

    idb = vc->idb->hwptr;
    vcq = vc->fr_vcq;
    p = VCQ_PRIORITY_ARRAY(vc);
    for (i = 0; i < p->lowest_custom; i++) {
	if (vcq->outputq[i].qhead) {
	    pak = pak_dequeue(&(vcq->outputq[i]));
	    if (pak) {
		pak->flags &= ~PAK_DLQ;
		vcq->output_qcount--;
		vcq->output_dequeued[i]++;
		pak->oqnumber = i;
		/*
		 * Do any compression on output
 		 * Store pre-compression size for potential credit
  		 * The test below should be more generic.
		 */
 		datagramsize_old = pak->datagramsize;
 		if (idb->tcp_compress ||
		     ((pak->nif_output != NULL) &&
 		      (pak->nif_output->compression_negotiated_enabled))) {
		    pak = reg_invoke_compress_header(pak->linktype, pak);
		}
		if (idb->comp_db) {
		    pak = reg_invoke_encaps_compress(idb->enctype, pak);
		}
  		/*
  		 * Credit traffic shaping statistics for overcharging which
  		 * results from last-minute knowledge of compression savings.
  		 * Put check beyond all compression routines to insure all
  		 * bytes savings are accounted for...
  		 */
  		traffic_shape_credit(pak, datagramsize_old);

		return(pak);
	    }
	}
    }

    /*
     * Now run the custom queues looking to see what should be transmitted.
     */
    i = vcq->custom_queuenum;
    do {
	if (vcq->outputq[i].qhead) {
	    pak = pak_dequeue(&(vcq->outputq[i]));
	    if (pak) {
		pak->flags &= ~PAK_DLQ;
		vcq->output_qcount--;
		vcq->output_dequeued[i]++;
		pak->oqnumber = i;
	    }
	    break;
	}
	i = VCQ_PRIORITY_QUEUE_INC(i);
    } while (i != vcq->custom_queuenum);

    if (!pak) {
	vcq->custqremain[i] = fr_vcq_custom_qbytecnt(vc,i);
	return(NULL);
    }

    /*
     * Decrement byte count for current queue.  If we've transmitted too
     * much from the current queue, reset byte count and move queue
     * pointer to next queue.
     */
    if (vcq->custqremain[i] > pak->datagramsize) {
	vcq->custqremain[i] -= pak->datagramsize;
	if (custom_debug) {
	    buginf("\nintf %s: dlci %d PQ: (Pk size: %d) Q # was %d now %d",
		   idb->hw_namestring, vc->this_dlci,
		   pak->datagramsize, vcq->custom_queuenum, i);
	}
    } else {
	vcq->custqremain[i] = fr_vcq_custom_qbytecnt(vc, i);
	i = VCQ_PRIORITY_QUEUE_INC(i);
	if (custom_debug) {
	    buginf("\nintf %s: dlci %d PQ: (Pk size: %d) Inc. Q # to %d",
		   idb->hw_namestring, vc->this_dlci,
		   pak->datagramsize, i);
	}
    }
    /*
     * Update pointer to queue array, reset byte count for former current
     * queue.
     */
    if (vcq->custom_queuenum != i) {
	vcq->custqremain[vcq->custom_queuenum] =
	    fr_vcq_custom_qbytecnt(vc, vcq->custom_queuenum);
	vcq->custom_queuenum = i;
    }

    /*
     * Do any compression on output
     * Store pre-compression size for potential credit
     * The test below should be more generic.
     */
    datagramsize_old = pak->datagramsize;
    if (idb->tcp_compress ||
 	 ((pak->nif_output != NULL) &&
 	  (pak->nif_output->compression_negotiated_enabled))) {
	pak = reg_invoke_compress_header(pak->linktype, pak);
    }
    /*
     * Credit traffic shaping statistics for overcharging which
     * results from last-minute knowledge of compression savings.
     * Put check beyond all compression routines to insure all
     * bytes savings are accounted for...
     */
    traffic_shape_credit(pak, datagramsize_old);

    return(pak);
}


/*
 * fr_vcq_fcfs_enqueue
 * Enqueue a packet for output according to fcfs order
 */

boolean fr_vcq_fcfs_enqueue(fr_pvc_info_type *vc,
			    paktype *pak,
			    enum HEADTAIL which)
{
    struct VCQ *vcq;
    int value;

    if (pak->flags & PAK_PRIORITY)
	which = HEAD;
    value = PRIORITY_NORMAL;

    /*
     * Hang datagram on appropriate output queue
     */
    vcq = vc->fr_vcq;
    if (which == TAIL) {
	if (pak_enqueue(&(vcq->outputq[value]), pak)) {
	    pak->flags |= PAK_DLQ;
	    vcq->output_qcount++;
	    return(TRUE);
	}
    } else {
	if (pak_requeue(&(vcq->outputq[value]), pak)) {
	    pak->flags |= PAK_DLQ;
	    vcq->output_qcount++;
	    return(TRUE);
	}
    }
    return FALSE;
}



/*
 * fr_vcq_fcfs_dequeue
 * Dequeue the next packet from a vcq
 */

paktype *fr_vcq_fcfs_dequeue (fr_pvc_info_type *vc)
{
    struct VCQ *vcq;
    hwidbtype *idb;
    paktype *pak;
    ushort datagramsize_old;
	
    /*
     * Handle case where no prioritization is being done.
     */
    vcq = vc->fr_vcq;
    idb = vc->idb->hwptr;
    pak = pak_dequeue(&(vcq->outputq[PRIORITY_NORMAL]));
    if (pak) {
	vcq->output_qcount--;
	pak->flags &= ~PAK_DLQ;

	/*
	 * Do any compression on output
 	 * Store pre-compression size for potential credit
	 * The test of "idb->tcp_compress" should be more generic.
	 */
 	datagramsize_old = pak->datagramsize;
	if ((idb->tcp_compress || ((pak->nif_output != NULL) &&
			(pak->nif_output->compression_negotiated_enabled))))
	    pak = reg_invoke_compress_header(pak->linktype, pak);
	if (idb->comp_db)
	    pak = reg_invoke_encaps_compress(idb->enctype, pak);
  	/*
  	 * Credit traffic shaping statistics for overcharging which
  	 * results from last-minute knowledge of compression savings.
  	 * Put check beyond all compression routines to insure all
  	 * bytes savings are accounted for...
  	 */
  	traffic_shape_credit(pak, datagramsize_old);

	pak->oqnumber = PRIORITY_NORMAL;
    }
    return(pak);
}

/*
 * fr_enqueue
 */
void fr_enqueue (traffic_shape_t *shape, paktype *pak)
{
    fr_idb_struct_type *fr_idb = shape->shape_idb->hwptr->frame_relay_stuff;
    fr_pvc_info_type *vc;
    hwidbtype *output = pak->if_output->hwptr;
    frame_relay_hdr *hdr;
    boolean enq_ok;
    ushort dlci;
    enum HEADTAIL which = TAIL;
    fr_idb = output->frame_relay_stuff;
    hdr = (frame_relay_hdr *)(pak->datagramstart);
    dlci = DLCI_TO_NUM(GETSHORT(&hdr->dlci));
    
    vc = fr_idb->fr_dlci_tab[dlci];
    switch(vc->fr_vcq->queue_algorithm) {
    case FR_VCQ_PRIORITY:
    case FR_VCQ_CUSTOM:
	enq_ok = fr_vcq_priority_enqueue(vc, pak, which);
	break;
    case FR_VCQ_FCFS:
    default:
	enq_ok = fr_vcq_fcfs_enqueue(vc, pak, which);
    }

    if (enq_ok == FALSE) {
	vc->pvc_stats.num_dropped_pkts++;
	datagram_done(pak);
	return;
    } else {
	shape->shape_qcount++;
	return;
    }
}

/*
 * fr_vc_dequeue
 * dequeue from the specified VC
 */
paktype *fr_vc_dequeue (traffic_shape_t *shape)
{
    fr_pvc_info_type *fr_vc;
    paktype *pak = NULL;
    fr_idb_struct_type *fr_idb;
    hwidbtype *hwidb = shape->shape_idb->hwptr;
    
    /*
     * Handle custom queuing
     * Special case high priority queue for keepalives and such
     * Treat this and all queues below "lowest_custom" as Priority Qs
     */

    fr_idb = hwidb->frame_relay_stuff;
    if (!fr_idb) {
	return NULL;
    }
    fr_vc = fr_idb->fr_dlci_tab[shape->shape_dlci_num];
    if (!fr_vc)
	return(NULL);

    switch(fr_vc->fr_vcq->queue_algorithm) {
    case FR_VCQ_CUSTOM:
	pak = fr_vcq_custom_dequeue(fr_vc);
	break;
    case FR_VCQ_PRIORITY:
	pak = fr_vcq_priority_dequeue(fr_vc);
	break;
    case FR_VCQ_FCFS:
    default:
	pak = fr_vcq_fcfs_dequeue(fr_vc);
	break;
    }
    if (pak)
	shape->shape_qcount--;
    
    return pak;
}

void fr_vcq_default_queueing(fr_pvc_info_type *vc)
{
    fr_idb_struct_type *fr_idb;
    hwidbtype *hwidb;

    hwidb = vc->idb->hwptr;
    GET_FR_IDB(hwidb, fr_idb);
    if (!idb_vcq->fr_vcq_enable) {
	return;
    }
    
    if (vc->fr_vcq) {
	vc->fr_vcq->queue_algorithm = FR_VCQ_FCFS;
    }
    fr_vcq_queue_setup(vc, vc->fr_vcq);
}


/*
 * fr_vcq_queue_setup
 *     Setup VC queueing for a VC
 */
void fr_vcq_queue_setup (fr_pvc_info_type *vc, struct VCQ *vcq)
{
    fr_idb_struct_type *fr_idb;
    struct VCQ *setvcq;
    hwidbtype *hwidb;
    int status, i;
    pritype *p;

    hwidb = vc->idb->hwptr;

    GET_FR_IDB(hwidb, fr_idb);
    if (!idb_vcq->fr_vcq_enable) {
	return;
    }
    
    status = raise_interrupt_level(NETS_DISABLE);
    fr_vcq_write(vc, vcq);
    setvcq = vc->fr_vcq;

    switch (setvcq->queue_algorithm) {
    case FR_VCQ_CUSTOM:
	setvcq->oqueue = fr_vcq_priority_enqueue;
	setvcq->oqueue_dequeue = fr_vcq_custom_dequeue;
	setvcq->custom_queuenum = 1;
	for (i = 0; i < PRIORITY_MAXLIST; i++) {
	    setvcq->custqremain[i] = fr_vcq_custom_qbytecnt(vc, i);
	}
	p = cust_parray[setvcq->priority_list];
	if (p == NULL) {
	    p = priority_allocate_list(TRUE, setvcq->priority_list);
	}
	break;

    case FR_VCQ_PRIORITY:
	setvcq->oqueue = fr_vcq_priority_enqueue;
	setvcq->oqueue_dequeue = fr_vcq_priority_dequeue;
	p = parray[setvcq->priority_list];
	if (p == NULL) {
	    p = priority_allocate_list(FALSE, setvcq->priority_list);
	}
	break;

    default:
	setvcq->queue_algorithm = FR_VCQ_FCFS;
	setvcq->oqueue = fr_vcq_fcfs_enqueue;
	setvcq->oqueue_dequeue = fr_vcq_fcfs_dequeue;
	setvcq->priority_list = 0;
    }
    
    fr_vcq_reset(vc);
    reset_interrupt_level(status);
}

static void fr_vcq_queue_reset (int list_no)
{
    hwidbtype *hwidb;
    fr_idb_struct_type *fr_idb;
    fr_pvc_info_type *pvc = NULL;
    int i;
    list_element *element;
    
    FOR_ALL_HWIDBS_IN_LIST(HWIDBLIST_FR, element, hwidb) {
	if ((fr_idb = hwidb->frame_relay_stuff)) {
	    for (i = MIN_DLCI; i < MAX_DLCI; i++) {
		if ((pvc = fr_idb->fr_dlci_tab[i])) {
		    if (pvc->fr_vcq &&
			pvc->fr_vcq->priority_list == list_no) {
			fr_vcq_reset(pvc);
		    }
		}
	    }
	}
    }
}

static void fr_vcq_clear_counters (hwidbtype *hwidb)
{
    fr_idb_struct_type *fr_idb;
    fr_pvc_info_type *pvc = NULL;
    int i, j;
    
    if (hwidb->enctype == ET_FRAME_RELAY) {
	GET_FR_IDB(hwidb, fr_idb);
	if (!(fr_idb->fr_vcq &&
	      fr_idb->fr_vcq->fr_vcq_enable))
	    return;
	for (i = MIN_DLCI; i < MAX_DLCI; i++) {
	    if ((pvc = fr_idb->fr_dlci_tab[i])) {
		if (pvc->fr_vcq) {
		    for (j = 0; j < PRIORITY_MAXTYPE; j++) {
			pvc->fr_vcq->output_drops[j] = 0; 
		    }
		}
	    }
	}
    }
}
		  
static void fr_vcq_holdq_display (fr_pvc_info_type *pvc)
{

    struct VCQ *vcq;
    int i;

    vcq = pvc->fr_vcq;
    
    if (!vcq->priority_list) {
	printf("\n  Output queue %d/%d, %u drop",
	       vcq->outputq[VCQ_DEFAULT_QUEUE(pvc)].count,
	       vcq->outputq[VCQ_DEFAULT_QUEUE(pvc)].maximum,
	       vcq->output_drops[VCQ_DEFAULT_QUEUE(pvc)]);
	return;
    }
    if (vcq->custom_queuenum) {
	printf("\n  Output queues: (queue #: size/max/drops)\n     ");
	for (i = 0; i < PRIORITY_MAXTYPE; i++) {
	    printf("%d: %d/%d/%u ", i, vcq->outputq[i].count,
		   vcq->outputq[i].maximum,
		   (vcq->output_drops[i]));
	    if (i%5 == 4)
		printf("\n     ");
	}
    } else {
	printf("\n  Output queue: high %d/%d/%u, medium %d/%d/%u, normal %d/%d/%u, low %d/%d/%u",
	       vcq->outputq[PRIORITY_HIGH].count,
	       vcq->outputq[PRIORITY_HIGH].maximum,
	       vcq->output_drops[PRIORITY_HIGH],

	       vcq->outputq[PRIORITY_MEDIUM].count,
	       vcq->outputq[PRIORITY_MEDIUM].maximum,
	       vcq->output_drops[PRIORITY_MEDIUM],
	       
	       vcq->outputq[PRIORITY_NORMAL].count,
	       vcq->outputq[PRIORITY_NORMAL].maximum,
	       vcq->output_drops[PRIORITY_NORMAL],
		
	       vcq->outputq[PRIORITY_LOW].count,
	       vcq->outputq[PRIORITY_LOW].maximum,
	       vcq->output_drops[PRIORITY_LOW]);

    }
}

static void fr_vcq_show_default (fr_pvc_info_type *pvc)
{
    printf("\n  %s dlci %d is first come first serve default queueing \n",
	   pvc->idb->namestring,
	   pvc->this_dlci);
}

static void fr_vcq_show_queueing (hwidbtype *hwidb, ushort dlci)
{
    fr_idb_struct_type *fr_idb;
    fr_pvc_info_type *pvc;
    struct VCQ *vcq;

    GET_FR_IDB(hwidb, fr_idb);
    if ((pvc = fr_idb->fr_dlci_tab[dlci])) {
	if ((vcq = pvc->fr_vcq)) {
	    switch (vcq->queue_algorithm) {
	    case FR_VCQ_CUSTOM:
		show_queueing(QUEUE_CUSTOM, vcq->priority_list);
		break;
		
	    case FR_VCQ_PRIORITY:
		show_queueing(QUEUE_PRIORITY, vcq->priority_list);
		break;
			
	    case FR_VCQ_FCFS:
		fr_vcq_show_default(pvc);
		break;
		
	    default:
		
	    }
	    fr_vcq_holdq_display(pvc);
	}
    }
}


/*
 * fr_vcq_init
 */
void fr_vcq_init (subsystype *subsys)
{
    /*
     * Set up traffic shaping
     */
    fr_traffic_init();
    
  /* parray and cust_parray are initialized by priority.c
   */
    reg_add_fr_mapclass_nvgen(fr_vcq_mapclass_nvgen, "fr_vcq_mapclass_nvgen");
    reg_add_fr_idb_init(fr_vcq_idb_init, "fr_vcq_idb_init");
    reg_add_fr_idb_uninit(fr_vcq_idb_uninit, "fr_vcq_idb_uninit");
    reg_add_fr_show_pvc(fr_vcq_show_queueing, "fr_vcq_show_queueing");
    reg_add_fr_dlci_class(fr_vcq_frclass, "fr_vcq_dlci_class");
    reg_add_clear_counters(fr_vcq_clear_counters, "fr_vcq_clear_counters");
    reg_add_queue_reset(fr_vcq_queue_reset, "fr_vcq_queue_reset");
    reg_add_fr_mapclass_remove(fr_vcq_mapclass_remove,
			      " fr_vcq_mapclass_remove");
    reg_add_encaps_queuing_allowed(ET_FRAME_RELAY, fr_vcq_intq_chk,
			    "fr_vcq_intq_chk");
    fr_vcq_parser_init();
    fr_class_init();
}

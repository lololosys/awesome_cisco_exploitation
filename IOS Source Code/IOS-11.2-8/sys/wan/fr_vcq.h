/* $Id: fr_vcq.h,v 3.1.2.3 1996/05/29 06:35:01 fowler Exp $
 * $Source: /release/112/cvs/Xsys/wan/fr_vcq.h,v $
 *------------------------------------------------------------------
 * Frame Relay VC Queueing
 *
 * March 1996, Charles Poe
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: fr_vcq.h,v $
 * Revision 3.1.2.3  1996/05/29  06:35:01  fowler
 * CSCdi58766:  Link errors in 11.2 build
 * Branch: California_branch
 *
 * Revision 3.1.2.2  1996/05/24  21:21:38  ccpoe
 * CSCdi58460:  map-class frame-relay traffic-shaping features dont work
 * on FR SVC
 * Branch: California_branch
 *
 * Revision 3.1.2.1  1996/05/15  05:54:49  fowler
 * Frame relay VC queuing and rate enforcement
 * Branch: California_branch
 *
 * Revision 3.1  1996/04/26  05:47:29  ccpoe
 * placeholder
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


enum fr_vcq_type {
    FR_VCQ_CUSTOM,
    FR_VCQ_PRIORITY,
    FR_VCQ_FCFS
};

#define FR_VCQ_HOLDQ 40

#define VCQ_DEFAULT_QUEUE(vc) (vc->fr_vcq->custom_queuenum ? 1 : PRIORITY_NORMAL)
#define VCQ_PRIORITY_QUEUE_INC(i)           ((i & (PRIORITY_MAXTYPE - 2)) + 1)

#define FR_PVC_INC(i) (((i + 1) > MAX_DLCI) ? MIN_DLCI : (i + 1))
#define FR_PVC_DEC(i) (((i - 1) < MIN_DLCI) ? MAX_DLCI : (i - 1))


#define FOR_ALL_IDB_VC(hwidb, vc, i) \
       for (i = MIN_DLCI; i <= MAX_DLCI  \
	    && (vc = hwidb->frame_relay_stuff->fr_dlci_tab[i]) ; i++)


#define FOR_ALL_VCQ_VC(hwidb, i) \
    { for (hwidb = (hwidbtype *) hwidbQ->qhead; \
	 (hwidb && hwidb->frame_relay_stuff \
	  && hwidb->frame_relay_stuff->fr_vcq_enable); \
	 hwidb = hwidb->next) \
    for (i = MIN_DLCI; i <= MAX_DLCI; i++)) }


typedef boolean (*fr_oqueue_t)(fr_pvc_info_type *, paktype *, enum HEADTAIL);
typedef paktype * (*fr_oqueue_dequeue_t)(fr_pvc_info_type *);
    
struct VCQ {
    fr_pvc_info_type *vcq_vc;
    ushort flag;
    ushort queue_algorithm;         /* queuing used on the interface */
    ushort priority_list;       /* list # if priority output queuing */
    ushort custom_queuenum;     /* Current queue # if custom queuing */
    ushort output_qcount;       /* size of all priority output queues */
    ulong custqremain[PRIORITY_MAXTYPE]; /* bytes to send w/ customq */
    int output_defqcount[PRIORITY_MAXTYPE]; /* maximum queue size */
    queuetype outputq[PRIORITY_MAXTYPE];    /* output hold queues */
    fr_oqueue_t oqueue;            /* enqueue packet for output */
    fr_oqueue_dequeue_t oqueue_dequeue; /* dequeue packet for output */
    ulong output_drops[PRIORITY_MAXTYPE]; /* # dropped on output, this q*/
    ulong output_dequeued[PRIORITY_MAXTYPE]; /* # dequeued from this q */
};

struct fr_idb_vcq {
    ushort fr_vcq_enable;
    ushort fr_vcq_index;
    mapclass_type *classp;
};

#define idb_vcq fr_idb->fr_vcq

extern pritype *cust_parray[];
extern pritype *parray[];
extern void fr_traffic_init(void);
extern void fr_vcq_mapparams (mapclass_type *, struct VCQ *);

static inline pritype *
VCQ_PRIORITY_ARRAY (fr_pvc_info_type *vc)
{
    if (!vc->fr_vcq) {
	return NULL;
    }
    if (vc->fr_vcq->custom_queuenum)
	return cust_parray[vc->fr_vcq->priority_list];
    return parray[vc->fr_vcq->priority_list];
}



static inline void vcq_delete (struct VCQ *vcq) {
    free(vcq);
}

/*
 * pvc_find_vcq
 *    index through pvc array of a hwidb to locate the VCQ
 */
static inline struct VCQ *pvc_find_vcq (hwidbtype *hwidb, ushort dlci)
{
   if (hwidb->frame_relay_stuff &&
       hwidb->frame_relay_stuff->fr_dlci_tab[dlci]) {
       return (hwidb->frame_relay_stuff->fr_dlci_tab[dlci]->fr_vcq);
   }
   return NULL;
}

/*
 * mapclass_find_vcq
 *    Find the vcq from a give mapclass
 */
static inline struct VCQ *mapclass_find_vcq (mapclass_type *classp)
{
    struct fr_vcq_params *vcq_p;
    if (classp && classp->params) {
	vcq_p = ((fr_params_type *)classp->params)->vcq_params;
	if (vcq_p) {
	    return (vcq_p->map_vcq);
	}
    }
    return NULL;
}

/*
 * GET_VCQ_PARAMS
 */

static inline struct fr_vcq_params *get_vcq_params (mapclass_type *classp)
{
    if (classp && classp->params) {
	return (((fr_params_type *)classp->params)->vcq_params);
    }
    return NULL;
}


static inline struct fr_vcq_params *vc_vcq_params (fr_pvc_info_type *vc)
{
    mapclass_type *classp;
    
    if (vc->fr_vcq && vc->pvc_class) {
	classp = vc->pvc_class;
	if (classp && classp->params) {
	    return (((fr_params_type *)classp->params)->vcq_params);
	}
    }
    return NULL;
}

/*
 * return TRUE if vc queueing is configured. Note: this doesn't mean
 *             vc queueing is executed, or running on this vc
 */
static inline boolean is_vcq_configured (fr_pvc_info_type *pvc)
{
    struct fr_vcq_params *vcq_params;

    if (pvc->fr_vcq) {
	return TRUE;
    }
    vcq_params = get_vcq_params(pvc->pvc_class);
    if (vcq_params &&
	vcq_params->map_vcq->priority_list) {
	return TRUE;
    }
    return FALSE;
}
    
extern void fr_vcq_init (subsystype *);
extern void fr_vcq_queue_setup (fr_pvc_info_type *, struct VCQ *);
extern void fr_vcq_default_queueing(fr_pvc_info_type *);
extern void fr_vcq_parser_init(void);
extern void fr_vcq_mapclass (parseinfo *);
extern void fr_vcq_mapclass_nvgen (mapclass_type *);
extern void fr_vcq_frclass (mapclass_type *, fr_pvc_info_type *);
extern boolean fr_vcq_fcfs_enqueue(fr_pvc_info_type *, paktype *,
				   enum HEADTAIL);
extern paktype *fr_vcq_fcfs_dequeue(fr_pvc_info_type *);
extern void fr_vcq_queue_add(fr_pvc_info_type *, boolean);
extern void fr_vcq_intf_queueing(hwidbtype *);
void fr_enqueue(traffic_shape_t *, paktype *);
paktype *fr_vc_dequeue (traffic_shape_t *shape);

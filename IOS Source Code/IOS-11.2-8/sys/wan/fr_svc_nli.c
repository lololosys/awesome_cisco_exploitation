/* $Id: fr_svc_nli.c,v 3.1.64.8 1996/07/29 22:11:36 snyder Exp $
 * $Source: /release/112/cvs/Xsys/wan/fr_svc_nli.c,v $
 *------------------------------------------------------------------
 * fr_svc_nli.c : Intermediate layer for decoupling of Call Control 
 *
 * May 1995, Shankar Natarajan
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: fr_svc_nli.c,v $
 * Revision 3.1.64.8  1996/07/29  22:11:36  snyder
 * CSCdi64472:  declare some arrays const in shr_frame, shr_frame_svc
 *              2380 out of data, 40 image
 * Branch: California_branch
 *
 * Revision 3.1.64.7  1996/07/21  16:58:23  shankar
 * CSCdi63253:  FR SVC called party router crash if pack received when
 * not ready. Add more configuration checks to make sure things are
 * functional on the receiving side.
 * Branch: California_branch
 *
 * Revision 3.1.64.6  1996/06/25  00:42:42  shankar
 * CSCdi61146:  SVC mapclass parameters change in back-back testing
 * Branch: California_branch
 *
 * Revision 3.1.64.5  1996/06/22  19:27:33  shankar
 * CSCdi61102:  Parameter Negotiation for SVCs needs a little more checking
 * Branch: California_branch
 *
 * Revision 3.1.64.4  1996/05/30  23:27:48  ccpoe
 * CSCdi58460:  map-class frame-relay traffic-shaping features dont work
 * on FR SVC
 * Branch: California_branch
 *
 * Revision 3.1.64.3  1996/05/24  21:21:35  ccpoe
 * CSCdi58460:  map-class frame-relay traffic-shaping features dont work
 * on FR SVC
 * Branch: California_branch
 *
 * Revision 3.1.64.2  1996/05/08  02:31:01  skoh
 * CSCdi57007:  Erroneous implementation of fr_svc_rem_nli_from_idb_queue
 * Branch: California_branch
 *
 * Revision 3.1.64.1  1996/04/25  23:23:47  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.1.68.1  1996/04/24  04:11:32  shankar
 * Branch: ATM_Cal_branch
 * Patch FR_SVC_branch changes
 *
 * Revision 3.1  1995/11/09  13:49:12  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/09/01  19:53:35  shankar
 * Files for Arkansas feature
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "passive_timers.h"
#include "sys_registry.h"
#include "media_registry.h"
#include "interface_private.h"
#include "packet.h"
#include "config.h"
#include "../ui/debug.h"
#include "parser.h"
#include "frame_relay.h"
#include "fr_debug.h"
#include "../if/network.h"
#include "../parser/parser_defs_exec.h"
#include "../wan/parser_defs_frame.h"
#include "../h/types.h"
#include "../wan/fr_svc.h"
#include "../if/static_map.h"
#include "../wan/fr_registry.h"

/*
 * Messages defined for layer 3 interface into the router code.
 * Defined and used in a manner similar to the Q921/Q931
 * primitives.	All router related functions are handled through 
 * the NLI. Frame Relay map creation/teardown, handling of 
 * outgoing messages, queueing of outgoing packets etc. etc. 
 *
 * Help strings defined for various NLI messages. Used in 
 * debug statements.
 */
static generic_def nli_event_help[] =  {
	{NL_CALL_REQ,   	"NL_CALL_REQ"},
	{NL_CALL_IND,   	"NL_CALL_IND"},
	{NL_CALL_CNF,   	"NL_CALL_CNF"},
	{NL_CALL_REJ,   	"NL_CALL_REJ"},
	{NL_REL_REQ,    	"NL_REL_REQ"},
	{NL_REL_IND,    	"NL_REL_IND"},
	{NL_REL_CNF,    	"NL_REL_CNF"},
	{NL_REL_REJ,    	"NL_REL_REJ"},
	{L3_CALL_REL_REQ,	"L3_CALL_REL_REQ"},
	{L3_CALL_REQ,   	"L3_CALL_REQ"},
	{NL_CALL_ACK,   	"NL_CALL_ACK"},
	{NL_CALL_NACK,  	"NL_CALL_NACK"},
	{NL_MSG_UNKNOWN, 	"NL_MSG_UNKNOWN"}
};

/*
 * Help strings defined for various NLI states. Used in 
 * debug statements.
 */
static generic_def nli_state_help[] = {
	{STATE_NL_NULL,		"STATE_NL_NULL"},
	{STATE_NL_CALL_REQ,     "STATE_NL_CALL_REQ"},
	{STATE_NL_CALL_IND,     "STATE_NL_CALL_IND"},
	{STATE_NL_CALL_CNF,     "STATE_NL_CALL_CNF"},
	{STATE_NL_CALL_REJ,     "STATE_NL_CALL_REJ"},
	{STATE_NL_REL_REQ,      "STATE_NL_REL_REQ"},
	{STATE_NL_REL_IND,      "STATE_NL_REL_IND"},
	{STATE_NL_REL_CNF,      "STATE_NL_REL_CNF"},
	{STATE_NL_REL_REJ,      "STATE_NL_REL_REJ"},
	{STATE_L3_CALL_REL_REQ, "STATE_L3_CALL_REL_REQ"},
	{STATE_L3_CALL_REQ,     "STATE_L3_CALL_REQ"},
	{STATE_NL_CALL_ACK,     "STATE_NL_CALL_ACK"},
	{STATE_NL_CALL_NACK,    "STATE_NL_CALL_NACK"}
};
/* 
 * fr_svc_nli_lookup
 * Given a call_id, search through the nli linked list 
 * to find the appropriate nli data structure.
 * 
 * The maintaining of an nli queue within the fr_idb is 
 * important from the standpoint of the following:
 *	- Call teardown when FR SVCs are turned off. In this
 * 	  case, we can just walk the list and tear down
 *	  all calls in the list. 
 *	- When the interface goes down. 
 * 	- Displaying all the VCs upon request. 
 */
fr_svc_nli_type *fr_svc_nli_lookup (
	fr_idb_struct_type *fr_idb,
	uint call_id)
{
    fr_svc_nli_type 	*nli;
    uint		iq;
    leveltype		level;

    /* 
     * Walk the queue.
     */
    level = raise_interrupt_level(NETS_DISABLE);
    for (iq = 0, nli = fr_idb->nli_flink; 
		(iq < MAX_CALL_ID_PER_IDB) && nli; iq++) {
	if (nli->call_id == call_id) {
	    /* 
	     * Found a NLI structure that has the same call ID 
	     * as the one that we are looking for. 
	     */
	    reset_interrupt_level(level);
	    return(nli);
	} else {
	    /* 
	     * another lap.
	     */
	    nli = nli->flink;
	    continue;
	}
    }
    reset_interrupt_level(level);
    /* 
     * A NLI structure with the specified call ID does not exist.
     */
    return(NULL);
}
/* 
 * fr_svc_select_next_call_id
 * A brain-dead scheme to select the next free call id. 
 * Walk the nli queue, peek into each block. If a particular
 * number is free, grab it. MAX_CALL_ID_PER_IDB is currently 
 * defined to be 64. This choice is just an act of randomness!
 */
uint fr_svc_select_next_call_id (hwidbtype *hwidb)
{
    fr_svc_nli_type 		*nli;
    uint 		ip;
    fr_idb_struct_type	*fr_idb;
    leveltype		level;

    fr_idb = hwidb->frame_relay_stuff;
    /* 
     * Call Id must be greater than 1. Hence, the index
     * variable within the "for" loop starts with 1. 
     */
    nli = fr_idb->nli_flink;
    level = raise_interrupt_level(NETS_DISABLE);
    for (ip = 1; (ip < MAX_CALL_ID_PER_IDB) && nli ; ip++) {
	for (; nli; nli = nli->flink) {
	    if (nli->call_id == ip) {
		/* 
		 * NLI with a matching call ID was found.
		 */
		nli = nli->flink;
		break;
	    }
	}
    }
    reset_interrupt_level(level);
    /* 
     * If we are here, it means that we found a call Id that 
     * is not being used. Return i even if there is nothing in the 
     * nli queue. The least value that i can have is 1, a valid ID. 
     */
    return(ip);
}
/* 
 * fr_svc_initialize_nli
 * Initialize the various fields within the NLI structure. 
 * In particular, a call ID gets assigned. 
 */
void fr_svc_initialize_nli (
	fr_idb_struct_type *fr_idb,
	paktype *pak,
	fr_svc_nli_type *nli,
	staticmap_type *map)
{
    int i;

    nli->dlci = 0;
    nli->call_id = fr_svc_select_next_call_id(fr_idb->hwidb);
    nli->nl_state = STATE_NL_NULL;
    nli->map = map;
    nli->flink = NULL;
    nli->blink = NULL;
    nli->map_params = NULL; 	/* Parameters from user config */
    nli->in_params = NULL;	/* Parameters from call negotiation */

    /* 
     * Pointers to frmaptype structures that in turn hold
     * SVC map pointers. 
     */
    for (i=0; i < MAX_MAPS_PER_NLI; i++)
    	nli->frmap[i] = NULL; 

    queue_init(&nli->svc_holdQ, NLI_DEF_Q_SIZE);
}
/* 
 * fr_svc_rem_nli_from_idb_queue
 * The fr_idb maintains a list of NLI structures. Every time a
 * nli structure gets removed, we must remove it from the list.
 */
void fr_svc_rem_nli_from_idb_queue (
	fr_idb_struct_type *fr_idb,
	staticmap_type *map,
	fr_svc_nli_type *nli)
{
    fr_svc_nli_type 	*tmp_nli;
    int 		ix;
    struct fr_svc_params        *params;
    leveltype 	level;

    tmp_nli = nli;

    if (map) {
    	params = ((fr_params_type *)map->class->params)->svc_params;
    	map->nli = NULL;
    } 

    if (!tmp_nli)
	return;

    /* 
     * some routine cleanup before the nli structure gets freed. 
     */
    for (ix = 0; ix < MAX_MAPS_PER_NLI; ix++) {
    	nli->frmap[ix] = NULL; 
    }
    nli->map_params = NULL;
    nli->in_params = NULL;

    /* 
     * If both head and tail pointers are empty, we have nothing in
     * the queue. 
     */
    if (!fr_idb->nli_blink && !fr_idb->nli_flink) {
    	if (NLI_DBG)
	    buginf("\nNLI: IDB NLI queue empty, nli not removed : %x", nli);
	return;
    }

    level = raise_interrupt_level(NETS_DISABLE);
    /* 
     * If nli->blink, this means that this is not the first 
     * element in the queue. If !nli->blink, then this is the 
     * first element in the queue. In the latter case, since the 
     * first element in the queue is being removed, set the 
     * head pointer to point to the next element.
     */
    if (nli->blink)
    	nli->blink->flink = nli->flink;
    else 
	fr_idb->nli_flink = fr_idb->nli_flink->flink;

    /* 
     * If nli->flink, this means that this is not the last
     * element in the queue. If !nli->flink, then this is the 
     * last element in the queue. In the latter case, since the 
     * last element in the queue is being removed, set the 
     * tail pointer to the previous element.
     */
    if (nli->flink)
    	nli->flink->blink = nli->blink;
    else
	fr_idb->nli_blink = nli->blink;

    free(tmp_nli);
    nli = NULL;
    reset_interrupt_level(level);
}
/* 
 * fr_svc_add_nli_to_idb_queue
 * The fr_idb maintains a list of NLI structures. Every time a
 * nli structure gets created, we must add it to the list. 
 */
void fr_svc_add_nli_to_idb_queue (
	fr_idb_struct_type *fr_idb,
	fr_svc_nli_type *nli)
{
    fr_svc_nli_type 	*tmp_nli;
    leveltype 	level;

    level = raise_interrupt_level(NETS_DISABLE);
    /*
     * If both head and tail are NULL, we have no elements in
     * the queue. Add this svc to the queue and set the next
     * and back pointers accordingly.
     */
    if (!fr_idb->nli_blink && !fr_idb->nli_flink) {
	fr_idb->nli_blink = nli;
	fr_idb->nli_blink->flink = NULL;
	fr_idb->nli_blink->blink = NULL;
	fr_idb->nli_flink = nli;
    	reset_interrupt_level(level);
	return;
    }
    /*
     * If the queue length is non-zero, both head and tail
     * must be non-zero. add this svc to the end of the queue
     * and adjust pointers.
     */
    fr_idb->nli_blink->flink = nli;
    tmp_nli = fr_idb->nli_blink;
    fr_idb->nli_blink = nli;
    fr_idb->nli_blink->flink = NULL;
    fr_idb->nli_blink->blink = tmp_nli;
    reset_interrupt_level(level);
}
/*
 * fr_svc_l3if_action
 * Protocol layer <-----> L3 IF interaction
 * This routine is called when the network layer wants to send out 
 * a packet, but no path currently exists and a maplist is defined
 * for this particular destination. We're assuming that we have 
 * already done a map lookup (in frame_relay.c within the fr_encaps
 * routine. Also assuming that if there exists a NLI structure with 
 * call state in CALL_CNF, we will not be in this function.
 */
boolean fr_svc_l3if_action (
	fr_idb_struct_type *fr_idb,
	paktype *pak,
	staticmap_type *map,
	uint call_id,
	uchar event)
{
    fr_svc_nli_type 	*nli;
    uint		ret;
    fr_params_type	*fr_params;
    struct fr_svc_params *svc_params;
    
    ret = FALSE;
    /* 
     * we come to this routine after having done a static map table 
     * search and so we are guaranteed to have a params structure 
     * in place. Except if someone did a "no map-class fr <name>"
     * before turning off SVCs. In which case map->class will not 
     * exist. 
     */
    if (!map)
	return(ret);

    if (!map->class) {
	/* 
 	 * Appears to be a configuration problem or 
	 * someone has removed a map. 
	 */
	if (map->nli)
	    fr_svc_rem_nli_from_idb_queue(fr_idb, map, map->nli);
	return(ret);
    }

    fr_params = (fr_params_type *)map->class->params;

    if (!fr_params)
	return(ret);
    else
	svc_params = fr_params->svc_params;

    if (!svc_params)
	return (ret);

    /* 
     * The nli structure will already exist when calls are being torn down. 
     * But will not exist when a call is being placed. 
     */
    nli = map->nli;

    switch (event) {
	/* 
	 * INTERNAL CALL SETUP REQUEST
	 */
	case L3_CALL_REQ:
    	    if (!nli) {
		nli = malloc_named(sizeof(fr_svc_nli_type), "FR SVC NLI I");
		if (!nli)
	    	    break;

		map->nli = nli;
		fr_svc_initialize_nli(fr_idb, pak, nli, map);
		nli->map_params = svc_params;
		nli->swidb = pak->if_output;
		fr_svc_add_nli_to_idb_queue(fr_idb, nli);
    	    }
    
	    /* 
	     * The call must be in the NULL state. If we are in any other 
	     * state, we may have a problem. We could also be here if the 
	     * call is being triggered by a broadcast packet.
	     */
    	    if ((nli->nl_state == STATE_NL_NULL) || 
				(nli->nl_state == STATE_L3_CALL_REQ)) {

		/* 
		 * Get some information from the map-list configuration.
		 * retrieve the local address, the destination address etc.
		 * Also the number type, the number length etc. 
		 */
		memcpy((uchar *)&svc_params->svc_addr.saddr, 
			(uchar *)&map->list_ptr->addr.saddr,
		      	sizeof(svc_addr_block));

		/* 
		 * Probably need to find a better place to set the MTU. 
		 * The reason it is done here is to accomodate changes in 
		 * MTU. However, ther should be no reason for anyone to 
		 * change the MTU on the serial.
		 */
		svc_params->inc_fmif =  pak->if_output->hwptr->maxmtu;
		svc_params->out_fmif =  pak->if_output->hwptr->maxmtu;
		svc_params->inc_max_fmif =  pak->if_output->hwptr->maxmtu;
		svc_params->out_max_fmif =  pak->if_output->hwptr->maxmtu;

    		ret = svc_process_nli_event(nli, pak, map, L3_CALL_REQ, nli->call_id);
    	    } else {
		/* 
		 * The circumstances leading to this code path
		 * should only happen for broadcast packets. If the call is not 
		 * in the NULL state and is not in the CALL_REQ state, we should 
		 * have an entry in the fr map table. For broadcast packets, we need
		 * to trigger an SVC if the "broadcast" flag is set. But we don't
		 * want to search the FR map table because proto_addr probably 
		 * contains a broadcast address and not a specific destination.
		 */
		if (NLI_DBG)
	    	    buginf("\nNLI: Broadcasting over an SVC, DLCI %d", nli->dlci);
    	    }
	    break;

	/* 
	 * INTERNAL CALL TEARDOWN REQUEST
	 */
	case L3_CALL_REL_REQ: 
	    nli = fr_svc_nli_lookup(fr_idb, call_id);

	    if (!nli)
		return(ret);

	    if (NLI_DBG)
		buginf("\nNLI: L3 Call Release Req for Call ID %d", call_id);

	    if ((nli->nl_state != STATE_NL_CALL_CNF) &&
		(nli->nl_state != STATE_NL_CALL_REQ) &&
		(nli->nl_state != STATE_NL_CALL_IND)) {
		if (NLI_DBG)
		    buginf("\nNLI: L3 Call Release Req for non-confirmed Call ID %d", 
						call_id);
	    }	
	    ret = svc_process_nli_event(nli, pak, map, L3_CALL_REL_REQ, nli->call_id);
	    fr_svc_rem_nli_from_idb_queue(fr_idb, map, nli);
	    break;
	default: 
	    break;
    }
    return(ret);
}

/* 
 * Transitions from the STATE_NL_NULL State
 */
const static NLStateTable NL_StateTable_00[] = {
	{NL_CALL_REQ,		NLx_Unknown},
	{NL_CALL_IND, 		NL0_CallInd}, 
	{NL_CALL_CNF, 		NLx_CallCnf}, 
	{NL_CALL_REJ, 		NLx_Unknown}, 
	{NL_REL_REQ, 		NLx_Unknown},
	{NL_REL_IND, 		NLx_RelInd},
	{NL_REL_CNF, 		NLx_RelCnf},
	{NL_REL_REJ, 		NLx_Unknown},
	{L3_CALL_REQ, 		NL0_L3CallReq},
	{L3_CALL_REL_REQ, 	NLx_L3CallRej},
	{MSG_UNKNOWN,		NLx_Unknown}
};
/* 
 * Transitions from the STATE_NL_CALL_REQ State
 */
const static NLStateTable NL_StateTable_01[] = {
	{NL_CALL_REQ,		NLx_Unknown},
	{NL_CALL_IND, 		NL1_CallInd}, 
	{NL_CALL_CNF, 		NLx_CallCnf}, 
	{NL_CALL_REJ, 		NLx_Unknown}, 
	{NL_REL_REQ, 		NLx_Unknown},
	{NL_REL_IND, 		NLx_RelInd},
	{NL_REL_CNF, 		NLx_RelCnf},
	{NL_REL_REJ, 		NLx_Unknown},
	{L3_CALL_REQ, 		NLx_Unknown},
	{L3_CALL_REL_REQ, 	NLx_L3CallRej},
	{MSG_UNKNOWN,		NLx_Unknown}
};
/* 
 * Transitions from the STATE_NL_CALL_IND State
 */
const static NLStateTable NL_StateTable_02[] = {
	{NL_CALL_REQ,		NLx_Unknown},
	{NL_CALL_IND, 		NLx_Unknown}, 
	{NL_CALL_CNF, 		NLx_CallCnf}, 
	{NL_CALL_REJ, 		NLx_Unknown}, 
	{NL_REL_REQ, 		NLx_Unknown},
	{NL_REL_IND, 		NLx_RelInd},
	{NL_REL_CNF, 		NLx_RelCnf},
	{NL_REL_REJ, 		NLx_Unknown},
	{L3_CALL_REQ, 		NLx_Unknown},
	{L3_CALL_REL_REQ, 	NLx_L3CallRej},
	{MSG_UNKNOWN,		NLx_Unknown}
};
/* 
 * Transitions from the STATE_NL_CALL_CNF State
 */
const static NLStateTable NL_StateTable_03[] = {
	{NL_CALL_REQ,		NLx_Unknown},
	{NL_CALL_IND, 		NLx_Unknown}, 
	{NL_CALL_CNF, 		NLx_Unknown}, 
	{NL_CALL_REJ, 		NLx_Unknown}, 
	{NL_REL_REQ, 		NLx_Unknown},
	{NL_REL_IND, 		NLx_RelInd},
	{NL_REL_CNF, 		NLx_RelCnf},
	{NL_REL_REJ, 		NLx_Unknown},
	{L3_CALL_REQ, 		NLx_Unknown},
	{L3_CALL_REL_REQ, 	NLx_L3CallRej},
	{NL_MSG_UNKNOWN,	NLx_Unknown}
};
/* 
 * Transitions from the STATE_NL_CALL_REJ State
 */
const static NLStateTable NL_StateTable_04[] = {
	{NL_CALL_REQ,		NLx_Unknown},
	{NL_CALL_IND, 		NLx_Unknown}, 
	{NL_CALL_CNF, 		NLx_Unknown}, 
	{NL_CALL_REJ, 		NLx_Unknown}, 
	{NL_REL_REQ, 		NLx_Unknown},
	{NL_REL_IND, 		NLx_Unknown},
	{NL_REL_CNF, 		NLx_Unknown},
	{NL_REL_REJ, 		NLx_Unknown},
	{L3_CALL_REQ, 		NLx_Unknown},
	{L3_CALL_REL_REQ, 	NLx_L3CallRej},
	{NL_MSG_UNKNOWN,	NLx_Unknown}
};
/* 
 * Transitions from the STATE_NL_REL_REQ State
 */
const static NLStateTable NL_StateTable_05[] = {
	{NL_CALL_REQ,		NLx_Unknown},
	{NL_CALL_IND, 		NLx_Unknown}, 
	{NL_CALL_CNF, 		NLx_Unknown}, 
	{NL_CALL_REJ, 		NLx_Unknown}, 
	{NL_REL_REQ, 		NLx_Unknown},
	{NL_REL_IND, 		NLx_RelInd},
	{NL_REL_CNF, 		NLx_RelCnf},
	{NL_REL_REJ, 		NLx_Unknown},
	{L3_CALL_REQ, 		NLx_Unknown},
	{L3_CALL_REL_REQ, 	NLx_L3CallRej},
	{NL_MSG_UNKNOWN,	NLx_Unknown}
};
/* 
 * Transitions from the STATE_NL_REL_IND State
 */
const static NLStateTable NL_StateTable_06[] = {
	{NL_CALL_REQ,		NLx_Unknown},
	{NL_CALL_IND, 		NLx_Unknown}, 
	{NL_CALL_CNF, 		NLx_Unknown}, 
	{NL_CALL_REJ, 		NLx_Unknown}, 
	{NL_REL_REQ, 		NLx_Unknown},
	{NL_REL_IND, 		NLx_RelInd},
	{NL_REL_CNF, 		NLx_RelCnf},
	{NL_REL_REJ, 		NLx_Unknown},
	{L3_CALL_REQ, 		NLx_Unknown},
	{L3_CALL_REL_REQ, 	NLx_L3CallRej},
	{NL_MSG_UNKNOWN,	NLx_Unknown}
};
/* 
 * Transitions from the STATE_NL_REL_CNF State
 */
const static NLStateTable NL_StateTable_07[] = {
	{NL_CALL_REQ,		NLx_Unknown},
	{NL_CALL_IND, 		NLx_CallInd}, 
	{NL_CALL_CNF, 		NLx_CallCnf}, 
	{NL_CALL_REJ, 		NLx_Unknown}, 
	{NL_REL_REQ, 		NLx_Unknown},
	{NL_REL_IND, 		NLx_RelInd},
	{NL_REL_CNF, 		NLx_RelCnf},
	{NL_REL_REJ, 		NLx_Unknown},
	{L3_CALL_REQ, 		NLx_Unknown},
	{L3_CALL_REL_REQ, 	NLx_L3CallRej},
	{NL_MSG_UNKNOWN,	NLx_Unknown}
};
/* 
 * Transitions from the STATE_NL_REL_REJ State
 */
const static NLStateTable NL_StateTable_08[] = {
	{NL_CALL_REQ,		NLx_Unknown},
	{NL_CALL_IND, 		NLx_Unknown}, 
	{NL_CALL_CNF, 		NLx_Unknown}, 
	{NL_CALL_REJ, 		NLx_Unknown}, 
	{NL_REL_REQ, 		NLx_Unknown},
	{NL_REL_IND, 		NLx_Unknown},
	{NL_REL_CNF, 		NLx_Unknown},
	{NL_REL_REJ, 		NLx_Unknown},
	{L3_CALL_REQ, 		NLx_Unknown},
	{L3_CALL_REL_REQ, 	NLx_L3CallRej},
	{NL_MSG_UNKNOWN,	NLx_Unknown}
};

/* 
 * Transitions from the STATE_L3_CALL_REL_REQ State
 */
const static NLStateTable NL_StateTable_98[] = {
	{NL_CALL_REQ,		NLx_Unknown},
	{NL_CALL_IND, 		NLx_Unknown}, 
	{NL_CALL_CNF, 		NLx_Unknown}, 
	{NL_CALL_REJ, 		NLx_Unknown}, 
	{NL_REL_REQ, 		NLx_Unknown},
	{NL_REL_IND, 		NLx_Unknown},
	{NL_REL_CNF, 		NLx_RelCnf},
	{NL_REL_REJ, 		NLx_Unknown},
	{L3_CALL_REQ, 		NLx_Unknown},
	{L3_CALL_REL_REQ, 	NLx_L3CallRej},
	{NL_MSG_UNKNOWN,	NLx_Unknown}
};

/* 
 * Transitions from the STATE_L3_CALL_REQ State
 */
const static NLStateTable NL_StateTable_99[] = {
	{NL_CALL_REQ,		NLx_Unknown},
	{NL_CALL_IND, 		NLx_Unknown}, 
	{NL_CALL_CNF, 		NLx_CallCnf}, 
	{NL_CALL_REJ, 		NLx_Unknown}, 
	{NL_REL_REQ, 		NLx_Unknown},
	{NL_REL_IND, 		NLx_RelInd},
	{NL_REL_CNF, 		NLx_RelCnf},
	{NL_REL_REJ, 		NLx_Unknown},
	{L3_CALL_REQ, 		NL0_L3CallReq},
	{L3_CALL_REL_REQ, 	NLx_L3CallRej},
	{NL_MSG_UNKNOWN,	NLx_Unknown}
};

/* 
 * NL State transition function. For any existing state,
 * handle a particular message from L3 CC by indexing into a
 * table of current states.
 */
const static NLStateTable *NL_StateTableFunc[] = {
	NL_StateTable_00,		/* STATE_NL_NULL */
	NL_StateTable_01,		/* STATE_NL_CALL_REQ */
	NL_StateTable_02,		/* STATE_NL_CALL_IND */
	NL_StateTable_03,		/* STATE_NL_CALL_CNF */
	NL_StateTable_04,		/* STATE_NL_CALL_REJ */
	NL_StateTable_05,		/* STATE_NL_REL_REQ */
	NL_StateTable_06,		/* STATE_NL_REL_IND */
	NL_StateTable_07,		/* STATE_NL_REL_CNF */
	NL_StateTable_08,		/* STATE_NL_REL_REJ */
	NL_StateTable_98,		/* STATE_L3_CALL_REL_REQ*/
	NL_StateTable_99,		/* STATE_L3_CALL_REQ */
	NULL				/* End of table */
};

/* 
 * svc_process_nli_event
 * The central dispatch point for NL processing. This is the core function 
 * from where packets are queued, NL state changes are controlled, etc. 
 * This routine handles both incoming and outgoing instances. 
 */
boolean svc_process_nli_event (
		fr_svc_nli_type *nli,
		paktype *pak,
		staticmap_type *map,
		uchar	event, 
		uint call_id)
{
    const NLStateTable		*state;
    int				iy;

    iy = 0;
    state = NL_StateTableFunc[nli->nl_state];

    if (NLI_DBG)
    	buginf("\nNLI STATE: %s, Call ID %d state %d", print_nl_event(event), 
							call_id, nli->nl_state);

    while (TRUE) {
	if (NLI_DBG)
	    buginf("\nNLI: Walking the event table %d", ++iy);
	if (state->message == event) {
	    break;
	} else {
	    if (state->message == NL_MSG_UNKNOWN) {
		if (NLI_DBG)
	    	buginf("\nNLI STATE: Unknown event/state, event %s", 
					print_nl_event(event));
		return(FALSE);
	    } else {
	    	state++;
		continue;
	    }
	}
    }
    (state->EventFunc)(nli, pak, map, call_id, event);        

    /* 
     * Always return TRUE. This mechanism may need to change 
     * to allow for better control.
     */
    return(TRUE);
}
/* 
 * print_nl_event
 * Utility event for printing debugs. 
 */
char *print_nl_event(uchar event)
{
    return(nli_event_help[event-1].help);
}

/* 
 * print_nl_state
 * Utility event for printing debugs
 */
char *print_nl_state(uchar state)
{
    return(nli_state_help[state].help);
}

/* 
 * NLx_Unknown 
 * Handle unknown state/event combinations. 
 */
void NLx_Unknown (
	fr_svc_nli_type *nli,
	paktype *pak,
	staticmap_type *map,
	uint call_id,
	uint event)
{
    uchar 		tstate;
    fr_idb_struct_type 	*fr_idb;

    if (NLI_DBG)
    	buginf("\nNLI: NLx_Unknown"); 

    tstate = nli->nl_state;
    nli->nl_state = STATE_NL_NULL;

    if (NLI_DBG)
    	buginf("\nNLI: Unknown State/Event, id %d, init %s, event %s, state %s",
			print_nl_state(tstate), 
			call_id, print_nl_event(event), 
			print_nl_state(nli->nl_state));
    /* 
     * Will this point be reached always due to an incoming packet??
     * If it is an outgoing packet, pak->if_input may not be set!!
     */
    fr_idb = pak->if_input->hwptr->frame_relay_stuff;
    fr_svc_rem_nli_from_idb_queue(fr_idb, map, nli);
}
/* 
 * NL0_CallInd 
 * Incoming Call Indication. The call is currently in the NULL STATE. 
 * This is an incoming call and L3 CC has seen a CALL PROCEEDING. 
 */
void NL0_CallInd (
	fr_svc_nli_type *nli,
	paktype *pak,
	staticmap_type *map,
	uint call_ref,
	uint event)
{
    uchar		tstate;
    fr_idb_struct_type 	*fr_idb;
    fr_params_type	*fr_params;
    frmaptype 		*ret;
    boolean 		p2p_ret, success;
    mapgroup_type 	*group_ptr;
    staticmap_type	*mapi;
    maplist_type	*list_ptr;
    int 		iz, map_count;
    idbtype 		*swidb;
    addrtype 		proto_addr;
    addrtype		*pProto;

    if (NLI_DBG)
    	buginf("\nNLI: NL0_CallInd"); 

    map_count = 0;
    ret = 0;
    pProto = NULL;
    success = FALSE;
    if (!nli) {
	nli = malloc_named(sizeof(fr_svc_nli_type), "FR SVC NLI II");
	if (!nli)
	    return;
	fr_idb = pak->if_input->hwptr->frame_relay_stuff;
	fr_svc_initialize_nli(fr_idb, pak, nli, map);
	nli->swidb = pak->if_input;

	fr_params = (fr_params_type *)map->params;
	nli->map_params = fr_params->svc_params;
	map->nli = nli;

	fr_svc_add_nli_to_idb_queue(fr_idb, nli);
    }

    tstate = nli->nl_state;
    nli->nl_state = STATE_NL_CALL_IND;


    FOR_ALL_SWIDBS_ON_HW(pak->if_input->hwptr, swidb) {
    	FOR_ALL_MAPGROUPS_ON_IDB(swidb, group_ptr) {
	list_ptr = group_ptr->maplist;
	FOR_ALL_MAPS_IN_LIST(iz, list_ptr, mapi) {
	    if (NLI_DBG)
		buginf("\nChecking maplist \"%s\" ", list_ptr->name);

	    if (mapi->list_ptr != map->list_ptr)
		continue; 
	    if (NLI_DBG)
		buginf("\nworking with maplist \"%s\" ", mapi->list_ptr->name);
	    /* 
	     * First, look to see whether a map already exists. 
	     * If one does, quietly get out. If a map does not
	     * exist, enter one.
	     */
	    if (is_p2p(swidb)) {
		memset(&proto_addr, 0, sizeof(addrtype));
		memcpy(&proto_addr.addr, (uchar *)&swidb, sizeof(swidb));
		proto_addr.length = sizeof(swidb);

	    	ret = reg_invoke_fr_addr_lookup(swidb, &proto_addr, LINK_ILLEGAL);
	    } else  {
		if (LINK_LEARN_OK(mapi->protocol)) {
		    memset(&proto_addr, 0, sizeof(addrtype));
		    pProto = &proto_addr;
		    pProto->addr.fr_address = nli->dlci;
	    	    ret = reg_invoke_fr_addr_lookup(swidb, &proto_addr, mapi->protocol);
		} else { 
	    	    ret = reg_invoke_fr_addr_lookup(swidb, &mapi->addr, mapi->protocol);
		}
	    }

	    if (!ret) {
		if (is_p2p(swidb)) {
		    p2p_ret = reg_invoke_fr_p2p_intf_dlci(swidb, nli->dlci,  
				mapi->broadcast, 
				(mapi->encap_flavor == ET_FR_IETF ? FR_ENCAPS_IETF:
				FR_ENCAPS_CISCO), FR_LINK_ILLEGAL_SVC, 
				mapi->class, proto_addr);
		    if (p2p_ret)
			success = TRUE;
		} else {
		    if (LINK_LEARN_OK(mapi->protocol)) {
			memset(&proto_addr, 0, sizeof(addrtype));
			pProto = &proto_addr;
			pProto->addr.fr_address = nli->dlci;
    	    	    	ret = reg_invoke_fr_addr_enter(swidb, &proto_addr, nli->dlci, 
		    	mapi->broadcast, (mapi->encap_flavor == ET_FR_IETF ? 
			FR_ENCAPS_IETF: FR_ENCAPS_CISCO), FR_MAP_SVC_DYNAMIC, 
		    	SVC_DYNAMIC, mapi->protocol, NO_OPTIONS, mapi->class);
		    } else {
    	    	    	ret = reg_invoke_fr_addr_enter(swidb, &mapi->addr, nli->dlci, 
		    	mapi->broadcast, (mapi->encap_flavor == ET_FR_IETF ? 
			FR_ENCAPS_IETF: FR_ENCAPS_CISCO), FR_MAP_SVC_DYNAMIC, 
		    	SVC_DYNAMIC, mapi->protocol, NO_OPTIONS, mapi->class);
		    }
		    /* 
		     * Here is some gory code. A multipoint interface can have 
		     * multiple maps associated with it. Maps are created when 
		     * the SVC gets created. All pointers are maintained within 
		     * the frmap array. 
		     */
		    if (ret) {
    	    		nli->frmap[map_count++] = ret;
			success = TRUE;
		    }
		}
	    } else {
		/* 
		 * This path is taken when a map already exists. 
		 * We only want to update some information in the 
		 * database.
		 */
		success = TRUE;
	    }

	    /* 
	     * Update the nli pointer within the maps. There is a case 
	     * where we may fail to set up a map because one already exists. 
	     * Because of such cases, we need to look at the "success" value. 
	     */
	    if (success) {
		mapi->nli = nli;
	    	mapi->sm_swidb = swidb;
	    }
	}
    	} 
    }

    if (NLI_DBG)
    	buginf("\nNLI: State :%s, Event : %s, Next : %s", 
			print_nl_state(tstate), print_nl_event(event), 
			print_nl_state(nli->nl_state));
}
/* 
 * NL1_CallInd 
 * Incoming Call Indication. But we are already in CALL REQ state. 
 * ignore this message. This is a little different from NLx_CallInd 
 * in that no NL state change occurs.
 */
void NL1_CallInd (
	fr_svc_nli_type *nli,
	paktype *pak,
	staticmap_type *map,
	uint call_ref,
	uint event)
{
    uchar 		tstate;

    if (NLI_DBG)
    	buginf("\nNLI: NL1_CallInd"); 

    tstate = nli->nl_state;

    if (NLI_DBG)
    	buginf("\nNLI: State : %s, Event : %s, Next : %s", 
			print_nl_state(tstate), print_nl_event(event), 
			print_nl_state(nli->nl_state));
}
/* 
 * NLx_CallInd 
 * Incoming Call Indication. Meaning that L3 CC saw a CALL PROCEEDING. 
 */
void NLx_CallInd (
	fr_svc_nli_type *nli,
	paktype *pak,
	staticmap_type *map,
	uint call_ref,
	uint event)
{
    uchar		tstate;

    if (NLI_DBG)
    	buginf("\nNLI: NLx_CallInd"); 

    tstate = nli->nl_state;
    /* 
     * We have a CALL IND in a state other than NULL. This can only
     * be if we initiated the call. In which case nli must be non-zero.
     */
    if (!nli)
	return;

    nli->nl_state = STATE_NL_CALL_IND;

    if (NLI_DBG)
    	buginf("\nNLI: State : %s, Event : %s, Next : %s", 
			print_nl_state(tstate), print_nl_event(event), 
			print_nl_state(nli->nl_state));
}
/* 
 * NLx_CallCnf 
 * Incoming Call Confirmation. Meaning that L3 CC saw a CALL CONNECT. 
 * When in the STATE_NL_NULL, we could potentially see a NL_CALL_CNF
 * without seeing a NL_CALL_IND. This happens if we receive a CONNECT
 * message in response to a SETUP, instead of receiving a CALL PROCEEDING
 * message. 
 */
void NLx_CallCnf (
	fr_svc_nli_type *nli,
	paktype *pak,
	staticmap_type *map,
	uint call_ref,
	uint event)
{
    uchar		tstate;
    frmaptype		*ret;
    boolean 		p2p_ret, success;
    mapgroup_type 	*group_ptr;
    staticmap_type	*mapi;
    maplist_type	*list_ptr;
    int 		iy, map_count;
    frame_relay_hdr 	*hdr;
    idbtype 		*swidb;
    addrtype            proto_addr;
    addrtype		*pProto;
    fr_idb_struct_type 	*fr_idb;

    if (NLI_DBG)
    	buginf("\nNLI: NLx_CallCnf"); 

    map_count = 0;
    ret = 0;
    pProto = NULL;
    success = FALSE;
    tstate = nli->nl_state;
    nli->nl_state = STATE_NL_CALL_CNF;
	
    if (NLI_DBG)
    	buginf("\nNLI: State : %s, Event : %s, Next : %s", 
			print_nl_state(tstate), print_nl_event(event), 
			print_nl_state(nli->nl_state));

    FOR_ALL_SWIDBS_ON_HW(pak->if_input->hwptr, swidb) {
    FOR_ALL_MAPGROUPS_ON_IDB(swidb, group_ptr) {
	list_ptr = group_ptr->maplist;
	FOR_ALL_MAPS_IN_LIST(iy, list_ptr, mapi) {
	    if (NLI_DBG)
		buginf("\nChecking maplist \"%s\" ", list_ptr->name);

	    if (mapi->list_ptr != map->list_ptr)
		continue; 

	    if (NLI_DBG)
		buginf("\nworking with maplist \"%s\" ", mapi->list_ptr->name);
	    /* 
	     * First, look to see whether a map already exists. 
	     * If one does, quietly get out. If a map does not
	     * exist, enter one.
	     */
	    if (is_p2p(swidb)) {
		memset(&proto_addr, 0, sizeof(addrtype));
		memcpy(&proto_addr.addr, (uchar *)&swidb, sizeof(swidb));
		proto_addr.length = sizeof(swidb);

		ret = reg_invoke_fr_addr_lookup(swidb,  &proto_addr, LINK_ILLEGAL);
	    } else {
		if (LINK_LEARN_OK(mapi->protocol)) {
		    memset(&proto_addr, 0, sizeof(addrtype));
		    pProto = &proto_addr;
		    pProto->addr.fr_address = nli->dlci;
	    	    ret = reg_invoke_fr_addr_lookup(swidb, &proto_addr, mapi->protocol);
		} else { 
	    	    ret = reg_invoke_fr_addr_lookup(swidb, &mapi->addr, mapi->protocol);
		}
	    }

	    if (!ret) {
		if (is_p2p(swidb)) {
			p2p_ret = reg_invoke_fr_p2p_intf_dlci(swidb, nli->dlci,  
				mapi->broadcast,
				(mapi->encap_flavor == ET_FR_IETF ? FR_ENCAPS_IETF:
				FR_ENCAPS_CISCO), FR_LINK_ILLEGAL_SVC, 
				 mapi->class, proto_addr);
		    if (p2p_ret)
			success = TRUE;
		} else {
		    if (LINK_LEARN_OK(mapi->protocol)) {
			memset(&proto_addr, 0, sizeof(addrtype));
			pProto = &proto_addr;
			pProto->addr.fr_address = nli->dlci;
    	    	    	ret = reg_invoke_fr_addr_enter(swidb, &proto_addr, nli->dlci, 
		    	mapi->broadcast, (mapi->encap_flavor == ET_FR_IETF ? 
			FR_ENCAPS_IETF: FR_ENCAPS_CISCO), FR_MAP_SVC_DYNAMIC, 
		    	SVC_DYNAMIC, mapi->protocol, NO_OPTIONS, mapi->class);
		    } else {
	    	    	ret = reg_invoke_fr_addr_enter(swidb, &mapi->addr, nli->dlci, 
		    	mapi->broadcast, (mapi->encap_flavor == ET_FR_IETF ? 
			FR_ENCAPS_IETF: FR_ENCAPS_CISCO), FR_MAP_SVC_DYNAMIC, 
		    	SVC_DYNAMIC, mapi->protocol, NO_OPTIONS, mapi->class);
		    }
		    if (ret) {
		    	/* 
		     	 * Here is some gory code. A multipoint interface can have 
		     	 * multiple maps associated with it. Maps are created when 
		     	 * the SVC gets created. All pointers are maintained within 
		     	 * the frmap array. 
		     	 */
    	    		nli->frmap[map_count++] = ret;
			success = TRUE;
		    }
		}
	    } else {
		/* 
		 * This path is taken when a map already exists. 
		 * We only want to update some information in the 
		 * database.
		 */
		success = TRUE;
	    }

	    /* 
	     * Update the nli pointer within the maps. There is a corner case 
	     * where we may fail to set up a map because one already exists. 
	     * Because of such cases, we need to look at the "success" value. 
	     */
	    if (success) {
		mapi->nli = nli;
	    	mapi->sm_swidb = swidb;
	    }
	}
    }
    }

    /* 
     * need to empty the packets queued up on the holdq. 
     * Packets have already been encapsulated but with a DLCI value of 0.
     * All we need to do is stuff in the right DLCI and the linktype.
     */
    fr_idb = pak->if_input->hwptr->frame_relay_stuff;
    while (TRUE) {
	pak = p_dequeue(&nli->svc_holdQ);
	if ( !pak )
	    break;
	hdr = (frame_relay_hdr *)pak->datagramstart;
	PUTSHORT(&hdr->dlci, (NUM_TO_DLCI(nli->dlci) | FRAME_RELAY_EA_BITS));
	PUTSHORT(&hdr->type, link2serial(pak->linktype));
	if (NLI_DBG)
	    buginf("\nNLI: Emptying holdQ, link %d, dlci %d, size %d", 
			pak->linktype, nli->dlci, pak->datagramsize);
	datagram_out(pak);
    }
}
/* 
 * NLx_RelInd
 * Incoming Release Indication. Meaning that L3 CC saw a incoming DISCONNECT, a 
 * incoming RELEASE or an outgoing DISCONNECT. 
 */
void NLx_RelInd (
	fr_svc_nli_type *nli,
	paktype *pak,
	staticmap_type *map,
	uint call_id,
	uint event)
{
    uchar		tstate;
    mapgroup_type 	*group_ptr;
    staticmap_type	*mapi;
    maplist_type	*list_ptr;
    int 		ix, map_count;
    idbtype		*swidb;

    if (NLI_DBG)
    	buginf("\nNLI: NLx_RelInd %s", pak->if_input->namestring); 

    map_count = 0;
    tstate = nli->nl_state;
    nli->nl_state = STATE_NL_REL_IND;
	
    FOR_ALL_SWIDBS_ON_HW(pak->if_input->hwptr, swidb) {
    FOR_ALL_MAPGROUPS_ON_IDB(swidb, group_ptr) {
	list_ptr = group_ptr->maplist;
	FOR_ALL_MAPS_IN_LIST(ix, list_ptr, mapi) {
    	    if (mapi && (mapi->list_ptr == map->list_ptr)) {
		mapi->nli = NULL; 
	    	mapi->sm_swidb = NULL;
		/* 
		 * If this is a P2P interface, remove the interface dlci.
		 */
		if (is_p2p(swidb)) {
		    reg_invoke_fr_p2p_intf_dlci_remove(swidb, nli->dlci);
		    continue;
		}
		/* 
		 * If this is not a P2P subif, we need to clear 
		 * the frmap array to clean out all maps. The idea is that 
		 * all maps are removed so that ordering does not matter.
		 */
		if (nli->frmap[map_count]) {
    		    reg_invoke_fr_addr_remove(nli->frmap[map_count]);
		}
		nli->frmap[map_count] = NULL;
		map_count++;
    	    }
	}
    }
    }

    if (NLI_DBG)
    	buginf("\nNLI: NLx_RelInd State : %s, Event : %s, Next : %s", 
			print_nl_state(tstate), print_nl_event(event), 
			print_nl_state(nli->nl_state));
    /* 
     * Clean out the NLI from the queues that we maintain. 
     * fr_idb is not available, so I can't do the following! :-(
     * fr_svc_rem_nli_from_idb_queue(fr_idb, map, nli);
     */
    /*
     * Clean out the holdQ
     */
    while (TRUE) {
	pak = p_dequeue(&nli->svc_holdQ);
	if ( !pak )
	    break;
	datagram_done(pak);
    }
}
/* 
 * NLx_RelCnf
 * Incoming Release Confirmation. Meaning that L3 CC had an outgoing DISCONNECT which
 * got a RELEASE as a response or L3 CC sees an incoming RELEASE. 
 */
void NLx_RelCnf (
	fr_svc_nli_type *nli,
	paktype *pak,
	staticmap_type *map,
	uint call_id,
	uint event)
{
    uchar		tstate;
    fr_idb_struct_type	*fr_idb;
    mapgroup_type 	*group_ptr;
    staticmap_type	*mapi;
    maplist_type	*list_ptr;
    int 		iu, map_count;
    idbtype 		*swidb;

    if (NLI_DBG)
    	buginf("\nNLI: NLx_RelCnf"); 

    map_count = 0;
    tstate = nli->nl_state;
    nli->nl_state = STATE_NL_NULL;
	
    if (NLI_DBG)
    	buginf("\nNLI: State : %s, Event : %s, Next : %s", 
			print_nl_state(tstate), print_nl_event(event), 
			print_nl_state(nli->nl_state));

    fr_idb = pak->if_input->hwptr->frame_relay_stuff;

    FOR_ALL_SWIDBS_ON_HW(pak->if_input->hwptr, swidb) {
    FOR_ALL_MAPGROUPS_ON_IDB(swidb, group_ptr) {
	list_ptr = group_ptr->maplist;
	FOR_ALL_MAPS_IN_LIST(iu, list_ptr, mapi) {
    	    if (mapi && (mapi->list_ptr == map->list_ptr)) {
		mapi->nli = NULL; 
	    	mapi->sm_swidb = NULL;
		/* 
		 * If this is a P2P interface, remove the interface dlci.
		 */
		if (is_p2p(swidb)) {
		    reg_invoke_fr_p2p_intf_dlci_remove(swidb, nli->dlci);
		    continue;
		}
		/* 
		 * If this is not a P2P subif, we need to clear 
		 * the frmap array to clean out all maps. The idea is that 
		 * all maps are removed so that ordering does not matter.
		 */
		if (nli->frmap[map_count]) {
		    reg_invoke_fr_addr_remove(nli->frmap[map_count]);
		}
		nli->frmap[map_count] = NULL;
		map_count++;
    	    }
	}
    }
    }

    /*
     * Clean out the holdQ
     */
    while (TRUE) {
	pak = p_dequeue(&nli->svc_holdQ);
	if ( !pak )
	    break;
	datagram_done(pak);
    }
    /* 
     * Clean out the NLI from the queues that we maintain. 
     */
    fr_svc_rem_nli_from_idb_queue(fr_idb, map, nli);
}

/* 
 * NL0_L3CallReq
 * Call Request came from Layer 3. This means that this is an outgoing 
 * call. We need to allocate resources, queue the packet and return. 
 * Once the circuit gets established, we'll empty the queue.
 */
void NL0_L3CallReq (
	fr_svc_nli_type *nli,
	paktype *pak,
	staticmap_type *map,
	uint call_id,
	uint event)
{
    uchar		tstate;
    paktype		*newpak;
    boolean 		ret = 0;

    if (NLI_DBG)
    	buginf("\nNLI: NL0_L3CallReq"); 

    tstate = nli->nl_state;
    nli->nl_state = STATE_L3_CALL_REQ;
	
    if (NLI_DBG)
    	buginf("\nNLI: State : %s, Event : %s, Next : %s", 
			print_nl_state(tstate), print_nl_event(event), 
			print_nl_state(nli->nl_state));
    /* 
     * we need to put the packet on a holdq and continue. 
     * so that we don't hold up L3 functions
     */
    newpak = pak_duplicate(pak);
    if (!QUEUEFULL(&nli->svc_holdQ)) {
	p_enqueue(&nli->svc_holdQ, newpak);
	if (NLI_DBG)
	    buginf("\nNLI: Enqueued outgoing packet on holdq");
	if (tstate != STATE_NL_NULL)
	    return;
    } else {
	datagram_done(newpak);
	return;
    }
    /* 
     * The svc pointer is NULL at this point 'cause this is a new call
     * we need to send in the packet pointer because we need some idb 
     * information. 
     */
    if (tstate == STATE_NL_NULL)
    	ret = svc_process_l3_event(pak, NULL, nli->call_id, SETUP_REQUEST, 
			nli->map_params, OUT);
    if (!ret) {
	if (NLI_DBG)
	    buginf("\nNLI: Error in triggering call");
    }
}
/* 
 * NLx_L3CallRej
 * Call Reject came from Layer 3. Turning off SVCs or some other 
 * catastrophic event. 
 */
void NLx_L3CallRej (
	fr_svc_nli_type *nli,
	paktype *pak,
	staticmap_type *map,
	uint call_id,
	uint event)
{
    uchar		tstate;
    mapgroup_type 	*group_ptr;
    staticmap_type	*mapi;
    maplist_type	*list_ptr;
    int 		iw, map_count;
    idbtype 		*swidb; 
    fr_svc_info_type 	*svc;
    fr_idb_struct_type  *fr_idb;
    int			ret;

    if (NLI_DBG)
    	buginf("\nNLI: NLx_L3CallRej"); 

    map_count = 0;
    tstate = nli->nl_state;
    nli->nl_state = STATE_L3_CALL_REL_REQ;
	
    if (NLI_DBG)
    	buginf("\nNLI: State : %s, Event : %s, Next : %s", 
			print_nl_state(tstate), print_nl_event(event), 
			print_nl_state(nli->nl_state));
    if (tstate != STATE_NL_NULL) {
	fr_idb = pak->if_output->hwptr->frame_relay_stuff;
	for (svc = fr_idb->svc_head; svc; svc = svc->next) {
	    if (svc->call_id == nli->call_id) {
		break;
	    }
	}
	if (svc) {
    	    ret = svc_process_l3_event(pak, svc, nli->call_id, TIDL_TIMEOUT, 
			nli->map_params, OUT);
    	    nli->nl_state = STATE_NL_NULL;
	    if (!ret)
		return;
	}
    }
    nli->nl_state = STATE_NL_NULL;
    tstate = nli->nl_state;
    if (NLI_DBG)
    	buginf("\nNLI: junk:  State : %s, Event : %s, Next : %s", 
			print_nl_state(tstate), print_nl_event(event), 
			print_nl_state(nli->nl_state));

    /* 
     * we need to tear down the call if it exists. If it does not, 
     * do nothing. Release any relevant memory. If line goes down
     * because the user requested it or for unknown reasons, we don't
     * have a packet (pak is NULL) to deal with. In this case, walk all 
     * idbs. 
     */
    if (pak) {
	FOR_ALL_SWIDBS_ON_HW(pak->if_input->hwptr, swidb)
    	FOR_ALL_MAPGROUPS_ON_IDB(swidb, group_ptr) {
	    list_ptr = group_ptr->maplist;
	    FOR_ALL_MAPS_IN_LIST(iw, list_ptr, mapi) {
    	    	if (mapi && (mapi->list_ptr == map->list_ptr)) {
		    mapi->nli = NULL; 
	    	    mapi->sm_swidb = NULL;
		    if (is_p2p(swidb)) {
			reg_invoke_fr_p2p_intf_dlci_remove(swidb, nli->dlci);
			continue;
		    }
		    /* 
		     * If this is not a P2P subif, we need to clear 
		     * the frmap array to clean out all maps. The idea is that 
		     * all maps are removed so that ordering does not matter.
		     */
		    if (nli->frmap[map_count]) {
    		    	reg_invoke_fr_addr_remove(nli->frmap[map_count]);
		    }
		    nli->frmap[map_count] = NULL;
		    map_count++;
    	    	}
	    }
    	}
    } else {
	FOR_ALL_SWIDBS(swidb) {
    	    FOR_ALL_MAPGROUPS_ON_IDB(swidb, group_ptr) {
	    	list_ptr = group_ptr->maplist;
	    	FOR_ALL_MAPS_IN_LIST(iw, list_ptr, mapi) {
    	    	    if (mapi && (mapi->list_ptr == map->list_ptr)) {
		        mapi->nli = NULL; 
	    	        mapi->sm_swidb = NULL;
		    	if (is_p2p(swidb)) {
			    reg_invoke_fr_p2p_intf_dlci_remove(swidb, nli->dlci);
			    continue;
		    	}
		    	/* 
		     	 * If this is not a P2P subif, we need to clear 
		     	 * the frmap array to clean out all maps. The idea is that 
		     	 * all maps are removed so that ordering does not matter.
		     	 */
		    	if (nli->frmap[map_count]) {
    		    	    reg_invoke_fr_addr_remove(nli->frmap[map_count]);
			}
		        nli->frmap[map_count] = NULL;
		        map_count++;
    	    	    }
	    	}
    	    }
	}
    }
}
/*
 * fr_svc_call_id_to_nli
 * Central place where we link the incoming call ID to a nli structure.
 * This is critical because the NLI needs to know the parameters associated
 * with the incoming message.
 */
fr_svc_nli_type *fr_svc_call_id_to_nli (
    paktype *pak,
    fr_idb_struct_type *fr_idb,
    fr_svc_info_type *svc,
    uint call_id)
{
    fr_svc_nli_type	*nli;
    maplist_type 	*list_ptr; 
    int			in;
    staticmap_type	*map;
    boolean 		parameter_match, ret; 

    parameter_match = FALSE;

    list_ptr = fr_svc_nli_map_search(svc);

    if (!list_ptr)
	return(NULL);
    
    nli = fr_svc_nli_lookup(fr_idb, call_id);
    if (!nli) {
	nli = malloc_named(sizeof(fr_svc_nli_type), "FR SVC NLI III");
	if (!nli)
	    return(NULL);
	fr_svc_initialize_nli(fr_idb, NULL, nli, NULL);
	fr_svc_add_nli_to_idb_queue(fr_idb, nli);
    	FOR_ALL_MAPS_IN_LIST(in, list_ptr, map) {
	    if (map) {
		/* 
		 * There are multiple maps configured on one SVC. 
		 * Each of them must point to the same NLI structure.
		 * Also their call parameters must be the same.
  		 * Find the first map and set nli->map. The only reason for 
		 * doing this is because we'll access map->list_ptr later.
		 */
	   	nli->map = map;
		nli->map_params = ((fr_params_type *)map->class->params)->svc_params;
		break;
	    }
	}
    }
    /* 
     * All maps point to the same NLI which in turn points to the 
     * same DLCI. The call parameters are also the same.
     * 
     * Move the Min CIR values over from the map-class to the 
     * in_params block. We need it for record keeping.
     */
    nli->dlci = svc->params.dlci;
    nli->in_params = &svc->params;
    if (!svc->params.min_inc_cir) {
    	nli->in_params->min_inc_cir = nli->map_params->min_inc_cir;
    	nli->in_params->min_inc_cir_mag = nli->map_params->min_inc_cir_mag;
    	nli->in_params->min_inc_cir_mult = nli->map_params->min_inc_cir_mult;
    }
    if (!svc->params.min_out_cir) {
    	nli->in_params->min_out_cir = nli->map_params->min_out_cir;
    	nli->in_params->min_out_cir_mag = nli->map_params->min_out_cir_mag;
    	nli->in_params->min_out_cir_mult = nli->map_params->min_out_cir_mult;
    }
    /*
     * If this is an incoming call, move the address info over to the
     * map_params structure so that it is accessible to the FR SVC MIB.
     */
    if (nli->in_params && (nli->in_params->call_ref & 0x8000)) {
	memcpy((uchar *)&nli->map_params->svc_addr,
		(uchar *)&list_ptr->addr.saddr,
		sizeof(svc_addr_block));
    }

    parameter_match = nli_parameter_negotiation(svc, list_ptr); 

    if (!parameter_match) {
	/* 
	 * Our SVC version of parameter negotiation. 
	 * If we are unable to allocate resorces, tear down the call.
	 */
	ret = svc_process_l3_event(pak, svc, nli->call_id, SPL_RELEASE_REQ, 
			nli->map_params, IN);
    	fr_svc_rem_nli_from_idb_queue(fr_idb, nli->map, nli);
	return(NULL);
    }

    /* 
     * Successful case where parameter negotiation succeeded, and all 
     * is well.
     */
    return(nli);
}
/* 
 * fr_svc_nli_map_search
 * We run into all kinds of problems if the user has not 
 * configured a map-list on both ends of the call. When an 
 * incoming message is received, check the map-list table,
 * compare the calling-pty and the called-party addresses
 * with corresponding fields within entries in the maplist
 * and return the first match. 
 *
 * At this point we do not know  which protocol is initiating
 * the call and we do not care. We will set up VCs for all 
 * configured protocols. 
 */
maplist_type *fr_svc_nli_map_search( fr_svc_info_type *svc)
{
    maplist_type *list_ptr;
    boolean source, dest;

    source = FALSE;
    dest = FALSE;

    /* 
     * I made the decision to compare only the address fields
     * not the type fields or the plan fields. Lets keep this
     * check simple. shankar 5/95
     * There are two sections to each check. One where source 
     * address field is comapared with destination address field and the 
     * second where source address field is compared with the source 
     * address field. The first case will occur normally. The second case
     * will occur when the address fields do not get flipped around. The 
     * first case occurs when an incoming SETUP message is processed. Because 
     * of the way we store addresses in the SVC structure, replies to 
     * this setup message will not have the fields transposed.
     */
    FOR_ALL_MAPLISTS(list_ptr) {
	if (!strcmp((char *)&list_ptr->addr.daddr.addr, 
			(char *)&svc->params.svc_addr.saddr.addr)) {
	    if ((list_ptr->addr.daddr.subaddr == svc->params.svc_addr.saddr.subaddr)
		|| (list_ptr->addr.saddr.subaddr == svc->params.svc_addr.saddr.subaddr))
	    	source = TRUE;
	} else if ((svc->last_msg_seen != MSG_SETUP) && 
			(!strcmp((char *)&list_ptr->addr.saddr.addr,
			(char *)&svc->params.svc_addr.saddr.addr))) {
	    if ((list_ptr->addr.daddr.subaddr == svc->params.svc_addr.saddr.subaddr)
		|| (list_ptr->addr.saddr.subaddr == svc->params.svc_addr.saddr.subaddr))
	    	source = TRUE;
	}
	if (!strcmp((char *)&list_ptr->addr.saddr.addr, 
			(char *)&svc->params.svc_addr.daddr.addr)) {
	    if ((list_ptr->addr.saddr.subaddr == svc->params.svc_addr.daddr.subaddr)
		|| (list_ptr->addr.daddr.subaddr == svc->params.svc_addr.daddr.subaddr))
	    	dest = TRUE;
	} else if ((svc->last_msg_seen != MSG_SETUP) &&
			(!strcmp((char *)&list_ptr->addr.daddr.addr,
			(char *)&svc->params.svc_addr.daddr.addr))) {
	    if ((list_ptr->addr.saddr.subaddr == svc->params.svc_addr.daddr.subaddr)
		|| (list_ptr->addr.daddr.subaddr == svc->params.svc_addr.daddr.subaddr))
	    	dest = TRUE;
	}

	if (source && dest) {
	    if (NLI_DBG) {
		buginf("\nNLI: Map-list search: Found maplist %s", list_ptr->name);
		buginf("\ndaddr.subaddr %d, saddr.subaddr %d, saddr.subaddr %d",
				list_ptr->addr.daddr.subaddr,
				svc->params.svc_addr.saddr.subaddr,
				list_ptr->addr.saddr.subaddr);
		buginf("\nsaddr.subaddr %d, daddr.subaddr %d, daddr.subaddr %d",
				list_ptr->addr.saddr.subaddr,
				svc->params.svc_addr.daddr.subaddr,
				list_ptr->addr.daddr.subaddr);
	    }
	    return(list_ptr);
	}
	/* 
	 * Reset the flags before we look at the next map.
	 */
	source = FALSE;
	dest = FALSE;
    }
    if (NLI_DBG)
	buginf("\nNLI: Map-list search: Relevant map-list not found %s %s", 
				(char *)&svc->params.svc_addr.saddr.addr,
				(char *)&svc->params.svc_addr.daddr.addr);
return(NULL);
}
/* 
 * nli_parameter_negotiation
 * Negotiation of parameters between the calling side and the called 
 * side. The following criteria apply. 
 * - Accept any CIR as long as it is greater than the minimum configured
 * value. 
 * - EBS and CBS are not negotiated.
 */
boolean nli_parameter_negotiation (fr_svc_info_type *svc, maplist_type *list_ptr) 
{
    int			ik;
    staticmap_type	*map;
    mapclass_type 	*class_ptr;
    struct fr_svc_params *map_params = NULL;
    struct fr_svc_params *svc_params;
    boolean 		in_cir_okay, out_cir_okay, fmif_okay;
    int			ret;

    ret = FALSE;
    svc_params = &svc->params;
    in_cir_okay = FALSE;
    out_cir_okay = FALSE;
    fmif_okay = TRUE;

    if (NLI_DBG)
	buginf("\nnli_parameter_negotiation");

    if ((svc->last_msg_seen != MSG_SETUP) && 
			(svc->last_msg_seen != MSG_CONNECT)) {
	return(TRUE);
    }

    /* 
     * The LLC params IE is optional. It may or may not be 
     * included in the SETUP message. Return TRUE if it 
     * was not included. In the CONNECT message, the Link Layer Core IE 
     * is not mandatory.
     */
    if (!svc->ll_core_ie_present) {
	return(TRUE);
    }

    /* 
     * Very intense parameter negotiation follows.
     */
    FOR_ALL_MAPS_IN_LIST(ik, list_ptr, map) {
	in_cir_okay = FALSE;
	out_cir_okay = FALSE;
	if (map) {
	    class_ptr = map->class; 

	    /* 
 	     * There is a chance that the box has not been configured yet. 
 	     * In which case, class_ptr will be NULL.
	     */
	     if (!class_ptr || !(class_ptr->params))
		return(FALSE);

	    map_params = ((fr_params_type *)class_ptr->params)->svc_params;
	    
	    /* 
 	     * There is a chance that the box has not been configured yet. 
 	     * In which case, map_params will be NULL.
	     */
	     if (!map_params)
		return(FALSE);

	    /* 
    	     * Let's reject any MTU value that does not match what 
  	     * we have configured on our interfaces. 
	     */
	     if ((svc_params->inc_fmif != map_params->inc_fmif) ||
			(svc_params->out_fmif != map_params->out_fmif))
		fmif_okay = FALSE;
	    
	    /*
	     * Lets accept any indicated CIR as long as it is 
	     * greater than the minimum.
	     */
	    if(svc_params->inc_cir >= map_params->min_inc_cir) {
		in_cir_okay = TRUE; 
	    }

	    if(svc_params->out_cir >= map_params->min_out_cir) {
		out_cir_okay = TRUE; 
	    }
	    if (in_cir_okay && out_cir_okay && fmif_okay)
		ret = TRUE; 

	    return(ret);
	}
    }
    return(ret);
}
/* 
 * fr_svc_send_msg_to_nli
 * Interface routine between NLI and layer3CC. 
 * All SDL functions that have business with the NLI 
 * will call this function.
 */
boolean fr_svc_send_msg_to_nli (
	hwidbtype *hwidb,
	paktype *pak,
	fr_svc_info_type *svc,
	uint 	event)
{
    fr_svc_nli_type    	*nli;
    boolean 		ret;
    fr_idb_struct_type  *fr_idb;

    fr_idb = hwidb->frame_relay_stuff;

    nli = fr_svc_call_id_to_nli(pak, fr_idb, svc, svc->call_id);

    if (!nli)
	return(FALSE);

    ret = svc_process_nli_event(nli, pak, nli->map, event, svc->call_id);

    return(ret);
}


/* $Id: fr_svc.c,v 3.1.64.17 1996/09/11 16:49:46 liqin Exp $
 * $Source: /release/112/cvs/Xsys/wan/fr_svc.c,v $
 *------------------------------------------------------------------
 * fr_svc.c: Central file for frame relay SVCs. 
 *
 * May 1995, Shankar Natarajan
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: fr_svc.c,v $
 * Revision 3.1.64.17  1996/09/11  16:49:46  liqin
 * CSCdi68280:  replace malloc with malloc_named in frame relay code
 * Branch: California_branch
 *
 * Revision 3.1.64.16  1996/08/03  23:41:23  fox
 * CSCdi63400:  high priority compatibility queue must die
 * Branch: California_branch
 * Remove high priority compatibility queue and activatehigh flag.
 * Convert high priority cfork'd processes to new-style processes with
 * watched queues, timers, and booleans.  Split Multilink PPP input
 * into separate high priority process.  Split load and busy calculations
 * into separate processes.  Speed up PPP input and scheduler processing.
 * Miscellaneous code changes.
 *
 * Revision 3.1.64.15  1996/07/23  13:31:22  thille
 * CSCdi63329: Centralize logging for getbuffer() and pak_duplicate()
 * failures
 * Branch: California_branch
 * Shamelessly rip off Mr. Katz' code from CSCdi36037.  Provide
 * centralized logging for getbuffer() and pak_duplicate() failures and
 * then rip out the 9 bazillion different messages that where in the
 * various callers to report the failure.
 *
 * Revision 3.1.64.14  1996/07/04  17:05:40  chifei
 * CSCdi58333:  Some frame relay SVC commands are available in
 * non-enterprise images
 * Branch: California_branch
 * Moved command handling out of frame_relay_command()
 *
 * Revision 3.1.64.13  1996/07/03  01:57:48  chifei
 * CSCdi58333:  Some frame relay SVC commands are available in
 * non-enterprise images
 * Branch: California_branch
 * Modularized "frame svc" and "frame lapf" commands to go with FR SVC
 * subsystem; fixed/clean-up LAPF command format; consolidated DL
 * primitives define's.
 *
 * Revision 3.1.64.12  1996/07/01  19:10:40  skoh
 * CSCdi59475:  Change units for Be/Bc from bytes to bits
 * Branch: California_branch
 *
 * When Be and Bc value less than 131072, which can be fitted into
 * 14 bits fields(131072/8), is given, the value should not be converted to
 * the form of multiplier and magnitude.
 *
 * Revision 3.1.64.11  1996/06/27  21:38:15  skoh
 * CSCdi59475:  Change units for Be/Bc from bytes to bits
 * Branch: California_branch
 *
 * Revision 3.1.64.10  1996/06/25  14:39:56  shankar
 * CSCdi61103:  Broadcast triggering of SVCs needs a configuration hook
 * for users
 * Branch: California_branch
 *
 * Revision 3.1.64.9  1996/06/12  14:09:13  shankar
 * CSCdi57333:  FR SVC Timer handler needs SVC event intelligence
 * Branch: California_branch
 *
 * Revision 3.1.64.8  1996/06/06  21:48:58  shankar
 * CSCdi59783:  Stacktrace when configuring mapclass sub-commands
 * Branch: California_branch
 *
 * Revision 3.1.64.7  1996/05/30  23:27:46  ccpoe
 * CSCdi58460:  map-class frame-relay traffic-shaping features dont work
 * on FR SVC
 * Branch: California_branch
 *
 * Revision 3.1.64.6  1996/05/24  21:21:31  ccpoe
 * CSCdi58460:  map-class frame-relay traffic-shaping features dont work
 * on FR SVC
 * Branch: California_branch
 *
 * Revision 3.1.64.5  1996/05/23  14:32:19  chifei
 * CSCdi57332:  FR SVC initialization point needs to be re-examined
 * Branch: California_branch
 * This is layer 2 specific change.
 *
 * Revision 3.1.64.4  1996/05/22  23:38:19  shankar
 * CSCdi57332:  FR SVCs are initialized only when user configures it.
 * Branch: California_branch
 *
 * Revision 3.1.64.3  1996/05/20  05:37:47  skoh
 * CSCdi56032:  New SVC commands in 11.2 (partial)
 * Branch: California_branch
 * Syntax change in fr svc map-class subcommands.
 *
 * Revision 3.1.64.2  1996/05/15  05:54:39  fowler
 * Frame relay VC queuing and rate enforcement
 * Branch: California_branch
 *
 * Revision 3.1.64.1  1996/04/25  23:23:32  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.1.68.1  1996/04/24  04:11:25  shankar
 * Branch: ATM_Cal_branch
 * Patch FR_SVC_branch changes
 *
 * Revision 3.1  1995/11/09  13:49:05  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/09/01  18:47:07  shankar
 * Initial commit
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "passive_timers.h"
#include "sys_registry.h"
#include "media_registry.h"
#include "subsys.h"
#include "interface_private.h"
#include "packet.h"
#include "config.h"
#include "../ui/debug.h"
#include "parser.h"
#include "frame_relay.h"
#include "fr_debug.h"
#include "../if/network.h"
#include "../ui/parse_util.h"
#include "../parser/parser_defs_exec.h"
#include "../wan/parser_defs_frame.h"
#include "fr_switch.h"
#include "../h/types.h"
#include "../wan/fr_svc.h"
#include "fr_l2.h"
#include "../if/static_map.h"
#include "../wan/fr_svc_registry.h"
#include "../wan/fr_registry.regh"
#include "../wan/dialer_registry.h"

watched_boolean *fr_svc_timer_bool = NULL;
/* 
 * When the FR SVC subsystem gets started up, a process to parse
 * incoming messages and a process to handle call control timers
 * is created. These flags indicate whether SVCs are initialized 
 * or not. 
 */
static int ie_parse_process_pid = NO_PROCESS;
static int svc_timer_process_pid = NO_PROCESS;

/* 
 * fr_svc_init
 * The main function that initializes SVC related processes, functions
 * and other infrastructure.
 */
void fr_svc_init ()
{

    /* 
     * If SVCs have already been started, do nothing. 
     */
    if ((ie_parse_process_pid != NO_PROCESS) &&
	(svc_timer_process_pid != NO_PROCESS))
	return;

    if (ie_parse_process_pid == NO_PROCESS) {
	ie_parse_process_pid = process_create(l3_ie_parse_process, 
					"FR SVC IE Parser",
					LARGE_STACK, PRIO_HIGH);
    }
    if (svc_timer_process_pid == NO_PROCESS) {
	svc_timer_process_pid = process_create(fr_svc_timer_handler, 
					"FR SVC Timer Handler",
					 NORMAL_STACK, PRIO_NORMAL);
	fr_svc_timer_bool = create_watched_boolean("SVC Timer Flag", 0);
    }

    svc_outgoing_msg_init();
    svc_incoming_msg_init();
    reg_add_assign_svc_mem(fr_assign_svc_mem, "fr_assign_svc_mem");
    reg_add_staticmap_nvgen(ET_FRAME_RELAY, fr_map_nvgen, "fr_map_nvgen");
    reg_add_fr_svc_l3_action(fr_svc_l3if_action, "fr_svc_l3if_action");
    reg_add_fr_svc_l2_to_l3(fr_svc_l2_to_l3, "fr_svc_l2_to_l3");
    reg_add_fr_svc_periodic(fr_svc_periodic, "fr_svc_periodic");
    reg_add_fr_mapclass_parse(fr_mapclass_command, "FR mapclass params"); 
}
/* 
 * fr_map_nvgen
 * display map group attributes. Called from ../if/static_map.c and used 
 * to display map attributes when doing a "show config" or a "write term".
 */
void fr_map_nvgen (staticmap_type *map_ptr)
{
    char addrstr[64];

    if (!map_ptr)
	return;

    print_proto_addr(NULL, &map_ptr->addr, map_ptr->protocol, addrstr);

    nv_write(TRUE, "%s", addrstr);
    if (map_ptr->class)
	nv_add(TRUE, " class %s", map_ptr->class->name);
    if (map_ptr->broadcast)
	nv_add(TRUE, " %s", "broadcast");
    if (map_ptr->broadcast_trigger)
	nv_add(TRUE, " %s", "trigger");
    if (map_ptr->encap_flavor == ET_FR_IETF)  
	nv_add(TRUE, " %s", "ietf");
}
/* 
 * fr_svc_mapclass_nvgen
 * Does map-class nvgen
 */
void fr_svc_mapclass_nvgen (mapclass_type *class_ptr)
{
    int temp;

    fr_params_type *fr_params = class_ptr->params;
    struct fr_svc_params *svc_params = NULL;


    if (!fr_params)
	return;
    else
	svc_params = fr_params->svc_params;

    if (!svc_params)
	return;

    nv_write(svc_params->inc_cir && 
	     (svc_params->inc_cir != FR_SVC_IN_CIR_DEF), 
	     "%s %d", "frame-relay cir in", svc_params->inc_cir);
    nv_write(svc_params->out_cir &&
	     (svc_params->out_cir != FR_SVC_OUT_CIR_DEF), 
	     "%s %d", "frame-relay cir out", svc_params->out_cir);
    nv_write(svc_params->min_inc_cir && 
	     (svc_params->min_inc_cir != FR_SVC_MIN_IN_CIR_DEF), 
	     "%s %d", "frame-relay mincir in", svc_params->min_inc_cir);
    nv_write(svc_params->min_out_cir && 
	     (svc_params->min_out_cir != FR_SVC_MIN_OUT_CIR_DEF),
	     "%s %d", "frame-relay mincir out", svc_params->min_out_cir);

    temp = fr_svc_magmult_to_value(svc_params->inc_cbs_mag,
				   svc_params->inc_cbs);

    nv_write(svc_params->inc_cbs && (temp != FR_SVC_IN_CBS_DEF), "%s %d", 
	     "frame-relay bc in", (temp << 3));

    temp = fr_svc_magmult_to_value(svc_params->out_cbs_mag,
				   svc_params->out_cbs);

    nv_write(svc_params->out_cbs && (temp != FR_SVC_OUT_CBS_DEF), "%s %d", 
			"frame-relay bc out", (temp << 3));

    temp = fr_svc_magmult_to_value(svc_params->inc_ebs_mag,
				   svc_params->inc_ebs);

    nv_write(svc_params->inc_ebs && (temp != FR_SVC_IN_EBS_DEF), "%s %d", 
			"frame-relay be in", (temp << 3));

    temp = fr_svc_magmult_to_value (svc_params->out_ebs_mag,
				    svc_params->out_ebs);

    nv_write(svc_params->out_ebs && (temp != FR_SVC_OUT_EBS_DEF), 
	     "%s %d", "frame-relay be out", (temp << 3));

    nv_write(svc_params->holdq && (svc_params->holdq != FR_SVC_HOLDQ_SIZE), 
	     "%s %d", "frame-relay holdq", svc_params->holdq);

    nv_write(svc_params->idle_timer && 
	     (svc_params->idle_timer != FR_SVC_IDLE_TIMER), 
	     "%s %d", "frame-relay idle-timer", svc_params->idle_timer);

    nv_write(svc_params->inc_max_fmif &&
	     (svc_params->inc_max_fmif != FR_SVC_IN_FMIF), 
	     "%s %d", "frame-relay fmif in", svc_params->inc_max_fmif);
    nv_write(svc_params->out_max_fmif &&
	     (svc_params->out_max_fmif != FR_SVC_OUT_FMIF),
	     "%s %d", "frame-relay fmif out", svc_params->out_max_fmif);
}	

/*
 * fr_svc_magmult_to_value
 * Some values are internally represented by the combination of a magnitude
 * and a multiplier. But when we nvgen the value we must display the value
 * that the user entered.
 */
int fr_svc_magmult_to_value (uchar magnitude, uint multiplier)
{
    int temp, tmp_mult, ip;

    temp = 0;
    if (magnitude) {
        tmp_mult = 1;
        for (ip = 0; ip < magnitude; ip++)
            tmp_mult *= 10;
        temp = (multiplier) * tmp_mult;
    } else {
        temp = multiplier;
    }
    return(temp);
}

static void fr_svc_params_remove (fr_params_type *fr_params)
{
    if (fr_params->svc_params) {
	free(fr_params->svc_params);
    }
    fr_params->svc_params = NULL;
}

void fr_svc_mapclass_remove (mapclass_type *class_ptr)
{
    maplist_type        *list_ptr;
    staticmap_type      *mapi;
    int i;
    
    if (class_ptr->encap == ET_FRAME_RELAY) {
	FOR_ALL_MAPLISTS(list_ptr) {
	    FOR_ALL_MAPS_IN_LIST(i, list_ptr, mapi) {
		if (mapi->class == class_ptr) {
		    mapi->class = NULL;
		}
	    }
	}
    }
    if (class_ptr->params) {
	fr_svc_params_remove((fr_params_type *)class_ptr->params);
    }
}

/*
 * fr_supported_protocol
 * Verify that given protocol is supported. These days this includes 
 * pretty much everything under the sun.
 */
boolean fr_supported_protocol (uint protocol)
{
    return(
	(protocol == LINK_IP) ||
	(protocol == LINK_NOVELL) ||
	(protocol == LINK_APPLETALK) ||
	(protocol == LINK_VINES) ||
	(protocol == LINK_DECNET) ||
	(protocol == LINK_CLNS) ||
	(protocol == LINK_XNS) ||
	(protocol == LINK_PAD) ||
	(protocol == LINK_BRIDGE) ||
	(protocol == LINK_CDP) ||
	(protocol == LINK_RSRB) ||
	(protocol == LINK_STUN) ||
	(protocol == LINK_LLC2) ||
	(protocol == LINK_APOLLO));
}
/* 
 * fr_map_command
 * frame-relay map command used through the map-list 
 * The main function of this routine is to handle user entered 
 * combinations of <protocol, addr>. Keywords "broadcast" and 
 * "IETF" may also be specified by the user. Make sure that the 
 * fr_params_type structure records this information. 
 */
void fr_map_command (parseinfo *csb)
{
    staticmap_type *map_ptr;
    maplist_type *list_ptr;
    svc_addr_block *params;
    addrtype addr;
    uint protocol;
    boolean new_map;

    list_ptr = csb->maplist;
    protocol = GETOBJ(int,1);

    if (!fr_supported_protocol(protocol)) {
	printf("\n%% FR SVC Protocol not supported - %s", 
						print_linktype(protocol));
	return;
    }

    memset(&addr, 0, sizeof(addrtype));
    if(!parse_proto_addr(csb, NONULL, &addr, GETOBJ(paddr,1), protocol)){
	return;
    }

    map_ptr = staticmap_lookup_list(list_ptr, &addr, protocol, 
							ET_FRAME_RELAY);
    if (csb->sense)  {
	/*
	 * First update parameters given on the command line
	 */
	if (!map_ptr) {
	    params = malloc_named(sizeof(svc_addr_block), "FR SVC Static Map");
	    if (!params) {
		printf("\n%% Can't create map");
		return;
	    }
	    fr_svc_addr_block_init(params);
	    new_map = TRUE;
	} else {
	    params = (svc_addr_block *)map_ptr->params;
	    new_map = FALSE;
	}

	if (new_map)
	    fr_staticmap_add(list_ptr, NULL, &addr, protocol, 
						GETOBJ(string,2), params);
	else
	    staticmap_update(list_ptr, map_ptr, NULL, GETOBJ(string,2));

	/* 
	 * Keywords "broadcast" and "IETF" need to be registered within
	 * the staticmap type structure. This code path will 
	 * be followed even when a map is being updated so that the keywords
	 * can be removed or added. These keywords need to be stored within the 
	 * staticmap structure because they can be unique for each map and not 
	 * belong to a class of maps.
	 */
    	map_ptr = staticmap_lookup_list(list_ptr, &addr, protocol, 
							ET_FRAME_RELAY);
	if (new_map)
	    map_ptr->params = params;

	map_ptr->broadcast = GETOBJ(int,4);
	map_ptr->broadcast_trigger = GETOBJ(int,6);
	map_ptr->encap_flavor = GETOBJ(int,5) ? ET_FR_IETF : ET_FRAME_RELAY;

	/* 
	 * The source and the destination addresses were entered as a part
	 * of the map-list command. We need to let each fr_params_type 
	 * structure know this information. This keeps us from having to
	 * search the map-list when forming an outgoing SETUP message. 
	 */
	memcpy((uchar *)&params->saddr, (uchar *)&list_ptr->addr.saddr,
			sizeof(fr_svc_addr_type));
	memcpy((uchar *)&params->daddr, (uchar *)&list_ptr->addr.daddr,
			sizeof(fr_svc_addr_type));

    } else {
	if (map_ptr) {
	    staticmap_delete(list_ptr, map_ptr);

	    /* 
 	     * No FR SVC NLI action must happen on this map since 
	     * it no longer exists. 
	     */
	    map_ptr->class = NULL;
	} else {
	    printf("\n%% Can't find address map");
	}
    }
    return;
}

/* 
 * fr_staticmap_add
 * The interface into the code in ../if/static_map.c
 * All maps are entered as ET_FRAME_RELAY. The distinction between cisco
 * wncaps and IETF is identified by the "encap flavor" variable within 
 * the fr_params_type structure. 
 */
void fr_staticmap_add (
	maplist_type *list_ptr, 
	hwaddrtype *hwconf,
	addrtype *addr, 
	uint protocol, 
	char *class,
	svc_addr_block *params)
{
	if (!staticmap_enter(list_ptr, addr, hwconf, protocol, ET_FRAME_RELAY,
				MAP_STATIC, class, params))
	    printf("\n%% fr_staticmap_add: Can't add map to list");
}
/* 
 * fr_svc_addr_block_init
 * Initialize the address block that holds the local address, 
 * the destination address and the connected party address. 
 */
void fr_svc_addr_block_init (svc_addr_block *params)
{
    memset(&params->saddr, 0, sizeof(fr_svc_addr_type));
    memset(&params->daddr, 0, sizeof(fr_svc_addr_type));
    memset(&params->caddr, 0, sizeof(fr_svc_addr_type));
}

/* 
 * fr_params_init
 * Initialize Frame Relay static map parameters
 * All BW defaults are assumed to be 56000 or 
 * 56 * (10**3).
 */
void fr_params_init (struct fr_svc_params *svc_params)
{
    svc_params->call_ref = 0;

    svc_params->inc_fmif = FR_SVC_IN_FMIF;
    svc_params->out_fmif = FR_SVC_OUT_FMIF;
    svc_params->inc_max_fmif = FR_SVC_IN_FMIF;
    svc_params->out_max_fmif = FR_SVC_OUT_FMIF;

    svc_params->inc_cir = FR_SVC_IN_CIR_DEF;
    svc_params->inc_cir_mag = FR_SVC_MAG_DEF;
    svc_params->inc_cir_mult = FR_SVC_MULT_DEF;

    svc_params->out_cir = FR_SVC_OUT_CIR_DEF;
    svc_params->out_cir_mag = FR_SVC_MAG_DEF;
    svc_params->out_cir_mult = FR_SVC_MULT_DEF;

    svc_params->min_inc_cir = FR_SVC_IN_CIR_DEF;
    svc_params->min_inc_cir_mag = FR_SVC_MAG_DEF;
    svc_params->min_inc_cir_mult = FR_SVC_MULT_DEF;

    svc_params->min_out_cir = FR_SVC_OUT_CIR_DEF;
    svc_params->min_out_cir_mag = FR_SVC_MAG_DEF;
    svc_params->min_out_cir_mult = FR_SVC_MULT_DEF;

    svc_params->inc_cbs = FR_SVC_IN_CBS_DEF;
    svc_params->out_cbs = FR_SVC_OUT_CBS_DEF;

    svc_params->inc_cbs_mag = 0;
    svc_params->out_cbs_mag = 0;
    svc_params->inc_ebs_mag = 0;
    svc_params->out_ebs_mag = 0;

    svc_params->inc_ebs = FR_SVC_IN_EBS_DEF;
    svc_params->out_ebs = FR_SVC_OUT_EBS_DEF;

    svc_params->cause = 0;

    svc_params->dlci = 0;
    svc_params->req_svc_dlci = 0;
    svc_params->req_svc_type = FR_SVC_DEF_PRIORITY;

    svc_params->msg_type = 0;

    svc_params->idle_timer = FR_SVC_IDLE_TIMER;

    svc_params->holdq = FR_SVC_HOLDQ_SIZE;

    svc_params->broadcast = FALSE;
    svc_params->encapsulation = ET_FRAME_RELAY;
}
/*
 * fr_mapclass_command
 * Parse Frame Relay mapclass commands
 * This routine is driven primarily by the writeup in Q.933 particularly 
 * the section on the link core IE. The way the data is transmitted within 
 * IE during a SETUP message has contributed to the concept of a "magnitude"
 * and a "multiplier" for each parameter.
 */
void fr_mapclass_command (parseinfo *csb) 
{
    uchar               magnitude;
    uint		multiplier;
    uint		temp;

    mapclass_type *class_ptr = csb->mapclass;
    fr_params_type *fr_params = class_ptr->params;
    struct fr_svc_params *svc_params = NULL;


    if (class_ptr->encap != ET_FRAME_RELAY) {
	    printf("\n%% Can't configure Frame Relay parameters in non FR map class");
	return;
    }
    /*
     * if no parameters configured yet, initialize parameter list
     */
    if (!fr_params) {
	fr_params = malloc_named(sizeof(fr_params_type),
				 "FR SVC Map Class");
	if (!fr_params)
	    return;
	class_ptr->params = fr_params;
    }
    
    if (!fr_params->svc_params) {
	svc_params = malloc_named(sizeof(struct fr_svc_params),
				  "FR SVC Class Params");
	if (!svc_params)
	    return;
    	fr_params_init(svc_params);
	fr_params->svc_params = svc_params;
    } else {
	svc_params = fr_params->svc_params; 
    }

    /* 
     * Initialize the multiplier to be the number the user entered. 
     * If the user entered number is divisible by 10, magnitude and 
     * multiplier will get changed. If not divisible by 10, the while
     * loop will be bypassed and multiplier will still have the 
     * correct value. "multiplier" is declared as an uint so that it
     * should handle the specified range of 9600 - 45000000. 
     */
    magnitude = 0;
    multiplier = GETOBJ(int,1);
    temp = GETOBJ(int,1);
    /* 
     * If GETOBJ(int,1) is zero, we'll get stuck within the while loop.
     */
    if (GETOBJ(int,1)) {
    	while (!(temp % 10)) {
    	    magnitude++;
	    multiplier = temp/10;
	    temp = multiplier;
    	}
    }
    /* 
     * Need a little bit of input checking to make sure that the 
     * entered values will fit into the allocated number of bits.
     * The values must have trailing zeroes if large in size.
     */
    switch (csb->which) {
	case FR_MAP_CLASS_CIR:
	case FR_MAP_CLASS_MIN_CIR:
	case FR_MAP_CLASS_IN_CIR:
	case FR_MAP_CLASS_MIN_IN_CIR:
	case FR_MAP_CLASS_OUT_CIR:
	case FR_MAP_CLASS_MIN_OUT_CIR:
	    /* 
	     * Check if we need more than 11 bits to hold the value.
	     * 11 bits is the fieldsize allocated within the Link Core IE
	     * for this parameter. 
	     */
	    if (multiplier > 2048) {
		printf("\nMap Class Error: Numbers > 2048 need trailing 0's"
							",\n %d %d %d %d", 
			csb->which, GETOBJ(int,1), magnitude, multiplier);
	    	return;
	    }
	    break;
	case FR_MAP_CLASS_CBS:
	case FR_MAP_CLASS_EBS:
	case FR_MAP_CLASS_IN_CBS:
	case FR_MAP_CLASS_OUT_CBS:
	case FR_MAP_CLASS_IN_EBS:
	case FR_MAP_CLASS_OUT_EBS:
	    magnitude = 0;
	    if (GETOBJ(int,1) && GETOBJ(int,1) <= 131072) {
		multiplier = GETOBJ(int,1) >> 3;
	    } else {
		if (GETOBJ(int,1)) {
		    temp = GETOBJ(int,1) >> 3;
		    multiplier = temp;
		    while (!(temp % 10)) {
			magnitude++;
			multiplier = temp/10;
			temp = multiplier;
		    }
		}
	    }
	    /* 
	     * Check if we need more than 14 bits to hold the value.
	     * 14 bits is the fieldsize allocated within the Link Core IE
	     * for this parameter. 
	     */
	    if (multiplier > 16384) {
		printf("\nMap Class Error: Numbers > 131072 need trailing 0's,"
		       "\n %d %d %d %d", 
		       csb->which, GETOBJ(int,1), magnitude, multiplier);
	    	return;
	    }
	    break;
	default:
	    break;
	}

    switch (csb->which) {
        case FR_MAP_CLASS_CIR:
            svc_params->inc_cir =
		csb->sense ? GETOBJ(int,1) : FR_SVC_IN_CIR_DEF;
            svc_params->inc_cir_mag =
		csb->sense ? magnitude : FR_SVC_MAG_DEF;
            svc_params->inc_cir_mult =
		csb->sense ? multiplier : FR_SVC_MULT_DEF;
            svc_params->out_cir =
		csb->sense ? GETOBJ(int,1) : FR_SVC_OUT_CIR_DEF;
            svc_params->out_cir_mag =
		csb->sense ? magnitude : FR_SVC_MAG_DEF;
            svc_params->out_cir_mult =
		csb->sense ? multiplier : FR_SVC_MULT_DEF;
            break;
        case FR_MAP_CLASS_MIN_CIR:
            svc_params->min_inc_cir =
		csb->sense ? GETOBJ(int,1) : FR_SVC_MIN_IN_CIR_DEF;
            svc_params->min_inc_cir_mag =
		csb->sense ? magnitude : FR_SVC_MAG_DEF;
            svc_params->min_inc_cir_mult = csb->sense ? multiplier : FR_SVC_MULT_DEF;
            svc_params->min_out_cir =
		csb->sense ? GETOBJ(int,1) : FR_SVC_MIN_OUT_CIR_DEF;
            svc_params->min_out_cir_mag =
		csb->sense ? magnitude : FR_SVC_MAG_DEF;
            svc_params->min_out_cir_mult =
		csb->sense ? multiplier : FR_SVC_MULT_DEF;
            break;
        case FR_MAP_CLASS_CBS:
            svc_params->inc_cbs = csb->sense ? multiplier : FR_SVC_IN_CBS_DEF;
            svc_params->inc_cbs_mag = csb->sense ? magnitude : 0;
            svc_params->out_cbs = csb->sense ? multiplier : FR_SVC_OUT_CBS_DEF;
            svc_params->out_cbs_mag = csb->sense ? magnitude : 0;
            break;
        case FR_MAP_CLASS_EBS:
            svc_params->inc_ebs = csb->sense ? multiplier : FR_SVC_IN_EBS_DEF;
            svc_params->inc_ebs_mag = csb->sense ? magnitude : 0;
            svc_params->out_ebs = csb->sense ? multiplier : FR_SVC_OUT_EBS_DEF;
            svc_params->out_ebs_mag = csb->sense ? magnitude : 0;
            break;
        case FR_MAP_CLASS_FMIF:
            svc_params->inc_fmif = csb->sense ? GETOBJ(int,1) : FR_SVC_IN_FMIF;
            svc_params->inc_max_fmif =
		csb->sense ? GETOBJ(int,1) : FR_SVC_IN_FMIF;
            svc_params->out_fmif =
		csb->sense ? GETOBJ(int,1) : FR_SVC_OUT_FMIF;
            svc_params->out_max_fmif =
		csb->sense ? GETOBJ(int,1) : FR_SVC_OUT_FMIF;
            break;
	case FR_MAP_CLASS_IN_CIR:
	    svc_params->inc_cir =
		csb->sense ? GETOBJ(int,1) : FR_SVC_IN_CIR_DEF;
	    svc_params->inc_cir_mag =
		csb->sense ? magnitude : FR_SVC_MAG_DEF;
	    svc_params->inc_cir_mult =
		csb->sense ? multiplier : FR_SVC_MULT_DEF;
	    break;
	case FR_MAP_CLASS_MIN_IN_CIR:
	    svc_params->min_inc_cir =
		csb->sense ? GETOBJ(int,1) : FR_SVC_MIN_IN_CIR_DEF;
	    svc_params->min_inc_cir_mag =
		csb->sense ? magnitude : FR_SVC_MAG_DEF;
	    svc_params->min_inc_cir_mult =
		csb->sense ? multiplier : FR_SVC_MULT_DEF;
	    break;
	case FR_MAP_CLASS_OUT_CIR:
	    svc_params->out_cir =
		csb->sense ? GETOBJ(int,1) : FR_SVC_OUT_CIR_DEF;
	    svc_params->out_cir_mag =
		csb->sense ? magnitude : FR_SVC_MAG_DEF;
	    svc_params->out_cir_mult =
		csb->sense ? multiplier : FR_SVC_MULT_DEF;
	    break;
	case FR_MAP_CLASS_MIN_OUT_CIR:
	    svc_params->min_out_cir =
		csb->sense ? GETOBJ(int,1) : FR_SVC_MIN_OUT_CIR_DEF;
	    svc_params->min_out_cir_mag =
		csb->sense ? magnitude : FR_SVC_MAG_DEF;
	    svc_params->min_out_cir_mult =
		csb->sense ? multiplier : FR_SVC_MULT_DEF;
	    break;
	case FR_MAP_CLASS_IN_CBS:
	    svc_params->inc_cbs = csb->sense ? multiplier : FR_SVC_IN_CBS_DEF;
	    svc_params->inc_cbs_mag = csb->sense ? magnitude : 0;
	    break;
	case FR_MAP_CLASS_OUT_CBS:
	    svc_params->out_cbs = csb->sense ? multiplier : FR_SVC_OUT_CBS_DEF;
	    svc_params->out_cbs_mag = csb->sense ? magnitude : 0;
	    break;
	case FR_MAP_CLASS_IN_EBS:
	    svc_params->inc_ebs = csb->sense ? multiplier : FR_SVC_IN_EBS_DEF;
	    svc_params->inc_ebs_mag = csb->sense ? magnitude : 0;
	    break;
	case FR_MAP_CLASS_OUT_EBS:
	    svc_params->out_ebs = csb->sense ? multiplier : FR_SVC_OUT_EBS_DEF;
	    svc_params->out_ebs_mag = csb->sense ? magnitude : 0;
	    break;
	case FR_MAP_CLASS_IDLE_TIMER:
	    svc_params->idle_timer =
		csb->sense ? GETOBJ(int,1) : FR_SVC_IDLE_TIMER;
	    break;
	case FR_MAP_CLASS_IN_FMIF:
	    svc_params->inc_fmif = csb->sense ? GETOBJ(int,1) : FR_SVC_IN_FMIF;
	    svc_params->inc_max_fmif =
		csb->sense ? GETOBJ(int,1) : FR_SVC_IN_FMIF;
	    break;
	case FR_MAP_CLASS_OUT_FMIF:
	    svc_params->out_fmif =
		csb->sense ? GETOBJ(int,1) : FR_SVC_OUT_FMIF;
	    svc_params->out_max_fmif =
		csb->sense ? GETOBJ(int,1) : FR_SVC_OUT_FMIF;
	    break;
	case FR_MAP_CLASS_HOLDQ:
	    svc_params->holdq = csb->sense ? GETOBJ(int,1) : FR_SVC_HOLDQ_SIZE;
	    break;
    }
}

/* 
 * rem_fr_svc_from_idb_queue
 * The fr_idb maintains a list of SVC structures. Every time a
 * svc structure gets removed, we must remove it from the list.
 */
void rem_fr_svc_from_idb_queue (
	hwidbtype *hwidb,
	fr_svc_info_type *svc)
{
    fr_svc_info_type 	*tmp_svc;
    fr_idb_struct_type 	*fr_idb;
    leveltype 		level;

    fr_idb = hwidb->frame_relay_stuff;

    tmp_svc = svc;
    if (!tmp_svc)
	return;

    /* 
     * Nothing in the queue if  both head and tail are NULL.
     */
    if (!fr_idb->svc_tail && !fr_idb->svc_head) {
    	if (fr_svc_detd_debug)
	    buginf("\nSVC: IDB SVC queue empty, SVC not removed : %x", svc);
	return;
    }

    level = raise_interrupt_level(NETS_DISABLE);
    /* 
     * If svc->back, this means that this is not the first 
     * element in the queue. If !svc->back, then this is the 
     * first element in the queue. In the latter case, since the 
     * last(first) element in the queue is being removed, set the 
     * head pointer to NULL.
     */
    if (svc->back)
    	svc->back->next = svc->next;
    else {
	if (svc->next)
	    fr_idb->svc_head = svc->next;
	else
	    fr_idb->svc_head = NULL;
    }

    /* 
     * If svc->next, this means that this is not the last
     * element in the queue. If !svc->next, then this is the 
     * last element in the queue. In the latter case, since the 
     * last(first) element in the queue is being removed, set the 
     * tail pointer to NULL.
     */
    if (svc->next)
    	svc->next->back = svc->back;
    else {
	if (svc->back)
	    fr_idb->svc_tail = svc->back;
	else
	    fr_idb->svc_tail = NULL;
    }
    reset_interrupt_level(level);
}

/* 
 * add_fr_svc_to_idb_queue
 * Add a svc structure to the linked list maintained by the idb. 
 * This queue is useful for the fr_svc_timer process. 
 */
void add_fr_svc_to_idb_queue (
	hwidbtype *hwidb,
	fr_svc_info_type *svc)
{
    fr_svc_info_type 	*tmp_svc;
    fr_idb_struct_type 	*fr_idb;
    leveltype 		level;

    fr_idb = hwidb->frame_relay_stuff;

    /* 
     * If both head and tail are NULL, we have no elements in 
     * the queue. Add this svc to the queue and set the next 
     * and back pointers accordingly. 
     */
    level = raise_interrupt_level(NETS_DISABLE);
    if (!fr_idb->svc_tail && !fr_idb->svc_head) {
	fr_idb->svc_tail = svc;
	fr_idb->svc_tail->next = NULL;
	fr_idb->svc_tail->back = NULL;
	fr_idb->svc_head = svc;
	if (fr_idb->fr_svc_b2b)
	    svc->fr_svc_b2b = TRUE;
	else
	    svc->fr_svc_b2b = FALSE;
	
    	reset_interrupt_level(level);
	return;
    }

    if ((!fr_idb->svc_tail && fr_idb->svc_head) || 
			(fr_idb->svc_tail && !fr_idb->svc_head)) {
	if (fr_svc_detd_debug)
	    buginf("\nSVC: FR IDB has invalid queue pointers, aborting");
    }


    /* 
     * If the queue length is non-zero, both head and tail
     * must be non-zero. add this svc to the end of the queue
     * and adjust pointers. 
     */
    fr_idb->svc_tail->next = svc;
    tmp_svc = fr_idb->svc_tail;
    fr_idb->svc_tail = svc;
    fr_idb->svc_tail->next = NULL;
    fr_idb->svc_tail->back = tmp_svc;
    if (fr_idb->fr_svc_b2b) {
	svc->fr_svc_b2b = TRUE;
    } else {
	svc->fr_svc_b2b = FALSE;
    }
    reset_interrupt_level(level);
}

void fr_svc_periodic(hwidbtype *hwidb)
{
    fr_idb_struct_type 	*fr_idb;
    fr_pvc_info_type  	*fr_pvc;
    mapgroup_type       *group_ptr;
    staticmap_type      *mapi;
    maplist_type        *list_ptr;
    paktype		*pak;
    idbtype             *swidb;
    fr_svc_nli_type	*nli, *tmp_nli;
    int               	in, map_count;
    leveltype		level;
    

    fr_idb = hwidb->frame_relay_stuff;
    
    /* 
     * This is a case where the line went down. We started the T309
     * timer when this happened. This is not a Q.933 supported timer
     * for FR SVCs. It is a cisco specific implementation.
     * We don't want the line to flap up and down. So wait for the
     * T309 timer to expire. Default interval is 90 seconds.
     */
    map_count = 0;
    if (hwidb->state != IDBS_UP) {
	if (TIMER_RUNNING_AND_AWAKE(fr_idb->timer_309)) {
	    level = raise_interrupt_level(NETS_DISABLE);
	    for (nli = fr_idb->nli_flink; nli; ) {
		FOR_ALL_SWIDBS_ON_HW(hwidb, swidb) {
		    FOR_ALL_MAPGROUPS_ON_IDB(swidb, group_ptr) {
			list_ptr = group_ptr->maplist;
			FOR_ALL_MAPS_IN_LIST(in, list_ptr, mapi) {
			    /* 
			     * Clear all maps because the L3_CALL_REL_REQ
			     * may actually never get sent out. 
			     */
			    if (mapi && (mapi->list_ptr)) {
				if (nli->frmap[map_count]) {
				    reg_invoke_fr_addr_remove(nli->frmap[map_count]);
				}
				nli->frmap[map_count] = NULL;
				map_count++;
			    }
			}
		    }
		}
		tmp_nli = nli->flink;
		fr_pvc = fr_idb->fr_dlci_tab[nli->dlci];
		if (nli->dlci == fr_pvc->this_dlci) {
		    pak = getbuffer(sizeof(svc_release_msg));
		    if (!pak)
			return;
		    pak->datagramstart = pak->network_start;
		    pak->datagramsize = sizeof(svc_release_msg);
		    pak->if_output = fr_idb->hwidb->firstsw;
		    pak->if_input = fr_pvc->idb;
		    pak->if_output->hwptr = fr_idb->hwidb;
		    pak->linktype = LINK_ADDRESS;
		    pak->flags |= PAK_PRIORITY;
		    reg_invoke_fr_svc_l3_action(fr_idb, pak,
						nli->map, nli->call_id,
						L3_CALL_REL_REQ);
		    TIMER_STOP(fr_idb->timer_309);
		    retbuffer(pak);
		}   
		nli = tmp_nli;
	    }
	    reset_interrupt_level(level);
	}

    }
    /*
     * Tear down idle SVCs. This is a case where the line protocol is up 
     * and everything is well but no traffic is flowing over the SVC. 
     * The t_last_activity timer has expired. 
     */
    for (in = MIN_DLCI; in < MAXDLCI; in++) {
	if ((fr_pvc = fr_idb->fr_dlci_tab[in])) {
	    if (fr_pvc->pvc_type == SVC_DYNAMIC) {
		/* 
	    	 * If svc_timer is 0, tear down the svc.
	    	 */
		if ((TIMER_RUNNING_AND_AWAKE(fr_pvc->t_last_activity))) { 
	    	    TIMER_STOP(fr_pvc->t_last_activity);
		    if (fr_pvc->svc_idle_timer)
    	    		TIMER_START(fr_pvc->t_last_activity, 
				fr_pvc->svc_idle_timer);
		    level = raise_interrupt_level(NETS_DISABLE);
		    for (nli = fr_idb->nli_flink; nli; ) {
			tmp_nli = nli->flink;
		  	if (nli->dlci == fr_pvc->this_dlci) { 
			    pak = getbuffer(sizeof(svc_release_msg));
			    if (!pak)
				return;
			    pak->datagramstart = pak->network_start;
			    pak->datagramsize = sizeof(svc_release_msg);
			    pak->if_output = fr_idb->hwidb->firstsw;
			    pak->if_input = fr_pvc->idb;
			    pak->if_output->hwptr = fr_idb->hwidb;
			    pak->linktype = LINK_ADDRESS;
			    pak->flags |= PAK_PRIORITY;
	    		    if (fr_svc_detd_debug)
			    	buginf("\nsvc_periodic: Idle timer in Periodic expired");
			    reg_invoke_fr_svc_l3_action(fr_idb, pak,
				nli->map, nli->call_id, L3_CALL_REL_REQ);
			    retbuffer(pak);
			}
			nli = tmp_nli;
		    }
	    	    reset_interrupt_level(level);
		}
	    }
	}
    }
}

/* 
 * fr_svc_timer_handler
 * Main timer handler for FR SVCs. Runs constantly with frequent suspends.
 * Five timers are supported. T303, T305, T308, T310 and T322. Please see 
 * Q.931 for details of each. 
 */
forktype fr_svc_timer_handler()
{

    hwidbtype 		*hwidb;
    fr_idb_struct_type 	*fr_idb;
    fr_svc_info_type	*svc;
    uint		count;

    count = 0;
    process_watch_queue(iframeQ, ENABLE, RECURRING);
    process_watch_boolean(fr_svc_timer_bool, ENABLE, RECURRING);

    while (TRUE) {

	/* 
 	 * Once activated, run the process for 35 seconds in one second
	 * intervals. This is because the largest Q.933 timer has a 
	 * default value of 30 seconds and we need to run this process
	 * long enough to catch that. 
	 */
	if (((++count) % 35) == 0) {
 	    process_wait_for_event();
	    process_set_boolean(fr_svc_timer_bool, FALSE);
  	}

	process_sleep_for(ONESEC);

    	FOR_ALL_HWIDBS(hwidb) {
	
	if (!hwidb)
            continue;

            if (hwidb->enctype != ET_FRAME_RELAY)
                continue;

            fr_idb = hwidb->frame_relay_stuff;

            /*
             * Make sure here that SVCs are turned on.
             */
            if (!fr_idb->fr_svc_configured)
                continue;

            /*
             * Walk through the linked list of SVCs. One reason why the
             * SVCs are maintained in a linked list within the fr_idb.
             */
            for (svc = fr_idb->svc_head; svc; svc = svc->next) {

                if (!svc)
                    break;

                fr_svc_t303_expired(hwidb, fr_idb, svc);

                fr_svc_t305_expired(hwidb, fr_idb, svc);

                fr_svc_t308_expired(hwidb, fr_idb, svc);

                fr_svc_t309_expired(hwidb, fr_idb, svc);

                fr_svc_t310_expired(hwidb, fr_idb, svc);

                fr_svc_t322_expired(hwidb, fr_idb, svc);
            }
        }
    }
}
/*
 * fr_svc_t303_expired
 * T303. SETUP sent, timed out waiting for CONNECT or CALL PROCEEDING
 */
void fr_svc_t303_expired (
                hwidbtype *hwidb,
                fr_idb_struct_type *fr_idb,
                fr_svc_info_type *svc)
{
        /*
         * T303. SETUP sent, waiting for CONNECT or CALL PROCEEDING
         */
        if (TIMER_RUNNING_AND_AWAKE(svc->svc_t303) &&
                (svc->call_state == STATE_CALL_INITIATED)) {
            if (svc->t303_retry_count < T303_RETRY_COUNT) {
                svc->t303_retry_count++;
                /*
                 * SETUP needs to be retransmitted here.
                 * Retransmit must happen only for the first timeout.
                 * See page 184 of Q.931, Table 9.2
                 */
                if (svc->t303_retry_count == 1)
                    fr_svc_send_msg(hwidb, svc, MSG_SETUP,
                                svc->params.call_ref, &svc->params);
                    TIMER_STOP(svc->svc_t303);
                    TIMER_START(svc->svc_t303, T303_DEFAULT);
		    process_set_boolean(fr_svc_timer_bool, TRUE);
                    if (fr_svc_detd_debug)
                        buginf("\nSVC TIMER :%s: Restarting T303, call ref %d",
                                hwidb->hw_namestring, svc->params.call_ref);
            } else {
                if (fr_svc_detd_debug)
                    buginf("\nSVC TIMER :%s:T303 retry count exceeded, call %d",
                        hwidb->hw_namestring, svc->params.call_ref);
                    /*
                     * Table 9.2, Q.931, Retry count exceeded, send a RELEASE.
                     */
                    fr_svc_send_msg(hwidb, svc, MSG_RELEASE,
                        svc->params.call_ref, &svc->params);

                    /*
                     * No need to reset nli->state because,
                     * we'll get a RELEASE COMPLETE back which
                     * will do it for us.
                     */
                    TIMER_STOP(svc->svc_t303);
                    svc->t303_retry_count = 0;
            }
        } else if (TIMER_RUNNING_AND_AWAKE(svc->svc_t303) &&
                        (svc->call_state != STATE_CALL_INITIATED)) {
            /*
             * We are not in the STATE_CALL_INITIATED state.
             * T303 is irrelevent. Maybe it is a hanging timer.
             */
            TIMER_STOP(svc->svc_t303);
	    svc->t303_retry_count = 0;
        }
}

/*
 * fr_svc_t305_expired
 * T305. DISCONNECT sent, timed out waiting for RELEASE or DISCONNECT
 * This timer is not restarted.
 */
void fr_svc_t305_expired (
                hwidbtype *hwidb,
                fr_idb_struct_type *fr_idb,
                fr_svc_info_type *svc)
{
        /*
         * T305. DISCONNECT sent, waiting for RELEASE or DISCONNECT
         * This timer is not restarted.
         */
        if (TIMER_RUNNING_AND_AWAKE(svc->svc_t305) &&
                (svc->call_state == STATE_DISCONNECT_REQUEST)) {
            TIMER_STOP(svc->svc_t305);
            if (fr_svc_detd_debug)
                buginf("\nSVC TIMER :%s: T305 expired, call ref %d",
                        hwidb->hw_namestring, svc->params.call_ref);

            svc->call_state = STATE_RELEASE_REQUEST;
            fr_svc_send_msg(hwidb, svc, MSG_RELEASE,
            svc->params.call_ref, &svc->params);

            /*
             * No need to reset nli->state because,
             * we'll get a RELEASE COMPLETE back which
             * will do it for us.
             */
            TIMER_START(svc->svc_t308, T308_DEFAULT);
	    process_set_boolean(fr_svc_timer_bool, TRUE);
        } else if (TIMER_RUNNING_AND_AWAKE(svc->svc_t305) &&
                (svc->call_state != STATE_DISCONNECT_REQUEST)) {

            /*
             * We are not in the STATE_DISCONNECT_REQUEST state.
             * T305 is irrelevent. Maybe it is a hanging timer.
             */
            TIMER_STOP(svc->svc_t305);
        }
}
/* 
 * fr_svc_t308_expired
 * T308. RELEASE sent, timed out waiting for RELEASE COMPLETE or RELEASE 
 */
void fr_svc_t308_expired (
		hwidbtype *hwidb,
		fr_idb_struct_type *fr_idb, 
		fr_svc_info_type *svc)
{
		/* 
		 * T308. RELEASE sent, witing for RELEASE COMPLETE or RELEASE 
		 */
		if (TIMER_RUNNING_AND_AWAKE(svc->svc_t308) &&
		    	(svc->call_state == STATE_RELEASE_REQUEST)) {
		    if (svc->t308_retry_count < T308_RETRY_COUNT) {
			svc->t308_retry_count++;
			TIMER_STOP(svc->svc_t308);
		    	svc->call_state = STATE_RELEASE_REQUEST;
			fr_svc_send_msg(hwidb, svc, MSG_RELEASE, 
				svc->params.call_ref, &svc->params);
			TIMER_START(svc->svc_t308, T308_DEFAULT);
			process_set_boolean(fr_svc_timer_bool, TRUE);
			if (fr_svc_detd_debug) {
			    buginf("\nSVC TIMER :%s: Restarting T308, call %d", 
					hwidb->hw_namestring, svc->params.call_ref);
			}
		    } else {
			if (fr_svc_detd_debug)
			    buginf("\nSVC TIMER :%s:T308 retry count exceeded, call%d", 
					hwidb->hw_namestring, svc->params.call_ref);
			/* 
	  	 * Page 184, Q.931, Table 9.2
			 */
		fr_svc_send_msg(hwidb, svc, MSG_RELEASE, 
					svc->params.call_ref, &svc->params);
			/* 
			 * No need to reset nli->state because, 
			 * we'll get a RELEASE COMPLETE back which 
			 * will do it for us. 
			 */
			TIMER_STOP(svc->svc_t308);
		    	svc->t308_retry_count = 0;
		    }
		} else if (TIMER_RUNNING_AND_AWAKE(svc->svc_t308) &&
				(svc->call_state != STATE_RELEASE_REQUEST)) {
			/* 
			 * We are not in the STATE_RELEASE_REQUEST state.
			 * T308 is irrelevent. Maybe it is a hanging timer.
			 */
			TIMER_STOP(svc->svc_t308);
		}
}
		/* 
 * fr_svc_t309_expired
 * T309: Link layer is down. Let idle timer or periodic timer deal with it. 
 * Do nothing for here. Can populate later if need arises.
 */
void fr_svc_t309_expired (
		hwidbtype *hwidb,
		fr_idb_struct_type *fr_idb, 
		fr_svc_info_type *svc)
{
		/* 
		 * Page 184, Q.931, Table 9.2
		 * T309. Data Link layer is down. Calls in stable states are not 
		 * lost. Timer is not restarted. Kinda complicated and confusing
		 * to implement now. I'll revist after beta tests. shankar 7/95.
		 * No need to change anything now. SVCs idle timer works well. 
		 * so don't need anything here. shankar 10/95
		 */
		if (TIMER_RUNNING_AND_AWAKE(svc->svc_t309)) {
		    /* 
 		     * Do nothing now. SVC timer process will deal with it.
		     */
			;
		}
}
/* 
 * fr_svc_t310_expired
 * T310. CALL PROCEEDING sent, timed out wait for a CONNECT. 
 */
void fr_svc_t310_expired (
		hwidbtype *hwidb,
		fr_idb_struct_type *fr_idb, 
		fr_svc_info_type *svc)
{
		/* 
		 * T310. CALL PROCEEDING sent, wait for a CONNECT. 
		 */
		if (TIMER_RUNNING_AND_AWAKE(svc->svc_t310) &&
				(svc->call_state == STATE_CALL_PROCEEDING)) {
		    if (fr_svc_detd_debug)
			buginf("\nSVC TIMER :%s: T310 expired, call reference %d", 
					hwidb->hw_namestring, svc->params.call_ref);
		    svc->call_state = STATE_DISCONNECT_REQUEST;
	    	    fr_svc_send_msg(hwidb, svc, MSG_DISCONNECT, 
					svc->params.call_ref, &svc->params);
		    TIMER_START(svc->svc_t305, T305_DEFAULT);
		    process_set_boolean(fr_svc_timer_bool, TRUE);
		    TIMER_STOP(svc->svc_t310);
		}
}
/* 
 * fr_svc_t322_expired
 * T322. STATUS ENQ sent, Expect to receive a STATUS. 
 */
void fr_svc_t322_expired (
		hwidbtype *hwidb,
		fr_idb_struct_type *fr_idb, 
		fr_svc_info_type *svc)
{
		/* 
		 * T322. STATUS ENQ sent, Expect to receive a STATUS. 
		 */
		if (TIMER_RUNNING_AND_AWAKE(svc->svc_t322)) {
		    if (svc->t322_retry_count < T322_RETRY_COUNT) {
			svc->t322_retry_count++;
			TIMER_STOP(svc->svc_t322);
			TIMER_START(svc->svc_t322, T322_DEFAULT);
			process_set_boolean(fr_svc_timer_bool, TRUE);
			if (fr_svc_detd_debug) {
			    buginf("\nSVC TIMER :%s: Restarting T322, call reference %d", 
					hwidb->hw_namestring, svc->params.call_ref);
			}
		    } else {
			if (fr_svc_detd_debug) {
			    buginf("\nSVC TIMER :%s:T322 retry count exceeded, call%d", 
					hwidb->hw_namestring, svc->params.call_ref);
			}
			/* 
			 * STATUS ENQ may be retransmitted several times. 
		 * Page 184, Q.931, Table 9.2
			 */
		fr_svc_send_msg(hwidb, svc, MSG_STATUS_ENQUIRY, 
					svc->params.call_ref, &svc->params);
		    	TIMER_STOP(svc->svc_t322);
		        svc->t322_retry_count = 0;
		    }
		}
	    }


/* 
 * fr_assign_svc_mem
 * routine that will allocate memory for the svc structure. 
 * The function of this routine is to allocate svc memory, 
 * initialize the various fields. 
 */
fr_svc_info_type *fr_assign_svc_mem(void)
{
    fr_svc_info_type 	*svc;

    svc = malloc_named(sizeof(fr_svc_info_type), "FR SVC Assign");

    if (!svc) {
	return(NULL);
    }
    memset((uchar *)svc, 0, sizeof(fr_svc_info_type));
    return(svc);
}
/* 
 * fr_svc_l2_to_l3
 * Message coming in from layer 2.
 * Standard interface between layer 3 and layer 2. 
 */
void fr_svc_l2_to_l3 (
	uchar event,
	fr_idb_struct_type *fr_idb, 
	paktype *pak)
{
    switch (event) {
	case FR_DL_EST_IND:
	    /* 
	     * Nothing needs to be done. This is a normal situation.
	     * Wait till we see a FR_DL_EST_CNF before setting the 
	     * fr_idb->lapfup flag and triggering SVCs.
	     */
	    if (pak)
	    	datagram_done(pak);
	    break;
	case FR_DL_EST_CNF:
	    /* 
 	     * Decided not to do anything here. Layer 3 will
	     * need no coaxing from layer 2 during setup. Only 
	     * during teardown. 
	     */
	    if (pak)
	    	datagram_done(pak);
	    break;
	case FR_DL_REL_IND:
	    /* 
	     * Need to start tearing down all existing SVCs. 
	     * Free NLI, SVC etc. Only on this interface, however.
	     */
	    fr_svc_teardown_calls(fr_idb);
	    if (pak)
	    	datagram_done(pak);
	    break;
	case FR_DL_REL_CNF: 
	    /* 
	     * Not going to do anything. We'll assume that FR_DL_REL_IND
	     * preceeded this and that all SVCs have been cleared. 
	     */
	    if (pak)
	    	datagram_done(pak);
	    break;
	case FR_DL_DATA_IND:
	    /* 
 	     * Incoming I frames 
	     */
	    process_enqueue_pak(iframeQ, pak);
	    break;
	default:
	    if (fr_svc_detd_debug)
		buginf("\nL2 to L3 COMMUNIQUE : Illegal message %d", event);
	    if (pak)
	    	datagram_done(pak);
	    break;
    }
}

/* 
 * fr_svc_l3_to_l2
 * Message being sent to layer2.
 * Standard interface between later2 and layer3.
 */
void fr_svc_l3_to_l2 (
	uchar event,
	fr_idb_struct_type *fr_idb, 
	paktype *pak)
{
    switch (event) {
	case FR_DL_EST_REQ: 
	    /* 
 	     * Layer 3 telling layer 2 to get cranking.
	     */
	    reg_invoke_fr_svc_lapf_l3_to_l2(FR_DL_EST_REQ, fr_idb, pak);
	    break;
	case FR_DL_REL_REQ:
	    /* 
	     * Layer 3 telling layer 2 to release data link 
	     */
	    reg_invoke_fr_svc_lapf_l3_to_l2(FR_DL_REL_REQ, fr_idb, pak);
	    break;
	case FR_DL_DATA_REQ:
	    /* 
	     * Outgoing I frames
	     */
	    reg_invoke_fr_svc_lapf_l3_to_l2(FR_DL_DATA_REQ, fr_idb, pak);
	    break;
	default:
	    if (fr_svc_detd_debug)
		buginf("\nL3 to L2 COMMUNIQUE : Illegal message %d", event);
	    break;
    }
}

/* 
 * frame_relay_svc_startup
 * Startup routine for FR SVC initialization. 
 * The SVC subsystem has been separated from the frame-relay subsystem. 
 */
void frame_relay_svc_startup (subsystype *subsys)
{
    fr_svc_parser_init();
    reg_add_fr_svc_teardown(fr_svc_teardown_processes, 
					"fr_svc_teardown_processes");
    reg_add_fr_svc_teardown_calls(fr_svc_teardown_calls, 
					"fr_svc_teardown_calls");
    reg_add_fr_mapclass_nvgen(fr_svc_mapclass_nvgen,"fr_svc_mapclass_nvgen");
    reg_add_fr_mapclass_remove(fr_svc_mapclass_remove,
			       "fr_svc_mapclass_remove");
    lapf_init();
}

/*
 * fr_svc_teardown_calls
 * A generic routine to teardown all calls. This will be invoked if the
 * user turns off SVCs, if layer 2 goes down, etc. etc.
 */
void fr_svc_teardown_calls (fr_idb_struct_type *fr_idb)
{
    fr_svc_nli_type	*nli, *tmp_nli;
    int                 call_id;
    staticmap_type      *map, *tmp_map;
    maplist_type 	*list;
    int			i;
    boolean		map_found;
    paktype 		*pak;
    idbtype 		*idb;

    nli = fr_idb->nli_flink;
    map_found = FALSE;
    idb = NULL;
    pak = NULL;

    if (!nli)
	return;

    for (nli = fr_idb->nli_flink; nli; ) {
	call_id = nli->call_id;
	tmp_nli = nli->flink;
	map = nli->map;
	/* 
	 * Make sure that the map still exists. Someone may have done 
	 * a "no map-list <name> prior to "no frame svc" in which case,
	 * the maps will cease to exist but nli->map will not have been
	 * updated. 
	 */
	 FOR_ALL_MAPLISTS(list) {
	     FOR_ALL_MAPS_IN_LIST(i, list, tmp_map) {
		if (tmp_map == map) {
		    map_found = TRUE;
		    idb = tmp_map->sm_swidb;
		    break;
		}
	     }
	     if (map_found == TRUE)
		break;
	 }
	/*
	 * Make sure that we don't try to remove something before
	 * it has been set up. The nli->map gets setup only when
	 * a call is placed.
	 */
	if (!map || !map_found) {
	    /* 
	     * the map has been cleared out from under us. 
	     * possibly by a "no map-list" command. Do not 
	     * proceed. The nli structure will be cleared 
	     * by other mechanisms.
	     */
	    nli->map = NULL; 
	}

	/* 
	 * idb may be NULL because we may not have a match when we 
	 * did a map search. 
	 */
	if (idb && fr_l3_debug)
	    buginf("\nSVC CALL TEARDOWN: Tearing down call ID #%d idb %s", 
						call_id, idb->namestring);

	if (map_found) {
	    if (idb)
	    	pak = getbuffer(sizeof(svc_setup_msg));
	    if (pak && idb) {
	    	pak->datagramstart = pak->network_start;
	    	pak->datagramsize = sizeof(svc_setup_msg);
	    	pak->if_input = idb;
	    	pak->if_output = idb;
	    	pak->linktype = LINK_ADDRESS;
	    	pak->flags |= PAK_PRIORITY;
	    	reg_invoke_fr_svc_l3_action(fr_idb, pak, map, call_id, 
						L3_CALL_REL_REQ);
	    }
	    if (pak)
	    	retbuffer(pak);
	}

	nli = tmp_nli;
	idb = NULL; 
	pak = NULL;
	map_found = FALSE;
    }
}
/*
 * fr_svc_teardown_processes
 * A generic routine to teardown support processes for SVCs.
 */
void fr_svc_teardown_processes (boolean yes)
{
    int                 pid = 0;
    paktype 		*pak;

    if (yes) {
	process_watch_queue(iframeQ, DISABLE, RECURRING);
	if (ie_parse_process_pid != NO_PROCESS) {
	    pid = ie_parse_process_pid;
	    ie_parse_process_pid = NO_PROCESS;
	    while ((pak = process_dequeue(iframeQ)) != NULL)
		retbuffer(pak);
	    delete_watched_queue(&iframeQ);
	    process_kill(pid);
	}
	if (svc_timer_process_pid != NO_PROCESS) {
	    pid = svc_timer_process_pid;
	    svc_timer_process_pid = NO_PROCESS;
	    process_kill(pid);
	}
    }
}

/*
 * fr_svc_enable_cmd
 * - to handle "frame-relay svc" command
 */
void fr_svc_enable_cmd (parseinfo *csb)
{
    idbtype *swidb;
    hwidbtype *idb;
    fr_idb_struct_type *fr_idb;

    if (!csb || !(swidb = csb->interface) || !(idb = swidb->hwptr) ||
	!(fr_idb = idb->frame_relay_stuff))
	return;
    /*
     * some common FR command validation 
     */
    if (idb->enctype != ET_FRAME_RELAY) {
	if (!csb->nvgen) {
	    printf("\n%%%s :Frame Relay not configured on interface", 
		   idb->hw_namestring);
	}
	return;
    }
    if (reg_invoke_dialer_huntgroup_member(idb)) {
	if (!csb->nvgen) {
	    printf("\nCannot change frame relay configuration of hunt group"
		   " member");
	}
	return;
    }

    if (csb->nvgen) {
	switch (csb->which) {
	case FRAME_RELAY_SVC_ENABLE:
	    nv_write(fr_idb->fr_svc_configured, "%s", csb->nv_command);
	    nv_add(fr_idb->fr_svc_b2b, " %s", "back-to-back");
	    break;
	default:
	    break;
	}
	return;
    }

    switch (csb->which) {
    case FRAME_RELAY_SVC_ENABLE:
	/* 
	 * SVCs will get initialized when the user 
	 * configures svcs on an interface
	 */
	fr_svc_init();
	if ((GETOBJ(int,1)) != FRAME_RELAY_SVC_B2B_ENABLE) {
	    fr_idb->fr_svc_b2b = FALSE;
	} else  {
	    fr_idb->fr_svc_b2b = TRUE;
 	}

	if ((csb->sense) && !fr_idb->fr_svc_configured) {
	    /*
	     * Tell Layer 2 to establish itself.
	     * fr_idb->svc_enabled will be set to TRUE
	     * when layer 2 replies.
	     */
	    if (GETOBJ(int,1))
 	    	fr_idb->fr_svc_b2b = TRUE;
	    fr_idb->fr_svc_enabled = TRUE;
	    fr_idb->fr_svc_configured = TRUE;
	    TIMER_START(fr_idb->timer_309, T309_DEFAULT);
	    reg_invoke_fr_svc_lapf_l3_to_l2(FR_DL_EST_REQ, fr_idb, NULL);
	} else if ((!csb->sense) && (fr_idb->fr_svc_configured)) {
	    fr_idb->fr_svc_b2b = FALSE;
	    fr_svc_teardown_calls(fr_idb);
	    reg_invoke_fr_svc_lapf_l3_to_l2(FR_DL_REL_REQ, fr_idb, NULL);
	    fr_idb->fr_svc_enabled = FALSE;
	    fr_idb->fr_svc_configured = FALSE;
	    TIMER_STOP(fr_idb->timer_309);
	}
	break;
    }
}


/*
 * FR subsystem header
 */

#define FR_SVC_MAJVERSION 1
#define FR_SVC_MINVERSION 0
#define FR_SVC_EDITVERSION  1

SUBSYS_HEADER(frame_relay_svc, FR_SVC_MAJVERSION, FR_SVC_MINVERSION, 
		FR_SVC_EDITVERSION, frame_relay_svc_startup, 
		SUBSYS_CLASS_PROTOCOL, NULL, NULL);

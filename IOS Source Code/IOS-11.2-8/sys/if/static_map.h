/* $Id: static_map.h,v 3.4.4.8 1996/08/15 19:31:22 fowler Exp $
 * $Source: /release/112/cvs/Xsys/if/static_map.h,v $
 *------------------------------------------------------------------
 * static_map.h -- Definitions for generic static maps.
 *
 * October 1993, Lionel Bustini
 *
 * Copyright (c) 1993-1996,1997 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: static_map.h,v $
 * Revision 3.4.4.8  1996/08/15  19:31:22  fowler
 * CSCdi60278:  Removing frame-relay traffic shaping from an int causes
 *              traceback
 *              Combine some VC queuing and traffic shaping parameters
 * Branch: California_branch
 *
 * Revision 3.4.4.7  1996/06/25  14:10:30  shankar
 * CSCdi61103:  Broadcast triggering of SVCs needs a configuration hook
 * for users
 * Branch: California_branch
 *
 * Revision 3.4.4.6  1996/05/24  21:15:54  ccpoe
 * CSCdi58460:  map-class frame-relay traffic-shaping features dont work
 * on FR SVC
 * Branch: California_branch
 *
 * Revision 3.4.4.5  1996/05/17  11:23:41  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.3.40.3  1996/05/05  23:10:16  ppearce
 * Sync to IbuMod_Calif_baseline_960504
 *
 * Revision 3.3.40.2  1996/04/03  14:39:06  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.3.40.1  1996/03/17  17:51:34  ppearce
 * Sync IbuMod_Calif_branch to V111_1_3
 *
 * Revision 3.4.4.4  1996/05/15  05:40:29  fowler
 * Frame relay VC queuing and rate enforcement
 * Branch: California_branch
 *
 * Revision 3.4.4.3  1996/04/25  23:13:44  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.4.4.2  1996/04/08  20:51:53  ronnie
 * CSCdi51093:  no atm arp-server command causes crash
 * Branch: California_branch
 *
 * Revision 3.4.4.1  1996/03/18  20:14:49  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.20.2  1996/03/16  06:56:08  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.3.20.1  1996/01/24  22:19:11  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.4  1996/03/09  22:55:27  widmer
 * CSCdi47890:  HTTP server doesnt work
 * Add HTTP server
 *
 * Revision 3.3  1995/11/22  20:45:07  ronnie
 * CSCdi44212:  Many fixes done at Telecom Finland
 *
 * Revision 3.2  1995/11/17  09:31:11  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:52:45  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.6  1995/11/08  21:03:31  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.5  1995/09/30  05:03:31  dblair
 * CSCdi41333:  DDR Callback initial commit
 * Adding Callback feature for DDR interfaces.
 *
 * Revision 2.4  1995/09/28  20:50:41  ronnie
 * CSCdi40510:  Too many ATM SVCs set up to same destination
 *
 * Revision 2.3  1995/08/18  14:23:44  schrupp
 * CSCdi38181:  Add ATMSIG throttling and SSCOP changes
 *
 * Revision 2.2  1995/08/08  16:47:55  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi38391]
 *
 * Revision 2.1  1995/06/07 20:55:02  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#ifndef _STATIC_MAP_INCLUDE_
#define _STATIC_MAP_INCLUDE_

#include "address.h"

#define STATIC_MAP_HASH	64
#define MAPLIST_MAX_NAME_SIZE 32

enum STATIC_MAP_TYPE {
    MAP_STATIC, 
    MAP_DYNAMIC 
};

/* link and party states */
enum BC_LINKSTATE {
  LINK_NULL,
  LINK_OPENING,
  LINK_FAILED,
  LINK_ACTIVE
};

enum BC_PARTYSTATE {
  PARTY_NULL,
  PARTY_OPENING,
  PARTY_ACTIVE
};

typedef struct atm_map_type_ {
    /* fields for this svc_map */
    struct staticmap_type_ *next_svc;  /* pointer to next svc static map */
    hwaddrtype bc_hwaddr;	       /* dynamically resolved hw addr MTP */
    enum BC_LINKSTATE map_linkstate;   /* state of this link, MTP */
    enum BC_PARTYSTATE map_partystate; /* state of this party, MTP */
    sys_timestamp map_timestamp;       /* timestamp for open */

    /* fields for svc_map chain--only used by first svc map entry */
    vc_info_t *bc_vc;                  /* vc for link */
    enum BC_LINKSTATE bc_linkstate;    /* state of link, MTP */
    boolean all_maps_active;           /* flag for all parties good */
    void* bc_call_id;                  /* call_id of this MTP call, addparty*/
    sys_timestamp bc_timestamp;        /* timestamp for open */
} atm_map_type;

/* Frame-relay SVC stuff */
struct fr_svc_addr_type_ {
    uchar       addr[20];                       /* address in string form */
    uchar       num_len;                        /* support variable lengths */
    uchar       num_type;                       /* National/International */
    uchar       num_plan;                       /* E164 or X121 */
    uchar       subaddr;                        /* Subaddress */
};

typedef struct svc_addr_block_ {
    fr_svc_addr_type           saddr;          /* Local X121/E164 address */
    fr_svc_addr_type           daddr;          /* Destination X121/E164 address */
    fr_svc_addr_type           caddr;          /* Connected X121/E164 address */
} svc_addr_block;

struct fr_svc_params {
	svc_addr_block		svc_addr;	/* all relevant addresses */

	uint 			call_ref;	/* Call Reference */
	ushort  		inc_fmif;	/* Incoming max FMIF size */
	ushort  		out_fmif;	/* Outgoing max FMIF size */
	ushort  		inc_max_fmif;	/* Incoming max FMIF size */
	ushort  		out_max_fmif;	/* Outgoing max FMIF size */
	uint 			inc_cir;	/* In CIR for this Group */
	ushort			inc_cir_mag;	/* Incoming CIR for call */
	ushort			filler1;	/* aligning byte */
	uint			inc_cir_mult;  	/* multiplier for incoming CIR */
	uint			min_inc_cir;	/* min Incoming CIR for call */
	ushort  		min_inc_cir_mag;/* min Incoming CIR mag for call */
	ushort			filler2;	/* aligning byte */
	uint			min_inc_cir_mult;/* mult for min incoming CIR */
	uint			out_cir;	/* Out CIR for this Group */
	ushort			out_cir_mag;	/* Out magnitude for this Group */
	ushort			filler3;	/* aligning byte */
	uint			out_cir_mult;	/* Out multiplier for this Group */
	uint			min_out_cir;	/* min outgoing CIR for call */
	ushort  		min_out_cir_mag;/* min outgoing CIR mag for call */
	ushort			filler4;	/* aligning byte */
	uint			min_out_cir_mult;/* mult for min outgoing CIR */
	uint 			inc_cbs;	/* In Committed Burst Size */
	uint 			out_cbs;	/* Out Committed Burst Size */
	uint 			inc_ebs;	/* In Excess Burst Size */
	uint 			out_ebs;	/* Out Excess Burst Size */

	uchar 			inc_cbs_mag;	/* See FRFTC95.029 */
	uchar 			out_cbs_mag;	/* See FRFTC95.029 */
	uchar 			inc_ebs_mag;	/* See FRFTC95.029 */
	uchar 			out_ebs_mag;	/* See FRFTC95.029 */

	int     		cause;		/* Cause of last error */

	ushort 			dlci;		/* dlci for the call */
	ushort			req_svc_dlci;	/* requested dlci for call */
	uchar			req_svc_type;	/* PREFERRED or EXCLUSIVE */

	uchar 			msg_type;	/* Message type being processed */

	uint			idle_timer;	/* idle timer for pvc */
	ushort			holdq;		/* size of the holdq */

	boolean 		broadcast;	/* Will this VC carry broadcasts */
	int			encapsulation;	/* Is it IETF or cisco */
};


/*
 * VC Queueing params for mapclass
 */
struct fr_vcq_params {
    ulong map_vcq_cnt;
    struct VCQ *map_vcq;	/* holder of a VCQ before assigning to a VC */
    uint	cir;	
    uint 	bc;
    uint 	be;
    ulong fr_ave_rate;
    ulong fr_peak_rate;
    ulong fr_tval;
    boolean fr_becn_response;
};

struct fr_params_type_ {
    struct fr_vcq_params  *vcq_params;
    struct fr_svc_params  *svc_params;
};

struct mapclass_type_ {
    struct mapclass_type_ *next;    /* pointer to next map class in queue */
    uint ref_count;                 /* number of refs to this class */
    boolean visible;                /* visible/deleted flag */
    char *name;                     /* map class name */
    uint encap;                     /* encapsulation type */
    void *params;             	    /* class parameters */
};

struct staticmap_type_ {
    struct staticmap_type_ *next;   /* pointer to next static map */
    struct staticmap_type_ *next_hw;/* pointer to next static map */
    struct staticmap_type_ *next_bc;/* pointer to next broadcast static map */
    addrtype addr;	    	    /* protocol address */
    hwaddrtype hwconf;	    	    /* configured hw address */
    hwaddrtype hwaddr;	    	    /* dynamically resolved hw add. (SVC) */
    uint protocol;		    /* protocol */
    uint encap;			    /* encapsulation */
    sys_timestamp update_time;	    /* Time last updated */
    enum STATIC_MAP_TYPE map_type;  /* either STATIC or DYNAMIC */
    void *params;	    	    /* encapsulation parameters */
    mapclass_type *class;	    /* pointer to class */
    idbtype *sm_swidb;                
    atm_map_type *atm_map;          /* atm specific struct */
    boolean conn_tag;               /* used for throttling */

    /* 
     * Frame-relay SVC specific fields. 
     */
    boolean   		broadcast;	/* carry broadcasts or not */
    boolean		broadcast_trigger; 	/* trigger SVCs on broadcast? */
    uint      		encap_flavor;	/* encap flavor, IETF or CISCO */
    fr_svc_nli_type	*nli;	    	/* Pointer to an NLI structure */
    ushort    		priority;	/* Priority for dlci */
    struct maplist_type_ *list_ptr;	/* Need a backward pointer */
};

struct maplist_type_ {
    struct maplist_type_ *next;	    /* pointer to next map list in queue */
    uint ref_count;		    /* number of refs to this list */
    boolean visible;		    /* visible/deleted flag */
    char *name;			    /* map list name */
    staticmap_type *hash[STATIC_MAP_HASH];
    staticmap_type *hwhash[STATIC_MAP_HASH];
    svc_addr_block 	addr;	    /* X121/E164 addressaes */	
    uchar 	priority_grp;	    /* priority group */
    mapclass_type *class;	    /* pointer to class */
};

struct mapgroup_type_ {
    struct mapgroup_type_ *next;    /* pointer to next map link */
    struct maplist_type_ *maplist;  /* pointer to map list */
};

extern queuetype *maplistQ;
extern queuetype *mapclassQ;

extern parser_mode *map_class_mode;
extern parser_mode *map_list_mode;

#define FOR_ALL_MAPS_IN_LIST(i, list, map) \
    for ((i) = 0; (i) < STATIC_MAP_HASH; (i)++) \
	for ((map) = (list)->hash[(i)]; (map); (map) = (map)->next)

#define FOR_ALL_MAPLISTS(list) \
    for ((list) = (maplist_type *)maplistQ->qhead; (list);(list) = (list)->next)

#define FOR_ALL_MAPGROUPS_ON_IDB(idb, group) \
    for ((group) = (idb)->mapgroup; (group); (group) = (group)->next)

#define FOR_ALL_MAPCLASS(cls) \
    for ((cls) = (mapclass_type *)mapclassQ->qhead; (cls); (cls) = (cls)->next)


extern void mapclass_command(parseinfo *);
extern void maplist_command(parseinfo *);
extern void mapgroup_command(parseinfo *);
extern void static_map_parser_init(void);
extern void staticmap_delete(maplist_type *, staticmap_type *);
extern staticmap_type *staticmap_enter(maplist_type *, addrtype *, 
				       hwaddrtype *, uint, uint, 
				       enum STATIC_MAP_TYPE, 
				       char *, void *);
extern void staticmap_update(maplist_type *, staticmap_type *, hwaddrtype *,
			     char *);
extern staticmap_type *staticmap_broadcast(idbtype *, uint, uint);
extern staticmap_type *staticmap_lookup(idbtype *, addrtype *, 
					uint, uint, boolean);
extern staticmap_type *staticmap_lookup_list(maplist_type *, addrtype *, 
					     uint, uint);
extern staticmap_type *staticmap_lookup_prefix(maplist_type *, addrtype *, 
					       uint, uint, ushort);
extern staticmap_type *staticmap_hwlookup(idbtype *, hwaddrtype *, uint,
					  boolean, link_t);
extern staticmap_type *staticmap_hwlookup_list(maplist_type *, hwaddrtype *,
					       uint, boolean, link_t);
extern maplist_type *maplist_lookup(char *name);
extern maplist_type *maplist_create(char *name, boolean visibile);
extern void mapgroup_add(idbtype *, mapgroup_type *);
extern void staticmap_replace_hw(maplist_type *, staticmap_type *,
				 hwaddrtype *);
extern void mapgroup_create(uchar *, uchar *, idbtype *);
extern mapgroup_type *mapgroup_lookup(idbtype *, char *);
extern void mapgroup_delete(idbtype *, mapgroup_type *);
extern mapclass_type *mapclass_lookup(char *, uint);
extern mapclass_type *mapclass_create(char *, uint, boolean);
#endif _STATIC_MAP_INCLUDE_


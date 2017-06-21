/* $Id: dialer_private.h,v 3.1.16.11 1996/09/10 07:32:41 dblair Exp $
 * $Source: /release/112/cvs/Xsys/wan/dialer_private.h,v $
 *------------------------------------------------------------------
 * Dialer private definitions.
 *
 * February 1996, Lionel Bustini
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: dialer_private.h,v $
 * Revision 3.1.16.11  1996/09/10  07:32:41  dblair
 * CSCdi62263:  Dialer profile with Multilink not working
 * Branch: California_branch
 * Fix botched comment during previous commit of CSCdi62263
 *
 * Revision 3.1.16.10  1996/09/10  07:17:39  dblair
 * CSCdi62263:  Dialer profile with Multilink not working
 * Branch: California_branch
 * Fix multilink and dialer profiles
 *
 * Revision 3.1.16.9  1996/09/10  02:43:19  suresh
 * CSCdi68129:  dialer pool-member 1 min-link 1 doesnt reserve the link
 * Branch: California_branch
 * Completely rewritten code for sharing of ISDN interfaces
 *
 * Revision 3.1.16.8  1996/08/27  16:31:08  wmay
 * CSCdi67232:  dialer callback pending checks not efficient
 * Branch: California_branch
 *
 * Revision 3.1.16.7  1996/08/13  20:02:51  wmay
 * CSCdi65253:  crash in dialer_holdq_output: dpak structure is reused for
 * broadcasts - make sure that we clear out the holdq field of that
 * structure after each call to reg_invoke_dialer_pending
 * Branch: California_branch
 *
 * Revision 3.1.16.6  1996/07/23  22:04:21  suresh
 * CSCdi62055:  Dialer profile error message misleading
 * Branch: California_branch
 * Code Cleanup for change from dialer user to dialer profile.
 *
 * Revision 3.1.16.5  1996/07/19  19:50:36  wmay
 * CSCdi60728:  ISDN dialer holdQ not handled correctly - do not allocate
 * holdq until needed, and pass the holdq correctly as calls and callbacks
 * are made.  Redo a little callback code to deal with the holdq better and
 * rename some of the structures.  Add a debug dialer holdq (hidden).
 * Branch: California_branch
 *
 * Revision 3.1.16.4  1996/07/19  01:17:43  suresh
 * CSCdi63165:  Code Cleanup: dialer pool
 * Branch: California_branch
 * Cleaned up the dialer pool code.
 *
 * Revision 3.1.16.3  1996/06/16  21:20:56  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 3.1.16.2  1996/05/10  01:44:44  wmay
 * CPP and PPP half-bridge commit
 * Branch: California_branch
 *
 * Revision 3.1.16.1  1996/04/27  06:36:16  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 3.1.2.4  1996/04/23  00:24:23  suresh
 * Branch: Dial1_branch
 * Add Dialer Profiles support to Dial1_branch
 *
 * Revision 3.1.2.3  1996/04/17  00:00:22  syiu
 * Branch: Dial1_branch
 * Sync Dial1_branch to V111_1_3
 *
 * Revision 3.1.2.2  1996/03/08  17:30:31  dblair
 * Branch: Dial1_branch
 * Change for multilink virtual bundle interfaces to support ISDN BRI,
 * PRI, and Dialer interfaces
 *
 * Revision 3.1.2.1  1996/02/23  21:31:25  lbustini
 * Branch: Dial1_branch
 * Move dialer fields out of the idb.
 *
 * Revision 3.1  1996/02/23  19:29:49  lbustini
 * Placeholders for Dial1_branch.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __DIALER_PRIVATE_H__
#define __DIALER_PRIVATE_H__

#define DIALERHASH 64
#define DIALER_ODDPARITY_MAXCHAR 128

#define DIALER_IDLE_TIME	120 * ONESEC
#define DIALER_FAST_IDLE_TIME	20 * ONESEC
#define DIALER_ENABLE_TIME	15 * ONESEC
#define DIALER_CARRIER_TIME	30 * ONESEC
#define DIALER_HOLDQ_TIME	45 * ONESEC /* holdq timeout = 45 + enable time*/
#define DIALER_LOAD_TIME	2 * ONESEC
#define DIALER_DEBOUNCE_TIME	30 * ONESEC
#define DIALER_DEFAULT_PULSEINTERVAL 1 * ONESEC

#define DIALER_MAXLIST		11	/* DIALLST_MAXLIST + 1 */
#define DIALER_MAX_CALLER       25	/* max characters in caller id entry */
#define DIALER_MAX_NUM_HOLDQ    100

#define DIALER_CRN_LEN		4	/* V.25bis prefix length */


/*
 * structure to store numbers for caller id screening.
 */
struct dialer_call_tab_ {
    struct dialer_call_tab_ *next;
    struct dialer_call_tab_ *prev;
    char dialer_call_string[DIALER_MAX_CALLER+1];  /* add one for NULL term. */
};


/*
 * Structure of dialer map for multiple addresses/phone numbers per interface.
 * Split to accomodate multiple phone numbers per address: 
 *   dialermaptype   : common data
 *   dialerentrytype : number-specific data
 */
struct dialermaptype_ {
    struct dialermaptype_ *next;
    struct dialerentrytype_ *entry;	/* list of phone numbers */
    struct dialerentrytype_ *cip;	/* call in progress */
    dialerdbtype *ddb;
    addrtype proto_addr;
    uint link;
    boolean dynamic;			/* True if dynamically created */
};

typedef struct dialerentrytype_ {
    struct dialerentrytype_ *next;
    struct dialermaptype_ *head;
    char *dialer_string;
    char *hostname;
    char *class;
    uint speed;                 /* for ISDN, 64K or 56K */
    dialer_stat stat;
    char *modem_chat_regexp;    /* Chat script regular expression for modem */
    regexp *modem_chat_regcomp; /* Compiled modem regular expression */
    char *system_chat_regexp;  /* Chat script regular expression for system */
    regexp *system_chat_regcomp; /* Compiled chat script regexp */
    boolean broadcast;          /* true if broadcast set */
    boolean spc;                /* true if semi permanent call link */
} dialerentrytype;

#define DIALER_HOLD_PAK(pak)	((pak->dialer_flags & PAK_DDR_NO_HOLDQ) == 0)
#define DIALER_NEW_ADDR_LINKTYPE(pak)	(pak->linktype == LINK_CLNS)

/*
 * Structure for connected members of a rotary group.
 */
struct dialergrouptype_ {
    struct dialergrouptype_ *next;
    struct dialergrouptype_ *prev;
    dialerdbtype *huntddb;	/* pointer to rotary group master ddb */
    dialerdbtype *firstddb;	/* first interface in connected group */
    cpp_group_type *cgroup;     /* Bundle specific stuff for cpp */ 
    sys_timestamp debounce_timer; /* timer run after line is taken down */
    sys_timestamp activity_timer; /* grace period before disconnec. call(s) */
    sys_timestamp call_timer;	/* timer started when placing a call */
    ulong nmember;		/* number of members in this group */
    char number_called[MAXSTR];	/* phone number group is connected to */
    char remote_name[MAXSTR];	/* remote name group is connected to */
};
 
/*
 * Structure to store relevant information when a packet is being processed.
 */
struct dialerpaktype_ {
    dialerdbtype *ddb;                /* original ddb */
    paktype *pak;		/* packet to process */
    uint linktype;		/* packet link type */
    long address;		/* next hop address */
    addrtype *proto_addr;	/* protocol adress */
    dialermaptype *map;		/* map used for dialing */
    boolean interesting;	/* packet classification */
    char *header;		/* packet source and destination addresses */
    char *dial_number;		/* number to dial when placing a call */
    dialer_stat *stats;		/* statistics for call */
    dialer_holdq_type *holdQ;	/* dialer hold queue */
};

/*
 * Head of dialer pool list. 
 */
typedef struct dialerpoolheadtype_ {
    struct dialerpoolheadtype_ *next_head;
    dialerpooltype *first_profile_pool;
    dialerpooltype *first_member_pool;
    ushort number_of_profile_pools;
    ushort number_of_member_pools;
    uchar pool_number;
} dialerpoolheadtype;

/*
 * Structure associated to a dialer pool member or profile
 */
struct dialerpooltype_ {
    struct dialerpooltype_ *next_in_interface; /* next pool in interface */
    struct dialerpooltype_ *next_in_head;      /* next pool in head */
    dialerpoolheadtype *pool_head;
    dialerdbtype *ddb;
    uchar pool_number;
    uchar pool_priority;
    short max_link;
    short min_link;
    short dial_out;
    short dial_in;
};

/*
 * Types of dialer lists.
 */
enum DIALERLIST {
    DIALLST_NONE,		/* no dialer-list x protocol p */
    DIALLST_DENY,		/* dialer-list x protocol p deny */
    DIALLST_PERMIT,		/* dialer-list x protocol p permit */
    DIALLST_PROLIST,		/* dialer-list x protocol p list l */
    DIALLST_LIST		/* dialer-list x list l */
};

/*
 * An item in a dialer list.
 */
union dllist {
    int number;		/* Non-CLNS list number */
    char *name;		/* CLNS filter name */
};

typedef struct dlisttype_ {
    struct dlisttype_ *next;
    union dllist testlist;
    uchar linktype;
    uchar list_type;
} dlisttype;

struct dialerlisttype_ {
    dialerplisttype plist;	/* Fast dialer lists */
    dlisttype *dlist;		/* Regular dialer lists */
};

struct dialer_holdq_type_ {
    queuetype queue;
    sys_timestamp holdQ_timer;
};

/*
 * routines to handle the timers for the dialer_holdq_type above.
 * These can be replaced with mgd_timers pretty easilly just by
 * replacing these routines.
 */
static inline void init_dialer_holdq_timer (dialer_holdq_type *tmp)
{
    TIMER_STOP(tmp->holdQ_timer);
}

static inline void start_dialer_holdq_timer (dialer_holdq_type *tmp,
					     ulong ticks)
{
    TIMER_START(tmp->holdQ_timer, ticks);
}
				 
static inline void stop_dialer_holdq_timer (dialer_holdq_type *tmp)
{
    TIMER_STOP(tmp->holdQ_timer);
}

static inline boolean dialer_holdq_timer_running (dialer_holdq_type *tmp)
{
    return(TIMER_RUNNING(tmp->holdQ_timer));
}

static inline
boolean dialer_holdq_timer_running_and_awake (dialer_holdq_type *tmp)
{
    return(TIMER_RUNNING_AND_AWAKE(tmp->holdQ_timer));
}

/*
 * dialer callback  structure
 *
 * callback_timer is used to time when to do the callback when we are the
 * SERVER, and timing how long to wait on CLIENT side.
 */
typedef struct callback_type {
    dqueue_t	dqueue;			/* link to global list */
    dialer_holdq_type *holdQ;		/* ptr to holdq for this callback */
    enum CALLBACK_TYPE callback_type;	/* Server or client */
    sys_timestamp callback_timer; 	/* callback timer */
    dialermaptype * pdialermap;		/* dialer map for callback server */
    hwidbtype *initial_idb;		/* idb call came in on */
    char number_called[MAXSTR];		/* number called */
    char remote_name[MAXSTR];		/* remote username */
} callback_type;

static inline void start_callback_timer (callback_type *call, ulong ticks)
{
    TIMER_START(call->callback_timer, ticks);
}

static inline void stop_callback_timer (callback_type *call)
{
    TIMER_STOP(call->callback_timer);
}

/*
 * Map-class parameters structure.
 */
typedef struct dialer_params_type_ {
    boolean callback_server_user;
    boolean callback_server_dial;
    boolean voice_call;
    boolean do_nsf;	   /* outgoing nsf support */
    ushort  nsf_type;	   /* outgoing nsf value */
    ulong idle_ticks;      /* default idle ticks */
    ulong fast_idle_ticks; /* default fast ticks */
    ulong enable_ticks;    /* default enable ticks */
    ulong carrier_ticks;   /* default carrier ticks */
    uint speed;            /* ISDN: 64Kbs or 56Kbs */
    boolean spc;           /* true if semi permanent call link */
} dialer_params_type;

/*
 * A few useful macros.
 */
#define GET_NETDDB(ddb)		((ddb)->netcfg_ddb)
#define V25BIS_DIALER(ddb)	((ddb)->dialer == DIALER_INBAND_SYNC)
#define DTR_DIALER(ddb)		((ddb)->dialer == DIALER_DTR_SYNC)
#define ASYNC_DIALER(ddb)	((ddb)->dialer == DIALER_INBAND_ASYNC)
#define ISDN_DIALER(ddb)	((ddb)->dialer == DIALER_ISDN)
#define IS_PROFILE(ddb)         ((ddb)->dialer == DIALER_PROFILE)
#define IS_SPOOFING_INT(ddb)	((ddb)->dialer_spoofing_int)
#define IS_SPOOFING_PROT(ddb)	((ddb)->dialer_spoofing_prot)
#define IS_SPOOFING(ddb)	(IS_SPOOFING_INT(ddb) || IS_SPOOFING_PROT(ddb))

#define DIALER_HOLDQ_EMPTY(q) (((q) == NULL) || QUEUEEMPTY(q))
#define IS_CALL_ALLOWED(ddb)  ((ddb)->dialer_num_call < \
			       (ddb)->dialer_max_call)
#define CONNECTED_GROUP(ddb)  ((ddb)->conn_group != NULL)
#define SLAVE_DIALER(ddb)     (HUNTGROUP_MEMBER(ddb) || DIALPOOL_MEMBER(ddb))

static inline void start_idle_timer (dialerdbtype *ddb)
{
    TIMER_START(ddb->dialer_idle_timer, ddb->dialer_idle_ticks);
    TIMER_STOP(ddb->dialer_fast_idle_timer);
}

#define START_IDLE_TIMER(ddb) start_idle_timer(ddb)

static inline void set_dialer_state (dialerdbtype *ddb,
				     enum DIALER_STATE state)
{
    ddb->dialer_state = state;
}

/*
 * Macro to traverse all rotary group members under a hunt master IDB
 * Lite version.
 */ 
#define FOR_ALL_HUNTDDBS_ON_MASTER(huntddb, ddb)	\
    for ((ddb) = (huntddb)->hunt_group_first;		\
	 (ddb);						\
	 (ddb) = (ddb)->hunt_group_next)

/*
 * Macro to traverse all groups of connected interfaces in a rotary group.
 */
#define FOR_ALL_CONNECTEDGROUPS_ON_MASTER(ddb, group)	\
    for ((group) = (ddb)->conn_group;		 	\
	 (group);					\
	 (group) = (group)->next)

/*
 * Macro to traverse all members of a connected group.
 */
#define FOR_ALL_MEMBERS_IN_GROUP(group, i, ddb)		\
    for ((i) = 0, (ddb) = (group)->firstddb;		\
	 ((i) < (group)->nmember) && (ddb);		\
	 (i)++, (ddb) = (ddb)->next_conn)

/*
 * Macro to traverse all members of a dialer pool.
 */
#define FOR_ALL_MEMBERS_IN_POOL(head, pool)		\
    for ((pool) = (head)->first_member_pool;		\
	 (pool);					\
	 (pool) = (pool)->next_in_head)

/*
 * Macro to traverse all profiles of a dialer pool.
 */
#define FOR_ALL_PROFILES_IN_POOL(head, pool)		\
    for ((pool) = (head)->first_profile_pool;		\
	 (pool);					\
	 (pool) = (pool)->next_in_head)

/*
 * Macro to traverse all pools an interface is a member of.
 */
#define FOR_ALL_POOLS_IN_INTERFACE(ddb, pool)		\
    for ((pool) = (ddb)->dialer_pool;			\
	 (pool);					\
	 (pool) = (pool)->next_in_interface)

#define FOR_ALL_HEAD_OF_POOLS(head)                     \
    for ((head) = dialer_pool_head;			\
	 (head);					\
	 (head) = (head)->next_head)

/*
 * Macro to fastswitch over DDR output interfaces.
 */
#define DDR_FASTSWITCH(IN, OUT, CACHE, SET_NEXT, LIST, CLASSIFY, OFFSET,\
		       DIALER_CONNGROUP, RESUME)			\
    dialerdbtype *ddb;							\
    long dialerlist;							\
									\
    ddb = get_dialerdb(OUT);						\
    dialerlist = ddb->dialer_list;					\
    /*									\
     * Dialer rotary groups pick the output interface here.		\
     */									\
    if (ddb->hunt_group_lead) {						\
	if (!dialerlist) {						\
	    /*								\
	     * Passive rotary groups not supported.			\
	     */								\
	    IN->mci_checked = TRUE;					\
	    return(FALSE);						\
	}								\
	ddb = set_output_dialer_ddb(DIALER_CONNGROUP);	                \
	OUT = ddb->ownidb;						\
    }									\
									\
    /*									\
     * Set proper (actual) values for the second round.			\
     */									\
    SET_NEXT(OUT, ddb);							\
									\
    /*									\
     * Classify packet and possibly reset the idle timer.		\
     */									\
    if (dialerlist) {							\
	dialerlist = ddb->netcfg_ddb->dialer_plist->plist.LIST;		\
	if (!dialerlist) {      /* not interesting */			\
	    goto RESUME;						\
	} else if (dialerlist > 0) {					\
	    CLASSIFY(OFFSET, RESUME);					\
	}								\
    }									\
    TIMER_START(ddb->dialer_idle_timer,	ddb->dialer_idle_ticks);	\
    TIMER_STOP(ddb->dialer_fast_idle_timer);				\
    goto RESUME;

/*
 * set_output_dialer_idb
 * Return output idb, given a rotary group.  Used when fastswitching.
 */
static inline dialerdbtype *set_output_dialer_ddb (dialergrouptype *grptr)
{
    dialerdbtype *ddb;

    ddb = grptr->firstddb;
    grptr->firstddb = ddb->next_conn;
    return(ddb);
}

/*
 * dialer.c
 */
extern dialerentrytype *dialer_name_lookup(dialerdbtype *, char *);
extern boolean dial_if(dialerdbtype *, dialerpaktype *);
extern boolean dialer_place_call(dialerdbtype *, dialerpaktype *);
extern void dialer_raw_enqueue(hwidbtype *, paktype *);
extern void dialer_disconnect_group(dialerdbtype *);
extern void dialer_dtr_drop(hwidbtype *);
extern void dialer_call_disconnected(dialerdbtype *, enum DIALER_STATE);
extern void dialer_out_call_connected(dialerdbtype *);
extern void dialer_in_call_connected(dialerdbtype *);
extern void dialer_cannot_dial(dialerdbtype *, dialerpaktype *);
extern void dialer_fail(dialerdbtype *);
extern boolean dialer_calling_num(char *, dialerdbtype *);
extern boolean dialer_check_number(dialerdbtype *, const char *);
extern boolean dialer_vencapsulate(paktype *, long);
extern void dialer_reset(hwidbtype *);
extern dialermaptype *dialer_addr_lookup(dialerdbtype *, addrtype *, uint);
extern dialerentrytype *dialer_string_lookup(dialermaptype *, char *);
extern void dialer_goingdown(hwidbtype *);
extern dialerdbtype *dialer_pending(dialerdbtype *, dialerpaktype *);
extern void start_fast_idle(dialerdbtype *);
extern void dialer_get_packet_header(paktype *, char *);
extern dialerdbtype *pick_best_dialer_idb(dialerdbtype *, dialerdbtype *, 
					  ulong *, sys_timestamp *);
extern void dialer_set_call_data(dialerdbtype *, dialerpaktype *);
extern void dialer_create_dynamic_map(uint, hwidbtype *, addrtype *);
extern void dialer_delete_dynamic_map(uint, hwidbtype *, addrtype *);

extern dialer_holdq_type *create_dialer_holdq(dialerdbtype *);
extern void dialer_holdq_timer_start(dialer_holdq_type *, ulong);
extern void dialer_holdq_free(dialer_holdq_type **);
extern void dialer_holdq_discard_queue(dialer_holdq_type **, char *);
extern void dialer_enqueue_pak(dialer_holdq_type *, paktype *, long);
/*
 * dialer_holdq_enqueue
 * Enqueue packet into the dialer hold queue.
 */
static inline void dialer_holdq_enqueue (dialerpaktype *dpak)
{
    
    if (dpak->holdQ == NULL) {
	return;
    }
    if (DIALER_HOLD_PAK(dpak->pak))
	dialer_enqueue_pak(dpak->holdQ, dpak->pak, dpak->address);
    /*
     * Clear the holdq from the dpak - we've enqueued the packet, and
     * we might re-use the dpak structure again.
     */
    dpak->holdQ = NULL;
}

/*
 * dialer_increment_call
 * Increment number of call on the physical interface all the way to the
 * rotary group leader.
 */
static inline void dialer_increment_call (dialerdbtype *ddb)
{
    dialerdbtype *tempddb;

    for (tempddb = ddb; tempddb != NULL; tempddb = tempddb->hunt_group_master) {
    /*
     * This limit check shouldn't be necessary, but due to the
     * complex interactions of virtual, dialer, and real serial interfaces
     * for the same call, some limit checking is necessary to keep
     * this number in bounds.
     */
        if (tempddb->dialer_num_call < tempddb->dialer_num_link)
            tempddb->dialer_num_call++;
    }
}

/*
 * dialer_decrement_call
 * Decrement number of call on the physical interface all the way to the
 * rotary group leader.
 */
static inline void dialer_decrement_call (dialerdbtype *ddb)
{
    dialerdbtype *tempddb;

    for (tempddb = ddb; tempddb != NULL; tempddb = tempddb->hunt_group_master) {
    /*
     * This limit check shouldn't be necessary, but due to
     * complex interactions of virtual, dialer, and real serial interfaces
     * for the same call, some limit checking is necessary to keep
     * this number in bounds.
     */
        if (tempddb->dialer_num_call > 0)
            tempddb->dialer_num_call--;
    }
}

/*
 * dialer_parser.c
 */
extern ushort get_dialer_minsum(dialerdbtype *);
extern ushort get_dialer_used(dialerdbtype *);
extern void dialer_deleteidb(idbtype *);
extern boolean dialer_parser_createidb(uint64 *, parseinfo *); 
extern dialerdbtype *init_dialer_idb(hwidbtype *);
extern void reset_dialer_fields(dialerdbtype *);
extern void add_to_hunt_group(dialerdbtype *, int, boolean, hwidbtype *);
extern void copy_hunt_fields(dialerdbtype *, dialerdbtype *);
extern void copy_priority_fields(dialerdbtype *, dialerdbtype *);
extern void copy_dialer_fields(dialerdbtype *, dialerdbtype *);
extern void propagate_hunt_changes(hwidbtype *);
extern void dialer_set_member_encap(parseinfo *, hwidbtype *, encapstype);
extern void dialer_set_leader_encap(parseinfo *, hwidbtype *, encapstype);
extern void dialer_set_number_called(dialerdbtype *, char *);
extern void dialer_adjust_encap_vectors(hwidbtype *, int);
extern void dialer_member_adjust_encap_vectors(dialerdbtype *, boolean);
extern void dialer_clear_map(dialerdbtype *);
extern void dialer_print_proto_addr(idbtype *, addrtype *, uint, char *);
extern void dialer_clear_group(dialerdbtype *, dialermaptype *);
extern void dialer_mapclass_nvgen(mapclass_type *);
extern void dialer_parser_caller(parseinfo *, dialerdbtype *);
extern void dialer_pri_shutdown(hwidbtype *);
extern void nv_dialer_map(dialerdbtype *);
extern void default_set_dialer(dialerdbtype *, boolean);
extern void dialer_retrieve_maps(hwidbtype *, ulong);
extern void show_dialer_info(dialerdbtype *);
extern void show_dialer_state(dialerdbtype *);
extern void show_dialer_stats(dialerdbtype *);
extern boolean dialer_interface_command_ok(parseinfo *);
extern boolean dtr_dialer(hwidbtype *);
extern void dialer_command_inband(boolean, dialerdbtype *, int );
extern void dialer_remove(hwidbtype *);
extern boolean dialer_add_pool(dialerdbtype *, int, int, int, int);
extern void dialer_remove_pool(dialerdbtype *, uchar);

/*
 * dialer_profile.c
 */
extern dialerpooltype *get_dialer_pool(dialerdbtype *, uchar);
extern dialerdbtype *get_free_dialer_in_pool(dialerdbtype *);
extern dialerdbtype *dialer_fsm_idle(dialerdbtype *, dialerpaktype *);
extern dialerdbtype *dialer_fsm_pending(dialerdbtype *, dialerpaktype *);
extern dialerdbtype *dialer_fsm_up(dialerdbtype *, dialerpaktype *);
extern dialerdbtype *dialer_find_profile_by_name(char *);
extern void dialer_call_failed(dialerdbtype *, dialerdbtype *);
extern void dialer_bind_profile(dialerdbtype *, dialerdbtype *);
extern void dialer_unbind_profile(dialerdbtype *);
extern void dialer_bind_settings(dialerdbtype *, dialerdbtype *);
extern boolean dialer_match_pool(dialerdbtype *);
extern boolean dialer_match_pool_or_caller_id(dialerdbtype *, char *);
extern boolean dialer_may_match_name(hwidbtype *);
extern boolean dialer_match_name(dialerdbtype *, char *);
extern boolean is_profile(hwidbtype *);
extern boolean dialpool_member(hwidbtype *);
extern boolean dialer_match_bound_ddb(dialerdbtype *, dialerdbtype *);

/*
 * dialer_chain.c
 */
extern void dialer_parser_init(void);

/*
 * dialer_callback.c
 */
callback_type *callback_pending(dialerpaktype *);
callback_type *find_callback_by_dial_string(char *);
callback_type *find_callback_by_call(dialerdbtype *);
callback_type *find_callback_by_idb(hwidbtype *);
callback_type *create_callback(dialer_holdq_type *, enum CALLBACK_TYPE,
				char *, hwidbtype *, char *, dialermaptype *);
void remove_callback(callback_type *);

void callback_dial(callback_type *);
boolean callback_start(hwidbtype *, usernametype *, boolean, char *);
void callback_system_init(void);
void callback_onesec(void);
#endif __DIALER_PRIVATE_H__

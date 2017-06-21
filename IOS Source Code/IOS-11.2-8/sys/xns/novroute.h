/* $Id: novroute.h,v 3.4.6.3 1996/05/03 02:20:36 sluong Exp $
 * $Source: /release/112/cvs/Xsys/xns/novroute.h,v $
 *------------------------------------------------------------------
 * novroute.h -- Novell routing definitions
 *
 * November 10, 1992 - Joanne Boyle
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: novroute.h,v $
 * Revision 3.4.6.3  1996/05/03  02:20:36  sluong
 * Branch: California_branch
 * IPX ACL Violation Logging, Plaining English Filters, Display SAP by
 * name,
 * NLSP MIBS, Scaleable RIP/SAP, Modularity, and SAP query by name.
 *
 * Revision 3.4.6.2  1996/04/09  22:33:31  john
 * CSCdi52606:  IPX static routes can cause error reading from NETWORK on
 *              client. Allow configurable metrics.
 * Branch: California_branch
 *
 * Revision 3.4.6.1  1996/04/02  05:38:35  dkatz
 * CSCdi36031:  Running out of memory running 2 or more EIGRP ASs
 * Branch: California_branch
 * EIGRP Jumbo Patch.
 *
 * Revision 3.4  1996/03/01  07:34:53  mschaef
 * CSCdi50473:  NLSP route aggregation conformance (phase 1)
 *
 * Fix assorted nits and bugs
 *
 * Revision 3.3  1995/11/20  17:55:44  sluong
 * CSCdi43294:  filtering saps between protocols are not feasible
 *
 * Revision 3.2  1995/11/17  19:24:38  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  14:00:06  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.5  1995/11/08  21:39:55  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.4  1995/09/23  21:43:28  dkatz
 * CSCdi38306:  ISIS shouldnt run SPF on leaf route changes
 * Generalize NLSP's interface to the ISIS LSP structure so code and
 * structures can be shared with ISIS-IP.
 *
 * Revision 2.3  1995/08/25  14:12:04  hampton
 * Minor Novell IPX Cleanups.  Remove some unused variables and routines.
 * Eliminate direct references to the system clock.  [CSCdi39328]
 *
 * Revision 2.2  1995/07/01 03:39:31  hampton
 * Minor Novell IPX Cleanups.  Remove some unused variables and routines.
 * Eliminate direct references to the system clock.  [CSCdi36668]
 *
 * Revision 2.1  1995/06/07 23:29:56  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __NOVROUTE_H__
#define __NOVROUTE_H__

#include "../iprouting/route.h"
#include "../util/radix.h"

#define NOV_CONNECTED_DISTANCE   0
#define NOV_STATIC_DISTANCE      1
#define NOV_IGRP_DISTANCE      100
#define NOV_RIP_DISTANCE       120
#define NOV_NETMASK_ALL        0xffffffff
#define IPXRADIXDATAOFF        8                    /* 8 bits before data */
#define IPXRADIXKEYLEN         (sizeof(ulong)+1)    /* length + address */

union novrdbunion {
    struct ripigrp_rdbtype {
        ulong metric;                   /* metric for this particular route */
        ulong delay;                    /* composite delay */
        ulong bandwidth;                /* composite bandwidth */
        ushort mtu;                     /* composite MTU */
        uchar load;                     /* composite loading */
        uchar reliability;              /* composite reliability */
        uchar hopcount;                 /* hop count */
    } ripigrp_info;
    struct nlsp_rdbtype {
	nov_cost_type cost;             /* cost summary for order of pref */
    } nlsp_info;
};


struct novrdbtype_ {
    /* RIP stuff gleened from old pathentry type */
        struct novrdbtype_ *next;       /* next ptr */
	isis_lsp_linkage lsp_linkage;	/* LSP linkage */
    	struct novndbtype_ *ndb;
	ulong procmask;			/* routing process for this path */
        ulong uses;                     /* uses */
        sys_timestamp timer;            /* per path expiry timer */
        nidbtype *nidb;                 /* incoming (virtual) interface */
	boolean snapshot_no_age;  	/* T if no age since snapshot idb */
        ulong srcnet;                   /* src net */
        uchar srchost[IEEEBYTES];       /* src host */
        uchar robin_reset;              /* reset value for round robin use */
        uchar robin;                    /* current round robin value */
	union novrdbunion protocol_info; /* Protocol specific info */
};

typedef struct novlsptype_ {
    struct novlsptype_ *next;       /* next ptr */
    idbtype *idb;                   /* lsp interface */
} novlsptype;

/*
 * Temporary tracking structure for display routines
 */
typedef struct rdb_list_ {
    struct rdb_list_ *next;
    novrdbtype *rdb;
} nov_rdb_list;

/*
 * These options flags are defined outside of the normal 'flags' word,
 * because these are protocols, just various 'parameters'
 */

union novndbunion {
    struct ripigrp_ndbtype {
        ulong tag;              /* arbitrary tag */
    } ripigrp_info;
    struct nlsp_ndbtype {
    	struct novndbtype_ *spf_next;  /* temp next ptr for spf use */
	boolean spf_pessimal;
    } nlsp_info;
};
/* Don't NVGEN this static route... */
#define IPX_NDB_OPTION_STATIC_NO_NVGEN (0x01)
#define IPX_NDB_OPTION_VALID (0x02)
#define IPX_NDB_OPTION_DELETED (0x04)
#define IPX_NDB_OPTION_ALWAYS_LOOKUP_BACKUP (0x08)
#define IPX_NDB_OPTION_INTERFACE_STATIC (0x10)

/*
 * Differentiate real ndb's from placeholder
 * ndb's (inserted by update thread):
 */

enum IPX_NDB_TYPE {
    IPX_NDB_RESERVED = 0,
    IPX_NDB_ACTIVE = 1,
    IPX_NDB_PLACEHOLDER = 2};

/* 
 * DON'T move anything between ext_delay and rt_mask. There are byte alignment
 * contrains which is glued to the generic radix processing code.
 */
typedef struct novndbtype_ {
	  /* RIP stuff from old ripentrytype */
    	struct novndbtype_ *next;  /* next ptr */
	struct novndbtype_ *next_newest; /* Next in 'newest' chain */
	struct novndbtype_ *prev_newest; /* Previous in 'newest' chain */
	struct novndbtype_ *next_atlr; /* Next 'lost route' to advertise */
	struct novndbtype_ *prev_atlr;
    	struct novndbtype_ *temp_next;  /* temp next ptr for route display */
	backuptype *backup_info; /* Backup routes information */
    	novrdbtype *nextpath;   /* next path to use */
    	novrdbtype *path_list;  /* path descriptors */
    	novlsptype *lsp_list;   /* lsp descriptors (aggregated routes only) */
 	enum IPX_NDB_TYPE type; /* active ndb or update placeholder */

	/* 
	 * DON'T move anything between ext_delay and rt_mask. There are 
	 * byte alignment contrains which is glued to the generic radix 
	 * processing code.
	 */
	ushort ext_delay;
	uchar options;		/* OPTIONS flags, as defined above */
	uchar keylen;           /* radix key length */
    	ulong dstnet;           /* dest. net */
    	ushort delay;           /* Units set by upper protocol... */
    	uchar hopcount;        	/* number of hops */
	uchar masklen;          /* mask length */
	ulong rt_mask;          /* network mask */
	/* 
	 * DON'T move anything between ext_delay and rt_mask. There are 
	 * byte alignment contrains which is glued to the generic radix 
	 * processing code.
	 */
	sys_timestamp atlr_time;/* When to advertise to Lost Routers */
    	sys_timestamp ndb_changed; /* time last changed; flash upd supt */
    	ulong flags;            /* type flags */
    	sys_timestamp holddownuntil;    /* hold down timer */
	ulong ext_hop;		/* external hop count */
	ulong igrp_delay;	/* same as *best* rdb->delay */
        ulong path_metric;      /* path metric */
        ulong procmask;         /* process mask */
	ushort last_delay;      /* Last delay used (valid after poison) */
	ushort reverse_poisoned;
    	uchar paths;            /* number of paths to this dest. */
        uchar novpdbindex;      /* index of owning novell pdb */
    	uchar area_count;      	/* number of hops */
	uchar last_hopcount;    /* previous hop count */
	union novndbunion protocol_info;
} novndbtype;

static inline int ipx_count_route_paths (novndbtype *ndb) {
    int i;
    novrdbtype *rdb;
    for (i = 0, rdb = ndb->path_list; rdb != NULL; rdb = rdb->next, i++)
      ;
    return(i);
}

static inline int ipx_count_server_paths (novell_sapentrytype *sap)
{
    int i;
    novell_sapentry_rdb *srdb;
    for (i = 0, srdb = sap->paths; srdb != NULL; srdb = srdb->next, i++)
      ;
    return(i);
}

typedef novndbtype *nov_routetable[NOV_NETHASHLEN];
typedef novndbtype *nov_statictable[NOV_NETHASHLEN];

/*
 * Backup service record entry to support both delayed SPF service
 * processing as ell as NLSP/ISIS potential pseudonode:
 */
typedef struct nov_svc_type_ {
    struct nov_svc_type_ *next;
    sys_timestamp update_timestamp;
    novpdbtype *pdb;
    nidbtype *svc_nidb;
    ushort svc_type;
    ulong svc_snet;
    uchar svc_shost[IEEEBYTES];
    ushort svc_sock;
    ushort svc_hops;
    ushort svc_ext_hops;
    ulong svc_flags;
    int svc_name_len;
    isis_lsp_linkage **lsp_lh;
    ushort lsp_version;
    ushort lsp_index;
    nov_cost_type cost;
    /* %%% FIX THIS ... DUPLICATION OF DATA %%% */
    novell_serveridtype sid;
} nov_svc_type;

typedef nov_svc_type *nov_backup_saptable[NOV_NETHASHLEN];

/*
 * Unreachable route descriptor: map unreachable SAPs to
 * exprected future routes (NLSP only).
 */
typedef struct nov_urd_type_ {
    struct nov_urd_type_ *next;   /* Link to next element */
    ulong dstnet;                 /* Network number */
    backuptype *backup_info;      /* Unreachable SAP info */
} nov_urd_type;

typedef nov_urd_type *nov_urd_table_type[NOV_NETHASHLEN];

struct novpdbtype_ {
    struct novpdbtype_ *next;	/* next */
    char *name;                 /* pointer to protocol name string */
    char *pname;                /* pointer to process name string */
    boolean running;            /* TRUE if process already running */
    int pid;                    /* process ID of running process */
    int index;                  /* routing process index (0 - MAXROUTERS) */
    ulong mask;                 /* routing process mask */
    ulong proctype;             /* protocol type flag, e.g. NET_IGRP */
    ulong distflag;             /* type flags for sending routing updates */
    ulong sap_distflag;         /* type flags for sending service updates */
    queuetype workQ;		/* Redist, lost, and validate queue */
    boolean debug_set;          /* Debug status */
    ulong debug_net;            /* Target network, if any */
    queuetype summaryQ;         /* Configured aggregated address */

    ushort route_type;
    ulong redistallowed;
    void (*redist_callback)(struct novpdbtype_ *, novndbtype *, int);  
    void (*sap_redist_callback)
	(struct novpdbtype_ *, novell_sapentrytype *, int);  
				/* pointer to redistributing process routine */
    void (*add_route)(struct  novpdbtype_ *, ulong, ulong, ulong, ulong);
    void (*add_server)(struct novpdbtype_ *, ulong, ulong);
    void (*rdb_changed_callback)(struct novpdbtype_ *, struct novndbtype_ *,
				 struct novrdbtype_ *, enum IPX_ROUTE_EVENT,
				 ulong);
    void (*srdb_changed_callback)(struct novpdbtype_ *,
				  struct sapentry_ *,
				  novell_sapentry_rdb *,
				  enum IPX_ROUTE_EVENT, ulong);
    void (*lost_route)(struct novpdbtype_ *, novndbtype *);
    void (*redistribute)(struct novpdbtype_ *, struct novpdbtype_ *, boolean); 
				/* pointer to start redistribute routine */
    boolean malloc_backup;   /* Used with add_route, want malloced backups */
    /*
     * Determine if incoming RIP route is best XROUTE in NLSP/ISIS area
     */
    boolean (*xroute_callback)(struct novpdbtype_ *,
			       novndbtype *,
			       ushort, ushort);
    /*
     * Determine if incoming SAP service is best instance in NLSP/ISIS area
     */
    boolean (*xsap_callback)(struct novpdbtype_ *,
			     novell_sapentrytype *,
			     ushort);
    /*
     * Search the NLSP backup table for a backup (RIP) route
     * (similar to add_route)
     */
    boolean (*inject_backup_route_callback)(ulong net, ulong netmask);

    /*
     * Possibly store a (RIP) backup route in the NLSP backup table.
     * Called when a better NLSP or XROUTE already exists.
     */
    void (*backup_route_callback)(struct novpdbtype_ *, nidbtype *,
				  ulong, ulong, ushort, ulong,
				  uchar *, ushort, ushort, novpdbtype *pdb);
    /*
     * Inject all backup routes when we become designated router. The
     * backup route table represents part of our potential pseudonode.
     */
    void (*designated_router_callback)(boolean, novpdbtype *);

    forktype (*process)(struct novpdbtype_ *); 
			        /* pointer to routing process */
    void (*restart)(struct novpdbtype_ *);
			        /* routine to start/restart routing process */
    void (*cleanup)(struct novpdbtype_ *);
			        /* routine to flush routing process */

    queuetype inputq;           /* pointer to input queue */
    int changed;                /* state change flag */
    short sumlist[MAX_NOVPROCS];/* summary lists for redistribution */
    short ifltr_all;            /* access list for all incoming information */
    short ofltr_all;            /* access list for all outgoing information */
    short ofltr_proto[MAX_NOVPROCS]; /* list for filtering redisted protocols */
    short sap_ifltr_all;        /* access list for all incoming information */
    short sap_ofltr_all;        /* access list for all outgoing information */
    short sap_ofltr_proto[MAX_NOVPROCS]; /* list for filtering redisted protocols */
    boolean (*holddown)(struct novpdbtype_ *, novndbtype *, novrdbtype *,ulong);
    boolean holdokay;           /* TRUE if holddowns permitted */
    int distance;               /* distance for this protocol */
    int def_distance;           /* default value */
    int distance2;              /* distance 2 for this protocol */
    int def_distance2;          /* default value */
    int distance3;              /* distance 3 for this protocol */
    int def_distance3;          /* default value */
    boolean defaultmetricflag;  /* TRUE if default metric is set */
    ulong default_metric;       /* default metric for readvertised routes */
    ulong broadcasttime;        /* broadcast interval in ticks */
    ulong def_broadcasttime;    /* default value */
    ulong invalidtime;          /* invalid after this many ticks */
    ulong def_invalidtime;      /* default value */
    ulong holdtime;             /* hold-down time in ticks */
    ulong def_holdtime;         /* default value */
    ulong flushtime;            /* flush entry after this many ticks */
    ulong def_flushtime;        /* default value */
    int netcount;               /* count of networks we're routing on */
    ulong networks[MAXNETWORKS];/* the table of network numbers */
    ulong arg1;
    boolean all_nets;		/* running protocol on all ipx networks */
    boolean default_include_net; /* TRUE if a net is included by default  */
    boolean entered;		/* TRUE if user entered command to create
				   this process */

    /*
     * Metric modification fields
     */
    ulong mmetric_in;
    ulong mmetric_out;
    short mmetric_list_in;
    short mmetric_list_out;
    ulong exteriors_out;
    ulong exteriors_in;


    /*
     * igrp2 specific fields
     */
    ushort asystem;             /* our AS number, if any */
    ddbtype *ddb;		/* Pointer to dual descriptor block */
    watched_queue *watched_inputq; /* Special watched input queue */
    watched_queue *watched_workQ; /* Special watched work queue */
    ushort igrp_hopcount;       /* hopcount of network_update() caller */
    ulong igrp_maxhops;         /* maximum believable hop count */
    ulong igrp_def_maxhops;     /* default maximum hop count */
    ulong igrp_variance;        /* amount of acceptable variance */
    ulong redist_hops;
    ulong redist_delay;
    boolean shutdown;           /* Force calling dual_linkdown */

    /*
     * For each TOS specified, there needs to be different K factors.
     * For now we are assuming a single TOS.
     */
    ushort igrp_k1;             /* IGRP metric factor 1 */
    ushort igrp_k1_def;         /* IGRP metric factor 1, default */
    ushort igrp_k2;             /* IGRP metric factor 2 */
    ushort igrp_k2_def;         /* IGRP metric factor 2, default */
    ushort igrp_k3;             /* IGRP metric factor 3 */
    ushort igrp_k3_def;         /* IGRP metric factor 3, default */
    ushort igrp_k4;             /* IGRP metric factor 4 */
    ushort igrp_k4_def;         /* IGRP metric factor 4, default */
    ushort igrp_k5;             /* IGRP metric factor 5 */
    ushort igrp_k5_def;         /* IGRP metric factor 5, default */

    ulong *mib_table;           /* pointer to some of the mib values */

    /*
     * NLSP specific fields:
     */

    clns_pdbtype *osi_pdb;          /* pdb running integrated isis */
};

/*
 * Backup route table entry to support NLSP/ISIS potential pseudonode:
 */
typedef struct novroutetype_ {
    struct novroutetype_ *next;	     /* Link to next element */
    sys_timestamp update_timestamp;  /* When entry created or refreshed */
    novpdbtype *pdb;                 /* pdb of source protocol */
    nidbtype *nidb;                  /* Network (circuit) descriptor */
    ulong dstnet;                    /* Network number */
    ulong rt_mask;                   /* Network mask */
    ushort metric;                   /* Metric */
    ulong snet;                      /* Next hop network address */
    uchar shost[IEEEBYTES];          /* Next hop node address */
    ushort hopcount;                 /* Hop count */
    ushort delay;                    /* Delay (ticks) */
} novroutetype;

typedef novroutetype *nov_backup_routetable[NOV_NETHASHLEN];

/*
 * Structure used to maintain information about a routing process that
 * has been named, but not started.  This is used to carry information
 * over from the 'router' command to the 'network' command.
 */
typedef struct nov_spawntype_ {
    struct nov_spawntype_ *next;
    novpdbtype *protocol;
    int address;
    boolean sense;
} nov_spawntype;

typedef struct nov_sumroutetype_ {
    rntype     rt_nodes[2];  /* radix pointer info */
    novndbtype *ndb;         /* ndb route info */
} nov_sumroutetype;

typedef struct ipxradixkeytype_ {
    uchar pad[3];
    uchar length;
    ulong value;
} ipxradixkeytype;

/*
 * ipxaddr2radixkey
 *
 * Move IPX address into a special format for the radix code.
 * The first three (ignored) bytes are a padding.
 * The next byte is the length of the length byte + key
 * followed by the key.
 *
 * Return a pointer to the length byte of this data structure.
 */
static inline uchar *
ipxaddr2radixkey (ulong source, ipxradixkeytype *key)
{
    key->length = IPXRADIXKEYLEN;
    key->value  = source;
    return(&key->length);
}

/*
 * ipx_route_valid
 *
 * Returns TRUE if an IPX route is valid (i.e., not unreachable)
 */
static inline boolean ipx_route_valid (novndbtype *ndb)
{
    extern nidbtype *nullnidb;

    if ((ndb->delay == NOVELL_INFDELAY && !(ndb->flags & NET_IGRP)) ||
	ndb->path_list == NULL || ndb->path_list->nidb == nullnidb ||
	(ndb->flags & NET_IGRP && ndb->delay == METRIC_INACCESSIBLE) ||
	TIMER_RUNNING(ndb->holddownuntil) ||
	(!(ndb->options & IPX_NDB_OPTION_VALID))) {
	return(FALSE);
    } else {
	return(TRUE);
    }
}

#endif __NOVROUTE_H__

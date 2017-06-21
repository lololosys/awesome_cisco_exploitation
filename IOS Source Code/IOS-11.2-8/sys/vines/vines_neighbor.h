/* $Id: vines_neighbor.h,v 3.6.18.2 1996/05/11 16:17:33 thille Exp $
 * $Source: /release/112/cvs/Xsys/vines/vines_neighbor.h,v $
 *------------------------------------------------------------------
 * vines_neighbor.c - support for keeping Vines neighbor tables
 * 
 * September 1992, David R. Hampton
 * from a module dated February 1990, David R. Hampton
 *
 * Copyright (c) 1990-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: vines_neighbor.h,v $
 * Revision 3.6.18.2  1996/05/11  16:17:33  thille
 * CSCdi51397:  Duplicate strings in vines code
 * Branch: California_branch
 * Clean up some duplicate literals and save 1140 bytes in gs7-j-m.
 *
 * Revision 3.6.18.1  1996/03/18  22:32:02  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.5.2.2  1996/03/07  11:04:50  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.5.2.1  1996/02/20  21:46:37  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.6  1996/02/08  20:13:55  thille
 * CSCdi48347: format string in inlines, code size impact
 * Pass 1 - Clean up some literal strings in inlines, duplicate literals,
 * etc.  11,356 bytes saved on gs7-j-m, 9336 bytes saved on igs-j-l
 *
 * Revision 3.5  1996/01/22  07:44:41  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.4  1995/12/08  04:56:19  dstine
 * CSCdi45233:  Namespace breakage for RBTree
 *         Just normalize all RBTree* names.
 *
 * Revision 3.3  1995/12/07  22:33:35  hampton
 * Label the large chunks of memory allocated at system initialization
 * time.  This makes it easier to see where all the memory in router is
 * going.  [CSCdi45209]
 *
 * Revision 3.2  1995/11/17  17:58:07  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:44:54  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:14:39  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include <string.h>
#include <ciscolib.h>
extern uint vines_v0_count, vines_v0_servers;
extern uint vines_v1_count, vines_v1_servers;
extern rbTree *vines_neighbor_tree;

typedef struct vneigh_fn_data_ {
    vinesidbtype *idb;
    ulong net;
    ushort host;
    ulong flag;
    char *string;
    idbtype *phys_idb;
} vneigh_fn_data;

/**********************************************************************
 *
 *			      NEIGHBORS
 *
 **********************************************************************/

/*
 * Neighbor flags
 */
#define V_N_SELF	0x0001
#define V_N_STATIC	0x0002
#define V_N_DYNAMIC	0x0004
#define V_N_IGRP	0x0008
#define V_N_TEST	0x0010
#define V_N_REDIRECT	0x0020
#define V_N_PLACEHOLDER	0x0040
#define V_N_USE_ROUTE	0x8000

#define V_N_ME		0x0003

/*
 * A 'path' to a neighbor.  There is approximately one of these per
 * neighbor per interface through wich it can be reached.  Only the best
 * metric paths (and static paths) are maintained in the tables.
 */
struct vinespathtype_ {
    vinespathtype *next;		/* all paths for this neighbor */
    vinespathtype *next_best;		/* best paths for this neighbor */
    vinesneighbortype *neighbor;
    dqueue_t	   by_idb;		/* all paths via the same interface */
    ulong          magic;
    ulong          flags;
    ulong          uses;
    boolean	   snapshot_no_age; 	/* TRUE if no age since snapshot idb */
    sys_timestamp  timer;		/* time entry last updated */
    sys_timestamp  gctime;		/* time when garbage collected  */
    ulong   	   metric;		/* metric in 0.125 second counts */
    vinesidbtype  *idb;
    int	           enctype;
    hwaddrtype     dlc_addr;

    /* Non-sequenced RTP stuff */
    uchar     controller;
    uchar     machine;
    ulong     rtp_counts[VRTP_LAST];
    mgd_timer rc_timer;
    ulong     rc_state;			/* 0 = long pause, [123] = retry, 4 = give up */

    /* Sequenced RTP stuff */
    int       state;
    int       srtp_flags;
    ulong     reflected_timestamp;	/* local timestamp as reflected from neighbor */
    ulong     origin_timestamp;
    ulong     local_timestamp;
    ulong     packet_id;
    queuetype reassemblyQ;
    mgd_timer reassembly_timer;
    mgd_timer retry_timer;
    int       retry_count;
    ulong     srtp_rx_req_counts[vsrtp_rx_req_max];
    ulong     srtp_rx_upd_counts[vsrtp_rx_upd_max];
    ulong     srtp_rx_redir_count;
    ulong     srtp_rx_reinit_count;
    ulong     srtp_tx_req_counts[5];
    ulong     srtp_tx_upd_count;
    ulong     srtp_tx_resp_count;
    ulong     srtp_tx_redir_count;
    ulong     srtp_tx_reinit_count;
};

/*
 * A neighbor.  There is one of these per vines station on any adjacent
 * physical network.  If a station appears on more than one adjacent
 * physical network, there is only one neighbor entry allocated for it.
 * The metric value reflects the current best metric value of all paths
 * to this neighbor.
 */
struct vinesneighbortype_ {
    treeLink link;
    ulong   magic;
    ulong   newflags;
    ulong   net;
    ushort  host;
    uchar   node;
    uchar   version;
    ulong   metric;	/* metric in 0.125 second counts */
    boolean is_router;

    /* Sequenced RTP stuff */
    sys_timestamp timer;
    int     srtp_flags;

    int path_count;
    vinespathtype *available_paths;
    vinespathtype *best_path;
    vinespathtype *primary_path;
};


/**********************************************************************
 *
 *	           Neighbor Tree Interface Routines
 *
 **********************************************************************/

/*
 * vneigh_EntryKey
 * Create a treeKey for a neighbor which is always unique.
 */
static inline treeKey
vneigh_EntryKey (ulong net, ushort host)
{
    treeKey key;
    key.ll = ((ulonglong)net << 16) | host;
    return key;
}

/*
 * vneigh_NeighborCast
 * Do data type conversion between links and neighbors.
 */
static inline vinesneighbortype*
vneighCast (treeLink* link) { return(vinesneighbortype*)link; }

/*
 * vneigh_UnprotectNeighbor
 * Allow a neighbor to be freed or reused.
 */
static inline void
vneigh_UnprotectNeighbor (vinesneighbortype* s) 
{ 
    RBTreeNodeProtect(&s->link,FALSE); 
}

/*
 * vneigh_ProtectNeighbor
 * Protect a neighbor from being freed.
 */
static inline void
vneigh_ProtectNeighbor (vinesneighbortype* s) 
{ 
    RBTreeNodeProtect(&s->link,TRUE); 
}

/*
 * vneigh_NeighborProtected
 * Return protection status of neighbor treeLink entry.
 */
static inline boolean
vneigh_NeighborProtected (vinesneighbortype* s)
{
    return RBTreeNodeProtected(&s->link);
}

/*
 * vneigh_GetNextNeighbor - Search for the next neighbor and return it w/o protection
 */
static inline vinesneighbortype*
vneigh_GetNextNeighbor (vinesneighbortype* neighbor)
{
    vinesneighbortype* s = neighbor;

    if (vines_neighbor_tree == NULL)
	return NULL;
    if (s == NULL) {
	s = vneighCast(RBTreeFirstNode(vines_neighbor_tree));
    } else {
	s = vneighCast(RBTreeNextNode(&s->link));
    }
    return s;
}

/*
 * vneigh_GetFirstNeighbor
 * Get the first neighbor from the tree.
 */
static inline vinesneighbortype*
vneigh_GetFirstNeighbor (void)
{ 
    return vneigh_GetNextNeighbor(NULL); 
}

/*
 * vneigh_GetBestNeighbor
 * Return the specified neighbor OR the next greater neighbor.
 */
static inline vinesneighbortype*
vneigh_GetBestNeighbor (treeKey key)
{
    return vneighCast(RBTreeLexiNode(vines_neighbor_tree,key));
}

/*
 * vneigh_InsertEntry
 * Add a neighbor to our table.
 */
static inline void
vneigh_InsertEntry (vinesneighbortype* s) 
{
    treeKey key = vneigh_EntryKey(s->net,s->host);
    if (vines_neighbor_tree != NULL)
	RBTreeInsert(key,vines_neighbor_tree,&s->link);
}

/*
 * vneigh_DeleteEntry
 * Delete a neighbor from our table.
 */
static inline void
vneigh_DeleteEntry (vinesneighbortype* s)
{
    if ((vines_neighbor_tree != NULL)&&(s!=NULL))
	RBTreeDelete(vines_neighbor_tree,&s->link);
}

/*
 * vneigh_ReinitNeighbor
 * Release any resources in use for this neighbor
 */
static inline void
vneigh_ReinitNeighbor (vinesneighbortype* s)
{
	/* Currently no special resources */
}

/*
 * vneigh_CreateEntry
 * initialize a new neighbor
 */
static inline vinesneighbortype*
vneigh_CreateEntry (ulong net, ushort host)
{
    vinesneighbortype* s;

    if (vines_neighbor_tree == NULL)
	return NULL;

    s = (vinesneighbortype*) RBTreeGetFreeNode(vines_neighbor_tree);
    if (s != NULL)
	vneigh_ReinitNeighbor(s);
    else
	s = malloc_named(sizeof(vinesneighbortype), vines_neighbor_malloc_str);
    if (s == NULL)
	return NULL;
    memset(s, 0, sizeof(vinesneighbortype)); /* clear node */
    s->net = net;
    s->host = host;
#ifdef notdef
    if (vinestree_debug)
	errmsg(&msgsym(NEWNEIGHBOR, VINES), s->net, s->host);
#endif
    vneigh_InsertEntry(s);
    return s;
}

/*
 * vneigh_FindEntry
 *
 * Find a neighbor for the given address. No NULL parameters allowed.
 */
static inline vinesneighbortype*
vneigh_FindEntry (ulong net, ushort host)
{
    vinesneighbortype* s;
    treeKey key;

    key = vneigh_EntryKey(net, host);
    if (vines_neighbor_tree == NULL)
	return (NULL);
    s = vneighCast(RBTreeSearch(vines_neighbor_tree,key));
    return (s);
}

/*
 * vneigh_WalkTree
 *
 * Call the specified function for each node in the tree.
 */
static inline boolean
vneigh_WalkTree (treeProc proc, vneigh_fn_data *data)
{
    return(RBTreeForEachNode(proc, (void *)data, vines_neighbor_tree, TRUE));
}

/*
 * vneigh_WalkPartialTree
 *
 * Call the specified function for each node in the tree.
 */
static inline boolean
vneigh_WalkPartialTree (treeProc proc, vinesneighbortype *s, void *data)
{
    if (s)
	return(RBTreeForEachNodeTilFalse(proc, data, vines_neighbor_tree,
					 &s->link, TRUE));
    return(RBTreeForEachNodeTilFalse(proc, data, vines_neighbor_tree,
				     NULL, TRUE));
}

static inline vinesneighbortype *
vneigh_FindSelf (boolean debug, char *tag)
{
    vinesneighbortype *self;

    self = vneigh_FindEntry(vines_network, vines_host);
    if (!self && debug)
	buginf(vines_cannot_find_neighbor_msg, tag);
    return(self);
}


/**********************************************************************
 *
 *			General Neighbor Inlines
 *
 **********************************************************************/

static inline boolean vines_servers_present (vinesidbtype *idb)
{
    return(idb->v0_servers || idb->v1_servers);
}

static inline boolean vines_routers_present (vinesidbtype *idb)
{
    return(idb->v0_servers || idb->v0_routers ||
	   idb->v1_servers || idb->v1_routers);
}

extern boolean vines_vsrtp_enabled(vinesidbtype *);

static inline boolean vines_rtp_present (vinesidbtype *idb)
{
    if (!vines_vsrtp_enabled(idb))
	return(TRUE);
    return(idb->v0_servers || idb->v0_routers || idb->v0_clients);
}

static inline boolean vines_srtp_present (vinesidbtype *idb)
{
    return(idb->v1_servers || idb->v1_routers || idb->v1_clients ||
	   !vines_rtp_present(idb));
}

static inline boolean vines_anyone_present (vinesidbtype *idb)
{
    return(vines_rtp_present(idb) || vines_srtp_present(idb));
}

static inline boolean vines_any_servers_present (void)
{
    return(vines_v0_servers || vines_v1_servers);
}

static inline boolean vines_any_rtp_present (void)
{
    return(vines_v0_count);
}

static inline boolean vines_any_srtp_present (void)
{
    return(vines_v1_count);
}

/*
 * Macros to traverse all paths in a neighbor entry
 */
#define FOR_ALL_PATHS(neighbor, path) \
    for ((path) = (neighbor)->available_paths; (path); (path) = (path)->next)

#define FOR_ALL_PATHS_W_PREV(neighbor, path, prev) \
    prev = (vinespathtype *)&(neighbor)->available_paths; \
    for ((path) = (neighbor)->available_paths; (path); (prev) = (path), (path) = (path)->next)

/*
 * vines_get_path
 *
 * Move the best_path pointer to the next best usable path for this
 * neighbor.
 */
static inline vinespathtype *vines_get_path (
    vinesneighbortype *neighbor)
{
    vinespathtype *path;

    if (!neighbor->best_path)
	return(NULL);
    path = neighbor->best_path;
    neighbor->best_path = path->next_best;
    path->uses++;
    return(path);
}

extern vinespathtype *vines_get_path_by_xxx (vinesneighbortype *, ulong, vinesidbtype *, int, hwaddrtype *);
extern void vines_flush_all_paths_by_xxx(int, vinesidbtype *);


#ifdef __INTERFACE_H__
static inline vinespathtype *vines_get_path_by_idb (
    vinesneighbortype *neighbor,
    vinesidbtype *idb)
{
    return(vines_get_path_by_xxx(neighbor, 0, idb, ET_NULL, NULL));
}

static inline vinespathtype *vines_get_path_by_flags (
    vinesneighbortype *neighbor,
    ulong flags)
{
    return(vines_get_path_by_xxx(neighbor, flags, NULL, ET_NULL, NULL));
}

static inline void vines_flush_all_paths_by_flags (int flag)
{
    vines_flush_all_paths_by_xxx(flag, NULL);
}

static inline void vines_flush_all_paths_by_idb (vinesidbtype *idb)
{
    vines_flush_all_paths_by_xxx(V_N_DYNAMIC, idb);
}
#endif

/**********************************************************************
 *
 *                      External Definitions
 *
 **********************************************************************/
 
extern void vneigh_ShowStats(parseinfo *);
extern vinespathtype *vines_get_path_by_xxx(vinesneighbortype *, ulong, vinesidbtype *, int, hwaddrtype *);
extern vinesneighbortype *vines_flush_paths_by_flag(vinesneighbortype *, int, vinespathtype *);
extern void vines_flush_dynamic_paths(ulong, ushort);
extern void vines_fixup_neighbor(vinesneighbortype *);
extern boolean vines_check_path_list(vinesneighbortype *, vinespathtype **);
extern void vines_insert_new_path(vinesneighbortype *, vinespathtype *);
extern vinespathtype *vines_make_new_path(vinesneighbortype *);
extern vinespathtype *vines_add_path(vinesneighbortype *, ulong, vinesidbtype *, int, hwaddrtype *, int, int, uchar);
extern vinesneighbortype *vines_delete_path(vinesneighbortype *, ulong, vinesidbtype *, int, hwaddrtype *);
extern int vines_get_neighbor_metric(ulong, ushort);
extern boolean vines_best_path_uses_idb(vinesneighbortype *, vinesidbtype *);
extern boolean vines_verify_neighbor_structure(vinesneighbortype *);
extern vinesneighbortype *vines_add_neighbor(ulong, ushort, ulong, int, int, uchar);
extern void vines_delete_neighbor_entry(vinesneighbortype *);
extern void vines_delete_neighbor(ulong, ushort);
extern void vines_poison_all_paths_by_idb(vinesidbtype *);
extern boolean vines_neighbor_update(paktype *, vinesneighbortype **, vinespathtype **, char **, int, uchar);
extern void vines_neighbor_ager(boolean);
extern void vines_display_neighbors(ulong, ushort, vinesidbtype *);
extern void vines_neighbor_init(void);
extern boolean vines_best_path_from_delta_or_snapshot(vinesroutetype *);
extern void vines_new_neighbor(vinesneighbortype *, vinespathtype *path);
extern int vines_get_slowest_update_interval(vinesneighbortype *);
extern void vneigh_display_interface(vinesidbtype *idb);
extern void vines_start_aging_paths(idbtype *idb, paktype *pak);
extern void vines_flush_static_paths(idbtype *, int);
extern ulong vneigh_get_path_count(void);
extern ulong vneigh_get_neighbor_count(void);

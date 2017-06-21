/* $Id: ospf_dbase.c,v 3.5.26.9 1996/08/28 12:53:12 thille Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/ospf_dbase.c,v $
 *------------------------------------------------------------------
 * ospf_dbase.c -- Manages Link State Advertisement Database
 *
 * September 12 1990, Phanindra Jujjavarapu
 *
 * Copyright (c) 1990-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ospf_dbase.c,v $
 * Revision 3.5.26.9  1996/08/28  12:53:12  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.5.26.8  1996/08/26  23:47:56  myeung
 * CSCdi67216:  OSPF area DCbitless LSA count is corrupted
 * Branch: California_branch
 *
 * Revision 3.5.26.7  1996/08/23  21:36:36  myeung
 * CSCdi66427:  LSA refresh is sent across OSPF demand circuit
 * Branch: California_branch
 *
 * Revision 3.5.26.6  1996/06/04  21:52:15  myeung
 * CSCdi58058:  ospf nssa - summary address with not-advertise does not
 * work
 * Branch: California_branch
 *
 * Revision 3.5.26.5  1996/05/23  12:06:41  klmj
 * CSCdi45519:  OSPF: Incorrect metric calculation for External Type
 * 1 routes (note:  this also fixes CSCdi44939)
 * Branch: California_branch
 *
 * Revision 3.5.26.4  1996/05/10  17:40:41  myeung
 * CSCdi48102:  OSPF self-originated MAXAGE external LSA stuck in database
 * Branch: California_branch
 *
 * Revision 3.5.26.3  1996/04/30  23:14:16  myeung
 * CSCdi56225:  OSPF type 7 LSA is not handled correctly
 * Branch: California_branch
 * - Fix request list for type 7 LSA
 * - Remove translated type 5 LSA when type 7 LSA is not reachable
 *
 * Revision 3.5.26.2  1996/04/16  19:01:43  rchandra
 * CSCdi54830:  IP features commit
 * Branch: California_branch
 *
 * Revision 3.5.26.1  1996/03/18  20:28:22  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.5.2.1  1996/03/07  09:49:43  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.5  1996/01/22  06:14:27  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.4  1996/01/05  10:17:34  hampton
 * Move/rename the files containing the deprecated timer callback
 * routines.  [CSCdi46482]
 *
 * Revision 3.3  1995/11/18  18:48:11  myeung
 * CSCdi43168:  Clean up OSPF flooding
 * - Consolidate flooding functions
 *
 * Revision 3.2  1995/11/17  17:35:50  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:09:33  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.9  1995/10/12  11:50:51  myeung
 * CSCdi42047:  Clear up OSPF db flags
 * - Replace bit mask with bit field variable.
 *
 * Revision 2.8  1995/10/08  23:44:23  myeung
 * CSCdi41790:  Boolean db_free in db structure should be replaced by bit
 * field
 * - Backout the patch
 * - Decide to use bit variable instead of bit mask to replace boolean
 *
 * Revision 2.7  1995/10/08  22:49:35  myeung
 * CSCdi41790:  Boolean db_free in db structure should be replaced by bit
 * field
 *
 * Revision 2.6  1995/09/05  22:09:01  myeung
 * CSCdi39807:  OSPF need access-list debugging
 * - Add access-list debugging for SPF calculation and flooding
 * - Add separate debugging for intra-area, inter-area and external SPF
 *
 * Revision 2.5  1995/08/01  15:15:53  myeung
 * CSCdi36150:  OSPF database synchronization problem after route recovers
 * - Always take care of the elasped time when calculating LSA age
 * - The maximum age used for comparsion between LSA should be 3600
 *
 * Revision 2.4  1995/06/28  03:21:27  hampton
 * Use the passive timer macros instead of direct references to the system
 * clock.  [CSCdi36502]
 *
 * Revision 2.3  1995/06/19 01:08:06  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.2  1995/06/16  00:21:36  aboonkon
 * CSCdi32647: OSPF external route is not removed when the external LSA is
 *             flushed
 *
 * Remove unnecessary setting of metric to infinity for an external LSA
 * that is being flushed.  The side-effect of that is: it clears the
 * metric type bit which eventually leads to a change of metric type seen
 * by the receiver and, thus, the route is not removed.
 *
 * Revision 2.1  1995/06/07  21:09:18  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Routines Exported from this module
 *
 * db_add_lsa:	adds a new lsa or replaces existing one and returns pointer 
 * db_find_lsa:	searches database and returns a pointer to database element
 * db_get_first_db: get the first db in the database
 * db_get_next_db: get the next db in the database
 * db_free: deletes a database element and frees the memory.
 * db_install:	takes an lsa and tries to install it in dbase
 *
 */

#include "master.h"
#include <ciscolib.h>
#include "../os/old_timer_callbacks.h"
#include "mgd_timers.h"
#include "interface_private.h"
#include "packet.h"
#include "../ui/debug.h"
#include "route.h"
#include "iprouting_debug.h"
#include "../ip/ip.h"
#include "logger.h"
#include "../iprouting/msg_ospf.c"	/* Not a typo, see logger.h */
#include "../util/tree.h"
#include "ospf_pak.h"
#include "ospf_dbase.h"
#include "ospf.h"

/* 
 * Functions
 */

/*
 * sl_insert
 * Insert an element into the sorted list. The key must be unique.
 */
static sl_link *sl_insert (sl_link **head, sl_link *link) 
{
    sl_link *cur_link = *head, **prev = head;

    while (cur_link) {
        if (cur_link->key >= link->key) {
            if (cur_link->key == link->key)
                return NULL;
	    else 
                break;
        }
        prev = &cur_link->next;
        cur_link = cur_link->next;
    }
    link->next = cur_link;
    link->prev = prev;
    *prev = link;
    if (cur_link)
        cur_link->prev = &link->next;
    return link;
}


/*
 * sl_delete
 * Delete an element from the sorted list
 */
static sl_link *sl_delete (sl_link *link) 
{
    *link->prev = link->next;
    if (link->next)
        link->next->prev = link->prev;
    link->prev = NULL;
    link->next = NULL;    
    return link;
}


/*
 * sl_search
 * Search for an element in the sorted list
 */
static sl_link *sl_search (sl_link *head, ulong key)
{
    while (head) {
        if (head->key >= key) {
            if (head->key == key)
	        return head;
	    else
	        break;
	}
        head = head->next;
    }
    return NULL;
}


/*
 * db_get_first_db
 * Get the first element in the Link State database
 */
lsdbtype *db_get_first_db (rbTree *tree)
{
    lsidnodetype *lsidnode;
    lsidnode = (lsidnodetype *) RBTreeFirstNode(tree);
    if (lsidnode)
        return (lsdbtype *) lsidnode->sl_list;
    return NULL;
}


/*
 * db_get_next_db
 * Get the next element in the Link State database
 */
lsdbtype *db_get_next_db (lsdbtype *db) 
{
    lsdbtype *next_db;
    lsidnodetype *lsidnode;

    next_db = (lsdbtype *) db->link.next;
    if (!next_db) {
        lsidnode = (lsidnodetype *) RBTreeNextNode(&db->lsidnode->link);
        if (!lsidnode)
            return NULL;
        next_db = (lsdbtype *) lsidnode->sl_list;
    }
    return next_db;
}


/* 
 * db_valid_db
 * Check if a db is valid - if it is in the database
 */
inline boolean db_valid_db (lsdbtype *db)
{
    return (db->link.prev || db->link.next);
}


/*
 * db_add_lsa:
 * Takes a link state advertisement and
 * adds it to an area's link state database and
 * returns a pointer to that entry.
 */
boolean db_add_lsa (
    lsdbtype **ret_ptr,		/* return pointer to db */
    areatype *area,		/* Add to this area's database */
    ulong lsid,			/* Link State ID */
    ulong adv_rtr,		/* Advertising Router */	
    lsatype *data,		/* data to added */
    uchar lsa_type)		/* type of this link advertisement */
{
    lsdbtype *db;
    rbTree *tree;
    treeKey key;
    lsidnodetype *lsidnode = NULL;

    tree = area->ar_lsdbase[lsa_type];
    key.l = lsid;
    lsidnode = (lsidnodetype *) RBTreeSearch(tree, key);
    if (lsidnode) {
	if ((lsa_type == LSA_TYPE_RTR) || (lsa_type == LSA_TYPE_NET)) {
	    db = (lsdbtype *) lsidnode->sl_list;
	    db->link.key = adv_rtr;
	} else {
            db = (lsdbtype *) sl_search(lsidnode->sl_list, adv_rtr);
	}
	if (db) { 
            if (db->db_lsa_ptr == NULL) {
                goto install_data;
            } else {
                *ret_ptr = db;
                return (TRUE);
            }
	}
    }

    if ((db = malloc(sizeof(lsdbtype))) == NULL) {
	*ret_ptr = NULL;
	return(FALSE);
    }
    *ret_ptr = db;

    if (!lsidnode) {
        if ((lsidnode = malloc(sizeof(lsidnodetype))) == NULL) {
	    free(db);
            *ret_ptr = NULL;
	    return(FALSE);
	}
        key.l = lsid;
        RBTreeInsert(key, tree, &lsidnode->link);
        lsidnode->tree_belong = tree;
        lsidnode->sl_list = NULL;
    }    
    db->link.key = adv_rtr;
    sl_insert(&lsidnode->sl_list, &db->link);
    db->lsidnode = lsidnode;

  install_data:  
    free(db->db_lsa_ptr);
    /* had a previously used one around */
    db->db_freeme = FALSE;
    db->db_lsa_ptr = data;
    db->db_dist = 
	(lsa_type < LSA_TYPE_SUM_NET) ? INTRA_PATH_COST_MAX : SUM_LS_INFINITY;
    ospf_event(OC_DB_ADD, lsid, ((ulong) db) | (lsa_type << 24));
    return(FALSE);

} /* end of db_add_lsa */


/*
 * db_find_lsa:
 * looksup database for a
 * Link state advertisement
 */
lsdbtype *db_find_lsa (areatype *area,	/* search in this area */
		       ulong lsid,
		       ulong adv_rtr,
		       uchar lsa_type)	/* type advertisement */
{
    lsdbtype *db;
    rbTree *tree;
    treeKey key;
    lsidnodetype *lsidnode = NULL;

    tree = area->ar_lsdbase[lsa_type];
    key.l = lsid;
    lsidnode = (lsidnodetype *) RBTreeSearch(tree, key);
    if (lsidnode) {
	/* 
	 * For router and network LSA, the second key adv_rtr is not used.
	 */
	if ((lsa_type == LSA_TYPE_RTR) || (lsa_type == LSA_TYPE_NET))
	    db = (lsdbtype *) lsidnode->sl_list;
	else 
            db = (lsdbtype *) sl_search(lsidnode->sl_list, adv_rtr);
	if (db && (db->db_lsa_ptr != NULL))
            return db;
    }
    return(NULL);
}


static inline lsdbtype *find_other_db_sub (
    rbTree *tree,
    ulong dest, 
    ulong mask, 
    ulong *rtr_id, 
    int index,
    boolean check_mask)
{
    treeKey key;
    lsidnodetype *lsidnode = NULL;
    ex_lsatype *ex_lsa;
    lsdbtype *db;
    sl_link *link;
    int j;

    key.l = dest;
    lsidnode = (lsidnodetype *) RBTreeSearch(tree, key);
    if (!lsidnode)
        return (NULL);
    for (link = lsidnode->sl_list; link; link = link->next) {
	db = (lsdbtype *) link;
	if ((db->db_lsa_ptr == NULL) || (ADV_AGE(db) >= MAXAGE) 
	    || db->db_freeme)
	    continue;
	ex_lsa = (ex_lsatype *)db->db_lsa_ptr;
	if (check_mask && (ex_lsa->la_mask != mask))
	    continue;

	for (j = 0; j < index; j++) {
	    if (ex_lsa->la_hdr.lsh_adv_rtr == rtr_id[j])
		break;
	}
	if (j == index)
	    return(db);
    }
    return(NULL);
}
 

/*
 * find_ex_db
 * Look up database for external LSA using hash chain based on destination.
 * We won't pick the LSA generated by rtr_id.
 */
lsdbtype *find_ex_db (
    areatype *area,
    ulong dest,    
    ulong rtr_id,
    uchar lsa_type)
{
    int index;
    index = rtr_id ? 1 : 0;
    return (find_other_db_sub(area->ar_lsdbase[lsa_type], dest, 0,
			      &rtr_id, index, FALSE));
} /* end of find_ex_db */


/*
 * find_other_ex_db
 * Look up database for external LSA that is not advertised by router 
 * IDs passed.  index is the number of router ID passed in rtr_id.
 */

lsdbtype *find_other_ex_db (
    areatype *area,
    ulong dest,
    ulong mask,
    ulong *rtr_id,
    int index,
    uchar lsa_type)
{
    return (find_other_db_sub(area->ar_lsdbase[lsa_type], dest, mask,
			      rtr_id, index, TRUE));
} /* end of find_other_ex_db */

/*
 * find_other_sum_db
 * looksup database for summary
 * Link state advertisement that is not
 * advertised by router ids passed.
 */

lsdbtype *find_other_sum_db (
    areatype *area,
    ulong dest,
    ulong mask,
    ulong *rtr_id,
    int index,
    uchar lsa_type)
{
    return (find_other_db_sub(area->ar_lsdbase[lsa_type], dest, 
			      mask, rtr_id, index, TRUE));
} /* end of find_other_sum_db */


/*
 * db_free_check
 */
static boolean db_free_check (pdbtype *pdb, lsdbtype *db) 
{
    /*
     * Clear last reason
     */
    db->db_nf_freeme = FALSE;
    db->db_nf_retrans = FALSE;
    db->db_nf_nbr_ex_load = FALSE;
    db->db_nf_rt_table = FALSE;
    db->db_nf_flood_dly = FALSE;
    db->db_nf_ack_cnt = FALSE;
    db->db_nf_dbase_lock = FALSE;
    db->db_nf_in_maxageq = FALSE;
    db->db_nf_partial = FALSE;

    GET_TIMESTAMP(db->db_free_time);

    if (db->db_ackcnt > 0)
        db->db_nf_ack_cnt = TRUE;
    if (pdb->ospf_dbase_lock)
        db->db_nf_dbase_lock = TRUE;
    if (db->db_retrans)
        db->db_nf_retrans = TRUE;
    if (pdb->ospf_nbr_ex_load)
        db->db_nf_nbr_ex_load = TRUE;
    if (db->db_rt_table)
	db->db_nf_rt_table = TRUE;
    if (db->db_flood_dly)
	db->db_nf_flood_dly = TRUE;
    if (db->db_in_maxageq)
        db->db_nf_in_maxageq = TRUE;
    if (!db->db_freeme)
        db->db_nf_freeme = TRUE;
    if (db->db_partial)
	db->db_nf_partial = TRUE;
    return (!(db->db_nf_freeme || db->db_nf_retrans ||
	      db->db_nf_nbr_ex_load || db->db_nf_rt_table ||
	      db->db_nf_flood_dly || db->db_nf_ack_cnt ||
	      db->db_nf_dbase_lock || db->db_nf_in_maxageq ||
	      db->db_nf_partial));
}


/*
 * db_free
 * frees database entry if
 * it is not waiting for any acks
 * Return TRUE if db is deleted, FALSE if not
 */
boolean db_free (pdbtype *pdb, lsdbtype *db)
{
    ex_lsatype *ex_lsa;
    lsidnodetype *lsidnode;

    /*
     * If OSPF_CLEANING_UP is set, delete without checking
     * else do not delete the db if 
     *   1. we are waiting for ack for this db  or 
     *   2. database lock (during suspend)      or 
     *   3. db is in the maxage delete queue already
     */
    if (!(pdb->ospf_flag & OSPF_CLEANING_UP)) {
        if (!db_free_check(pdb, db))
	    return (FALSE);
    }
   
    lsidnode = db->lsidnode;
    ospf_event(OC_DB_FREE, lsidnode->link.key.l, ((ulong) db) | 
	       (LSA_TYPE(db) << 24));

    if ((LSA_TYPE(db) == LSA_TYPE_ASE) && (db->db_lsa_ptr->lsh_age == 0)
	&& (ADV_RTR(db) == pdb->ospf_rtr_id)) {
	/*
	 * BAD BAD thing. we are deleting our own external LSA that 
	 * should not be deleted.
	 */
	ex_lsa = (ex_lsatype *)db->db_lsa_ptr;
	errmsg(&msgsym(NOTREDIST5, OSPF), LSA_ID(db), ex_lsa->la_mask);
    }

    free_plist(db);
    if (db->db_lsa_ptr != NULL)
	free(db->db_lsa_ptr);
    db->db_lsa_ptr = NULL;

    sl_delete(&db->link);
    free(db);

    if (lsidnode->sl_list == NULL) {
        RBTreeDelete(lsidnode->tree_belong, &lsidnode->link);
        RBTreeTrimFreeList(lsidnode->tree_belong);
    }
    return (TRUE);

} /* end of db_free */


/*
 * add_lsa
 * adds a Link state advertisement
 * to the database.
 */
static void add_lsa (
    lsdbtype	*db,
    ulong	lsid,
    ulong	adv_rtr,
    lsatype	*src_lsa)
{
    lsatype	*lsa;

    if ((lsa = malloc( src_lsa->lsh_length)) == NULL) {
	return;
    }
    free(db->db_lsa_ptr);
    bcopy(src_lsa, lsa, src_lsa->lsh_length);
    /* had a previously used one around */
    db->db_lsa_ptr = lsa;
    db->lsidnode->link.key.l = lsid;
    db->link.key = adv_rtr;
    lsa_checksum(db);
    return;
} /* end of add_lsa */


/*
 * db_lookup_lsa
 * looks up LSA and decides
 * if it is more recent than the 
 * one we received.
 */
boolean db_lookup_lsa (areatype *area, lsatype *lsa)
{
    lsdbtype 	*db;

    db = db_find_lsa(area, lsa->lsh_id, lsa->lsh_adv_rtr, lsa->lsh_type);
    if ((!db) || (db->db_lsa_ptr == NULL))
        return(FALSE);
    if (MORE_RECENT(lsa, db->db_lsa_ptr, DB_ELAPSE(db)))
	return(FALSE);
    return(TRUE);
} /* end of db_lookup_lsa */


/*
 * ospf_db_find_lsa
 * Provide supernet LSA lookup functionality for Summary and External
 * (type 3 and 5) LSA.
 * First, look up for LSA with normal mask.   If it is of the two types
 * mentioned above, check that the mask is right.   If yes, just return.
 * If not, look again with LSA ID with host bit.
 *
 * Use ex_lsa for both summary and external because it has the same format,
 * i.e. mask is right behind OSPF header.    This is safe, unless OSPF
 * packet changes format (unlikely).
 */
lsdbtype *ospf_db_find_lsa (areatype *area,
			    ulong net_number,
			    ulong net_mask,
			    ulong adv_rtr,
			    uchar lsa_type)
{
    lsdbtype *db;
    ex_lsatype *ex_lsa;

    db = db_find_lsa(area, net_number, adv_rtr, lsa_type);

    if ((lsa_type == LSA_TYPE_SUM_NET) || (lsa_type == LSA_TYPE_ASE) ||
	(lsa_type == LSA_TYPE_7_ASE)) {
	if (db) {
	    ex_lsa = (ex_lsatype *)db->db_lsa_ptr;
	    if (ex_lsa->la_mask == net_mask)
		return(db);
	}
	db = db_find_lsa(area, (net_number | ~net_mask), adv_rtr, lsa_type);
    }
    return(db);
} /* end of ospf_db_find_lsa */
    

/*
 * ospf_address_to_idb
 * maps an ip address to
 * ospf interface in a given
 * area.
 */
static idbtype *ospf_address_to_idb (
    areatype *area,
    ipaddrtype address)
{
    idbtype *idb;

    for (idb = area->ar_if_list; idb; idb = idb->ospf_info->next)
	if (address == idb->ip_address)
	    return(idb);
    return(NULL);

} /* end of address_to_idb */


/*
 * compare_rtr_links
 * compares the router links
 * entry by entry to detect
 * any change in the LSA.
 */
static boolean compare_rtr_links(
    link_info *new_links,
    link_info *links,
    ushort count)
{
    ulong length;
    ulong old_length;
    ushort old_count;
    link_info *old_links;
    ushort new_count;

    length = old_length = 0;
    for (new_count = count; new_count; new_count--,
	 new_links = (link_info *)((ulong)new_links + length)) {
	for (old_count = count, old_links = links; old_count; old_count--, 
	     old_links = (link_info *)((ulong)old_links + old_length)) {

	    old_length = (12 + old_links->lnk_tos_count * 4);
	    if ((new_links->lnk_id == old_links->lnk_id)
		&& (new_links->lnk_data == old_links->lnk_data)
		&& (new_links->lnk_con_type == old_links->lnk_con_type)
		&& (new_links->lnk_tos_count == old_links->lnk_tos_count)
		&& (new_links->lnk_tos0_metric == old_links->lnk_tos0_metric))
		break;
	}
	if (old_count == 0)
	    /* New link did not match 
	     * with a old link
	     */
	    return(FALSE);
	else 
	    length = (12 + new_links->lnk_tos_count * 4);
    }
    return(TRUE);
} /* compare_rtr_links */


/*
 * lsa_cmp
 * Compare lsa for changes
 * set db_mask_change if mask has changed in type 2,3,5 LSA so
 * we will delete the old route before installing the new one.
 * Need to do this because mask is not part of index to an LSA
 * which means even when mask changes, it will map to the same
 * LSA.   And that causes new route to be installed but the
 * old route does not get deleted.
 */
static ushort lsa_cmp(
    areatype *area,
    lsatype  *lsa,
    lsdbtype   *db)
{
    ushort spf_run;
    lsatype	*db_lsa;
    rtr_lsatype *rtr_lsa;
    rtr_lsatype *db_rtr_lsa;
    net_lsatype *net_lsa;
    net_lsatype *db_net_lsa;
    ex_lsatype	*ex_lsa;
    ex_lsatype	*db_ex_lsa;
    sum_lsatype	*sum_lsa;
    sum_lsatype	*db_sum_lsa;

    spf_run = 0;
    db_lsa = db->db_lsa_ptr;

    /*
     * Catch the mask change first.
     * Since mask in different LSA type located at the same
     * position, we can use net lsa type to reference it.
     */
    if ((lsa->lsh_type > LSA_TYPE_RTR) && 
	(lsa->lsh_type <= LSA_TYPE_7_ASE) && 
	(lsa->lsh_type != LSA_TYPE_SUM_ASB)) {
	net_lsa = (net_lsatype *)lsa;
	db_net_lsa = (net_lsatype *)db_lsa;
	if (net_lsa->la_mask != db_net_lsa->la_mask) {
	    db->db_mask_change = TRUE;
	    return TRUE;
	}
    }
    if (lsa->lsh_options != db_lsa->lsh_options)
	return TRUE;
    if ((lsa->lsh_age == MAXAGE) || (db_lsa->lsh_age == MAXAGE))
	return TRUE;
    if (lsa->lsh_length != db_lsa->lsh_length)
	return TRUE;
    switch(lsa->lsh_type) {
      case LSA_TYPE_RTR:
	rtr_lsa = (rtr_lsatype *)lsa;
	db_rtr_lsa = (rtr_lsatype *)db_lsa;
	if ((rtr_lsa->la_lnk_cnt != db_rtr_lsa->la_lnk_cnt)
	    || (rtr_lsa->la_E_B != db_rtr_lsa->la_E_B)
 	    || (!(compare_rtr_links(&(rtr_lsa->la_link),&(db_rtr_lsa->la_link),
				    rtr_lsa->la_lnk_cnt)))) {
	    return TRUE;
	}
	break;
      case LSA_TYPE_NET:
	net_lsa = (net_lsatype *)lsa;
	db_net_lsa = (net_lsatype *)db_lsa;
	if (bcmp(lsa->lsh_data, db_lsa->lsh_data, 
		 lsa->lsh_length - LINK_STATE_HEADER_SZ)) {
	    return TRUE;
	} 
	break;
      case LSA_TYPE_SUM_NET:
	sum_lsa = (sum_lsatype *)lsa;
	db_sum_lsa = (sum_lsatype *)db->db_lsa_ptr;
	if (sum_lsa->la_tos_metric != db_sum_lsa->la_tos_metric)
	    return TRUE;
	break;
      case LSA_TYPE_SUM_ASB:
	sum_lsa = (sum_lsatype *)lsa;
	db_sum_lsa = (sum_lsatype *)db->db_lsa_ptr;
	if (sum_lsa->la_tos_metric != (db_sum_lsa->la_tos_metric))
	    return TRUE;
	break;
      case LSA_TYPE_ASE:
      case LSA_TYPE_7_ASE:
	ex_lsa = (ex_lsatype *)lsa;
	db_ex_lsa = (ex_lsatype *)db->db_lsa_ptr;
	if ((db_ex_lsa->la_metric.as_forw_addr 
	     != ex_lsa->la_metric.as_forw_addr)
	    || (db_ex_lsa->la_metric.as_ex_tag 
		!= ex_lsa->la_metric.as_ex_tag)
	    || (db_ex_lsa->la_metric.as_e_tos_metric 
		  != ex_lsa->la_metric.as_e_tos_metric)) {
	    /* there is a change. */
	    return TRUE;
	}
	break;
    }
    return(FALSE);
} /* end of lsa_cmp */


void ospf_dec_lsa_table (pdbtype *pdb,
                         areatype *area,
                         uchar type,
                         ushort chksum)
{
    if (type == LSA_TYPE_ASE) {
	PDB_SUB_MIB_TABLE(pdb, EXLSA_CHKSUMSUM, chksum);
	PDB_DEC_MIB_TABLE(pdb, EXLSA_COUNT);
    } else {
	area->ar_lsa_chksumsum -= chksum;
	area->ar_lsa_count--;
    }
}

void ospf_inc_lsa_table (pdbtype *pdb,
                         areatype *area,
                         uchar type,
                         ushort chksum)
{
    if (type == LSA_TYPE_ASE) {
	PDB_ADD_MIB_TABLE(pdb, EXLSA_CHKSUMSUM, chksum);
	PDB_INC_MIB_TABLE(pdb, EXLSA_COUNT);
    } else {
	area->ar_lsa_chksumsum += chksum;
	area->ar_lsa_count++;
    }
}


/*
 * db_install:
 * install a new copy and start spf and calculate routes.  
 *
 *
 *           Self-originated |  Flood to      |   Area
 * -------------------------------------------------------------------------
 * src_fl    Yes             |  every router  |   in this area
 * fl        No	  	     |  "" except nbr |   ""
 * ex_fl     No		     |  ""    ""      |   All area except stub area
 * src_ex_fl Yes	     |  ""    ""      |   ""
 * -------------------------------------------------------------------------
 */
void db_install (lsdbtype **db_ptr,
		 lsatype *lsa,
		 boolean do_not_age,
		 areatype *area,
		 idbtype *idb,
		 nbrtype *nbr,
		 list_queue *src_fl,
		 list_queue *fl,
		 list_queue *ex_fl,
		 list_queue *src_ex_fl)
{
    pdbtype *pdb;
    ospf_pdb_info *ospf_pdb;
    boolean self_lsa;
    ushort spf_run;
    lsdbtype *db;
    lsatype *data;
    idbtype *s_idb;
    boolean old_dcbitless_lsa_count, old_non_indication_lsa_count;
    boolean old_indication_lsa_count;
    boolean found = (*db_ptr != NULL);

    spf_run = 0;
    /*
     * this advertisement is more recent and needs to be 
     * installed if necessary.
     */
    pdb = area->ar_pdb_ptr;
    ospf_pdb = pdb->ospf_pdb;
    self_lsa = (lsa->lsh_adv_rtr == ROUTER_ID(area));
    if (!found) {
	/*
	 * Create a db IF LSA we received is not self originated
	 *             OR LSA is self originated AND it is not a router LSA
	 *                (We should not store info coming from outside 
	 *		   telling us about our topology)
	 */
	if ((!self_lsa) ||
	    ((self_lsa) && (lsa->lsh_type != LSA_TYPE_RTR))) {
	    db_add_lsa(db_ptr,
		       area, 
		       lsa->lsh_id, 
		       lsa->lsh_adv_rtr, 
		       NULL,
		       lsa->lsh_type);
	    db = *db_ptr;
	    if (db == NULL) {
		errmsg(&msgsym(NOMEMORY, OSPF), "link state database");
		return;
	    }
	    if ((data = malloc(lsa->lsh_length)) == NULL) {
		return;
	    }
	    bcopy((uchar *)lsa, (uchar *)data, lsa->lsh_length);
	    db->db_lsa_ptr = data;
	    GET_TIMESTAMP(db->db_time);
	}
    }
    db = *db_ptr;

    old_dcbitless_lsa_count = area->ar_dcbitless_lsa_count;
    old_non_indication_lsa_count = NON_INDICATION_LSA_COUNT(area);
    old_indication_lsa_count = area->ar_indication_lsa_count;
    /* now process it according to LSA type */
    if (self_lsa) {
	/*
	 * Don't allow DoNotAge for self originated LSA. We can get one
	 * if we reboot and get our old LSA from neighbor over demand
	 * circuit.
	 */
	do_not_age = FALSE;
	switch (lsa->lsh_type) {
	  case LSA_TYPE_RTR:
	    /*
	     * LSA we received is self originated. This indicates
	     * that this lsa was originated before the last
	     * time the router was restarted. advance the 
	     * sequence number and originate new instance of
	     * the advertisement, no matter we have one or not.
	     */
	    ospf_flood_acl_buginf("\nOSPF: we received our own old rtr lsa");
	    if (db) {
		/* 
		 * db can be NULL as we do not install our rtr lsa from
		 * other even we don't have one.
		 */
		if (db->db_retrans)
		    rem_db_retrans_list(db, NULL);
		db->db_lsa_ptr->lsh_seq = lsa->lsh_seq;
		lsa_checksum(db);
	    }
	    if (build_rtr_lsa(area, src_fl, TRUE))
		spf_run |= SPF_SCHD_RTR;
	    break;
	  case LSA_TYPE_NET:
	    ospf_flood_acl_buginf("\nOSPF: we received our own old net lsa");
	    if (db->db_retrans)
		rem_db_retrans_list(db, NULL);
	    s_idb = ospf_address_to_idb(area, lsa->lsh_id);
	    if (s_idb && (s_idb->ospf_info->if_state == IFS_DR)) {
		/* 
		 * We are DR, originate a new one. 
		 */
		db->db_lsa_ptr->lsh_seq = lsa->lsh_seq;
		lsa_checksum(db);
		if (build_net_lsa(s_idb, src_fl, TRUE))
		    spf_run |= SPF_SCHD_NET;
	    } else {
		/*
		 * We are not DR, premature it.
		 */
		if (found && !db->db_freeme) {
		    ospf_dec_lsa_table(pdb, area, lsa->lsh_type, 
					db->db_lsa_ptr->lsh_chksum);
		}
		/*
		 * The old LSA may not have DC bit set
		 */
		db->db_lsa_ptr->lsh_options |= OPTION_DC_BIT;
		db->db_lsa_ptr->lsh_seq = NEXTSEQ(lsa->lsh_seq);
		lsa_checksum(db);
		db->db_lsa_ptr->lsh_age = MAXAGE;
		db->db_freeme = TRUE;
		db->db_lsa_changed = TRUE;
		GET_TIMESTAMP(db->db_time);
		/* free it when flooding is done */
		add_to_flood_list(db, src_fl);
	    }
	    break;
	  case LSA_TYPE_SUM_ASB:
	  case LSA_TYPE_SUM_NET:
	  case LSA_TYPE_ASE:
	  case LSA_TYPE_7_ASE:
	    if (!found) {
		/*
		 * The old LSA may not have DC bit set
		 */
		db->db_lsa_ptr->lsh_options |= OPTION_DC_BIT;
		db->db_lsa_ptr->lsh_age = MAXAGE;
		db->db_freeme = TRUE;
	    } else {
		if (db->db_retrans) 
		    rem_db_retrans_list(db, NULL);
		if (!db->db_freeme) {
		    db->db_lsa_ptr->lsh_age = 0;
		    /* 
		     * take out our old lsa stats - we'll reissue the lsa 
		     */
		    ospf_dec_lsa_table(pdb, area, lsa->lsh_type, 
					db->db_lsa_ptr->lsh_chksum);
		} else {
		    /*
		     * Make sure it is MAXAGE, so the stupid
		     * ospf_insert_maxage() will not delete it 
		     * from MAXAGEQ mistakenly.
		     */
		    db->db_lsa_ptr->lsh_age = MAXAGE;
		}
	    }
	    GET_TIMESTAMP(db->db_time);
	    db->db_lsa_ptr->lsh_seq = NEXTSEQ(lsa->lsh_seq);
	    lsa_checksum(db);
	    db->db_lsa_changed = TRUE;
            if (!db->db_freeme) {
                /* 
		 * put in new lsa stats 
		 */
                ospf_inc_lsa_table(pdb, area, lsa->lsh_type, 
				    db->db_lsa_ptr->lsh_chksum);
	    }
	    if (lsa->lsh_type == LSA_TYPE_ASE) {
		db->db_pdbdist = MAXDISTANCE;
		add_to_flood_list(db, src_ex_fl);
		ospf_flood_acl_buginf("\nOSPF: Flushing old Ex Link State %i",
				      lsa->lsh_id);
	    } else {
		add_to_flood_list(db, src_fl);
	    }
	    break;
	} /* end of switch */
    } else { /* not self originated */
	if (!found) {
	    db->db_lsa_changed = TRUE;
	    PDB_INC_MIB_TABLE(pdb, RCNLSA_COUNT);
            /*
             * add the checksum and increment the count for the area
             * database statistics
             */
            ospf_inc_lsa_table(pdb, area, lsa->lsh_type, lsa->lsh_chksum);
	    if (!GET_DC_BIT(lsa->lsh_options)) {
		if (lsa->lsh_type == LSA_TYPE_ASE)
		    ospf_pdb->ospf_dcbitless_lsa_count++;
		else
		    area->ar_dcbitless_lsa_count++;
	    }
	    switch (lsa->lsh_type) {
	      case LSA_TYPE_RTR:
		spf_run |= SPF_SCHD_RTR;
		break;
	      case LSA_TYPE_NET:
		spf_run |= SPF_SCHD_NET;
		break;
	      case LSA_TYPE_SUM_ASB:
		/*
		 * if it is indication LSA, update the counter
		 */
		if (LSA_ID(db) == ADV_RTR(db))
		    area->ar_indication_lsa_count++;
	      case  LSA_TYPE_SUM_NET:
		ospf_spf_inter_acl_pass = 
		    acl_ospf_spf_inter_debug(lsa->lsh_id);
		ospf_schedule_partial_spf(area, db);
		ospf_spf_inter_acl_pass = FALSE;
		break;
	      case LSA_TYPE_ASE:
		/* new ase has come in - just add it */
		ospf_spf_ext_acl_pass =
		    acl_ospf_spf_ext_debug(lsa->lsh_id);
		ospf_schedule_partial_spf(area, db);
		ospf_spf_ext_acl_pass = FALSE;
		add_to_flood_list(db, ex_fl);
		break;
	      case LSA_TYPE_7_ASE:
		ospf_spf_ext_acl_pass =
		    acl_ospf_spf_ext_debug(lsa->lsh_id);
		ospf_schedule_partial_spf(area, db);
		ospf_spf_ext_acl_pass = FALSE;
		break;
	    }
	    if (do_not_age) {
		if (lsa->lsh_type == LSA_TYPE_ASE)
		    ospf_pdb->ospf_do_not_age_lsa_count++;
		else
		    area->ar_do_not_age_lsa_count++;
	    }
	} else { /* foundlsa */
	    switch (lsa->lsh_type) {
	        case LSA_TYPE_RTR:
	        case LSA_TYPE_NET:
		    ospf_spf_intra_acl_pass = 
			acl_ospf_spf_intra_debug(lsa->lsh_id);
		    break;
	        case LSA_TYPE_SUM_NET:
	        case LSA_TYPE_SUM_ASB:
		    ospf_spf_inter_acl_pass =
			acl_ospf_spf_inter_debug(lsa->lsh_id);
		    break;
	        case LSA_TYPE_ASE:
		case LSA_TYPE_7_ASE:
		    ospf_spf_ext_acl_pass =
			acl_ospf_spf_ext_debug(lsa->lsh_id);
	    }
            /*
             * remove the stats for our currently installed lsa, as it
             * is assumed that we will either replace it or remove it
             * in the case of MAXAGE.
             */
	    if (!db->db_freeme) {
                ospf_dec_lsa_table(pdb, area, lsa->lsh_type,
                                    db->db_lsa_ptr->lsh_chksum);
		if ((lsa->lsh_type == LSA_TYPE_SUM_ASB) && 
		    (LSA_ID(db) == ADV_RTR(db))) {
		    area->ar_indication_lsa_count--;
		}
		if (!GET_DC_BIT(db->db_lsa_ptr->lsh_options)) {
		    if (lsa->lsh_type == LSA_TYPE_ASE)
			ospf_pdb->ospf_dcbitless_lsa_count--;
		    else
			area->ar_dcbitless_lsa_count--;
		}
		if (db->db_do_not_age) {
		    if (lsa->lsh_type == LSA_TYPE_ASE)
			ospf_pdb->ospf_do_not_age_lsa_count--;
		    else
			area->ar_do_not_age_lsa_count--;
		}
	    }
	    /* if there is a retrans list free it */
	    if (db->db_retrans != NULL)
		rem_db_retrans_list(db, NULL);
	    /* see if they're the same - if not schedule spf */
	    /*
	     * Assume no change
	     */
	    db->db_lsa_changed = FALSE;
	    if (lsa_cmp(area, lsa, db)) {
		db->db_lsa_changed = TRUE;
		spf_run |= SPF_SCHD_BIT(lsa->lsh_type);
 		ospf_spf_acl_buginf("\nOSPF: Detect change in LSA"
				    " type %d, LSID %i, from %i area %s",
				    lsa->lsh_type, lsa->lsh_id, 
				    lsa->lsh_adv_rtr, area->ar_id_str);
		PDB_INC_MIB_TABLE(pdb, RCNLSA_COUNT);
	    }
	    /* if new one is no longer maxage, remove free flag */
	    if ((ADV_AGE(db) >= MAXAGE) && (lsa->lsh_age <MAXAGE)) {
		db->db_freeme = FALSE;
		ospf_spf_acl_buginf("\nOSPF: SPF due to NON-MAXAGE in "
				    "lsa %d from %i",
				    lsa->lsh_type, lsa->lsh_adv_rtr);
		spf_run |= SPF_SCHD_BIT(lsa->lsh_type);
	    } else { /* 
		      * our copy is not maxage and neither is the new one  or 
		      *	our copy is not maxage and the new one is  or
	              * our copy is maxage & so is the new one 
		      */
		if (lsa->lsh_age >= MAXAGE) {
		    db->db_freeme = TRUE;
		    spf_run |= SPF_SCHD_BIT(lsa->lsh_type);
 		    if (spf_run & (SPF_SCHD_RTR | SPF_SCHD_NET))
 			ospf_spf_acl_buginf("\nOSPF: Detect MAXAGE "
					    "in LSA type %d, LS ID %i, "
					    "from %i",
					    lsa->lsh_type, lsa->lsh_id,
					    lsa->lsh_adv_rtr);
		}
	    }
            /*
             * stick the new lsa info into the database and increment
             * the stats.
             */
	    add_lsa(db, lsa->lsh_id, lsa->lsh_adv_rtr, lsa);
	    if (!db->db_freeme) {
                ospf_inc_lsa_table(pdb, area, lsa->lsh_type,
                                    db->db_lsa_ptr->lsh_chksum);
		if (do_not_age) {
		    if (lsa->lsh_type == LSA_TYPE_ASE)
			ospf_pdb->ospf_do_not_age_lsa_count++;
		    else
			area->ar_do_not_age_lsa_count++;
		}
		if (!GET_DC_BIT(lsa->lsh_options)) {
		    if (lsa->lsh_type == LSA_TYPE_ASE)
			ospf_pdb->ospf_dcbitless_lsa_count++;
		    else
			area->ar_dcbitless_lsa_count++;
		}
		if ((lsa->lsh_type == LSA_TYPE_SUM_ASB) && 
		    (LSA_ID(db) == ADV_RTR(db))) {
		    area->ar_indication_lsa_count++;
		}
	    }
	    GET_TIMESTAMP(db->db_time);
            delete_route_old_mask(pdb, area, db);
	    switch (lsa->lsh_type) {
	      case LSA_TYPE_RTR:
	      case LSA_TYPE_NET:
		ospf_spf_intra_acl_pass = FALSE;
		break;
	      case LSA_TYPE_SUM_NET:
		if (spf_run & SPF_SCHD_SNET) {
		    ospf_schedule_partial_spf(area, db);
		    spf_run = 0;
		}
		ospf_spf_inter_acl_pass = FALSE;
		break;
	      case LSA_TYPE_SUM_ASB:
		if (spf_run & SPF_SCHD_ASB) {
		    ospf_schedule_partial_spf(area, db);
		    spf_run = 0;
		}
		ospf_spf_inter_acl_pass = FALSE;
		break;
	      case LSA_TYPE_ASE:
		if (spf_run & SPF_SCHD_ASE) {
		    ospf_schedule_partial_spf(area, db);
		    spf_run = 0;
		}
		ospf_spf_ext_acl_pass = FALSE;
		add_to_flood_list(db, ex_fl);
		break;
	      case LSA_TYPE_7_ASE:
		if (spf_run & SPF_SCHD_7_ASE) {
		    ospf_schedule_partial_spf(area, db);
		    spf_run = 0;
		}
		ospf_spf_ext_acl_pass = FALSE;
	    }
	}
	if (lsa->lsh_type != LSA_TYPE_ASE)
	    add_to_flood_list(db, fl);
    }
    /*
     * Flush DoNotAge LSA if we have got DCbitless LSA
     * and check if we should originate or remove indication LSA.
     */
    if ((old_dcbitless_lsa_count == 0) &&
	(area->ar_dcbitless_lsa_count > 0)) {
	pdb->ospf_flag |= OSPF_FLUSH_DO_NOT_AGE;
    } else {
	if (old_indication_lsa_count != area->ar_indication_lsa_count) {
	    pdb->ospf_flag |= OSPF_CHECK_INDICATION;
	} else if (old_non_indication_lsa_count == 0) {
	    if (NON_INDICATION_LSA_COUNT(area) > 0)
		pdb->ospf_flag |= OSPF_CHECK_INDICATION;
	} else {
	    if (NON_INDICATION_LSA_COUNT(area) == 0)
		pdb->ospf_flag |= OSPF_CHECK_INDICATION;
	}
    }
    if (db) {
	/* 
	 * db can be NULL as we do not install our rtr lsa from
	 * other even we don't have one.
	 */
	db->db_do_not_age = do_not_age;
	ospf_insert_maxage(pdb, db);
	if (spf_run)
	    spf(area, spf_run, LSA_ID(db));
    } else {
	if (spf_run)
	    spf(area, spf_run, lsa->lsh_id);
    }
} /* end of db_install */


/*
 * ar_age_lsa
 * age a link state and free
 * it if necessary.
 */
static ushort ar_age_lsa (
    areatype *area,
    ushort type,
    list_queue *flood_list)
{
    lsdbtype *loop_db, *db, *adv_rtr_db;
    pdbtype *pdb;
    ushort spf_schd;
    ushort chksum;
    ulong adv_rtr;
    int count = 0;
    boolean flush;

    spf_schd = 0;
    pdb = area->ar_pdb_ptr;

    loop_db = db_get_first_db(area->ar_lsdbase[type]);
    while (loop_db) {
        db = loop_db;
        loop_db = db_get_next_db(loop_db);
	if (db->db_lsa_ptr == NULL)
	    continue;
	check_ospf_suspend_limit(area->ar_pdb_ptr, DBASE_ACTIVE, &count);
	flush = FALSE;
	if (db->db_do_not_age) {
	    /*
	     * For DoNotAge LSA, only flush if the adv rtr is not
	     * reachable for MAXAGE.
	     */
	    adv_rtr = ADV_RTR(db);
	    adv_rtr_db = db_find_lsa(area, adv_rtr, adv_rtr, LSA_TYPE_RTR);
	    if (adv_rtr_db && adv_rtr_db->db_rtr_reachable)
		GET_TIMESTAMP(db->db_time);
	    if (DB_ELAPSE(db) >= MAXAGE)
		flush = TRUE;
	} else if (ADV_AGE(db) >= MAXAGE) {
	    flush = TRUE;
	}
	if (flush) {
	    if (!db->db_freeme) {
		/* Currently active route? */
		if (db->db_rt_table) {
		    spf_schd |= SPF_SCHD_BIT(type);
		}
		db->db_lsa_ptr->lsh_age = MAXAGE;
		db->db_lsa_changed = TRUE;
		GET_TIMESTAMP(db->db_time);
		db->db_freeme = TRUE;
		ospf_dec_lsa_table(pdb, area, type, db->db_lsa_ptr->lsh_chksum);
		if (type == LSA_TYPE_ASE) {
		    if (db->db_do_not_age)
			pdb->ospf_pdb->ospf_do_not_age_lsa_count--;
		    if (!GET_DC_BIT(db->db_lsa_ptr->lsh_options))
			pdb->ospf_pdb->ospf_dcbitless_lsa_count--;
		} else {
		    if (db->db_do_not_age)
			area->ar_do_not_age_lsa_count--;
		    if (!GET_DC_BIT(db->db_lsa_ptr->lsh_options))
			area->ar_dcbitless_lsa_count--;
		}
		if ((db->db_lsa_ptr->lsh_type == LSA_TYPE_SUM_ASB) && 
		    (LSA_ID(db) == ADV_RTR(db))) {
		    area->ar_indication_lsa_count--;
		}
		db->db_do_not_age = FALSE;
		/*
		 * Add to the flood list if 
		 *   - the area is not a stub area  OR
		 *   - if area is stub, the link state type is not type
		 *     4 or 5.
		 */
		if (!(area->ar_stub) || 
		     ((LSA_TYPE(db) != LSA_TYPE_ASE)
		      && (LSA_TYPE(db) != LSA_TYPE_SUM_ASB)) ) {
		    add_to_flood_list(db, flood_list);
		}
		ospf_insert_maxage(pdb, db);
	    }
	    continue;
	}
	chksum = db->db_lsa_ptr->lsh_chksum;
	lsa_checksum(db);
	if (chksum != db->db_lsa_ptr->lsh_chksum) {
	    buginf("OSPF: Checksum Failure in database in area %s\n",
		   area->ar_id_str);
	    buginf(" LSID %i ADV RTR %i Type %d Old Checksum %#x "
		   "New Checksum %#x\n",
		   LSA_ID(db), ADV_RTR(db), LSA_TYPE(db), chksum, 
		   db->db_lsa_ptr->lsh_chksum);
	}
    }
    return(spf_schd);
} /* end of ar_age_lsa */


/* 
 * ar_age_dbase:
 */
void ar_age_dbase (areatype *area)
{
    list_queue	flood_list;
    list_queue	ex_list;
    ushort	spfsched;
    boolean	old_non_indication_lsa_count, old_indication_lsa_count;

    ospf_flood_buginf("\nOSPF: Aging Link States in Area %s",area->ar_id_str);

    flood_list.q_first = NULL;
    flood_list.q_last = NULL;
    ex_list.q_first = NULL;
    ex_list.q_last = NULL;
    old_non_indication_lsa_count = NON_INDICATION_LSA_COUNT(area);
    old_indication_lsa_count = area->ar_indication_lsa_count;
    spfsched = 0;
    spfsched |= ar_age_lsa(area, LSA_TYPE_RTR, &flood_list);
    spfsched |= ar_age_lsa(area, LSA_TYPE_NET, &flood_list);
    check_ospf_suspend(area->ar_pdb_ptr, DBASE_ACTIVE);
    spfsched |= ar_age_lsa(area, LSA_TYPE_SUM_NET, &flood_list);
    spfsched |= ar_age_lsa(area, LSA_TYPE_SUM_ASB, &flood_list);
    spfsched |= ar_age_lsa(area, LSA_TYPE_7_ASE, &flood_list);
    /* flood Maxage Link States */
    flood_area(area, &flood_list, NULL, NULL);
    if ((area->ar_pdb_ptr->ospf_area_brtr == FALSE)
	|| ((area->ar_pdb_ptr->ospf_area_brtr) &&
	              (area->ar_id == BACKBONE_AREA_ID))) {
	spfsched |= ar_age_lsa(area, LSA_TYPE_ASE, &ex_list);
	check_ospf_suspend(area->ar_pdb_ptr, DBASE_ACTIVE);
	ex_flood_area(area->ar_pdb_ptr, &ex_list, NULL, NULL);
    }
    if ((old_non_indication_lsa_count > 0) &&
	(NON_INDICATION_LSA_COUNT(area) == 0)) {
	/*
	 * Non-indication DCbitless LSA become zero,
	 * withdraw indication LSA.
	 */
	area->ar_pdb_ptr->ospf_flag |= OSPF_CHECK_INDICATION;
    } else if (old_indication_lsa_count != area->ar_indication_lsa_count) {
	area->ar_pdb_ptr->ospf_flag |= OSPF_CHECK_INDICATION;
    }
    /* something with a valid route has aged out.
     * this looks ugly and expensive but will only happen when
     * many areas are sending out valid LSAs that are almost
     * at MaxAge (shouldn't even happen) 
     */
    if (spfsched)
	spf(area, spfsched, 0);
    mgd_timer_start(&area->ar_age_timer, AGE_TIME * ONESEC);
} /* end of ar_age_dbase */

/* end of ospf_dbase.c */

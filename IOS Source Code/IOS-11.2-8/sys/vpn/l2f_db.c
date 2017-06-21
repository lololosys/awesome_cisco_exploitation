/* $Id: l2f_db.c,v 1.2.40.2 1996/04/27 06:34:17 syiu Exp $
 * $Source: /release/112/cvs/Xsys/vpn/l2f_db.c,v $
 *------------------------------------------------------------------
 * l2f_db.c  --  slow to store, fast to retrieve database of MIDs
 *
 * January 1996, Tim Kolar
 *
 * Copyright (c) 1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: l2f_db.c,v $
 * Revision 1.2.40.2  1996/04/27  06:34:17  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 1.2.2.2  1996/04/25  23:56:42  syiu
 * Branch: Dial1_branch
 * Dial1_branch to IosSec_postmerge_960419
 *
 * Revision 1.2.2.1  1996/03/25  05:32:12  tkolar
 * Branch: Dial1_branch
 * Enable Tunnel authentication.
 *
 * Revision 1.2.40.1  1996/03/18  22:46:10  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 1.2.14.1  1996/03/07  11:06:11  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 1.2  1996/01/31  18:38:36  tkolar
 * Branch: Dial1_branch
 * Get all of the VPN code into the repository, with the right names
 * and in the right places.
 *
 * Revision 1.1  1996/01/29  18:09:10  tkolar
 * Populate the VPN directory.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "types.h"
#include "config.h"
#include "../util/md5.h"
#include "l2f.h"
#include "vpn_debug_flags.h"

struct l2fmid_elem_ {
    void *next;
    int mid;
    int clid;
    l2f_midtype *midptr;
};

typedef struct l2fmid_elem_ l2fmid_elem;
queuetype l2f_tunnelQ;
queuetype l2f_midstorageQ;

/* ---------------------------------------------------------------- */
/*                     MID/CID/IDB storage functions                */
/*              These use a really inefficient lookup method.       */
/* ---------------------------------------------------------------- */

/*
 * l2f_init_mid_storage
 */
void l2f_init_mid_storage(void)
{
    queue_init(&l2f_midstorageQ, 0);
}

/*
 * l2f_get_mid
 */
l2f_midtype *l2f_get_mid(int clid, int mid)
{
    int queue_size, count;
    l2fmid_elem *elem;

    queue_size = QUEUESIZE(&l2f_midstorageQ);
    for (count = 0; count < queue_size; count++) {
	elem = dequeue(&l2f_midstorageQ);
	enqueue(&l2f_midstorageQ, elem);
	if ((elem->mid == mid) && (elem->clid == clid)) {
	    return(elem->midptr);
	}
    }

    return(NULL);
}

/*
 * l2f_get_mid_by_idb
 *
 * Get the MID based on the hwidb
 */
l2f_midtype *l2f_get_mid_by_idb (hwidbtype *hwidb) 
{
    l2f_tunneltype *tunnel;
    l2f_midtype *mid;
    int tunnel_size, mid_size;

    tunnel_size = QUEUESIZE(&l2f_tunnelQ);
    tunnel = l2f_getnext_tunnel();
    while ((tunnel_size > 0) && tunnel) {
        mid_size = QUEUESIZE(&tunnel->tunnel_mid_elemQ);
        mid = l2f_getnext_mid_from_tunnel(tunnel);
        while ((mid_size > 0) && mid) {
            if (mid->idb == hwidb)
               return(mid);
	    mid = l2f_getnext_mid_from_tunnel(tunnel);
            mid_size--;
        }
        tunnel = l2f_getnext_tunnel();
        tunnel_size--;        
    }
    return(NULL);
}

/*
 * l2f_store_mid
 */
boolean l2f_store_mid (int clid, int mid, l2f_midtype *midptr)
{
    l2fmid_elem *newelem;

    newelem = malloc(sizeof(l2fmid_elem));
    if (newelem == NULL) {
	return(FALSE);
    }
    newelem->clid = clid;
    newelem->mid = mid;
    newelem->midptr = midptr;
    enqueue(&l2f_midstorageQ, newelem);
    return(TRUE);
}

/*
 * l2f_remove_mid
 */
void l2f_remove_mid(int clid, int mid)
{
    int queue_size, count;
    l2fmid_elem *elem;

    queue_size = QUEUESIZE(&l2f_midstorageQ);
    for (count = 0; count < queue_size; count++) {
	elem = dequeue(&l2f_midstorageQ);
	if ((elem->mid == mid) && (elem->clid == clid)) {
	    free(elem);
	    return;
	}
	enqueue(&l2f_midstorageQ, elem);
    }
    return;
}

/*
 * l2f_tunnel_exists
 *
 * Check if the tunnel exists 
 */
boolean l2f_tunnel_exists (l2f_tunneltype *tunnel)
{
    leveltype level;
    l2f_tunneltype *next_tunnel;

    level = raise_interrupt_level(NETS_DISABLE);

    next_tunnel = l2f_tunnelQ.qhead;
    while (next_tunnel) {
       if (next_tunnel == tunnel) {
          reset_interrupt_level(level);
          return(TRUE);
       }
       next_tunnel = next_tunnel->next;
    }	   

    reset_interrupt_level(level);
    return(FALSE);
}

/*
 * l2f_store_tunnel
 *
 * Store the Tunnel in the Tunnel Queue
 */
boolean l2f_store_tunnel (l2f_tunneltype *tunnel)
{
    if (!tunnel)
       return(FALSE);

    /*
     * If there is one already there, do not store
     */
    if (l2f_tunnel_exists(tunnel)) {
       if (l2f_error_debug)
          buginf("\nL2F Tunnel already inserted NAS/GW CLID %d/%d", 
                 tunnel->nas_clid, tunnel->gw_clid);
       return(FALSE);
    }

    /*
     * Store in the Tunnel queue
     */
    p_enqueue(&l2f_tunnelQ, tunnel);
    return(TRUE);
}

/*
 * l2f_find_tunnel_by_clid
 *
 *    If we are the nas, look up the tunnel by the clid that we assigned 
 *     to the gateway.  If we are the gateway, look up the tunnel by the clid
 *     that we assigned to the nas.
 *
 *    The CLIDs we hand out are unique for both the NAS calling us, and 
 *      us calling a GW, so this lookup is valid.
 */
l2f_tunneltype *l2f_find_tunnel_by_clid (int clid)
{
    leveltype level;
    l2f_tunneltype *next_tunnel;

    level = raise_interrupt_level(NETS_DISABLE);

    next_tunnel = l2f_tunnelQ.qhead;
    while (next_tunnel) {
       if (next_tunnel->instigator && (next_tunnel->gw_clid == clid)) {
            reset_interrupt_level(level);
  	    return(next_tunnel);
       } else if (next_tunnel->nas_clid == clid) {
            reset_interrupt_level(level);
	    return(next_tunnel);
       }
       next_tunnel = next_tunnel->next;
    }	   

    reset_interrupt_level(level);
    return(NULL);

}

/*
 * l2f_find_active_tunnel_by_name
 *
 * Find Active tunnel by Name. On a NAS, the name is the name of the HG.
 * On a HG, it is the name of the NAS.
 */
l2f_tunneltype *l2f_find_active_tunnel_by_name (char *name)
{
    leveltype level;
    l2f_tunneltype *next_tunnel;
  
    level = raise_interrupt_level(NETS_DISABLE);

    next_tunnel = l2f_tunnelQ.qhead;
    while (next_tunnel) {
       if (!next_tunnel->instigator) {
	    if (!strcmp(name, next_tunnel->nas_name)) {
               reset_interrupt_level(level);
	       return(next_tunnel);
	    }
       } else {
	    if (!strcmp(name, next_tunnel->gw_domain)) {
               reset_interrupt_level(level);
	       return(next_tunnel);
	    }
       }
       next_tunnel = next_tunnel->next;
    }

    reset_interrupt_level(level);
    return(NULL);
}

/*
 * l2f_find_active_tunnel_by_names
 *
 * Find the active tunnels by NAS and GW names
 */ 
l2f_tunneltype *l2f_find_active_tunnel_by_names (char *nas, char *gw)
{
    leveltype level;
    l2f_tunneltype *next_tunnel;
  
    level = raise_interrupt_level(NETS_DISABLE);

    next_tunnel = l2f_tunnelQ.qhead;
    while (next_tunnel) {
       if (!strcmp(nas, next_tunnel->nas_name) &&
           !strcmp(gw, next_tunnel->gw_name)) {
               reset_interrupt_level(level);
	       return(next_tunnel);
       }
       next_tunnel = next_tunnel->next;
    }

    reset_interrupt_level(level);
    return(NULL);
}

/*
 * l2f_find_active_tunnel_by_address
 *
 *   This is for looking up tunnels. We must be the instigator
 *
 *   This is only used for stackability, where all of the names are
 *    identical and all of the IP addresses are different.
 */ 
l2f_tunneltype *l2f_find_active_tunnel_by_address (ipaddrtype
							  ip_address)
{
    leveltype level;
    l2f_tunneltype *next_tunnel;

    level = raise_interrupt_level(NETS_DISABLE);

    next_tunnel = l2f_tunnelQ.qhead;
    while (next_tunnel) {
       if (next_tunnel->instigator && 
           (next_tunnel->ip_address == ip_address) &&
           (next_tunnel->state != L2FTUNNEL_CLOSED) && 
           (next_tunnel->state != L2FTUNNEL_CLOSING)) {
           reset_interrupt_level(level);
           return(next_tunnel);
       }
       next_tunnel = next_tunnel->next;
    }
   
    reset_interrupt_level(level);
    return(NULL);
}

/*
 * l2f_destroy_tunnel_structure
 */
boolean l2f_destroy_tunnel_structure (l2f_tunneltype *target_tunnel)
{
    l2f_tunneltype *tunnel, *prev_tunnel;
    leveltype level;

    level = raise_interrupt_level(NETS_DISABLE);

    tunnel = l2f_tunnelQ.qhead;
    prev_tunnel = NULL;

    while (tunnel) {
      if (tunnel == target_tunnel) {
          remqueue(&l2f_tunnelQ, tunnel, prev_tunnel);
          free(tunnel);
          reset_interrupt_level(level);
          return(TRUE);
      }
      prev_tunnel = tunnel;
      tunnel = tunnel->next;
    }	   
    reset_interrupt_level(level);
    return(FALSE);
}

/*
 * l2f_getnext_tunnel
 *
 * Get the next tunnel. 
 * Dequeue from the head and enqueue to the tail again, ad infinitum 
 * The caller should cycle through the queuesize or some other check
 * to make sure it knows when to stop this. 
 */
l2f_tunneltype *l2f_getnext_tunnel (void)
{
   l2f_tunneltype *tunnel;
   tunnel = p_dequeue(&l2f_tunnelQ);
   if (tunnel)
      p_enqueue(&l2f_tunnelQ, tunnel);
   return(tunnel);
}

/*
 * l2f_tunnel_mid_elem_exists
 *
 * Check if Mid element exists on tunnel for the given mid
 */
boolean l2f_tunnel_mid_elem_exists (l2f_tunneltype *tunnel, l2f_midtype *mid)
{
    leveltype level;
    l2f_mid_elem_type *next_midelem;

    level = raise_interrupt_level(NETS_DISABLE);

    next_midelem = tunnel->tunnel_mid_elemQ.qhead;
    while (next_midelem) {
       if (next_midelem->mid == mid) {
          reset_interrupt_level(level);
          return(TRUE);
       }
       next_midelem = next_midelem->next;
    }	   

    reset_interrupt_level(level);
    return(FALSE);
}

/*
 * l2f_store_tunnel_mid_elem
 *
 * Queue the mid in the tunnel MID queue. Do it by way of another
 * queue - MID element - which points to the mid
 */
boolean l2f_store_tunnel_mid_elem (l2f_tunneltype *tunnel, l2f_midtype *mid)
{
    l2f_mid_elem_type *mid_elem;

    if (!(tunnel && mid))
       return(FALSE);

    /*
     * If there is one already there, do not store
     */
    if (l2f_tunnel_mid_elem_exists(tunnel, mid)) {
       if (l2f_error_debug)
          buginf("\nL2F Tunnel already has MID=%d", mid->mid);
       return(FALSE);
    }

    mid_elem = malloc_named(sizeof(l2f_mid_elem_type), "l2f mid element");
    if (!mid_elem) {
	return(FALSE);
    }

    mid_elem->mid = mid;

    /*
     * Store in the Tunnel MID queue
     */
    p_enqueue(&tunnel->tunnel_mid_elemQ, mid_elem);
    return(TRUE);
}

/*
 * l2f_getnext_mid_from_tunnel
 *
 * Get the next mid from the tunnel
 */
l2f_midtype *l2f_getnext_mid_from_tunnel (l2f_tunneltype *tunnel)
{
   l2f_mid_elem_type *mid_elem;
   
   mid_elem = p_dequeue(&tunnel->tunnel_mid_elemQ);
   if (mid_elem) {
      p_enqueue(&tunnel->tunnel_mid_elemQ, mid_elem);
      return(mid_elem->mid);
   }
   return(NULL);
}

/*
 * l2f_remove_mid_elem_from_tunnel
 *
 * Remove the mid element from the tunnel
 */
boolean l2f_remove_mid_elem_from_tunnel (l2f_tunneltype *tunnel, l2f_midtype *mid)
{
    l2f_mid_elem_type *mid_elem, *prev_mid_elem;
    leveltype level;

    level = raise_interrupt_level(NETS_DISABLE);

    mid_elem = tunnel->tunnel_mid_elemQ.qhead;
    prev_mid_elem = NULL;

    while (mid_elem) {
      if (mid_elem->mid == mid) {
          remqueue(&tunnel->tunnel_mid_elemQ, mid_elem, prev_mid_elem);
          free(mid_elem);
          reset_interrupt_level(level);
          return(TRUE);
      }
      prev_mid_elem = mid_elem;
      mid_elem = mid_elem->next;
    }	   
    reset_interrupt_level(level);
    return(FALSE);
}

/*
 * l2f_remove_mids_from_tunnel
 *
 * Remove all the MID elements AND the corresponding mids 
 * from the tunnel
 */
void l2f_remove_mids_from_tunnel (l2f_tunneltype *tunnel)
{
    l2f_mid_elem_type *mid_elem;

    mid_elem = p_dequeue(&tunnel->tunnel_mid_elemQ);
    while (mid_elem) {
          if (mid_elem->mid)
             l2f_destroy_mid_structure(mid_elem->mid, TRUE);
          free(mid_elem);
          mid_elem = p_dequeue(&tunnel->tunnel_mid_elemQ);
    }
}

/*
 * l2f_lock_mids_in_tunnel
 *
 * Lock all MID elements within a tunnel to collect tunnel and MIDs
 * information.
 * Must call l2f_unlock_mids_in_tunnel() to undo mem_lock() later.
 *
 * Note: Tunnel will always be locked even without MID element.
 *	 Also saved additional mem_lock().
 */
struct l2f_midlock_info *l2f_lock_mids_in_tunnel (l2f_tunneltype *tunnel,
    int *mid_size)
{
    int i,size;
    l2f_midtype *mid;
    l2f_mid_elem_type *mid_elem;
    struct l2f_midlock_info *mid_lock, *mid_locks;

    if (mid_size)
	*mid_size = 0;
    if (!tunnel || !mid_size)
	return(NULL);

    mem_lock(tunnel);				/* mem_lock() on tunnel */
    size = QUEUESIZE(&tunnel->tunnel_mid_elemQ);
    if (!size)
	return(NULL);

    mid_lock = mid_locks = malloc(size * sizeof(struct l2f_midlock_info));
    if (mid_locks) {
	*mid_size = size;
	mid_elem = tunnel->tunnel_mid_elemQ.qhead;
	for (i = 0; mid_elem && i < size; i++, mid_lock++) {
	    mid = mid_lock->mid = mid_elem->mid;
	    mem_lock(mid);			/* mem_lock() on MID */
	    mid_lock->name = mid->name;		/* save it for unlock */
	    if (mid_lock->name) {
		mem_lock(mid_lock->name);	/* additional mem_lock() */
	    }
	    mid_lock->owner = mid->owner;	/* save it for unlock */
	    if (mid_lock->owner) {
		mem_lock(mid_lock->owner);	/* additional mem_lock() */
	    }
	    mid_elem = mid_elem->next;
	}
    }

    return(mid_locks);
}

/*
 * l2f_unlock_mids_in_tunnel
 *
 * Undo the effect of l2f_lock_mids_in_tunnel().
 *
 * Note: Added additional undo of mem_lock().
 */
void l2f_unlock_mids_in_tunnel (l2f_tunneltype *tunnel,
    struct l2f_midlock_info *mid_locks, int mid_size)
{
    int i;
    l2f_midtype *mid;
    struct l2f_midlock_info *mid_lock;

    if (mid_locks) {
	for (i = 0, mid_lock = mid_locks; i < mid_size; i++, mid_lock++) {
	    mid = mid_lock->mid;
	    if (mid) {
		if (mid_lock->name) {
		    free(mid_lock->name);	/* additional undo mem_lock() */
		}
		if (mid_lock->owner) {
		    free(mid_lock->owner);	/* additional undo mem_lock() */
		}
		free(mid);			/* undo mem_lock() */
	    }
	}
	free(mid_locks);
    }

    if (tunnel)
	free(tunnel);				/* undo mem_lock() */
}

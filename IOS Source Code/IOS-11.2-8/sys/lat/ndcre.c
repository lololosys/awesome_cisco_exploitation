/* $Id: ndcre.c,v 3.2 1995/11/17 17:34:42 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/lat/ndcre.c,v $
 *------------------------------------------------------------------
 * $Log: ndcre.c,v $
 * Revision 3.2  1995/11/17  17:34:42  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:27:36  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:26:56  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*-----------------------------------------------------------------------
    nd_create		(i) create node structure
    
    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO
    
    This material is confidential information of Meridian Technology
    Corporation. Furnished under license. All rights reserved. 
 -----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------
    This function finds or creates the named the named remote node
    structure. If an existing structure is found, its unknown or
    unreachable status is cleared (calling this routine is makes the
    node known).

    CALL BY:	nd_ptr nd_create(name,src)
		byte	*name;
		word	src[3];
 
    ON EXIT:	Return a pointer to the existing or newly created node
		structure. Return zero if no node can be allocated.

	ND_ERROR | MAX_LIMIT	Reach maximum limit of remote nodes
	ND_ERROR | NO_MEM	Remote node structure allocation failed.
 -----------------------------------------------------------------------*/
#include "mtcsys.h"
#include "laths.h"
#include "latnd.h"
#include "laterr.h"
#include "latfnc.h"
#include "sysfnc.h"

extern hs_ptr	hs;			/* Local node definitions	*/
extern int	lat_error;		/* LAT error code		*/
extern int	le_nd_retain;		/* Retain remote node support	*/

nd_ptr nd_create(name,src,idb)
    byte	*name;			/* Name of node to create	*/
    word	src[3];			/* Ethernet source address	*/
    idbtype     *idb;			/* Source ethernet interface    */
{
    nd_ptr	nd;			/* Pointer to ND structure	*/
    int		hash;			/* Index into hash table	*/
    int		i;			/* Array index			*/

/*-----------------------------------------------------------------------
    Lookup the node name to see if it already exists.  If so, return
    that node pointer.
 -----------------------------------------------------------------------*/
    if ((nd = nd_show(name,idb)) != 0)
    {
	nd->nd_flags &= ~(ND_UNKNOWN | ND_UNREACH);
	nd->nd_timer  = LS_MC_DEF;
    }    

/*-----------------------------------------------------------------------
    There is no match on node name, try to create a new node structure.
    If normal allocation fails, purge out the oldest node structures.
    First, test to assure that supplied Ethernet address in not multi-
    cast.
 -----------------------------------------------------------------------*/
    else
    {
    	if (src[0] == hs->hs_multicast[0])
	{
	    nd = 0;
	}
        else if ((hs->hs_nd_maximum > 0 && hs->hs_nd_current >= hs->hs_nd_maximum) ||
	    (nd = nd_alloc()) == 0)
	{
#if LE_ND_RETAIN
	    if (le_nd_retain)
	    {
		for (i = -hs->hs_nd_age_max; i < LS_MC_DEF; i += hs->hs_nd_interval)
		{
		    if (nd_purge(i))
		    {
			if ((hs->hs_nd_maximum == 0 || hs->hs_nd_current < hs->hs_nd_maximum) &&
			    (nd = nd_alloc()) != 0)
			{
			    break;
			}
		    }
		}
	    }
#endif
	}

/*-----------------------------------------------------------------------
    If no node was allocated, determine specific error code.
 -----------------------------------------------------------------------*/
	if (nd == 0)
	{
	    if (LOBYTE(src[0]) & 0x1)
	    	lat_error = ND_ERROR | INV_ARG;
            else if (hs->hs_nd_maximum > 0 && hs->hs_nd_current >= hs->hs_nd_maximum)
	        lat_error = ND_ERROR | MAX_LIMIT;
	    else
	        lat_error = ND_ERROR | NO_MEM;
	}

/*-----------------------------------------------------------------------
    If a new node was created insert it into the node linked list.
 -----------------------------------------------------------------------*/
	else
	{
	    if (++hs->hs_nd_current > hs->hs_nd_highest)
	        hs->hs_nd_highest++;
		
	    for (i = name[0], hash = 0; i != 0; i--)
		hash += name[i];
	    hash = nd_hash(hash);

            nd->nd_link[0] = 0;
	    nd->nd_link[1] = hs->hs_nd_hash[hash][1];
	    hs->hs_nd_hash[hash][1] = nd;
	    if (nd->nd_link[1] != 0)
	        nd->nd_link[1]->nd_link[0] = nd;
	    else
	        hs->hs_nd_hash[hash][0] = nd;

/*-----------------------------------------------------------------------
    Fill in the new node structure.
 -----------------------------------------------------------------------*/
	    nd->nd_ls[0]     = 0;
	    nd->nd_ls[1]     = 0;
	    nd->nd_host_addr[0] = src[0];
	    nd->nd_host_addr[1] = src[1];
	    nd->nd_host_addr[2] = src[2];
	    nd->nd_idb       = idb;
	    nd->nd_timer     = LS_MC_DEF;
            nd->nd_usage     = -1;
	    nd->nd_flags     = ND_INIT;
	    nd->nd_dll_max   = 0;
	    nd->nd_seq       = 0;
	    nd->nd_changes   = 0;
	    
	    nd->nd_acl[0]    = 0;
	    nd->nd_ident[0]  = 0;
	    nd_str_move_sl(&nd->nd_node[0],name);

	    nd->nd_prot_hi   = 0;
	    nd->nd_prot_lo   = 0;
	    nd->nd_prot_ver  = 0;
	    nd->nd_prot_eco  = 0;

#if LE_ND_STATS
	    nd->nd_facility  = 0;
	    nd->nd_prod_code = 0;
	    nd->nd_prod_ver  = 0;
	    nd_zero(nd);
#endif

	}
    }
    return(nd);
}

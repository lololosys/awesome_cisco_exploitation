/*-----------------------------------------------------------------------
    nd_delete		(i) delete node structure
    
    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO
    
    This material is confidential information of Meridian Technology
    Corporation. Furnished under license. All rights reserved. 
 -----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------
    This function deletes a remote node structure. It assumes there
    are no active sessions or learned services associcated with the
    structure. If the remote node hold list is at a deficit, the nd
    structure is placed into it.

    CALL BY:	void nd_delete(nd)
		nd_ptr	nd;
 
    ON EXIT:	Node structure deleted or put into hold list.
 -----------------------------------------------------------------------*/
#include "mtcsys.h"
#include "laths.h"
#include "latnd.h"
#include "latfnc.h"
#include "sysfnc.h"

extern hs_ptr	hs;			/* Local node definitions	*/
extern nd_ptr	nd_hold[2];		/* ND structure hold list	*/
extern int	nd_hold_num;		/* Number remote nodes held	*/
extern int	ls_hold_num;		/* Number services held		*/

void nd_delete(nd)

    nd_ptr	nd;			/* Pointer to ND structure	*/
{
    int		hash;			/* Index into hash table	*/
    int		i;			/* Array index			*/

/*-----------------------------------------------------------------------
    Check to see if the node is in the hold list.  If it is not, the
    structure must be unlinked from the node hash table.
 -----------------------------------------------------------------------*/
    if (!(nd->nd_flags & ND_HOLD))
    {
        for (i = nd->nd_node[0], hash = 0; i != 0; i--)
	    hash += nd->nd_node[i];  
        hash = nd_hash(hash);

	if (nd->nd_link[0] != 0)
	    nd->nd_link[0]->nd_link[1] = nd->nd_link[1];
	else
	    hs->hs_nd_hash[hash][1] = nd->nd_link[1];
	
	if (nd->nd_link[1] != 0)
	    nd->nd_link[1]->nd_link[0] = nd->nd_link[0];
	else
	    hs->hs_nd_hash[hash][0] = nd->nd_link[0];

/*-----------------------------------------------------------------------
    If the node is marked permanent, add it to the node hold list and mark
    as such.
 -----------------------------------------------------------------------*/
	if (ls_hold_num > nd_hold_num)
	{
	    nd_hold_num++;
	    
	    nd->nd_link[0] = 0;
	    nd->nd_link[1] = nd_hold[1];
	    nd_hold[1] = nd;
	    if (nd->nd_link[1] != 0)
	        nd->nd_link[1]->nd_link[0] = nd;
	    else
		nd_hold[0] = nd;

	    nd->nd_flags |= ND_HOLD;
	}

/*-----------------------------------------------------------------------
    Otherwise, we free the node to dynamic memory.    
 -----------------------------------------------------------------------*/
	else 
	{
	    hs->hs_nd_current--;
	    nd_free(nd);
	}
    }

/*-----------------------------------------------------------------------
    Executing this else implies that the node is in the node hold list.
    Remove from list and deallocate.
 -----------------------------------------------------------------------*/
    else 
    {
        nd_hold_num--;
	
	if (nd->nd_link[0] != 0)
	    nd->nd_link[0]->nd_link[1] = nd->nd_link[1];
	else
	    nd_hold[1] = nd->nd_link[1];
	
	if (nd->nd_link[1] != 0)
	    nd->nd_link[1]->nd_link[0] = nd->nd_link[0];
	else
	    nd_hold[0] = nd->nd_link[0];
	
	hs->hs_nd_current--;
	nd_free(nd);
    }		
}

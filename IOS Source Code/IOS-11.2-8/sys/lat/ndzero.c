/* $Id: ndzero.c,v 3.2 1995/11/17 17:34:49 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/lat/ndzero.c,v $
 *------------------------------------------------------------------
 * $Log: ndzero.c,v $
 * Revision 3.2  1995/11/17  17:34:49  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:27:45  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:27:13  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*-----------------------------------------------------------------------
    nd_zero		(e) zero node counters
    
    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO
    
    This material is confidential information of Meridian Technology
    Corporation. Furnished under license. All rights reserved. 
 -----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------
    This function zeros the node counters. If no node specified, the
    function zeros all node counters.

    CALL BY:    void nd_zero(nd)
    		nd_ptr	nd;
 
    ON EXIT:    Node counter(s) set to zero.
 -----------------------------------------------------------------------*/
#include "mtcsys.h"
#include "laths.h"
#include "latnd.h"
#include "latfnc.h"

extern	hs_ptr	hs;			/* Local node definitions	*/
typedef byte	ND_VIEW(nd_byte);	/* Pointer to network words	*/

void nd_zero(nd)

    nd_ptr	nd;			/* Pointer to remote node	*/
{
#if LE_ND_STATS
    nd_byte	ptr;			/* Pointer to counters		*/
    int		i;			/* Do index			*/

/*-----------------------------------------------------------------------
    If no node specified, zero all counters.
 -----------------------------------------------------------------------*/
    if (nd == 0)
    {
        for (i = 0; i < ND_HASHES; i++)
        {
	    for (nd = hs->hs_nd_hash[i][0]; nd != 0; nd = nd->nd_link[0])
	        nd_zero(nd);
	}
    }
    
/*-----------------------------------------------------------------------
    Loop through the node counter by word and zero.
 -----------------------------------------------------------------------*/
    else
    {
        for (ptr = (nd_byte)&nd->nd_stats, i = 0;  i < sizeof(nd->nd_stats); i++)
            *ptr++ = 0;
    }
#endif
}

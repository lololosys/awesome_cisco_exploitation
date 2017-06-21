/* $Id: lsperm.c,v 3.2 1995/11/17 17:34:28 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/lat/lsperm.c,v $
 *------------------------------------------------------------------
 * $Log: lsperm.c,v $
 * Revision 3.2  1995/11/17  17:34:28  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:27:22  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:26:32  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*-----------------------------------------------------------------------
    ls_permanent	(e) create permanent learned service structure

    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO

    This material is confidential information of Meridian Technology
    Corporation.  Furnished under license.  All rights reserved.
 -----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------
    This function creates a permanent learned service structure. If a
    node and Ethernet address are specified, the service can be used
    immediately for connections. Otherwise, the entry is placed in
    the hold list, thus reserving space for the service.

    CALL BY:	ls_ptr ls_permanent(name,node,dest)
    		byte	*name;
		byte	*node;
		word	dest[3];

    ON EXIT:	The permanent learned service structure is created
    		and either placed in the hold or active list. If
		any error occurs, the function returns zero and sets
		lat_error as follows:

	LS_ERROR | MAX_SPACE	Exceeded limit of ls structures
	LS_ERROR | NO_MEM	Failed to allocate ls structure
	ND_ERROR | MAX_SPACE	Exceeded limit of nd structures
	ND_ERROR | NO_MEM	Failed to allocate nd structure
 -----------------------------------------------------------------------*/
#include "mtcsys.h"
#include "laths.h"
#include "latls.h"
#include "latnd.h"
#include "latfnc.h"
#include "sysfnc.h"

extern	hs_ptr	hs;			/* Local node definitions	*/

ls_ptr ls_permanent(name,node,dest,idb)
    byte	*name;			/* Service name			*/
    byte	*node;			/* Remote node name		*/
    word	dest[3];		/* Remote node ethernet address	*/
    idbtype	*idb;			/* Interface to use		*/
 {
    ls_ptr	ls = NULL;		/* Pointer to new LS structure	*/
    nd_ptr	nd;			/* Pointer to new ND structure	*/
    nd_ptr	nx;			/* Pointer to an ND structure	*/
    int		i;			/* Do index			*/

/*-----------------------------------------------------------------------
    Create a node and learned service structure. If we cannot allocate
    the structures, delete services and structures until we can; but...
    don't touch the node we just created or found!
 -----------------------------------------------------------------------*/
    while ((nd = nd_create(node,dest,idb)) == 0 || (ls = ls_create(name,nd)) == 0)
    {
	for (i = 0; i < ND_HASHES; i++)
	{
	    for (nx = hs->hs_nd_hash[i][0]; nx != 0; nx = nx->nd_link[0])
	    {
		if (nx->nd_usage == -1 && nx != nd)
		{
		    while (nx->nd_ls[0] != 0)
			ls_delete(nx->nd_ls[0]);
		    nd_delete(nx);
		    break;
		}
	    }
	    if (nx != 0)
		break;
	}

	if (i == ND_HASHES)
	    break;
    }

/*-----------------------------------------------------------------------
    Initialize the permanent service.
------------------------------------------------------------------------*/
    if (ls != 0)
    {
	ls->ls_flags = LS_PERM;

/*-----------------------------------------------------------------------
    If the node name is zero then the service goes onto hold list.
    To make sure we see next service annoucment message with service,
    mark all remote nodes as initializing.
------------------------------------------------------------------------*/
	if (*node == 0)
	{
	    ls_delete(ls);
	    nd_delete(nd);

	    for (i = 0; i < ND_HASHES; i++)
	    {
	        for (nd = hs->hs_nd_hash[i][0]; nd != 0; nd = nd->nd_link[0])
	             nd->nd_flags |= ND_INIT;
	    }
	}

/*-----------------------------------------------------------------------
    We can use service immediately. Set the remote ACL groups to our
    IDL groups and mark remote node for initialization.
------------------------------------------------------------------------*/
	else
	{
	    nd->nd_flags |= ND_INIT;
	    nd_str_move_sl(&nd->nd_acl[0],&hs->hs_idl_group[0]);
	}
    }

/*-----------------------------------------------------------------------
    If no learned service, delete any remote node we created.
    First, make sure we created it; i.e., check that no other services
    are linked in the node learned service list.
------------------------------------------------------------------------*/
    else if (nd != 0 && nd->nd_ls[0] == 0)
	nd_delete(nd);

    return(ls);
}

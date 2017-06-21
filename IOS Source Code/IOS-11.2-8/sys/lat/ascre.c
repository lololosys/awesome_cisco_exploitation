/*-----------------------------------------------------------------------
    as_create		(e) create advertised service

    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO

    This material is confidential information of Meridian Technology
    Corporation.  Furnished under license.  All rights reserved.
 -----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------
    This function creates the advertised service structure, initializing
    the name, identification, environment ID, flags, and rating.  Note,
    this function initializes the service to no associated objects or
    CME ports.  The as_set_object function must be called for each object
    from which the service is available (if object support is enabled).
    The as_set_port function must be called for each CME port from which
    the service is available (if CME support is enabled).

    CALL BY:	as_ptr as_create(name,ident,aid,flags,rating)
    		byte	 *name;
    		byte	 *ident;
    		aid_type aid;
    		int	 flags;
    		int	 rating;

    ON EXIT:	Returns the address of the created advertised service
		structure address. If no service can be created, the
		function returns zero and lat_error  will be set to one
		of the following errors:

	AS_ERROR | INV_ARG	Rating is not 0-255 or dynamic
	AS_ERROR | MAX_LIMIT	New service exceeds as_maximum services
	AS_ERROR | MAX_SPACE	New service exceeds maximum message size
	AS_ERROR | NO_MEM	Memory for as structure could not be allocated
	AS_ERROR | NU_NAME	Service already exists with name
 -----------------------------------------------------------------------*/
#include "mtcsys.h"
#include "laths.h"
#include "latas.h"
#include "laterr.h"
#include "latmsg.h"
#include "latfnc.h"
#include "sysfnc.h"

extern	hs_ptr	hs;			/* Local node definitions	*/
extern	int	lat_error;		/* LAT error code		*/
#if LE_OBJECTS
extern	int	ob_highest;		/* Largest number of objects	*/
#endif
#if CME_SLAT
extern	int	cm_port_maximum;	/* Maximum number of CME ports	*/
#endif

as_ptr as_create(name,ident,aid,flags,rating)

    byte	*name;			/* Service name			*/
    byte	*ident;			/* Identification string	*/
    aid_type	aid;			/* Service identification	*/
    int		flags;			/* Initial state flags		*/
    int		rating;			/* Static rating for service	*/
 {
    as_ptr	as;			/* Pointer to new AS structure	*/
    as_ptr	ps;			/* Pointer to previous AS	*/
#if (LE_OBJECTS || CME_SLAT)
    int		i;			/* Loop index			*/
#endif

/*----------------------------------------------------------------------
    Initialize as to zero; therefore, if any tests fail a value of zero
    is returned and no advertised service structure is created.
 -----------------------------------------------------------------------*/
    as = 0;

/*-----------------------------------------------------------------------
    If the name is not unique then we drop through with an error.
------------------------------------------------------------------------*/
    if (as_show(name) != 0)
    	lat_error = AS_ERROR | NU_NAME;

/*-----------------------------------------------------------------------
    If the rating is not between 0 and AS_RATING or it is not dynamic,
    then we drop through with an error.
------------------------------------------------------------------------*/
    else if ( (rating < 0 || rating > AS_RATING) && !(flags & (AS_DYNAMIC | AS_CLIENT)))
    	lat_error = AS_ERROR | INV_ARG;

/*-----------------------------------------------------------------------
    If the service is offered, check if enough room for message.
------------------------------------------------------------------------*/
    else if ((flags & AS_OFFERED) && (hs->hs_as_space < (*name + *ident + 3)))
        lat_error = AS_ERROR | MAX_SPACE;

/*-----------------------------------------------------------------------
    Check to see if new service exceeds maximum services.
------------------------------------------------------------------------*/
    else if (hs->hs_as_maximum > 0 && hs->hs_as_current >= hs->hs_as_maximum)
        lat_error = AS_ERROR | MAX_LIMIT;

/*----------------------------------------------------------------------
    If as_alloc returns with no errors, then we fill in the new structure.
------------------------------------------------------------------------*/
    else if ((as = as_alloc()) == 0)
        lat_error = AS_ERROR | NO_MEM;

    else
    {
        if (++hs->hs_as_current > hs->hs_as_highest)
	    hs->hs_as_highest++;

	as->as_aid    = aid;
    	as->as_flags  = (byte)flags;
    	as->as_rating = (byte)rating;
    	as_str_move_sl(&as->as_service[0],name);
    	as_str_move_sl(&as->as_ident[0],ident);
	as->as_password[0] = 0;

#if LE_OBJECTS
	as->as_cur_objects = 0;
	as->as_last_object = ob_highest - 1;
	for (i = 0; i < (AS_OBJECTS-1)/8+1; i++)
	    as->as_ob_map[i] = 0;
#endif
#if CME_SLAT
	as->as_cur_ports = 0;
	as->as_last_port = cm_port_maximum - 1;
	for (i = 0; i < (cm_port_maximum-1)/8+1; i++)
	    as->as_port_map[i] = 0;
#endif

/*-----------------------------------------------------------------------
    Update the changed service mask in the host structure (hs), and
    decrement the space remaining to reflect the addition of this new
    service.
 -----------------------------------------------------------------------*/
	if (as->as_flags & AS_OFFERED)
	{
	    hs->hs_as_next_changes |= (SVC_SERVICES | SVC_DESCRIPTOR);
	    hs->hs_as_space -= (as->as_service[0] + as->as_ident[0] + 3);
	}

/*-----------------------------------------------------------------------
    Scan the current advertised services for insertion point. We keep the
    list in alphabetical order.
------------------------------------------------------------------------*/
        for (ps = hs->hs_as_head[0]; ps != 0; ps = ps->as_link[0])
        {
           if ((as_str_diff_sl(&ps->as_service[0],name)) > 0)
    	        break;
        }

/*-----------------------------------------------------------------------
    Now insert as before ps. Correctly handle refernces to header.
------------------------------------------------------------------------*/
        as->as_link[0] = ps;

	if (ps != 0)
	{
	    as->as_link[1] = ps->as_link[1];
	    ps->as_link[1] = as;
	}
	else
	{
	    as->as_link[1]    = hs->hs_as_head[1];
	    hs->hs_as_head[1] = as;
	}

	if (as->as_link[1] != 0)
	    as->as_link[1]->as_link[0] = as;
	else
	    hs->hs_as_head[0] = as;
    }
    return(as);
}

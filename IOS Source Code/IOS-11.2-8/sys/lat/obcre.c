/*-----------------------------------------------------------------------
    ob_create		(e) create object structure

    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO

    This material is confidential information of Meridian Technology
    Corporation.  Furnished under license.  All rights reserved.
 -----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------
    This function creates an object structure, initializing the name,
    oid, flags, and maximum sessions to supplied parameters. The object
    IDL groups are set initially to no groups enabled. All configuration
    fields are set to zero.

    CALL BY:	ob_ptr ob_create(name,oid,flags,max_lcl,max_rmt)
    		byte	 *name;
		oid_type oid;
    		int	 flags;
    		int	 max_lcl;
    		int	 max_rmt;

    ON EXIT:	The object structure is created, and ob contains the
    	        structure's address.  If the object is not created, the
    		function returns zero and lat_error is set as follows:

	OB_ERROR | INV_ARG	Invalid local/remote/dynamic combination
	OB_ERROR | MAX_LIMIT	New object exceeds maximum limit
	OB_ERROR | NO_MEM	Failed to allocate object structure
	OB_ERROR | NU_IDENT	Object identifier is already used
	OB_ERROR | NU_NAME	Object name is already used
 -----------------------------------------------------------------------*/
#include "mtcsys.h"
#include "latob.h"
#include "laterr.h"
#include "laterr.h"
#include "latfnc.h"
#include "sysfnc.h"

extern	ob_ptr	ob_map[];		/* Object mapping table		*/
extern	ob_ptr	ob_head[2];		/* OB structure list header	*/
extern	int	ob_current;		/* Current number objects	*/
extern	int	ob_maximum;		/* Maximum number objects	*/
extern	int	ob_highest;		/* Highest number objects	*/
extern	int	lat_error;		/* LAT error code		*/

typedef	byte	OB_VIEW(ob_byte);	/* Pointer to byte fields	*/

ob_ptr ob_create(name,oid,flags,max_lcl,max_rmt)

    byte	*name;			/* The service name		*/
    oid_type	oid;			/* The object's identifier 	*/
    int		flags;			/* Initial state flags		*/
    int		max_lcl;		/* Maximum allowed local  ses.	*/
    int		max_rmt;		/* Maximum allowed remote ses.	*/

 {
    ob_ptr	ob;			/* Pointer to new OB structure	*/
    ob_ptr	ps;			/* Pointer to previous OB	*/
    int		i;			/* Do index			*/
    ob_byte	ptr;			/* Pointer to OB Data-b struct	*/

/*-----------------------------------------------------------------------
    Initialize ob to zero; therefore, if all tests fail a value of zero is
    returned and no object structure is created.
 -----------------------------------------------------------------------*/
    ob = 0;

/*-----------------------------------------------------------------------
    If the name is not unique, then we drop through with an error.
------------------------------------------------------------------------*/
    if (ob_show(name) != 0)
        lat_error = OB_ERROR | NU_NAME;

/*-----------------------------------------------------------------------
    Make sure clone is not set.
------------------------------------------------------------------------*/
    else if (flags & OB_CLONED)
	lat_error = OB_ERROR | INV_ARG;

/*-----------------------------------------------------------------------
    Make sure we have a valid combination of local/remote/dynamic.
------------------------------------------------------------------------*/
    else if ((flags & OB_LOCAL)   && ((flags & (OB_REMOTE | OB_DYNAMIC)) || max_rmt != 0))
	lat_error = OB_ERROR | INV_ARG;

    else if ((flags & OB_REMOTE)  && ((flags & (OB_LOCAL  | OB_DYNAMIC)) || max_lcl != 0))
	lat_error = OB_ERROR | INV_ARG;

/*-----------------------------------------------------------------------
    If the oid is not unique, then we drop through with an error.
------------------------------------------------------------------------*/
    else if (ob_find(oid) != 0)
        lat_error = OB_ERROR | NU_IDENT;

/*-----------------------------------------------------------------------
    If new object exceeds limit or structure cannot be allocated, then
    we drop through with an error.
------------------------------------------------------------------------*/
    else if (ob_maximum > 0 && ob_current >= ob_maximum)
        lat_error = OB_ERROR | MAX_LIMIT;

    else if ((ob = ob_alloc()) == 0)
        lat_error = OB_ERROR | NO_MEM;

/*-----------------------------------------------------------------------
    Initialize the object structure.
------------------------------------------------------------------------*/
    else
    {
	if (++ob_current > ob_highest)
	    ob_highest++;

   	ob->ob_oid     = oid;
  	ob->ob_flags   = flags & ~OB_DEL_PEND;

    	ob->ob_lcl_use = 0;
     	ob->ob_lcl_max = max_lcl;
    	ob->ob_rmt_use = 0;
     	ob->ob_rmt_max = max_rmt;

	for (ptr = (ob_byte)&ob->ob_lcl_data, i = 0; i < sizeof(data_b); i++)
	    *ptr++ = 0;
	for (ptr = (ob_byte)&ob->ob_tmp_data, i = 0; i < sizeof(data_b); i++)
	    *ptr++ = 0;

	ob->ob_idl[0] = 0;
    	ob_str_move_sl(&ob->ob_name[0],name);

/*------------------------------------------------------------------------
    Find the correct place to put the object structure into the object
    structure linked list. We link objects alphabetically by name.
-------------------------------------------------------------------------*/
        for (ps = ob_head[0]; ps != 0; ps = ps->ob_link[0])
        {
           if ((ob_str_diff_sl(&ps->ob_name[0],name)) > 0)
    	        break;
        }

/*------------------------------------------------------------------------
    Link ob before ps. Correctly handle linking to front/end of list.
-------------------------------------------------------------------------*/
        ob->ob_link[0] = ps;

	if (ps != 0)
	{
	    ob->ob_link[1] = ps->ob_link[1];
	    ps->ob_link[1] = ob;
	}
	else
	{
	    ob->ob_link[1] = ob_head[1];
	    ob_head[1]     = ob;
	}

	if (ob->ob_link[1] != 0)
	    ob->ob_link[1]->ob_link[0] = ob;
	else
	    ob_head[0] = ob;

/*-----------------------------------------------------------------------
    Add object pointer to the object mapping table.
 ------------------------------------------------------------------------*/
        for (i = 0; i < AS_OBJECTS; i++)
	{
	    if (ob_map[i] == 0)
	    {
		ob_map[i] = ob;
		break;
	    }
	}
    }
    return (ob);
}

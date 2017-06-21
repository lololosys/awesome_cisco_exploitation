/*-----------------------------------------------------------------------
    ob_rename		(e) rename object structure

    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO

    This material is confidential information of Meridian Technology
    Corporation.  Furnished under license.  All rights reserved.
 -----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------
    This function renames an object structure. It is primarily used to
    name cloned objects.

    CALL BY:	int ob_rename(ob,name,oid)
    		ob_ptr 	 ob;
    		byte	 *name;
		oid_type oid;

    ON EXIT:	Returns SUCCESS if object renamed, FAILURE otherwise and
    		lat_error set as follows:

	OB_ERROR | NU_IDENT	Object identifier is already used
	OB_ERROR | NU_NAME	Object name is already used
 -----------------------------------------------------------------------*/
#include "mtcsys.h"
#include "latob.h"
#include "laterr.h"
#include "latfnc.h"
#include "sysfnc.h"

extern	ob_ptr	ob_head[2];		/* OB structure list header	*/
extern	int	lat_error;		/* LAT error code		*/

int ob_rename(ob,name,oid)

    ob_ptr	ob;			/* Pointer to new OB structure	*/
    byte	*name;			/* The service name		*/
    oid_type	oid;			/* The object's identifier 	*/
 {
    ob_ptr	ps;			/* Pointer to previous OB	*/
    int		sts;			/* Status return		*/

/*-----------------------------------------------------------------------
    Preset failure.
------------------------------------------------------------------------*/
    sts = FAILURE;

/*-----------------------------------------------------------------------
    If the name is not unique, then we drop through with an error.
------------------------------------------------------------------------*/
    if (!ob_str_equal_sl(&ob->ob_name[0],name) && ob_show(name) != 0)
        lat_error = OB_ERROR | NU_NAME;

/*-----------------------------------------------------------------------
    If the oid is not unique, then we drop through with an error.
------------------------------------------------------------------------*/
    else if (ob->ob_oid != oid && ob_find(oid) != 0)
        lat_error = OB_ERROR | NU_IDENT;

/*------------------------------------------------------------------------
    Unlink the structure and place into correct position.
-------------------------------------------------------------------------*/
    else
    {
	sts = SUCCESS;

/*------------------------------------------------------------------------
    Set the new name and oid.
-------------------------------------------------------------------------*/
	ob->ob_oid = oid;
	ob_str_move_sl(&ob->ob_name[0],name);

/*------------------------------------------------------------------------
    Unlink the structure.
-------------------------------------------------------------------------*/
	if (ob->ob_link[0] != 0)
	    ob->ob_link[0]->ob_link[1] = ob->ob_link[1];
	else
	    ob_head[1] = ob->ob_link[1];

	if (ob->ob_link[1] != 0)
	    ob->ob_link[1]->ob_link[0] = ob->ob_link[0];
	else
	    ob_head[0] = ob->ob_link[0];

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
    }
    return(sts);
}

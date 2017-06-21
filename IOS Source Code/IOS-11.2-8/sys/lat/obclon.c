/*-----------------------------------------------------------------------
    ob_clone		(i) clone object

    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO

    This material is confidential information of Meridian Technology
    Corporation.  Furnished under license.  All rights reserved.
 -----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------
    This function clones an object structure, if the supplied object
    is a template. Otherwise, it does nothing.

    CALL BY:	ob_ptr ob_clone(ob)
		ob_ptr   ob;

    ON EXIT:	If a template object, a cloned object is created with
	    	a null OID and name and the address of the new object
	    	is returned. Otherwise, the function returns the address
	    	of the supplied object.

	OB_ERROR | MAX_LIMIT	New object exceeds maximum limit
	OB_ERROR | NO_MEM	Failed to allocate object structure
 -----------------------------------------------------------------------*/
#include "mtcsys.h"
#include "latob.h"
#include "laterr.h"
#include "latfnc.h"
#include "sysfnc.h"

extern	ob_ptr	ob_head[2];		/* OB structure list header	*/
extern	int	ob_current;		/* Current number objects	*/
extern	int	ob_maximum;		/* Maximum number objects	*/
extern	int	ob_highest;		/* Highest number objects	*/
extern	int	lat_error;		/* LAT error code		*/

typedef	byte	OF_VIEW(ob_byte);	/* Pointer to byte fields	*/

ob_ptr ob_clone(ob)

    ob_ptr	ob;			/* Pointer to new OB structure	*/
 {
    ob_ptr	ox;			/* Pointer to origianl OB 	*/
    ob_byte	ptr;			/* Pointer to OB data		*/
    ob_byte	org;			/* Pointer to OB data		*/
    int		i;			/* Do index			*/

/*-----------------------------------------------------------------------
    Initialize ob to zero; therefore, if all tests fail a value of zero is
    returned and no object structure is created.
 -----------------------------------------------------------------------*/
    if (ob->ob_flags & OB_TEMPLATE)
    {
	ox = ob;

/*-----------------------------------------------------------------------
    If new object exceeds limit or structure cannot be allocated, then
    we drop through with an error.
------------------------------------------------------------------------*/
	if (ob_maximum > 0 && ob_current >= ob_maximum)
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

	    ob->ob_oid     = ox->ob_oid;
	    ob->ob_flags   = (ox->ob_flags & ~OB_TEMPLATE) | OB_CLONED;
	    ob->ob_name[0] = 0;
	
	    ob->ob_lcl_use = 0;
	    ob->ob_lcl_max = ox->ob_lcl_max;
	    ob->ob_rmt_use = 0;
	    ob->ob_rmt_max = ox->ob_rmt_max;

	    ptr = (ob_byte)&ob->ob_lcl_data;
	    for (i = 0; i < sizeof(data_b); i++)
		*ptr++ = 0;
		
	    org = (ob_byte)&ox->ob_lcl_data;
	    ptr = (ob_byte)&ob->ob_lcl_data;
	    for (i = 0; i < sizeof(data_b); i++)
		*ptr++ = *org++;
	
	    org = (ob_byte)&ox->ob_idl[0];
	    ptr = (ob_byte)&ob->ob_idl[0];
	    for (i = 0; i < sizeof(ob->ob_idl); i++)
		*ptr++ = *org++;

/*------------------------------------------------------------------------
    Link new object at end of object list.
-------------------------------------------------------------------------*/
	    ob->ob_link[0] = 0;
	    ob->ob_link[1] = ob_head[1];
	    ob_head[1] = ob;
	    if (ob->ob_link[1] != 0)
		ob->ob_link[1]->ob_link[0] = ob;
	    else
		ob_head[0] = ob;
	}
    }
    return (ob);
}

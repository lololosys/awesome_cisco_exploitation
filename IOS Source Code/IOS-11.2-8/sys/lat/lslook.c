/*-----------------------------------------------------------------------
    ls_look		(i) search the ls hold list for service

    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO

    This material is confidential information of Meridian Technology
    Corporation. Furnished under license. All rights reserved.
 -----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------
    This function searches the ls_hold list by service name and return
    a pointer to structure which matches name.

    CALL BY:	ls_ptr ls_look(name);
		byte *name;

    ON EXIT:	Return a pointer to the matching learned service in hold
    	        list.  If no match, return zero.
 -----------------------------------------------------------------------*/
#include "mtcsys.h"
#include "latls.h"
#include "latfnc.h"
#include "sysfnc.h"

extern ls_ptr	ls_hold[2];		/* LS hold header		*/

ls_ptr ls_look(name)

    byte	*name;			/* New learned service name	*/
{
    ls_ptr	ls;			/* Pointer to LS structure	*/

/*-----------------------------------------------------------------------
    Search the ls hold structure.  If a match is found, return ls.
 -----------------------------------------------------------------------*/
    for (ls = ls_hold[0]; ls != 0; ls = ls->ls_host[0])
    {
	if (ls_str_equal_sl(&ls->ls_service[0],name))
	    break;
    }

    return(ls);
}

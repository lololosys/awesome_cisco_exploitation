/*-----------------------------------------------------------------------
    ox_session		(x) setup object slot data

    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO

    This material is confidential information of Meridian Technology
    Corporation. Furnished under license. All rights reserved.
 -----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------
    This function is called by the LAT engine (if not configured for
    object support) when object data is needed for outgoing slots.
    The function fills in the supplied object structure with the
    desired data.

    CALL BY:	void ox_session(sb,ob,reason)
    		sb_ptr	sb;
		ob_ptr	ob;
    		int	reason;

    ON EXIT:	Object structure data fields set.
 -----------------------------------------------------------------------*/
#include "mtcsys.h"
#include "laths.h"
#include "latob.h"
#include "latsb.h"
#include "latfnc.h"
#include "sysfnc.h"

extern	hs_ptr	hs;			/* Local node definitions	*/

void ox_session(sb,ob,reason)

    sb_ptr	sb;			/* Pointer to session block	*/
    ob_ptr	ob;			/* Pointer to object block	*/
    int		reason;			/* Session delete reason	*/
{

/*-----------------------------------------------------------------------
    If generating a start slot, fill in object name (ob_name), type
    (ob_lcl_data.db_type), and idl group flags (ob_idl).
 -----------------------------------------------------------------------*/
    if (reason == OX_START)
    {
    }

/*-----------------------------------------------------------------------
    If generating a DATA-B REPORT slot, fill in ob->ob_lcl_data.xxx 
    fields.
 -----------------------------------------------------------------------*/
    if (reason == OX_CONFIG_REP)
    {
    }
}

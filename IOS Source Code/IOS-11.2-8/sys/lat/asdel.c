/*-----------------------------------------------------------------------
    as_delete		(e) delete advertised service

    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO

    This material is confidential information of Meridian Technology
    Corporation.  Furnished under license.  All rights reserved.
 -----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------
    This function deletes the specified advertised service.

    CALL BY:	void as_delete(as)
    		as_ptr     as;

    ON EXIT:	The advertised service is deleted. A queue scan is
    		requested.
 -----------------------------------------------------------------------*/
#include "mtcsys.h"
#include "laths.h"
#include "latas.h"
#include "latmsg.h"
#include "latfnc.h"
#include "sysfnc.h"

extern	hs_ptr	hs;			/* Local node definitions	*/

void as_delete(as)

    as_ptr	as;                     /* Pointer to AS structure	*/
{
/*-----------------------------------------------------------------------
    If service was being offered to network, update the changed service
    mask in the host structure (hs) and the available size to reflect
    the deleted service.
 ------------------------------------------------------------------------*/
    if (as->as_flags & AS_OFFERED)
    {
	hs->hs_as_next_changes |= SVC_SERVICES;
	hs->hs_as_space += (as->as_service[0] + as->as_ident[0] + 3);
    }

/*-----------------------------------------------------------------------
    Remove the entry from list and deallocate.
 -----------------------------------------------------------------------*/
    if (as->as_link[0] != 0)
	as->as_link[0]->as_link[1] = as->as_link[1];
    else
	hs->hs_as_head[1] = as->as_link[1];

    if (as->as_link[1] != 0)
	as->as_link[1]->as_link[0] = as->as_link[0];
    else
	hs->hs_as_head[0] = as->as_link[0];

    hs->hs_as_current--;
    as_free(as);

#if (LE_OBJECTS || CME_SLAT) && LE_OBJECT_HIC
/*-----------------------------------------------------------------------
    Request a queue scan as this change may effect current entries.
 -----------------------------------------------------------------------*/
    {
	extern int qr_changes;
	qr_changes = SUCCESS;
    }
#endif
}

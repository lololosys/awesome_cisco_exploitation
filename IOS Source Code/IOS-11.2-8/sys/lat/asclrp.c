/* $Id: asclrp.c,v 3.2 1995/11/17 17:31:48 hampton Exp $
 * $Source: /vws/aam/cherf.111/ios/sys/lat/asclrp.c,v $
 *------------------------------------------------------------------
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: asclrp.c,v $
 * Revision 3.2  1995/11/17  17:31:48  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:24:39  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:21:27  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/*-----------------------------------------------------------------------
    as_clr_port	(e) clear CME port from advertised service

    Copyright 1992 Meridian Technology Corporation, St. Louis, MO

    This material is confidential information of Meridian Technology
    Corporation.  Furnished under license.  All rights reserved.
 -----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------
    This function removes a CME port identifier (id) association from
    the specified advertised service.

    It is the responsibility of the implementor to assure that the passed
    CME port identifier is both unique and idempotent and that it be an
    integer value in the range 0 to (cm_port_maximum-1).

    This function may only be used by platforms which elect to use CME
    support (CME_SLAT = 1).

    CALL BY:    void as_clr_port(as,id)
    		as_ptr    as;
    		int	  id;

    ON EXIT:	The CME port specified by the id argument is removed
    		from the service bit-mapping array.  A queue scan
		is requested.  If a null AS pointer is passed, the
		specified CME port is removed from all advertised
		services.
 -----------------------------------------------------------------------*/
#include "mtcsys.h"
#include "latas.h"
#include "laths.h"
#include "latfnc.h"

extern	hs_ptr	hs;			/* Local node definitions 	*/
#if LE_OBJECT_HIC
extern 	int	qr_changes;
#endif
#if CME_SLAT
extern	int	cm_port_maximum;	/* Maximum number of CME ports	*/
#endif

void as_clr_port(
    as_ptr	as,			/* Pointer to AS structure	*/
    int		id)			/* Object identification	*/
{
#if CME_SLAT

    as_ptr	ax;			/* Pointer to AS structure	*/

    if (id < 0 || id >= cm_port_maximum)
    	;

/*-----------------------------------------------------------------------
    Clear the bit in the AS mapping array for all advertised services.
    Signal for a queue scan if HIC object processing is enabled.
 -----------------------------------------------------------------------*/
    else if (as == 0)
    {
#if LE_OBJECT_HIC
	qr_changes = SUCCESS;
#endif
	for (ax = hs->hs_as_head[0]; ax != 0; ax = ax->as_link[0])
	{
	    ax->as_cur_ports--;
	    ax->as_port_map[id/8] &= ~(1 << (id%8));
	}
    }

/*-----------------------------------------------------------------------
    Clear the bit in the specified advertised service mapping array and
    signal for a queue scan if HIC object processing is enabled.
 -----------------------------------------------------------------------*/
    else
    {
#if LE_OBJECT_HIC
	qr_changes = SUCCESS;
#endif
	as->as_cur_ports--;
	as->as_port_map[id/8] &= ~(1 << (id%8));
    }
#endif /* CME_SLAT */

    return;
}

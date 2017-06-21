/* $Id: assetp.c,v 3.2 1995/11/17 17:31:55 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/lat/assetp.c,v $
 *------------------------------------------------------------------
 * Associate CME port with advertised service
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: assetp.c,v $
 * Revision 3.2  1995/11/17  17:31:55  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:24:53  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:21:52  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*-----------------------------------------------------------------------
    as_set_port	(e) associate CME port with advertised service

    Copyright 1992 Meridian Technology Corporation, St. Louis, MO

    This material is confidential information of Meridian Technology
    Corporation.  Furnished under license.  All rights reserved.
 -----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------
    This function associates a CME port identifier (id) with an advertised
    service.

    It is the responsibility of the implementor to assure that the passed
    CME port identifier is both unique and idempotent and that it be an
    integer value in the range 0 to (cm_port_maximum-1).

    This function may only be used by platforms which elect to use CME
    support (CME_SLAT = 1).

    CALL BY:    int as_set_port(as,id)
    		as_ptr    as;
    		int	  id;

    ON EXIT:	Return SUCCESS if port mapping is set, FAILURE
		otherwise and lat_error set as follows:

	AS_ERROR | MAX_SPACE	New port exceeds maximum number
 -----------------------------------------------------------------------*/
#include "mtcsys.h"
#include "latas.h"
#include "laterr.h"
#include "latfnc.h"

extern	int	lat_error;		/* LAT error code		*/
#if CME_SLAT
extern	int	cm_port_maximum;	/* Maximum number of CME ports	*/
#endif

int as_set_port(
    as_ptr	as,			/* Pointer to AS structure	*/
    int		id)			/* Object identification	*/
{
    int		sts;			/* SUCCESS/FAILURE return value	*/

/*-----------------------------------------------------------------------
    Preset return status to FAILURE and test for valid ID value.
 -----------------------------------------------------------------------*/
    sts = FAILURE;

#if CME_SLAT

    if (id < 0 || id >= cm_port_maximum)
    	lat_error = AS_ERROR | MAX_SPACE;

/*-----------------------------------------------------------------------
    Set the bit in the AS mapping array and signal for a queue scan if
    HIC object processing is enabled.
 -----------------------------------------------------------------------*/
    else if (as != 0)
    {
#if LE_OBJECT_HIC
	extern int qr_changes;
	qr_changes = SUCCESS;
#endif
	as->as_cur_ports++;
	as->as_port_map[id/8] |= (1 << (id%8));
	sts = SUCCESS;
    }
#endif

    return(sts);
}

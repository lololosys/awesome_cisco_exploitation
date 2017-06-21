/* $Id: strmov.c,v 3.2 1995/11/17 17:35:27 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/lat/strmov.c,v $
 *------------------------------------------------------------------
 * $Log: strmov.c,v $
 * Revision 3.2  1995/11/17  17:35:27  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:28:47  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:29:10  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*-----------------------------------------------------------------------
    str_move		(i) copy counted text string

    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO

    This material is confidential information of Meridian Technology
    Corporation.  Furnished under license.  All rights reserved.
 -----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------
    This function is used by the LAT engine to copy a counted text from
    source to destination address. The first byte of the source is the
    length of the copy. The routine assumes the destination is large
    enough to hold the source string.

    CALL BY:	byte *str_move(dst,src)
    		byte	    *dst;
    		byte	    *src;

    ON EXIT:	The source string (including length) is copied into the
    		destination. The function returns a pointer to the next
    		byte in the destination.
 -----------------------------------------------------------------------*/
#include "mtcsys.h"
#include "latsys.h"
#include "latfnc.h"
#include "sysfnc.h"

byte *str_move(dst,src)

    byte	*dst;			/* Pointer to string descriptor	*/
    byte	*src;			/* Pointer to string descriptor	*/
{
    int		i;			/* String position counter	*/

/*-----------------------------------------------------------------------
    Store the source string in the destination string structure. We always
    copy the first byte as this is the string size.
 -----------------------------------------------------------------------*/
    for (i = *src; i >= 0; --i)
    	*dst++ = *src++;

    return(dst);
}

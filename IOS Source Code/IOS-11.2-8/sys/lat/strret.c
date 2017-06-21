/* $Id: strret.c,v 3.2 1995/11/17 17:35:29 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/lat/strret.c,v $
 *------------------------------------------------------------------
 * $Log: strret.c,v $
 * Revision 3.2  1995/11/17  17:35:29  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:28:50  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:29:17  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*-----------------------------------------------------------------------
    str_return		(e) copy counted text string to asciz string

    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO

    This material is confidential information of Meridian Technology
    Corporation.  Furnished under license.  All rights reserved.
 -----------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
    This function may be used by environments to copy LAT enigne counted
    text string to an ASCIZ string, truncating as needed. The max size
    includes the terminating zero.

    CALL BY:	int str_return(dst,src,max)
		byte	*dst;
		byte	*src;
		int	max;

    ON EXIT:	Copies string as ASCIZ string, truncating to length if needed.
    		Returns FAILURE if string truncates, SUCCESS otherwise.
 ---------------------------------------------------------------------------*/
#include "mtcsys.h"
#include "latsys.h"
#include "latfnc.h"
#include "sysfnc.h"

int str_return(dst,src,max)

    byte	*dst;			/* Pointer to byte string	*/
    byte 	*src;			/* Pointer to ASCIIZ string	*/
    int		max;			/* Max length of string		*/
{
    int		sts;			/* Status return		*/
    int		i;			/* Counter			*/

/*--------------------------------------------------------------------------
    Set the length of the copy to smaller of src size or max.
 ---------------------------------------------------------------------------*/
    if (*src++ >= --max)
        sts = FAILURE;
    else
    {
        sts = SUCCESS;
        max = *(src-1);
    }

/*--------------------------------------------------------------------------
    Loop through source until EOS or reach maximum.
 ---------------------------------------------------------------------------*/
    for (i = 0; i < max; i++)
	*dst++ = *src++;

    *dst = EOS;
    return(sts);
}

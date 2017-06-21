/* $Id: strchk.c,v 3.2 1995/11/17 17:35:19 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/lat/strchk.c,v $
 *------------------------------------------------------------------
 * $Log: strchk.c,v $
 * Revision 3.2  1995/11/17  17:35:19  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:28:32  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:28:41  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*-----------------------------------------------------------------------
    str_check		(i) check input frame string

    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO

    This material is confidential information of Meridian Technology
    Corporation.  Furnished under license.  All rights reserved.
 -----------------------------------------------------------------------*/
/*--------------------------------------------------------------------------
    This function is used by the LAT engine to check an input name does
    not exceed the maximum and contains legal characters.  Note that max
    is the maximum number of allowed characters plus the count byte.

    CALL BY:	int str_check(src,max)
		if_byte src;
		int	max;

    ON EXIT:	Return SUCCESS if the string is valid, FAILURE if string
    		is too long or has an illegal character.
 ---------------------------------------------------------------------------*/
#include "mtcsys.h"
#include "latsys.h"
#include "latfnc.h"
#include "sysfnc.h"

extern	byte	c_codes[256];		/* Character code table		*/

int str_check(src,max)

    if_byte 	src;			/* Ptr to counted byte string	*/
    int		max;			/* Max length of string		*/
{
    int		sts;			/* SUCCESS/FAILURE return value	*/

/*--------------------------------------------------------------------------
    If length exceeds maximum, return failure.
 ---------------------------------------------------------------------------*/
    if (*src >= max)
	sts = FAILURE;

/*--------------------------------------------------------------------------
    Scan the string for legal characters.
 ---------------------------------------------------------------------------*/
    else
    {
	sts = SUCCESS;

	for (max = *src++; max > 0; --max)
	{
	    if (!c_codes[*src++])
	        sts = FAILURE;
	}
    }
    return(sts);
}

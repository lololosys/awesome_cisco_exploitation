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
#include "latfnc.h"

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

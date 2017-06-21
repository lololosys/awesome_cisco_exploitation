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
#include "latfnc.h"

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

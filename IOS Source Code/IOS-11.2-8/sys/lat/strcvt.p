/*-----------------------------------------------------------------------
    str_convert		(e) convert (and check) asciz to counted text

    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO

    This material is confidential information of Meridian Technology
    Corporation.  Furnished under license.  All rights reserved.
 -----------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
    This function may be used by environments to convert an ASCIZ string
    into a counted byte string for use by the LAT engine. Note, the max
    argument includes the count byte.

    If the legal argument is a non-zero number, the function will check
    the src string does not exceed max length, contains only legal LAT
    characters and convert the string to upper case (only if LE_UPPER
    is enabled).

    If legal is zero, the function simply copies src to dst, truncating
    if necessary.

    CALL BY:	int str_convert(dst,src,max,legal)
		byte  *dst;
		byte  *src;
		int   max;
		int   legal;

    ON EXIT:	Return SUCCESS if the string is converted, FAILURE if string
    		is too long or has an illegal character.
 ---------------------------------------------------------------------------*/
#include "mtcsys.h"
#include "latfnc.h"

extern	byte	c_codes[256];		/* Character code table		*/

int str_convert(dst,src,max,legal)

    byte	*dst;			/* Pointer to byte string	*/
    byte 	*src;			/* Pointer to ASCIIZ string	*/
    int		max;			/* Max length of string		*/
    int		legal;			/* Uppercase conversion flag	*/
{
    byte	c;			/* Converted character		*/
    int		i;			/* Counter			*/

/*--------------------------------------------------------------------------
    Loop through source until EOS or reach maximum.
 ---------------------------------------------------------------------------*/
    for (i = 1; (c = *src++) != 0 && i < max; i++)
    {

/*--------------------------------------------------------------------------
    If legal enabled, check for legal characters and potential case convert.
 ---------------------------------------------------------------------------*/
	if (legal)
	{
	    if (!c_codes[c])
	        break;

#if LE_UPPER
	    if (c_codes[c] == 2)
	        c -= 32;
#endif
	}
	dst[i] = c;
    }
    --i;

/*--------------------------------------------------------------------------
    Set length to copied length and see if end of source was reached.
 ---------------------------------------------------------------------------*/
    *dst = (byte)(LOBYTE(i));
    return((c == 0) ? SUCCESS : FAILURE);
}

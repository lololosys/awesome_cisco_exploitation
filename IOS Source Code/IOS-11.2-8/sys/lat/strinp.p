/*-----------------------------------------------------------------------
    str_input		(i) copy, check (and convert) input frame string

    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO

    This material is confidential information of Meridian Technology
    Corporation.  Furnished under license.  All rights reserved.
 -----------------------------------------------------------------------*/
/*--------------------------------------------------------------------------
    This function is used by the LAT engine to copy a counted string from
    an input message frame to local storage. Note, max includes the count
    byte.

    If the legal argument is a non-zero number, the function will check
    the src string does not exceed max length, contains only legal LAT
    characters and convert the string to upper case (only if LE_UPPER
    is enabled).

    If legal is zero, the function simply copies src to dst, truncating
    if necessary.

    CALL BY:	int str_input(dst,src,max,legal)
		byte	*dst;
		if_byte src;
		int	max;
		int	legal;

    ON EXIT:	Return SUCCESS if the string is converted, FAILURE if string
    		is too long or has an illegal character (if legal is set, the
		output string length is set to zero).
 ---------------------------------------------------------------------------*/
#include "mtcsys.h"
#include "latfnc.h"

extern	byte	c_codes[256];		/* Character code table		*/

int str_input(dst,src,max,legal)

    byte	*dst;			/* Pointer to byte string	*/
    if_byte 	src;			/* Pointer to ASCIIZ string	*/
    int		max;			/* Max length of string		*/
    int		legal;			/* Uppercase conversion flag	*/
{
    byte	c;			/* Converted character		*/
    int		sts;			/* SUCCESS/FAILURE return value	*/
    int		i;			/* Counter			*/

/*--------------------------------------------------------------------------
    Set the length of the copy to smaller of src size or max-1.
 ---------------------------------------------------------------------------*/
    if (*src++ > --max)
       sts = FAILURE;
    else
    {
       max = *(src-1);
       sts = SUCCESS;
    }

    *dst++ = max;

/*--------------------------------------------------------------------------
    Loop through source until EOS or reach maximum.
 ---------------------------------------------------------------------------*/
    for (i = 0; i < max; i++)
    {
	c = *src++;

/*--------------------------------------------------------------------------
    If legal enable, check for legal characters and potential case convert.
 ---------------------------------------------------------------------------*/
	if (legal)
	{
	    if (!c_codes[c])
	        sts = FAILURE;

#if LE_UPPER
	    if (c_codes[c] == 2)
	        c -= 32;
#endif
	}
	*dst++ = c;
    }

    if (!sts && legal)
        *(dst-i-1) = 0;

    return(sts);
}

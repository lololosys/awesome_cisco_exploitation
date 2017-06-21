/*-----------------------------------------------------------------------
    str_equal		(i) compare counted text strings for equality

    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO

    This material is confidential information of Meridian Technology
    Corporation.  Furnished under license.  All rights reserved.
 -----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------
    This function is used by the LAT engine to compare two counted text
    strings for equality. The function returns SUCCESS if strings are
    equal and FAILURE otherwise.

    CALL BY:	int str_equal(dst,src)
    		byte	*dst;
    		byte	*src;

    ON EXIT:	Return SUCCESS if strings are equal and FAILURE otherwise.
 -----------------------------------------------------------------------*/
#include "mtcsys.h"
#include "latfnc.h"

extern	byte	c_codes[256];		/* Character code table		*/

int str_equal(dst,src)

    byte	*dst;			/* Pointer to string descriptor	*/
    byte	*src;			/* Pointer to string descriptor	*/
{
    int         sts;                    /* Return argument		*/
    int		len;			/* Length of strings		*/

/*-----------------------------------------------------------------------
    Check if strings have equal length.
 -----------------------------------------------------------------------*/
    if ((len = *dst++) != *src++)
	sts = FAILURE;

/*-----------------------------------------------------------------------
    Compare strings for equality.
 -----------------------------------------------------------------------*/
    else
    {
	for (sts = SUCCESS; len > 0; --len)
	{
#if LE_UPPER
	    if (*dst++ != *src++)
	    {
		sts = FAILURE;
		break;
	    }
#else
	    {
    		byte	c1,c2;

		c1 = *dst++;
		c2 = *src++;

	        if (codes[c1] == 2)
	            c1 -= 32;

	        if (codes[c2] == 2)
	            c2 -= 32;

	        if (c1 != c2)
	        {
		    sts = FAILURE;
		    break;
	        }
	    }
#endif
	}
    }
    return(sts);
}

/*-----------------------------------------------------------------------
    sx_put_char		(x) put chararacters to client
    
    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO
    
    This material is confidential information of Meridian Technology
    Corporation. Furnished under license. All rights reserved. 
 -----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------
    This function is called by the LAT engine whenever data arrives for
    the client. 

    CALL BY:	void sx_put_char(sb,ptr,len)
    		sb_ptr	sb;
		if_byte ptr;
    		int	len;
 
    ON EXIT:	Characters copied to read buffers.
 -----------------------------------------------------------------------*/
#include "mtcsys.h"
#include "latsb.h"
#include "latfnc.h"
#include "sysfnc.h"

void sx_put_char(sb,ptr,len)

    sb_ptr	sb;			/* Pointer to session block	*/
    if_byte	ptr;			/* Pointer to slot space	*/
    int		len;			/* Number of characters copied	*/
{

/*-----------------------------------------------------------------------
    Using the SID, Get the environment session data.
 -----------------------------------------------------------------------*/
    <get environment session data from sb->sb_sid>

/*-----------------------------------------------------------------------
    Copy len number of characters starting from ptr.  Note: function is
    never called with len of zero.
 -----------------------------------------------------------------------*/
    <buffer copy code>

/*-----------------------------------------------------------------------
    If and only if we have disposed of the output, extend a credit using
    the following statements.
 -----------------------------------------------------------------------*/
    sb->sb_lcl_credits++;
    sb->sb_flags |= SB_CREDITS;
}

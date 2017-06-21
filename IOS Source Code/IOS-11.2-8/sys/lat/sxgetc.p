/*-----------------------------------------------------------------------
    sx_get_char		(x) get chararacters from client

    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO

    This material is confidential information of Meridian Technology
    Corporation. Furnished under license. All rights reserved.
 -----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------
    This function is called by the LAT engine whenever the environment
    indicates it is generating a circuit message and the session has any
    extended credits and has inidicated it has available data.

    CALL BY:	int sx_get_char(sb,ptr,max)
    		sb_ptr	sb;
		of_byte ptr;
    		int	max;

    ON EXIT:	Returns number of characters copied from client to slot.
 -----------------------------------------------------------------------*/
#include "mtcsys.h"
#include "latsb.h"
#include "latfnc.h"
#include "sysfnc.h"

int sx_get_char(sb,ptr,max)

    sb_ptr	sb;			/* Pointer to session block	*/
    of_byte	ptr;			/* Pointer to slot space	*/
    int		max;			/* Maximum slot space available	*/
{
    int		len;			/* Number output characters	*/

/*-----------------------------------------------------------------------
    Using the SID, get the environment session data. Set the output
    length to zero.
 -----------------------------------------------------------------------*/
    <get environment session structure from sb->sb_sid>
    len = 0;

/*-----------------------------------------------------------------------
    Output characters to slot.  ptr is the pointer to the output position
    in circuit message, max is the maximum number of characters which may
    be copied.
 -----------------------------------------------------------------------*/
    <buffer copy code>

/*-----------------------------------------------------------------------
    If there are still more characters to be sent, reset the DATA_A flag
    which was cleared by the LAT engine on call to this function.
 -----------------------------------------------------------------------*/
    if (len == max)
	sb->sb_flags |= SB_DATA_A;

/*-----------------------------------------------------------------------
    Return actual number of characters put into slot.
 -----------------------------------------------------------------------*/
    return(len);
}

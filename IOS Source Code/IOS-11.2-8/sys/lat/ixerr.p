/*-----------------------------------------------------------------------
    ix_error		(x) report illegal lat frame

    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO

    This material is confidential information of Meridian Technology
    Corporation. Furnished under license. All rights reserved.
 -----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------
    This function is called by the LAT engine whenever an illegal LAT
    frame is detected. If possible, the frame should be stored for later
    display by the manegement interface.

    CALL BY:	void ix_error(frame,src,len,err)
    		if_byte	frame;
    		word	src[3];
    		int	len;
    		int	err;

    ON EXIT:	The engine discards the frame.
 -----------------------------------------------------------------------*/
#include "mtcsys.h"
#include "latmsg.h"
#include "latslt.h"
#include "latfnc.h"
#include "sysfnc.h"

void ix_error(frame,src,len,err)

    if_byte	frame;		/* Pointer to illegal frame		*/
    word	src[3];		/* Source address of frame		*/
    int		len;		/* Length of frame			*/
    int		err;		/* Error code (see latco.h).		*/
{
}

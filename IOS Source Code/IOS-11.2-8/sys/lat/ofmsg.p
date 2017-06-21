/*-----------------------------------------------------------------------
    of_message		(x) output lat ethernet message

    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO

    This material is confidential information of Meridian Technology
    Corporation. Furnished under license. All rights reserved.
 -----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------
    Output the supplied message to the ethernet. If a multicast message
    or addressed to ourselves, also pass message as input to ourselves.

    CALL BY:	void of_message(of)
    		of_ptr	of;

    EXIT WITH:	Message output to Ethernet and/or passed to ourselves.
 -----------------------------------------------------------------------*/
#include "mtcsys.h"
#include "laths.h"
#include "latfnc.h"
#include "sysfnc.h"

extern	hs_ptr	  hs;			/* Local node definitions	*/
extern	of_ptr	  of_local[2];		/* Local frame list header	*/
extern	word	  dst_addr[3];		/* Destination address		*/

void of_message(of)

    of_ptr	of;			/* Pointer to output frame	*/
{
/*-----------------------------------------------------------------------
    Count this transmission.
 -----------------------------------------------------------------------*/
    count_h(ch_total_xmit);

#if LE_SERVER_LS | LE_OBJECT_HIC
/*-----------------------------------------------------------------------
    If this is multicast message, pass to us first.
 -----------------------------------------------------------------------*/
    if (of->of_dst[0] == hs->hs_multicast[0])
    {
	dst_addr[0] = of->of_dst[0];
	dst_addr[1] = of->of_dst[1];
	dst_addr[2] = of->of_dst[2];
	if_message(of->of_data,&hs->hs_host_addr[0],&dst_addr[0]);
    }
#endif

/*-----------------------------------------------------------------------
    If buffer already being output, mark to retransmission. Otherwise
    mark message as being output.
 -----------------------------------------------------------------------*/
    if (of->of_size & OF_OUTPUT)
	of->of_size |= OF_REXMIT;

    else
    {
	of->of_size |= OF_OUTPUT;

/*-----------------------------------------------------------------------
    If message is sent to ourselves, link to local frame list.
 -----------------------------------------------------------------------*/
	if (of->of_dst[0] == hs->hs_host_addr[0] &&
	    of->of_dst[1] == hs->hs_host_addr[1] &&
	    of->of_dst[2] == hs->hs_host_addr[2])
	{
	    if (of_local[0] == 0)
		of_local[0] = of;
	    else
		of_local[1]->of_i_o = of;

	    of_local[1] = of;
	    of->of_i_o  = 0;
	}

/*-----------------------------------------------------------------------
    Otherwise, message goes on the wire. If output fails and message
    is marked for delete, free output frame.
 -----------------------------------------------------------------------*/
	else
	{
	    <system specific code>
	}
    }
}
